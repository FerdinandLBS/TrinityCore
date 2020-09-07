
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
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"

class spell_army_teleport : public SpellScriptLoader
{
public:
    spell_army_teleport() : SpellScriptLoader("spell_army_teleport") { }

    class spell_army_teleport_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_army_teleport_SpellScript);

        void HandleAfterCast() {
            Unit* caster = GetCaster();

            for (int i = 85903; i <= 85906; i++) {
                caster->CastSpell(caster, i);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_army_teleport_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_army_teleport_SpellScript();
    }
};

class spell_holly_light : public SpellScriptLoader
{
public:
    spell_holly_light() : SpellScriptLoader("spell_holly_light") { }

    class spell_holly_light_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_holly_light_SpellScript);

        void HandleAfterCast() {
            Unit* t;
            Unit* u = GetCaster();
            const Spell* si = GetSpell();

            if (!u || !si || !(t = si->m_targets.GetUnitTarget()))
                return;


            HealInfo hi(u, t, t->GetMaxHealth() / 10, GetSpellInfo(), SpellSchoolMask::SPELL_SCHOOL_MASK_HOLY);
            t->HealBySpell(hi, false);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_holly_light_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_holly_light_SpellScript();
    }
};

class spell_dark_ceremony : public SpellScriptLoader
{
public:
    spell_dark_ceremony() : SpellScriptLoader("spell_dark_ceremony") { }

    class spell_dark_ceremony_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dark_ceremony_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = this->GetCaster();
            Unit* target = GetSpell()->m_targets.GetUnitTarget();
            uint32 ratio = 1;

            if (!target || !caster)
                return;

            if (target->GetMaxHealth() < caster->GetMaxHealth() / 2) {
                ratio = 3;
            }

            caster->SetPower(POWER_MANA, target->GetMaxHealth()*ratio/50 + 150 + caster->GetPower(POWER_MANA));
            caster->DealDamage(caster, target, target->GetMaxHealth()*ratio / 10);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_dark_ceremony_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dark_ceremony_SpellScript();
    }
};

Unit* SelectMostHpPctFriedly(Unit* who, float range) {
    Unit* unit = nullptr;
    Trinity::AllFriendlyCorpseInGrid u_check(who);
    Trinity::UnitLastSearcher<Trinity::AllFriendlyCorpseInGrid> searcher(who, unit, u_check);
    Cell::VisitAllObjects(who, searcher, range);

    return unit;
}

class spell_palading_rise_ally : public SpellScriptLoader
{
public:
    spell_palading_rise_ally() : SpellScriptLoader("spell_palading_rise_ally") { }

    class spell_palading_rise_ally_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_palading_rise_ally_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();

            std::vector<Creature*> tempList;

            Trinity::AllFriendlyCorpseInGrid check(caster);
            Trinity::CreatureListSearcher<Trinity::AllFriendlyCorpseInGrid> searcher(caster, tempList, check);
            Cell::VisitGridObjects(caster, searcher, 40.0f);

            if (tempList.empty())
                return;

            for (Creature* creature : tempList)
            {
                if (creature->GetEntry() < 45000)
                    continue;

                AssistanceAI* ai = (AssistanceAI*)creature->GetAI();
                ai->Reborn(100);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_palading_rise_ally_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_palading_rise_ally_SpellScript();
    }
};

class spell_dark_ranger_black_bolt : public SpellScriptLoader
{
public:
    spell_dark_ranger_black_bolt() : SpellScriptLoader("spell_dark_ranger_black_bolt") { }

    class spell_dark_ranger_black_bolt_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dark_ranger_black_bolt_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetCaster())
                return;

            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode != AURA_REMOVE_BY_DEATH)
                return;

            GetCaster()->CastSpell(GetTarget(), 85921, aurEff);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_dark_ranger_black_bolt_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dark_ranger_black_bolt_AuraScript();
    }
};

class spell_death_contract : public SpellScriptLoader
{
public:
    spell_death_contract() : SpellScriptLoader("spell_death_contract") { }

    class spell_death_contract_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_death_contract_SpellScript);

        void HandleAfterCast()
        {
            Unit* caster = GetCaster();
            Unit* target = GetSpell()->m_targets.GetUnitTarget();

            if (!target) {
                return;
            }

            caster->SetHealth(caster->GetHealth()+ target->GetMaxHealth() * 3);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_death_contract_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_death_contract_SpellScript();
    }
};

class spell_rain_of_fire : public SpellScriptLoader
{
public:
    spell_rain_of_fire() : SpellScriptLoader("spell_rain_of_fire") { }

    class spell_rain_of_fire_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rain_of_fire_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect* aurEff)
        {
            Unit* caster = GetCaster();
            if (caster)
                caster->CastSpell(GetDynobjOwner()->GetPosition(), 85882, true);
        }

        void Register() override
        {
            OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_rain_of_fire_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_rain_of_fire_AuraScript();
    }
};

class spell_doom_curse : public SpellScriptLoader
{
public:
    spell_doom_curse() : SpellScriptLoader("spell_doom_curse") { }

    class spell_doom_curse_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_doom_curse_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetCaster())
                return;

            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode != AURA_REMOVE_BY_DEATH)
                return;

            GetCaster()->CastSpell(GetTarget(), 85881, aurEff);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_doom_curse_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_doom_curse_AuraScript();
    }
};

void AddSC_war3_hero_spells_script()
{
    new spell_army_teleport();
    new spell_dark_ceremony();
    new spell_dark_ranger_black_bolt();
    new spell_death_contract();
    new spell_palading_rise_ally();
    new spell_rain_of_fire();
    new spell_doom_curse();
}
