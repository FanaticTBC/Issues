/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Ungoro Crater
SD%Complete: 100
SDComment: Support for Quest: 4245, 4491
SDCategory: Ungoro Crater
EndScriptData */

/* ContentData
npc_a-me
npc_ringo
EndContentData */


#include "EscortAI.h"
#include "FollowerAI.h"

#define SAY_READY -1000200
#define SAY_AGGRO1 -1000201
#define SAY_SEARCH -1000202
#define SAY_AGGRO2 -1000203
#define SAY_AGGRO3 -1000204
#define SAY_FINISH -1000205

#define SPELL_DEMORALIZINGSHOUT  13730

#define QUEST_CHASING_AME 4245
#define ENTRY_TARLORD 6519
#define ENTRY_TARLORD1 6519
#define ENTRY_STOMPER 6513

/*######
## npc_a-me
######*/


class npc_ame : public CreatureScript
{
public:
    npc_ame() : CreatureScript("npc_ame")
    { }

    class npc_ameAI : public EscortAI
    {
        public:
        npc_ameAI(Creature *c) : EscortAI(c) {}
    
        uint32 DEMORALIZINGSHOUT_Timer;
    
        void WaypointReached(uint32 i, uint32 pathID)
        override {
            Player* player = GetPlayerForEscort();
    
            if (!player)
                return;
    
            switch (i)
            {
    
             case 19:
                me->SummonCreature(ENTRY_STOMPER, -6391.69, -1730.49, -272.83, 4.96, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                DoScriptText(SAY_AGGRO1, me, player);
                break;
                case 28:
                DoScriptText(SAY_SEARCH, me, player);
                break;
                case 38:
                me->SummonCreature(ENTRY_TARLORD, -6370.75, -1382.84, -270.51, 6.06, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                DoScriptText(SAY_AGGRO2, me, player);
                break;
                case 49:
                me->SummonCreature(ENTRY_TARLORD1, -6324.44, -1181.05, -270.17, 4.34, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                DoScriptText(SAY_AGGRO3, me, player);
                break;
             case 55:
                DoScriptText(SAY_FINISH, me, player);
                player->GroupEventHappens(QUEST_CHASING_AME,me);
                break;
    
            }
       }
    
        void Reset()
        override {
          DEMORALIZINGSHOUT_Timer = 5000;
        }
    
        void EnterCombat(Unit* who)
        override {}
    
        void JustSummoned(Creature* summoned)
        override {
            summoned->AI()->AttackStart(me);
        }
    
        void JustDied(Unit* killer)
        override {
            if (_playerGUID)
            {
                if (Player* player = GetPlayerForEscort())
                    player->FailQuest(QUEST_CHASING_AME);
            }
        }
    
        void UpdateAI(const uint32 diff)
        override {
            EscortAI::UpdateAI(diff);
            if (!UpdateVictim())
                return;
    
            if (DEMORALIZINGSHOUT_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_DEMORALIZINGSHOUT);
                DEMORALIZINGSHOUT_Timer = 70000;
            }else DEMORALIZINGSHOUT_Timer -= diff;
    
        }

        virtual void QuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_CHASING_AME)
            {
                ((EscortAI*)(me->AI()))->Start(true, false, player->GetGUID(), quest);
                DoScriptText(SAY_READY, me, player);
                me->SetUInt32Value(UNIT_FIELD_BYTES_1,0);
                // Change faction so mobs attack
                me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_PASSIVE);
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ameAI(creature);
    }
};



/*######
## npc_ringo
######*/

enum eRingo
{
    SAY_RIN_START_1             = -1000416,
    SAY_RIN_START_2             = -1000417,

    SAY_FAINT_1                 = -1000418,
    SAY_FAINT_2                 = -1000419,
    SAY_FAINT_3                 = -1000420,
    SAY_FAINT_4                 = -1000421,

    SAY_WAKE_1                  = -1000422,
    SAY_WAKE_2                  = -1000423,
    SAY_WAKE_3                  = -1000424,
    SAY_WAKE_4                  = -1000425,

    SAY_RIN_END_1               = -1000426,
    SAY_SPR_END_2               = -1000427,
    SAY_RIN_END_3               = -1000428,
    EMOTE_RIN_END_4             = -1000429,
    EMOTE_RIN_END_5             = -1000430,
    SAY_RIN_END_6               = -1000431, // signed for 6784
    SAY_SPR_END_7               = -1000432,
    EMOTE_RIN_END_8             = -1000433,

    SPELL_REVIVE_RINGO          = 15591,
    QUEST_A_LITTLE_HELP         = 4491,
    NPC_SPRAGGLE                = 9997,
    FACTION_ESCORTEE            = 113
};


class npc_ringo : public CreatureScript
{
public:
    npc_ringo() : CreatureScript("npc_ringo")
    { }

    class npc_ringoAI : public FollowerAI
    {
        public:
        npc_ringoAI(Creature* pCreature) : FollowerAI(pCreature) { }
    
