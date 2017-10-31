
/* ScriptData
SDName: Boss_Tomb_Of_Seven
SD%Complete: 90
SDComment: Learning Smelt Dark Iron if tribute quest rewarded. Missing event and re-spawn GO Spectral Chalice.
SDCategory: Blackrock Depths
EndScriptData */


#include "def_blackrock_depths.h"

#define FACTION_NEUTRAL             734
#define FACTION_HOSTILE             754

#define SPELL_SUNDERARMOR           24317
#define SPELL_SHIELDBLOCK           12169
#define SPELL_STRIKE                15580


class boss_angerrel : public CreatureScript
{
public:
    boss_angerrel() : CreatureScript("boss_angerrel")
    { }

    class boss_angerrelAI : public ScriptedAI
    {
        public:
        boss_angerrelAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
        
        InstanceScript* pInstance;
    
        uint32 SunderArmor_Timer;
        uint32 ShieldBlock_Timer;
        uint32 Strike_Timer;
    
        void Reset()
        override {
            SunderArmor_Timer = 8000;
            ShieldBlock_Timer = 15000;
            Strike_Timer = 12000;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void JustDied(Unit* killer)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //SunderArmor_Timer
            if (SunderArmor_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_SUNDERARMOR);
                SunderArmor_Timer = 28000;
            }else SunderArmor_Timer -= diff;
    
            //ShieldBlock_Timer
            if (ShieldBlock_Timer < diff)
            {
                DoCast(me,SPELL_SHIELDBLOCK);
                ShieldBlock_Timer = 25000;
            }else ShieldBlock_Timer -= diff;
    
            //Strike_Timer
            if (Strike_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_STRIKE);
                Strike_Timer = 10000;
            }else Strike_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_angerrelAI(creature);
    }
};


#define SPELL_SINISTERSTRIKE        15581
#define SPELL_BACKSTAB              15582
#define SPELL_GOUGE                 13579


class boss_doperel : public CreatureScript
{
public:
    boss_doperel() : CreatureScript("boss_doperel")
    { }

    class boss_doperelAI : public ScriptedAI
    {
        public:
        boss_doperelAI(Creature *c) : ScriptedAI(c) 
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 SinisterStrike_Timer;
        uint32 BackStab_Timer;
        uint32 Gouge_Timer;
        
        InstanceScript* pInstance;
    
        void Reset()
        override {
            SinisterStrike_Timer = 8000;
            BackStab_Timer = 12000;
            Gouge_Timer = 6000;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void JustDied(Unit* killer)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //SinisterStrike_Timer
            if (SinisterStrike_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_SINISTERSTRIKE);
                SinisterStrike_Timer = 7000;
            }else SinisterStrike_Timer -= diff;
    
            //BackStab_Timer
            if (BackStab_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_BACKSTAB);
                BackStab_Timer = 6000;
            }else BackStab_Timer -= diff;
    
            //Gouge_Timer
            if (Gouge_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_GOUGE);
                Gouge_Timer = 8000;
            }else Gouge_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_doperelAI(creature);
    }
};


#define SPELL_SHADOWBOLT        17483                       //Not sure if right ID
#define SPELL_MANABURN          10876
#define SPELL_SHADOWSHIELD      22417


class boss_haterel : public CreatureScript
{
public:
    boss_haterel() : CreatureScript("boss_haterel")
    { }

    class boss_haterelAI : public ScriptedAI
    {
        public:
        boss_haterelAI(Creature *c) : ScriptedAI(c) 
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 ShadowBolt_Timer;
        uint32 ManaBurn_Timer;
        uint32 ShadowShield_Timer;
        uint32 Strike_Timer;
    
        InstanceScript* pInstance;
    
        void Reset()
        override {
            ShadowBolt_Timer = 15000;
            ManaBurn_Timer = 3000;
            ShadowShield_Timer = 8000;
            Strike_Timer = 12000;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
        
        void JustDied(Unit* killer)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //ShadowBolt_Timer
            if (ShadowBolt_Timer < diff)
            {
                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM,0);
                if (target) DoCast(target,SPELL_SHADOWBOLT);
                ShadowBolt_Timer = 7000;
            }else ShadowBolt_Timer -= diff;
    
