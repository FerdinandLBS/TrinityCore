﻿/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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
#include "Chat.h"
#include "CinematicMgr.h"
#include "Common.h"
#include "Corpse.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "Group.h"
#include "GuildMgr.h"
#include "Language.h"
#include "Log.h"
#include "MapManager.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvP.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "WhoListStorage.h"
#include "World.h"
#include "WorldPacket.h"
#include <cstdarg>
#include <zlib.h>
#include "Item.h"
#include "ScriptedGossip.h"

#include <Windows.h>

#include <iostream>
#include <string>

#pragma execution_character_set("utf-8")


void WorldSession::HandleRepopRequestOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_REPOP_REQUEST Message");

    recvData.read_skip<uint8>();

    if (GetPlayer()->IsAlive() || GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    if (GetPlayer()->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        return; // silently return, client should display the error by itself

    // the world update order is sessions, players, creatures
    // the netcode runs in parallel with all of these
    // creatures can kill players
    // so if the server is lagging enough the player can
    // release spirit after he's killed but before he is updated
    if (GetPlayer()->getDeathState() == JUST_DIED)
    {
        TC_LOG_DEBUG("network", "HandleRepopRequestOpcode: got request after player %s(%d) was killed and before he was updated",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        GetPlayer()->KillPlayer();
    }

    //this is spirit release confirm?
    GetPlayer()->RemoveGhoul();
    GetPlayer()->RemovePet(nullptr, PET_SAVE_NOT_IN_SLOT, true);
    GetPlayer()->BuildPlayerRepop();
    GetPlayer()->RepopAtGraveyard();
}

void BuildSpecailTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();

    AddGossipItemFor(player, 4, "黑石塔 55 - 65", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 200);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildRaidTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, 4, "熔火之心(60)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 301);
    AddGossipItemFor(player, 4, "黑翼之巢(80)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 302);
    AddGossipItemFor(player, 4, "安其拉(60)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 303);
    AddGossipItemFor(player, 4, "祖尔格拉布(60)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 304);
    AddGossipItemFor(player, 4, "祖阿曼(70)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 305);
    AddGossipItemFor(player, 4, "卡拉赞(70)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 306);
    AddGossipItemFor(player, 4, "奥妮克希亚巢穴(80)", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 307);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildNorthlandTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, 13, "达拉然", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 101);
    AddGossipItemFor(player, 9, "乌特加德堡垒", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 104);
    AddGossipItemFor(player, 9, "魔枢", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 103);
    AddGossipItemFor(player, 9, "龙眠神殿", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 102);
    AddGossipItemFor(player, 9, "古代王国", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 105);
    AddGossipItemFor(player, 9, "达克萨隆要塞", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 111);
    AddGossipItemFor(player, 9, "古达克", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 106);
    AddGossipItemFor(player, 9, "风暴群山", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 107);
    AddGossipItemFor(player, 9, "纳克萨玛斯", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 110);
    AddGossipItemFor(player, 9, "银色比武场", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 108);
    AddGossipItemFor(player, 9, "冰冠堡垒", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 109);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildOutlandTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, 13, "沙塔斯", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 201);
    AddGossipItemFor(player, 9, "地狱火堡垒", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 202);
    AddGossipItemFor(player, 9, "盘牙水库", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 203);
    AddGossipItemFor(player, 9, "奥金顿", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 204);
    AddGossipItemFor(player, 9, "风暴战舰", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 205);
    AddGossipItemFor(player, 9, "戈鲁尔巢穴", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 206);
    AddGossipItemFor(player, 9, "黑暗神庙", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 207);
    AddGossipItemFor(player, 9, "时光之穴", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 208);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void BuildDungeonTeleport(Player* player, Item* item, uint32 sender)
{
    PlayerMenu* menu = player->PlayerTalkClass;
    menu->ClearMenus();
    AddGossipItemFor(player, 4, "怒焰裂谷 15 - 21", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 51);
    AddGossipItemFor(player, 4, "死亡矿井 15 - 21", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 52);
    AddGossipItemFor(player, 4, "哀嚎洞穴 15 - 25", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 53);
    AddGossipItemFor(player, 4, "影牙城堡 16 - 26", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 54);
    AddGossipItemFor(player, 4, "黑暗深渊 20 - 30", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 55);
    AddGossipItemFor(player, 4, "暴风城监狱 20 - 30", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 56);
    AddGossipItemFor(player, 4, "诺莫瑞根 24 - 34", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 57);
    AddGossipItemFor(player, 4, "剃刀沼泽 25 - 30", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 59);
    AddGossipItemFor(player, 4, "血色修道院 26 - 40", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 58);
    AddGossipItemFor(player, 4, "剃刀高地 34 - 40", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 64);
    AddGossipItemFor(player, 4, "奥达曼 35 - 40", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 61);
    AddGossipItemFor(player, 4, "祖尔法拉克 43 - 46", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 66);
    AddGossipItemFor(player, 4, "玛拉顿 43 - 48", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 60);
    AddGossipItemFor(player, 4, "厄运之槌 54 - 58", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 62);
    AddGossipItemFor(player, 4, "通灵学院 59 - 61", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 63);
    AddGossipItemFor(player, 4, "斯坦索姆 56 - 60", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 65);
    AddGossipItemFor(player, 4, "黑石深渊 49 - 57", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 67);
    AddGossipItemFor(player, 4, "沉没的神庙 50- 60", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 68);
    AddGossipItemFor(player, 4, "黑石塔 55 - 65", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 69);

    SendGossipMenuFor(player, 4, item->GetGUID());
}

void GossipSelect_Item(Player* player, Item* item, uint32 sender, uint32 action)
{
    switch (action) {
    case GOSSIP_ACTION_INFO_DEF + 1: // StormWind City
        player->TeleportTo(0, -8730.59f, 722.68f, 101.7f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 2: // TLB
        player->TeleportTo(0, -4799.36, -1107.36, 501.7, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 3: // DNSS
        player->TeleportTo(1, 9961, 2055, 1329, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 4: // 埃索达
        player->TeleportTo(530, -3998.3, -11864.1, 1, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 5: // 奥格瑞玛
        player->TeleportTo(1, 1676.25f, -4313.45, 62.0f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 6: // 雷霆崖
        player->TeleportTo(1, -1150.877197f, 15.459573f, 180.088318f, 1.300810f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 7: // 幽暗城
        player->TeleportTo(0, 1596.05835f, 240.41658f, -13.89129f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 8: // 银月城
        player->TeleportTo(530, 9930.45, -7129.1, 48, 6);  
        break;
    case GOSSIP_ACTION_INFO_DEF + 9: // 棘齿城
        player->TeleportTo(1, -977, -3788, 6, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:
        player->TeleportTo(0, -14302, 518, 9, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:
        player->TeleportTo(1, -7156.56, -3825.1, 8.7, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF+12: //外域
        CloseGossipMenuFor(player);
        BuildOutlandTeleport(player, item, sender);
        return;
    case GOSSIP_ACTION_INFO_DEF + 13: // 诺森德
        CloseGossipMenuFor(player);
        BuildNorthlandTeleport(player, item, sender);
        return;
    case GOSSIP_ACTION_INFO_DEF + 14: // GM岛 
        player->TeleportTo(1, 16222.1, 16252.1, 12.5872, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 15: // 5人本
        CloseGossipMenuFor(player);
        BuildDungeonTeleport(player, item, sender);
        return;
    case GOSSIP_ACTION_INFO_DEF + 16: // 团队本
        CloseGossipMenuFor(player);
        BuildRaidTeleport(player, item, sender);
        return;
    case GOSSIP_ACTION_INFO_DEF + 51: // 怒焰裂谷
        player->TeleportTo(389, 3.8, -14.8, -17, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 52: // 矿井
        player->TeleportTo(36, -16, -383, 62, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 53: // 哀嚎
        player->TeleportTo(43, -163.49f, 132.89f, -73.66, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 54: // 影牙
        player->TeleportTo(33, -229.1f, 2109.17f, 77, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 55: // 深渊
        player->TeleportTo(48, -151.88f, 106.95f, -39.3f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 56: // 监狱
        player->TeleportTo(34, 54.2f, 0.28f, -18, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 57: // 诺莫瑞根
        player->TeleportTo(90, -327.5f, -4.7f, -152.3f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 58: // 血色
        player->TeleportTo(0, 2894.34f, -809.55f, 160.33f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 59: // 剃刀沼泽
        player->TeleportTo(47, 1943, 1544.63f, 82, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 60: // 玛拉顿
        player->TeleportTo(349, 1019.69f, -458.3f, -43, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 61: // 奥达曼
        player->TeleportTo(70, -226.8, 49.1f, -45.9f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 62: // 厄运
        player->TeleportTo(429, -201.11f, -328.66f, -2.7f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 63: // 通灵学院
        player->TeleportTo(289, 196.39f, 127, 135, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 64: // 剃刀高地
        player->TeleportTo(129, 2592.55f, 1107.5f, 51.5f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 65: // 斯坦索姆
        player->TeleportTo(329, 3394.13f, -3380.16f, 143.0f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 66: // 祖尔法拉克
        player->TeleportTo(209, 1213.52f, 841.59f, 9, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 67: // 黑石深渊
        player->TeleportTo(230, 458.3f, 26.5f, -70.64f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 68: // 沉没的神庙
        player->TeleportTo(109, -319.23f, 99.9f, -131.85f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 69: // 黑石塔
        player->TeleportTo(229, 78.5f,-225.0f, 50.0f, 6);
        break;
       
    case GOSSIP_ACTION_INFO_DEF + 101: // 达拉燃
        player->TeleportTo(571, 5797, 795, 664, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 102: // 龙眠神殿
        player->TeleportTo(571, 3546.607178f, 273.218842f, 342.722f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 103: // 魔枢
        player->TeleportTo(571, 3831.737061f, 6960.383789f, 104.784271f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 104: // 乌特加德堡垒
        player->TeleportTo(571, 1260.176636f, -4843.805664f, 215.763993f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 105: // 古代王国
        player->TeleportTo(571, 3695.932129f, 2143.285889f, 34.147270f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 106: // 古达克
        player->TeleportTo(571, 6938.497559f, -4452.765137f, 450.868896f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 107: // 风暴群山
        player->TeleportTo(571, 8949.208008f, -1266.415894f, 1025.499391f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 108: // 银色比武场
        player->TeleportTo(571, 8486.941406f, 775.859863f, 558.568299f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 109: // 冰冠堡垒
        player->TeleportTo(571, 5864.67f, 2169.83f, 636.1f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 110: // 纳克萨玛斯
        player->TeleportTo(571, 3666.089844f, -1269.738403f, 243.508927f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 111: // 达克萨隆要塞
        player->TeleportTo(571, 4772.635742f, -2046.703125f, 238.28464f, 0.061439f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 201: // 沙塔斯
        player->TeleportTo(530, -1859.95f, 5438.85f, -10.3f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 202: // 地狱火
        player->TeleportTo(530, -321.64f, 3082.49f, 32.6f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 203: // 水库
        player->TeleportTo(530, 764.034058f, 6866.363770f, -68.277512f, 6.266417f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 204: // 奥金顿
        player->TeleportTo(530, -3377.06f, 4954.24f, -66.5f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 205: // 风暴战舰
        player->TeleportTo(530, 3101.006592f, 1537.525879f, 190.31f, 4.649131f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 206: // 格鲁尔
        player->TeleportTo(565, 62.784199f, 35.462002f, -3.983500f, 1.418440f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 207: // 黑庙
        player->TeleportTo(564, 96.45f, 1002.35f, -86.8f, 6);
        break;
    case GOSSIP_ACTION_INFO_DEF + 208: // 时光之穴
        player->TeleportTo(1, -8509.349606f, -4356.310059f, -208.358994f, 6);
        break;
    // Raid telepot
    case GOSSIP_ACTION_INFO_DEF + 301: // 熔火之心
        player->TeleportTo(409, 1087.588f, -477.341f, -107.0f, 0.786652f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 305: // 祖阿曼
        player->TeleportTo(530, 6832.783203f, -7858.009766f, 163.976166f, 4.697f);
        break;
    case GOSSIP_ACTION_INFO_DEF + 308: // 奥妮克西娅
        player->TeleportTo(1, -4745.300293f, -3753.068604f, 50.219667f, 4.697f);
        break;
        /*
    case GOSSIP_ACTION_INFO_DEF + 64: // 
        player->TeleportTo(, f, f, f, 6);
        break;*/
    }
    CloseGossipMenuFor(player);
}

void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_GOSSIP_SELECT_OPTION");

    uint32 gossipListId;
    uint32 menuId;
    ObjectGuid guid;
    std::string code = "";
    recvData >> guid >> menuId >> gossipListId;
    //自定义代码---开始
    Item *pItem = _player->GetItemByGuid(guid);
    //如果item是炉石,则return
    if (pItem)
    {
        if (pItem->GetEntry() == 60000)
        {
            GossipSelect_Item(_player, pItem, _player->PlayerTalkClass->GetGossipOptionSender(gossipListId), _player->PlayerTalkClass->GetGossipOptionAction(gossipListId));
            return;
        }
    }
    //自定义代码---结束
    if (!_player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
    {
        recvData.rfinish();
        return;
    }

    if (_player->PlayerTalkClass->IsGossipOptionCoded(gossipListId))
        recvData >> code;

    // Prevent cheating on C++ scripted menus
    if (_player->PlayerTalkClass->GetGossipMenu().GetSenderGUID() != guid)
        return;

    Creature* unit = nullptr;
    GameObject* go = nullptr;
    if (guid.IsCreatureOrVehicle())
    {
        unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_GOSSIP);
        if (!unit)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - %s not found or you can't interact with him.", guid.ToString().c_str());
            return;
        }
    }
    else if (guid.IsGameObject())
    {
        go = _player->GetGameObjectIfCanInteractWith(guid);
        if (!go)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - %s not found or you can't interact with it.", guid.ToString().c_str());
            return;
        }
    }
    else
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - unsupported %s.", guid.ToString().c_str());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if ((unit && unit->GetScriptId() != unit->LastUsedScriptID) || (go && go->GetScriptId() != go->LastUsedScriptID))
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - Script reloaded while in use, ignoring and set new scipt id");
        if (unit)
            unit->LastUsedScriptID = unit->GetScriptId();
        if (go)
            go->LastUsedScriptID = go->GetScriptId();
        _player->PlayerTalkClass->SendCloseGossip();
        return;
    }
    if (!code.empty())
    {
        if (unit)
        {
            if (!unit->AI()->GossipSelectCode(_player, menuId, gossipListId, code.c_str()))
                _player->OnGossipSelect(unit, gossipListId, menuId);
        }
        else
        {
            if (!go->AI()->GossipSelectCode(_player, menuId, gossipListId, code.c_str()))
                _player->OnGossipSelect(go, gossipListId, menuId);
        }
    }
    else
    {
        if (unit)
        {
            if (!unit->AI()->GossipSelect(_player, menuId, gossipListId))
                _player->OnGossipSelect(unit, gossipListId, menuId);
        }
        else
        {
            if (!go->AI()->GossipSelect(_player, menuId, gossipListId))
                _player->OnGossipSelect(go, gossipListId, menuId);
        }
    }
}

void WorldSession::HandleWhoOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_WHO Message");

    uint32 matchCount = 0;

    uint32 levelMin, levelMax, racemask, classmask, zonesCount, strCount;
    uint32 zoneids[10];                                     // 10 is client limit
    std::string packetPlayerName, packetGuildName;

    recvData >> levelMin;                                   // maximal player level, default 0
    recvData >> levelMax;                                   // minimal player level, default 100 (MAX_LEVEL)
    recvData >> packetPlayerName;                           // player name, case sensitive...

    recvData >> packetGuildName;                            // guild name, case sensitive...

    recvData >> racemask;                                   // race mask
    recvData >> classmask;                                  // class mask
    recvData >> zonesCount;                                 // zones count, client limit = 10 (2.0.10)

    if (zonesCount > 10)
        return;                                             // can't be received from real client or broken packet

    for (uint32 i = 0; i < zonesCount; ++i)
    {
        uint32 temp;
        recvData >> temp;                                   // zone id, 0 if zone is unknown...
        zoneids[i] = temp;
        TC_LOG_DEBUG("network", "Zone %u: %u", i, zoneids[i]);
    }

    recvData >> strCount;                                   // user entered strings count, client limit=4 (checked on 2.0.10)

    if (strCount > 4)
        return;                                             // can't be received from real client or broken packet

    TC_LOG_DEBUG("network", "Minlvl %u, maxlvl %u, name %s, guild %s, racemask %u, classmask %u, zones %u, strings %u", levelMin, levelMax, packetPlayerName.c_str(), packetGuildName.c_str(), racemask, classmask, zonesCount, strCount);

    std::wstring str[4];                                    // 4 is client limit
    for (uint32 i = 0; i < strCount; ++i)
    {
        std::string temp;
        recvData >> temp;                                   // user entered string, it used as universal search pattern(guild+player name)?

        if (!Utf8toWStr(temp, str[i]))
            continue;

        wstrToLower(str[i]);

        TC_LOG_DEBUG("network", "String %u: %s", i, temp.c_str());
    }

    std::wstring wpacketPlayerName;
    std::wstring wpacketGuildName;
    if (!(Utf8toWStr(packetPlayerName, wpacketPlayerName) && Utf8toWStr(packetGuildName, wpacketGuildName)))
        return;

    wstrToLower(wpacketPlayerName);
    wstrToLower(wpacketGuildName);

    // client send in case not set max level value 100 but Trinity supports 255 max level,
    // update it to show GMs with characters after 100 level
    if (levelMax >= MAX_LEVEL)
        levelMax = STRONG_MAX_LEVEL;

    uint32 team = _player->GetTeam();

    uint32 gmLevelInWhoList  = sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST);
    uint32 displayCount = 0;

    WorldPacket data(SMSG_WHO, 500);                      // guess size
    data << uint32(matchCount);                           // placeholder, count of players matching criteria
    data << uint32(displayCount);                         // placeholder, count of players displayed

    WhoListInfoVector const& whoList = sWhoListStorageMgr->GetWhoList();
    for (WhoListPlayerInfo const& target : whoList)
    {
        // player can see member of other team only if CONFIG_ALLOW_TWO_SIDE_WHO_LIST
        if (target.GetTeam() != team && !HasPermission(rbac::RBAC_PERM_TWO_SIDE_WHO_LIST))
            continue;

        // player can see MODERATOR, GAME MASTER, ADMINISTRATOR only if CONFIG_GM_IN_WHO_LIST
        if (!HasPermission(rbac::RBAC_PERM_WHO_SEE_ALL_SEC_LEVELS) && target.GetSecurity() > AccountTypes(gmLevelInWhoList))
            continue;

        // check if target is globally visible for player
        if (_player->GetGUID() != target.GetGuid() && !target.IsVisible())
            if (AccountMgr::IsPlayerAccount(_player->GetSession()->GetSecurity()) || target.GetSecurity() > _player->GetSession()->GetSecurity())
                continue;

        // check if target's level is in level range
        uint8 lvl = target.GetLevel();
        if (lvl < levelMin || lvl > levelMax)
            continue;

        // check if class matches classmask
        uint8 class_ = target.GetClass();
        if (!(classmask & (1 << class_)))
            continue;

        // check if race matches racemask
        uint32 race = target.GetRace();
        if (!(racemask & (1 << race)))
            continue;

        uint32 playerZoneId = target.GetZoneId();
        uint8 gender = target.GetGender();

        bool showZones = true;
        for (uint32 i = 0; i < zonesCount; ++i)
        {
            if (zoneids[i] == playerZoneId)
            {
                showZones = true;
                break;
            }

            showZones = false;
        }
        if (!showZones)
            continue;

        std::wstring const& wideplayername = target.GetWidePlayerName();
        if (!(wpacketPlayerName.empty() || wideplayername.find(wpacketPlayerName) != std::wstring::npos))
            continue;

        std::wstring const& wideguildname = target.GetWideGuildName();
        if (!(wpacketGuildName.empty() || wideguildname.find(wpacketGuildName) != std::wstring::npos))
            continue;

        std::string aname;
        if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(playerZoneId))
            aname = areaEntry->area_name[GetSessionDbcLocale()];

        bool s_show = true;
        for (uint32 i = 0; i < strCount; ++i)
        {
            if (!str[i].empty())
            {
                if (wideguildname.find(str[i]) != std::wstring::npos ||
                    wideplayername.find(str[i]) != std::wstring::npos ||
                    Utf8FitTo(aname, str[i]))
                {
                    s_show = true;
                    break;
                }
                s_show = false;
            }
        }
        if (!s_show)
            continue;

        // 49 is maximum player count sent to client - can be overridden
        // through config, but is unstable
        if ((matchCount++) >= sWorld->getIntConfig(CONFIG_MAX_WHO))
            continue;

        data << target.GetPlayerName();                   // player name
        data << target.GetGuildName();                    // guild name
        data << uint32(lvl);                              // player level
        data << uint32(class_);                           // player class
        data << uint32(race);                             // player race
        data << uint8(gender);                            // player gender
        data << uint32(playerZoneId);                     // player zone id

        ++displayCount;
    }

    data.put(0, displayCount);                            // insert right count, count displayed
    data.put(4, matchCount);                              // insert right count, count of matches

    SendPacket(&data);
    TC_LOG_DEBUG("network", "WORLD: Send SMSG_WHO Message");
}

void WorldSession::HandleLogoutRequestOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_LOGOUT_REQUEST Message, security - %u", GetSecurity());

    if (ObjectGuid lguid = GetPlayer()->GetLootGUID())
        DoLootRelease(lguid);

    bool instantLogout = (GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) && !GetPlayer()->IsInCombat()) ||
                         GetPlayer()->IsInFlight() || HasPermission(rbac::RBAC_PERM_INSTANT_LOGOUT);

    /// TODO: Possibly add RBAC permission to log out in combat
    bool canLogoutInCombat = GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING);

    uint32 reason = 0;
    if (GetPlayer()->IsInCombat() && !canLogoutInCombat)
        reason = 1;
    else if (GetPlayer()->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR))
        reason = 3;                                         // is jumping or falling
    else if (GetPlayer()->duel || GetPlayer()->HasAura(9454)) // is dueling or frozen by GM via freeze command
        reason = 2;                                         // FIXME - Need the correct value

    WorldPacket data(SMSG_LOGOUT_RESPONSE, 1+4);
    data << uint32(reason);
    data << uint8(instantLogout);
    SendPacket(&data);

    if (reason)
    {
        LogoutRequest(0);
        return;
    }

    //instant logout in taverns/cities or on taxi or for admins, gm's, mod's if its enabled in worldserver.conf
    if (instantLogout)
    {
        LogoutPlayer(true);
        return;
    }

    // not set flags if player can't free move to prevent lost state at logout cancel
    if (GetPlayer()->CanFreeMove())
    {
        if (GetPlayer()->GetStandState() == UNIT_STAND_STATE_STAND)
            GetPlayer()->SetStandState(UNIT_STAND_STATE_SIT);

        data.Initialize(SMSG_FORCE_MOVE_ROOT, (8+4));    // guess size
        data << GetPlayer()->GetPackGUID();
        data << (uint32)2;
        SendPacket(&data);
        GetPlayer()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    LogoutRequest(GameTime::GetGameTime());
}

void WorldSession::HandlePlayerLogoutOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_PLAYER_LOGOUT Message");
}

void WorldSession::HandleLogoutCancelOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_LOGOUT_CANCEL Message");

    // Player have already logged out serverside, too late to cancel
    if (!GetPlayer())
        return;

    LogoutRequest(0);

    WorldPacket data(SMSG_LOGOUT_CANCEL_ACK, 0);
    SendPacket(&data);

    // not remove flags if can't free move - its not set in Logout request code.
    if (GetPlayer()->CanFreeMove())
    {
        //!we can move again
        data.Initialize(SMSG_FORCE_MOVE_UNROOT, 8);       // guess size
        data << GetPlayer()->GetPackGUID();
        data << uint32(0);
        SendPacket(&data);

        //! Stand Up
        GetPlayer()->SetStandState(UNIT_STAND_STATE_STAND);

        //! DISABLE_ROTATE
        GetPlayer()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }
}

void WorldSession::HandleTogglePvP(WorldPacket& recvData)
{
    // this opcode can be used in two ways: Either set explicit new status or toggle old status
    if (recvData.size() == 1)
    {
        bool newPvPStatus;
        recvData >> newPvPStatus;
        GetPlayer()->ApplyModFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP, newPvPStatus);
        GetPlayer()->ApplyModFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_TIMER, !newPvPStatus);
    }
    else
    {
        GetPlayer()->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        GetPlayer()->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_TIMER);
    }

    if (GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
    {
        if (!GetPlayer()->IsPvP() || GetPlayer()->pvpInfo.EndTimer)
            GetPlayer()->UpdatePvP(true, true);
    }
    else
    {
        if (!GetPlayer()->pvpInfo.IsHostile && GetPlayer()->IsPvP())
            GetPlayer()->pvpInfo.EndTimer = GameTime::GetGameTime();     // start toggle-off
    }

    //if (OutdoorPvP* pvp = _player->GetOutdoorPvP())
    //    pvp->HandlePlayerActivityChanged(_player);
}

void WorldSession::HandleZoneUpdateOpcode(WorldPacket& recvData)
{
    uint32 newZone;
    recvData >> newZone;

    TC_LOG_DEBUG("network", "WORLD: Recvd ZONE_UPDATE: %u", newZone);

    // use server side data, but only after update the player position. See Player::UpdatePosition().
    GetPlayer()->SetNeedsZoneUpdate(true);

    //GetPlayer()->SendInitWorldStates(true, newZone);
}

void WorldSession::HandleSetSelectionOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    _player->SetSelection(guid);
}

void WorldSession::HandleStandStateChangeOpcode(WorldPacket& recvData)
{
    // TC_LOG_DEBUG("network", "WORLD: Received CMSG_STANDSTATECHANGE"); -- too many spam in log at lags/debug stop
    uint32 animstate;
    recvData >> animstate;

    _player->SetStandState(animstate);
}

void WorldSession::HandleBugOpcode(WorldPacket& recvData)
{
    uint32 suggestion, contentlen, typelen;
    std::string content, type;

    recvData >> suggestion >> contentlen >> content;

    recvData >> typelen >> type;

    if (suggestion == 0)
        TC_LOG_DEBUG("network", "WORLD: Received CMSG_BUG [Bug Report]");
    else
        TC_LOG_DEBUG("network", "WORLD: Received CMSG_BUG [Suggestion]");

    TC_LOG_DEBUG("network", "%s", type.c_str());
    TC_LOG_DEBUG("network", "%s", content.c_str());

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_BUG_REPORT);

    stmt->setString(0, type);
    stmt->setString(1, content);

    CharacterDatabase.Execute(stmt);
}

void WorldSession::HandleReclaimCorpseOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RECLAIM_CORPSE");

    ObjectGuid guid;
    recvData >> guid;

    if (_player->IsAlive())
        return;

    // do not allow corpse reclaim in arena
    if (_player->InArena())
        return;

    // body not released yet
    if (!_player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    Corpse* corpse = _player->GetCorpse();
    if (!corpse)
        return;

    // prevent resurrect before 30-sec delay after body release not finished
    if (time_t(corpse->GetGhostTime() + _player->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP)) > GameTime::GetGameTime())
        return;

    if (!corpse->IsWithinDistInMap(_player, CORPSE_RECLAIM_RADIUS, true))
        return;

    // resurrect
    _player->ResurrectPlayer(_player->InBattleground() ? 1.0f : 0.5f);

    // spawn bones
    _player->SpawnCorpseBones();
}

void WorldSession::HandleResurrectResponseOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_RESURRECT_RESPONSE");

    ObjectGuid guid;
    uint8 status;
    recvData >> guid;
    recvData >> status;

    if (GetPlayer()->IsAlive())
        return;

    if (status == 0)
    {
        GetPlayer()->ClearResurrectRequestData();           // reject
        return;
    }

    if (!GetPlayer()->IsResurrectRequestedBy(guid))
        return;

    GetPlayer()->ResurrectUsingRequestData();
}

void WorldSession::SendAreaTriggerMessage(char const* Text, ...)
{
    va_list ap;
    char szStr [1024];
    szStr[0] = '\0';

    va_start(ap, Text);
    vsnprintf(szStr, 1024, Text, ap);
    va_end(ap);

    uint32 length = strlen(szStr)+1;
    WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 4+length);
    data << length;
    data << szStr;
    SendPacket(&data);
}

void WorldSession::HandleAreaTriggerOpcode(WorldPacket& recvData)
{
    uint32 triggerId;
    recvData >> triggerId;

    TC_LOG_DEBUG("network", "CMSG_AREATRIGGER. Trigger ID: %u", triggerId);

    Player* player = GetPlayer();
    if (player->IsInFlight())
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) in flight, ignore Area Trigger ID:%u",
            player->GetName().c_str(), player->GetGUID().GetCounter(), triggerId);
        return;
    }

    AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(triggerId);
    if (!atEntry)
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) send unknown (by DBC) Area Trigger ID:%u",
            player->GetName().c_str(), player->GetGUID().GetCounter(), triggerId);
        return;
    }

    if (!player->IsInAreaTriggerRadius(atEntry))
    {
        TC_LOG_DEBUG("network", "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) too far, ignore Area Trigger ID: %u",
            player->GetName().c_str(), player->GetGUID().GetCounter(), triggerId);
        return;
    }

    if (player->isDebugAreaTriggers)
        ChatHandler(player->GetSession()).PSendSysMessage(LANG_DEBUG_AREATRIGGER_REACHED, triggerId);

    if (sScriptMgr->OnAreaTrigger(player, atEntry))
        return;

    if (player->IsAlive())
        if (uint32 questId = sObjectMgr->GetQuestForAreaTrigger(triggerId))
            if (player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(questId);

    if (sObjectMgr->IsTavernAreaTrigger(triggerId))
    {
        // set resting flag we are in the inn
        player->SetRestFlag(REST_FLAG_IN_TAVERN, atEntry->id);

        if (sWorld->IsFFAPvPRealm())
            player->RemoveByteFlag(UNIT_FIELD_BYTES_2, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_FFA_PVP);

        return;
    }

    if (Battleground* bg = player->GetBattleground())
        if (bg->GetStatus() == STATUS_IN_PROGRESS)
            bg->HandleAreaTrigger(player, triggerId);

    if (OutdoorPvP* pvp = player->GetOutdoorPvP())
        if (pvp->HandleAreaTrigger(_player, triggerId))
            return;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(triggerId);
    if (!at)
        return;

    bool teleported = false;
    if (player->GetMapId() != at->target_mapId)
    {
        if (Map::EnterState denyReason = sMapMgr->PlayerCannotEnter(at->target_mapId, player, false))
        {
            bool reviveAtTrigger = false; // should we revive the player if he is trying to enter the correct instance?
            switch (denyReason)
            {
                case Map::CANNOT_ENTER_NO_ENTRY:
                    TC_LOG_DEBUG("maps", "MAP: Player '%s' attempted to enter map with id %d which has no entry", player->GetName().c_str(), at->target_mapId);
                    break;
                case Map::CANNOT_ENTER_UNINSTANCED_DUNGEON:
                    TC_LOG_DEBUG("maps", "MAP: Player '%s' attempted to enter dungeon map %d but no instance template was found", player->GetName().c_str(), at->target_mapId);
                    break;
                case Map::CANNOT_ENTER_DIFFICULTY_UNAVAILABLE:
                    TC_LOG_DEBUG("maps", "MAP: Player '%s' attempted to enter instance map %d but the requested difficulty was not found", player->GetName().c_str(), at->target_mapId);
                    if (MapEntry const* entry = sMapStore.LookupEntry(at->target_mapId))
                        player->SendTransferAborted(entry->MapID, TRANSFER_ABORT_DIFFICULTY, player->GetDifficulty(entry->IsRaid()));
                    break;
                case Map::CANNOT_ENTER_NOT_IN_RAID:
                {
                    WorldPacket data(SMSG_RAID_GROUP_ONLY, 4 + 4);
                    data << uint32(0);
                    data << uint32(2); // You must be in a raid group to enter this instance.
                    player->SendDirectMessage(&data);
                    TC_LOG_DEBUG("maps", "MAP: Player '%s' must be in a raid group to enter instance map %d", player->GetName().c_str(), at->target_mapId);
                    reviveAtTrigger = true;
                    break;
                }
                case Map::CANNOT_ENTER_CORPSE_IN_DIFFERENT_INSTANCE:
                {
                    WorldPacket data(SMSG_CORPSE_NOT_IN_INSTANCE);
                    player->SendDirectMessage(&data);
                    TC_LOG_DEBUG("maps", "MAP: Player '%s' does not have a corpse in instance map %d and cannot enter", player->GetName().c_str(), at->target_mapId);
                    break;
                }
                case Map::CANNOT_ENTER_INSTANCE_BIND_MISMATCH:
                    if (MapEntry const* entry = sMapStore.LookupEntry(at->target_mapId))
                    {
                        char const* mapName = entry->name[player->GetSession()->GetSessionDbcLocale()];
                        TC_LOG_DEBUG("maps", "MAP: Player '%s' cannot enter instance map '%s' because their permanent bind is incompatible with their group's", player->GetName().c_str(), mapName);
                        // is there a special opcode for this?
                        // @todo figure out how to get player localized difficulty string (e.g. "10 player", "Heroic" etc)
                        ChatHandler(player->GetSession()).PSendSysMessage(player->GetSession()->GetTrinityString(LANG_INSTANCE_BIND_MISMATCH), mapName);
                    }
                    reviveAtTrigger = true;
                    break;
                case Map::CANNOT_ENTER_TOO_MANY_INSTANCES:
                    player->SendTransferAborted(at->target_mapId, TRANSFER_ABORT_TOO_MANY_INSTANCES);
                    TC_LOG_DEBUG("maps", "MAP: Player '%s' cannot enter instance map %d because he has exceeded the maximum number of instances per hour.", player->GetName().c_str(), at->target_mapId);
                    reviveAtTrigger = true;
                    break;
                case Map::CANNOT_ENTER_MAX_PLAYERS:
                    player->SendTransferAborted(at->target_mapId, TRANSFER_ABORT_MAX_PLAYERS);
                    reviveAtTrigger = true;
                    break;
                case Map::CANNOT_ENTER_ZONE_IN_COMBAT:
                    player->SendTransferAborted(at->target_mapId, TRANSFER_ABORT_ZONE_IN_COMBAT);
                    reviveAtTrigger = true;
                    break;
                default:
                    break;
            }

            if (reviveAtTrigger) // check if the player is touching the areatrigger leading to the map his corpse is on
                if (!player->IsAlive() && player->HasCorpse())
                    if (player->GetCorpseLocation().GetMapId() == at->target_mapId)
                    {
                        player->ResurrectPlayer(0.5f);
                        player->SpawnCorpseBones();
                    }

            return;
        }

        if (Group* group = player->GetGroup())
            if (group->isLFGGroup() && player->GetMap()->IsDungeon())
                teleported = player->TeleportToBGEntryPoint();
    }

    if (!teleported)
        player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
}

void WorldSession::HandleUpdateAccountData(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_UPDATE_ACCOUNT_DATA");

    uint32 type, timestamp, decompressedSize;
    recvData >> type >> timestamp >> decompressedSize;

    TC_LOG_DEBUG("network", "UAD: type %u, time %u, decompressedSize %u", type, timestamp, decompressedSize);

    if (type > NUM_ACCOUNT_DATA_TYPES)
        return;

    if (decompressedSize == 0)                               // erase
    {
        SetAccountData(AccountDataType(type), 0, "");

        WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, 4+4);
        data << uint32(type);
        data << uint32(0);
        SendPacket(&data);

        return;
    }

    if (decompressedSize > 0xFFFF)
    {
        recvData.rfinish();                   // unnneded warning spam in this case
        TC_LOG_ERROR("network", "UAD: Account data packet too big, size %u", decompressedSize);
        return;
    }

    ByteBuffer dest;
    dest.resize(decompressedSize);

    uLongf realSize = decompressedSize;
    if (uncompress(dest.contents(), &realSize, recvData.contents() + recvData.rpos(), recvData.size() - recvData.rpos()) != Z_OK)
    {
        recvData.rfinish();                   // unnneded warning spam in this case
        TC_LOG_ERROR("network", "UAD: Failed to decompress account data");
        return;
    }

    recvData.rfinish();                       // uncompress read (recvData.size() - recvData.rpos())

    std::string adata;
    dest >> adata;

    SetAccountData(AccountDataType(type), timestamp, adata);

    WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA_COMPLETE, 4+4);
    data << uint32(type);
    data << uint32(0);
    SendPacket(&data);
}

void WorldSession::HandleRequestAccountData(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_REQUEST_ACCOUNT_DATA");

    uint32 type;
    recvData >> type;

    TC_LOG_DEBUG("network", "RAD: type %u", type);

    if (type >= NUM_ACCOUNT_DATA_TYPES)
        return;

    AccountData* adata = GetAccountData(AccountDataType(type));

    uint32 size = adata->Data.size();

    uLongf destSize = compressBound(size);

    ByteBuffer dest;
    dest.resize(destSize);

    if (size && compress(dest.contents(), &destSize, (uint8 const*)adata->Data.c_str(), size) != Z_OK)
    {
        TC_LOG_DEBUG("network", "RAD: Failed to compress account data");
        return;
    }

    dest.resize(destSize);

    WorldPacket data(SMSG_UPDATE_ACCOUNT_DATA, 8+4+4+4+destSize);
    data << uint64(_player ? _player->GetGUID() : ObjectGuid::Empty);
    data << uint32(type);                                   // type (0-7)
    data << uint32(adata->Time);                            // unix time
    data << uint32(size);                                   // decompressed length
    data.append(dest);                                      // compressed data
    SendPacket(&data);
}

void WorldSession::HandleSetActionButtonOpcode(WorldPacket& recvData)
{
    uint8 button;
    uint32 packetData;
    recvData >> button >> packetData;
    TC_LOG_DEBUG("network", "CMSG_SET_ACTION_BUTTON Button: %u Data: %u", button, packetData);

    if (!packetData)
        GetPlayer()->removeActionButton(button);
    else
        GetPlayer()->addActionButton(button, ACTION_BUTTON_ACTION(packetData), ACTION_BUTTON_TYPE(packetData));
}

void WorldSession::HandleCompleteCinematic(WorldPacket& /*recvData*/)
{
    // If player has sight bound to visual waypoint NPC we should remove it
    GetPlayer()->GetCinematicMgr()->EndCinematic();
}

void WorldSession::HandleNextCinematicCamera(WorldPacket& /*recvData*/)
{
    // Sent by client when cinematic actually begun. So we begin the server side process
    GetPlayer()->GetCinematicMgr()->BeginCinematic();
}

void WorldSession::HandleFeatherFallAck(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_MOVE_FEATHER_FALL_ACK");

    // no used
    recvData.rfinish();                       // prevent warnings spam
}

void WorldSession::HandleMoveUnRootAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
/*
    uint64 guid;
    recvData >> guid;

    // now can skip not our packet
    if (_player->GetGUID() != guid)
    {
        recvData.rfinish();                   // prevent warnings spam
        return;
    }

    TC_LOG_DEBUG("network", "WORLD: CMSG_FORCE_MOVE_UNROOT_ACK");

    recvData.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recvData, &movementInfo);
    recvData.read_skip<float>();                           // unk2
*/
}

void WorldSession::HandleMoveRootAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
/*
    uint64 guid;
    recvData >> guid;

    // now can skip not our packet
    if (_player->GetGUID() != guid)
    {
        recvData.rfinish();                   // prevent warnings spam
        return;
    }

    TC_LOG_DEBUG("network", "WORLD: CMSG_FORCE_MOVE_ROOT_ACK");

    recvData.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recvData, &movementInfo);
*/
}

void WorldSession::HandleSetActionBarToggles(WorldPacket& recvData)
{
    uint8 actionBar;
    recvData >> actionBar;

    if (!GetPlayer())                                        // ignore until not logged (check needed because STATUS_AUTHED)
    {
        if (actionBar != 0)
            TC_LOG_ERROR("network", "WorldSession::HandleSetActionBarToggles in not logged state with value: %u, ignored", uint32(actionBar));
        return;
    }

    GetPlayer()->SetByteValue(PLAYER_FIELD_BYTES, PLAYER_FIELD_BYTES_OFFSET_ACTION_BAR_TOGGLES, actionBar);
}

void WorldSession::HandlePlayedTime(WorldPacket& recvData)
{
    uint8 unk1;
    recvData >> unk1;                                      // 0 or 1 expected

    WorldPacket data(SMSG_PLAYED_TIME, 4 + 4 + 1);
    data << uint32(_player->GetTotalPlayedTime());
    data << uint32(_player->GetLevelPlayedTime());
    data << uint8(unk1);                                    // 0 - will not show in chat frame
    SendPacket(&data);
}

void WorldSession::HandleInspectOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_INSPECT");

    Player* player = ObjectAccessor::GetPlayer(*_player, guid);
    if (!player)
    {
        TC_LOG_DEBUG("network", "CMSG_INSPECT: No player found from %s", guid.ToString().c_str());
        return;
    }

    if (!GetPlayer()->IsWithinDistInMap(player, INSPECT_DISTANCE, false))
        return;

    if (GetPlayer()->IsValidAttackTarget(player))
        return;

    uint32 talent_points = 0x47;
    uint32 guid_size = player->GetPackGUID().size();
    WorldPacket data(SMSG_INSPECT_TALENT, guid_size+4+talent_points);
    data << player->GetPackGUID();

    if (GetPlayer()->CanBeGameMaster() || sWorld->getIntConfig(CONFIG_TALENTS_INSPECTING) + (GetPlayer()->GetTeamId() == player->GetTeamId()) > 1)
        player->BuildPlayerTalentsInfoData(&data);
    else
    {
        data << uint32(0);                                  // unspentTalentPoints
        data << uint8(0);                                   // talentGroupCount
        data << uint8(0);                                   // talentGroupIndex
    }

    player->BuildEnchantmentsInfoData(&data);
    SendPacket(&data);
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    Player* player = ObjectAccessor::GetPlayer(*_player, guid);

    if (!player)
    {
        TC_LOG_DEBUG("network", "MSG_INSPECT_HONOR_STATS: No player found from %s", guid.ToString().c_str());
        return;
    }

    if (!GetPlayer()->IsWithinDistInMap(player, INSPECT_DISTANCE, false))
        return;

    if (GetPlayer()->IsValidAttackTarget(player))
        return;

    WorldPacket data(MSG_INSPECT_HONOR_STATS, 8+1+4*4);
    data << uint64(player->GetGUID());
    data << uint8(player->GetHonorPoints());
    data << uint32(player->GetUInt32Value(PLAYER_FIELD_KILLS));
    data << uint32(player->GetUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION));
    data << uint32(player->GetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION));
    data << uint32(player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS));
    SendPacket(&data);
}

void WorldSession::HandleWorldTeleportOpcode(WorldPacket& recvData)
{
    uint32 time;
    uint32 mapid;
    float PositionX;
    float PositionY;
    float PositionZ;
    float Orientation;

    recvData >> time;                                      // time in m.sec.
    recvData >> mapid;
    recvData >> PositionX;
    recvData >> PositionY;
    recvData >> PositionZ;
    recvData >> Orientation;                               // o (3.141593 = 180 degrees)

    TC_LOG_DEBUG("network", "WORLD: Received CMSG_WORLD_TELEPORT");

    if (GetPlayer()->IsInFlight())
    {
        TC_LOG_DEBUG("network", "Player '%s' (GUID: %u) in flight, ignore worldport command.",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());
        return;
    }

    TC_LOG_DEBUG("network", "CMSG_WORLD_TELEPORT: Player = %s, Time = %u, map = %u, x = %f, y = %f, z = %f, o = %f",
        GetPlayer()->GetName().c_str(), time, mapid, PositionX, PositionY, PositionZ, Orientation);

    if (HasPermission(rbac::RBAC_PERM_OPCODE_WORLD_TELEPORT))
        GetPlayer()->TeleportTo(mapid, PositionX, PositionY, PositionZ, Orientation);
    else
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
}

void WorldSession::HandleWhoisOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_WHOIS");
    std::string charname;
    recvData >> charname;

    if (!HasPermission(rbac::RBAC_PERM_OPCODE_WHOIS))
    {
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
        return;
    }

    if (charname.empty() || !normalizePlayerName (charname))
    {
        SendNotification(LANG_NEED_CHARACTER_NAME);
        return;
    }

    Player* player = ObjectAccessor::FindConnectedPlayerByName(charname);

    if (!player)
    {
        SendNotification(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, charname.c_str());
        return;
    }

    uint32 accid = player->GetSession()->GetAccountId();

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WHOIS);

    stmt->setUInt32(0, accid);

    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
    {
        SendNotification(LANG_ACCOUNT_FOR_PLAYER_NOT_FOUND, charname.c_str());
        return;
    }

    Field* fields = result->Fetch();
    std::string acc = fields[0].GetString();
    if (acc.empty())
        acc = "Unknown";
    std::string email = fields[1].GetString();
    if (email.empty())
        email = "Unknown";
    std::string lastip = fields[2].GetString();
    if (lastip.empty())
        lastip = "Unknown";

    std::string msg = charname + "'s " + "account is " + acc + ", e-mail: " + email + ", last ip: " + lastip;

    WorldPacket data(SMSG_WHOIS, msg.size()+1);
    data << msg;
    SendPacket(&data);

    TC_LOG_DEBUG("network", "Received whois command from player %s for character %s",
        GetPlayer()->GetName().c_str(), charname.c_str());
}

void WorldSession::HandleComplainOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_COMPLAIN");

    uint8 spam_type;                                        // 0 - mail, 1 - chat
    ObjectGuid spammer_guid;
    uint32 unk1 = 0;
    uint32 unk2 = 0;
    uint32 unk3 = 0;
    uint32 unk4 = 0;
    std::string description = "";
    recvData >> spam_type;                                 // unk 0x01 const, may be spam type (mail/chat)
    recvData >> spammer_guid;                              // player guid
    switch (spam_type)
    {
        case 0:
            recvData >> unk1;                              // const 0
            recvData >> unk2;                              // probably mail id
            recvData >> unk3;                              // const 0
            break;
        case 1:
            recvData >> unk1;                              // probably language
            recvData >> unk2;                              // message type?
            recvData >> unk3;                              // probably channel id
            recvData >> unk4;                              // time
            recvData >> description;                       // spam description string (messagetype, channel name, player name, message)
            break;
    }

    // NOTE: all chat messages from this spammer automatically ignored by spam reporter until logout in case chat spam.
    // if it's mail spam - ALL mails from this spammer automatically removed by client

    // Complaint Received message
    WorldPacket data(SMSG_COMPLAIN_RESULT, 1);
    data << uint8(0);
    SendPacket(&data);

    TC_LOG_DEBUG("network", "REPORT SPAM: type %u, %s, unk1 %u, unk2 %u, unk3 %u, unk4 %u, message %s",
        spam_type, spammer_guid.ToString().c_str(), unk1, unk2, unk3, unk4, description.c_str());
}

void WorldSession::HandleRealmSplitOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_REALM_SPLIT");

    uint32 unk;
    std::string split_date = "01/01/01";
    recvData >> unk;

    WorldPacket data(SMSG_REALM_SPLIT, 4+4+split_date.size()+1);
    data << unk;
    data << uint32(0x00000000);                             // realm split state
    // split states:
    // 0x0 realm normal
    // 0x1 realm split
    // 0x2 realm split pending
    data << split_date;
    SendPacket(&data);
    //TC_LOG_DEBUG("response sent %u", unk);
}

void WorldSession::HandleFarSightOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_FAR_SIGHT");

    bool apply;
    recvData >> apply;

    if (apply)
    {
        TC_LOG_DEBUG("network", "Added FarSight %s to player %u", _player->GetGuidValue(PLAYER_FARSIGHT).ToString().c_str(), _player->GetGUID().GetCounter());
        if (WorldObject* target = _player->GetViewpoint())
            _player->SetSeer(target);
        else
            TC_LOG_DEBUG("network", "Player %s (%s) requests non-existing seer %s", _player->GetName().c_str(), _player->GetGUID().ToString().c_str(), _player->GetGuidValue(PLAYER_FARSIGHT).ToString().c_str());
    }
    else
    {
        TC_LOG_DEBUG("network", "Player %u set vision to self", _player->GetGUID().GetCounter());
        _player->SetSeer(_player);
    }

    GetPlayer()->UpdateVisibilityForPlayer();
}

void WorldSession::HandleSetTitleOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_SET_TITLE");

    int32 title;
    recvData >> title;

    // -1 at none
    if (title > 0 && title < MAX_TITLE_INDEX)
    {
       if (!GetPlayer()->HasTitle(title))
            return;
    }
    else
        title = 0;

    GetPlayer()->SetUInt32Value(PLAYER_CHOSEN_TITLE, title);
}

void WorldSession::HandleTimeSyncResp(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_TIME_SYNC_RESP");

    uint32 counter, clientTicks;
    recvData >> counter >> clientTicks;

    if (counter != _player->m_timeSyncCounter - 1)
        TC_LOG_DEBUG("network", "Wrong time sync counter from player %s (cheater?)", _player->GetName().c_str());

    TC_LOG_DEBUG("network", "Time sync received: counter %u, client ticks %u, time since last sync %u", counter, clientTicks, clientTicks - _player->m_timeSyncClient);

    uint32 ourTicks = clientTicks + (GameTime::GetGameTimeMS() - _player->m_timeSyncServer);

    // diff should be small
    TC_LOG_DEBUG("network", "Our ticks: %u, diff %u, latency %u", ourTicks, ourTicks - clientTicks, GetLatency());

    _player->m_timeSyncClient = clientTicks;
}

void WorldSession::HandleResetInstancesOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_RESET_INSTANCES");

    if (Group* group = _player->GetGroup())
    {
        if (group->IsLeader(_player->GetGUID()))
            group->ResetInstances(INSTANCE_RESET_ALL, false, _player);
    }
    else
        _player->ResetInstances(INSTANCE_RESET_ALL, false);
}

void WorldSession::HandleSetDungeonDifficultyOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "MSG_SET_DUNGEON_DIFFICULTY");

    uint32 mode;
    recvData >> mode;

    if (mode >= MAX_DUNGEON_DIFFICULTY)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUID().GetCounter(), mode);
        return;
    }

    if (Difficulty(mode) == _player->GetDungeonDifficulty())
        return;

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->IsDungeon())
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player (Name: %s, GUID: %u) tried to reset the instance while player is inside!",
            _player->GetName().c_str(), _player->GetGUID().GetCounter());
        return;
    }

    Group* group = _player->GetGroup();
    if (group)
    {
        if (group->IsLeader(_player->GetGUID()))
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* groupGuy = itr->GetSource();
                if (!groupGuy)
                    continue;

                if (!groupGuy->IsInMap(groupGuy))
                    return;

                if (groupGuy->GetMap()->IsNonRaidDungeon())
                {
                    TC_LOG_DEBUG("network", "WorldSession::HandleSetDungeonDifficultyOpcode: player %d tried to reset the instance while group member (Name: %s, GUID: %u) is inside!",
                        _player->GetGUID().GetCounter(), groupGuy->GetName().c_str(), groupGuy->GetGUID().GetCounter());
                    return;
                }
            }
            // the difficulty is set even if the instances can't be reset
            //_player->SendDungeonDifficulty(true);
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, _player);
            group->SetDungeonDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false);
        _player->SetDungeonDifficulty(Difficulty(mode));
    }
}

void WorldSession::HandleSetRaidDifficultyOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "MSG_SET_RAID_DIFFICULTY");

    uint32 mode;
    recvData >> mode;

    if (mode >= MAX_RAID_DIFFICULTY)
    {
        TC_LOG_ERROR("network", "WorldSession::HandleSetRaidDifficultyOpcode: player %d sent an invalid instance mode %d!", _player->GetGUID().GetCounter(), mode);
        return;
    }

    // cannot reset while in an instance
    Map* map = _player->FindMap();
    if (map && map->IsDungeon())
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUID().GetCounter());
        return;
    }

    if (Difficulty(mode) == _player->GetRaidDifficulty())
        return;

    Group* group = _player->GetGroup();
    if (group)
    {
        if (group->IsLeader(_player->GetGUID()))
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* groupGuy = itr->GetSource();
                if (!groupGuy)
                    continue;

                if (!groupGuy->IsInMap(groupGuy))
                    return;

                if (groupGuy->GetMap()->IsRaid())
                {
                    TC_LOG_DEBUG("network", "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", _player->GetGUID().GetCounter());
                    return;
                }
            }
            // the difficulty is set even if the instances can't be reset
            //_player->SendDungeonDifficulty(true);
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true, _player);
            group->SetRaidDifficulty(Difficulty(mode));
        }
    }
    else
    {
        _player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true);
        _player->SetRaidDifficulty(Difficulty(mode));
    }
}

