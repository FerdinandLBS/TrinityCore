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

/////////////////
// AggressorAI
/////////////////


Unit* SelectLeastHpPctFriendly(Unit* who, float range, bool isCombat) {
    Unit* unit = nullptr;
    Trinity::MostHPMissingPctInRange u_check(who, range, isCombat);
    Trinity::UnitLastSearcher<Trinity::MostHPMissingPctInRange> searcher(who, unit, u_check);
    Cell::VisitAllObjects(who, searcher, range);

    return unit;
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

Unit* AssistanceAI::SelectNextTarget(bool allowAutoSelect)
{
    // Provides next target selection after current target death.
    // This function should only be called internally by the AI
    // Targets are not evaluated here for being valid targets, that is done in _CanAttack()
    // The parameter: allowAutoSelect lets us disable aggressive pet auto targeting for certain situations

    // Passive pets don't do next target selection
    //if (me->HasReactState(REACT_PASSIVE))
    //    return nullptr;

    // Check pet attackers first so we don't drag a bunch of targets to the owner
    if (Unit* myAttacker = me->getAttackerForHelper())
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

    // Neither pet or owner had a target and aggressive pets can pick any target
    // To prevent aggressive pets from chain selecting targets and running off, we
    //  only select a random target if certain conditions are met.
    if (me->HasReactState(REACT_AGGRESSIVE) && allowAutoSelect)
    {
        if (!me->GetCharmInfo()->IsReturning() || me->GetCharmInfo()->IsFollowing() || me->GetCharmInfo()->IsAtStay())
            if (Unit* nearTarget = me->SelectNearestHostileUnitInAggroRange(true))
                return nearTarget;
    }

    // Default - no valid targets
    return nullptr;
}

void AssistanceAI::castSpell(WorldObject* target, uint32 index) {
    me->CastSpell(target, me->m_spells[index]);
    spellsTimer[index][SPELL_TIMER_CURRENT] = 0;
    gcd = 0;
}

void AssistanceAI::castSpell(const Position &dest, uint32 index) {
    me->CastSpell(dest, me->m_spells[index]);
    spellsTimer[index][SPELL_TIMER_CURRENT] = 0;
    gcd = 0;
}

bool AssistanceAI::isSpellReady(uint32 index) {
    return (spellsTimer[index][SPELL_TIMER_CURRENT] >= spellsTimer[index][SPELL_TIMER_ORIGIN] && gcd >= ASSIST_GCD);
};

bool AssistanceAI::AssistantsSpell(uint32 diff) {
    if (me->GetEntry() < 45000) {
        if (me->GetEntry() < 40000)
            return false;
    }

    bool casted = false;
    Unit* victim = me->GetVictim();
    Unit* ownerVictim = me->GetOwner()->GetVictim();
    if (victim == nullptr || (ownerVictim != nullptr && victim != ownerVictim)) {
        victim = me->GetOwner()->GetVictim();
        if (victim == nullptr) {
            victim = me->GetOwner()->GetCombatManager().GetAnyTarget();
        }
    }

    if (me->HasUnitState(UNIT_STATE_CASTING))
        return true;

    for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
        const SpellInfo* si = sSpellMgr->GetSpellInfo(me->m_spells[i]);

        if (si == nullptr)
            return false;

        if (si->IsPassive() || !isSpellReady(i)) {
            continue;
        }

        // heal and buff and no target spell
        if (si->IsSelfCast()) {
            if (si->Effects[0].IsEffect(SpellEffects::SPELL_EFFECT_APPLY_AURA) ||
                si->Effects[0].IsEffect(SPELL_EFFECT_APPLY_AREA_AURA_RAID)) {
                if (me->HasAura(me->m_spells[i])) {
                    continue;
                }
            }
            castSpell(me, i);
            casted = true;
            break;
        }

        if (si->RangeEntry->ID == 1 || si->RangeEntry->ID == 3) {
            castSpell(me, i);
            casted = true;
            break;
        }

        if (si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_TARGET_ENEMY ||
            si->Effects[0].TargetA.GetTarget() == TARGET_DEST_TARGET_ENEMY) {
            if (victim == nullptr)
                continue;

            float x, y, z;
            victim->GetPosition(x, y, z);

            if (!me->IsWithinLOS(x, y, z, LINEOFSIGHT_ALL_CHECKS, VMAP::ModelIgnoreFlags::M2) ||
                me->GetDistance(victim->GetPosition()) > si->RangeEntry->maxRangeHostile) {
                if (!me->HasUnitState(UNIT_STATE_CHASE)) {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(victim, 2.0f);
                }
                //me->GetMotionMaster()->MoveCloserAndStop(1, victim, dist-2.0f);
                return false;
            }
            if (si->Effects[0].IsEffect(SPELL_EFFECT_APPLY_AURA) && victim->HasAura(me->m_spells[i]) && !si->Effects[1].IsEffect(SPELL_EFFECT_SCHOOL_DAMAGE))
                continue;
            castSpell(victim, i);
            casted = true;
            break;
        }

        if ((si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
            si->Effects[1].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENEMY ||
            si->Effects[0].TargetA.GetTarget() == TARGET_DEST_DYNOBJ_ENEMY)
            && victim) {
            castSpell(victim->GetPosition(), i);
            casted = true;
            break;
        }

        /* We are healers */
        if (me->HasSpell(85980) || me->HasSpell(85970)) {
            if (si->Effects[0].TargetA.GetTarget() == TARGET_UNIT_TARGET_ALLY || si->Effects[1].TargetB.GetTarget() == TARGET_UNIT_DEST_AREA_ALLY) {
                //victim->GetThreatManager()
                Unit* t = SelectLeastHpPctFriendly(me, 40.0f, true);
                if (t) {
                    if (si->Effects[0].Effect == SPELL_EFFECT_APPLY_AURA && t->HasAura(si->Id))
                        continue;
                    castSpell(t, i);
                    return true;
                }
                else {
                    if (me->GetDistance(me->GetOwner()->GetPosition()) > 15.0f)
                        me->GetMotionMaster()->MoveCloserAndStop(1, me->GetOwner(), 15.0f);
                }
            }

            casted = true;
        }
    }

    return casted;
}