            //ManaBurn_Timer
            if (ManaBurn_Timer < diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                    DoCast(target,SPELL_MANABURN);
    
                ManaBurn_Timer = 13000;
            }else ManaBurn_Timer -= diff;
    
            //ShadowShield_Timer
            if (ShadowShield_Timer < diff)
            {
                DoCast(me,SPELL_SHADOWSHIELD);
                ShadowShield_Timer = 25000;
            }else ShadowShield_Timer -= diff;
    
            //Strike_Timer
            if (Strike_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_STRIKE);
                Strike_Timer = 10000;
            }else Strike_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_haterelAI(creature);
    }
};


#define SPELL_MINDBLAST             15587
#define SPELL_HEAL                  15586
#define SPELL_PRAYEROFHEALING       15585
#define SPELL_SHIELD                10901


class boss_vilerel : public CreatureScript
{
public:
    boss_vilerel() : CreatureScript("boss_vilerel")
    { }

    class boss_vilerelAI : public ScriptedAI
    {
        public:
        boss_vilerelAI(Creature *c) : ScriptedAI(c) 
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 MindBlast_Timer;
        uint32 Heal_Timer;
        uint32 PrayerOfHealing_Timer;
        uint32 Shield_Timer;
        
        InstanceScript* pInstance;
    
        void Reset()
        override {
            MindBlast_Timer = 10000;
            Heal_Timer = 35000;
            PrayerOfHealing_Timer = 25000;
            Shield_Timer = 3000;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
        
        void JustDied(Unit* killer)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //MindBlast_Timer
            if (MindBlast_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_MINDBLAST);
                MindBlast_Timer = 7000;
            }else MindBlast_Timer -= diff;
    
            //Heal_Timer
            if (Heal_Timer < diff)
            {
                DoCast(me,SPELL_HEAL);
                Heal_Timer = 20000;
            }else Heal_Timer -= diff;
    
            //PrayerOfHealing_Timer
            if (PrayerOfHealing_Timer < diff)
            {
                DoCast(me,SPELL_PRAYEROFHEALING);
                PrayerOfHealing_Timer = 30000;
            }else PrayerOfHealing_Timer -= diff;
    
            //Shield_Timer
            if (Shield_Timer < diff)
            {
                DoCast(me,SPELL_SHIELD);
                Shield_Timer = 30000;
            }else Shield_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_vilerelAI(creature);
    }
};


#define SPELL_FROSTBOLT         16799
#define SPELL_FROSTARMOR        15784                       //This is actually a buff he gives himself
#define SPELL_BLIZZARD          19099
#define SPELL_FROSTNOVA         15063
#define SPELL_FROSTWARD         15004


class boss_seethrel : public CreatureScript
{
public:
    boss_seethrel() : CreatureScript("boss_seethrel")
    { }

    class boss_seethrelAI : public ScriptedAI
    {
        public:
        boss_seethrelAI(Creature *c) : ScriptedAI(c) 
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 FrostArmor_Timer;
        uint32 Frostbolt_Timer;
        uint32 Blizzard_Timer;
        uint32 FrostNova_Timer;
        uint32 FrostWard_Timer;
        
        InstanceScript* pInstance;
    
        void Reset()
        override {
            FrostArmor_Timer = 2000;
            Frostbolt_Timer = 6000;
            Blizzard_Timer = 18000;
            FrostNova_Timer = 12000;
            FrostWard_Timer = 25000;
    
            me->CastSpell(me,SPELL_FROSTARMOR,true);
        }
    
        void EnterCombat(Unit *who)
        override {
        }
        
        void JustDied(Unit* killer)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim())
                return;
    
            //FrostArmor_Timer
            if (FrostArmor_Timer < diff)
            {
                DoCast(me, SPELL_FROSTARMOR);
                FrostArmor_Timer = 180000;
            }else FrostArmor_Timer -= diff;
    
