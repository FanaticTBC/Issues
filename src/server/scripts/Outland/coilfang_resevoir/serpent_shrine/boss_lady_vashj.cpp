/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 */

/* ScriptData
SDName: Boss_Lady_Vashj
SD%Complete: 99
SDComment: Missing blizzlike Shield Generators coords
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */


#include "def_serpent_shrine.h"
#include "SimpleAI.h"
#include "Spell.h"

#define SAY_INTRO                   -1548042
#define SAY_AGGRO1                  -1548043
#define SAY_AGGRO2                  -1548044
#define SAY_AGGRO3                  -1548045
#define SAY_AGGRO4                  -1548046
#define SAY_PHASE1                  -1548047
#define SAY_PHASE2                  -1548048
#define SAY_PHASE3                  -1548049
#define SAY_BOWSHOT1                -1548050
#define SAY_BOWSHOT2                -1548051
#define SAY_SLAY1                   -1548052
#define SAY_SLAY2                   -1548053
#define SAY_SLAY3                   -1548054
#define SAY_DEATH                   -1548055

#define SPELL_SURGE                 38044
#define SPELL_MULTI_SHOT            38310
#define SPELL_SHOCK_BLAST           38509
#define SPELL_ENTANGLE              38316
#define SPELL_STATIC_CHARGE_TRIGGER 38280
#define SPELL_FORKED_LIGHTNING      40088
#define SPELL_SHOOT                 40873
#define SPELL_POISON_BOLT           40095
#define SPELL_TOXIC_SPORES          38575
#define SPELL_MAGIC_BARRIER         38112

#define MIDDLE_X                    30.134
#define MIDDLE_Y                    -923.65
#define MIDDLE_Z                    42.9

#define SPOREBAT_X                  30.977156
#define SPOREBAT_Y                  -925.297761
#define SPOREBAT_Z                  77.176567
#define SPOREBAT_O                  5.223932

#define SHIED_GENERATOR_CHANNEL       19870
#define ENCHANTED_ELEMENTAL           21958
#define TAINTED_ELEMENTAL             22009
#define COILFANG_STRIDER              22056
#define COILFANG_ELITE                22055
#define TOXIC_SPOREBAT                22140
#define TOXIC_SPORES_TRIGGER          22207

float ElementPos[8][4] =
{
    {8.3, -835.3, 21.9, 5},
    {53.4, -835.3, 21.9, 4.5},
    {96, -861.9, 21.8, 4},
    {96, -986.4, 21.4, 2.5},
    {54.4, -1010.6, 22, 1.8},
    {9.8, -1012, 21.7, 1.4},
    {-35, -987.6, 21.5, 0.8},
    {-58.9, -901.6, 21.5, 6}
};

float ElementWPPos[8][3] =
{
    {71.700752, -883.905884, 41.097168},
    {45.039848, -868.022827, 41.097015},
    {14.585141, -867.894470, 41.097061},
    {-25.415508, -906.737732, 41.097061},
    {-11.801594, -963.405884, 41.097067},
    {14.556657, -979.051514, 41.097137},
    {43.466549, -979.406677, 41.097027},
    {69.945908, -964.663940, 41.097054}
};

float SporebatWPPos[8][3] =
{
    {31.6,-896.3,59.1},
    {9.1, -913.9, 56},
    {5.2, -934.4, 52.4},
    {20.7, -946.9, 49.7},
    {41, -941.9, 51},
    {47.7, -927.3, 55},
    {42.2, -912.4, 51.7},
    {27, -905.9, 50}
};

float CoilfangElitePos[3][4] =
{
    {28.84, -923.28, 42.9, 6},
    {31.183281, -953.502625, 41.523602, 1.640957},
    {58.895180, -923.124268, 41.545307, 3.152848}
};

float CoilfangStriderPos[3][4] =
{
    {66.427010, -948.778503, 41.262245, 2.584220},
    {7.513962, -959.538208, 41.300422, 1.034629},
    {-12.843201, -907.798401, 41.239620, 6.087094}
};

float ShieldGeneratorChannelPos[4][4] =
{
    {49.6262, -902.181, 43.0975, 3.95683},
    {10.988, -901.616, 42.5371, 5.4373},
    {10.3859, -944.036, 42.5446, 0.779888},
    {49.3126, -943.398, 42.5501, 2.40174}
};

