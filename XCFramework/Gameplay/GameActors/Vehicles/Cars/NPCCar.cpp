/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "NPCCar.h"
#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

NPCCar::NPCCar(void)
{
}

NPCCar::~NPCCar(void)
{
}

void NPCCar::PreLoad(const void* fbBuffer)
{
    const FBCar* carBuff = (FBCar*)fbBuffer;

    Car::PreLoad(XCVec3(carBuff->Position()->x(), carBuff->Position()->y(), carBuff->Position()->z()), carBuff->XCMeshResourceName()->c_str());
    PhysicsActor::PreLoad(fbBuffer);
}

void NPCCar::Update(float dt)
{
    Integrator(dt);
    ClearForce();

    XCVec3 pos;
    XMStoreFloat3(&pos, m_Position);

    m_MTranslation = XMMatrixTranslation(pos.x, pos.y, pos.z);

    m_currentPosition = m_Position;

    Car::Update(dt);
}

void NPCCar::Destroy()
{
    Car::Destroy();
}