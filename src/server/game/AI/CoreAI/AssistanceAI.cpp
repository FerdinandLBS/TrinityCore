/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CombatAI.h"
#include "ConditionMgr.h"
#include "Creature.h"
#include "CreatureAIImpl.h"
#include "Log.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Vehicle.h"
#include "FollowMovementGenerator.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "AssistanceAI.h"
#include "TemporarySummon.h"
#include "Pet.h"

/////////////////
// AggressorAI
/////////////////

#pragma execution_character_set("utf-8")

const char* getCustomGreeting(int entry) {
    switch (entry) {
    case 45000:
        return "希望这次你准备了烈酒";
    case 45003:
        return "我到了，让我们速战速决吧";
    case 45005:
        return "为了巫妖王！";
    case 46000:
        return "为您效劳";
    case 46030:
        return "我到了，请多多指教";
    }
    return nullptr;
}

Unit* SelectMostHpPctFriedly(Unit* who, float range, bool isCombat) {
    Unit* unit = nullptr;
    Trinity::MostHPPctInRange u_check(who, range, isCombat);
    Trinity::UnitLastSearcher<Trinity::MostHPPctInRange> searcher(who, unit, u_check);
    Cell::VisitAllObjects(who, searcher, range);

    return unit;
}

Unit* SelectLeastHpPctFriendly(Unit* who, float range, bool isCombat) {
    Unit* unit = nullptr;
    Trinity::MostHPMissingPctInRange u_check(who, range, isCombat);
    Trinity::UnitLastSearcher<Trinity::MostHPMissingPctInRange> searcher(who, unit, u_check);
    Cell::VisitAllObjects(who, searcher, range);

    return unit;
}

void UnitAddCriticalRate(Unit* who, int rate) {
    if (!who)
        return;

    CastSpellExtraArgs args;
    args.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT0, rate - 1);
    args.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT1, rate - 1);
    who->CastSpell(who, 88000, args);
}

//////////////////
// Assistance AI
//////////////////
int32 AssistanceAI::Permissible(Creature const* creature)
{
    // have some hostile factions, it will be selected by IsHostileTo check at MoveInLineOfSight
    if (!creature->IsCivilian() && !creature->IsNeutralToAll())
        return PERMIT_BASE_REACTIVE;

    return PERMIT_BASE_NO;
}

Unit* getAttackerForHelper(Unit* unit, Unit* exclude)                 // If someone wants to help, who to give them
{
    Unit* victim;
    if (!unit->IsEngaged())
        return nullptr;

    CombatManager const& mgr = unit->GetCombatManager();
    // pick arbitrary targets; our pvp combat > owner's pvp combat > our pve combat > owner's pve combat
    Unit* owner = unit->GetCharmerOrOwner();
    if (mgr.HasPvECombat()) {
        for (auto const& pair : mgr.GetPvECombatRefs()) {
            victim = pair.second->GetOther(unit);
            if (victim != exclude) {
                return victim;
            }
        }
    }
    if (owner && (owner->GetCombatManager().HasPvECombat())) {
        for (auto const& pair : owner->GetCombatManager().GetPvECombatRefs()) {
            victim = pair.second->GetOther(unit);
            if (victim != exclude) {
                return victim;
            }
        }
    }
    if (mgr.HasPvPCombat()) {
        for (auto const& pair : mgr.GetPvPCombatRefs()) {
            victim = pair.second->GetOther(unit);
            if (victim != exclude) {
                return victim;
            }
        }
    }
    if (owner && (owner->GetCombatManager().HasPvPCombat()))
        return owner->GetCombatManager().GetPvPCombatRefs().begin()->second->GetOther(owner);
    return nullptr;
}

Unit* AssistanceAI::SelectNextTarget(bool allowAutoSelect, Unit* exclude)
{
    // Check pet attackers first so we don't drag a bunch of targets to the owner
    if (Unit* myAttacker = getAttackerForHelper(me, exclude))
        if (!myAttacker->HasBreakableByDamageCrowdControlAura())
            return myAttacker;

    // Not sure why we wouldn't have an owner but just in case...
    if (!me->GetCharmerOrOwner())
        return nullptr;

    // Check owner attackers
    if (Unit* ownerAttacker = me->GetCharmerOrOwner()->getAttackerForHelper())
        if (!ownerAttacker->HasBreakableByDamageCrowdControlAura())
            return ownerAttacker;

    // Check owner victim
    // 3.0.2 - Pets now start attacking their owners victim in defensive mode as soon as the hunter does
    if (Unit* ownerVictim = me->GetCharmerOrOwner()->GetVictim())
        return ownerVictim;

    // Default - no valid targets
    return nullptr;
}

