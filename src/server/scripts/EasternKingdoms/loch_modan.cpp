/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Loch_Modan
SD%Complete: 100
SDComment: Quest support: 309, 3181
SDCategory: Loch Modan
EndScriptData */

/* ContentData
npc_mountaineer_pebblebitty
npc_miran
EndContentData */


#include "EscortAI.h"

/*######
## npc_mountaineer_pebblebitty
######*/

#define GOSSIP_MP "Open the gate please, i need to get to Searing Gorge"

#define GOSSIP_MP1 "But i need to get there, now open the gate!"
#define GOSSIP_MP2 "Ok, so what is this other way?"
#define GOSSIP_MP3 "Doesn't matter, i'm invulnerable."
#define GOSSIP_MP4 "Yes..."
#define GOSSIP_MP5 "Ok, i'll try to remember that."
#define GOSSIP_MP6 "A key? Ok!"

class npc_mountaineer_pebblebitty : public CreatureScript
{
public:
    npc_mountaineer_pebblebitty() : CreatureScript("npc_mountaineer_pebblebitty")
    { }

    class npc_mountaineer_pebblebittyAI : public ScriptedAI
    {
    public:
        npc_mountaineer_pebblebittyAI(Creature* creature) : ScriptedAI(creature)
        {}


        virtual bool GossipHello(Player* pPlayer) override
        {
            if (me->IsQuestGiver())
                pPlayer->PrepareQuestMenu( me->GetGUID() );

            if (!pPlayer->GetQuestRewardStatus(3181) == 1)
                pPlayer->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            SEND_PREPARED_GOSSIP_MENU(pPlayer, me);

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->SEND_GOSSIP_MENU_TEXTID(1833, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->SEND_GOSSIP_MENU_TEXTID(1834, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                    player->SEND_GOSSIP_MENU_TEXTID(1835, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+4:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                    player->SEND_GOSSIP_MENU_TEXTID(1836, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+5:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                    player->SEND_GOSSIP_MENU_TEXTID(1837, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+6:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, GOSSIP_MP6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                    player->SEND_GOSSIP_MENU_TEXTID(1838, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+7:
                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mountaineer_pebblebittyAI(creature);
    }
};



/*#########
##npc_miran
#########*/

enum eMiran
{
    MIRAN_SAY_AMBUSH_ONE        = -1780127,
    MIRAN_SAY_AMBUSH_TWO        = -1780128, 
    DARK_IRON_RAIDER_SAY_AMBUSH = -1780129, 
    MIRAN_SAY_QUEST_END         = -1780130,

    QUEST_PROTECTING_THE_SHIPMENT  = 309,
    DARK_IRON_RAIDER               = 2149

};


class npc_miran : public CreatureScript
{
public:
    npc_miran() : CreatureScript("npc_miran")
    { }

    class npc_miranAI : public EscortAI
    {
        public:
        npc_miranAI(Creature* pCreature) : EscortAI(pCreature) { }    
    
        void Reset() override {}
        
        void EnterCombat(Unit *pWho) override {}
    
        void WaypointReached(uint32 uiPointId, uint32 pathID)
        override {
            Player* pPlayer = GetPlayerForEscort();
            if (!pPlayer)
                return;
    
            switch(uiPointId)
            {
            case 8:
                DoScriptText(MIRAN_SAY_AMBUSH_ONE, me);
                me->SummonCreature(DARK_IRON_RAIDER, -5697.27,-3736.36,318.54, 2.02, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
                me->SummonCreature(DARK_IRON_RAIDER, -5697.27,-3736.36,318.54, 2.07, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
                if (Unit* scoff = me->FindNearestCreature(DARK_IRON_RAIDER, 30))
                    DoScriptText(DARK_IRON_RAIDER_SAY_AMBUSH, scoff);
                DoScriptText(MIRAN_SAY_AMBUSH_TWO, me);
            break;
            case 11:
                DoScriptText(MIRAN_SAY_QUEST_END, me);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_PROTECTING_THE_SHIPMENT, me);
            break;
            }
        }
    
        void JustSummoned(Creature* pSummoned)
        override {
            pSummoned->AI()->AttackStart(me);
        }

        virtual void QuestAccept(Player* pPlayer, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_PROTECTING_THE_SHIPMENT)
            {        
                me->SetFaction(231);

                ((EscortAI*)(me->AI()))->Start(true, false, pPlayer->GetGUID(), quest);
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_miranAI(creature);
    }
};


/*######
## AddSC
######*/

void AddSC_loch_modan()
{

    new npc_mountaineer_pebblebitty();
    
    new npc_miran();
}
