#include "bot_ai.h"
#include "botcommon.h"
#include "botdatamgr.h"
#include "botmgr.h"
#include "Creature.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
/*
NPCbot giver NPC by Trickerer (<https://github.com/Trickerer/> <onlysuffering@gmail.com>)
Complete - ???
*/

#pragma execution_character_set("utf-8")

#define HIRE GOSSIP_SENDER_BOTGIVER_HIRE
#define HIRE_CLASS GOSSIP_SENDER_BOTGIVER_HIRE_CLASS
#define HIRE_ENTRY GOSSIP_SENDER_BOTGIVER_HIRE_ENTRY

typedef std::set<Creature const*> NpcBotRegistry;
extern NpcBotRegistry _existingBots;

class script_bot_giver : public CreatureScript
{
public:
    script_bot_giver() : CreatureScript("script_bot_giver") { }

    struct bot_giver_AI : public CreatureAI
    {
        bot_giver_AI(Creature* creature) : CreatureAI(creature) {}

        void UpdateAI(uint32 /*diff*/) {}

        bool GossipHello(Player* player) override
        {
            if (!BotMgr::IsNpcBotModEnabled())
            {
                player->PlayerTalkClass->SendCloseGossip();
                return true;
            }

            if (me->isMoving())
                me->BotStopMovement();

            AddGossipItemFor(player, GOSSIP_ICON_TALK, "我需要你的服务", HIRE, GOSSIP_ACTION_INFO_DEF + 1);

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "算了", 0, GOSSIP_ACTION_INFO_DEF + 2);

            player->PlayerTalkClass->SendGossipMenu(GOSSIP_BOTGIVER_GREET, me->GetGUID());
            return true;
        }

        bool GossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            if (!BotMgr::IsNpcBotModEnabled())
            {
                player->PlayerTalkClass->SendCloseGossip();
                return true;
            }

