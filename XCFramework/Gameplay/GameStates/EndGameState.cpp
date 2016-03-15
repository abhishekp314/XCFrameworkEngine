/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/EndGameState.h"


using namespace GameState;

EndGameState::EndGameState(void)
{
    m_isPaused = false;
}

EndGameState::~EndGameState(void)
{
}

void EndGameState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);
}

void EndGameState::Update(float dt)
{
}

void EndGameState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void EndGameState::Destroy()
{
    IGameState::Destroy();
}