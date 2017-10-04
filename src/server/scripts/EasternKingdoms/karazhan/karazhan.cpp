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
SDName: Karazhan
SD%Complete: 100
SDComment: Support for Barnes (Opera controller) and Berthold (Doorman), Support for Quest 9645.
SDCategory: Karazhan
EndScriptData */

/* ContentData
npc_barnes
npc_berthold
npc_image_of_medivh
npc_archmage_leryda
go_sealed_tome
woefulhealer
npc_arcane_anomaly
EndContentData */


#include "def_karazhan.h"
#include "EscortAI.h"
#include "Chat.h"
#include "ObjectMgr.h"

/*######
# npc_barnesAI
######*/

#define GOSSIP_READY        "I'm not an actor."

#define SAY_READY           "Splendid, I'm going to get the audience ready. Break a leg!"
#define SAY_OZ_INTRO1       "Finally, everything is in place. Are you ready for your big stage debut?"
#define OZ_GOSSIP1          "I'm not an actor."
#define SAY_OZ_INTRO2       "Don't worry, you'll be fine. You look like a natural!"
#define OZ_GOSSIP2          "Ok, I'll give it a try, then."

#define SAY_RAJ_INTRO1      "The romantic plays are really tough, but you'll do better this time. You have TALENT. Ready?"
#define RAJ_GOSSIP1         "I've never been more ready."

struct Dialogue
{
    int32 textid;
    uint32 timer;
};

static Dialogue OzDialogue[]=
{
    {-1532103, 6000},
    {-1532104, 18000},
    {-1532105, 9000},
    {-1532106, 15000}
};

static Dialogue HoodDialogue[]=
{
    {-1532107, 6000},
    {-1532108, 10000},
    {-1532109, 14000},
    {-1532110, 15000}
};

static Dialogue RAJDialogue[]=
{
    {-1532111, 5000},
    {-1532112, 7000},
    {-1532113, 14000},
    {-1532114, 14000}
};

// Entries and spawn locations for creatures in Oz event
float Spawns[6][2]=
{
    {17535, -10896},                                        // Dorothee
    {17546, -10891},                                        // Roar
    {17547, -10884},                                        // Tinhead
    {17543, -10902},                                        // Strawman
    {17603, -10892},                                        // Grandmother
    {17534, -10900},                                        // Julianne
};

float StageLocations[6][2]=
{
    {-10866.711f, -1779.816f},                                // Open door, begin walking (0)
    {-10894.917f, -1775.467f},                                // (1)
    {-10896.044f, -1782.619f},                                // Begin Speech after this (2)
    {-10894.917f, -1775.467f},                                // Resume walking (back to spawn point now) after speech (3)
    {-10866.711f, -1779.816f},                                // (4)
    {-10866.700f, -1781.030f}                                 // Summon mobs, open curtains, close door (5)
};

#define CREATURE_SPOTLIGHT  19525

#define SPELL_SPOTLIGHT     25824
#define SPELL_TUXEDO        32616

#define SPAWN_Z             90.5
#define SPAWN_Y             -1758
#define SPAWN_O             4.738


class npc_barnes : public CreatureScript
{
public:
    npc_barnes() : CreatureScript("npc_barnes")
    { }

