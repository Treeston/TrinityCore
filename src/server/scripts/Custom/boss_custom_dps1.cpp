#include <boost/format.hpp>
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "SpellAuras.h"

static const uint32 BASECENTRY = 100000;
static const uint32 BASEOENTRY = 1000000;
static const uint32 BASEGOSSIP = 60400;
static const uint32 BASETEXT = 16740000;
static const WorldLocation ENTRY_POINT(169, -76.59f, 2410.39f, 92.008f, 2.6429f);
enum WreckerTalk
{
    YELL_ENCOUNTER_BEGIN        = 1, // Little creature watch while I smash!
    EMOTE_ENCOUNTER_BEGIN       = 2, // %s begins to charge the wall! Take it down before it can get there!

    YELL_WEAKEN                 = 3, // Tired. So...tired.
    EMOTE_WEAKEN                = 4, // %s is weakened while taking a rest. Strike now!

    YELL_WEAKEN_END             = 5, // Coming for you now, puny mortals!
    EMOTE_WEAKEN_END            = 6, // %s charges at the barrier with renewed strength!

    YELL_BERSERK                = 7, // Wrecker tired of games. Now, smash!
    EMOTE_BERSERK               = 8, // %s begins to relentlessly pound the wall! Take it down!

    YELL_ANTICHEAT              = 9, // Hmph. Friend will not help you here.

    YELL_DEATH                  =10, // Why... So tired....
};

enum Spells
{
    SPELL_SLEEPY                = 30457,
    SPELL_SLEEPY_VISUAL         =  6606,
    SPELL_BANISH                = 42354,
    SPELL_SPEED_UP              = 62375,
    SPELL_BERSERK_AURA          = 70084,
    SPELL_BERSERK               = 41924,
    SPELL_ICY_GRIP              = 70177,
    SPELL_ICY_CHAINS            = 29991,
    SPELL_CHARGE                = 74399,
    SPELL_TELEPORT_VISUAL       = 64446
};

enum Events
{
    EVENT_SPEED_UP  = 1,
    EVENT_SLEEPY,
    EVENT_WAKE_UP,
    EVENT_BEGIN_WALKING,
    EVENT_ICY_GRIP,
    EVENT_CHARGE_WALL,
    EVENT_POWER_DOWN
};

enum Data
{
    DATA_IS_ENGAGED = 1
};

enum GossipMenuEntries
{
    GOSSIP_WRECKER_DIFFICULTY = BASETEXT+0,
    GOSSIP_WRECKER_BUSY = BASETEXT+1
};

enum Entries
{
    NPC_WRECKER = BASECENTRY + 0,
    NPC_NOVARCH = BASECENTRY + 1,

    GO_GATE = BASEOENTRY + 0
};

static float const AGGRO_DISTANCE = 40.0f;

static uint8 const numWeaken = 4;
Position weakenPositions[numWeaken+1] =
{
    { -189.8f, 2454.2f, 92.008f },
    { -251.7f, 2454.2f, 92.008f },
    { -313.6f, 2454.2f, 92.008f },
    { -375.5f, 2454.2f, 92.008f/*, M_PI*/ },
    { -430.3f, 2454.2f, 92.008f }
};