            //Frostbolt_Timer
            if (Frostbolt_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_FROSTBOLT);
                Frostbolt_Timer = 15000;
            }else Frostbolt_Timer -= diff;
    
            //Blizzard_Timer
            if (Blizzard_Timer < diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                    DoCast(target,SPELL_BLIZZARD);
    
                Blizzard_Timer = 22000;
            }else Blizzard_Timer -= diff;
    
            //FrostNova_Timer
            if (FrostNova_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_FROSTNOVA);
                FrostNova_Timer = 14000;
            }else FrostNova_Timer -= diff;
    
            //FrostWard_Timer
            if (FrostWard_Timer < diff)
            {
                DoCast(me,SPELL_FROSTWARD);
                FrostWard_Timer = 68000;
            }else FrostWard_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_seethrelAI(creature);
    }
};


#define SPELL_HAMSTRING             9080
#define SPELL_CLEAVE                15579
#define SPELL_MORTALSTRIKE          15708


#define GOSSIP_ITEM_TEACH_1 "Teach me the art of smelting dark iron"
#define GOSSIP_ITEM_TEACH_2 "Continue..."
#define GOSSIP_ITEM_TRIBUTE "I want to pay tribute"


class boss_gloomrel : public CreatureScript
{
public:
    boss_gloomrel() : CreatureScript("boss_gloomrel")
    { }

    class boss_gloomrelAI : public ScriptedAI
    {
        public:
        boss_gloomrelAI(Creature *c) : ScriptedAI(c) 
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        uint32 Hamstring_Timer;
        uint32 Cleave_Timer;
        uint32 MortalStrike_Timer;
        
        InstanceScript* pInstance;
    
        void Reset()
        override {
            Hamstring_Timer = 19000;
            Cleave_Timer = 6000;
            MortalStrike_Timer = 10000;
    
            me->SetFaction(FACTION_NEUTRAL);
        }
    
        void EnterCombat(Unit *who)
        override {
        }
        
        void JustDied(Unit* killer)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim() )
                return;
    
            //Hamstring_Timer
            if (Hamstring_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_HAMSTRING);
                Hamstring_Timer = 14000;
            }else Hamstring_Timer -= diff;
    
            //Cleave_Timer
            if (Cleave_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CLEAVE);
                Cleave_Timer = 8000;
            }else Cleave_Timer -= diff;
    
            //MortalStrike_Timer
            if (MortalStrike_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_MORTALSTRIKE);
                MortalStrike_Timer = 12000;
            }else MortalStrike_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }

        virtual bool GossipHello(Player* player) override
        {
            if (player->GetQuestRewardStatus(4083) == 1 && player->GetSkillValue(SKILL_MINING) >= 230 && !player->HasSpell(14891) )
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (player->GetQuestRewardStatus(4083) == 0 && player->GetSkillValue(SKILL_MINING) >= 230)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRIBUTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                
            player->SEND_GOSSIP_MENU_TEXTID(2601, me->GetGUID());

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                    player->SEND_GOSSIP_MENU_TEXTID(2606, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+11:
                    player->CLOSE_GOSSIP_MENU();
                    me->CastSpell(player, 14894, false);
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] Continue...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
                    player->SEND_GOSSIP_MENU_TEXTID(2604, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+22:
                    player->CLOSE_GOSSIP_MENU();
                    if (pInstance)
                    {
                        //are 5 minutes expected? go template may have data to despawn when used at quest
                        pInstance->DoRespawnGameObject(pInstance->GetData64(DATA_GO_CHALICE),MINUTE*5);
                    }
                    break;
            }
            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_gloomrelAI(creature);
    }
};


enum DoomrelSpells
{
    SPELL_SHADOWBOLTVOLLEY                                 = 15245,
    SPELL_IMMOLATE                                         = 12742,
    SPELL_CURSEOFWEAKNESS                                  = 12493,
    SPELL_DEMONARMOR                                       = 13787,
    SPELL_SUMMON_VOIDWALKERS                               = 15092
};

#define GOSSIP_ITEM_CHALLENGE   "Your bondage is at an end, Doom'rel. I challenge you!"


class boss_doomrel : public CreatureScript
{
public:
    boss_doomrel() : CreatureScript("boss_doomrel")
    { }

