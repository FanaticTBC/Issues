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
SDName: Boss_Midnight
SD%Complete: 100
SDComment:
SDCategory: Karazhan
EndScriptData */


#include "def_karazhan.h"

#define SAY_MIDNIGHT_KILL           -1532000
#define SAY_APPEAR1                 -1532001
#define SAY_APPEAR2                 -1532002
#define SAY_APPEAR3                 -1532003
#define SAY_MOUNT                   -1532004
#define SAY_KILL1                   -1532005
#define SAY_KILL2                   -1532006
#define SAY_DISARMED                -1532007
#define SAY_DEATH                   -1532008
#define SAY_RANDOM1                 -1532009
#define SAY_RANDOM2                 -1532010
#define EMOTE_CALL_ATTUMEN          -1999933
#define EMOTE_MOUNT                 -1999934

#define SPELL_SHADOWCLEAVE          29832
#define SPELL_INTANGIBLE_PRESENCE   29833
#define SPELL_CHARGE                29847                   //Only when mounted
#define SPELL_KNOCKDOWN             29711

#define MOUNTED_DISPLAYID           16040

//Attumen (TODO: Use the summoning spell instead of creature id. It works , but is not convenient for us)
#define SUMMON_ATTUMEN 15550


class boss_midnight : public CreatureScript
{
public:
    boss_midnight() : CreatureScript("boss_midnight")
    { }

    class boss_midnightAI : public ScriptedAI
    {
        public:
        boss_midnightAI(Creature *c) : ScriptedAI(c) 
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint64 Attumen;
        uint8 Phase;
        uint32 Mount_Timer;
        uint32 KnockDownTimer;
    
        InstanceScript *pInstance;
    
        void Reset()
        override {
            Phase = 1;
            Attumen = 0;
            Mount_Timer = 0;
            KnockDownTimer = 20000 + rand()%5000;
    
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetVisible(true);
    
            if(pInstance)
                pInstance->SetData(DATA_ATTUMEN_EVENT, NOT_STARTED);
        }
    
        void EnterCombat(Unit* who) 
        override {
            if(pInstance)
                pInstance->SetData(DATA_ATTUMEN_EVENT, IN_PROGRESS);
        }
    
