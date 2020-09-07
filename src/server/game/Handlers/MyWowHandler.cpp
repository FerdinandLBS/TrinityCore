/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WorldSession.h"
#include "AccountMgr.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "CharacterPackets.h"
#include "Chat.h"
#include "CinematicMgr.h"
#include "Common.h"
#include "Corpse.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "Group.h"
#include "GuildMgr.h"
#include "Language.h"
#include "Log.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvP.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"
#include "ScriptedGossip.h"
#include "Item.h"
#include "MyWowHandler.h"
#include "botmgr.h"
#include "bot_ai.h"

#pragma execution_character_set("utf-8")

enum eWarIIIHeroSommonSpell {
    WARIII_HERO_SPELL_MK = 86100,
    WARIII_HERO_SPELL_SS = 85959,
    WARIII_HERO_SPELL_WATCHER = 85999,
    WARIII_HERO_SPELL_TINKER = 85961,
    WARIII_HERO_SPELL_MAGE = 85979,
    WARIII_HERO_SPELL_LICH = 85987,
    WARIII_HERO_SPELL_TC = 85998,
    WARIII_HERO_SPELL_DR = 85997,
    WARIII_HERO_SPELL_PALADIN = 85958,
    WARIII_HERO_SPELL_FG = 85940,
    WARIII_HERO_SPELL_BEAST_MASTER = 85926,
    WARIII_HERO_SPELL_DREADLORD = 85969,
    WARIII_HERO_SPELL_DK = 85915,
    WARIII_HERO_SPELL_BLADE_MASTER = 85901,
    WARIII_HERO_SPELL_DEAMON = 85889,
    WARIII_HERO_SPELL_BLOOD_MAGE = 85874,
};

const int WarIIIHeroSummonSpell[] = {
    WARIII_HERO_SPELL_MK,
    WARIII_HERO_SPELL_SS,
    WARIII_HERO_SPELL_WATCHER,
    WARIII_HERO_SPELL_TINKER,
    WARIII_HERO_SPELL_MAGE,
    WARIII_HERO_SPELL_LICH,
    WARIII_HERO_SPELL_TC,
    WARIII_HERO_SPELL_DR,
    WARIII_HERO_SPELL_PALADIN,
    WARIII_HERO_SPELL_FG,
    WARIII_HERO_SPELL_BEAST_MASTER,
    WARIII_HERO_SPELL_DREADLORD,
    WARIII_HERO_SPELL_DK,
    WARIII_HERO_SPELL_BLADE_MASTER,
    WARIII_HERO_SPELL_DEAMON,
    WARIII_HERO_SPELL_BLOOD_MAGE
};

const char* WarIIIHeroName[] = {
    "山丘之王",
    "暗影猎手",
    "守望者",
    "地精修补匠",
    "大法师",
    "巫妖",
    "牛头人酋长",
    "黑暗游侠",
    "圣骑士",
    "丛林守护者",
    "兽王",
    "恐惧魔王",
    "死亡骑士",
    "剑圣",
    "深渊魔王",
    "血魔法师"
};

#define WARIII_HERO_COUNT 16

enum eMyWowSpells {
    MW_SPELL_WARIII_HERO_BUFF = 86001

};

void BuildSpecailTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();

    AddGossipItemFor(player, 4, "黑石塔 55 - 65", GOSSIP_SENDER_INFO, MW_GOSSIP_ACTION_TRANS + 200);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildWarIIIHeroMenu(Player* player, Item* item, uint32 sender)
{
    if (!player || !player->IsAlive())
        return;

    PlayerMenu* menu = player->PlayerTalkClass;
    uint8 level = player->GetLevel();
    menu->ClearMenus();

    for (int i = 0; i < WARIII_HERO_COUNT; i++) {
        std::ostringstream message;
        message << "召唤 【 " << WarIIIHeroName[i] << " 】";
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, message.str().c_str(), MW_GOSSIP_SENDER_SUMMON, GOSSIP_ACTION_INFO_DEF + i);
    }

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_MAIN, MW_GOSSIP_ACTION_SUB_MENU);
    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildTransMenu(Player* player, Item* item, uint32 sender)
{
    if (!player || !player->IsAlive())
        return;

    PlayerMenu* menu = player->PlayerTalkClass;
    uint8 level = player->GetLevel();
    menu->ClearMenus();

    if (player->GetTeam() == ALLIANCE)
    {
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "暴风城", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 1);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "铁炉堡", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 2);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "达纳苏斯", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 3);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "埃索达", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 4);
    }
    else
    {
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "奥格瑞玛", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 5);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "雷霆崖", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 6);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "幽暗城", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 7);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "银月城", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 8);
    }

    if (level > 13)
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "棘齿城", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 9);

    if (level > 35)
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "藏宝海湾", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 10);

    if (level > 39)
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "热沙岗", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 11);

    if (level > 10)
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, "|cffFF8800GM岛|r", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 14);

    if (player->GetLevel() >= 58)
        AddGossipItemFor(player, GOSSIP_ICON_TABARD, "|cffFF0005外域|r", MW_GOSSIP_SENDER_TRANS_OUTLAN, MW_GOSSIP_ACTION_SUB_MENU);

    if (player->GetLevel() >= 69)
        AddGossipItemFor(player, GOSSIP_ICON_TABARD, "|cff0143FF诺森德|r", MW_GOSSIP_SENDER_TRANS_NORTHLAND, MW_GOSSIP_ACTION_SUB_MENU);


    AddGossipItemFor(player, GOSSIP_ICON_TABARD, "|cff05F3255人副本|r", MW_GOSSIP_SENDER_TRANS_FIVE_DUNGEON, MW_GOSSIP_ACTION_SUB_MENU);
    AddGossipItemFor(player, GOSSIP_ICON_TABARD, "|cffFF0BFF团队副本|r", MW_GOSSIP_SENDER_TRANS_RAID, MW_GOSSIP_ACTION_SUB_MENU);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_MAIN, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, GOSSIP_ICON_TAXI, item->GetGUID());
}

#define BOT_SPECS_COUNT 31
const char* SpecStrings[BOT_SPECS_COUNT] =
{
    "武器",
    "狂怒",
    "防护",
    "神圣",
    "防护",
    "惩戒",
    "兽王",
    "射击",
    "生存",
    "刺杀",
    "战斗",
    "敏锐",
    "戒律",
    "神圣",
    "暗影",
    "鲜血",
    "冰霜",
    "邪恶",
    "元素",
    "增强",
    "恢复",
    "奥术",
    "火焰",
    "冰霜",
    "痛苦",
    "恶魔",
    "毁灭",
    "平衡",
    "野性",
    "恢复",
    "未知"
};

const char* classString[] = {
    "战士",
    "圣骑士",
    "猎人",
    "潜行者",
    "牧师",
    "死亡骑士",
    "萨满",
    "法师",
    "术士",
    "10",
    "德鲁伊",

    "剑圣",
    "破坏者",
    "魔法弓箭手",
    "恐惧魔王",
    "破法者",
    "黑暗游侠"
};

