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
SDName: Boss_Shadow_Hunter_Voshgajin
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */



#define SPELL_CURSEOFBLOOD      24673
#define SPELL_HEX               16708
#define SPELL_CLEAVE            20691

class boss_shadow_hunter_voshgajin : public CreatureScript
{
public:
    boss_shadow_hunter_voshgajin() : CreatureScript("boss_shadow_hunter_voshgajin")
    { }

    class boss_shadowvoshAI : public ScriptedAI
    {
        public:
        boss_shadowvoshAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 CurseOfBlood_Timer;
        uint32 Hex_Timer;
        uint32 Cleave_Timer;
    
        void Reset()
        override {
            CurseOfBlood_Timer = 2000;
            Hex_Timer = 8000;
            Cleave_Timer = 14000;
    
            //me->CastSpell(me,SPELL_ICEARMOR, TRIGGERED_FULL_MASK);
        }
    
        void JustEngagedWith(Unit *who)
        override {
        }
    
        void UpdateAI(const uint32 diff)
        override {
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //CurseOfBlood_Timer
            if (CurseOfBlood_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CURSEOFBLOOD);
                CurseOfBlood_Timer = 45000;
            }else CurseOfBlood_Timer -= diff;
    
            //Hex_Timer
            if (Hex_Timer < diff)
            {
                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM,0);
                if (target) DoCast(target,SPELL_HEX);
                Hex_Timer = 15000;
            }else Hex_Timer -= diff;
    
            //Cleave_Timer
            if (Cleave_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CLEAVE);
                Cleave_Timer = 7000;
            }else Cleave_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_shadowvoshAI(creature);
    }
};


void AddSC_boss_shadowvosh()
{
    new boss_shadow_hunter_voshgajin();
}

