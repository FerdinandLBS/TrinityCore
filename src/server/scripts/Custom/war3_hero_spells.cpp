
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

            if (!target || !caster)
                return;

            caster->SetPower(POWER_MANA, target->GetMaxHealth()/50 + 50);
            caster->DealDamage(caster, target, target->GetMaxHealth() / 10);
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

        bool Load() override
        {
            return GetCaster();
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (!GetCaster())
                return;

            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode != AURA_REMOVE_BY_DEATH || !IsExpired())
                return;

            GetCaster()->CastSpell(GetTarget(), 85921, aurEff);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_dark_ranger_black_bolt_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_DEFAULT);
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

            HealInfo info(caster, caster, target->GetHealth()*3, GetSpell()->m_spellInfo, SpellSchoolMask::SPELL_SCHOOL_MASK_SHADOW);
            caster->HealBySpell(info, false);
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


void AddSC_war3_hero_spells_script()
{
    new spell_army_teleport();
    new spell_dark_ceremony();
    new spell_dark_ranger_black_bolt();
    new spell_death_contract();
    new spell_palading_rise_ally();
}
