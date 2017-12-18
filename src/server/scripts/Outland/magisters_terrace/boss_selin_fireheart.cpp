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
SDName: Boss_Selin_Fireheart
SD%Complete: 90
SDComment: Heroic and Normal Support. Needs further testing.
SDCategory: Magister's Terrace
EndScriptData */


#include "def_magisters_terrace.h"
#include <list>

#define SAY_AGGRO                       -1585000
#define SAY_ENERGY                      -1585001
#define SAY_EMPOWERED                   -1585002
#define SAY_KILL_1                      -1585003
#define SAY_KILL_2                      -1585004
#define SAY_DEATH                       -1585005
#define EMOTE_CRYSTAL                   -1585006

//Crystal efect spells
#define SPELL_FEL_CRYSTAL_COSMETIC      44374
#define SPELL_FEL_CRYSTAL_DUMMY         44329
#define SPELL_FEL_CRYSTAL_VISUAL        44355
#define SPELL_MANA_RAGE                 44320               // This spell triggers 44321, which changes scale and regens mana Requires an entry in spell_script_target

//Selin's spells
#define SPELL_DRAIN_LIFE                44294
#define SPELL_FEL_EXPLOSION             44314

#define SPELL_DRAIN_MANA                46153               // Heroic only

#define CRYSTALS_NUMBER                 5
#define DATA_CRYSTALS                   6

#define CREATURE_FEL_CRYSTAL            24722


class boss_selin_fireheart : public CreatureScript
{
public:
    boss_selin_fireheart() : CreatureScript("boss_selin_fireheart")
    { }

    class boss_selin_fireheartAI : public ScriptedAI
    {
        public:
        boss_selin_fireheartAI(Creature* c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
    
            Crystals.clear();
            // GUIDs per instance is static, so we only need to load them once.
            if(pInstance)
            {
                uint32 size = pInstance->GetData(DATA_FEL_CRYSTAL_SIZE);
                for(uint8 i = 0; i < size; ++i)
                {
                    uint64 guid = pInstance->GetData64(DATA_FEL_CRYSTAL);
                    Crystals.push_back(guid);
                }
            }
            Heroic = c->GetMap()->IsHeroic();
        }
    
        InstanceScript* pInstance;
    
        std::vector<uint64> Crystals;
    
        uint32 DrainLifeTimer;
        uint32 DrainManaTimer;
        uint32 FelExplosionTimer;
        uint32 DrainCrystalTimer;
        uint32 CheckTimer;
    
        bool IsDraining;
        bool DrainingCrystal;
        bool Heroic;
        uint64 CrystalGUID;                                     // This will help us create a pointer to the crystal we are draining. We store GUIDs, never units in case unit is deleted/offline (offline if player of course).
    
