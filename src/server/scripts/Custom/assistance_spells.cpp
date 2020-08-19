
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

#pragma execution_character_set("utf-8")

class spell_assistance_dismiss : public SpellScriptLoader
{
public:
    spell_assistance_dismiss() : SpellScriptLoader("spell_assistance_dismiss") { }

    class spell_assistance_dismiss_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_assistance_dismiss_SpellScript);

        const char* GetRandomBye() {
            switch (irand(0, 3)) {
            case 0:
                return "??";
            case 1:
                return "?????";
            case 2:
                return "????????";
            case 3:
                return "....";
            default:
                return "????";
            }
        }

        void Dismiss(Creature* p, Player* owner) {
            if (!p)
                return;

            p->CastStop();
            p->StopMoving();
            p->GetMotionMaster()->Clear();
            if (p->IsAlive()) {
                p->CastSpell(p, 62940);
                p->SetFacingToObject(owner);
                p->Say(GetRandomBye(), Language::LANG_UNIVERSAL, owner);
                p->DespawnOrUnsummon(Milliseconds(1050));
            }
            else {
                p->DespawnOrUnsummon(Milliseconds(0));
            }
            AssistanceAI* ai = (AssistanceAI*)p->GetAI();
            ai->AIFlag = AssistanceAI::AI_ACTION_FLAG::AI_ACTION_PASSIVE;
        }

        void HandleAfterCast()
        {
            Player* player = this->GetCaster()->ToPlayer();
            Unit* target = player->GetSelectedUnit();

            if (player == nullptr) {
                return;
            }

            if (target == nullptr || target->GetEntry() >= 46000 || target->GetEntry() < 45000) {
                player->GetSession()->SendAreaTriggerMessage("??????????");
                return;
            }

            if (target->GetOwner() != player) {
                player->GetSession()->SendAreaTriggerMessage("???????????");
                return;
            }

            Dismiss((Creature*)target, player);
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


class spell_assistance_final_skill : public SpellScriptLoader
{
public:
    spell_assistance_final_skill() : SpellScriptLoader("spell_assistance_final_skill") { }

    class spell_assistance_final_skill_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_assistance_final_skill_SpellScript);

        SpellCastResult HandleCheckCast() {
            Spell* spell = GetSpell();
            Player* player = this->GetCaster()->ToPlayer();
            SpellCastResult result = SpellCastResult::SPELL_FAILED_BAD_TARGETS;
            Unit* target = spell->m_targets.GetObjectTarget()->ToUnit();
            Unit* victim;

            if (!player || !target || player != target->GetOwner()) {
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
            }
            Creature* t = target->ToCreature();
            if (!t) {
                return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
            }

            victim = target->GetVictim();
            if (!victim) {
                victim = ObjectAccessor::GetUnit(*target, target->GetTarget());
            }

            switch (target->GetEntry()) {
            case 45001: // Warden
                result = target->CastSpell(target, 85893);
                break;
            case 45002: // Shadow Shaman
                result = target->CastSpell(target, 85918);
                if (result == SpellCastResult::SPELL_CAST_OK) {
                    target->HandleEmoteCommand(EMOTE_STATE_DANCE);
                }
                break;
            case 45003: // Mage
                result = target->CastSpell(target, 85908);
                break;
            case 45005: // Lich
                if (victim)
                    result = target->CastSpell(victim->GetPosition(), 85942);
                else
                    result = target->CastSpell(target->GetPosition(), 85942);
                break;
            case 45007: // Paladin
                result = target->CastSpell(target, 85943);
                break;
            case 45008: // Dradlord
                if (victim)
                    result = target->CastSpell(victim->GetPosition(), 85854);
                else
                    result = target->CastSpell(target->GetPosition(), 85854);
                break;
            case 45010: // Forest Guard
                result = target->CastSpell(target, 85934);
                break;
            case 45013: // Blade Master
                result = target->CastSpell(target, 85897);
                break;
            case 45014: // Pitlord
                result = target->CastSpell(victim, 85883);
                break;
            case 45016: // Blood Mage
                result = target->CastSpell(target, 85869);
                break;
            default:
                ;
            }

            return result;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_assistance_final_skill_SpellScript::HandleCheckCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_assistance_final_skill_SpellScript();
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

class spell_assistance_teleport_to_me : public SpellScriptLoader
{
public:
    spell_assistance_teleport_to_me() : SpellScriptLoader("spell_assistance_teleport_to_me") { }

    class spell_assistance_teleport_to_me_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_assistance_teleport_to_me_SpellScript);

        void HandleAfterCast()
        {
            Player* caster = GetCaster()->ToPlayer();

            if (caster == nullptr) {
                return;
            }

            int spells[5] = { 86001 ,85980,85990,85970 , 85895};

            for (int i = 0; i < 5; i++) {
                Aura* arua = caster->GetAura(spells[i]);
                if (arua == nullptr)
                    continue;

                Unit* assistance = arua->GetCaster();
                if (assistance && assistance->IsAlive() && assistance->GetOwner() == caster) {
                    assistance->NearTeleportTo(caster->GetPosition());
                }
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_assistance_teleport_to_me_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_assistance_teleport_to_me_SpellScript();
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
    new spell_assistance_teleport_to_me();
    new spell_assistance_final_skill();
}