    class boss_doomrelAI : public ScriptedAI
    {
        public:
        boss_doomrelAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript* pInstance;
        uint32 ShadowVolley_Timer;
        uint32 Immolate_Timer;
        uint32 CurseOfWeakness_Timer;
        uint32 DemonArmor_Timer;
        bool Voidwalkers;
        int Rand;
        int RandX;
        int RandY;
        Creature* Summoned;
    
        void Reset()
        override {
            ShadowVolley_Timer = 10000;
            Immolate_Timer = 18000;
            CurseOfWeakness_Timer = 5000;
            DemonArmor_Timer = 16000;
            Voidwalkers = false;
    
            me->SetFaction(FACTION_NEUTRAL);
            
            // was set before event start, so set again
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
    
            if (pInstance)
            {
                if (pInstance->GetData(DATA_GHOSTKILL) >= 7)
                    me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                else
                    me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void EnterEvadeMode(EvadeReason /* why */)
        override {
            me->RemoveAllAuras();
            me->GetThreatManager().ClearAllThreat();
            me->CombatStop(true);
            me->InitCreatureAddon();
            if (me->IsAlive())
                me->GetMotionMaster()->MoveTargetedHome();
            me->SetLootRecipient(nullptr);
            if (pInstance)
                pInstance->SetData64(DATA_EVENSTARTER, 0);
        }
        
        void JustDied(Unit *who)
        override {
            if (pInstance)
                pInstance->SetData(DATA_GHOSTKILL, 1);
        }
    
        void SummonVoidwalkers(Unit* victim)
        {
            Rand = rand()%5;
            switch (rand()%2)
            {
                case 0: RandX = 0 - Rand; break;
                case 1: RandX = 0 + Rand; break;
            }
            Rand = 0;
            Rand = rand()%5;
            switch (rand()%2)
            {
                case 0: RandY = 0 - Rand; break;
                case 1: RandY = 0 + Rand; break;
            }
            Rand = 0;
            Summoned = DoSpawnCreature(16119, RandX, RandY, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 120000);
            if(Summoned)
                ((CreatureAI*)Summoned->AI())->AttackStart(victim);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (!UpdateVictim() )
                return;
    
            //ShadowVolley_Timer
            if (ShadowVolley_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_SHADOWBOLTVOLLEY);
                ShadowVolley_Timer = 12000;
            }else ShadowVolley_Timer -= diff;
    
            //Immolate_Timer
            if (Immolate_Timer < diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                    DoCast(target,SPELL_IMMOLATE);
    
                Immolate_Timer = 25000;
            }else Immolate_Timer -= diff;
    
            //CurseOfWeakness_Timer
            if (CurseOfWeakness_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_CURSEOFWEAKNESS);
                CurseOfWeakness_Timer = 45000;
            }else CurseOfWeakness_Timer -= diff;
    
            //DemonArmor_Timer
            if (DemonArmor_Timer < diff)
            {
                DoCast(me,SPELL_DEMONARMOR);
                DemonArmor_Timer = 300000;
            }else DemonArmor_Timer -= diff;
    
            //Summon Voidwalkers
            if (!Voidwalkers && me->GetHealthPct() < 51 )
            {
                SummonVoidwalkers(me->GetVictim());
                SummonVoidwalkers(me->GetVictim());
                SummonVoidwalkers(me->GetVictim());
                Voidwalkers = true;
            }
    
            DoMeleeAttackIfReady();
        }

        virtual bool GossipHello(Player* player) override
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU_TEXTID(2601, me->GetGUID());

            return true;

        }


        virtual bool GossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "[PH] Continue...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->SEND_GOSSIP_MENU_TEXTID(2605, me->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->CLOSE_GOSSIP_MENU();
                    //start event here
                    me->SetFaction(FACTION_HOSTILE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->AI()->AttackStart(player);
                    if (pInstance)
                        pInstance->SetData64(DATA_EVENSTARTER,player->GetGUID());
                    break;
            }
            return true;

        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_doomrelAI(creature);
    }
};


void AddSC_boss_tomb_of_seven()
{

    new boss_angerrel();

    new boss_doperel();

    new boss_haterel();

    new boss_vilerel();

    new boss_seethrel();

    new boss_gloomrel();

    new boss_doomrel();
}