        void KilledUnit(Unit *victim)
        override {
            if(Phase == 2)
            {
                if (Unit *pUnit = ObjectAccessor::GetUnit(*me, Attumen))
                DoScriptText(SAY_MIDNIGHT_KILL, pUnit);
            }
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            if(Phase == 1 || Phase == 2)
            {
                if(KnockDownTimer < diff)
                {
                    if(DoCast(me->GetVictim(),SPELL_KNOCKDOWN) == SPELL_CAST_OK)
                    {
                        KnockDownTimer = 10000 + rand()%10000;
                    }                
                } else { KnockDownTimer -= diff; }
            }
            
            if(Phase == 1 && (me->IsBelowHPPercent(95.0f)))
            {
                DoScriptText(EMOTE_CALL_ATTUMEN, me);
                Phase = 2;
                Creature *pAttumen = DoSpawnCreature(SUMMON_ATTUMEN, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
                if(pAttumen)
                {
                    Attumen = pAttumen->GetGUID();
                    pAttumen->AI()->AttackStart(me->GetVictim());
                    SetMidnight(pAttumen, me->GetGUID());
                    switch(rand()%3)
                    {
                    case 0: DoScriptText(SAY_APPEAR1, pAttumen); break;
                    case 1: DoScriptText(SAY_APPEAR2, pAttumen); break;
                    case 2: DoScriptText(SAY_APPEAR3, pAttumen); break;
                    }
                }
            }
            else if(Phase == 2 && (me->IsBelowHPPercent(25.0f)))
            {
                if (Unit *pAttumen = ObjectAccessor::GetUnit(*me, Attumen))
                    Mount(pAttumen);
            }
            else if(Phase ==3)
            {
                if(Mount_Timer)
                {
                    if(Mount_Timer <= diff)
                    {
                        Mount_Timer = 0;
                        me->SetVisible(false);
                        me->GetMotionMaster()->MoveIdle();
                        if (Unit *pAttumen = ObjectAccessor::GetUnit(*me, Attumen))
                        {
                            pAttumen->SetUInt32Value(UNIT_FIELD_DISPLAYID, MOUNTED_DISPLAYID);
                            pAttumen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            if(pAttumen->GetVictim())
                            {
                                pAttumen->GetMotionMaster()->MoveChase(pAttumen->GetVictim());
                                pAttumen->SetUInt64Value(UNIT_FIELD_TARGET, pAttumen->GetVictim()->GetGUID());
                            }
                            pAttumen->SetFloatValue(OBJECT_FIELD_SCALE_X,1);
                        }
                    } else Mount_Timer -= diff;
                }
            }
    
            if(Phase != 3)
                DoMeleeAttackIfReady();
        }
    
        void Mount(Unit *pAttumen)
        {
            DoScriptText(SAY_MOUNT, pAttumen);
            DoScriptText(EMOTE_MOUNT, me);
            Phase = 3;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pAttumen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            float angle = me->GetAngle(pAttumen);
            float distance = me->GetDistance2d(pAttumen);
            float newX = me->GetPositionX() + cos(angle)*(distance/2) ;
            float newY = me->GetPositionY() + sin(angle)*(distance/2) ;
            float newZ = 50;
            //me->Relocate(newX,newY,newZ,angle);
            //me->SendMonsterMove(newX, newY, newZ, 0, true, 1000);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(0, newX, newY, newZ);
            distance += 10;
            newX = me->GetPositionX() + cos(angle)*(distance/2) ;
            newY = me->GetPositionY() + sin(angle)*(distance/2) ;
            pAttumen->GetMotionMaster()->Clear();
            pAttumen->GetMotionMaster()->MovePoint(0, newX, newY, newZ);
            //pAttumen->Relocate(newX,newY,newZ,-angle);
            //pAttumen->SendMonsterMove(newX, newY, newZ, 0, true, 1000);
            Mount_Timer = 1000;
        }
    
        void SetMidnight(Creature *, uint64);                   //Below ..
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_midnightAI(creature);
    }
};



class boss_attumen : public CreatureScript
{
public:
    boss_attumen() : CreatureScript("boss_attumen")
    { }