void WorldSession::HandleCancelMountAuraOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_CANCEL_MOUNT_AURA");

    //If player is not mounted, so go out :)
    if (!_player->IsMounted())                              // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_CHAR_NON_MOUNTED);
        return;
    }

    if (_player->IsInFlight())                               // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_YOU_IN_FLIGHT);
        return;
    }

    _player->RemoveAurasByType(SPELL_AURA_MOUNTED); // Calls Dismount()
}

void WorldSession::HandleMoveSetCanFlyAckOpcode(WorldPacket& recvData)
{
    // fly mode on/off
    TC_LOG_DEBUG("network", "WORLD: CMSG_MOVE_SET_CAN_FLY_ACK");

    ObjectGuid guid;                                            // guid - unused
    recvData >> guid.ReadAsPacked();

    recvData.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recvData, &movementInfo);

    recvData.read_skip<float>();                           // unk2

    _player->m_unitMovedByMe->m_movementInfo.flags = movementInfo.GetMovementFlags();
}

void WorldSession::HandleRequestPetInfoOpcode(WorldPacket& /*recvData */)
{
    /*
        TC_LOG_DEBUG("network", "WORLD: CMSG_REQUEST_PET_INFO");
        recvData.hexlike();
    */
}

void WorldSession::HandleSetTaxiBenchmarkOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SET_TAXI_BENCHMARK_MODE");

    uint8 mode;
    recvData >> mode;

    mode ? _player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK) : _player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_TAXI_BENCHMARK);

    TC_LOG_DEBUG("network", "Client used \"/timetest %d\" command", mode);
}

