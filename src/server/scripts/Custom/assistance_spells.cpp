
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

#include "bot_ai.h"
#include "botmgr.h"
#include <unordered_map>
#include <map>

#pragma execution_character_set("utf-8")

class YellEvent : public BasicEvent
{
public:
    YellEvent(Unit* who, const std::string txt) : _who(who) ,_txt(txt) { }

protected:
    bool Execute(uint64 /*e_time*/, uint32 /*p_time*/)
    {
        if (!_who->IsAlive()) {
            return true;
        }

        _who->Yell(_txt, Language::LANG_UNIVERSAL);

        return true;
    }

private:
    Unit* _who;
    const std::string _txt;
    YellEvent(YellEvent const&);
};

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
                return "再见";
            case 1:
                return "你自己保重";
            case 2:
                return "我要去休息一下了";
            case 3:
                return "....";
            default:
                return "后会有期";
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
                player->GetSession()->SendAreaTriggerMessage("必须选择一个魔兽英雄");
                return;
            }

            if (target->GetOwner() != player) {
                player->GetSession()->SendAreaTriggerMessage("必须选择自己的魔兽英雄");
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

const char* getFinalSkillCooldownReport(Unit* u) {
    switch (u->GetEntry()) {
    case 45001: // Warden
        return "复仇之力再度觉醒了...";
    case 45002: // Shadow Shaman
        return "巫毒术准备好了";
    case 45003: // Mage
        return "暴风城的部队已经集结完毕";
    case 45005: // Lich
        return "尘归尘！土归土！";
    case 45007: // Paladin
        return "圣光，请再次眷顾我";
    case 45008: // Dradlord
        return "虚空中的力量回来了";
    case 45010: // Forest Guard
        return "宁静已经准备好了";
    case 45013: // Blade Master
        return "我的剑刃饥渴难耐";
    case 45014: // Pitlord
        return "让他们遭受折磨";
    case 45016: // Blood Mage
        return "复仇之炎，燃烧吧！";
    default:
        return "";
    }

}

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
            uint32 spId;

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
            case 45002: // Shadow Shaman
                result = target->CastSpell(target, 85918);
                spId = 85918;
                if (result == SpellCastResult::SPELL_CAST_OK) {
                    target->HandleEmoteCommand(EMOTE_STATE_DANCE);
                }
                break;
            case 45005: // Lich
                if (victim)
                    result = target->CastSpell(victim->GetPosition(), 85942);
                else
                    result = target->CastSpell(target->GetPosition(), 85942);
                spId = 85942;
                break;
            case 45007: // Paladin
                result = target->CastSpell(target, 85943);
                spId = 85943;
                break;
            case 45010: // Forest Guard
                result = target->CastSpell(target, 85934);
                spId = 85934;
                break;
            case 45013: // Blade Master
                result = target->CastSpell(target, 85897);
                spId = 85897;
                break;
            default:
                return result;
            }

            if (result == SPELL_CAST_OK) {
                const SpellInfo* si = sSpellMgr->GetSpellInfo(spId);
                if (si) {
                    target->m_Events.AddEvent(new YellEvent(target, getFinalSkillCooldownReport(target)), target->m_Events.CalculateTime(Milliseconds(si->GetRecoveryTime())));
                }
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

            for (Unit::ControlList::iterator itr = caster->m_Controlled.begin(); itr != caster->m_Controlled.end();)
            {
                Unit* unit = *itr;
                ++itr;
                if (unit->GetEntry() >= 45000 && unit->GetEntry() < 46000)
                    ((AssistanceAI*)unit->GetAI())->ResetPosition();
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

class spell_bot_auto_format : public SpellScriptLoader
{
public:
    spell_bot_auto_format() : SpellScriptLoader("spell_bot_auto_format") { }

    class spell_bot_auto_format_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bot_auto_format_SpellScript);

        void HandleAfterCast()
        {
            Player* owner = this->GetCaster()->ToPlayer();
            if (!owner || !owner->GetBotMgr())
                return;

            Unit* target = owner->GetSelectedUnit();
            if (!target || target->GetEntry() < 70000) {
                std::unordered_map<ObjectGuid /*guid*/, Creature* /*bot*/>* map = owner->GetBotMgr()->GetBotMap();
                auto iter = map->begin();
                while (iter != map->end()) {
                    ((bot_ai*)iter->second->GetAI())->SetBotCommandState(BOT_COMMAND_FOLLOW);

                    ++iter;
                }
            }
            else {
                if (owner->GetBotMgr()->GetBot(target->GetGUID()))
                    ((bot_ai*)target->GetAI())->SetBotCommandState(BOT_COMMAND_FOLLOW);
                else
                    target->Whisper("你不是我的雇主", Language::LANG_UNIVERSAL, owner, false);
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_bot_auto_format_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_bot_auto_format_SpellScript();
    }
};

class spell_bot_hold_format : public SpellScriptLoader
{
public:
    spell_bot_hold_format() : SpellScriptLoader("spell_bot_hold_format") { }

    class spell_bot_hold_format_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bot_hold_format_SpellScript);

        SpellCastResult checkCastHandler() {
            Player* owner = this->GetCaster()->ToPlayer();
            if (!owner)
                return SPELL_FAILED_BAD_TARGETS;


            Unit* target = owner->GetSelectedUnit();
            if (!target || target->GetEntry() < 70000) {
                return SPELL_FAILED_BAD_TARGETS;
            }

            return SPELL_CAST_OK;
        }

        void HandleAfterCast()
        {
            Player* owner = this->GetCaster()->ToPlayer();
            if (!owner)
                return;


            Unit* target = owner->GetSelectedUnit();
            if (!target) {
                return;
            }
            else {
                Spell* sp = GetSpell();
                if (!sp)
                    return;


                if (target->GetEntry() >= 70000) {
                    Position p;

                    p = sp->m_targets.GetDstPos()->GetPosition();
                    p.m_positionY += 1.0f;
                    ((bot_ai*)target->GetAI())->SetBotCommandState(BOT_COMMAND_STAY);
                    target->GetMotionMaster()->MovePoint(owner->GetMapId(), p);
                }
            }
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_bot_hold_format_SpellScript::checkCastHandler);
            AfterCast += SpellCastFn(spell_bot_hold_format_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_bot_hold_format_SpellScript();
    }
};


class spell_bot_surroud_format : public SpellScriptLoader
{
public:
    spell_bot_surroud_format() : SpellScriptLoader("spell_bot_surroud_format") { }

    class spell_bot_surroud_format_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bot_surroud_format_SpellScript);

        void HandleAfterCast()
        {
            float followDis = 0.72f;
            float angle = 0;
            float stepAngle;
            int i = 0;
            Player* owner = this->GetCaster()->ToPlayer();
            if (!owner || !owner->GetBotMgr())
                return;

            Unit* target = owner->GetSelectedUnit();
            if (!target) {
                followDis = 0.f;
                target = owner;
            }
            else {
                if (!target->IsHostileTo(owner)) {
                    followDis = 0.3f;
                }
            }

            stepAngle = 2* M_PI / owner->GetNpcBotsCount();
            std::unordered_map<ObjectGuid /*guid*/, Creature* /*bot*/>* map = owner->GetBotMgr()->GetBotMap();
            auto iter = map->begin();
            while (iter != map->end())
            {
                Position p;

                if (target == owner) {
                    if (i < 4) {
                        stepAngle = M_PI / 2;
                        followDis = 4.f;
                    }
                    else if (i < 12) {
                        stepAngle = M_PI / 4;
                        followDis = 8.f;
                    }
                    else if (i < 28) {
                        stepAngle = M_PI / 8;
                        followDis = 12.f;
                    }
                    else {
                        stepAngle = M_PI / 16;
                        followDis = 16.f;
                    }
                    if (i == 4 || i == 12 || i == 28)
                        angle = 0;
                    iter->second->GetMotionMaster()->MovePoint(owner->GetMapId(), target->GetNearPosition(followDis, angle));
                    angle += stepAngle;
                }
                else {
                    iter->second->GetMotionMaster()->MovePoint(owner->GetMapId(), target->GetNearPosition(followDis * ((bot_ai*)iter->second->GetAI())->GetAttackRange(), angle));
                    angle += stepAngle;
                }
                ((bot_ai*)iter->second->GetAI())->SetBotCommandState(BOT_COMMAND_STAY);
                i++;
                ++iter;
            }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_bot_surroud_format_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_bot_surroud_format_SpellScript();
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

    new spell_bot_auto_format();
    new spell_bot_hold_format();
    new spell_bot_surroud_format();
}
