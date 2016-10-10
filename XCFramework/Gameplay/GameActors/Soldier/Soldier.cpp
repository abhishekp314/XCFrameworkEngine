/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Soldier.h"

#include "Gameplay/XCCamera/XCCameraManager.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Engine/Resource/ResourceManager.h"

#include "Gameplay/GameActors/GameActorsFactory.h"

const f32 Soldier::MAX_PITCH_ANGLE = XC_PIDIV4;

Soldier::Soldier(void)
{
}

Soldier::~Soldier(void)
{
}

void Soldier::PreLoad(const void* fbBuffer)
{
    const FBSoldier* soldierBuff = (FBSoldier*)fbBuffer;

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(soldierBuff->XCMeshResourceName()->c_str());

    //Get initial position
    m_currentPosition.SetValues(soldierBuff->Position()->x(), soldierBuff->Position()->y(), soldierBuff->Position()->z(), 0.0f);

    m_material.Ambient = XCVec4Unaligned(0.1f, 0.1f, 0.1f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;

    m_secondaryLookAxis  = XCVec4();
    m_secondaryUpAxis    = XCVec4();
    m_secondaryRightAxis = XCVec4();

    m_totalPitchAngle = 0.0f;

    //Gun mesh
    GameActorsFactory& actorFactory = (GameActorsFactory&)SystemLocator::GetInstance()->RequestSystem("GameActorsFactory");

    m_gun = ((Gun*) actorFactory.CreateActor("Gun"));
    m_gun->PreLoad(this, m_currentPosition, "Gun");

    PhysicsActor::PreLoad(fbBuffer);
}

void Soldier::Load()
{
    m_MTranslation = MatrixTranslate(m_currentPosition);

    m_MRotation = m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_secondaryLookAxis = m_look;
    m_secondaryUpAxis = m_up;
    m_secondaryRightAxis = m_right;

    //Call the sub actors methods
    m_gun->Load();

    XCVec4 pos = XCVec4(1.0f, 3.0f, 2.0f, 0.0f);
    m_gun->GetSubActor()->InitOffsets(pos, m_secondaryLookAxis, m_secondaryRightAxis, m_secondaryUpAxis);

    PhysicsActor::Load();
}

void Soldier::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    InitXPhysics(m_currentPosition, XCVec4(), XCVec4(), 10, (f32)0.8);
}

void Soldier::UpdateState()
{
    if (m_pMesh && m_pMesh->GetResource<XCMesh>()->IsLoaded())
    {
        m_useShaderType = m_pMesh->GetResource<XCMesh>()->IsSkinnedMesh() ? ShaderType_SkinnedCharacter : ShaderType_LightTexture;
    }

    PhysicsActor::UpdateState();
}

void Soldier::Update(f32 dt)
{
    //Update the rotation based on initial and transformed.
    m_MRotation = m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    PhysicsActor::Update(dt);

    m_gun->Update(dt);
}

void Soldier::AccelerateCar(f32 distance)
{
    m_Position += (distance * m_look);
}

void Soldier::Walk(f32 scalarForce)
{
    AddForce(m_look * scalarForce);
}

void Soldier::Strafe(f32 scalarForce)
{
    AddForce(m_right * scalarForce);
}

void Soldier::Jump(f32 scalarForce)
{
    AddForce(m_up * scalarForce);
}

void Soldier::Yaw(f32 angle, f32 scalarForce)
{
    //Total RotationOffset will be applied from Pitch. So remember to always YAW AND PITCH together. ApplyOffsetRotation will be applied from Pitch()
    XCVec4 quaternionAxis = QuaternionRotationAxis(XCVec4(0, 1, 0, 0), angle);
    XCMatrix4 rotation = MatrixRotationQuaternion(quaternionAxis);

    m_look  = VectorTransformNormal(m_look, rotation);
    m_right = VectorTransformNormal(m_right, rotation);
    m_up    = VectorTransformNormal(m_up, rotation);

    m_secondaryLookAxis  = VectorTransformNormal(m_secondaryLookAxis, rotation);
    m_secondaryRightAxis = VectorTransformNormal(m_secondaryRightAxis, rotation);
    m_secondaryUpAxis    = VectorTransformNormal(m_secondaryUpAxis, rotation);

    //Rotate the soldier with it's initial rotations.
    m_transformedRotation *= rotation;

    //Set the offset gun rotation too
    m_gun->SetOffsetLook(m_secondaryLookAxis);
    m_gun->SetOffsetUp(m_secondaryUpAxis);
    m_gun->SetOffsetRight(m_secondaryRightAxis);

    m_gun->SetOffsetRotation(rotation);
}