void WorldSession::HandleQueryInspectAchievements(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid.ReadAsPacked();

    TC_LOG_DEBUG("network", "CMSG_QUERY_INSPECT_ACHIEVEMENTS [%s] Inspected Player [%s]", _player->GetGUID().ToString().c_str(), guid.ToString().c_str());
    Player* player = ObjectAccessor::GetPlayer(*_player, guid);
    if (!player)
        return;

    if (!GetPlayer()->IsWithinDistInMap(player, INSPECT_DISTANCE, false))
        return;

    if (GetPlayer()->IsValidAttackTarget(player))
        return;

    player->SendRespondInspectAchievements(_player);
}

void WorldSession::HandleWorldStateUITimerUpdate(WorldPacket& /*recvData*/)
{
    // empty opcode
    TC_LOG_DEBUG("network", "WORLD: CMSG_WORLD_STATE_UI_TIMER_UPDATE");

    WorldPacket data(SMSG_WORLD_STATE_UI_TIMER_UPDATE, 4);
    data << uint32(GameTime::GetGameTime());
    SendPacket(&data);
}

void WorldSession::HandleReadyForAccountDataTimes(WorldPacket& /*recvData*/)
{
    // empty opcode
    TC_LOG_DEBUG("network", "WORLD: CMSG_READY_FOR_ACCOUNT_DATA_TIMES");

    SendAccountDataTimes(GLOBAL_CACHE_MASK);
}

