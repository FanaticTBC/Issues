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
SDName: Boss_Hazzarah
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */


#include "def_zulgurub.h"

#define SPELL_MANABURN         26046
#define SPELL_SLEEP            24664

class boss_hazzarah : public CreatureScript
{
public:
    boss_hazzarah() : CreatureScript("boss_hazzarah")
    { }

    class boss_hazzarahAI : public ScriptedAI
    {
        public:
        boss_hazzarahAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 ManaBurn_Timer;
        uint32 Sleep_Timer;
        uint32 Illusions_Timer;
        Creature* Illusion;
    
        void Reset()
        override {
            ManaBurn_Timer = 4000 + rand()%6000;
            Sleep_Timer = 10000 + rand()%8000;
            Illusions_Timer = 10000 + rand()%8000;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //ManaBurn_Timer
            if (ManaBurn_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_MANABURN);
                ManaBurn_Timer = 8000 + rand()%8000;
            }else ManaBurn_Timer -= diff;
    
            //Sleep_Timer
            if (Sleep_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_SLEEP);
                Sleep_Timer = 12000 + rand()%8000;
            }else Sleep_Timer -= diff;
    
            //Illusions_Timer
            if (Illusions_Timer < diff)
            {
                //We will summon 3 illusions that will spawn on a random gamer and attack this gamer
                //We will just use one model for the beginning
                Unit* target = nullptr;
                for(int i = 0; i < 3;i++)
                {
                    target = SelectTarget(SELECT_TARGET_RANDOM,0);
                    if(!target)
                        return;
    
                    Illusion = me->SummonCreature(15163,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,30000);
                    if(Illusion)
                        ((CreatureAI*)Illusion->AI())->AttackStart(target);
                }
    
                Illusions_Timer = 15000 + rand()%10000;
            }else Illusions_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_hazzarahAI(creature);
    }
};


void AddSC_boss_hazzarah()
{
    new boss_hazzarah();
}