    class npc_barnesAI : public npc_escortAI
    {
        public:
        npc_barnesAI(Creature* c) : npc_escortAI(c)
        {
            RaidWiped = false;
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript* pInstance;
    
        uint64 SpotlightGUID;
    
        uint32 TalkCount;
        uint32 TalkTimer;
        uint32 CurtainTimer;
        uint32 WipeTimer;
        uint32 Event;
    
        bool PerformanceReady;
        bool RaidWiped;
        bool IsTalking;
    
        void Reset()
        override {
            TalkCount = 0;
            TalkTimer = 2000;
            CurtainTimer = 5000;
            WipeTimer = 5000;
    
            PerformanceReady = false;
            IsTalking = false;
    
            if(pInstance)
            {
                pInstance->SetData(DATA_OPERA_EVENT, NOT_STARTED);
    
                Event = pInstance->GetData(DATA_OPERA_PERFORMANCE);
    
                 if (GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_STAGEDOORLEFT)))
                 {
                    if (RaidWiped)
                        Door->UseDoorOrButton();
                    else
                        Door->ResetDoorOrButton();
                 }
    
                 if (GameObject* Curtain = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_CURTAINS)))
                    Curtain->ResetDoorOrButton();
            }
        }
    
        void EnterCombat(Unit* who) override {}
    
        void WaypointReached(uint32 i)
        override {
            switch(i)
            {
                case 2:
                    RemoveEscortState(STATE_ESCORT_ESCORTING);
                    TalkCount = 0;
                    IsTalking = true;
    
                    float x,y,z;
                    me->GetPosition(x, y, z);
                    if (Creature* Spotlight = me->SummonCreature(CREATURE_SPOTLIGHT, x, y, z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 50000))
                    {
                        Spotlight->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Spotlight->CastSpell(Spotlight, SPELL_SPOTLIGHT, false);
                        SpotlightGUID = Spotlight->GetGUID();
                    }
                    break;
    
                case 5:
                    if(pInstance)
                    {
                        if (GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_STAGEDOORLEFT)))
                            Door->ResetDoorOrButton();
                    }
                    RemoveEscortState(STATE_ESCORT_ESCORTING);
                    PerformanceReady = true;
                    break;
            }
        }
    
        void Talk(uint32 count)
        {
            int32 text = 0;
    
            switch(Event)
            {
                case EVENT_OZ:
                    if (OzDialogue[count].textid)
                         text = OzDialogue[count].textid;
                    if(OzDialogue[count].timer)
                        TalkTimer = OzDialogue[count].timer;
                    break;
    
                case EVENT_HOOD:
                    if (HoodDialogue[count].textid)
                        text = HoodDialogue[count].textid;
                    if(HoodDialogue[count].timer)
                        TalkTimer = HoodDialogue[count].timer;
                    break;
    
                case EVENT_RAJ:
                     if (RAJDialogue[count].textid)
                         text = RAJDialogue[count].textid;
                    if(RAJDialogue[count].timer)
                        TalkTimer = RAJDialogue[count].timer;
                    break;
            }
    
            if(text)
                 DoScriptText(text, me);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            npc_escortAI::UpdateAI(diff);
    
            if(IsTalking)
            {
                if(TalkTimer < diff)
                {
                    if(TalkCount > 3)
                    {
                        if (Unit* Spotlight = ObjectAccessor::GetUnit((*me), SpotlightGUID))
                        {
                            Spotlight->RemoveAllAuras();
                            Spotlight->SetVisible(false);
                        }
    
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                        IsTalking = false;
                        AddEscortState(STATE_ESCORT_ESCORTING);
                        return;
                    }
    
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                    Talk(TalkCount);
                    ++TalkCount;
                }else TalkTimer -= diff;
            }
    
            if(PerformanceReady)
            {
                if(CurtainTimer)
                {
                    if(CurtainTimer <= diff)
                    {
                        PrepareEncounter();
    
                        if(!pInstance)
                            return;
    
                        if (GameObject* Curtain = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_CURTAINS)))
                            Curtain->UseDoorOrButton();
    
                        CurtainTimer = 0;
                    }else CurtainTimer -= diff;
                }
    
                if(!RaidWiped)
                {
                    if(WipeTimer < diff)
                    {
                        Map *map = me->GetMap();
                        if(!map->IsDungeon()) return;
    
                        Map::PlayerList const &PlayerList = map->GetPlayers();
                        if(PlayerList.isEmpty())
                            return;
    
                        RaidWiped = true;
                        for(const auto & i : PlayerList)
                        {
                            if (i.GetSource()->IsAlive() && !i.GetSource()->IsGameMaster())
                            {
                                RaidWiped = false;
                                break;
                            }
                        }
    
                        if(RaidWiped)
                        {
                            RaidWiped = true;
                            EnterEvadeMode();
                            return;
                        }
    
                        WipeTimer = 15000;
                    }else WipeTimer -= diff;
                }
    
            }
    
            if(!UpdateVictim())
                return;
    
            DoMeleeAttackIfReady();
        }
    
        void StartEvent()
        {
            if(!pInstance)
                return;
    
            pInstance->SetData(DATA_OPERA_EVENT, IN_PROGRESS);
    
            if (GameObject* Door = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_GAMEOBJECT_STAGEDOORLEFT)))
                Door->UseDoorOrButton();
    
            me->CastSpell(me, SPELL_TUXEDO, true);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    
            Start(false, false, false);
        }
    
        void PrepareEncounter()
        {
            uint8 index = 0;
            uint8 count = 0;
            switch(Event)
            {
                case EVENT_OZ:
                    index = 0;
                    count = 4;
                    break;
    
                case EVENT_HOOD:
                    index = 4;
                    count = index+1;
                    break;
    
                case EVENT_RAJ:
                    index = 5;
                    count = index+1;
                    break;
            }
    
            for( ; index < count; ++index)
            {
                uint32 entry = ((uint32)Spawns[index][0]);
                float PosX = Spawns[index][1];
                if (Creature* pCreature = me->SummonCreature(entry, PosX, SPAWN_Y, SPAWN_Z, SPAWN_O, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                {
                                                                // In case database has bad flags
                    pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
            }
    
            CurtainTimer = 10000;
            PerformanceReady = true;
            RaidWiped = false;
        }

        virtual bool GossipHello(Player* player) override
        {
            // Check for death of Moroes.
            if (pInstance && (pInstance->GetData(DATA_OPERA_EVENT) < DONE))
            {
                if(pInstance->GetData(DATA_MOROES_EVENT) >= DONE)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, OZ_GOSSIP1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                    if(!((npc_barnes::npc_barnesAI*)me->AI())->RaidWiped)
                        player->SEND_GOSSIP_MENU_TEXTID(8970, me->GetGUID()); //try again text
                    else
                        player->SEND_GOSSIP_MENU_TEXTID(8975, me->GetGUID());
                }else {
                    player->SEND_GOSSIP_MENU_TEXTID(8978, me->GetGUID()); //Someone should take care of Moroes
                }
            }

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch(action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, OZ_GOSSIP2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU_TEXTID(8971, me->GetGUID());
                    break;

                case GOSSIP_ACTION_INFO_DEF+2:
                    player->CLOSE_GOSSIP_MENU();
                    ((npc_barnes::npc_barnesAI*)me->AI())->StartEvent();
                    break;
            }

            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_barnesAI(creature);
    }
};




/*###
# npc_hastings
####*/

#define HASTINGS_TEXT_HELLO             554
#define HASTINGS_TEXT_MENU1             555
#define HASTINGS_TEXT_MENU2             556
#define HASTINGS_GOSSIP_ITEM_1          557
#define HASTINGS_GOSSIP_ITEM_2          19999

class npc_hastings : public CreatureScript
{
public:
    npc_hastings() : CreatureScript("npc_hastings")
    { }

    class npc_hastingsAI : public ScriptedAI
    {
    public:
        npc_hastingsAI(Creature* creature) : ScriptedAI(creature)
        {}


        virtual bool GossipHello(Player* player) override
        {
            player->ADD_GOSSIP_ITEM_TEXTID(0, HASTINGS_GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            player->SEND_GOSSIP_MENU_TEXTID(HASTINGS_TEXT_HELLO,me->GetGUID());
            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:     
                    player->ADD_GOSSIP_ITEM_TEXTID(0, HASTINGS_GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU_TEXTID(HASTINGS_TEXT_MENU1,me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:     
                    player->SEND_GOSSIP_MENU_TEXTID(HASTINGS_TEXT_MENU2,me->GetGUID()); 
                    break;
            }

            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_hastingsAI(creature);
    }
};



/*###
# npc_calliard
####*/

#define CALLIARD_TEXT_HELLO             454
#define CALLIARD_TEXT_MENU1             455
#define CALLIARD_GOSSIP_ITEM_1          456

class npc_calliard : public CreatureScript
{
public:
    npc_calliard() : CreatureScript("npc_calliard")
    { }

    class npc_calliardAI : public ScriptedAI
    {
    public:
        npc_calliardAI(Creature* creature) : ScriptedAI(creature)
        {}


        virtual bool GossipHello(Player* player) override
        {
            player->ADD_GOSSIP_ITEM_TEXTID(0, CALLIARD_GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    
            player->SEND_GOSSIP_MENU_TEXTID(CALLIARD_TEXT_HELLO,me->GetGUID());

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            if(action == GOSSIP_ACTION_INFO_DEF + 1)
                player->SEND_GOSSIP_MENU_TEXTID(CALLIARD_TEXT_MENU1, me->GetGUID());

            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_calliardAI(creature);
    }
};



/*###
# npc_image_of_medivh
####*/

//TODO translate
#define SAY_DIALOG_MEDIVH_1         "Vous avez attiré mon attention, dragon. Mais on ne m'effraie pas aussi facilement que les villageois d'en-dessous."
#define SAY_DIALOG_ARCANAGOS_2      "Votre jeu avec la force des arcanes est allé trop loin, Medivh. Vous avez attiré l'attention de puissances au-delà de votre compréhension. Vous devez quitter Karazhan sur le champ !!"
#define SAY_DIALOG_MEDIVH_3         "Vous osez me défier dans ma propre demeure ? Votre arrogance est stupéfiante, même pour un dragon !"
#define SAY_DIALOG_ARCANAGOS_4      "Une puissance obscure cherche à vous utiliser, Medivh ! Si vous restez, de sombres jours vont suivre. Il faut vous dépêcher, nous n'avons pas beaucoup de temps !"
#define SAY_DIALOG_MEDIVH_5         "Je ne vois pas de quoi vous voulez parler, dragon... mais je ne vais pas me laisser intimider par tant d'insolence. Je quitterai Karazhan quand je l'aurai décidé !"
#define SAY_DIALOG_ARCANAGOS_6      "Vous ne me laissez pas le choix. Je vous arrêterai par la force si vous ne voulez pas revenir à la raison."
#define EMOTE_DIALOG_MEDIVH_7       "commence à invoquer un sort d'une grande puissance, tissant sa propre essence avec la magie."
#define SAY_DIALOG_ARCANAGOS_8      "Qu'avez-vous fait, magicien ? Cela ne peut être... ! Je brûle de... à l'intérieur !"
#define SAY_DIALOG_MEDIVH_9         "Il n'aurait pas dû me mettre en colère. Je dois désormais aller... récupérer ma puissance..."

#define MOB_ARCANAGOS               17652
#define SPELL_FIRE_BALL             30967
#define SPELL_UBER_FIREBALL         30971
#define SPELL_CONFLAGRATION_BLAST   30977
#define SPELL_MANA_SHIELD           31635

static float MedivPos[4] = {-11161.49f,-1902.24f,91.48f,1.94f};
static float ArcanagosPos[4] = {-11169.75f,-1881.48f,95.39f,4.83f};


class npc_image_of_medivh : public CreatureScript
{
public:
    npc_image_of_medivh() : CreatureScript("npc_image_of_medivh")
    { }

    class npc_image_of_medivhAI : public ScriptedAI
    {
        public:
        npc_image_of_medivhAI(Creature* c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript *pInstance;
    
        uint64 ArcanagosGUID;
    
        uint32 YellTimer;
        uint32 Step;
        uint32 FireMedivhTimer;
        uint32 FireArcanagosTimer;
    
        bool EventStarted;
    
        void Reset()
        override {
            ArcanagosGUID = 0;
    
            if(pInstance && pInstance->GetData64(DATA_IMAGE_OF_MEDIVH_NIGHTBANE) == 0)
            {
                pInstance->SetData64(DATA_IMAGE_OF_MEDIVH_NIGHTBANE, me->GetGUID());
                (*me).GetMotionMaster()->MovePoint(1,MedivPos[0],MedivPos[1],MedivPos[2]);
                Step = 0;
            }else
            {
                me->DespawnOrUnsummon();
            }
        }
        void EnterCombat(Unit* who)override {}
    
        void MovementInform(uint32 type, uint32 id)
        override {
            if(type != POINT_MOTION_TYPE)
                return;
            if(id == 1)
            {
                StartEvent();
                me->SetOrientation(MedivPos[3]);
                me->SetOrientation(MedivPos[3]);
            }
        }
    
        void StartEvent()
        {
            Step = 1;
            EventStarted = true;
            Creature* Arcanagos = me->SummonCreature(MOB_ARCANAGOS,ArcanagosPos[0],ArcanagosPos[1],ArcanagosPos[2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,20000);
            if(!Arcanagos)
                return;
            ArcanagosGUID = Arcanagos->GetGUID();
            Arcanagos->SetDisableGravity(true);
            (*Arcanagos).GetMotionMaster()->MovePoint(0,ArcanagosPos[0],ArcanagosPos[1],ArcanagosPos[2]);
            Arcanagos->SetOrientation(ArcanagosPos[3]);
            me->SetOrientation(MedivPos[3]);
            YellTimer = 10000;
        }
    
    
        uint32 NextStep(uint32 step)
        {
            Unit* arca = ObjectAccessor::GetUnit((*me),ArcanagosGUID);
            Map *map = me->GetMap();
            switch(step)
            {
            case 1:
                me->Yell(SAY_DIALOG_MEDIVH_1,LANG_UNIVERSAL,nullptr);
                return 10000;
            case 2:
                if(arca)
                    (arca->ToCreature())->Yell(SAY_DIALOG_ARCANAGOS_2,LANG_UNIVERSAL,nullptr);
                return 20000;
            case 3:
                me->Yell(SAY_DIALOG_MEDIVH_3,LANG_UNIVERSAL,nullptr);
                return 10000;
            case 4:
                if(arca)
                    (arca->ToCreature())->Yell(SAY_DIALOG_ARCANAGOS_4, LANG_UNIVERSAL, nullptr);
                return 20000;
            case 5:
                me->Yell(SAY_DIALOG_MEDIVH_5, LANG_UNIVERSAL, nullptr);
                return 20000;
            case 6:
                if(arca)
                    (arca->ToCreature())->Yell(SAY_DIALOG_ARCANAGOS_6, LANG_UNIVERSAL, nullptr);
                return 10000;
            case 7:
                FireArcanagosTimer = 500;
                return 5000;
            case 8:
                FireMedivhTimer = 500;
                DoCast(me, SPELL_MANA_SHIELD);
                return 10000;
            case 9:
                me->TextEmote(EMOTE_DIALOG_MEDIVH_7, nullptr, false);
                return 10000;
            case 10:
                if(arca)
                    me->CastSpell(arca, SPELL_CONFLAGRATION_BLAST, false);
                return 1000;
            case 11:
                if(arca)
                    (arca->ToCreature())->Yell(SAY_DIALOG_ARCANAGOS_8, LANG_UNIVERSAL, nullptr);
                return 5000;
            case 12:
                arca->GetMotionMaster()->MovePoint(0, -11010.82,-1761.18, 156.47);
                arca->SetKeepActive(true);
                arca->InterruptNonMeleeSpells(true);
                arca->SetSpeedRate(MOVE_FLIGHT, 2.0f);
                return 10000;
            case 13:
                me->Yell(SAY_DIALOG_MEDIVH_9, LANG_UNIVERSAL, nullptr);
                return 10000;
            case 14:
                me->SetVisible(false);
                me->ClearInCombat();
    
                if(map->IsDungeon())
                {
                    InstanceMap::PlayerList const &PlayerList = ((InstanceMap*)map)->GetPlayers();
                    for (const auto & i : PlayerList)
                    {
                        if(i.GetSource()->IsAlive())
                        {
                            if(i.GetSource()->GetQuestStatus(9645) == QUEST_STATUS_INCOMPLETE)
                                i.GetSource()->CompleteQuest(9645);
                        }
                    }
                }
                return 50000;
            case 15:
                arca->DealDamage(arca,arca->GetHealth(),nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                return 5000;
            default : 
                return 9999999;
            }
    
        }
    
        void UpdateAI(const uint32 diff)
        override {
    
            if(YellTimer < diff)
            {
                if(EventStarted)
                {
                    YellTimer = NextStep(Step++);
                }
            }else YellTimer -= diff;
    
            if(Step >= 7 && Step <= 12 )
            {
                Unit* arca = ObjectAccessor::GetUnit((*me),ArcanagosGUID);
    
                if(FireArcanagosTimer < diff)
                {
                    if(arca)
                        arca->CastSpell(me, SPELL_FIRE_BALL, false);
                    FireArcanagosTimer = 6000;
                }else FireArcanagosTimer -= diff;
    
                if(FireMedivhTimer < diff)
                {
                    if(arca)
                        DoCast(arca, SPELL_FIRE_BALL);
                    FireMedivhTimer = 5000;
                }else FireMedivhTimer -= diff;
    
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_image_of_medivhAI(creature);
    }
};


/*######
## npc_archmage_leryda
######*/

class npc_archmage_leryda : public CreatureScript
{
public:
    npc_archmage_leryda() : CreatureScript("npc_archmage_leryda")
    { }

    class npc_archmage_lerydaAI : public ScriptedAI
    {
    public:
        npc_archmage_lerydaAI(Creature* creature) : ScriptedAI(creature)
        {}


        virtual bool GossipHello(Player* pPlayer) override
        {
            if (me->IsQuestGiver())
                pPlayer->PrepareQuestMenu(me->GetGUID());
                
            if (!pPlayer->HasItemCount(29290, 1, true) && !pPlayer->HasItemCount(29279, 1, true) && !pPlayer->HasItemCount(29283, 1, true) &&
                 !pPlayer->HasItemCount(29287, 1, true) && pPlayer->GetReputationRank(967) == REP_EXALTED) {
                if (pPlayer->GetQuestStatus(11031) == QUEST_STATUS_COMPLETE || pPlayer->GetQuestStatus(11032) == QUEST_STATUS_COMPLETE
                        || pPlayer->GetQuestStatus(11033) == QUEST_STATUS_COMPLETE || pPlayer->GetQuestStatus(11034) == QUEST_STATUS_COMPLETE) {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Je voudrais une chevalière de grand guérisseur.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Je voudrais une chevalière de maître assassin.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Je voudrais une chevalière de l'archimage.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Je voudrais une chevalière du grand protecteur.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                }
            }
            
            SEND_PREPARED_GOSSIP_MENU(pPlayer, me);
            
            return true;
        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            uint32 itemId = 0;
            switch (action) {
            case GOSSIP_ACTION_INFO_DEF+1:
                itemId = 29290;
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                itemId = 29283;
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                itemId = 29287;
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                itemId = 29279;
                break;
            }
            
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1);
            if (msg == EQUIP_ERR_OK)
            {
                Item* item = player->StoreNewItem( dest, itemId, true);
                player->SendNewItem(item, 1, true, false);
            }
            
            player->CLOSE_GOSSIP_MENU();
            
            return true;
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_archmage_lerydaAI(creature);
    }
};



class SealedTome : public GameObjectScript
{
public:
    SealedTome() : GameObjectScript("go_sealed_tome")
    {}

    struct SealedTomeAI : public GameObjectAI
    {
        SealedTomeAI(GameObject* obj) : GameObjectAI(obj) { }

        bool GossipHello(Player* player) override
        {
            if (rand() % 3 == 1)
                player->CastSpell(player, RAND(30762, 30763, 30764, 30765, 30766), true);

            return false;
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new SealedTomeAI(go);
    }
};

/*######
## woefulhealer
######*/
 
class woefulhealer : public CreatureScript
{
public:
    woefulhealer() : CreatureScript("woefulhealer")
    { }

    enum woefulhealer_defines
    {
        TIMER_GLOBALWAIT_MIN       =  2000,
        TIMER_GLOBALWAIT_MAX       =  5000,
        MAXMANA                    =  10000,
        SPELL_FLASHHEAL            =  25235,
        VALUE_FLASHHEAL            =  1200,  
        SPELL_PRAYEROFHEALING      =  25308,
        VALUE_PRAYEROFHEALING      =  1300,
    };

    class woefulhealerAI : public ScriptedAI
    {
        public:
        woefulhealerAI(Creature *c) : ScriptedAI(c) {}
        
        bool flagsset;
        Player* owner; //may be null
        uint16 wait_timer; //Time between two heals
        
        void Reset()
        override {
            owner = me->GetCharmerOrOwner() ? nullptr : me->GetCharmerOrOwner()->ToPlayer();
            if(owner)
                me->GetMotionMaster()->MoveFollow(owner, 0.8f, 210.0f);
            me->SetReactState(REACT_PASSIVE);
            wait_timer = urand(TIMER_GLOBALWAIT_MIN, TIMER_GLOBALWAIT_MAX);
            flagsset = false;
        }
        
        void EnterCombat(Unit* who) override {}
      
        void UpdateAI(const uint32 diff)
        override {                
            if (!flagsset) 
            {        
                me->SetMaxPower(POWER_MANA, MAXMANA);
                me->SetPower(POWER_MANA, MAXMANA);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                flagsset = true;
            } //does not seem to work in reset... how to fix this?
                
            if ( !me->IsAlive() || me->IsNonMeleeSpellCast(false) )
                return;
        
            if (wait_timer > diff)
            {
                wait_timer -= diff;
                return;    
            }
            //Reset timer before next heal
            wait_timer = urand(TIMER_GLOBALWAIT_MIN, TIMER_GLOBALWAIT_MAX);
    
            Unit* single_heal_target = nullptr;
            uint8 players_needing_aoe_heal = 0;
            uint32 lowest_hp = 0;
            
            //Select heal target(s)
            for(const auto& i : me->GetMap()->GetPlayers())
            {
                if (Player* i_pl = i.GetSource())
                {
                    // select players close enough and with enough health deficit
                    if (i_pl->IsAlive() && i_pl->GetDistance(me) < 40.0f
                        && ((i_pl->GetMaxHealth() - i_pl->GetHealth()) > VALUE_PRAYEROFHEALING)) // VALUE_FLASHHEAL and VALUE_PRAYEROFHEALING are about the same... let's only use one to simplify the logic
                    {
                        if (!lowest_hp || lowest_hp > i_pl->GetHealth())
                        {
                            single_heal_target = i_pl;
                            lowest_hp = i_pl->GetHealth();
                        }
                        if (owner && i_pl->IsInSameGroupWith(owner))
                            players_needing_aoe_heal++;
                    }
                }
            }
                
            if (!single_heal_target)
                return;
                
            // Cast 
            if (players_needing_aoe_heal >= 3)
                DoCast(me, SPELL_PRAYEROFHEALING);
            else
                DoCast(single_heal_target, SPELL_FLASHHEAL);
                
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new woefulhealerAI(creature);
    }
};


#define ANOMALY_SPELL_MANA_SHIELD 29880
#define ANOMALY_SPELL_BLINK 29883
#define ANOMALY_SPELL_ARCANE_VOLLEY 29885
#define ANOMALY_SPELL_LOOSE_MANA 29882
#define BLINK_DISTANCE 30.0f


class npc_arcane_anomaly : public CreatureScript
{
public:
    npc_arcane_anomaly() : CreatureScript("npc_arcane_anomaly")
    { }

    class npc_arcane_anomalyAI : public ScriptedAI
    {
        public:
        npc_arcane_anomalyAI(Creature *c) : ScriptedAI(c) {}
        
        bool castedShield;
        uint32 blinkTimer;
        uint32 volleyTimer;
    
        void Reset()
        override {
            blinkTimer = 5000 + rand()%10000;
            volleyTimer = 10000 + rand()%5000;
            castedShield = false;
        }
    
        void EnterCombat(Unit* who) 
        override {
            DoCast(me, ANOMALY_SPELL_MANA_SHIELD, true);
            castedShield = true;
        }

        void JustDied(Unit* who) override 
        {
            DoCast(me, ANOMALY_SPELL_LOOSE_MANA, true);
        }
    
        //cannot die if we havent casted our mana shield
        void DamageTaken(Unit* pKiller, uint32 &damage)
        override {
            if(!castedShield && damage >= me->GetHealth())
                damage = me->GetHealth()-1; //down to 1 hp
        }
      
        void UpdateAI(const uint32 diff)
        override {                
            if(!UpdateVictim())
                return;
    
            if(blinkTimer < diff)
            {
                if(me->GetVictim() && me->GetDistance(me->GetVictim()) > BLINK_DISTANCE)
                    if (DoCast(me, ANOMALY_SPELL_BLINK) == SPELL_CAST_OK)
                        blinkTimer = urand(10 * SECOND*IN_MILLISECONDS, 15 * SECOND*IN_MILLISECONDS);
            } else blinkTimer -= diff;
    
            if(volleyTimer < diff)
            {
                if (DoCast(me, ANOMALY_SPELL_ARCANE_VOLLEY) == SPELL_CAST_OK)
                    volleyTimer = urand(20 * SECOND*IN_MILLISECONDS, 25 * SECOND*IN_MILLISECONDS);
            } else volleyTimer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_arcane_anomalyAI(creature);
    }
};


void AddSC_karazhan()
{
    new npc_barnes();
    new npc_image_of_medivh();
    new npc_archmage_leryda();
    new SealedTome();
    new woefulhealer();
    new npc_arcane_anomaly();
    new npc_hastings();
    new npc_calliard();
}
