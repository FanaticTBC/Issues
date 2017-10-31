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
SDName: Boss_Houndmaster_Loksey
SD%Complete: 100
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */



#define SPELL_SUMMONSCARLETHOUND        17164
#define SPELL_ENRAGE                    28747

#define SAY_AGGRO                       "Release the hounds!"
#define SOUND_AGGRO                     5841

class boss_houndmaster_loksey : public CreatureScript
{
public:
    boss_houndmaster_loksey() : CreatureScript("boss_houndmaster_loksey")
    { }

    class boss_houndmaster_lokseyAI : public ScriptedAI
    {
        public:
        boss_houndmaster_lokseyAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 Enrage_Timer;
    
        void Reset()
        override {
            Enrage_Timer = 6000000;
        }
    
        void EnterCombat(Unit *who)
        override {
            me->Yell(SAY_AGGRO,LANG_UNIVERSAL,nullptr);
            DoPlaySoundToSet(me,SOUND_AGGRO);
    
            DoCast(me,SPELL_SUMMONSCARLETHOUND);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //If we are <10% hp cast healing spells at self and Mograine
            if ( me->GetHealthPct() <= 10 && !me->IsNonMeleeSpellCast(false) && Enrage_Timer < diff)
            {
                DoCast(me,SPELL_ENRAGE);
                Enrage_Timer = 900000;
            }else Enrage_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_houndmaster_lokseyAI(creature);
    }
};


void AddSC_boss_houndmaster_loksey()
{
    new boss_houndmaster_loksey();
}
