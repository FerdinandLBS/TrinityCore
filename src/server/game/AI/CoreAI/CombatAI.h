/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_COMBATAI_H
#define TRINITY_COMBATAI_H

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
        for (uint32 i = 0; i < MAX_CREATURE_SPELLS; i++) {
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

class TC_GAME_API AggressorAI : public CreatureAI
{
    public:
        explicit AggressorAI(Creature* c) : CreatureAI(c) { }
        Unit* SelectNextTarget(bool allowAutoSelect);
        void UpdateAI(uint32) override;
        static int32 Permissible(Creature const* creature);
};

typedef std::vector<uint32> SpellVct;

class TC_GAME_API CombatAI : public CreatureAI
{
    public:
        explicit CombatAI(Creature* c) : CreatureAI(c) { }

        void InitializeAI() override;
        void Reset() override;
        void JustEngagedWith(Unit* who) override;
        void JustDied(Unit* killer) override;
        void UpdateAI(uint32 diff) override;
        void SpellInterrupted(uint32 spellId, uint32 unTimeMs) override;

        static int Permissible(Creature const* /*creature*/) { return PERMIT_BASE_NO; }

    protected:
        EventMap events;
        SpellVct spells;
};

class TC_GAME_API CasterAI : public CombatAI
{
    public:
        explicit CasterAI(Creature* c) : CombatAI(c) { m_attackDist = MELEE_RANGE; }
        void InitializeAI() override;
        void AttackStart(Unit* victim) override { AttackStartCaster(victim, m_attackDist); }
        void UpdateAI(uint32 diff) override;
        void JustEngagedWith(Unit* /*who*/) override;
    private:
        float m_attackDist;
};

struct TC_GAME_API ArcherAI : public CreatureAI
{
    public:
        explicit ArcherAI(Creature* c);
        void AttackStart(Unit* who) override;
        void UpdateAI(uint32 diff) override;

        static int32 Permissible(Creature const* /*creature*/) { return PERMIT_BASE_NO; }

    protected:
        float m_minRange;
};

struct TC_GAME_API TurretAI : public CreatureAI
{
    public:
        explicit TurretAI(Creature* c);
        bool CanAIAttack(Unit const* who) const override;
        void AttackStart(Unit* who) override;
        void UpdateAI(uint32 diff) override;

        static int32 Permissible(Creature const* /*creature*/) { return PERMIT_BASE_NO; }

    protected:
        float m_minRange;
};

#define VEHICLE_CONDITION_CHECK_TIME 1000
#define VEHICLE_DISMISS_TIME 5000

struct TC_GAME_API VehicleAI : public CreatureAI
{
    public:
        explicit VehicleAI(Creature* creature);

        void UpdateAI(uint32 diff) override;
        void MoveInLineOfSight(Unit*) override { }
        void AttackStart(Unit*) override { }
        void OnCharmed(bool isNew) override;

        static int32 Permissible(Creature const* creature);

    private:
        void LoadConditions();
        void CheckConditions(uint32 diff);
        bool m_HasConditions;
        uint32 m_ConditionsTimer;
        bool m_DoDismiss;
        uint32 m_DismissTimer;
};

#endif
