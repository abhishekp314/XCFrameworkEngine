/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "LiveDriveTexturePlane.h"
#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"
#include "Engine/Graphics/XC_Graphics.h"

LiveDriveTexturePlane::LiveDriveTexturePlane()
{
}

LiveDriveTexturePlane::LiveDriveTexturePlane(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3)
    : TexturedPlane(p1, p2, p3)
{
    m_texture = nullptr;
    m_liveDriveTexture = nullptr;
}

LiveDriveTexturePlane::~LiveDriveTexturePlane()
{
    delete(m_liveDriveTexture);
}

void LiveDriveTexturePlane::PreLoad(const void* fbBuffer)
{
    const FBLiveDriveTexturePlane* texPlaneBuff = (FBLiveDriveTexturePlane*)fbBuffer;
    m_currentPosition = toXMVECTOR(texPlaneBuff->Position()->x(), texPlaneBuff->Position()->y(), texPlaneBuff->Position()->z(), texPlaneBuff->Position()->w());
    m_initialRotation = toXMVECTOR(texPlaneBuff->Rotation()->x(), texPlaneBuff->Rotation()->y(), texPlaneBuff->Rotation()->z(), texPlaneBuff->Rotation()->w());
    m_initialScaling  = toXMVECTOR(texPlaneBuff->Scaling()->x(), texPlaneBuff->Scaling()->y(), texPlaneBuff->Scaling()->z(), texPlaneBuff->Scaling()->w());

    m_material.Ambient  = XCVec4(texPlaneBuff->Material()->Ambient()->x(), texPlaneBuff->Material()->Ambient()->y(), texPlaneBuff->Material()->Ambient()->z(), texPlaneBuff->Material()->Ambient()->w());
    m_material.Diffuse  = XCVec4(texPlaneBuff->Material()->Diffuse()->x(), texPlaneBuff->Material()->Diffuse()->y(), texPlaneBuff->Material()->Diffuse()->z(), texPlaneBuff->Material()->Diffuse()->w());
    m_material.Specular = XCVec4(texPlaneBuff->Material()->Specular()->x(), texPlaneBuff->Material()->Specular()->y(), texPlaneBuff->Material()->Specular()->z(), texPlaneBuff->Material()->Specular()->w());

    m_rasterType = (ERasterizer_Type) texPlaneBuff->RasterizerType();

    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
#if defined(XCGRAPHICS_DX11)
    m_liveDriveTexture = new Texture2D(graphicsSystem.GetRenderTexture(RENDERTARGET_LIVEDRIVE).GetShaderResourceView());
#endif
    m_texture = m_liveDriveTexture;

    SimpleActor::PreLoad(fbBuffer);
}

void LiveDriveTexturePlane::Update(float dt)
{
}

void LiveDriveTexturePlane::Draw(RenderContext& renderContext)
{
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    if (!graphicsSystem.isSecondaryDrawCall())
    {
        TexturedPlane::Draw(renderContext);
    }
}

void LiveDriveTexturePlane::Destroy()
{
    TexturedPlane::Destroy();
    m_liveDriveTexture->Destroy();
}