bool AssistanceAI::castSpell(WorldObject* target, uint32 index) {
    _lastSpellResult = me->CastSpell(target, me->m_spells[index]);

    if (_lastSpellResult == SpellCastResult::SPELL_CAST_OK) {
        spellsTimer[index][SPELL_TIMER_CURRENT] = 0;
        gcd = 0;
        return true;
    }
    return false;
}

bool AssistanceAI::castSpell(const Position &dest, uint32 index) {
    _lastSpellResult = me->CastSpell(dest, me->m_spells[index]);

    if (_lastSpellResult == SpellCastResult::SPELL_CAST_OK) {
        spellsTimer[index][SPELL_TIMER_CURRENT] = 0;
        gcd = 0;
        return true;
    }
    return false;
}

bool AssistanceAI::isSpellReady(int32 index) {
    if (index < 0)
        return true;

    return (spellsTimer[index][SPELL_TIMER_CURRENT] >= spellsTimer[index][SPELL_TIMER_ORIGIN] && gcd >= ASSIST_GCD);
};

float AssistanceAI::GetManaPct() {
    return (float)me->GetPower(Powers::POWER_MANA) / (float)me->GetMaxPower(Powers::POWER_MANA);
}


Unit* AssistanceAI::GetVictim() {
    Unit* victim = nullptr;
    Unit* owner = me->GetOwner();

    if (!me->IsAlive())
    {
        EngagementOver();
        goto bail;
    }
    isTargetChanged = false;
    victim = me->GetVictim();
    if (nullptr == victim || victim->IsIgnoringCombat() || victim->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE)) {
        me->DisengageWithTarget(victim);
        victim = SelectNextTarget(false, victim);
        if (victim) {
            me->Attack(victim, !isCaster());
            isTargetChanged = true;
        }
    }
    if (owner) {
        if (owner->IsEngaged()) {
            Unit* ownerTarget = ObjectAccessor::GetUnit(*owner, owner->GetTarget());
            if (ownerTarget && ownerTarget->IsAlive() && ownerTarget->IsHostileTo(me)) {
                // If owner selected one enemy
                victim = ownerTarget;
                if (victim != me->GetVictim()) {
                    me->Attack(victim, !isCaster());
                    isTargetChanged = true;
                }
            }
        }
    }

bail:
    if (victim && !victim->IsAlive()) {
        victim = nullptr;
    }

    if (!victim) {
        me->SetTarget(ObjectGuid::Empty);
    }

    return victim;
}

