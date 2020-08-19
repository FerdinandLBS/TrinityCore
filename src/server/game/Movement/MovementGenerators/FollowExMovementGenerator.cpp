#include "FollowExMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Optional.h"
#include "PathGenerator.h"
#include "Pet.h"
#include "Unit.h"
#include "Util.h"

static void DoMovementInform(Unit* owner, Unit* target)
{
    if (owner->GetTypeId() != TYPEID_UNIT)
        return;

    if (CreatureAI* AI = owner->ToCreature()->AI())
        AI->MovementInform(FOLLOW_MOTION_TYPE, target->GetGUID().GetCounter());
}

FollowExMovementGenerator::FollowExMovementGenerator(Unit* target, float range, ChaseAngle angle, float minHeight) :
    AbstractFollower(ASSERT_NOTNULL(target)), _range(range), _angle(angle)
{
    Mode = MOTION_MODE_DEFAULT;
    Priority = MOTION_PRIORITY_NORMAL;
    Flags = MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING;
    BaseUnitState = UNIT_STATE_FOLLOW;
    _minHeight = minHeight;
}
FollowExMovementGenerator::~FollowExMovementGenerator() = default;

static bool PositionOkay(Unit* owner, Unit* target, float range, float* distance, Optional<ChaseAngle> angle = {})
{
    float dis = owner->GetExactDistSq(target);

    if (distance)
        *distance = std::sqrt(dis)/3;

    if (dis > square(owner->GetCombatReach() + target->GetCombatReach() + range))
        return false;

    return !angle || angle->IsAngleOkay(target->GetRelativeAngle(owner));
}

void FollowExMovementGenerator::Initialize(Unit* owner)
{
    RemoveFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING | MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    AddFlag(MOVEMENTGENERATOR_FLAG_INITIALIZED);

    owner->StopMoving();
    UpdatePetSpeed(owner);

    _walkSpeed = owner->GetSpeed(UnitMoveType::MOVE_WALK);
    _swimSpeed = owner->GetSpeed(UnitMoveType::MOVE_SWIM);
    _runSpeed = owner->GetSpeed(UnitMoveType::MOVE_RUN);
    _path = nullptr;
    _lastTargetPosition.reset();
}

void FollowExMovementGenerator::Reset(Unit* owner)
{
    RemoveFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);

    Initialize(owner);
}

bool FollowExMovementGenerator::Update(Unit* owner, uint32 diff)
{
    float distance = 0;

    // owner might be dead or gone
    if (!owner || !owner->IsAlive())
        return false;

    // our target might have gone away
    Unit* const target = GetTarget();
    if (!target || !target->IsInWorld())
        return false;

    if (owner->IsFalling())
        return true;

    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting())
    {
        owner->StopMoving();
        _lastTargetPosition.reset();
        return true;
    }

    if (owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE))
    {
        if (_checkTimer > diff)
            _checkTimer -= diff;
        else
        {
            _checkTimer = 100;
            if (PositionOkay(owner, target, _range, &distance, _angle))
            {
                _path = nullptr;
                owner->StopMoving();
                owner->SetSpeed(MOVE_WALK, _walkSpeed);
                owner->SetSpeed(MOVE_RUN, _runSpeed);
                owner->SetSpeed(MOVE_SWIM, _swimSpeed);
                DoMovementInform(owner, target);
                return true;
            }
            else {
                float k = 0.3f * (distance) + 0.2f;

                if (k > 2)
                    k = 2;
                if (k < 0.5f)
                    k = 0.5f;

                owner->SetSpeed(MOVE_WALK, k * _walkSpeed);
                owner->SetSpeed(MOVE_RUN, k * _runSpeed);
                owner->SetSpeed(MOVE_SWIM, k * _swimSpeed);
            }
        }
    }

    if (owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE) && owner->movespline->Finalized())
    {
        _path = nullptr;
        owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
        DoMovementInform(owner, target);
    }

    if (!_lastTargetPosition || _lastTargetPosition->GetExactDistSq(target->GetPosition()) > 0.0f)
    {
        _lastTargetPosition = target->GetPosition();
        if (owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE) || !PositionOkay(owner, target, _range + FOLLOW_RANGE_TOLERANCE/2, nullptr))
        {
            if (!_path)
                _path = std::make_unique<PathGenerator>(owner);

            float x, y, z;

            // select angle
            float tAngle;
            float const curAngle = target->GetRelativeAngle(owner);
            if (_angle.IsAngleOkay(curAngle))
                tAngle = curAngle;
            else
            {
                float const diffUpper = Position::NormalizeOrientation(curAngle - _angle.UpperBound());
                float const diffLower = Position::NormalizeOrientation(_angle.LowerBound() - curAngle);
                if (diffUpper < diffLower)
                    tAngle = _angle.UpperBound();
                else
                    tAngle = _angle.LowerBound();
            }

            target->GetNearPoint(owner, x, y, z, _range, target->ToAbsoluteAngle(tAngle));

            if (owner->IsHovering())
                owner->UpdateAllowedPositionZ(x, y, z);

            bool success = _path->CalculatePath(x, y, z, false);
            if (!success || (_path->GetPathType() & PATHFIND_NOPATH))
            {
                owner->StopMoving();
                return true;
            }

            owner->AddUnitState(UNIT_STATE_FOLLOW_MOVE);

            Movement::MoveSplineInit init(owner);
            init.MovebyPath(_path->GetPath());
            init.SetWalk(target->IsWalking());
            if (owner->IsHovering())
                init.SetFly();
            init.SetFacing(target->GetOrientation());
            init.Launch();
        }
    }
    return true;
}

void FollowExMovementGenerator::Deactivate(Unit* owner)
{
    AddFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
}

void FollowExMovementGenerator::Finalize(Unit* owner, bool active, bool/* movementInform*/)
{
    AddFlag(MOVEMENTGENERATOR_FLAG_FINALIZED);
    if (active)
    {
        owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
        owner->SetSpeed(MOVE_WALK, _walkSpeed);
        owner->SetSpeed(MOVE_RUN, _runSpeed);
        owner->SetSpeed(MOVE_SWIM, _swimSpeed);
        UpdatePetSpeed(owner);
    }
}

void FollowExMovementGenerator::UpdatePetSpeed(Unit* owner)
{
    return;
    owner->UpdateSpeed(MOVE_RUN);
    owner->UpdateSpeed(MOVE_WALK);
    owner->UpdateSpeed(MOVE_SWIM);

}
