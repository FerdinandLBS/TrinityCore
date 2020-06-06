
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
#include "DynamicObject.h"

class spell_item_wlock_chaos_rain : public SpellScriptLoader
{
public:
    spell_item_wlock_chaos_rain() : SpellScriptLoader("spell_item_wlock_chaos_rain") { }


    class spell_item_wlock_chaos_rain_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_wlock_chaos_rain_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            return true;
        }

        void HandleProc(AuraEffect* aurEff)
        {
            if (rand() % 100 > 92) {
                GetCaster()->CastSpell(GetDynobjOwner()->GetPosition(), 85854, true);
            }
        }

        void Register() override
        {
            OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_item_wlock_chaos_rain_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_wlock_chaos_rain_AuraScript();
    }
};

void AddSC_legend_items_script()
{
    new spell_item_wlock_chaos_rain();
}