bool AssistanceAI::miniPetAI(uint32 diff) {
    return false;
}

void AssistanceAI::JustDied(Unit* killer) {
    if (me->GetEntry() >= 45000) {
        me->Yell("???.......", Language::LANG_UNIVERSAL);
        me->DespawnOrUnsummon(3000);
        me->GetOwner()->_popAssistance(me);
    }
}

void AssistanceAI::updateTimer(uint32 diff)
{
    if (me->GetEntry() < 40000)
        return;

    if (me->GetDistance(me->GetOwner()) > 100.0f) {
        me->NearTeleportTo(me->GetOwner()->GetPosition(), true);
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
        if (gcd < 1000) {
            gcd += diff;
        }
    }
    else {
        gcd = 1000;
        for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
            if (me->m_spells[i]) {
                const SpellInfo* si = sSpellMgr->GetSpellInfo(me->m_spells[i]);

                if (si == nullptr)
                    continue;

                auraApplied[i] = false;

                if ((si->Effects[0].Effect == SPELL_EFFECT_APPLY_AURA || si->Effects[0].Effect == SPELL_EFFECT_APPLY_AREA_AURA_RAID) && auraApplied[i] == false) {
                    castSpell(me, i);
                    auraApplied[i] = true;
                }
                spellsTimer[i][SPELL_TIMER_CURRENT] = si->GetRecoveryTime();
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

        if (si->Effects[0].Effect != SPELL_EFFECT_HEAL || si->CastTimeEntry->CastTime > 0)
            continue;

        if (si->RangeEntry->ID == 1 && me->GetHealthPct() < 100.0f) {
            castSpell(me, i);
            return;
        }

        Unit* t = SelectLeastHpPctFriendly(me, 30.0f, false);
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
void AssistanceAI::ResetPosition()
{
    UseInstanceHealing();

    if (AIFlag == AI_ACTION_FLAG::AI_ACTION_HOLD_POSITION)
        return;

    if (!(me->GetVictim() && me->EnsureVictim()->IsAlive())) {
        //MovementGenerator* mg = me->GetMotionMaster()->;
        if (!(me->IsCharmed() || me->IsSummon() || me->IsGuardian())) {
            return;
        }

        me->AttackStop();
        me->SetTarget(ObjectGuid::Empty);
        if (me->HasUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE)) {
            //return;
        }
        if (me->HasUnitMovementFlag(UNIT_STATE_FOLLOW)) {
            return;
        }

        if (me->GetCharmInfo() && !me->GetCharmInfo()->IsFollowing() && !me->GetCharmInfo()->IsReturning())
        {
            me->GetCharmInfo()->SetIsReturning(true);
        }
        me->GetMotionMaster()->Clear();

        float dist = 1.0f;
        if (me->HasSpell(86001))
            dist *= 3;

        me->GetMotionMaster()->MoveFollow(me->GetCharmerOrOwner(), dist, me->GetFollowAngle());
        me->SetUnitMovementFlags(UNIT_STATE_FOLLOW);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
    }
}

bool AssistanceAI::isCaster()
{
    switch (me->GetEntry()) {
    case 44000:
    case 45002:
    case 45003:
    case 45005:
    case 45007:
        return true;
    }
    return false;
}

void AssistanceAI::EngagementStart(Unit* who) {
    if (who->GetEntry() == 36954) {
        me->DisengageWithTarget(who);
    }
}

void AssistanceAI::UpdateAI(uint32 diff/*diff*/)
{
    bool isTargetChanged = false;

    // update spells cool down
    updateTimer(diff);

    // we are not in combat. return
    if (!me->IsEngaged() && (!me->GetOwner() || !me->GetOwner()->IsInCombat()))
        return ResetPosition();

    Unit* victim = me->SelectVictim();
    Unit* owner = me->GetOwner();

    // fix the target
    if (owner) {
        Unit* ov = owner->GetVictim();
        if (victim == nullptr || (victim != ov && ov)) {
            isTargetChanged = true;
            victim = ov;
        }
    }
    if (victim && victim->GetEntry() != 36954) {
        if ((!me->GetTarget() && victim != me->GetVictim()) || isTargetChanged == true) {
            if (isCaster()) {
                // Set our target

                me->SetTarget(victim->GetGUID());
                me->GetOwner()->EngageWithTarget(victim);
                me->GetMotionMaster()->Clear();
            }
            else {
                if (AIFlag == AI_ACTION_FLAG::AI_ACTION_NONE || isTargetChanged)
                    AttackStart(victim);
            }
        }
    } else
        return ResetPosition();

    bool res = AssistantsSpell(diff);
    if (res == false) {
        DoMeleeAttackIfReady();
    }
}