//Lady Vashj AI
struct boss_lady_vashjAI : public ScriptedAI
{
    boss_lady_vashjAI (Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
        Intro = false;
        JustCreated = true;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); //set it only once on creature create (no need do intro if wiped)
    }

    InstanceScript *pInstance;

    uint64 ShieldGeneratorChannel[4];

    uint32 AggroTimer;
    uint32 ShockBlast_Timer;
    uint32 Entangle_Timer;
    uint32 StaticCharge_Timer;
    uint32 ForkedLightning_Timer;
    uint32 Check_Timer;
    uint32 EnchantedElemental_Timer;
    uint32 TaintedElemental_Timer;
    uint32 CoilfangElite_Timer;
    uint32 CoilfangStrider_Timer;
    uint32 SummonSporebat_Timer;
    uint32 SummonSporebat_StaticTimer;
    uint8 EnchantedElemental_Pos;
    uint8 Phase;

    bool Entangle;
    bool InCombat;
    bool Intro;
    bool CanAttack;
    bool JustCreated;

    void Reset()
    override {
        AggroTimer = 19000;
        ShockBlast_Timer = 1+rand()%60000;
        Entangle_Timer = 30000;
        StaticCharge_Timer = 10000+rand()%15000;
        ForkedLightning_Timer = 2000;
        Check_Timer = 15000;
        EnchantedElemental_Timer = 5000;
        TaintedElemental_Timer = 50000;
        CoilfangElite_Timer = 45000+rand()%5000;
        CoilfangStrider_Timer = 60000+rand()%10000;
        SummonSporebat_Timer = 10000;
        SummonSporebat_StaticTimer = 30000;
        EnchantedElemental_Pos = 0;
        Phase = 0;

        Entangle = false;
        InCombat = false;
        if(JustCreated)
        {
            CanAttack = false;
            JustCreated = false;
        }else CanAttack = true;

        Unit *remo;
        for(uint64 i : ShieldGeneratorChannel)
        {
            remo = ObjectAccessor::GetUnit(*me, i);
            if (remo)
                remo->SetDeathState(JUST_DIED);
        }

        if(pInstance)
            pInstance->SetData(DATA_LADYVASHJEVENT, NOT_STARTED);
        ShieldGeneratorChannel[0] = 0;
        ShieldGeneratorChannel[1] = 0;
        ShieldGeneratorChannel[2] = 0;
        ShieldGeneratorChannel[3] = 0;

        me->SetCorpseDelay(1000*60*60);
    }

    //Called when a tainted elemental dies
    void EventTaintedElementalDeath()
    {
        //the next will spawn 50 seconds after the previous one's death
        if(TaintedElemental_Timer > 50000)
            TaintedElemental_Timer = 50000;
    }
    void KilledUnit(Unit *victim)
    override {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_SLAY1, me); break;
            case 1: DoScriptText(SAY_SLAY2, me); break;
            case 2: DoScriptText(SAY_SLAY3, me); break;
        }
    }

    void JustDied(Unit *victim)
    override {
        DoScriptText(SAY_DEATH, me);

        if(pInstance)
            pInstance->SetData(DATA_LADYVASHJEVENT, DONE);
    }

    void StartEvent()
    {
        switch(rand()%4)
        {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
            case 3: DoScriptText(SAY_AGGRO4, me); break;
        }

        InCombat = true;
        Phase = 1;

        if(pInstance)
            pInstance->SetData(DATA_LADYVASHJEVENT, IN_PROGRESS);
    }

    void EnterCombat(Unit *who)
    override {
        if (pInstance)
        {
            //remove old tainted cores to prevent cheating in phase 2
            Map *map = me->GetMap();
            Map::PlayerList const &PlayerList = map->GetPlayers();
            for(const auto & i : PlayerList)
            {
                if(Player* i_pl = i.GetSource())
                {
                    i_pl->DestroyItemCount(31088, 1, true);
                }
            }
        }
        if(Phase != 2)
            AttackStart(who);

        if(!InCombat)
            StartEvent();
    }

    void MoveInLineOfSight(Unit *who)
    override {
        if (!Intro)
        {
            Intro = true;
            DoScriptText(SAY_INTRO, me);
        }
        if (!CanAttack)
            return;
        if (!who || me->GetVictim())
            return;

        if (me->CanAttack(who) == CAN_ATTACK_RESULT_OK && who->isInAccessiblePlaceFor(me) && me->IsHostileTo(who))
        {
            float attackRadius = me->GetAttackDistance(who);
            if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && me->IsWithinLOSInMap(who))
            {
                //if(who->HasStealthAura())
                //    who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);

                if(Phase != 2)
                    AttackStart(who);

                if(!InCombat)
                    StartEvent();
            }
        }
    }

    void CastShootOrMultishot()
    {
        switch(rand()%2)
        {
            case 0:
                //Shoot
                //Used in Phases 1 and 3 after Entangle or while having nobody in melee range. A shot that hits her target for 4097-5543 Physical damage.
                DoCast(me->GetVictim(), SPELL_SHOOT);
                break;
            case 1:
                //Multishot
                //Used in Phases 1 and 3 after Entangle or while having nobody in melee range. A shot that hits 1 person and 4 people around him for 6475-7525 physical damage.
                DoCast(me->GetVictim(), SPELL_MULTI_SHOT);
                break;
        }
        if(rand()%3)
        {
            switch(rand()%2)
            {
                case 0: DoScriptText(SAY_BOWSHOT1, me); break;
                case 1: DoScriptText(SAY_BOWSHOT2, me); break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    override {
        if(!CanAttack && Intro)
        {
            if(AggroTimer < diff)
            {
                CanAttack = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AggroTimer=19000;
            }else
            {
                AggroTimer-=diff;
                return;
            }
        }
        //to prevent abuses during phase 2
        if(Phase == 2 && !me->GetVictim() && InCombat)
        {
            EnterEvadeMode();
            return;
        }
        //Return since we have no target
        if (!UpdateVictim() )
            return;

        if(Phase == 1 || Phase == 3)
        {
            //ShockBlast_Timer
            if (ShockBlast_Timer < diff)
            {
                //Shock Burst
                //Randomly used in Phases 1 and 3 on Vashj's target, it's a Shock spell doing 8325-9675 nature damage and stunning the target for 5 seconds, during which she will not attack her target but switch to the next person on the aggro list.
                DoCast(me->GetVictim(), SPELL_SHOCK_BLAST);
                me->TauntApply(me->GetVictim());

                ShockBlast_Timer = 1000+rand()%14000;       //random cooldown
            }else ShockBlast_Timer -= diff;

            //StaticCharge_Timer
            if(StaticCharge_Timer < diff)
            {
                //Static Charge
                //Used on random people (only 1 person at any given time) in Phases 1 and 3, it's a debuff doing 2775 to 3225 Nature damage to the target and everybody in about 5 yards around it, every 1 seconds for 30 seconds. It can be removed by Cloak of Shadows, Iceblock, Divine Shield, etc, but not by Cleanse or Dispel Magic.
                Unit *target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true);

                if(target && !target->HasAuraEffect(SPELL_STATIC_CHARGE_TRIGGER, 0))
                                                            //cast Static Charge every 2 seconds for 20 seconds
                        DoCast(target, SPELL_STATIC_CHARGE_TRIGGER);

                StaticCharge_Timer = 10000+rand()%20000;    //blizzlike
            }else StaticCharge_Timer -= diff;

            //Entangle_Timer
            if (Entangle_Timer < diff)
            {
                if(!Entangle)
                {
                    //Entangle
                    //Used in Phases 1 and 3, it casts Entangling Roots on everybody in a 15 yard radius of Vashj, immobilzing them for 10 seconds and dealing 500 damage every 2 seconds. It's not a magic effect so it cannot be dispelled, but is removed by various buffs such as Cloak of Shadows or Blessing of Freedom.
                    DoCast(me->GetVictim(), SPELL_ENTANGLE);
                    Entangle = true;
                    Entangle_Timer = 10000;
                }
                else
                {
                    CastShootOrMultishot();
                    Entangle = false;
                    Entangle_Timer = 20000+rand()%5000;
                }
            }else Entangle_Timer -= diff;

            //Phase 1
            if(Phase == 1)
            {
                //Start phase 2
                if ((me->GetHealth()*100 / me->GetMaxHealth()) < 70)
                {
                    //Phase 2 begins when Vashj hits 70%. She will run to the middle of her platform and surround herself in a shield making her invulerable.
                    Phase = 2;

                    me->GetMotionMaster()->Clear();
                    DoTeleportTo(MIDDLE_X, MIDDLE_Y, MIDDLE_Z);

                    Creature *pCreature;
                    for(uint8 i = 0; i < 4; i++)
                    {
                        pCreature = me->SummonCreature(SHIED_GENERATOR_CHANNEL, ShieldGeneratorChannelPos[i][0],  ShieldGeneratorChannelPos[i][1],  ShieldGeneratorChannelPos[i][2],  ShieldGeneratorChannelPos[i][3], TEMPSUMMON_CORPSE_DESPAWN, 0);
                        if (pCreature)
                            ShieldGeneratorChannel[i] = pCreature->GetGUID();
                    }
                    DoScriptText(SAY_PHASE2, me);
                }
            }
            //Phase 3
            else
            {
                //SummonSporebat_Timer
                if(SummonSporebat_Timer < diff)
                {
                    Creature *Sporebat = nullptr;
                    Sporebat = me->SummonCreature(TOXIC_SPOREBAT, SPOREBAT_X, SPOREBAT_Y, SPOREBAT_Z, SPOREBAT_O, TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if(Sporebat)
                    {
                        Unit *target = nullptr;
                        target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        if(target)
                            Sporebat->AI()->AttackStart(target);
                    }

                    //summon sporebats faster and faster
                    if(SummonSporebat_StaticTimer > 1000)
                        SummonSporebat_StaticTimer -= 1000;

                    SummonSporebat_Timer = SummonSporebat_StaticTimer;

                    if(SummonSporebat_Timer < 5000)
                        SummonSporebat_Timer = 5000;

                }else SummonSporebat_Timer -= diff;
            }

            //Melee attack
            DoMeleeAttackIfReady();

            //Check_Timer - used to check if somebody is in melee range
            if(Check_Timer < diff)
            {
                bool InMeleeRange = false;
                Unit *target;
                std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
                for(auto & itr : t_list)
                {
                    target = ObjectAccessor::GetUnit(*me, itr->getUnitGuid());
                                                            //if in melee range
                    if(target && target->IsWithinDistInMap(me, 5))
                    {
                        InMeleeRange = true;
                        break;
                    }
                }

                //if nobody is in melee range
                if(!InMeleeRange)
                    CastShootOrMultishot();

                Check_Timer = 5000;
            }else Check_Timer -= diff;
        }
        //Phase 2
        else
        {
            //ForkedLightning_Timer
            if(ForkedLightning_Timer < diff)
            {
                //Forked Lightning
                //Used constantly in Phase 2, it shoots out completely randomly targeted bolts of lightning which hit everybody in a roughtly 60 degree cone in front of Vashj for 2313-2687 nature damage.
                Unit *target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                if(!target)
                    target = me->GetVictim();

                DoCast(target, SPELL_FORKED_LIGHTNING);

                ForkedLightning_Timer = 2000+rand()%6000;   //blizzlike
            }else ForkedLightning_Timer -= diff;

            //EnchantedElemental_Timer
            if(EnchantedElemental_Timer < diff)
            {
                Creature *Elemental;
                Elemental = me->SummonCreature(ENCHANTED_ELEMENTAL, ElementPos[EnchantedElemental_Pos][0], ElementPos[EnchantedElemental_Pos][1], ElementPos[EnchantedElemental_Pos][2], ElementPos[EnchantedElemental_Pos][3], TEMPSUMMON_CORPSE_DESPAWN, 0);

                if(EnchantedElemental_Pos == 7)
                    EnchantedElemental_Pos = 0;
                else
                    EnchantedElemental_Pos++;

                EnchantedElemental_Timer = 10000+rand()%5000;
            }else EnchantedElemental_Timer -= diff;

            //TaintedElemental_Timer
            if(TaintedElemental_Timer < diff)
            {
                Creature *Tain_Elemental;
                uint32 pos = rand()%8;
                Tain_Elemental = me->SummonCreature(TAINTED_ELEMENTAL, ElementPos[pos][0], ElementPos[pos][1], ElementPos[pos][2], ElementPos[pos][3], TEMPSUMMON_DEAD_DESPAWN, 0);

                TaintedElemental_Timer = 120000;
            }else TaintedElemental_Timer -= diff;

            //CoilfangElite_Timer
            if(CoilfangElite_Timer < diff)
            {
                uint32 pos = rand()%3;
                Creature* CoilfangElite = nullptr;
                CoilfangElite = me->SummonCreature(COILFANG_ELITE, CoilfangElitePos[pos][0], CoilfangElitePos[pos][1], CoilfangElitePos[pos][2], CoilfangElitePos[pos][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                if(CoilfangElite)
                {
                    Unit *target = nullptr;
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    if(target)
                        CoilfangElite->AI()->AttackStart(target);
                    else if(me->GetVictim())
                        CoilfangElite->AI()->AttackStart(me->GetVictim());
                }
                CoilfangElite_Timer = 45000+rand()%5000;
            }else CoilfangElite_Timer -= diff;

            //CoilfangStrider_Timer
            if(CoilfangStrider_Timer < diff)
            {
                uint32 pos = rand()%3;
                Creature* CoilfangStrider = nullptr;
                CoilfangStrider = me->SummonCreature(COILFANG_STRIDER, CoilfangStriderPos[pos][0], CoilfangStriderPos[pos][1], CoilfangStriderPos[pos][2], CoilfangStriderPos[pos][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                 if(CoilfangStrider)
                {
                    Unit *target = nullptr;
                    target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    if(target)
                        CoilfangStrider->AI()->AttackStart(target);
                    else if(me->GetVictim())
                        CoilfangStrider->AI()->AttackStart(me->GetVictim());
                }
                CoilfangStrider_Timer = 60000+rand()%10000;
            }else CoilfangStrider_Timer -= diff;

            //Check_Timer
            if(Check_Timer < diff)
            {
                //Start Phase 3
                if(pInstance && pInstance->GetData(DATA_CANSTARTPHASE3))
                {
                    //set life 50%
                    me->SetHealth(me->GetMaxHealth()/2);

                    me->RemoveAurasDueToSpell(SPELL_MAGIC_BARRIER);

                    DoScriptText(SAY_PHASE3, me);

                    Phase = 3;

                    //return to the tank
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                }
                Check_Timer = 1000;
            }else Check_Timer -= diff;
        }
    }
};
class VashjSurgeAura : public Aura
{
    public:
        VashjSurgeAura(SpellInfo *spell, uint32 eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, nullptr)
            {}
};
//Enchanted Elemental
//If one of them reaches Vashj he will increase her damage done by 5%.
struct mob_enchanted_elementalAI : public ScriptedAI
{
    mob_enchanted_elementalAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
    }

    InstanceScript *pInstance;
    uint32 move;
    uint32 phase;
    float x, y, z;
    Unit *Vashj;

    void Reset()
    override {
        me->SetSpeedRate(MOVE_WALK,0.6);//walk
        me->SetSpeedRate(MOVE_RUN,0.6);//run
        move = 0;
        phase = 1;
        Vashj = nullptr;

        for (auto & ElementWPPo : ElementWPPos)//search for nearest waypoint (up on stairs)
        {
            if (!x || !y || !z)
            {
                x = ElementWPPo[0];
                y = ElementWPPo[1];
                z = ElementWPPo[2];
            }
            else
            {
                if (me->GetDistance(ElementWPPo[0],ElementWPPo[1],ElementWPPo[2]) < me->GetDistance(x,y,z))
                {
                    x = ElementWPPo[0];
                    y = ElementWPPo[1];
                    z = ElementWPPo[2];
                }
            }
        }
        if (pInstance)
            Vashj = ObjectAccessor::GetUnit((*me), pInstance->GetData64(DATA_LADYVASHJ));
    }

    void EnterCombat(Unit *who) override { return; }

    void MoveInLineOfSight(Unit *who)override {return;}

    void UpdateAI(const uint32 diff)
    override {
        if(!pInstance)
            return;

        if (!Vashj)
        {
            return;
        }

        if(move < diff)
        {
            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
            if (phase == 1)
            {
                me->GetMotionMaster()->MovePoint(0, x, y, z);
            }
            if (phase == 1 && me->GetDistance(x,y,z) < 0.1)
            {
                phase = 2;
            }
            if (phase == 2)
            {
                me->GetMotionMaster()->MovePoint(0, MIDDLE_X, MIDDLE_Y, MIDDLE_Z);
                phase = 3;
            }
            if (phase == 3)
            {
                me->GetMotionMaster()->MovePoint(0, MIDDLE_X, MIDDLE_Y, MIDDLE_Z);
                if(me->GetDistance(MIDDLE_X, MIDDLE_Y, MIDDLE_Z) < 3)
                {
                    SpellInfo *spell = (SpellInfo *)sSpellMgr->GetSpellInfo(SPELL_SURGE);
                    if( spell )
                    {
                        for(uint32 i = 0;i<3;i++)
                        {
                            if (!spell->Effects[i].Effect)
                                continue;

                            Vashj->AddAura(new VashjSurgeAura(spell, i, nullptr, Vashj, Vashj));
                        }
                    }
                    me->DealDamage(me, me->GetMaxHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                }
            }
            if(((boss_lady_vashjAI*)(Vashj->ToCreature())->AI())->InCombat == false || ((boss_lady_vashjAI*)(Vashj->ToCreature())->AI())->Phase != 2 || Vashj->IsDead())
            {
                //call Unsummon()
                me->DealDamage(me, me->GetMaxHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
            }
            move = 1000;
        }else move -= diff;
    }
};

//Tainted Elemental
//This mob has 7,900 life, doesn't move, and shoots Poison Bolts at one person anywhere in the area, doing 3,000 nature damage and placing a posion doing 2,000 damage every 2 seconds. He will switch targets often, or sometimes just hang on a single player, but there is nothing you can do about it except heal the damage and kill the Tainted Elemental
struct mob_tainted_elementalAI : public ScriptedAI
{
    mob_tainted_elementalAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
    }

    InstanceScript *pInstance;

    uint32 PoisonBolt_Timer;
    uint32 Despawn_Timer;

    void Reset()
    override {
        PoisonBolt_Timer = 5000+rand()%5000;
        Despawn_Timer = 30000;
    }

    void JustDied(Unit *killer)
    override {
        if(pInstance)
        {
            Creature *Vashj = nullptr;
            Vashj = (Creature*)(ObjectAccessor::GetUnit((*me), pInstance->GetData64(DATA_LADYVASHJ)));

            if(Vashj)
                ((boss_lady_vashjAI*)Vashj->AI())->EventTaintedElementalDeath();
        }
    }

    void EnterCombat(Unit *who)
    override {
        me->AddThreat(who, 0.1f);
    }

    void UpdateAI(const uint32 diff)
    override {
        //PoisonBolt_Timer
        if(PoisonBolt_Timer < diff)
        {
            Unit *target = nullptr;
            target = SelectTarget(SELECT_TARGET_RANDOM, 0);

            if(target && target->IsWithinDistInMap(me, 30))
                DoCast(target, SPELL_POISON_BOLT);

            PoisonBolt_Timer = 5000+rand()%5000;
        }else PoisonBolt_Timer -= diff;

        //Despawn_Timer
        if(Despawn_Timer < diff)
        {
            //call Unsummon()
            me->SetDeathState(DEAD);

            //to prevent crashes
            Despawn_Timer = 1000;
        }else Despawn_Timer -= diff;
    }
};

//Toxic Sporebat
//Toxic Spores: Used in Phase 3 by the Spore Bats, it creates a contaminated green patch of ground, dealing about 2775-3225 nature damage every second to anyone who stands in it.
struct mob_toxic_sporebatAI : public ScriptedAI
{
    mob_toxic_sporebatAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
        EnterEvadeMode();
    }

    InstanceScript *pInstance;

    uint32 movement_timer;
    uint32 ToxicSpore_Timer;
    uint32 bolt_timer;
    uint32 Check_Timer;

    void Reset()
    override {
        me->SetDisableGravity(true);
        me->SetFaction(14);
        movement_timer = 0;
        ToxicSpore_Timer = 5000;
        bolt_timer = 5500;
        Check_Timer = 1000;
    }

    void EnterCombat(Unit *who)
    override {

    }

    void MoveInLineOfSight(Unit *who)
    override {

    }

    void MovementInform(uint32 type, uint32 id)
    override {
        if(type != POINT_MOTION_TYPE)
            return;

        if(id == 1)
            movement_timer = 0;
    }

    void UpdateAI (const uint32 diff)
    override {

        /*if(!me->IsInCombat())
            me->SetInCombatState(false);*/

        //Random movement
        if (movement_timer < diff)
        {
            uint32 rndpos = rand()%8;
            me->GetMotionMaster()->MovePoint(1,SporebatWPPos[rndpos][0], SporebatWPPos[rndpos][1], SporebatWPPos[rndpos][2]);
            movement_timer = 6000;
        }else movement_timer -= diff;

        //toxic spores
        if(bolt_timer < diff)
        {
            Unit *target = nullptr;
            target = SelectTarget(SELECT_TARGET_RANDOM, 0);
            if(target)
            {
                Creature* trig = me->SummonCreature(TOXIC_SPORES_TRIGGER,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,30000);
                if(trig)
                {
                    trig->SetFaction(14);
                    trig->CastSpell(trig, SPELL_TOXIC_SPORES,true);
                }
            }
            bolt_timer = 10000+rand()%5000;
        }
        else bolt_timer -= diff;

        //Check_Timer
        if(Check_Timer < diff)
        {
            if(pInstance)
            {
                //check if vashj is death
                Unit *Vashj = nullptr;
                Vashj = ObjectAccessor::GetUnit((*me), pInstance->GetData64(DATA_LADYVASHJ));
                if(!Vashj || (Vashj && !Vashj->IsAlive()) || (Vashj && ((boss_lady_vashjAI*)(Vashj->ToCreature())->AI())->Phase != 3))
                {
                    //remove
                    me->SetDeathState(DEAD);
                    me->RemoveCorpse();
                    me->SetFaction(35);
                }
            }

            Check_Timer = 1000;
        }else Check_Timer -= diff;
    }
};

//Coilfang Elite
//It's an elite Naga mob with 170,000 HP. It does about 5000 damage on plate, and has a nasty cleave hitting for about 7500 damage
CreatureAI* GetAI_mob_coilfang_elite(Creature *_Creature)
{
    auto  ai = new SimpleAI (_Creature);

    ai->Spell[0].Enabled = true;
    ai->Spell[0].Spell_Id = 31345;                          //Cleave
    ai->Spell[0].Cooldown = 15000;
    ai->Spell[0].CooldownRandomAddition = 5000;
    ai->Spell[0].First_Cast = 5000;
    ai->Spell[0].Cast_Target_Type = CAST_HOSTILE_RANDOM;

    ai->EnterEvadeMode();

    return ai;
}

//Coilfang Strider
//It hits plate for about 8000 damage, has a Mind Blast spell doing about 3000 shadow damage, and a Psychic Scream Aura, which fears everybody in a 8 yard range of it every 2-3 seconds , for 5 seconds and increasing their movement speed by 150% during the fear.
#define SPELL_PANIC         38257
#define SPELL_MINDBLAST     38259

struct mob_coilfang_striderAI : public ScriptedAI
{
    mob_coilfang_striderAI(Creature *c) : ScriptedAI(c)
    {
        Reset();
    }

    uint32 Blast_Timer;

    void Reset()
    override {
        Blast_Timer = 8000;
    }

    void EnterCombat(Unit *who)
    override {
        DoCast(me,SPELL_PANIC,true);
    }

    void UpdateAI (const uint32 diff)
    override {

        if (!UpdateVictim() )
            return;

        if(Blast_Timer < diff)
        {
            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0),SPELL_MINDBLAST);
            Blast_Timer = 30000+rand()% 10000;
        }else Blast_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_coilfang_strider(Creature *_Creature)
{
    return new mob_coilfang_striderAI (_Creature);
}

struct mob_shield_generator_channelAI : public ScriptedAI
{
    mob_shield_generator_channelAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
    }

    InstanceScript *pInstance;
    uint32 Check_Timer;
    bool Casted;
    void Reset()
    override {
        Check_Timer = 0;
        Casted = false;
        me->SetUInt32Value(UNIT_FIELD_DISPLAYID , 11686);  //invisible

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void EnterCombat(Unit *who) override { return; }

    void MoveInLineOfSight(Unit *who) override { return; }

    void UpdateAI (const uint32 diff)
    override {
        if(!pInstance)
            return;

        if(Check_Timer < diff)
        {
            Unit *Vashj = nullptr;
            Vashj = ObjectAccessor::GetUnit((*me), pInstance->GetData64(DATA_LADYVASHJ));

            if(Vashj && Vashj->IsAlive())
            {
                //start visual channel
                if (!Casted || !Vashj->HasAuraEffect(SPELL_MAGIC_BARRIER,0))
                {
                    me->CastSpell(Vashj,SPELL_MAGIC_BARRIER,true);
                    Casted = true;
                }
            }
            Check_Timer = 1000;
        }else Check_Timer -= diff;
    }
};

class ItemTaintedCore : ItemScript
{
public:
    ItemTaintedCore() : ItemScript("item_tainted_core") {}

    bool OnUse(Player* player, Item* _Item, SpellCastTargets const& targets) override
    {
        InstanceScript *pInstance = (player->GetInstanceScript()) ? ((InstanceScript*)player->GetInstanceScript()) : nullptr;

        if (!pInstance)
        {
            player->GetSession()->SendNotification("Instance script not initialized");
            return true;
        }

        Creature *Vashj = nullptr;
        Vashj = (Creature*)(ObjectAccessor::GetUnit((*player), pInstance->GetData64(DATA_LADYVASHJ)));
        if (Vashj && ((boss_lady_vashjAI*)Vashj->AI())->Phase == 2)
        {
            if (targets.GetGOTarget() && targets.GetGOTarget()->GetTypeId() == TYPEID_GAMEOBJECT)
            {
                uint32 identifier;
                uint8 channel_identifier;
                switch (targets.GetGOTarget()->GetEntry())
                {
                case 185052:
                    identifier = DATA_SHIELDGENERATOR1;
                    channel_identifier = 0;
                    break;
                case 185053:
                    identifier = DATA_SHIELDGENERATOR2;
                    channel_identifier = 1;
                    break;
                case 185051:
                    identifier = DATA_SHIELDGENERATOR3;
                    channel_identifier = 2;
                    break;
                case 185054:
                    identifier = DATA_SHIELDGENERATOR4;
                    channel_identifier = 3;
                    break;
                default:
                    return true;
                }

                if (pInstance->GetData(identifier))
                {
                    player->GetSession()->SendNotification("Already deactivated");
                    return true;
                }

                //get and remove channel
                Unit *Channel = nullptr;
                Channel = ObjectAccessor::GetUnit((*Vashj), ((boss_lady_vashjAI*)Vashj->AI())->ShieldGeneratorChannel[channel_identifier]);
                if (Channel)
                {
                    //call Unsummon()
                    Channel->SetDeathState(JUST_DIED);
                }

                pInstance->SetData(identifier, 1);

                //remove this item
                player->DestroyItemCount(31088, 1, true);
                return true;
            }
            else if (targets.GetUnitTarget()->GetTypeId() == TYPEID_UNIT)
                return false;
            else if (targets.GetUnitTarget()->GetTypeId() == TYPEID_PLAYER)
            {
                player->DestroyItemCount(31088, 1, true);
                player->CastSpell(targets.GetUnitTarget(), 38134, true);
                return true;
            }
        }
        return true;
    }
};

CreatureAI* GetAI_boss_lady_vashj(Creature *_Creature)
{
    return new boss_lady_vashjAI (_Creature);
}

CreatureAI* GetAI_mob_enchanted_elemental(Creature *_Creature)
{
    return new mob_enchanted_elementalAI (_Creature);
}

CreatureAI* GetAI_mob_tainted_elemental(Creature *_Creature)
{
    return new mob_tainted_elementalAI (_Creature);
}

CreatureAI* GetAI_mob_toxic_sporebat(Creature *_Creature)
{
    return new mob_toxic_sporebatAI (_Creature);
}

CreatureAI* GetAI_mob_shield_generator_channel(Creature *_Creature)
{
    return new mob_shield_generator_channelAI (_Creature);
}

void AddSC_boss_lady_vashj()
{
    OLDScript *newscript;
    newscript = new OLDScript;
    newscript->Name="boss_lady_vashj";
    newscript->GetAI = &GetAI_boss_lady_vashj;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_enchanted_elemental";
    newscript->GetAI = &GetAI_mob_enchanted_elemental;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_tainted_elemental";
    newscript->GetAI = &GetAI_mob_tainted_elemental;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_toxic_sporebat";
    newscript->GetAI = &GetAI_mob_toxic_sporebat;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_coilfang_elite";
    newscript->GetAI = &GetAI_mob_coilfang_elite;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_coilfang_strider";
    newscript->GetAI = &GetAI_mob_coilfang_strider;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_shield_generator_channel";
    newscript->GetAI = &GetAI_mob_shield_generator_channel;
    sScriptMgr->RegisterOLDScript(newscript);

    new ItemTaintedCore();
}