        uint32 m_uiFaintTimer;
        uint32 m_uiEndEventProgress;
        uint32 m_uiEndEventTimer;
    
        uint64 SpraggleGUID;
    
        void Reset()
        override {
            m_uiFaintTimer = urand(30000, 60000);
            m_uiEndEventProgress = 0;
            m_uiEndEventTimer = 1000;
            SpraggleGUID = 0;
        }
        
        void EnterCombat(Unit *pWho) override {}
    
        void MoveInLineOfSight(Unit *pWho)
        override {
            FollowerAI::MoveInLineOfSight(pWho);
    
            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_SPRAGGLE)
            {
                if (me->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
                {
                    if (Player* pPlayer = GetLeaderForFollower())
                    {
                        if (pPlayer->GetQuestStatus(QUEST_A_LITTLE_HELP) == QUEST_STATUS_INCOMPLETE)
                            pPlayer->GroupEventHappens(QUEST_A_LITTLE_HELP, me);
                    }
    
                    SpraggleGUID = pWho->GetGUID();
                    SetFollowComplete(true);
                }
            }
        }
    
        void SpellHit(Unit* pCaster, const SpellInfo* pSpell)
        override {
            if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && pSpell->Id == SPELL_REVIVE_RINGO)
                ClearFaint();
        }
    
        void SetFaint()
        {
            if (!HasFollowState(STATE_FOLLOW_POSTEVENT))
            {
                SetFollowPaused(true);
    
                DoScriptText(RAND(SAY_FAINT_1,SAY_FAINT_2,SAY_FAINT_3,SAY_FAINT_4), me);
            }
    
            //what does actually happen here? Emote? Aura?
            me->SetStandState(UNIT_STAND_STATE_SLEEP);
        }
    
        void ClearFaint()
        {
            me->SetStandState(UNIT_STAND_STATE_STAND);
    
            if (HasFollowState(STATE_FOLLOW_POSTEVENT))
                return;
    
            DoScriptText(RAND(SAY_WAKE_1,SAY_WAKE_2,SAY_WAKE_3,SAY_WAKE_4), me);
    
            SetFollowPaused(false);
        }
    
        void UpdateFollowerAI(const uint32 uiDiff)
        override {
            if (!UpdateVictim())
            {
                if (HasFollowState(STATE_FOLLOW_POSTEVENT))
                {
                    if (m_uiEndEventTimer <= uiDiff)
                    {
                        Unit *pSpraggle = ObjectAccessor::GetUnit(*me, SpraggleGUID);
                        if (!pSpraggle || !pSpraggle->IsAlive())
                        {
                            SetFollowComplete();
                            return;
                        }
    
                        switch(m_uiEndEventProgress)
                        {
                            case 1:
                                DoScriptText(SAY_RIN_END_1, me);
                                m_uiEndEventTimer = 3000;
                                break;
                            case 2:
                                DoScriptText(SAY_SPR_END_2, pSpraggle);
                                m_uiEndEventTimer = 5000;
                                break;
                            case 3:
                                DoScriptText(SAY_RIN_END_3, me);
                                m_uiEndEventTimer = 1000;
                                break;
                            case 4:
                                DoScriptText(EMOTE_RIN_END_4, me);
                                SetFaint();
                                m_uiEndEventTimer = 9000;
                                break;
                            case 5:
                                DoScriptText(EMOTE_RIN_END_5, me);
                                ClearFaint();
                                m_uiEndEventTimer = 1000;
                                break;
                            case 6:
                                DoScriptText(SAY_RIN_END_6, me);
                                m_uiEndEventTimer = 3000;
                                break;
                            case 7:
                                DoScriptText(SAY_SPR_END_7, pSpraggle);
                                m_uiEndEventTimer = 10000;
                                break;
                            case 8:
                                DoScriptText(EMOTE_RIN_END_8, me);
                                m_uiEndEventTimer = 5000;
                                break;
                            case 9:
                                SetFollowComplete();
                                break;
                        }
    
                        ++m_uiEndEventProgress;
                    }
                    else
                        m_uiEndEventTimer -= uiDiff;
                }
                else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
                {
                    if (!HasFollowState(STATE_FOLLOW_PAUSED))
                    {
                        if (m_uiFaintTimer <= uiDiff)
                        {
                            SetFaint();
                            m_uiFaintTimer = urand(60000, 120000);
                        }
                        else
                            m_uiFaintTimer -= uiDiff;
                    }
                }
    
                return;
            }
    
            DoMeleeAttackIfReady();
        }

        virtual void QuestAccept(Player* pPlayer, Quest const* pQuest) override
        {
            if (pQuest->GetQuestId() == QUEST_A_LITTLE_HELP)
            {
                if (npc_ringoAI* pRingoAI = CAST_AI(npc_ringo::npc_ringoAI, me->AI()))
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    pRingoAI->StartFollow(pPlayer, FACTION_ESCORTEE, pQuest);
                }
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ringoAI(creature);
    }
};



void AddSC_ungoro_crater()
{

    new npc_ame();
    
    new npc_ringo();
}
