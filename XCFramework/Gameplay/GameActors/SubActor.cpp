/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameActors/SubActor.h"

SubActor::SubActor()
{
}

void SubActor::Init(IActor* bindedActor)
{
    m_bindedActor = bindedActor;

    m_offsetPosition = m_bindedActor->GetPosition();
    m_offsetLook = m_bindedActor->GetLook();
    m_offsetRight = m_bindedActor->GetRight();
    m_offsetUp = m_bindedActor->GetUp();

    m_offsetRotation = XMMatrixIdentity();
}

SubActor::~SubActor(void)
{
}

void SubActor::InitOffsets(XCVecIntrinsic4 position, XCVecIntrinsic4 look, XCVecIntrinsic4 right, XCVecIntrinsic4 up)
{
    m_offsetPosition = position;
    m_offsetLook = look;
    m_offsetRight = right;
    m_offsetUp = up;
}

void SubActor::UpdateOffsets(float dt)
{
}