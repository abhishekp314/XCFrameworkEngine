/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/LoadingWorldState.h"
#include "Gameplay/GameStates/GameStateTypes.h"
#include "Gameplay/GameFiniteStateMachine.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Assets/Packages/PackageConsts.h"

using namespace GameState;

LoadingWorldState::LoadingWorldState(void)
{
}

LoadingWorldState::~LoadingWorldState(void)
{
}

void GameState::LoadingWorldState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);
    World& world = SystemLocator::GetInstance()->RequestSystem<World>("World");

    m_worldLoader = new WorldSceneLoader(world, WORLD_DATA_FILEPATH);

    TaskManager& taskMgr = SystemLocator::GetInstance()->RequestSystem<TaskManager>("TaskManager");
    m_futureWorldLoaded = taskMgr.RegisterTask(m_worldLoader);
}

void LoadingWorldState::Update(float dt)
{
    //Wait for resource loader to complete the loading of resources. When done move to next state.
    if (m_futureWorldLoaded.get() > 0)
    {
        m_gameFSM->SetState("RunningState", STATE_DESTROY);
    }
}

void GameState::LoadingWorldState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void LoadingWorldState::Destroy()
{
    IGameState::Destroy();
}