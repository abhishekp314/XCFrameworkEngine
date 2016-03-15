/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameFiniteStateMachine.h"
#include "Engine/System/SystemLocator.h"

GameFiniteStateMachine::GameFiniteStateMachine()
{
}

GameFiniteStateMachine::~GameFiniteStateMachine(void)
{
    delete(m_worldSystem);
    delete(m_gameActorFactory);
    delete(m_gameStateFactory);
}

void GameFiniteStateMachine::Init()
{
    SystemContainer& container = (SystemContainer&)SystemLocator::GetInstance()->GetSystemContainer();

    container.RegisterSystem<GameStatesFactory>("GameStatesFactory");
    container.RegisterSystem<GameActorsFactory>("GameActorsFactory");
    container.RegisterSystem<World>("World");

    m_gameStateFactory = (GameStatesFactory*) &container.CreateNewSystem("GameStatesFactory");
    m_gameStateFactory->InitFactory();

    m_gameActorFactory = (GameActorsFactory*) &container.CreateNewSystem("GameActorsFactory");
    m_gameActorFactory->InitFactory();

    m_worldSystem = (World*)&container.CreateNewSystem("World");
    TaskManager* taskMgr = &SystemLocator::GetInstance()->RequestSystem<TaskManager>("TaskManager");
    m_worldSystem->Init(*taskMgr);


    m_IsGameExit = false;

    SetState("IntroState");
}

void GameFiniteStateMachine::Update(float dt)
{
    m_StateStack.back()->Update(dt);
}

void GameFiniteStateMachine::Draw(XC_Graphics& graphicsSystem)
{
    m_StateStack.back()->Draw(graphicsSystem);
}

void GameFiniteStateMachine::Destroy()
{
    for (auto& state : m_StateStack)
    {
        if (state)
        {
            state->Destroy();
        }
    }

    m_StateStack.clear();

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("GameStatesFactory");
    container.RemoveSystem("GameActorsFactory");
    container.RemoveSystem("World");

    m_worldSystem->Destroy();
    m_gameActorFactory->DestroyFactory();
    m_gameStateFactory->DestroyFactory();
}

void GameFiniteStateMachine::SetState(std::string stateName, EWhatToDoWithPreviousState _what)
{
    IGameState* tempState = nullptr;

    switch (_what)
    {
        case STATE_STORE:
            PushState(stateName);
            break;
        case STATE_NONE:
        case STATE_DESTROY:
            if (m_StateStack.size() > 0)
            {
                m_StateStack.back()->Destroy();
                PopState();
            }
            
            PushState(stateName);
            break;
        default: break;
    }

    if (!m_StateStack.back()->GetPauseState())
    {
        m_StateStack.back()->Init(*this);
    }

    m_CurrentGameState = stateName;
}

void GameFiniteStateMachine::PushState(std::string stateName)
{
    m_StateStack.push_back(m_gameStateFactory->CreateState(stateName));
}

IGameState* GameFiniteStateMachine::PopState()
{
    IGameState* temp = m_StateStack.back();
    m_StateStack.pop_back();
    return temp;
}

std::string GameFiniteStateMachine::GetCurrentState()
{
    return m_CurrentGameState;
}

void GameFiniteStateMachine::SetGameExit(bool value)
{
    m_IsGameExit = value;
}

bool GameFiniteStateMachine::GetGameExit()
{
    return m_IsGameExit;
} 