            uint32 sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);

            player->PlayerTalkClass->ClearMenus();
            bool subMenu = false;

            uint32 gossipTextId = GOSSIP_BOTGIVER_GREET;

            switch (sender)
            {
            case 0: //exit
                break;
            case 1: //BACK: return to main menu
                return GossipHello(player);
            case HIRE:
            {
                gossipTextId = GOSSIP_BOTGIVER_HIRE;

                if (player->GetNpcBotsCount() >= BotMgr::GetMaxNpcBots())
                {
                    WhisperTo(player, "你已经雇佣了太多佣兵");
                    break;
                }

                subMenu = true;

                uint8 availCount = 0;

                for (uint8 botclass = BOT_CLASS_WARRIOR; botclass < BOT_CLASS_END; ++botclass)
                {
                    if (!BotMgr::IsClassEnabled(botclass))
                        continue;

                    if (player->HaveBot() && BotMgr::GetMaxClassBots())
                    {
                        uint8 count = 0;
                        BotMap const* map = player->GetBotMgr()->GetBotMap();
                        for (BotMap::const_iterator itr = map->begin(); itr != map->end(); ++itr)
                            if (itr->second->GetBotClass() == botclass)
                                ++count;
                        if (count >= BotMgr::GetMaxClassBots())
                            continue;
                    }

                    std::ostringstream bclass;
                    bool validClass = true;
                    switch (botclass)
                    {
                    case BOT_CLASS_WARRIOR:         bclass << "战士";       break;
                    case BOT_CLASS_PALADIN:         bclass << "圣骑士";       break;
                    case BOT_CLASS_MAGE:            bclass << "法师";          break;
                    case BOT_CLASS_PRIEST:          bclass << "牧师";        break;
                    case BOT_CLASS_WARLOCK:         bclass << "术士";       break;
                    case BOT_CLASS_DRUID:           bclass << "德鲁伊";         break;
                    case BOT_CLASS_DEATH_KNIGHT:    bclass << "死亡骑士";  break;
                    case BOT_CLASS_ROGUE:           bclass << "潜行者";         break;
                    case BOT_CLASS_SHAMAN:          bclass << "萨满";        break;
                    case BOT_CLASS_HUNTER:          bclass << "猎人";        break;
                    case BOT_CLASS_BM:              bclass << "剑圣";   break;
                    case BOT_CLASS_SPHYNX:          bclass << "破坏者";     break;
                    case BOT_CLASS_ARCHMAGE:        bclass << "魔法弓箭手";       break;
                    case BOT_CLASS_DREADLORD:       bclass << "恐惧魔王";     break;
                    case BOT_CLASS_SPELLBREAKER:    bclass << "破法者"; break;
                    case BOT_CLASS_DARK_RANGER:     bclass << "黑暗游侠";   break;
                    default:                        validClass = false;         break;
                    }

                    if (validClass == false)
                        continue;

                    bclass << " (" << BotMgr::GetNpcBotCostStr(player->GetLevel(), botclass) << ")";

                    AddGossipItemFor(player, GOSSIP_ICON_TALK, bclass.str().c_str(), HIRE_CLASS, GOSSIP_ACTION_INFO_DEF + botclass);

                    if (++availCount >= BOT_GOSSIP_MAX_ITEMS - 1) //back
                        break;
                }

                if (availCount == 0)
                    gossipTextId = GOSSIP_BOTGIVER_HIRE_EMPTY;

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "算了", 0, GOSSIP_ACTION_INFO_DEF + 1);

                break;
            }
            case HIRE_CLASS:
            {
                gossipTextId = GOSSIP_BOTGIVER_HIRE_CLASS;

                uint8 botclass = action - GOSSIP_ACTION_INFO_DEF;

                uint32 cost = BotMgr::GetNpcBotCost(player->GetLevel(), botclass);
                if (!player->HasEnoughMoney(cost))
                {
                    WhisperTo(player, "钱不够!");
                    break;
                }

                subMenu = true;

                uint8 availCount = 0;

                //go through bots map to find what bots are available
                NpcBotRegistry const& allBots = _existingBots;
                for (NpcBotRegistry::const_iterator ci = allBots.begin(); ci != allBots.end(); ++ci)
                {
                    Creature const* bot = *ci;
                    bot_ai const* ai = bot->GetBotAI();
                    if (bot->GetBotClass() != botclass || !bot->IsAlive() || ai->IsTempBot() || ai->GetBotOwnerGuid() || bot->HasAura(BERSERK))
                        continue;

                    std::ostringstream message1;
                    message1 << "要雇佣" << bot->GetName() << "吗?";
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(-1, GOSSIP_ICON_TALK, bot->GetName(),
                        HIRE_ENTRY, GOSSIP_ACTION_INFO_DEF + bot->GetEntry(), message1.str(), cost, false);

                    if (++availCount >= BOT_GOSSIP_MAX_ITEMS - 1) //back
                        break;
                }

                if (availCount == 0)
                    gossipTextId = GOSSIP_BOTGIVER_HIRE_EMPTY;

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "<后退>", HIRE, GOSSIP_ACTION_INFO_DEF + 1);

                break;
            }
            case HIRE_ENTRY:
            {
                uint32 entry = action - GOSSIP_ACTION_INFO_DEF;
                Creature const* bot = BotDataMgr::FindBotInWorld(entry);
                if (!bot)
                {
                    //possible but still
                    TC_LOG_ERROR("entities.unit", "HIRE_NBOT_ENTRY: bot %u not found!", entry);
                    break;
                }

                bot_ai const* ai = bot->GetBotAI();
                if (bot->IsInCombat() || !bot->IsAlive() || bot_ai::CCed(bot) || ai->IsDuringTeleport() ||
                    bot->HasUnitState(UNIT_STATE_CASTING) || ai->GetBotOwnerGuid() || bot->HasAura(BERSERK))
                {
                    //TC_LOG_ERROR("entities.unit", "HIRE_NBOT_ENTRY: bot %u (%s) is unavailable all of the sudden!", entry);
                    std::ostringstream failMsg;
                    failMsg << bot->GetName() << "有点忙, 过会再试试.";
                    WhisperTo(player, failMsg.str().c_str());
                    break;
                }

                //laways returns true
                bot->GetBotAI()->OnGossipSelect(player, me, GOSSIP_SENDER_HIRE, GOSSIP_ACTION_INFO_DEF);

                if (player->HaveBot() && player->GetBotMgr()->GetBot(bot->GetGUID()))
                    WhisperTo(player, "合作愉快");

                break;
            }
            }

            if (subMenu)
                player->PlayerTalkClass->SendGossipMenu(gossipTextId, me->GetGUID());
            else
                player->PlayerTalkClass->SendCloseGossip();

            return true;
        }

        void WhisperTo(Player* player, char const* message)
        {
            me->Whisper(message, LANG_UNIVERSAL, player);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new bot_giver_AI(creature);
    }
};

void AddSC_script_bot_giver()
{
    new script_bot_giver();
}
