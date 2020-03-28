#ifndef ASSISTANCE_AI_H
#define ASSISTANCE_AI_H

#include "CreatureAI.h"

class Creature;

#define MAX_CREATURE_SPELL 8
#define MAX_TIMER_TYPE 2
#define ASSIST_GCD 1000

class TC_GAME_API AssistanceAI : public CreatureAI
{
public:
    enum CREATURE_SPELL_TIMER {
        SPELL_TIMER_CURRENT = 0,
        SPELL_TIMER_ORIGIN = 1
    };

    enum AI_ACTION_FLAG {
        AI_ACTION_NONE = 0,
        AI_ACTION_HIDE = 1,
        AI_ACTION_PASSIVE = 2,
        AI_ACTION_HOLD_POSITION = 3,
        AI_ACTION_STG_MOVE = 4
    };

public:
    uint32 AIFlag;
    explicit AssistanceAI(Creature* c) : CreatureAI(c) {
        AIFlag = AI_ACTION_NONE;
    }
    Unit* SelectNextTarget(bool allowAutoSelect);
    void UpdateAI(uint32) override;
    static int32 Permissible(Creature const* creature);
    void JustDied(Unit* /*killer*/);
private:
    bool timerReady = false;
    uint32 gcd;
    bool auraApplied[MAX_CREATURE_SPELL];
    uint32 spellsTimer[MAX_CREATURE_SPELL][MAX_TIMER_TYPE];

    void castSpell(WorldObject* target, uint32 index);
    void castSpell(const Position &dest, uint32 index);
    bool isSpellReady(uint32 index);
    void updateTimer(uint32 diff);
    void ResetPosition();
    bool isCaster();
    void EngagementStart(Unit * who);
    bool AssistantsSpell(uint32 diff);
    bool miniPetAI(uint32 diff);
    void UseInstanceHealing();
    float getSpecialFollowAngle() {
        /*
        85990: sword
        86001: tank
        85980: healer
        85970: magic
        85960: assistance
        */
        uint32 index = 0;
        for (uint32 i = 0; i < MAX_CREATURE_SPELL; i++) {
            switch (me->m_spells[i]) {
            case 85990:
                return 0.0f;
            case 86001:
                return static_cast<float>(1.6f*M_PI);
            case 85980:
                return static_cast<float>(0.8f*M_PI);
            case 85970:
                return static_cast<float>(0.4f*M_PI);
            case 85960:
                return static_cast<float>(1.2f*M_PI);
            }
        }
        return static_cast<float>(0.6f*M_PI);
    }
};

#endif
