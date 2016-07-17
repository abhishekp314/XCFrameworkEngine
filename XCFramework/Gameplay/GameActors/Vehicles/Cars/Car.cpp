/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Car.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Gameplay/XC_Camera/XC_CameraManager.h"
#include "Engine/Resource/ResourceManager.h"

Car::Car(void)
{
}

Car::~Car(void)
{
}

void Car::PreLoad(XCVec3& initialPosition, std::string pMesh)
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(pMesh.c_str());
    
    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);
    
    //Get initial position
    m_currentPosition = initialPosition;
    
    m_useShaderType = ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;
}

void Car::Load()
{
    m_MTranslation = MatrixTranslate(m_currentPosition);
    
    m_MRotation= MatrixRotationX(-XC_PIDIV2);
    
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    PhysicsActor::Load();
}

void Car::SetInitialPhysicsProperties()
{
    InitXPhysics(m_currentPosition, XCFloat4::XCFloat4ZeroVector, XCFloat4::XCFloat4ZeroVector, 10, (f32)0.8);
    PhysicsActor::SetInitialPhysicsProperties();
}

void Car::Update(f32 dt)
{
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    m_pMesh->GetResource<XCMesh>()->Update(dt);
    
    PhysicsActor::Update(dt);
}

void Car::Accelerate(f32 distance)
{
    m_Position += (distance * m_look);
}

void Car::Steer(f32 angle, f32 scalarForce)
{
    XCVec4 quaternionAxis = QuaternionRotationAxis(XCVec4(0, 1, 0, 0), angle);
    XCMatrix4 rotation = MatrixRotationQuaternion(quaternionAxis);

    m_look = VectorTransformNormalMatrix(m_look, rotation);
    m_right = VectorTransformNormalMatrix(m_right, rotation);

    //Rotate the car with it's initial rotations.
    m_MRotation *= rotation;

    AddForce(m_look * scalarForce);
}

void Car::Draw(RenderContext& context)
{
    // Set constants
    ICamera& cam = context.GetShaderManagerSystem().GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
       MatrixTranspose(m_World).GetUnaligned(),
       MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
       MatrixInverseTranspose(m_World).GetUnaligned(),
       XCMatrix::XCMatrixIdentity.GetUnaligned(),
        m_material
    };

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);

    PhysicsActor::Draw(context);
}

void Car::Destroy()
{
    PhysicsActor::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);
}