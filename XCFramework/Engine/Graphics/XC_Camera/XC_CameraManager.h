/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Engine/Graphics/XC_Camera/XC_CameraTypes.h"
#include "Engine/Graphics/XC_Camera/ICamera.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Gameplay/GameActors/IActor.h"
#include "Engine/Graphics/XC_Graphics.h"


class XC_CameraManager : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XC_CameraManager)

    XC_CameraManager();
    ~XC_CameraManager();

    void                            InitializeCameras(XC_Graphics& graphicsSystem, DirectInput& directinput, int clientWidth, int clientHeight);
    void                            Update(float dt);
    void                            Draw();
    void                            Destroy();

    void                            SetCameraType(ECameraType type) { m_currentCameraType = type; }
    ICamera*                        GetCurrentCamera() { return m_Cameras[m_currentCameraType].get(); }
    XCMatrix4                       GetViewMatrix() { return m_Cameras[m_currentCameraType]->GetViewMatrix(); }
    XCMatrix4                       GetProjMatrix() { return m_Cameras[m_currentCameraType]->GetProjectionMatrix(); }
    XCVecIntrinsic4                 GetCameraPosition() { return m_Cameras[m_currentCameraType]->GetPosition(); }
    XCVecIntrinsic4                 GetCameraTarget() { return m_Cameras[m_currentCameraType]->GetTarget(); }

    void                            onResize(int clientWidth, int clientHeight);

    void                            AttachCameraToActor(IActor* actor);
    void                            AttachCameraToActor(IActor* actor, XCVecIntrinsic4 distanceFromTarget);

private:
    std::map<ECameraType, std::unique_ptr<ICamera>>     m_Cameras;
    ECameraType                                         m_currentCameraType;

    XC_Graphics*                                        m_graphicsSystem;
    DirectInput*                                        m_directInput;
};