    class boss_attumenAI : public ScriptedAI
    {
        public:
        boss_attumenAI(Creature *c) : ScriptedAI(c)
        {
            Phase = 1;
    
            CleaveTimer = 10000 + (rand()%6)*IN_MILLISECONDS;
            CurseTimer = 30000;
            RandomYellTimer = 30000 + (rand()%31)*IN_MILLISECONDS;         //Occasionally yell
            ChargeTimer = 20000;
            ResetTimer = 0;
            KnockDownTimer = 0;
    
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript *pInstance;
    
        uint64 Midnight;
        uint8 Phase;
        uint32 CleaveTimer;
        uint32 CurseTimer;
        uint32 RandomYellTimer;
        uint32 ChargeTimer;                                     //only when mounted
        uint32 ResetTimer;
        uint32 KnockDownTimer;
    
        void Reset()
        override {
            ResetTimer = 2000;
            CleaveTimer = 15000 + rand()%5000;
            CurseTimer = 30000;
            KnockDownTimer = 10000 + rand()%5000;
        }
    
        void EnterCombat(Unit* who) override {}
    
        void KilledUnit(Unit *victim)
        override {
            switch(rand()%2)
            {
            case 0: DoScriptText(SAY_KILL1, me); break;
            case 1: DoScriptText(SAY_KILL2, me); break;
            }
        }
    
        void JustDied(Unit *victim)
        override {
            DoScriptText(SAY_DEATH, me);
            if (Unit *pMidnight = ObjectAccessor::GetUnit(*me, Midnight))
                pMidnight->DealDamage(pMidnight, pMidnight->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
    
            if(pInstance)
                pInstance->SetData(DATA_ATTUMEN_EVENT, DONE);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if(ResetTimer)
            {
                if(ResetTimer <= diff)
                {
                    ResetTimer = 0;
                    Unit *pMidnight = ObjectAccessor::GetUnit(*me, Midnight);
                    if(pMidnight)
                    {
                        pMidnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pMidnight->SetVisible(true);
                    }
                    Midnight = 0;
                    me->SetVisible(false);
                    me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                }
            } else ResetTimer -= diff;
    
            //Return since we have no target
            if (!UpdateVictim())
                return;
    
            if(me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE ))
                return;
    
            if(CleaveTimer < diff)
            {
                if(DoCast(me->GetVictim(), SPELL_SHADOWCLEAVE) == SPELL_CAST_OK)
                    CleaveTimer = 15000 + rand()%5000;
            } else CleaveTimer -= diff;
    
            if(CurseTimer < diff)
            {
                if (DoCast(me->GetVictim(), SPELL_INTANGIBLE_PRESENCE) == SPELL_CAST_OK)
                    CurseTimer = 30000;
            } else CurseTimer -= diff;
    
            if(RandomYellTimer < diff)
            {
                switch(rand()%2)
                {
                case 0: DoScriptText(SAY_RANDOM1, me); break;
                case 1: DoScriptText(SAY_RANDOM2, me); break;
                }
                RandomYellTimer = 30000 + (rand()%31)*IN_MILLISECONDS;
            } else RandomYellTimer -= diff;
    
            //only when mounted
            if(me->GetUInt32Value(UNIT_FIELD_DISPLAYID) == MOUNTED_DISPLAYID)
            {
                if(ChargeTimer < diff)
                {
                    Unit *target = nullptr;
                    std::list<HostileReference *> t_list = me->GetThreatManager().getThreatList();
                    std::vector<Unit *> target_list;
                    for(auto & itr : t_list)
                    {
                        target = ObjectAccessor::GetUnit(*me, itr->getUnitGuid());
                        if(target && target->GetDistance2d(me) > 5)
                            target_list.push_back(target);
                        target = nullptr;
                    }
                    if(target_list.size())
                        target = *(target_list.begin()+rand()%target_list.size());
    
                    if (target)
                    {
                        DoCast(target, SPELL_CHARGE);
                        ChargeTimer = 20000;
                    }
                } else ChargeTimer -= diff;
    
                if(KnockDownTimer < diff)
                {
                    if(DoCast(me->GetVictim(),SPELL_KNOCKDOWN) == SPELL_CAST_OK)
                        KnockDownTimer = 10000 + rand()%10000;
                } else KnockDownTimer -= diff;
            }
            else
            {
                if(me->IsBelowHPPercent(25.0f))
                {
                    Creature *pMidnight = ObjectAccessor::GetCreature(*me, Midnight);
                    if(pMidnight && pMidnight->GetTypeId() == TYPEID_UNIT)
                    {
                        ((boss_midnight::boss_midnightAI*)(pMidnight->AI()))->Mount(me);
                        me->SetHealth(pMidnight->GetHealth());
                    }
                }
            }
    
            DoMeleeAttackIfReady();
        }
    
        void SpellHit(Unit *source, const SpellInfo *spell)
        override {
            if(spell->Mechanic == MECHANIC_DISARM)
                DoScriptText(SAY_DISARMED, me);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_attumenAI(creature);
    }
};

void boss_midnight::boss_midnightAI::SetMidnight(Creature *pAttumen, uint64 value)
{
    ((boss_attumen::boss_attumenAI*)pAttumen->AI())->Midnight = value;
}


void AddSC_boss_attumen()
{
    new boss_attumen();

    new boss_midnight();
}
