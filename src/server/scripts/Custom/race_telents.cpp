
#include "ScriptMgr.h"
#include "Containers.h"
#include "DBCStores.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "PlayerAI.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

#include "CombatAI.h"
#include "AssistanceAI.h"
#include "Random.h"
#include "MotionMaster.h"
#include "WorldSession.h"
#include "Pet.h"


#include "SpellUtility.h"

#include "bot_ai.h"

bool IsChanneling(Unit const* u = nullptr) { return u->GetCurrentSpell(CURRENT_CHANNELED_SPELL); }
bool IsCasting(Unit const* u = nullptr) { return (u->HasUnitState(UNIT_STATE_CASTING) || IsChanneling(u) || u->IsNonMeleeSpellCast(false, false, true, false, false)); }

class spell_summon_green_gem : public SpellScriptLoader
{
public:
    spell_summon_green_gem() : SpellScriptLoader("spell_summon_green_gem") { }

    class spell_summon_green_gem_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_green_gem_SpellScript);

        SpellCastResult CheckCast() {
            Unit* caster = GetCaster();
            if (!caster)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            Player* player = caster->ToPlayer();

            if (!player)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            std::list<Creature*> minions;
            player->GetAllMinionsByEntry(minions, 46031);
            if (minions.size() >= 3) {
                return SpellCastResult::SPELL_FAILED_TOO_MANY_SOCKETS;
            }

            return SpellCastResult::SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_summon_green_gem_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_summon_green_gem_SpellScript();
    }
};

class spell_sunwell_power : public SpellScriptLoader
{
public:
    spell_sunwell_power() : SpellScriptLoader("spell_sunwell_power") { }

    class spell_sunwell_power_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sunwell_power_SpellScript);

        SpellCastResult CheckCast() {
            Unit* caster = GetCaster();
            if (!caster)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            Player* player = caster->ToPlayer();

            if (!player)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            std::list<Creature*> minions;
            player->GetAllMinionsByEntry(minions, 46031);
            if (minions.size() <= 0) {
                return SpellCastResult::SPELL_FAILED_NOT_READY;
            }

            return SpellCastResult::SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_sunwell_power_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sunwell_power_SpellScript();
    }
};

class spell_smart_chicken_aura : public SpellScriptLoader
{
public:
    spell_smart_chicken_aura() : SpellScriptLoader("spell_smart_chicken_aura") { }


    class spell_smart_chicken_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_smart_chicken_aura_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            Unit* teacher = eventInfo.GetActor();

            if (teacher == nullptr)
                return;

            Aura* learn = teacher->GetAura(87209);
            if (learn == nullptr)
                return;

            std::list<Creature*> list;
            teacher->GetAllMinionsByEntry(list, 46001);

            Unit* chicken;/* = learn->GetCaster();
            if (!chicken || IsCasting(chicken)) {
                return;
            }*/

            if (list.size() == 0)
                return;

            chicken = list.front();

            const Spell* sp = eventInfo.GetProcSpell();
            chicken->CastSpell(sp->m_targets, sp->GetSpellInfo()->Id);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_smart_chicken_aura_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_smart_chicken_aura_AuraScript();
    }
};

class spell_human_mage : public SpellScriptLoader
{
public:
    spell_human_mage() : SpellScriptLoader("spell_human_mage") { }


    class spell_human_mage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_human_mage_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            Player* caster = eventInfo.GetActor()->ToPlayer();

            if (caster == nullptr)
                return;

            if (eventInfo.GetDamageInfo()->GetDamage() <= 1) {
                return;
            }

            int spell;
            const Spell* sp = eventInfo.GetProcSpell();
            switch (eventInfo.GetDamageInfo()->GetSchoolMask()) {
            case SpellSchoolMask::SPELL_SCHOOL_MASK_FIRE:
                spell = 87215; // arcane
                caster->RemoveAura(87213);
                break;
            case SpellSchoolMask::SPELL_SCHOOL_MASK_FROST:
                spell = 87213; // fire
                caster->RemoveAura(87214);
                break;
            case SpellSchoolMask::SPELL_SCHOOL_MASK_ARCANE:
                spell = 87214; // frost
                caster->RemoveAura(87215);
                break;
            default:
                return;
            }
            caster->CastSpell(caster, spell, true);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_human_mage_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_human_mage_AuraScript();
    }
};


class spell_draenei_mage_learn : public SpellScriptLoader
{
public:
    spell_draenei_mage_learn() : SpellScriptLoader("spell_draenei_mage_learn") { }

    class spell_draenei_mage_learn_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_draenei_mage_learn_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            Unit* owner = caster->GetOwner();
            if (!owner)
                return;