class boss_custom_dps1 : public CreatureScript
{
public:
    boss_custom_dps1() : CreatureScript("boss_custom_dps1") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_custom_dps1AI(creature);
    }

    struct boss_custom_dps1AI : public ScriptedAI
    {
        boss_custom_dps1AI(Creature* creature) : ScriptedAI(creature), _startTime(0), _weakenCount(0), _speedupCount(0), _timeSinceDamage(0) { }

        void InitializeAI()
        {
            me->Respawn();
            SetCombatMovement(false);
            Reset();
        }

        void Reset()
        {
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
            me->SetCorpseDelay(5);
            me->SetReactState(REACT_PASSIVE);
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 20);
            _playerGUID = ObjectGuid::Empty;
        }

        void ResetEncounter()
        {
            if (_playerGUID)
                if (Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID))
                {
                    player->CastSpell(player, SPELL_TELEPORT_VISUAL, true);
                    player->TeleportTo(ENTRY_POINT);
                }
            _playerGUID = ObjectGuid::Empty;
            me->DespawnOrUnsummon();
        }

        void BeginEncounter(Unit* who)
        {
            if (IsEncounterActive())
                return;

            if (who->GetEntry() == NPC_NOVARCH)
                return;

            Player* pWho = who->GetCharmerOrOwnerPlayerOrPlayerItself();

            if (!pWho)
            {
                ResetEncounter();
                return;
            }

            if (pWho->IsGameMaster())
                return;

            _startTime = getMSTime();
            _playerGUID = pWho->GetGUID();
            _weakenCount = 0;
            _speedupCount = 1;
            _timeSinceDamage = 0;

            Talk(YELL_ENCOUNTER_BEGIN, pWho);
            me->AddThreat(pWho, 0.0f);
            pWho->SetInCombatWith(me);
            me->SetInCombatWith(pWho);
            me->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid::Empty);

            _events.Reset();
            _events.ScheduleEvent(EVENT_BEGIN_WALKING, 3 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_SLEEPY, 20 * IN_MILLISECONDS);
            me->SetPower(POWER_ENERGY, 20);
            _events.ScheduleEvent(EVENT_SPEED_UP, 10 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_POWER_DOWN, 1 * IN_MILLISECONDS);

            std::list<GameObject*> gateGOs;
            me->GetGameObjectListWithEntryInGrid(gateGOs, GO_GATE, 80.0f);
            for (GameObject* gate : gateGOs)
                gate->SetGoState(GO_STATE_READY);
        }

        void DamageTaken(Unit* source, uint32& damage) override
        {
           Player* pSource = source ? source->GetCharmerOrOwnerPlayerOrPlayerItself() : nullptr;
           if (!pSource || pSource->GetGUID() != _playerGUID)
           {
               damage = 0;
               if (pSource && !pSource->HasAura(SPELL_BANISH))
               {
                   Talk(YELL_ANTICHEAT);
                   pSource->CastSpell(pSource, SPELL_BANISH, true);
               }
           }
           _timeSinceDamage = 0;
        }

        inline bool IsEncounterActive() const { return !_playerGUID.IsEmpty(); }

        uint32 GetData(uint32 data) const override
        {
            switch (data)
            {
                case DATA_IS_ENGAGED:
                    return IsEncounterActive() ? 1u : 0u;
                default:
                    return 0u;
            }
        }

        void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
        {
            ResetEncounter();
        }

        void EnterCombat(Unit* who) override
        {
            BeginEncounter(who);
        }
        
        void MoveInLineOfSight(Unit* who) override
        {
            if (me->GetDistance2d(who) <= AGGRO_DISTANCE)
                BeginEncounter(who);
        }

        void SpellHitTarget(Unit* who, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_ICY_GRIP)
                me->CastSpell(who, SPELL_ICY_CHAINS);
        }

        void UpdateAI(uint32 diff) override
        {
            _timeSinceDamage += diff;
            if (!UpdateVictim())
                return;

            if (_weakenCount < numWeaken && _timeSinceDamage > 10 * IN_MILLISECONDS)
            {
                ResetEncounter();
                return;
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_SPEED_UP:
                        me->AddAura(SPELL_SPEED_UP, me);
                        if (--_speedupCount)
                            _events.ScheduleEvent(EVENT_SPEED_UP, 10 * IN_MILLISECONDS);
                        break;

                    case EVENT_SLEEPY:
                        ++_weakenCount;
                        me->RemoveAura(SPELL_SPEED_UP);
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        Talk(YELL_WEAKEN);
                        Talk(EMOTE_WEAKEN);
                        if (Aura* aura = me->AddAura(SPELL_SLEEPY, me))
                        {
                            aura->SetDuration(15 * IN_MILLISECONDS);
                            aura->SetMaxDuration(15 * IN_MILLISECONDS);
                        }
                        else
                            ResetEncounter();
                        me->AddAura(SPELL_SLEEPY_VISUAL, me);
                        _events.ScheduleEvent(EVENT_WAKE_UP, 15 * IN_MILLISECONDS);
                        break;
                        
                    case EVENT_WAKE_UP:
                        me->RemoveAura(SPELL_SLEEPY_VISUAL);
                        me->RemoveAura(SPELL_SLEEPY);

                        if (_weakenCount < numWeaken)
                        {
                            Talk(YELL_WEAKEN_END);
                            _speedupCount = 5;
                            _events.ScheduleEvent(EVENT_BEGIN_WALKING, 2 * IN_MILLISECONDS);
                            _events.ScheduleEvent(EVENT_SPEED_UP, 10 * IN_MILLISECONDS);
                            _events.ScheduleEvent(EVENT_SLEEPY, MINUTE * IN_MILLISECONDS);
                            me->SetPower(POWER_ENERGY, MINUTE);
                            _events.ScheduleEvent(EVENT_POWER_DOWN, 1 * IN_MILLISECONDS);
                        }
                        else
                        {
                            Talk(YELL_BERSERK);

                            me->SetPower(POWER_ENERGY, 100);

                            _events.ScheduleEvent(EVENT_CHARGE_WALL, 2 * IN_MILLISECONDS);
                            _events.ScheduleEvent(EVENT_ICY_GRIP, 10 * IN_MILLISECONDS);
                        }
                        break;

                    case EVENT_BEGIN_WALKING:
                        if (_weakenCount)
                            Talk(EMOTE_WEAKEN_END);
                        else
                            Talk(EMOTE_ENCOUNTER_BEGIN);
                        me->GetMotionMaster()->MovePoint(0, weakenPositions[_weakenCount]);
                        break;

                    case EVENT_ICY_GRIP:
                        DoCastAOE(SPELL_ICY_GRIP);
                        _events.ScheduleEvent(EVENT_ICY_GRIP, 10 * IN_MILLISECONDS);
                        break;

                    case EVENT_CHARGE_WALL:
                        Talk(EMOTE_BERSERK);
                        me->GetMotionMaster()->MoveCharge(weakenPositions[numWeaken].GetPositionX(), weakenPositions[numWeaken].GetPositionY(), weakenPositions[numWeaken].GetPositionZ(), 42.0f, 2);
                        break;

                    case EVENT_POWER_DOWN:
                        me->SetPower(POWER_ENERGY, _events.GetTimeUntilEvent(EVENT_SLEEPY) / IN_MILLISECONDS);
                        if (me->GetPower(POWER_ENERGY))
                            _events.ScheduleEvent(EVENT_POWER_DOWN, 1 * IN_MILLISECONDS);
                        break;
                }
        }

        void MovementInform(uint32 movetype, uint32 id)
        {
            if (id == 2)
            {
                DoCastAOE(SPELL_BERSERK);
                DoCastAOE(SPELL_BERSERK_AURA);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACK1H);
            }
        }

        void JustDied(Unit* killer) override
        {
            Player* pKiller = killer->GetCharmerOrOwnerPlayerOrPlayerItself();
            if (!pKiller || pKiller->GetGUID() != _playerGUID)
            {
                ResetEncounter();
                return;
            }

            uint32 killTime = GetMSTimeDiffToNow(_startTime);
            Talk(YELL_DEATH);
            me->Talk(Trinity::StringFormat("%%s has been defeated by $n in %d.%d seconds!", killTime / IN_MILLISECONDS, killTime % IN_MILLISECONDS), CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, 50000.0f, pKiller);
            ScriptedAI::JustDied(killer);
        }

        uint32 _startTime;
        ObjectGuid _playerGUID;
        uint8 _weakenCount;
        uint8 _speedupCount;
        EventMap _events;
        uint32 _timeSinceDamage;
    };
};

