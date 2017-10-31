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
SDName: Boss_High_Botanist_Freywinn
SD%Complete: 90
SDComment: some strange visual related to tree form(if aura lost before normal duration end). possible make summon&transform -process smoother(transform after delay)
SDCategory: Tempest Keep, The Botanica
EndScriptData */


#include "GameEventMgr.h"

#define SAY_AGGRO                   -1553000
#define SAY_KILL_1                  -1553001
#define SAY_KILL_2                  -1553002
#define SAY_TREE_1                  -1553003
#define SAY_TREE_2                  -1553004
#define SAY_DEATH                   -1553005

#define SPELL_TRANQUILITY           34550
#define SPELL_TREE_FORM             34551

#define SPELL_SUMMON_FRAYER         34557
#define ENTRY_FRAYER                19953

#define SPELL_PLANT_WHITE           34759
#define SPELL_PLANT_GREEN           34761
#define SPELL_PLANT_BLUE            34762
#define SPELL_PLANT_RED             34763


class boss_high_botanist_freywinn : public CreatureScript
{
public:
    boss_high_botanist_freywinn() : CreatureScript("boss_high_botanist_freywinn")
    { }

    class boss_high_botanist_freywinnAI : public ScriptedAI
    {
        public:
        boss_high_botanist_freywinnAI(Creature *c) : ScriptedAI(c) {}
    
        std::list<uint64> Adds_List;
    
        uint32 SummonSeedling_Timer;
        uint32 TreeForm_Timer;
        uint32 MoveCheck_Timer;
        uint32 DeadAddsCount;
        bool MoveFree;
    
        bool isEventActive()
        {
            const GameEventMgr::ActiveEvents& activeEvents = sGameEventMgr->GetActiveEventList();
            bool active = activeEvents.find(57) != activeEvents.end();
    
            return active;
        }
    
        void Reset()
        override {
            Adds_List.clear();
    
            SummonSeedling_Timer = 6000;
            TreeForm_Timer = 30000;
            MoveCheck_Timer = 1000;
            DeadAddsCount = 0;
            MoveFree = true;
            
            if (isEventActive())
                me->SetDisplayId(22805);
        }
    
        void EnterCombat(Unit *who)
        override {
            DoScriptText(SAY_AGGRO, me);
        }
    
        void JustSummoned(Creature *summoned)
        override {
            if( summoned->GetEntry() == ENTRY_FRAYER )
                Adds_List.push_back(summoned->GetGUID());
        }
    
        void DoSummonSeedling()
        {
            switch(rand()%4)
            {
                case 0: DoCast(me,SPELL_PLANT_WHITE); break;
                case 1: DoCast(me,SPELL_PLANT_GREEN); break;
                case 2: DoCast(me,SPELL_PLANT_BLUE); break;
                case 3: DoCast(me,SPELL_PLANT_RED); break;
            }
        }
    
        void KilledUnit(Unit* victim)
        override {
            switch(rand()%2)
            {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
            }
        }
    
        void JustDied(Unit* Killer)
        override {
            DoScriptText(SAY_DEATH, me);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if( !UpdateVictim() )
                return;
    
            if( TreeForm_Timer < diff )
            {
                switch(rand()%2)
                {
                case 0: DoScriptText(SAY_TREE_1, me); break;
                case 1: DoScriptText(SAY_TREE_2, me); break;
                }
    
                if( me->IsNonMeleeSpellCast(false) )
                    me->InterruptNonMeleeSpells(true);
    
                me->RemoveAllAuras();
    
                DoCast(me,SPELL_SUMMON_FRAYER,true);
                DoCast(me,SPELL_TRANQUILITY,true);
                DoCast(me,SPELL_TREE_FORM,true);
    
                me->GetMotionMaster()->MoveIdle();
                MoveFree = false;
    
                TreeForm_Timer = 75000;
            }else TreeForm_Timer -= diff;
    
            if( !MoveFree )
            {
                if( MoveCheck_Timer < diff )
                {
                    if( !Adds_List.empty() )
                    {
                        for(auto itr = Adds_List.begin(); itr != Adds_List.end(); ++itr)
                        {
                            if( Unit *temp = ObjectAccessor::GetUnit(*me,*itr) )
                            {
                                if( !temp->IsAlive() )
                                {
                                    Adds_List.erase(itr);
                                    ++DeadAddsCount;
                                    break;
                                }
                            }
                        }
                    }
    
                    if( DeadAddsCount < 3 && TreeForm_Timer-30000 < diff )
                        DeadAddsCount = 3;
    
                    if( DeadAddsCount >= 3 )
                    {
                        Adds_List.clear();
                        DeadAddsCount = 0;
    
                        me->InterruptNonMeleeSpells(true);
                        me->RemoveAllAuras();
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                        MoveFree = true;
                    }
                    MoveCheck_Timer = 500;
                }
                else MoveCheck_Timer -= diff;
    
                return;
            }
    
            /*if( me->HasAuraEffect(SPELL_TREE_FORM,0) || me->HasAuraEffect(SPELL_TRANQUILITY,0) )
                return;*/
    
            //one random seedling every 5 secs, but not in tree form
            if( SummonSeedling_Timer < diff )
            {
                DoSummonSeedling();
                SummonSeedling_Timer = 6000;
            }else SummonSeedling_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_high_botanist_freywinnAI(creature);
    }
};


void AddSC_boss_high_botanist_freywinn()
{

    new boss_high_botanist_freywinn();
}
