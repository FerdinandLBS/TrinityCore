
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

class spell_sunwell_power : public SpellScriptLoader
{
public:
    spell_sunwell_power() : SpellScriptLoader("spell_sunwell_power") { }

    class spell_sunwell_power_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sunwell_power_SpellScript);

        void HandleAfterCast()
        {
            Player* player = this->GetCaster()->ToPlayer();
            uint32 count = 1;

            if (player == nullptr) {
                return;
            }

            Aura* aura = player->GetAura(87203);
            CastSpellExtraArgs args;
            if (aura)
                count += aura->GetStackAmount() * 2;

            Aura* thisAura = player->GetAura(87204);
            if (thisAura)
                thisAura->SetStackAmount(count);
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(spell_sunwell_power_SpellScript::HandleAfterCast);
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


class spell_draenei_mage : public SpellScriptLoader
{
public:
    spell_draenei_mage() : SpellScriptLoader("spell_draenei_mage") { }

    class spell_draenei_mage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_draenei_mage_SpellScript);

        void HandleAfterCast()
        {
            Player* owner = this->GetCaster()->ToPlayer();
            if (!owner)
                return;

            Aura* aura = owner->GetAura(87217);
            if (!aura)
                return;

            CastSpellExtraArgs args;
            uint32 auraCount = aura->GetStackAmount();

            float intl = owner->GetStat(STAT_INTELLECT);
            args.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT0, intl*auraCount/14);
            owner->CastSpell(GetSpell()->m_targets, 87219, args);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_draenei_mage_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_draenei_mage_SpellScript();
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

void SummonRandomDemon(Unit* caster) {
    int index = rand() % 7;
    int spells[] = {87251, 87254, 87255, 87256, 87264, 87271, 87272};

    if (index == 0) {
        CastSpellExtraArgs args;
        args.AddSpellMod(SpellValueMod::SPELLVALUE_BASE_POINT0, 3);
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

    class spell_gnome_warlock_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gnome_warlock_SpellScript);

        void HandleAfterCast()
        {
            Player* owner = GetCaster()->ToPlayer();
            if (!owner)
                return;

            SummonRandomDemon(owner);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_gnome_warlock_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gnome_warlock_SpellScript();
    }
}; 

class spell_bloodelf_warlock : public SpellScriptLoader
{
public:
    spell_bloodelf_warlock() : SpellScriptLoader("spell_bloodelf_warlock") { }

    class spell_bloodelf_warlock_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bloodelf_warlock_SpellScript);

        int GetCostCount(Unit* caster, uint32* auraStack) {
        }

        SpellCastResult CheckCastHandler() {
            Unit* caster = GetCaster();
            Player* player = caster->ToPlayer();
            uint32 auraStack = 0;
            Aura* aura;

            if (!player)
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

            aura = caster->GetAura(87270);
            uint32 cost;

            if (aura == nullptr) {
                cost = 0;
            }
            else {
                auraStack = aura->GetStackAmount();
                cost = std::pow(2, auraStack - 1);
            }

            uint32 count = player->GetItemCount(6265);
            if (cost > count) {
                return SPELL_FAILED_NEED_MORE_ITEMS;
            }

            if (auraStack == 0) {
                caster->CastSpell(caster, 87270);
            }
            else {
                aura->SetStackAmount(auraStack + 1);
            }

            player->DestroyItemCount(player->GetItemByEntry(6265), cost, true);
            return SPELL_CAST_OK;
        }

        void HandleAfterCast()
        {
            Player* owner = GetCaster()->ToPlayer();
            if (!owner)
                return;

            SummonRandomDemon(owner);
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_bloodelf_warlock_SpellScript::CheckCastHandler);
            AfterCast += SpellCastFn(spell_bloodelf_warlock_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_bloodelf_warlock_SpellScript();
    }
};

void AddSC_race_telants_script()
{
    new spell_gnome_warlock();
    new spell_sunwell_power();
    new spell_draenei_mage();
    new spell_wild_imp_enhance_master();
    new spell_smart_chicken_aura();
    new spell_bloodelf_warlock();
}