bool AssistanceAI::AssistantsSpell(uint32 diff, Unit* victim) {
    uint32 id;

    if (me->GetEntry() < 45000) {
        if (me->GetEntry() < 40000)
            return false;
    }

    bool casted = false;

    if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        return true;

    uint32 i = -MAX_CREATURE_SPELL;
 redo:
    for (; i < MAX_CREATURE_SPELL && casted == false; i++) {
        const SpellInfo* si = nullptr;

        if (i < 0) {
            if (oneTimeSpells[i + MAX_CREATURE_SPELL])
                id = oneTimeSpells[i + MAX_CREATURE_SPELL];
            else
                continue;
        }
        else
            id = me->m_spells[i];

        si = sSpellMgr->GetSpellInfo(id);
        _lastSpellResult = SpellCastResult::SPELL_CAST_OK;
        if (si == nullptr)
            return false;

        if (si->IsPassive() || !isSpellReady(i)) {
            continue;
        }

        // Take care of specail AI
        switch (id) {
        case 85871: // Drain mana
            if (me->GetPower(POWER_MANA) < 175 && victim && victim->GetPower(POWER_MANA) > 56) {
                break;
            }
            continue;
        case 85912:
            if (me->GetHealthPct() > 60) {
                continue;
            }
            else {
                std::list<Creature*> list;
                me->GetAllMinionsByEntry(list, 46017);
                if (list.size() > 0) {
                    casted = castSpell(list.front(), i);
                    goto endloop;
                }
            }
            continue;
        case 85922: // Life Drain
            if (me->GetHealthPct() > 60) {
                continue;
            }
            break;
        case 85923: // Silence
            if (!victim || !victim->HasUnitState(UNIT_STATE_CASTING)) {
                continue;
            }
            break;
        case 85953: // Dark Ceremony
            if (GetManaPct() >= 0.6f) {
                continue;
            }
            {
                Unit* target = SelectMostHpPctFriedly(me, 40, true);
                if (target == nullptr)
                    continue;

                casted = castSpell(target, i);
            }
            goto endloop;
        case 85891: // Spirit of Revenge
        case 87278: // Wild Imp: Master Enhance
            casted = castSpell(me->GetOwner(), i);
            goto endloop;
        default:
            ;
        }

        // heal and buff and no target spell
        if (si->IsSelfCast() || si->Effects[0].Effect == SPELL_EFFECT_SUMMON) {
            if (si->Effects[0].IsEffect(SpellEffects::SPELL_EFFECT_APPLY_AURA) ||
                si->Effects[0].IsEffect(SPELL_EFFECT_APPLY_AREA_AURA_RAID)) {
                if (me->HasAura(id)) {
                    continue;
                }
            }

            // If we are tank. A self cast spell may be a final cast. We should keep it.
            if (_class == ASSISTANCE_CLASS::TANK && !si->Effects[0].IsEffect(SpellEffects::SPELL_EFFECT_SUMMON)) {
                if (!(si->GetRecoveryTime() > 30000 && me->GetHealthPct() <= 30)) {
                    continue;
                }
            }

            if (si->Effects[0].Effect == SPELL_EFFECT_TELEPORT_UNITS) {
                casted = castSpell(victim, i);
            }
            else {
                casted = castSpell(me, i);
            }
            break;
        }

        // Range Attack
        if ((si->RangeEntry->ID == 1 && si->Effects[0].RadiusEntry) ||
            si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_CONE_ENEMY_24) {
            if (victim == nullptr || si->Effects[0].RadiusEntry->RadiusMax/2 <= me->GetDistance(victim))
                continue;

            casted = castSpell(me, i);
            break;
        }

        if (si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
            si->Effects[0].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY) {
            if (victim == nullptr)
                continue;


            // If we are casting one debuff then check it
            if (si->Effects[0].IsEffect(SPELL_EFFECT_APPLY_AURA) && victim->HasAura(id) && !si->Effects[1].IsEffect(SPELL_EFFECT_SCHOOL_DAMAGE))
                continue;

            casted = castSpell(victim, i);
            break;
        }

        if ((si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
            si->Effects[1].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
            si->Effects[0].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
            && victim) {
            casted = castSpell(victim->GetPosition(), i);
            break;
        }

        /* We are healers */
        if (si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY ||
            si->Effects[1].TargetB.GetTarget() == TARGET_UNIT_DEST_AREA_ALLY ||
            si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_TARGET_CHAINHEAL_ALLY) {
            //victim->GetThreatManager()

            Unit* owner = me;
            if (me->GetOwner()->ToPlayer() == nullptr)
                owner = me->GetOwner();
            Unit* t = SelectLeastHpPctFriendly(owner, 50.0f, true);
            if (t && t->GetHealthPct() < 95) {
                if (si->Effects[0].Effect == SPELL_EFFECT_APPLY_AURA && t->HasAura(si->Id))
                    continue;
                casted = castSpell(t, i);
                break;
            }
            else {
                if (me->GetDistance(me->GetOwner()->GetPosition()) > 15.0f)
                    me->GetMotionMaster()->MoveCloserAndStop(1, me->GetOwner(), 15.0f);
            }
        }
    }
endloop:
    if (casted) {
        return true;
    }

    switch (_lastSpellResult) {
    case SpellCastResult::SPELL_FAILED_LINE_OF_SIGHT:
    case SpellCastResult::SPELL_FAILED_OUT_OF_RANGE:
        if (!me->HasUnitState(UNIT_STATE_CHASE) && isMovable && _type == ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER) {
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(victim, 2.0f);
        }
        break;
    case SpellCastResult::SPELL_FAILED_NO_POWER:
        _lastSpellResult = SpellCastResult::SPELL_CAST_OK;
        i++;
        goto redo;
        break;
    default:
        ;
    }

    return false;
}

float AssistanceAI::getSpecialFollowAngle() {
    uint32 index = 0;
    for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
        switch (me->m_spells[i]) {
        case 85990: // sword
            return 0.0f;
        case 86001: // tank
            return static_cast<float>(-M_PI/4);
        case 85980: // healer
            return static_cast<float>(M_PI/4);
        case 85970: // mage
            return static_cast<float>(3*M_PI/4);
        case 85895: // assistance
            return static_cast<float>(3*M_PI/2);
        }
    }
    return static_cast<float>(rand_chance()*M_PI);
}

