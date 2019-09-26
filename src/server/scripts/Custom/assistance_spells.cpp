
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

class spell_assistance_dismiss : public SpellScriptLoader
{
public:
    spell_assistance_dismiss() : SpellScriptLoader("spell_assistance_dismiss") { }

    class spell_assistance_dismiss_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_assistance_dismiss_SpellScript);

        char* GetRandomBye() {
            switch (irand(0, 3)) {
            case 0:
                return "??~";
            case 1:
                return "??~";
            case 2:
                return "???";
            default:
                return "ByeBye";
            }
        }

        void Dismiss(Creature* p, Player* owner) {
            if (p) {
                p->SetFacingToObject(owner);
                p->Yell(GetRandomBye(), Language::LANG_COMMON, owner);
                p->DespawnOrUnsummon(1500);
                AssistanceAI* ai = (AssistanceAI*)p->GetAI();
                owner->_popAssistance(p);
                ai->AIFlag = AssistanceAI::AI_ACTION_FLAG::AI_ACTION_PASSIVE;
            }
        }

        void HandleAfterCast()
        {
            Player* owner = (Player*)this->GetCaster();
            Unit* target = owner->GetSelectedUnit();

            if (owner->_isAssistance((Creature*)target) == true) {
                Dismiss((Creature*)target, owner);
                return;
            }

            for (int i = 0; i < 5; i++) {
                if (owner->m_assistances[i] != nullptr) {
                    Dismiss(owner->m_assistances[i], owner);
                }
                else {
                    return;
                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_assistance_dismiss_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_assistance_dismiss_SpellScript();
    }
};

class spell_assistance_attack_as_will : public SpellScriptLoader
{
public:
    spell_assistance_attack_as_will() : SpellScriptLoader("spell_assistance_attack_as_will") { }

    class spell_assistance_attack_as_will_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_assistance_attack_as_will_SpellScript);


        void SetFlag(Creature* target) {
            AssistanceAI* ai = (AssistanceAI*)target->GetAI();
            ai->AIFlag = AssistanceAI::AI_ACTION_FLAG::AI_ACTION_NONE;
        }

        void HandleAfterCast()
        {
            Player* owner = (Player*)this->GetCaster();
            Unit* target = owner->GetSelectedUnit();

            if (owner->_isAssistance((Creature*)target) == true) {
                SetFlag((Creature*)target);
                return;
            }

            for (int i = 0; i < 5; i++) {
                if (owner->m_assistances[i] != nullptr) {
                    SetFlag((Creature*)owner->m_assistances[i]);
                }
                else {
                    return;
                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_assistance_attack_as_will_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_assistance_attack_as_will_SpellScript();
    }
};

class spell_assistance_close_to_me : public SpellScriptLoader
{
public:
    spell_assistance_close_to_me() : SpellScriptLoader("spell_assistance_close_to_me") { }

    class spell_assistance_close_to_me_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_assistance_close_to_me_SpellScript);

        void SetFollow(Creature* target, Player* owner) {
            if (target == nullptr)
                return;

            //target->Say("", ChatMsg::CHAT_MSG_MONSTER_SAY, Language::LANG_COMMON);
            target->GetMotionMaster()->Clear();
            target->GetMotionMaster()->MoveFollow(owner, 1.0f, target->GetFollowAngle());
            AssistanceAI* ai = (AssistanceAI*)target->GetAI();
            ai->AIFlag = AssistanceAI::AI_ACTION_FLAG::AI_ACTION_PASSIVE;
            target->AttackStop();
        }

        void HandleAfterCast()
        {
            Player* owner = (Player*)this->GetCaster();
            Unit* target = owner->GetSelectedUnit();

            if (owner->_isAssistance((Creature*)target) == true) {
                SetFollow((Creature*)target, owner);
                return;
            }

            for (int i = 0; i < 5; i++) {
                if (owner->m_assistances[i] != nullptr) {
                    SetFollow((Creature*)owner->m_assistances[i], owner);
                }
                else {
                    return;
                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_assistance_close_to_me_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_assistance_close_to_me_SpellScript();
    }
};


void AddSC_assistance_spells_script()
{
    new spell_assistance_close_to_me();
    new spell_assistance_dismiss();
    new spell_assistance_attack_as_will();
}
