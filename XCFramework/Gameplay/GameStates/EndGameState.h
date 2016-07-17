/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"

namespace GameState
{
    class EndGameState : public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(EndGameState)

        EndGameState(void);
        virtual ~EndGameState(void);

        void                    Init();
        void                    Update(f32 dt);
        void                    Draw(XC_Graphics& graphicsSystem);
        void                    Destroy();
    };
}