            AssistanceAI* ai = (AssistanceAI*)caster->GetAI();
            for (int i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; i++) {
                Spell* spell = owner->GetCurrentSpell(i);
                if (spell && spell->GetSpellInfo()->SpellFamilyName == 3) {
                    if (SpellCastResult::SPELL_CAST_OK == caster->CastSpell(spell->m_targets, spell->GetSpellInfo()->Id)) {
                        return;
                    }
                }
            }
            ai->ResetPosition(true);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_draenei_mage_learn_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_draenei_mage_learn_SpellScript();
    }
};

class spell_draenei_mage_manatrans : public SpellScriptLoader
{
public:
    spell_draenei_mage_manatrans() : SpellScriptLoader("spell_draenei_mage_manatrans") { }


    class spell_draenei_mage_manatrans_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_draenei_mage_manatrans_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect* aurEff)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            Unit* owner = caster->GetOwner();
            if (!owner)
                return;

            int32 myPct, ownerPct;

            myPct = caster->GetPower(POWER_MANA) * 1000 / caster->GetMaxPower(POWER_MANA);
            ownerPct = owner->GetPower(POWER_MANA) * 1000 / owner->GetMaxPower(POWER_MANA);
            if (myPct > ownerPct && myPct >= 80) {
                int32 value = caster->GetMaxPower(POWER_MANA) / 13;
                owner->ModifyPower(POWER_MANA, value);
                caster->ModifyPower(POWER_MANA, -value);
            }
        }

        void Register() override
        {
            OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_draenei_mage_manatrans_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_draenei_mage_manatrans_AuraScript();
    }
};

class spell_draenei_mage_encourage : public SpellScriptLoader
{
public:
    spell_draenei_mage_encourage() : SpellScriptLoader("spell_draenei_mage_encourage") { }

    class spell_draenei_mage_encourage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_draenei_mage_encourage_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();

            if (!caster)
                return;

            Player* owner = caster->GetOwner()->ToPlayer();
            if (!owner)
                return;

            uint32 spell = 0;
            CastSpellExtraArgs args;
            //args.SetOriginalCaster(owner->GetGUID());
            args.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
            switch (rand() % 6) {
            case 0:
                spell = 44401;
                break;
            case 1:
                spell = 44544;
                break;
            case 2:
                spell = 57761;
                break;
            case 3:
                spell = 12536;
                break;
            case 4:
                spell = 64343;
                break;
            case 5:
                spell = 54741;
                break;
            default:
                return;
            }
            owner->CastSpell(owner, spell, args);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_draenei_mage_encourage_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_draenei_mage_encourage_SpellScript();
    }
};


class spell_wild_imp_enhance_master : public SpellScriptLoader
{
public:
    spell_wild_imp_enhance_master() : SpellScriptLoader("spell_wild_imp_enhance_master") { }

    class spell_wild_imp_enhance_master_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_wild_imp_enhance_master_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();

            caster->CastSpell(caster, 8329);
        }

        void Register() override
        {
            AfterHit += SpellHitFn(spell_wild_imp_enhance_master_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_wild_imp_enhance_master_SpellScript();
    }
};

void SummonRandomDemon(Unit* caster, Unit* originCaster) {
    int index = rand() % 7;
    int spells[] = { 87251, 87254, 87255, 87256, 87264, 87271, 87272 };

    if (index == 0 || originCaster) {
        CastSpellExtraArgs args;
        if (!index)
            args.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT0, 3);
        if (originCaster)
            args.SetOriginalCaster(originCaster->GetGUID());
        caster->CastSpell(caster, spells[index], args);
    }
    else {
        caster->CastSpell(caster, spells[index]);
    }
}

class spell_gnome_warlock : public SpellScriptLoader
{
public:
    spell_gnome_warlock() : SpellScriptLoader("spell_gnome_warlock") { }


    class spell_gnome_warlock_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gnome_warlock_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect* aurEff)
        {
            int spell;
            Unit* caster = GetCaster();
            uint32 entry = 0;
            Aura* doubleDeamonAura = caster->GetAura(87285);
            Player* p = caster->ToPlayer();
            Creature* pet;
            if (!p) {
                if (caster->GetEntry() >= 70000) {
                    pet = ((bot_ai*)caster->GetAI())->GetBotsPet();
                }
                else
                    return;
            }
            else
                pet = p->GetPet();

            if (pet)
                entry = pet->GetEntry();

            if (doubleDeamonAura) {
                uint32 ddEntry = doubleDeamonAura->GetCaster()->GetEntry();

                // TODO: add more pets here
                if (((entry == 1860 || entry == 70502) && ddEntry == 46026) || // Viodwalker
                    ((entry == 416 || entry == 70501) && ddEntry == 46025) ||  // imp
                    ((entry == 17252 || entry == 70505) && ddEntry == 46005) ||// felguard
                    ((entry == 1863 || entry == 70503) && ddEntry == 46004) || // succubus
                    ((entry == 417 || entry == 70504) && ddEntry == 46003))    // felhunter
                    return;

                doubleDeamonAura->GetCaster()->ToCreature()->DespawnOrUnsummon();
                if (!pet) {
                    return;
                }
            }
            switch (entry) {
            case 70502:
            case 1860: // VIODWALKER
                spell = 87286;
                break;
            case 70501:
            case 416: // IMP
                spell = 87284;
                break;
            case 70505:
            case 17252: // felguard
                spell = 87256;
                break;
            case 70503:
            case 1863: // succubus
                spell = 87255;
                break;
            case 70504:
            case 417: // felhunter
                spell = 87254;
                break;
            default:
                return;
            }

            CastSpellExtraArgs args(aurEff);
            args.AddSpellMod(SpellValueMod::SPELLVALUE_CRIT_CHANCE, 100);
            caster->CastSpell(caster, spell, args);
        }

        void Register() override
        {
            OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gnome_warlock_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gnome_warlock_AuraScript();
    }
};

