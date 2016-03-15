/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "GameActors/GameActorsFactory.h"

class WorldSceneLoader : public AsyncTask
{
public:
    WorldSceneLoader(World& world, std::string packageData);
    virtual ~WorldSceneLoader();

    virtual void            Init();
    virtual void            Run();

    IActor*                 CreateLoadActor(std::string actorName, const void* fbBuffer);
private:
    std::string             m_packageName;
    World&                  m_worldRef;
    GameActorsFactory*      m_actorFactory;
};