void Soldier::Pitch(f32 angle, f32 scalarForce)
{
    //Total RotationOffset will be applied from Pitch. So remember to always YAW AND PITCH together. ApplyOffsetRotation will be applied from Pitch()
    XCVec4 quaternionAxis = QuaternionRotationAxis(m_secondaryRightAxis, angle);
    XCMatrix4 rotation    = MatrixRotationAxis(m_secondaryRightAxis, angle);

    rotation = MatrixRotationQuaternion(quaternionAxis);

    //Compare if the look is within the bound looks
    XCVec4 tempLook = VectorTransformNormal(m_secondaryLookAxis, rotation);
    
    //The upper limit and lower limit should be with respect to the parent object look axis, which is constant in the sense without pitch (m_look), 45 degree pitch angle
    XCVec4 lookUB = VectorTransformNormal(m_look, MatrixRotationAxis(m_secondaryRightAxis, MAX_PITCH_ANGLE / 2));
    XCVec4 lookLB = VectorTransformNormal(m_look, MatrixRotationAxis(m_secondaryRightAxis, -MAX_PITCH_ANGLE / 2));
    
    if (lookLB.Get<Y>() >= tempLook.Get<Y>() && lookUB.Get<Y>() <= tempLook.Get<Y>())
    {
        m_secondaryUpAxis   = VectorTransformNormal(m_secondaryUpAxis, rotation);
        m_secondaryLookAxis = VectorTransformNormal(m_secondaryLookAxis, rotation);
    
        m_gun->SetOffsetLook(m_secondaryLookAxis);
        m_gun->SetOffsetUp(m_secondaryUpAxis);
        m_gun->SetOffsetRight(m_secondaryRightAxis);
    
        //This rotation needs to be set for offsets
        m_gun->SetOffsetRotation(m_gun->GetOffsetRotation() * rotation);
    }

    m_gun->ApplyOffsetRotation();
}

void Soldier::ApplyRotation(XCMatrix4& rotation)
{
    //This method makes sure that the rotation is applied to parent and it's sub actors.
    m_MRotation *= rotation;
    m_gun->SetOffsetRotation(m_gun->GetOffsetRotation() * rotation);
}

void Soldier::Draw(RenderContext& context)
{
    // Set constants
    ICamera& cam = context.GetGlobalShaderData().m_camera;

    PerObjectBuffer perObject = {};

    if (m_useShaderType == ShaderType_LightTexture)
    {
        perObject = {
            MatrixTranspose(m_World).GetUnaligned(),
            MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
            MatrixInverseTranspose(m_World).GetUnaligned(),
            XCMatrix4::XCMatrixIdentity.GetUnaligned(),
            m_material
        };
    }
    else if(m_useShaderType == ShaderType_SkinnedCharacter)
    {
        XCMatrix4 transform(m_pMesh->GetResource<XCMesh>()->GetRootTransform());
        transform *= m_World;

        perObject = {
            MatrixTranspose(transform).GetUnaligned(),
            MatrixTranspose(transform * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
            MatrixInverseTranspose(transform).GetUnaligned(),
            XCMatrix4::XCMatrixIdentity.GetUnaligned(),
            m_material
        };
    }

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);

    PhysicsActor::Draw(context);

    m_gun->Draw(context);
}

void Soldier::Destroy()
{
    PhysicsActor::Destroy();

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);

    m_gun->Destroy();
}