class npc_custom_dps1_gatewatcher : public CreatureScript
{
public:
    npc_custom_dps1_gatewatcher() : CreatureScript("npc_custom_dps1_gatewatcher") { }

    inline static Creature* _wrecker(Creature const* self) { return self->FindNearestCreature(NPC_WRECKER, 100.0f, true); }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_custom_dps1_gatewatcherAI(creature);
    }

    struct npc_custom_dps1_gatewatcherAI : public ScriptedAI
    {
        npc_custom_dps1_gatewatcherAI(Creature* creature) : ScriptedAI(creature) { }
    };

    bool OnGossipSelect(Player* player, Creature* me, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
            {
                player->PlayerTalkClass->SendCloseGossip();
                std::list<GameObject*> gateGOs;
                me->GetGameObjectListWithEntryInGrid(gateGOs, GO_GATE, 30.0f);
                for (GameObject* gate : gateGOs)
                    gate->SetGoState(GO_STATE_ACTIVE);
                break;
            }
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* me) override
    {
        if (player->IsGameMaster())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[GM] Open the door for me.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        if (Creature* wrecker = _wrecker(me))
        {
            if (!wrecker->AI()->GetData(DATA_IS_ENGAGED))
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I will enter.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->PlayerTalkClass->SendGossipMenu(GOSSIP_WRECKER_DIFFICULTY, me->GetGUID());
                return true;
            }
        }
        player->PlayerTalkClass->SendGossipMenu(GOSSIP_WRECKER_BUSY, me->GetGUID());
        return true;
    }
};

void AddSC_boss_custom_dps1()
{
    new boss_custom_dps1();
    new npc_custom_dps1_gatewatcher();
}