class spell_human_warlock : public SpellScriptLoader
{
public:
    spell_human_warlock() : SpellScriptLoader("spell_human_warlock") { }


    class spell_human_warlock_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_human_warlock_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

#define HUMAN_WARLOCK_TALENT_TICK 501
        void HandleProc(AuraEffect* aurEff)
        {
            Unit* u = GetCaster();
            if (!u)
                return;

            Aura* aura;

            if (aura = u->GetAura(47283)) {
                if (aura->GetDuration() >= aura->GetMaxDuration()- HUMAN_WARLOCK_TALENT_TICK) {
                    aura->SetCharges(2);
                }
            }

            if (aura = u->GetAura(17941)) {
                if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK) {
                    aura->SetCharges(2);
                }
            }

            if (aura = u->GetAura(34936)) {
                if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK) {
                    aura->SetCharges(2);
                }
            }

            if (aura = u->GetAura(47241)) {
                if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK) {
                    aura->SetDuration(aura->GetDuration() + 16000);
                    aura->SetMaxDuration(aura->GetMaxDuration() + 16000);
                }
            }

            {
                int crush[3] = { 18093, 63243, 63244 };
                for (int i = 0; i < 3; i++) {
                    if (aura = u->GetAura(crush[i])) {
                        if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK) {
                            aura->SetDuration(aura->GetDuration() + 6000);
                            aura->SetMaxDuration(aura->GetMaxDuration() + 6000);
                        }
                        break;
                    }
                }
            }

            {
                int crush[3] = { 64368, 64370, 64371 };
                for (int i = 0; i < 3; i++) {
                    if (aura = u->GetAura(crush[i])) {
                        if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK) {
                            aura->SetDuration(aura->GetDuration() + 16000);
                            aura->SetMaxDuration(aura->GetMaxDuration() + 16000);
                        }
                        break;
                    }
                }
            }

            {
                int crush[3] = { 54274, 54276, 54277 };
                for (int i = 0; i < 3; i++) {
                    if (aura = u->GetAura(crush[i])) {
                        if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK)
                            aura->SetCharges(aura->GetCharges() + 3);
                        break;
                    }
                }
            }

            {
                int crush[3] = { 47383, 71162, 71165 };
                for (int i = 0; i < 3; i++) {
                    if (aura = u->GetAura(crush[i])) {
                        if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK)
                            aura->SetCharges(aura->GetCharges() + 2);
                        break;
                    }
                }
            }

            {
                int crush[2] = { 63165, 63167 };
                for (int i = 0; i < 2; i++) {
                    if (aura = u->GetAura(crush[i])) {
                        if (aura->GetDuration() >= aura->GetMaxDuration() - HUMAN_WARLOCK_TALENT_TICK)
                            aura->SetCharges(aura->GetCharges() + 1);
                        break;
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_human_warlock_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_human_warlock_AuraScript();
    }
};

class spell_bloodelf_warlock : public SpellScriptLoader
{
public:
    spell_bloodelf_warlock() : SpellScriptLoader("spell_bloodelf_warlock") { }

    class spell_bloodelf_warlock_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bloodelf_warlock_SpellScript);

        void HandleAfterCast()
        {
            Unit* owner = GetCaster();
            if (!owner)
                return;

            SummonRandomDemon(owner, owner->GetOwner());
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_bloodelf_warlock_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_bloodelf_warlock_SpellScript();
    }
};

class spell_undead_warlock : public SpellScriptLoader
{
public:
    spell_undead_warlock() : SpellScriptLoader("spell_undead_warlock") { }

