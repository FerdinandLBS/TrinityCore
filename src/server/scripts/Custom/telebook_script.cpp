#include "Define.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Item.h"
#include "Player.h"
#include "GossipDef.h"
#include "WorldSession.h"
#include "ScriptedGossip.h"
// .. more includes
#pragma execution_character_set("utf-8")

#define TRANS_TO_BFC "传送暴风城"
#define TRANS_TO_TLB "传送铁炉堡"
#define TRANS_TO_DNSS "传送达纳苏斯"
#define TRANS_TO_ASD "传送埃索达"
#define TRANS_TO_AGRM "传送奥格瑞玛"
#define TRANS_TO_LTY "传送雷霆崖"
#define TRANS_TO_YAC "传送幽暗城"
#define TRANS_TO_YYC "传送银月城"

#include <Windows.h>

#include <iostream>
#include <string>

class telebook_script_class : ItemScript
{
public:
    telebook_script_class() : ItemScript("telebook_script") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        player->PlayerTalkClass->ClearMenus();
        this->OnUseGossipMenuSend(player, item);
        player->StopCastingCharm();
        return true;
    }

    void OnUseGossipMenuSend(Player* player, Item* item) {
        if (player->IsInFlight())
        {
            player->GetSession()->SendAreaTriggerMessage("无法在飞行中使用"); return;
        }
        else if (player->IsMounted())
        {
            player->GetSession()->SendAreaTriggerMessage("无法在坐骑中使用"); return;
        }
        else if (player->IsInCombat())
        {
            player->GetSession()->SendAreaTriggerMessage("无法在战斗中使用"); return;
        }

        if (!player->isDead())
        {
            PlayerMenu* menu = player->PlayerTalkClass;
            menu->ClearMenus();
            if (player->GetTeam() == ALLIANCE)//联盟
            {
                AddGossipItemFor(player, 2, (TRANS_TO_BFC), GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+1);
                AddGossipItemFor(player, 2, (TRANS_TO_TLB), GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+2);
                AddGossipItemFor(player, 2, TRANS_TO_DNSS, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+3);
                AddGossipItemFor(player, 2, TRANS_TO_ASD, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+4);
            }
            else//部落
            {
                AddGossipItemFor(player, 2, TRANS_TO_AGRM, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+5);
                AddGossipItemFor(player, 2, TRANS_TO_LTY, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+6);
                AddGossipItemFor(player, 2, TRANS_TO_YAC, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+7);
                AddGossipItemFor(player, 2, TRANS_TO_YYC, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF+8);
            }
            //中立城市

            AddGossipItemFor(player, 2, "传送棘齿城", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 9);
            AddGossipItemFor(player, 2, "传送藏宝海湾", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 10);
            AddGossipItemFor(player, 2, "传送加基森", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 11);

            if (player->getLevel()>=58)
                AddGossipItemFor(player, 2, "传送沙塔斯", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 12);

            if (player->getLevel()>=69)
                AddGossipItemFor(player, 2, "传送达拉然", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 13);

            AddGossipItemFor(player, 2, "GM岛", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 14);
            AddGossipItemFor(player, 1, "5人副本传送", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 15);
            AddGossipItemFor(player, 1, "团队副本传送", GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF + 16);

            SendGossipMenuFor(player, 2, item->GetGUID());
        }
    }
};

void AddSC_telebook_script()
{
    new telebook_script_class();
}