        void Reset()
        override {
            if(pInstance)
            {
                //for(uint8 i = 0; i < CRYSTALS_NUMBER; ++i)
                for(uint64 & Crystal : Crystals)
                {
                    //Unit* pUnit = ObjectAccessor::GetUnit(*me, FelCrystals[i]);
                    Unit* pUnit = ObjectAccessor::GetUnit(*me, Crystal);
                    if(pUnit)
                    {
                        if(!pUnit->IsAlive())
                            (pUnit->ToCreature())->Respawn();      // Let MaNGOS handle setting death state, etc.
    
                        // Only need to set unselectable flag. You can't attack unselectable units so non_attackable flag is not necessary here.
                        pUnit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
                GameObject* Door = GameObject::GetGameObject(*me, pInstance->GetData64(DATA_SELIN_ENCOUNTER_DOOR));
                if (Door)
                    Door->UseDoorOrButton();                        // Open the big encounter door. Close it in Aggro and open it only in JustDied(and here)
                                                                // Small door opened after event are expected to be closed by default
                // Set Inst data for encounter
                if (me->IsDead())
                    pInstance->SetData(DATA_SELIN_EVENT, DONE);
                else pInstance->SetData(DATA_SELIN_EVENT, NOT_STARTED);
            }else error_log(ERROR_INST_DATA);
    
            DrainLifeTimer = 3000 + rand()%4000;
            DrainManaTimer = DrainLifeTimer + 5000;
            FelExplosionTimer = 2100;
            DrainCrystalTimer = 10000 + rand()%5000;
            DrainCrystalTimer = 20000 + rand()%5000;
            CheckTimer = 1000;
    
            IsDraining = false;
            DrainingCrystal = false;
            CrystalGUID = 0;
        }
    
        void SelectNearestCrystal()
        {
            if(Crystals.empty())
                return;
    
            float ShortestDistance = 0;
            CrystalGUID = 0;
            Unit* pCrystal = nullptr;
            Unit* CrystalChosen = nullptr;
            //for(uint8 i =  0; i < CRYSTALS_NUMBER; ++i)
            for(uint64 & Crystal : Crystals)
            {
                pCrystal = nullptr;
                //pCrystal = ObjectAccessor::GetUnit(*me, FelCrystals[i]);
                pCrystal = ObjectAccessor::GetUnit(*me, Crystal);
                if(pCrystal && pCrystal->IsAlive())
                {
                    if(!ShortestDistance || (ShortestDistance > me->GetDistance2d(pCrystal)))
                    {
                        ShortestDistance = me->GetDistance2d(pCrystal);
                        CrystalGUID = pCrystal->GetGUID();
                        CrystalChosen = pCrystal;               // Store a copy of pCrystal so we don't need to recreate a pointer to closest crystal for the movement and yell.
                    }
                }
            }
            if( CrystalChosen )
            {
                DoScriptText(SAY_ENERGY, me);
                DoScriptText(EMOTE_CRYSTAL, me);
    
                CrystalChosen->CastSpell(CrystalChosen, SPELL_FEL_CRYSTAL_COSMETIC, TRIGGERED_FULL_MASK);
    
                float x, y, z;                                  // coords that we move to, close to the crystal.
                CrystalChosen->GetClosePoint(x, y, z, me->GetCombatReach(), CONTACT_DISTANCE);
                me->SetFullTauntImmunity(true);
                me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(1, x, y, z);
                DrainingCrystal = true;
            }
        }
    
        void ShatterRemainingCrystals()
        {
            if(Crystals.empty())
                return;
    
            //for(uint8 i = 0; i < CRYSTALS_NUMBER; ++i)
            for(uint64 & Crystal : Crystals)
            {
                //Creature* pCrystal = (ObjectAccessor::GetCreature(*me, FelCrystals[i]));
                Creature* pCrystal = (ObjectAccessor::GetCreature(*me, Crystal));
                if( pCrystal && pCrystal->IsAlive())
                    pCrystal->DealDamage(pCrystal, pCrystal->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
            }
        }
    
        void EnterCombat(Unit* who)
        override {
            me->SetPower(POWER_MANA, 0);
            DoScriptText(SAY_AGGRO, me);
    
            if( pInstance )
            {
                GameObject* EncounterDoor = GameObject::GetGameObject(*me, pInstance->GetData64(DATA_SELIN_ENCOUNTER_DOOR));
                if( EncounterDoor )
                    EncounterDoor->ResetDoorOrButton();               //Close the encounter door, open it in JustDied/Reset
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
    
        void MovementInform(uint32 type, uint32 id)
        override {
            if(type == POINT_MOTION_TYPE && id == 1)
            {
                Unit* CrystalChosen = ObjectAccessor::GetUnit(*me, CrystalGUID);
                if(CrystalChosen && CrystalChosen->IsAlive())
                {
                    // Make the crystal attackable
                    // We also remove NON_ATTACKABLE in case the database has it set.
                    CrystalChosen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    CrystalChosen->CastSpell(me, SPELL_MANA_RAGE, TRIGGERED_FULL_MASK);
                    IsDraining = true;
                }
                else
                {
                    // Make an error message in case something weird happened here
                    error_log("TSCR: Selin Fireheart unable to drain crystal as the crystal is either dead or despawned");
                    DrainingCrystal = false;
                }
            }
        }
    
        void JustDied(Unit* killer)
        override {
            DoScriptText(SAY_DEATH, me);
    
            if(!pInstance)
                return;
    
            pInstance->SetData(DATA_SELIN_EVENT, DONE);         // Encounter complete!
    
            GameObject* EncounterDoor = GameObject::GetGameObject((*me), pInstance->GetData64(DATA_SELIN_ENCOUNTER_DOOR));
            if( EncounterDoor )
                EncounterDoor->UseDoorOrButton();                   // Open the encounter door
    
            GameObject* ContinueDoor = GameObject::GetGameObject(*me, pInstance->GetData64(DATA_SELIN_DOOR));
            if (ContinueDoor)
                ContinueDoor->UseDoorOrButton();                    // Open the door leading further in
    
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if(!UpdateVictim())
                return;
    
            if(!DrainingCrystal)
            {
                uint32 maxPowerMana = me->GetMaxPower(POWER_MANA);
                if( maxPowerMana && ((me->GetPower(POWER_MANA)*100 / maxPowerMana) < 10) )
                {
                    if( DrainLifeTimer < diff )
                    {
                        DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_DRAIN_LIFE);
                        DrainLifeTimer = 10000;
                    }else DrainLifeTimer -= diff;
    
                    // Heroic only
                    if( Heroic )
                    {
                        if( DrainManaTimer < diff )
                        {
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0), SPELL_DRAIN_MANA);
                            DrainManaTimer = 10000;
                        }else DrainManaTimer -= diff;
                    }
                }
    
                if( FelExplosionTimer < diff )
                {
                    if(!me->IsNonMeleeSpellCast(false))
                    {
                        DoCast(me, SPELL_FEL_EXPLOSION);
                        FelExplosionTimer = 2000;
                    }
                }else FelExplosionTimer -= diff;
    
                // If below 10% mana, start recharging
                maxPowerMana = me->GetMaxPower(POWER_MANA);
                if( maxPowerMana && ((me->GetPower(POWER_MANA)*100 / maxPowerMana) < 10) )
                {
                    if(DrainCrystalTimer < diff)
                    {
                        SelectNearestCrystal();
                        if(Heroic)   DrainCrystalTimer = 10000 + rand()%5000;
                        else         DrainCrystalTimer = 20000 + rand()%5000;
                    }else DrainCrystalTimer -= diff;
                }
    
            }else
            {
                if( IsDraining )
                {
                    if (CheckTimer < diff)
                    {
                        Unit* CrystalChosen = ObjectAccessor::GetUnit(*me, CrystalGUID);
                        if(CrystalChosen)
                        {
                            if(CrystalChosen->GetUInt32Value(UNIT_CHANNEL_SPELL) == SPELL_MANA_RAGE)
                            {
                                me->StopMoving();
                            }else{
                                IsDraining = false;
                                DrainingCrystal = false;
    
                                DoScriptText(SAY_EMPOWERED, me);
    
                                Unit* CrystalChosen = ObjectAccessor::GetUnit(*me, CrystalGUID);
                                if( CrystalChosen && CrystalChosen->IsAlive() )
                                    // Use Deal Damage to kill it, not setDeathState.
                                    CrystalChosen->DealDamage(CrystalChosen, CrystalChosen->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                                CrystalGUID = 0;
    
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            }
                        }
                        CheckTimer = 1000;
                    }else CheckTimer -= diff;
                }
            }
            DoMeleeAttackIfReady();                             // No need to check if we are draining crystal here, as the spell has a stun.
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_selin_fireheartAI(creature);
    }
};



class mob_fel_crystal : public CreatureScript
{
public:
    mob_fel_crystal() : CreatureScript("mob_fel_crystal")
    { }

    class mob_fel_crystalAI : public ScriptedAI
    {
        public:
        mob_fel_crystalAI(Creature *c) : ScriptedAI(c) {}
    
        void Reset() override {}
        void EnterCombat(Unit* who) override {}
        void AttackStart(Unit* who) override {}
        void MoveInLineOfSight(Unit* who) override {}
        void UpdateAI(const uint32 diff) override {}
    
        void JustDied(Unit* killer)
        override {
            me->RemoveAurasDueToSpell(SPELL_MANA_RAGE);
            if(InstanceScript* pInstance = ((InstanceScript*)me->GetInstanceScript()))
            {
                Creature* Selin = (ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_SELIN)));
                if(Selin && Selin->IsAlive())
                {
                    if(((boss_selin_fireheart::boss_selin_fireheartAI*)Selin->AI())->CrystalGUID == me->GetGUID())
                    {
                        // Set this to false if we are the creature that Selin is draining so his AI flows properly
                        ((boss_selin_fireheart::boss_selin_fireheartAI*)Selin->AI())->DrainingCrystal = false;
                        ((boss_selin_fireheart::boss_selin_fireheartAI*)Selin->AI())->IsDraining = false;
                        Selin->RemoveAurasDueToSpell(SPELL_MANA_RAGE);
                        if(Selin->GetVictim())
                        {
                            Selin->AI()->AttackStart(Selin->GetVictim());
                            Selin->GetMotionMaster()->MoveChase(Selin->GetVictim());
                            Selin->SetFullTauntImmunity(false);
                        }
                    }
                }
            }else error_log(ERROR_INST_DATA);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_fel_crystalAI(creature);
    }
};


void AddSC_boss_selin_fireheart()
{

    new boss_selin_fireheart();

    new mob_fel_crystal();
}
