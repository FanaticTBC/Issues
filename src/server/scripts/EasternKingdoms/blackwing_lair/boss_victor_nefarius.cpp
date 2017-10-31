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
SDName: Boss_Victor_Nefarius
SD%Complete: 75
SDComment: Missing some text, Vael beginning event, and spawns Nef in wrong place
SDCategory: Blackwing Lair
EndScriptData */



#define SAY_GAMESBEGIN_1        -1469004
#define SAY_GAMESBEGIN_2        -1469005
#define SAY_VAEL_INTRO          -1469006                    //when he corrupts Vaelastrasz

#define GOSSIP_ITEM_1           "I've made no mistakes."
#define GOSSIP_ITEM_2           "You have lost your mind, Nefarius. You speak in riddles."
#define GOSSIP_ITEM_3           "Please do."

#define CREATURE_BRONZE_DRAKANOID       14263
#define CREATURE_BLUE_DRAKANOID         14261
#define CREATURE_RED_DRAKANOID          14264
#define CREATURE_GREEN_DRAKANOID        14262
#define CREATURE_BLACK_DRAKANOID        14265

#define CREATURE_CHROMATIC_DRAKANOID    14302
#define CREATURE_NEFARIAN               11583

#define ADD_X1 -7591.151855
#define ADD_X2 -7514.598633
#define ADD_Y1 -1204.051880
#define ADD_Y2 -1150.448853
#define ADD_Z1 476.800476
#define ADD_Z2 476.796570

#define NEF_X   -7445
#define NEF_Y   -1332
#define NEF_Z   536

#define HIDE_X  -7592
#define HIDE_Y  -1264
#define HIDE_Z  481

#define SPELL_SHADOWBOLT        21077
#define SPELL_FEAR              26070

//This script is complicated
//Instead of morphing Victor Nefarius we will have him control phase 1
//And then have him spawn "Nefarian" for phase 2
//When phase 2 starts Victor Nefarius will go into hiding and stop attacking
//If Nefarian despawns because he killed the players then this guy will EnterEvadeMode
//and allow players to start the event over
//If nefarian dies then he will kill himself then he will kill himself in his hiding place
//To prevent players from doing the event twice


class boss_victor_nefarius : public CreatureScript
{
public:
    boss_victor_nefarius() : CreatureScript("boss_victor_nefarius")
    { }

    class boss_victor_nefariusAI : public ScriptedAI
    {
        public:
        boss_victor_nefariusAI(Creature *c) : ScriptedAI(c)
        {
            NefarianGUID = 0;
            srand(time(nullptr));
            switch (rand()%20)
            {
                case 0:
                    DrakType1 = CREATURE_BRONZE_DRAKANOID;
                    DrakType2 = CREATURE_BLUE_DRAKANOID;
                    break;
                case 1:
                    DrakType1 = CREATURE_BRONZE_DRAKANOID;
                    DrakType2 = CREATURE_RED_DRAKANOID;
                    break;
                case 2:
                    DrakType1 = CREATURE_BRONZE_DRAKANOID;
                    DrakType2 = CREATURE_GREEN_DRAKANOID;
                    break;
                case 3:
                    DrakType1 = CREATURE_BRONZE_DRAKANOID;
                    DrakType2 = CREATURE_BLACK_DRAKANOID;
                    break;
                case 4:
                    DrakType1 = CREATURE_BLUE_DRAKANOID;
                    DrakType2 = CREATURE_BRONZE_DRAKANOID;
                    break;
                case 5:
                    DrakType1 = CREATURE_BLUE_DRAKANOID;
                    DrakType2 = CREATURE_RED_DRAKANOID;
                    break;
                case 6:
                    DrakType1 = CREATURE_BLUE_DRAKANOID;
                    DrakType2 = CREATURE_GREEN_DRAKANOID;
                    break;
                case 7:
                    DrakType1 = CREATURE_BLUE_DRAKANOID;
                    DrakType2 = CREATURE_BLACK_DRAKANOID;
                    break;
                case 8:
                    DrakType1 = CREATURE_RED_DRAKANOID;
                    DrakType2 = CREATURE_BRONZE_DRAKANOID;
                    break;
                case 9:
                    DrakType1 = CREATURE_RED_DRAKANOID;
                    DrakType2 = CREATURE_BLUE_DRAKANOID;
                    break;
                case 10:
                    DrakType1 = CREATURE_RED_DRAKANOID;
                    DrakType2 = CREATURE_GREEN_DRAKANOID;
                    break;
                case 11:
                    DrakType1 = CREATURE_RED_DRAKANOID;
                    DrakType2 = CREATURE_BLACK_DRAKANOID;
                    break;
                case 12:
                    DrakType1 = CREATURE_GREEN_DRAKANOID;
                    DrakType2 = CREATURE_BRONZE_DRAKANOID;
                    break;
                case 13:
                    DrakType1 = CREATURE_GREEN_DRAKANOID;
                    DrakType2 = CREATURE_BLUE_DRAKANOID;
                    break;
                case 14:
                    DrakType1 = CREATURE_GREEN_DRAKANOID;
                    DrakType2 = CREATURE_RED_DRAKANOID;
                    break;
                case 15:
                    DrakType1 = CREATURE_GREEN_DRAKANOID;
                    DrakType2 = CREATURE_BLACK_DRAKANOID;
                    break;
                case 16:
                    DrakType1 = CREATURE_BLACK_DRAKANOID;
                    DrakType2 = CREATURE_BRONZE_DRAKANOID;
                    break;
                case 17:
                    DrakType1 = CREATURE_BLACK_DRAKANOID;
                    DrakType2 = CREATURE_BLUE_DRAKANOID;
                    break;
                case 18:
                    DrakType1 = CREATURE_BLACK_DRAKANOID;
                    DrakType2 = CREATURE_GREEN_DRAKANOID;
                    break;
                case 19:
                    DrakType1 = CREATURE_BLACK_DRAKANOID;
                    DrakType2 = CREATURE_RED_DRAKANOID;
                    break;
            }
        }
    
