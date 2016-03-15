/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/PhysicsActor.h"

enum ENavigationType
{
    NAVIGATE_IDLE,
    NAVIGATE_CHASE,
    NAVIGATE_STATIC_TARGET,

    NAVIGATE_NONE
};

class AINavigator
{
public:
    static const float                  DISTANCE_ABS;
    static const float                  LOOK_TARGET_ANGLE_ABS;

    AINavigator(PhysicsActor* actor);
    virtual ~AINavigator(void);

    void                                SetNavigationState(ENavigationType type) { m_navigateType = type; }
    void                                SetStaticTargetNavigation(XCVecIntrinsic4 target);
    void                                SetChaseNavigation(PhysicsActor* actor);

    XCVecIntrinsic4                     GetTarget() { return m_targetPosition; }

    void                                Update(float dt);

    void                                RotateTowardsTarget(float angle);
    bool                                IsLookingAtTarget();
    void                                MoveOnLook();
    void                                MoveOnPath();
    bool                                HasArrivedAtTarget();

    XCVecIntrinsic4                     ComputePathToTarget();

    void                                ResetNavigation();

protected:
    void                                NavigateByChasing();
    void                                NavigateByTarget();

private:
    XCVecIntrinsic4                     m_path;
    XCVecIntrinsic4                     m_targetPosition;

    PhysicsActor*                       m_bindedActor;
    PhysicsActor*                       m_chaseActor;

    ENavigationType                     m_navigateType;
    float                               m_currentDelta;
};