bool AssistanceAI::hasSpell(uint32 id, uint32& index) {
    for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
        if (id == me->m_spells[i]) {
            index = i;
            return true;
        }
    }
    return false;
}

void AssistanceAI::ReadyToDie() {
    switch (me->GetEntry()) {
    case 46002:
        me->CastSpell(me->GetOwner(), 87278);
        break;
    }
}

void AssistanceAI::resetLifeTimer() {
    switch (me->GetEntry()) {
    case 46002:
        _lifeTimer = 20000;
        break;
    }
}

void AssistanceAI::Reborn(uint32 pct) {
    if (me->getDeathState() == ALIVE) {
        return;
    }
    me->setDeathState(ALIVE);
    me->SetHealth(uint32(me->GetMaxHealth()*pct/100));
    me->SetPower(POWER_MANA, uint32(me->GetMaxPower(POWER_MANA)));
    me->CastSpell(me, 85948);
    //me->m_Events.

    if (me->GetEntry() >= 45000 && me->GetEntry() <= 46000) {
        me->CastSpell(me, 86008);
    }

    AssistantsSpell(0, 0);
}

void AssistanceAI::JustDied(Unit* killer) {
    if (me->GetEntry() >= 45000) {
        uint32 index = 0;
        if (hasSpell(85949, index)) {
            if (spellsTimer[index][SPELL_TIMER_CURRENT] >= spellsTimer[index][SPELL_TIMER_ORIGIN]) {
                Reborn(100);
                spellsTimer[index][SPELL_TIMER_CURRENT] = 0;
                return;
            }
        }

        if (nullptr != me->GetOwner()->ToPlayer() && me->GetEntry() < 46000) {
            me->Yell(".......", Language::LANG_UNIVERSAL);
        }

        // Mage Assistant
        if (me->GetEntry() == 46030) {
            me->DespawnOrUnsummon(2s);
        }
        //me->DespawnOrUnsummon(14000);
    }
}

void AssistanceAI::updateTimer(uint32 diff)
{
    uint32 entry = me->GetEntry();
    if (entry < 40000)
        return;

    if (me->GetOwner() && me->GetOwner()->ToPlayer() && me->GetDistance(me->GetOwner()) > 50.0f) {
        me->NearTeleportTo(me->GetOwner()->GetPosition(), true);
        return;
    }

    // Update my level to owner's level
    if (me->GetLevel() < me->GetOwner()->GetLevel()) {
        me->SetLevel(me->GetOwner()->GetLevel());
        ((Guardian*)me)->InitStatsForLevel(me->GetLevel());
        return;
    }

    if (timerReady) {
        for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
            if (me->m_spells[i]) {
                if (spellsTimer[i][SPELL_TIMER_CURRENT] <  spellsTimer[i][SPELL_TIMER_ORIGIN])
                    spellsTimer[i][SPELL_TIMER_CURRENT] += diff;
            }
            else {
                break;
            }
        }
        if (gcd < ASSIST_GCD) {
            gcd += diff;
        }
        if (_lifeTimer > 0) {
            _lifeTimer -= diff;
            if (_lifeTimer <= 0) {
                ReadyToDie();
            }
        }
    }
    else {
        gcd = 1000;
        resetLifeTimer();
        for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
            if (me->m_spells[i]) {
                const SpellInfo* si = sSpellMgr->GetSpellInfo(me->m_spells[i]);

                if (si == nullptr)
                    continue;

                auraApplied[i] = false;

                if ((si->Effects[0].Effect == SPELL_EFFECT_APPLY_AURA || si->Effects[0].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID) && auraApplied[i] == false) {
                    if (si->GetRecoveryTime() == 0 &&
                        si->Id != 87278 &&
                        si->Id != 85891) {
                        castSpell(me, i);
                        auraApplied[i] = true;
                    }
                }
                spellsTimer[i][SPELL_TIMER_CURRENT] = si->GetRecoveryTime() - si->StartRecoveryTime;
                spellsTimer[i][SPELL_TIMER_ORIGIN] = si->GetRecoveryTime();
            }
        }
        timerReady = true;
    }
}

