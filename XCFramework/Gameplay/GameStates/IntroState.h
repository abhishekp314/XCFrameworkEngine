/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Engine/Graphics/BasicGeometry/CubeMesh.h"


namespace GameState
{

    class IntroState :public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(IntroState)

        IntroState(void);
        virtual ~IntroState(void);

        void                Init(GameFiniteStateMachine& gameFSM);
        void                Update(float dt);
        void                Draw(XC_Graphics& graphicsSystem);
        void                Destroy();
    };

}