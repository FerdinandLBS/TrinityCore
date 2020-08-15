
#ifndef _SPELL_UTILITY_H_
#define _SPELL_UTILITY_H_

#include <EventProcessor.h>
#include <Unit.h>
#include <Spell.h>

class TC_GAME_API SpellDelayCastEvent : public BasicEvent
{
public:
    SpellDelayCastEvent(uint32 spell, WorldObject* caster, WorldObject* target, CastSpellExtraArgs& args);
    SpellDelayCastEvent(uint32 spell, WorldObject* caster, SpellCastTargets& targets, CastSpellExtraArgs& args);
    SpellDelayCastEvent(uint32 spell, WorldObject* caster, Position& targets, CastSpellExtraArgs& args);
    ~SpellDelayCastEvent();

    bool Execute(uint64 e_time, uint32 p_time) override;
    void Abort(uint64 e_time) override {};
    bool IsDeletable() const override { return true; };

protected:
    WorldObject* m_Caster;
    uint32 m_Spell;
    WorldObject* objectTarget;
    SpellCastTargets spellTarget;
    Position positionTarget;
    CastSpellExtraArgs spellArgs;
    short mode;
};

#endif