        uint32 SpawnedAdds;
        uint32 AddSpawnTimer;
        uint32 ShadowBoltTimer;
        uint32 FearTimer;
        uint32 MindControlTimer;
        uint32 ResetTimer;
        uint32 DrakType1;
        uint32 DrakType2;
        uint64 NefarianGUID;
        uint32 NefCheckTime;
    
        void Reset()
        override {
            SpawnedAdds = 0;
            AddSpawnTimer = 10000;
            ShadowBoltTimer = 5000;
            FearTimer = 8000;
            ResetTimer = 900000;                                //On official it takes him 15 minutes(900 seconds) to reset. We are only doing 1 minute to make testing easier
            NefarianGUID = 0;
            NefCheckTime = 2000;
    
            me->SetUInt32Value(UNIT_NPC_FLAGS,1);
            me->SetFaction(FACTION_FRIENDLY);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    
        void BeginEvent(Player* target)
        {
            DoScriptText(SAY_GAMESBEGIN_2, me);
    
            //Trinity::Singleton<MapManager>::Instance().GetMap(me->GetMapId(), me)->GetPlayers().begin();
            /*
            list <Player*>::iterator i = sMapMgr->CreateMap(me->GetMapId(), me)->GetPlayers().begin();
    
            for (i = sMapMgr->CreateMap(me->GetMapId(), me)->GetPlayers().begin(); i != sMapMgr->CreateMap(me->GetMapId(), me)->GetPlayers().end(); ++i)
            {
            AttackStart((*i));
            }
            */
            me->SetUInt32Value(UNIT_NPC_FLAGS,0);
            me->SetFaction(103);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            AttackStart(target);
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void MoveInLineOfSight(Unit *who)
        override {
            //We simply use this function to find players until we can use Map->GetPlayers()
    
            if (who && who->GetTypeId() == TYPEID_PLAYER && me->IsHostileTo(who))
            {
                //Add them to our threat list
                me->GetThreatManager().AddThreat(who,0.0f);
            }
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim() )
                return;
    
            //Only do this if we haven't spawned nef yet
            if (SpawnedAdds < 42)
            {
                //ShadowBoltTimer
                if (ShadowBoltTimer < diff)
                {
                    Unit* target = nullptr;
                    target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if (target)
                        DoCast(target,SPELL_SHADOWBOLT);
    
                    ShadowBoltTimer = 3000 + (rand()%7000);
                }else ShadowBoltTimer -= diff;
    
                //FearTimer
                if (FearTimer < diff)
                {
                    Unit* target = nullptr;
                    target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if (target)
                        DoCast(target,SPELL_FEAR);
    
                    FearTimer = 10000 + (rand()%10000);
                }else FearTimer -= diff;
    
                //Add spawning mechanism
                if (AddSpawnTimer < diff)
                {
                    //Spawn 2 random types of creatures at the 2 locations
                    uint32 CreatureID;
                    Creature* Spawned = nullptr;
                    Unit* target = nullptr;
    
                    //1 in 3 chance it will be a chromatic
                    if (rand()%3 == 0)
                        CreatureID = CREATURE_CHROMATIC_DRAKANOID;
                    else CreatureID = DrakType1;
    
                    SpawnedAdds++;
    
                    //Spawn creature and force it to start attacking a random target
                    Spawned = me->SummonCreature(CreatureID,ADD_X1,ADD_Y1,ADD_Z1,5.000,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if (target && Spawned)
                    {
                        Spawned->AI()->AttackStart(target);
                        Spawned->SetFaction(FACTION_DRAGONFLIGHT_BLACK);
                    }
    
                    //1 in 3 chance it will be a chromatic
                    if (rand()%3 == 0)
                        CreatureID = CREATURE_CHROMATIC_DRAKANOID;
                    else CreatureID = DrakType2;
    
                    SpawnedAdds++;
    
                    target = nullptr;
                    Spawned = nullptr;
                    Spawned = me->SummonCreature(CreatureID,ADD_X2,ADD_Y2,ADD_Z2,5.000,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if (target && Spawned)
                    {
                        Spawned->AI()->AttackStart(target);
                        Spawned->SetFaction(FACTION_DRAGONFLIGHT_BLACK);
                    }
    
                    //Begin phase 2 by spawning Nefarian and what not
                    if (SpawnedAdds >= 42)
                    {
                        //Teleport Victor Nefarius way out of the map
                        //sMapMgr->CreateMap(me->GetMapId(), me)->CreatureRelocation(me,0,0,-5000,0);
    
                        //Inturrupt any spell casting
                        me->InterruptNonMeleeSpells(false);
    
                        //Root self
                        DoCast(me,33356);
    
                        //Make super invis
                        DoCast(me,8149);
    
                        //Teleport self to a hiding spot (this causes errors in the Trinity log but no real issues)
                        DoTeleportTo(HIDE_X,HIDE_Y,HIDE_Z);
                        me->AddUnitState(UNIT_STATE_FLEEING);
    
                        //Spawn nef and have him attack a random target
                        Creature* Nefarian = nullptr;
                        Nefarian = me->SummonCreature(CREATURE_NEFARIAN,NEF_X,NEF_Y,NEF_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,120000);
                        target = nullptr;
                        target = SelectTarget(SELECT_TARGET_RANDOM,0);
                        if (target && Nefarian)
                        {
                            Nefarian->AI()->AttackStart(target);
                            Nefarian->SetFaction(FACTION_DRAGONFLIGHT_BLACK);
                            NefarianGUID = Nefarian->GetGUID();
                        }
                        else error_log("TSCR: Blackwing Lair: Unable to spawn nefarian properly.");
                    }
    
                    AddSpawnTimer = 4000;
                }else AddSpawnTimer -= diff;
            }
            else if (NefarianGUID)
            {
                if (NefCheckTime < diff)
                {
                    Unit* Nefarian = nullptr;
                    Nefarian = ObjectAccessor::GetUnit((*me),NefarianGUID);
    
                    //If nef is dead then we die to so the players get out of combat
                    //and cannot repeat the event
                    if (!Nefarian || !Nefarian->IsAlive())
                    {
                        NefarianGUID = 0;
                        me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                    }
    
                    NefCheckTime = 2000;
                }else NefCheckTime -= diff;
            }
        }

        virtual bool GossipHello(Player* player) override
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU_TEXTID(7134,me->GetGUID());
            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU_TEXTID(7198, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    player->SEND_GOSSIP_MENU_TEXTID(7199, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    player->CLOSE_GOSSIP_MENU();
                    DoScriptText(SAY_GAMESBEGIN_1, me);
                    ((boss_victor_nefarius::boss_victor_nefariusAI*)me->AI())->BeginEvent(player);
                    break;
            }
            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_victor_nefariusAI(creature);
    }
};




void AddSC_boss_victor_nefarius()
{

    new boss_victor_nefarius();
}