void WorldSession::SendSetPhaseShift(uint32 PhaseShift)
{
    WorldPacket data(SMSG_SET_PHASE_SHIFT, 4);
    data << uint32(PhaseShift);
    SendPacket(&data);
}
// Battlefield and Battleground
void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_AREA_SPIRIT_HEALER_QUERY");

    Battleground* bg = _player->GetBattleground();

    ObjectGuid guid;
    recvData >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->IsSpiritService())                            // it's not spirit service
        return;

    if (bg)
        sBattlegroundMgr->SendAreaSpiritHealerQueryOpcode(_player, bg, guid);

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
        bf->SendAreaSpiritHealerQueryOpcode(_player, guid);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_AREA_SPIRIT_HEALER_QUEUE");

    Battleground* bg = _player->GetBattleground();

    ObjectGuid guid;
    recvData >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->IsSpiritService())                            // it's not spirit service
        return;

    if (bg)
        bg->AddPlayerToResurrectQueue(guid, _player->GetGUID());

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
        bf->AddPlayerToResurrectQueue(guid, _player->GetGUID());
}

void WorldSession::HandleHearthAndResurrect(WorldPacket& /*recvData*/)
{
    if (_player->IsInFlight())
        return;

    if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(_player->GetZoneId()))
    {
        bf->PlayerAskToLeave(_player);
        return;
    }

    AreaTableEntry const* atEntry = sAreaTableStore.LookupEntry(_player->GetAreaId());
    if (!atEntry || !(atEntry->flags & AREA_FLAG_WINTERGRASP_2))
        return;

    _player->BuildPlayerRepop();
    _player->ResurrectPlayer(1.0f);
    _player->TeleportTo(_player->m_homebindMapId, _player->m_homebindX, _player->m_homebindY, _player->m_homebindZ, _player->GetOrientation());
}

