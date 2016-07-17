/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if UNUSED 

#include "Engine/ObjectBase/IObjectDimensions.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Engine/Graphics/XC_Shaders/XC_RasterizerTypes.h"
#include "Engine/Graphics/XC_Materials/MaterialTypes.h"
#include "Engine/Graphics/XC_Textures/Texture2D.h"
#include "Engine/Graphics/VertexBuffer.h"
#include "Engine/Graphics/IndexBuffer.h"

class RenderContext;

class CubeMesh : public IObjectDimensions
{
public:
    CubeMesh(void);
    virtual ~CubeMesh(void);

    virtual void                    Init();
    virtual void                    Load();
    virtual void                    Update(f32 dt);
    virtual void                    Draw(RenderContext& context);
    virtual void                    Destroy();
    virtual void                    BuildBuffers();

    void                            PreLoad(XCVec4 initialPosition, XCVec4 initialRotation, XCVec4 initialScaling, BasicMaterial material, Texture2D* texture, RasterType rasterType);
    void                            CreateMeshBuffer();

protected:

    bool                            m_isBufferBuild;

    u32                    m_Stride;
    u32                    m_Offset;

    ShaderType                      m_useShaderType;
    BasicMaterial                   m_material;
    Texture2D*                      m_texture;

    VertexBuffer<VertexPosNormTex>  m_vertexBuffer;
    IndexBuffer<u32>       m_indexBuffer;

    RasterType                      m_rasterType;
};

#endif