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
SDName: Boss_Ambassador_Flamelash
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */



enum Spells
{
    SPELL_FIREBLAST                                        = 15573
};

class boss_ambassador_flamelash : public CreatureScript
{
public:
    boss_ambassador_flamelash() : CreatureScript("boss_ambassador_flamelash")
    { }

    class boss_ambassador_flamelashAI : public ScriptedAI
    {
        public:
        boss_ambassador_flamelashAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 FireBlast_Timer;
        uint32 Spirit_Timer;
        int Rand;
        int RandX;
        int RandY;
        Creature* Summoned;
    
        void Reset()
        override {
            FireBlast_Timer = 2000;
            Spirit_Timer = 24000;
        }
    
        void JustEngagedWith(Unit *who) override {}
    
        void SummonSpirits(Unit* victim)
        {
            Rand = rand()%10;
            switch (rand()%2)
            {
                case 0: RandX -= Rand; break;
                case 1: RandX += Rand; break;
            }
            Rand = 0;
            Rand = rand()%10;
            switch (rand()%2)
            {
                case 0: RandY -= Rand; break;
                case 1: RandY += Rand; break;
            }
            Summoned = DoSpawnCreature(9178, RandX, RandY, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
            if(Summoned)
                ((CreatureAI*)Summoned->AI())->AttackStart(victim);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //FireBlast_Timer
            if (FireBlast_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_FIREBLAST);
                FireBlast_Timer = 7000;
            }else FireBlast_Timer -= diff;
    
            //Spirit_Timer
            if (Spirit_Timer < diff)
            {
                SummonSpirits(me->GetVictim());
                SummonSpirits(me->GetVictim());
                SummonSpirits(me->GetVictim());
                SummonSpirits(me->GetVictim());
    
                Spirit_Timer = 30000;
            }else Spirit_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_ambassador_flamelashAI(creature);
    }
};


void AddSC_boss_ambassador_flamelash()
{
    new boss_ambassador_flamelash();
}