    class spell_undead_warlock_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_undead_warlock_SpellScript);

        void HandleAfterCast() {
            Unit* owner = GetCaster();
            CastSpellExtraArgs args;
            owner->m_Events.AddEvent(new SpellDelayCastEvent(87280, owner, GetSpell()->m_targets, args), owner->m_Events.CalculateTime(Milliseconds(1100)));
            //owner->CastSpell(GetSpell()->m_targets, 87280, true);
        }

        void HandleAfterHit()
        {
            Unit* owner = GetCaster();
            if (!owner)
                return;
            Creature* victim = GetHitCreature();
            if (!victim)
                return;

            CastSpellExtraArgs args;
            args.SetOriginalCaster(owner->GetGUID());
            victim->CastSpell(owner, 87251, args);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_undead_warlock_SpellScript::HandleAfterCast);
            AfterHit += SpellHitFn(spell_undead_warlock_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_undead_warlock_SpellScript();
    }
};

class spell_undead_warlock2 : public SpellScriptLoader
{
public:
    spell_undead_warlock2() : SpellScriptLoader("spell_undead_warlock2") { }

    class spell_undead_warlock2_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_undead_warlock2_SpellScript);

        void HandleAfterCast()
        {
            Unit* owner = GetCaster();
            if (!owner)
                return;
            Player* target = owner->ToPlayer();

            target->GetSpellHistory()->ModifyCooldown(87279, -1000);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_undead_warlock2_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_undead_warlock2_SpellScript();
    }
};

class spell_bloodelf_paladin : public SpellScriptLoader
{
public:
    spell_bloodelf_paladin() : SpellScriptLoader("spell_bloodelf_paladin") { }

    class spell_bloodelf_paladin_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bloodelf_paladin_SpellScript);

        void HandleAfterCast() {
            Unit* u = GetCaster();
            Unit* t = GetSpell()->m_targets.GetUnitTarget();
            uint32 m = u->GetHealth() / 2;


            if (t->IsFriendlyTo(u)) {
                CastSpellExtraArgs args;
                args.AddSpellBP0(m);
                args.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
                if (u != t)
                    u->CastSpell(t, 87402, args);
            }
            else {
                CastSpellExtraArgs args;
                args.AddSpellBP0(m);
                args.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
                u->CastSpell(t, 87401, args);
            }
            if (u != t)
                u->DealDamage(u, u, m, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_HOLY, GetSpellInfo(), false);

            CastSpellExtraArgs args;
            args.AddSpellBP0((m>50)?(m/50):1);
            args.SetTriggerFlags(TriggerCastFlags::TRIGGERED_FULL_MASK);
            u->CastSpell(u, 87403, args);
        }

        void HandleAfterHit()
        {
        }

        SpellCastResult CheckCastResult() {
            Unit* u = GetCaster();

            if (!u || u->isDead())
                return SPELL_FAILED_CASTER_DEAD;

            Unit* t = GetSpell()->m_targets.GetUnitTarget();
            if (!t)
                return SPELL_FAILED_BAD_TARGETS;
            if (t->isDead())
                return SPELL_FAILED_TARGETS_DEAD;

            uint32 m = u->GetHealth() / 2;
            if (m == 0)
                return SPELL_FAILED_UNKNOWN;

            if (!t->IsFriendlyTo(u) && !t->CanHaveThreatList()) {
                return SPELL_FAILED_BAD_TARGETS;
            }

            return SpellCastResult::SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_bloodelf_paladin_SpellScript::CheckCastResult);
            AfterCast += SpellCastFn(spell_bloodelf_paladin_SpellScript::HandleAfterCast);
            //AfterHit += SpellHitFn(spell_bloodelf_paladin_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_bloodelf_paladin_SpellScript();
    }
};

class spell_orc_shaman_aura : public SpellScriptLoader
{
public:
    spell_orc_shaman_aura() : SpellScriptLoader("spell_orc_shaman_aura") { }

    class spell_orc_shaman_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_orc_shaman_aura_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            HealInfo* hi =eventInfo.GetHealInfo();
            DamageInfo* di = eventInfo.GetDamageInfo();

            Unit* u = GetCaster();

            if (hi && hi->GetHeal() > 0) {

            }
            else if (di && di->GetDamage() > 0) {

            } else {
                return;
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_orc_shaman_aura_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_orc_shaman_aura_AuraScript();
    }
};

void AddSC_race_telants_script()
{
    new spell_gnome_warlock();
    new spell_sunwell_power();
    new spell_draenei_mage_learn();
    new spell_draenei_mage_encourage();
    new spell_draenei_mage_manatrans();
    new spell_wild_imp_enhance_master();
    new spell_smart_chicken_aura();
    new spell_bloodelf_warlock();
    new spell_human_mage();
    new spell_undead_warlock();
    new spell_undead_warlock2();
    new spell_summon_green_gem();
    new spell_human_warlock();
    new spell_bloodelf_paladin();
    new spell_orc_shaman_aura();
}