void AssistanceAI::UseInstanceHealing() {
    for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
        const SpellInfo* si = sSpellMgr->GetSpellInfo(me->m_spells[i]);

        if (si == nullptr || !isSpellReady(i) || si->IsPassive())
            continue;

        if (si->Effects[0].Effect != SPELL_EFFECT_HEAL || si->CastTimeEntry->Base > 0)
            continue;

        if (si->RangeEntry->ID == 1 && me->GetHealthPct() < 100.0f) {
            castSpell(me, i);
            return;
        }
        Unit* owner = me;
        if (me->GetOwner()->ToPlayer() == nullptr)
            owner = me->GetOwner();
        Unit* t = SelectLeastHpPctFriendly(owner, 30.0f, false);
        if (t == nullptr)
            continue;

        if (si->Effects[0].Effect == SPELL_EFFECT_APPLY_AURA && t->HasAura(si->Id)) {
            continue;
        }

        castSpell(t, i);
        return;
    }
}

// Unit is idle. Only heal spells can cast
void AssistanceAI::ResetPosition(bool force)
{
    UseInstanceHealing();

    if (AIFlag == AI_ACTION_FLAG::AI_ACTION_HOLD_POSITION)
        return;

    if (!isMovable) {
        me->StopMoving();
        return;
    }

    if (force || !(me->GetVictim() && me->EnsureVictim()->IsAlive())) {
        //MovementGenerator* mg = me->GetMotionMaster()->;
        if (!(me->IsCharmed() || me->IsSummon() || me->IsGuardian())) {
            return;
        }

        if (!force) {
            me->AttackStop();
            me->SetTarget(ObjectGuid::Empty);
            if (me->HasUnitState(UNIT_STATE_FOLLOW) && isInCombat == false) {
                return;
            }
        }

        me->StopMoving();
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveFollowEx(me->GetCharmerOrOwner(), _followDistance, _followAngle);
        if (!force) {
            isInCombat = false;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
        }
    }
}

bool AssistanceAI::AddOneTimeSpell(int32 spellId) {
    for (int i = 0; i < MAX_CREATURE_SPELL; i++) {
        if (oneTimeSpells[i] == 0) {
            oneTimeSpells[i] = spellId;
            return true;
        }
    }

    return false;
}

void AssistanceAI::JustAppeared() {
    uint32 effSpell = 0;
    const char* greeting = getCustomGreeting(me->GetEntry());
    Player* owner = me->GetOwner()? me->GetOwner()->ToPlayer():nullptr;

    if (greeting)
        me->Say(greeting, Language::LANG_UNIVERSAL, me->GetOwner());

    _followAngle = getSpecialFollowAngle();
    _followDistance = 1.2f;

    updateTimer(0);

    //  War III heroes
    if (me->GetEntry() >= 45000 && me->GetEntry() <= 45100) {
        me->CastSpell(me, 86008);
    } else if (owner) {
        UnitAddCriticalRate(me, owner->GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1+1) / 2 + owner->GetFloatValue(PLAYER_CRIT_PERCENTAGE) / 2);
    }

    _class = ASSISTANCE_CLASS::NONE;
    _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_MELEE;


    // Set class and attack type
    switch (me->GetEntry()) {
    case 45000: // MK
        me->SetCanDualWield(true);
    case 45006: // Tauren Cheif
    case 45012: // DK
        _class = ASSISTANCE_CLASS::TANK;
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_MELEE;
        break;
    case 45001: // watcher
        // Watcher dont have off hand animation
        //me->SetCanDualWield(true);
        me->ApplyAttackTimePercentMod(WeaponAttackType::BASE_ATTACK, 100, true);
        break;
    case 45002: // Shadow Shaman
    case 45010: // Forest Guard
        _class = ASSISTANCE_CLASS::HEALER;
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        break;
    case 45007: // Paladin
        _class = ASSISTANCE_CLASS::HEALER;
        break;
    case 45011: // BM
        me->SetCanDualWield(true);
        break;
    case 46002: // Wild Ghost
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        if (owner && owner->GetRace() == Races::RACE_BLOODELF) {
            
        }
        break;
    case 46016:
        me->SetCanDualWield(true);
        break;
    case 44000:
    case 45003:
    case 45004:
    case 45005:
    case 45009:
    case 45016:
    case 46000:
    case 46001:
    case 46006:
    case 46009:
    case 46011:
    case 46012:
    case 46015:
    case 46028:
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        break;
    case 46018:
        _awakeTimer = 1500;
        effSpell = 85907;
        break;
    case 46023: // Hellfire
        me->SetVisible(false);
        _awakeTimer = 1800;
        break;
    case 46003:
    case 46004:
    case 46005:
        if (owner && !owner->HasAura(87285)) {
            Pet* pet = owner->GetPet();
            uint32 entry = me->GetEntry();
            if (pet &&
                (
                    (entry==46005 && pet->GetEntry()== 17252) ||
                    (entry==46004 && pet->GetEntry()==1863) ||
                    (entry==46003 && pet->GetEntry()==417)
                )) {
                effSpell = 87285;
                me->ToTempSummon()->m_timer = (uint32)-1;
                me->ToTempSummon()->m_lifetime = (uint32)-1;
                _followDistance = 1;
                _followAngle = float(M_PI * 3 / 2);
            }
        }
        break;
    case 46026:
        effSpell = 87285;
        me->ToTempSummon()->m_timer = (uint32)-1;
        me->ToTempSummon()->m_lifetime = (uint32)-1;
        _followDistance = 1;
        _followAngle = float(M_PI * 3 / 2);
        break;
    case 46025:
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        me->ToTempSummon()->m_timer = (uint32)-1;
        me->ToTempSummon()->m_lifetime = (uint32)-1;
        _followDistance = 1;
        _followAngle = float(M_PI *3 / 2);
        effSpell = 87285;
        break;
    case 46020:
    case 46021:
    case 46019:
        _awakeTimer = 1500;
        effSpell = 85907;
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        break;
    case 46030:
        effSpell = 85907;
        _followDistance = 0.5f;
        _followAngle = float(M_PI * 5 / 4);
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        break;
    case 46007:
    case 46029: // Demon Portal
        isMovable = false;
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        break;
    case 46017:
        _awakeTimer = 4000;
        break;
    case 46024:
        effSpell = 89001;
        me->AddUnitMovementFlag(MOVEMENTFLAG_HOVER);
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        break;
    case 46031:
        effSpell = 89001;
        me->AddUnitMovementFlag(MOVEMENTFLAG_HOVER);
        _followDistance = 0.1f;
        me->SetFloatValue(UNIT_FIELD_HOVERHEIGHT, 4);
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
        canAttack = false;
        break;
    default:
        _type = ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_MELEE;
    }

    if (effSpell > 0) {
        me->CastSpell(me, effSpell);
    }
    AssistantsSpell(0, nullptr);

    me->StopMoving();
    me->GetMotionMaster()->Clear();
}

