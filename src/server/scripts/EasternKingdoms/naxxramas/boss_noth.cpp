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
SDName: Boss_Noth
SD%Complete: 40
SDComment: Missing Balcony stage
SDCategory: Naxxramas
EndScriptData */



#define SAY_AGGRO1              -1533075
#define SAY_AGGRO2              -1533076
#define SAY_AGGRO3              -1533077
#define SAY_SUMMON              -1533078
#define SAY_SLAY1               -1533079
#define SAY_SLAY2               -1533080
#define SAY_DEATH               -1533081

#define SPELL_BLINK                     29211               //29208, 29209 and 29210 too
#define SPELL_CRIPPLE                   29212
#define H_SPELL_CRIPPLE                 54814
#define SPELL_CURSE_PLAGUEBRINGER       28213
#define H_SPELL_CURSE_PLAGUEBRINGER     54835
#define SOUND_DEATH      8848

#define C_PLAGUED_WARRIOR               16984

// Teleport position of Noth on his balcony
#define TELE_X 2631.370
#define TELE_Y -3529.680
#define TELE_Z 274.040
#define TELE_O 6.277

// IMPORTANT: BALCONY TELEPORT NOT ADDED YET! WILL BE ADDED SOON!
// Dev note 26.12.2008: When is soon? :)

class boss_noth : public CreatureScript
{
public:
    boss_noth() : CreatureScript("boss_noth")
    { }

    class boss_nothAI : public ScriptedAI
    {
        public:
        boss_nothAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 Blink_Timer;
        uint32 Curse_Timer;
        uint32 Summon_Timer;
    
        void Reset()
        override {
            Blink_Timer = 25000;
            Curse_Timer = 4000;
            Summon_Timer = 12000;
        }
    
        void EnterCombat(Unit *who)
        override {
            switch (rand()%3)
            {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
            }
        }
    
        void KilledUnit(Unit* victim)
        override {
            switch (rand()%2)
            {
            case 0: DoScriptText(SAY_SLAY1, me); break;
            case 1: DoScriptText(SAY_SLAY2, me); break;
            }
        }
    
        void JustSummoned(Creature* summoned)
        override {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                summoned->AddThreat(target,0.0f);
        }
    
        void JustDied(Unit* Killer)
        override {
            DoScriptText(SAY_DEATH, me);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //Blink_Timer
            if (Blink_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CRIPPLE);
                DoCast(me,SPELL_BLINK);
    
                Blink_Timer = 25000;
            }else Blink_Timer -= diff;
    
            //Curse_Timer
            if (Curse_Timer < diff)
            {
                 DoCast(me->GetVictim(),SPELL_CURSE_PLAGUEBRINGER);
                Curse_Timer = 28000;
            }else Curse_Timer -= diff;
    
            //Summon_Timer
            if (Summon_Timer < diff)
            {
                DoScriptText(SAY_SUMMON, me);
    
                for(uint8 i = 0; i < 6; i++)
                      me->SummonCreature(C_PLAGUED_WARRIOR,2684.804,-3502.517,261.313,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
    
                Summon_Timer = 30500;
            } else Summon_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_nothAI(creature);
    }
};


void AddSC_boss_noth()
{
    new boss_noth();
}
