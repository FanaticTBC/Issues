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
SDName: Boss_Jin'do the Hexxer
SD%Complete: 85
SDComment: Mind Control not working because of core bug. Shades visible for all.
SDCategory: Zul'Gurub
EndScriptData */


#include "def_zulgurub.h"

#define SAY_AGGRO                       -1309014

#define SPELL_BRAINWASHTOTEM            24262
#define SPELL_POWERFULLHEALINGWARD      24309               //We will not use this spell. We will summon a totem by script cause the spell totems will not cast.
#define SPELL_HEX                       24053
#define SPELL_DELUSIONSOFJINDO          24306
#define SPELL_SHADEOFJINDO              24308               //We will not use this spell. We will summon a shade by script.

//Healing Ward Spell
#define SPELL_HEAL                      38588               //Totems are not working right. Right heal spell ID is 24311 but this spell is not casting...

//Shade of Jindo Spell
#define SPELL_SHADOWSHOCK               19460
#define SPELL_INVISIBLE                 24699


//Healing Ward

//Shade of Jindo

class boss_jindo : public CreatureScript
{
public:
    boss_jindo() : CreatureScript("boss_jindo")
    { }

    class boss_jindoAI : public ScriptedAI
    {
        public:
        boss_jindoAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript *pInstance;
    
        uint32 BrainWashTotem_Timer;
        uint32 HealingWard_Timer;
        uint32 Hex_Timer;
        uint32 Delusions_Timer;
        uint32 Teleport_Timer;
    
        Creature *Shade;
        Creature *Skeletons;
        Creature *HealingWard;
    
        void Reset()
        override {
            BrainWashTotem_Timer = 20000;
            HealingWard_Timer = 16000;
            Hex_Timer = 8000;
            Delusions_Timer = 10000;
            Teleport_Timer = 5000;
        }
    
        void EnterCombat(Unit *who)
        override {
            DoScriptText(SAY_AGGRO, me);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //BrainWashTotem_Timer
            if (BrainWashTotem_Timer < diff)
            {
                DoCast(me, SPELL_BRAINWASHTOTEM);
                BrainWashTotem_Timer = 18000 + rand()%8000;
            }else BrainWashTotem_Timer -= diff;
    
            //HealingWard_Timer
            if (HealingWard_Timer < diff)
            {
                //DoCast(me, SPELL_POWERFULLHEALINGWARD);
                HealingWard = me->SummonCreature(14987, me->GetPositionX()+3, me->GetPositionY()-2, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,30000);
                HealingWard_Timer = 14000 + rand()%6000;
            }else HealingWard_Timer -= diff;
    
            //Hex_Timer
            if (Hex_Timer < diff)
            {
                DoCast(me->GetVictim(), SPELL_HEX);
    
                if(me->GetThreat(me->GetVictim()))
                    DoModifyThreatPercent(me->GetVictim(),-80);
    
                Hex_Timer = 12000 + rand()%8000;
            }else Hex_Timer -= diff;
    
            //Casting the delusion curse with a shade. So shade will attack the same target with the curse.
            if (Delusions_Timer < diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                {
                    DoCast(target, SPELL_DELUSIONSOFJINDO);
    
                    Shade = me->SummonCreature(14986, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Shade)
                        Shade->AI()->AttackStart(target);
                }
    
                Delusions_Timer = 4000 + rand()%8000;
            }else Delusions_Timer -= diff;
    
            //Teleporting a random gamer and spawning 9 skeletons that will attack this gamer
            if (Teleport_Timer < diff)
            {
                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM,0);
                if (target && target->GetTypeId() == TYPEID_PLAYER)
                {
                    DoTeleportPlayer(target, -11583.7783,-1249.4278,77.5471,4.745);
    
                    if(me->GetThreat(me->GetVictim()))
                        DoModifyThreatPercent(target,-100);
    
                    Skeletons = me->SummonCreature(14826, target->GetPositionX()+2, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX()-2, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX()+4, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX()-4, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()+2, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()-2, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()+4, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()-4, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                    Skeletons = me->SummonCreature(14826, target->GetPositionX()+3, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(Skeletons)
                        Skeletons->AI()->AttackStart(target);
                }
    
                Teleport_Timer = 15000 + rand()%8000;
            }else Teleport_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_jindoAI(creature);
    }
};


class mob_healing_ward : public CreatureScript
{
public:
    mob_healing_ward() : CreatureScript("mob_healing_ward")
    { }

    class mob_healing_wardAI : public ScriptedAI
    {
        public:
        mob_healing_wardAI(Creature *c) : ScriptedAI(c)
        {
             pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 Heal_Timer;
    
        InstanceScript *pInstance;
    
        void Reset()
        override {
            Heal_Timer = 2000;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void UpdateAI (const uint32 diff)
        override {
            //Heal_Timer
            if(Heal_Timer < diff)
            {
                if(pInstance)
                {
                    Unit *pJindo = ObjectAccessor::GetUnit((*me), pInstance->GetData64(DATA_JINDO));
                    if(pJindo)
                        DoCast(pJindo, SPELL_HEAL);
                }
                Heal_Timer = 3000;
            }else Heal_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_healing_wardAI(creature);
    }
};


class mob_shade_of_jindo : public CreatureScript
{
public:
    mob_shade_of_jindo() : CreatureScript("mob_shade_of_jindo")
    { }

    class mob_shade_of_jindoAI : public ScriptedAI
    {
        public:
        mob_shade_of_jindoAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 ShadowShock_Timer;
    
        InstanceScript *pInstance;
    
        void Reset()
        override {
            ShadowShock_Timer = 1000;
            me->CastSpell(me, SPELL_INVISIBLE, TRIGGERED_FULL_MASK);
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void UpdateAI (const uint32 diff)
        override {
    
            //ShadowShock_Timer
            if(ShadowShock_Timer < diff)
            {
                DoCast(me->GetVictim(), SPELL_SHADOWSHOCK);
                ShadowShock_Timer = 2000;
            }else ShadowShock_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_shade_of_jindoAI(creature);
    }
};


void AddSC_boss_jindo()
{

    new boss_jindo();

    new mob_healing_ward();

    new mob_shade_of_jindo();
}