inline bool AssistanceAI::isCaster()
{
    return _type == ASSISTANCE_ATTACK_TYPE::ATTACK_TYPE_CASTER;
}

void AssistanceAI::EngagementStart(Unit* who) {
    if (who->GetEntry() == 36954) {
        me->DisengageWithTarget(who);
    }
}

void AssistanceAI::UpdateAI(uint32 diff/*diff*/)
{
    if (_updateTimer > diff) {
        _updateTimer -= diff;
        return;
    }
    else {
        _updateTimer = AAI_DEFAULT_UPDATE_TIMER;
    }

    // We are not awaken. Do nothing
    if (_awakeTimer > 0) {
        me->StopMoving();
        _awakeTimer -= diff;
        if (_awakeTimer <= 0) {
            me->SetVisible(true);
        }
        return;
    }

    if (!canAttack)
        return ResetPosition();

    // update spells cool down
    updateTimer(diff);

    // we are not in combat. return
    if (!me->IsEngaged() && (!me->GetOwner() || !me->GetOwner()->IsEngaged()))
        return ResetPosition();

    if (me->HasUnitState(UNIT_STATE_CASTING)) {
        return;
    }

    Unit* victim = GetVictim();

    if (victim && victim->GetEntry() != 36954) {
        if (!isInCombat) {
            isInCombat = true;
            me->StopMoving();
            me->GetMotionMaster()->Clear();
        }

        if (isTargetChanged == true) {
            if (!isCaster()) {
                if (AIFlag == AI_ACTION_FLAG::AI_ACTION_NONE || isTargetChanged)
                    me->GetMotionMaster()->MoveChase(victim);
            }
        }
    } else {
        return ResetPosition();
    }

    bool res = AssistantsSpell(diff, victim);
    if (isCaster()) {
        if (res == true) {
            // As a caster once we successfully casted one spell. We should stop if we are moving
            me->StopMoving();
            if (me->HasUnitState(UNIT_STATE_CHASE)) {
                me->GetMotionMaster()->Clear();
                me->ClearUnitState(UNIT_STATE_CHASE);
            }
        }
    }
    else {
        DoMeleeAttackIfReady();
    }
}

