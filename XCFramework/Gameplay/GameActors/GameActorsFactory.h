/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/ObjectFactory.h"
#include "Engine/System/ISystem.h"
#include "Gameplay/GameActors/GameActorDefines.h"

struct GameActorsInfo
{
    char    m_actorName[50];
};

class GameActorsFactory : public ObjectFactory, public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(GameActorsFactory)

    GameActorsFactory();
    virtual ~GameActorsFactory();

    void                            InitFactory();
    void                            RegisterActors();

    IActor*                         CreateActor(std::string userFriendlyName);
    
    template<class T>
    T*                              CreateActor(std::string actorName)
    {
        std::unique_lock<std::mutex> m(m_gameActorsFactoryLock);

        IActor* actor = (IActor*)CreateObject(actorName);
        actor->Init(++m_actorsCount);

        Logger("[GAME ACTORS FACTORY] Actor : %s  ID : %d", actorName.c_str(), m_actorsCount);

        m.unlock();

        return (T*) actor;
    }

    void                            LoadActor(FILE* packageFile, IActor* const resource);
    
    void                            DestroyFactory();

    int                             GetNoOfActorTypes() { return GAMEACTOR_MAX; }
    void                            GetAllActorTypes(GameActorsInfo* actorTypes);

private:
    std::map<EGameActorType, std::string>       m_registeredActors;
    std::mutex                                  m_gameActorsFactoryLock;

    int                                         m_actorsCount;
};