void WorldSession::HandleInstanceLockResponse(WorldPacket& recvPacket)
{
    uint8 accept;
    recvPacket >> accept;

    if (!_player->HasPendingBind())
    {
        TC_LOG_INFO("network", "InstanceLockResponse: Player %s (guid %u) tried to bind himself/teleport to graveyard without a pending bind!",
            _player->GetName().c_str(), _player->GetGUID().GetCounter());
        return;
    }

    if (accept)
        _player->BindToInstance();
    else
        _player->RepopAtGraveyard();

    _player->SetPendingBind(0, 0);
}

void WorldSession::HandleUpdateMissileTrajectory(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_UPDATE_MISSILE_TRAJECTORY");

    ObjectGuid guid;
    uint32 spellId;
    float elevation, speed;
    TaggedPosition<Position::XYZ> firePos;
    TaggedPosition<Position::XYZ> impactPos;
    uint8 moveStop;

    recvPacket >> guid >> spellId >> elevation >> speed;
    recvPacket >> firePos;
    recvPacket >> impactPos;
    recvPacket >> moveStop;

    Unit* caster = ObjectAccessor::GetUnit(*_player, guid);
    Spell* spell = caster ? caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) : nullptr;
    if (!spell || spell->m_spellInfo->Id != spellId || !spell->m_targets.HasDst() || !spell->m_targets.HasSrc())
    {
        recvPacket.rfinish();
        return;
    }

    spell->m_targets.ModSrc(firePos);
    spell->m_targets.ModDst(impactPos);

    spell->m_targets.SetElevation(elevation);
    spell->m_targets.SetSpeed(speed);

    if (moveStop)
    {
        uint32 opcode;
        recvPacket >> opcode;
        recvPacket.SetOpcode(opcode);
        HandleMovementOpcodes(recvPacket);
    }
}
