/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Teldrassil
SD%Complete: 100
SDComment: Quest support: 938
SDCategory: Teldrassil
EndScriptData */

/* ContentData
npc_mist
EndContentData */


#include "FollowerAI.h"

/*####
# npc_mist
####*/

enum eMist
{
    SAY_AT_HOME             = -1600412,
    EMOTE_AT_HOME           = -1600411,
    QUEST_MIST              = 938,
    NPC_ARYNIA              = 3519,

    POINT_ID_TO_FOREST      = 1,
    FACTION_DARNASSUS       = 79
};

const float m_afToForestLoc[] = {10648.7f, 1790.63f, 1324.08f};



class npc_mist : public CreatureScript
{
public:
    npc_mist() : CreatureScript("npc_mist")
    { }

    class npc_mistAI : public FollowerAI
    {
        public:
        npc_mistAI(Creature* pCreature) : FollowerAI(pCreature) { }
    
        uint32 m_uiPostEventTimer;
        uint32 m_uiPhasePostEvent;
    
        uint64 AryniaGUID;
    
        void Reset() 
        override { 
            m_uiPostEventTimer = 1000;
            m_uiPhasePostEvent = 0;
    
            AryniaGUID = 0;
        }
        
        void JustEngagedWith(Unit *pWho) override {}
    
        void MoveInLineOfSight(Unit *pWho)
        override {
            FollowerAI::MoveInLineOfSight(pWho);
    
            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE | STATE_FOLLOW_POSTEVENT) && pWho->GetEntry() == NPC_ARYNIA)
            {
                if (me->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
                {
                    if (Player* pPlayer = GetLeaderForFollower())
                    {
                        if (pPlayer->GetQuestStatus(QUEST_MIST) == QUEST_STATUS_INCOMPLETE)
                            pPlayer->GroupEventHappens(QUEST_MIST, me);
                    }
    
                    AryniaGUID = pWho->GetGUID();
                    SetFollowComplete(true);
                }
            }
        }
        
        void MovementInform(uint32 uiMotionType, uint32 uiPointId)
        override {
            FollowerAI::MovementInform(uiMotionType, uiPointId);
    
            if (uiMotionType != POINT_MOTION_TYPE)
                return;
    
            if (uiPointId == POINT_ID_TO_FOREST)
                SetFollowComplete();
        }
    
        void UpdateFollowerAI(const uint32 uiDiff)
        override {
            if (!UpdateVictim())
            {
                if (HasFollowState(STATE_FOLLOW_POSTEVENT))
                {
                    if (m_uiPostEventTimer <= uiDiff)
                    {
                        m_uiPostEventTimer = 3000;
    
                        Unit *pArynia = ObjectAccessor::GetUnit(*me, AryniaGUID);
                        if (!pArynia || !pArynia->IsAlive())
                        {
                            SetFollowComplete();
                            return;
                        }
    
                        switch(m_uiPhasePostEvent)
                        {
                        case 0:
                            DoScriptText(SAY_AT_HOME, pArynia);
                            break;
                        case 1:
                            DoScriptText(EMOTE_AT_HOME, me);
                            me->GetMotionMaster()->MovePoint(POINT_ID_TO_FOREST, m_afToForestLoc[0], m_afToForestLoc[1], m_afToForestLoc[2]);
                            break;
                        }
    
                        ++m_uiPhasePostEvent;
                    }
                    else
                        m_uiPostEventTimer -= uiDiff;
                }
    
                return;
            }
    
            DoMeleeAttackIfReady();
        }

        virtual void QuestAccept(Player* pPlayer, Quest const* pQuest) override
        {
            if (pQuest->GetQuestId() == QUEST_MIST)
            {
                if (npc_mistAI* pMistAI = CAST_AI(npc_mist::npc_mistAI, me->AI()))
                    pMistAI->StartFollow(pPlayer, FACTION_DARNASSUS, pQuest);
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mistAI(creature);
    }
};



void AddSC_teldrassil()
{

    new npc_mist();
}
