#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "SpellAuras.h"

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
    SPELL_REMORSELESS_WINTER    = 68981,
    SPELL_BERSERK               = 59620,
    SPELL_ICY_GRIP              = 70177,
    SPELL_ICY_CHAINS            = 29991,
    SPELL_CHARGE                = 74399
};

enum Events
{
    EVENT_SPEED_UP  = 1,
    EVENT_SLEEPY,
    EVENT_WAKE_UP,
    EVENT_BEGIN_WALKING,
    EVENT_ICY_GRIP,
    EVENT_CHARGE_WALL
};

enum Misc
{
    NPC_BUNNY  = 22515
};

static float const AGGRO_DISTANCE = 60.0f;

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
        boss_custom_dps1AI(Creature* creature) : ScriptedAI(creature), _weakenCount(0), _speedupCount(0), _timeSinceDamage(0) { }

        void InitializeAI()
        {
            me->Respawn();
            SetCombatMovement(false);
        }

        void Reset()
        {
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
            me->SetCorpseDelay(5);
            me->SetReactState(REACT_PASSIVE);
            _playerGUID = ObjectGuid::Empty;
        }

        void ResetEncounter()
        {
            _playerGUID = ObjectGuid::Empty;
            me->DespawnOrUnsummon();
        }

        void BeginEncounter(Unit* who)
        {
            if (IsEncounterActive())
                return;

            Player* pWho = who->GetCharmerOrOwnerPlayerOrPlayerItself();

            if (!pWho)
            {
                std::cout << "no player engaging" << std::endl;
                ResetEncounter();
                return;
            }

            if (pWho->IsGameMaster())
                return;

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
            _events.ScheduleEvent(EVENT_SPEED_UP, 10 * IN_MILLISECONDS);
        }

        void DamageTaken(Unit* source, uint32& damage) override
        {
           Player* pSource = source ? source->GetCharmerOrOwnerPlayerOrPlayerItself() : nullptr;
           if (!pSource || pSource->GetGUID() != _playerGUID)
           {
               std::cout << "Anticheat prevented " << damage << " damage." << std::endl;
               damage = 0;
               if (pSource && !pSource->HasAura(SPELL_BANISH))
               {
                   Talk(YELL_ANTICHEAT);
                   pSource->CastSpell(pSource, SPELL_BANISH, true);
               }
           }
           _timeSinceDamage = 0;
        }

        inline bool IsEncounterActive() { return !_playerGUID.IsEmpty(); }

        void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
        {
            std::cout << "EnterEvadeMode" << std::endl;
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

            if (_timeSinceDamage > 10 * IN_MILLISECONDS)
            {
                std::cout << "damage timeout" << std::endl;
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
                        }
                        else
                        {
                            Talk(YELL_BERSERK);
                            Talk(EMOTE_BERSERK);
                            DoCastAOE(SPELL_BERSERK);
                            DoCastAOE(SPELL_REMORSELESS_WINTER);

                            _events.ScheduleEvent(EVENT_CHARGE_WALL, 3 * IN_MILLISECONDS);
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
                        if (Creature* bunny = DoSummon(NPC_BUNNY, weakenPositions[numWeaken], 5 * IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                            DoCast(bunny, SPELL_CHARGE);
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACK1H);
                        break;
                }
        }

        void JustDied(Unit* killer) override
        {
            Talk(YELL_DEATH);
            ScriptedAI::JustDied(killer);
        }

        ObjectGuid _playerGUID;
        uint8 _weakenCount;
        uint8 _speedupCount;
        EventMap _events;
        uint32 _timeSinceDamage;
    };
};
void AddSC_boss_custom_dps1()
{
    new boss_custom_dps1();
}