/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Door.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Input/Directinput.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

Door::Door(void)
{
    m_actorType = GAMEACTOR_DOOR;
    m_directInput = (DirectInput*) &SystemLocator::GetInstance()->RequestSystem("InputSystem");

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    m_useShaderType = SHADERTYPE_LIGHTTEXTURE;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;
}

Door::~Door(void)
{
}

void Door::PreLoad(const void* fbBuffer)
{
    const FBDoor* doorBuff = (FBDoor*)fbBuffer;
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = (XCMesh*)resMgr.GetResource(doorBuff->XCMeshResourceName()->c_str());

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectBuffer)));
#endif

    PhysicsActor::PreLoad(fbBuffer);
}

void Door::PreLoad(Texture2D* tex, XCVec3 _initialPosition, XCMesh* pMesh)
{
    m_pMesh = pMesh;
    m_initialPosition = _initialPosition;
}

void Door::Load()
{
    BuildGeometryBuffer();
    PhysicsActor::Load();
}

void Door::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(XMLoadFloat3(&m_initialPosition), XMLoadFloat3(&vec), XMLoadFloat3(&vec), 1000, (float)0.2);
}


void Door::BuildGeometryBuffer()
{
    m_pMesh->createBuffers(VertexFormat_PositionNormalTexture);
}


void Door::Update(float dt)
{
    if (m_directInput->KeyDown(INPUT_KEY_I))
    {
        //m_initialPosition.z++;
    }

    if (m_directInput->KeyDown(INPUT_KEY_K))
    {
        //m_initialPosition.z--;
    }

    Integrator(dt);
    ClearForce();

#if defined(WIN32)
    m_MTranslation = XMMatrixTranslation(m_initialPosition.x, m_initialPosition.y, m_initialPosition.z);
#elif defined(XC_ORBIS)
    m_MTranslation = XMMatrixTranslation(m_initialPosition.getX(), m_initialPosition.getY(), m_initialPosition.getZ());
#endif

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->Update(dt);
    PhysicsActor::Update(dt);
}

void Door::Draw(RenderContext& context)
{
    context.SetRasterizerState(RASTERIZERTYPE_FILL_WIREFRAME);

    context.ApplyShader(m_useShaderType);

    // Set constants
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    cbPerObjectBuffer perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())),
        ToXCMatrix4Unaligned(InverseTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixIdentity()),
        m_material
    };

    XCShaderHandle* lightTexShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(SHADERTYPE_LIGHTTEXTURE);
#if defined(XCGRAPHICS_DX12)
    memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(cbPerObjectBuffer));
    lightTexShader->setConstantBuffer("cbPerObjectBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);
#else
    lightTexShader->setCBPerObject(context.GetDeviceContext(), perObject);
#endif

    m_pMesh->Draw(context, SHADERTYPE_LIGHTTEXTURE);
    PhysicsActor::Draw(context);
}

void Door::Destroy()
{
    PhysicsActor::Destroy();
}