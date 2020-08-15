
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
            Player* teacher = eventInfo.GetActor()->ToPlayer();

            if (teacher == nullptr)
                return;

            Aura* learn = teacher->GetAura(87209);
            if (learn == nullptr)
                return;
            Unit* chicken = learn->GetCaster();

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

            Player* owner = nullptr;
            if (caster->GetOwner()) {
                owner = caster->GetOwner()->ToPlayer();
            }
            if (!owner)
                return;

            AssistanceAI* ai = (AssistanceAI*)caster->GetAI();
            Spell* spell = owner->GetCurrentSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
            if (spell && spell->GetSpellInfo()->SpellFamilyName == 3) {
                if (SpellCastResult::SPELL_CAST_OK != caster->CastSpell(spell->m_targets, spell->GetSpellInfo()->Id)) {
                    ai->ResetPosition(true);
                }
            }
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
                int32 value = caster->GetMaxPower(POWER_MANA) / 13 ;
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
    int spells[] = {87251, 87254, 87255, 87256, 87264, 87271, 87272};

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
            if (!p)
                return;
            Pet* pet = p->GetPet();
            if (pet)
                entry = pet->GetEntry();

            if (doubleDeamonAura) {
                uint32 ddEntry = doubleDeamonAura->GetCaster()->GetEntry();

                // TODO: add more pets here
                if ((entry == 1860 && ddEntry == 46026) ||
                    (entry == 416 && ddEntry == 46025) ||
                    (entry == 17252 && ddEntry == 46005) ||
                    (entry == 1863 && ddEntry == 46004) ||
                    (entry == 417 && ddEntry == 46003))
                    return;

                doubleDeamonAura->GetCaster()->ToCreature()->DespawnOrUnsummon();
                if (!pet) {
                    return;
                }
            }
            switch (entry) {
            case 1860:
                spell = 87286;
                break;
            case 416:
                spell = 87284;
                break;
            case 17252:
                spell = 87256;
                break;
            case 1863:
                spell = 87255;
                break;
            case 417:
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
            owner->m_Events.AddEvent(new SpellDelayCastEvent(87280, owner, GetSpell()->m_targets, args), owner->m_Events.CalculateTime(1100));
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
}
