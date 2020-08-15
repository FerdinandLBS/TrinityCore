#include <SpellUtility.h>

SpellDelayCastEvent::SpellDelayCastEvent(uint32 spell, WorldObject* caster, WorldObject* target, CastSpellExtraArgs& args) {
    m_Spell = spell;
    objectTarget = target;
    spellArgs = args;
    mode = 0;
    m_Caster = caster;
}

SpellDelayCastEvent::SpellDelayCastEvent(uint32 spell, WorldObject* caster, SpellCastTargets& targets, CastSpellExtraArgs& args) {
    m_Spell = spell;
    spellTarget = targets;
    spellArgs = args;
    mode = 1;
    m_Caster = caster;
}

SpellDelayCastEvent::SpellDelayCastEvent(uint32 spell, WorldObject* caster, Position& targets, CastSpellExtraArgs& args) {
    m_Spell = spell;
    positionTarget = targets;
    spellArgs = args;
    mode = 2;
    m_Caster = caster;
}

SpellDelayCastEvent::~SpellDelayCastEvent() {

}

bool SpellDelayCastEvent::Execute(uint64 e_time, uint32 p_time) {
    SpellCastResult result;

    if (!m_Caster)
        return false;

    switch (mode) {
    case 0:
        result = m_Caster->CastSpell(objectTarget, m_Spell, spellArgs);
        break;
    case 1:
        result = m_Caster->CastSpell(spellTarget, m_Spell, spellArgs);
        break;
    case 2:
        result = m_Caster->CastSpell(positionTarget, m_Spell, spellArgs);
        break;
    }

    if (result == SpellCastResult::SPELL_CAST_OK) {
        return true;
    }
    return false;
}