void BuildBotManageMenuGroupManage(Player* player, Item* item, uint32 sender) {

    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    BotMgr* mgr = player->GetBotMgr();
    if (!mgr)
        return;

    int i = 1;
    Group* gr = player->GetGroup();

    if (gr) {
        std::unordered_map<ObjectGuid /*guid*/, Creature* /*bot*/>* map = mgr->GetBotMap();
        auto iter = map->begin();
        while (iter != map->end()) {
            if (gr->IsMember(iter->second->GetGUID())) {
                std::ostringstream str;
                bot_ai* ai = (bot_ai*)iter->second->GetAI();

                str << i++ << ". " << iter->second->GetName() << " (" << SpecStrings[ai->GetSpec() - 1] << " " << classString[ai->GetBotClass() - 1] << ")";
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, str.str().c_str(), MW_GOSSIP_SENDER_ACTION_BOT_REMOVE, GOSSIP_ACTION_INFO_DEF + iter->second->GetEntry());
            }
            iter++;
        }
        {
            std::ostringstream str;
            str << "当前队伍人数：" << gr->GetMembersCount();
            player->GetSession()->SendAreaTriggerMessage(str.str().c_str());
        }
    }


    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_BOT_MANAGE, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildBotManageMenuAddBot(Player* player, Item* item, uint32 sender) {
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    BotMgr* mgr = player->GetBotMgr();
    if (!mgr)
        return;

    uint32 i = 1;
    Group* gr = player->GetGroup();
    std::unordered_map<ObjectGuid /*guid*/, Creature* /*bot*/>* map = mgr->GetBotMap();
    auto iter = map->begin();
    while (iter != map->end()) {
        if (!gr || !gr->IsMember(iter->second->GetGUID())) {
            std::ostringstream str;
            bot_ai* ai = (bot_ai*)iter->second->GetAI();

            str << i++ << ". " << iter->second->GetName() << " (" << SpecStrings[ai->GetSpec()-1] << " " << classString[ai->GetBotClass()-1] << ")";

            AddGossipItemFor(player, GOSSIP_ICON_CHAT, str.str().c_str(), MW_GOSSIP_SENDER_ACTION_BOT_JOIN, GOSSIP_ACTION_INFO_DEF + iter->second->GetEntry());
        }
        iter++;
    }

    if (gr) {
        std::ostringstream str;
        str << "当前队伍人数：" << gr->GetMembersCount();
        player->GetSession()->SendAreaTriggerMessage(str.str().c_str());
    }

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_BOT_MANAGE, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void GossipActionBotRemove(Player* player, Item* item, uint32 action) {
    BotMgr* mgr = player->GetBotMgr();
    Group* gr = player->GetGroup();
    if (!gr || !mgr)
        return CloseGossipMenuFor(player);

    std::unordered_map<ObjectGuid /*guid*/, Creature* /*bot*/>* map = mgr->GetBotMap();
    auto iter = map->begin();
    while (iter != map->end()) {
        if (gr->IsMember(iter->second->GetGUID()) &&
            action- GOSSIP_ACTION_INFO_DEF == iter->second->GetEntry()) {

            mgr->RemoveBotFromGroup(iter->second, false);
            return BuildBotManageMenuGroupManage(player, item, 0);
        }
        iter++;
    }

    CloseGossipMenuFor(player);
}

void GossipActionBotJoin(Player* player, Item* item, uint32 action) {
    BotMgr* mgr = player->GetBotMgr();
    if (!mgr)
        return CloseGossipMenuFor(player);

    std::unordered_map<ObjectGuid /*guid*/, Creature* /*bot*/>* map = mgr->GetBotMap();
    auto iter = map->begin();
    while (iter != map->end()) {
        if (action - GOSSIP_ACTION_INFO_DEF == iter->second->GetEntry()) {

            mgr->AddBotToGroup(iter->second);
            return BuildBotManageMenuAddBot(player, item, 0);
        }
        iter++;
    }

    CloseGossipMenuFor(player);
}

void BuildBotManageMenu(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();

    if (player->GetGroup())
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "队伍管理", MW_GOSSIP_SENDER_BOT_REMOVE, MW_GOSSIP_ACTION_SUB_MENU);
    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "队伍扩充", MW_GOSSIP_SENDER_BOT_ADD, MW_GOSSIP_ACTION_SUB_MENU);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_MAIN, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 4, item->GetGUID());
}


void BuildMainMenu(Player* player, Item* item, uint32 sender)
{
    if (!player || !player->IsAlive())
        return;

    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();

    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "|cff05E605传送服务|r", MW_GOSSIP_SENDER_TRANS, MW_GOSSIP_ACTION_SUB_MENU);

    if (player->GetMap()->IsDungeon())
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "|cffCB0BC4魔兽争霸英雄|r", MW_GOSSIP_SENDER_WARIII_HERO, MW_GOSSIP_ACTION_SUB_MENU);

    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cfff5E605雇佣兵管理|r", MW_GOSSIP_SENDER_BOT_MANAGE, MW_GOSSIP_ACTION_SUB_MENU);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "关闭", MW_GOSSIP_SENDER_CLOSE, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, GOSSIP_ICON_TAXI, item->GetGUID());
}

void BuildRaidTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "熔火之心(60)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 301);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "黑翼之巢(80)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 302);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "安其拉(60)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 303);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "祖尔格拉布(60)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 304);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "祖阿曼(70)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 305);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "卡拉赞(70)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 306);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "奥妮克希亚巢穴(80)", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 307);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_TRANS, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 2, item->GetGUID());
}

void BuildNorthlandTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, 13, "达拉然", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 101);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "乌特加德堡垒", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 104);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "魔枢", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 103);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "龙眠神殿", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 102);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "古代王国", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 105);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "达克萨隆要塞", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 111);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "古达克", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 106);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "风暴群山", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 107);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "纳克萨玛斯", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 110);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "银色比武场", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 108);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "冰冠堡垒", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 109);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_TRANS, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildOutlandTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, 13, "沙塔斯", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 201);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "地狱火堡垒", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 202);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "盘牙水库", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 203);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "奥金顿", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 204);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "风暴战舰", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 205);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "戈鲁尔巢穴", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 206);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "黑暗神庙", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 207);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "时光之穴", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 208);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_TRANS, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildDungeonTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "怒焰裂谷 15 - 21", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 51);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "死亡矿井 15 - 21", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 52);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "哀嚎洞穴 15 - 25", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 53);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "影牙城堡 16 - 26", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 54);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "黑暗深渊 20 - 30", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 55);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "暴风城监狱 20 - 30", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 56);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "诺莫瑞根 24 - 34", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 57);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "剃刀沼泽 25 - 30", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 59);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "血色修道院 26 - 40", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 58);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "剃刀高地 34 - 40", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 64);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "奥达曼 35 - 40", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 61);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "祖尔法拉克 43 - 46", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 66);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "玛拉顿 43 - 48", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 60);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "厄运之槌 54 - 58", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 62);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "通灵学院 59 - 61", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 63);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "斯坦索姆 56 - 60", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 65);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "黑石深渊 49 - 57", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 67);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "沉没的神庙 50- 60", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 68);
    AddGossipItemFor(player, GOSSIP_ICON_TAXI, "黑石塔 55 - 65", MW_GOSSIP_SENDER_ACTION_DO_TRANS, MW_GOSSIP_ACTION_TRANS + 69);

    AddGossipItemFor(player, GOSSIP_ICON_TALK, "<<< 后退", MW_GOSSIP_SENDER_TRANS, MW_GOSSIP_ACTION_SUB_MENU);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void GossipActionDoTrans(Player* player, Item* item, uint32 action) {
    switch (action) {
    case MW_GOSSIP_ACTION_TRANS + 1: // StormWind City
        player->TeleportTo(0, -8730.59f, 722.68f, 101.7f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 2: // TLB
        player->TeleportTo(0, -4799.36f, -1107.36f, 501.7f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 3: // DNSS
        player->TeleportTo(1, 9961, 2055, 1329, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 4: // 埃索达
        player->TeleportTo(530, -3998.3f, -11864.1, 1, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 5: // 奥格瑞玛
        player->TeleportTo(1, 1676.25f, -4313.45f, 62.0f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 6: // 雷霆崖
        player->TeleportTo(1, -1150.877197f, 15.459573f, 180.088318f, 1.300810f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 7: // 幽暗城
        player->TeleportTo(0, 1596.05835f, 240.41658f, -13.89129f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 8: // 银月城
        player->TeleportTo(530, 9930.45f, -7129.1f, 48, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 9: // 棘齿城
        player->TeleportTo(1, -977, -3788, 6, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 10:
        player->TeleportTo(0, -14302, 518, 9, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 11:
        player->TeleportTo(1, -7156.56f, -3825.1f, 8.7f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 14: // GM岛 
        player->TeleportTo(1, 16222.1, 16252.1, 12.5872, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 51: // 怒焰裂谷
        player->TeleportTo(389, 3.8, -14.8, -17, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 52: // 矿井
        player->TeleportTo(36, -16, -383, 62, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 53: // 哀嚎
        player->TeleportTo(43, -163.49f, 132.89f, -73.66, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 54: // 影牙
        player->TeleportTo(33, -229.1f, 2109.17f, 77, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 55: // 深渊
        player->TeleportTo(48, -151.88f, 106.95f, -39.3f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 56: // 监狱
        player->TeleportTo(34, 54.2f, 0.28f, -18, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 57: // 诺莫瑞根
        player->TeleportTo(90, -327.5f, -4.7f, -152.3f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 58: // 血色
        player->TeleportTo(0, 2894.34f, -809.55f, 160.33f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 59: // 剃刀沼泽
        player->TeleportTo(47, 1943, 1544.63f, 82, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 60: // 玛拉顿
        player->TeleportTo(349, 1019.69f, -458.3f, -43, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 61: // 奥达曼
        player->TeleportTo(70, -226.8, 49.1f, -45.9f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 62: // 厄运
        player->TeleportTo(429, -201.11f, -328.66f, -2.7f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 63: // 通灵学院
        player->TeleportTo(289, 196.39f, 127, 135, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 64: // 剃刀高地
        player->TeleportTo(129, 2592.55f, 1107.5f, 51.5f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 65: // 斯坦索姆
        player->TeleportTo(329, 3394.13f, -3380.16f, 143.0f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 66: // 祖尔法拉克
        player->TeleportTo(209, 1213.52f, 841.59f, 9, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 67: // 黑石深渊
        player->TeleportTo(230, 458.3f, 26.5f, -70.64f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 68: // 沉没的神庙
        player->TeleportTo(109, -319.23f, 99.9f, -131.85f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 69: // 黑石塔
        player->TeleportTo(229, 78.5f, -225.0f, 50.0f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 101: // 达拉燃
        player->TeleportTo(571, 5797, 795, 664, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 102: // 龙眠神殿
        player->TeleportTo(571, 3546.607178f, 273.218842f, 342.722f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 103: // 魔枢
        player->TeleportTo(571, 3831.737061f, 6960.383789f, 104.784271f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 104: // 乌特加德堡垒
        player->TeleportTo(571, 1260.176636f, -4843.805664f, 215.763993f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 105: // 古代王国
        player->TeleportTo(571, 3695.932129f, 2143.285889f, 34.147270f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 106: // 古达克
        player->TeleportTo(571, 6938.497559f, -4452.765137f, 450.868896f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 107: // 风暴群山
        player->TeleportTo(571, 8949.208008f, -1266.415894f, 1025.499391f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 108: // 银色比武场
        player->TeleportTo(571, 8486.941406f, 775.859863f, 558.568299f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 109: // 冰冠堡垒
        player->TeleportTo(571, 5864.67f, 2169.83f, 636.1f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 110: // 纳克萨玛斯
        player->TeleportTo(571, 3666.089844f, -1269.738403f, 243.508927f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 111: // 达克萨隆要塞
        player->TeleportTo(571, 4772.635742f, -2046.703125f, 238.28464f, 0.061439f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 201: // 沙塔斯
        player->TeleportTo(530, -1859.95f, 5438.85f, -10.3f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 202: // 地狱火
        player->TeleportTo(530, -321.64f, 3082.49f, 32.6f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 203: // 水库
        player->TeleportTo(530, 764.034058f, 6866.363770f, -68.277512f, 6.266417f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 204: // 奥金顿
        player->TeleportTo(530, -3377.06f, 4954.24f, -66.5f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 205: // 风暴战舰
        player->TeleportTo(530, 3101.006592f, 1537.525879f, 190.31f, 4.649131f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 206: // 格鲁尔
        player->TeleportTo(565, 62.784199f, 35.462002f, -3.983500f, 1.418440f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 207: // 黑庙
        player->TeleportTo(564, 96.45f, 1002.35f, -86.8f, 6);
        break;
    case MW_GOSSIP_ACTION_TRANS + 208: // 时光之穴
        player->TeleportTo(1, -8509.349606f, -4356.310059f, -208.358994f, 6);
        break;
        // Raid telepot
    case MW_GOSSIP_ACTION_TRANS + 301: // 熔火之心
        player->TeleportTo(409, 1087.588f, -477.341f, -107.0f, 0.786652f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 305: // 祖阿曼
        player->TeleportTo(530, 6832.783203f, -7858.009766f, 163.976166f, 4.697f);
        break;
    case MW_GOSSIP_ACTION_TRANS + 307: // 奥妮克西娅
        player->TeleportTo(1, -4745.300293f, -3753.068604f, 50.219667f, 4.697f);
        break;
        /*
        case MW_GOSSIP_ACTION_TRANS + 64: //
        player->TeleportTo(, f, f, f, 6);
        break;*/
    }
    CloseGossipMenuFor(player);
}

void GossipActionDoSummon(Player* player, Item* item, uint32 action) {
    int canSummon = 1;
    if (!player || player->isDead())
        return;

    uint32 heroCount = 0;
    const Map* map = player->GetMap();
    if (!map) return;

    if (map->IsRaid()) {
        canSummon = 5;
    } else if (map->GetDifficulty() == Difficulty::DUNGEON_DIFFICULTY_HEROIC) {
        canSummon = 2;
    }

    for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end();)
    {
        Unit* unit = *itr;
        ++itr;
        if (unit->GetEntry() >= 45000 && unit->GetEntry() < 46000)
            heroCount++;
    }

    if (heroCount >= canSummon) {
        std::ostringstream message;
        message << "你已经召唤了太多英雄." << "当前最多召唤" << canSummon << "名英雄";
        player->GetSession()->SendAreaTriggerMessage(message.str().c_str());
        CloseGossipMenuFor(player);
        return;
    }
    int spellIndex = action - GOSSIP_ACTION_INFO_DEF;
    if (spellIndex < 0 || spellIndex >= WARIII_HERO_COUNT) {
        TC_LOG_ERROR("MyWow", "Invalid summon hero index: %d", spellIndex);
        return;
    }

    int spellId = WarIIIHeroSummonSpell[action - GOSSIP_ACTION_INFO_DEF];
    const SpellInfo* spi = sSpellMgr->GetSpellInfo(spellId);
    if (!spi) {
        TC_LOG_ERROR("MyWow", "Summon hero spell not found: Index=%d, Spell=%d", spellIndex, spellId);
        return;
    }

    std::list<Creature*> cList;
    player->GetAllMinionsByEntry(cList, spi->Effects[0].MiscValue);
    if (cList.size() > 0) {
        std::ostringstream message;
        message << "你已经召唤了" << WarIIIHeroName[spellIndex];
        player->GetSession()->SendAreaTriggerMessage(message.str().c_str());
        return BuildWarIIIHeroMenu(player, item, 0);
    }

    player->CastSpell(player, spellId);
    if (heroCount + 1 >= canSummon) {
        CloseGossipMenuFor(player);
    }
    else {
        return BuildWarIIIHeroMenu(player, item, 0);
    }
}

void GossipSelect_Item(Player* player, Item* item, uint32 sender, uint32 action)
{
    if (!player || player->isDead())
        return;
    if (!item)
        return;

    switch (sender) {
    case MW_GOSSIP_SENDER_MAIN:
        return BuildMainMenu(player, item, sender);
    case MW_GOSSIP_SENDER_TRANS:
        return BuildTransMenu(player, item, sender);
    case MW_GOSSIP_SENDER_TRANS_OUTLAN:
        return BuildOutlandTeleport(player, item, sender);
    case MW_GOSSIP_SENDER_TRANS_NORTHLAND:
        return BuildNorthlandTeleport(player, item, sender);
    case MW_GOSSIP_SENDER_TRANS_FIVE_DUNGEON:
        return BuildDungeonTeleport(player, item, sender);
    case MW_GOSSIP_SENDER_TRANS_RAID:
        return BuildRaidTeleport(player, item, sender);
    case MW_GOSSIP_SENDER_WARIII_HERO:
        return BuildWarIIIHeroMenu(player, item, sender);
    case MW_GOSSIP_SENDER_TRADE:
        break;
    case MW_GOSSIP_SENDER_SUMMON:
        return GossipActionDoSummon(player, item, action);
    case MW_GOSSIP_SENDER_BOT_MANAGE:
        return BuildBotManageMenu(player, item, sender);
    case MW_GOSSIP_SENDER_BOT_ADD:
        return BuildBotManageMenuAddBot(player, item, sender);
    case MW_GOSSIP_SENDER_BOT_REMOVE:
        return BuildBotManageMenuGroupManage(player, item, sender);
    case MW_GOSSIP_SENDER_ACTION_DO_TRANS:
        return GossipActionDoTrans(player, item, action);
    case MW_GOSSIP_SENDER_ACTION_BOT_JOIN:
        return GossipActionBotJoin(player, item, action);
    case MW_GOSSIP_SENDER_ACTION_BOT_REMOVE:
        return GossipActionBotRemove(player, item, action); 
    case MW_GOSSIP_SENDER_CLOSE:
        return CloseGossipMenuFor(player);
    }
}
