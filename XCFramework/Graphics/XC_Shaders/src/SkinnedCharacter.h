/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#ifndef __SKINNEDCHARACTER__
#define __SKINNEDCHARACTER__

#include "Engine/Utils/GraphicUtils.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Shaders/src/IShader.h"

class SkinnedCharacterShader : public IShader
{
public:
    static const unsigned int NbOfDescriptors = 7;

    SkinnedCharacterShader(ID3DDevice& device)
        : IShader(device)
    {}

    virtual ~SkinnedCharacterShader() {}

    virtual void                loadShader();
    virtual void                createBufferConstants();
    virtual void                destroy();
    virtual void                applyShader(ID3DDeviceContext& context);

#if defined(XCGRAPHICS_DX12)
    void                        setCBPerObject(ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
    void                        setCBBoneBuffers(ID3DDeviceContext & context, D3D12_GPU_DESCRIPTOR_HANDLE & gpuHandle);
#endif

    void                        setCBLightsPerFrame(ID3DDeviceContext& context, cbLightsPerFrame& lightsPerFrame);
    void                        setTexture2D(ID3DDeviceContext& context, D3DShaderResourceView* tex);
    void                        reset(ID3DDeviceContext& context);
                                
protected:                      
#if defined(XCGRAPHICS_DX12) || defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_GL)
    D3DConstantBuffer*          m_pCBLightsPerFrame;
    D3DConstantBuffer*          m_pCBSkinnedCharacter;
    D3DConstantBuffer*          m_pCBBoneBuffer;
#elif defined(XCGRAPHICS_GNM)
    sce::Gnm::Buffer            m_pCBSkinnedCharacter;     //Mapped with m_pCBSkinnedCharacter
    cbLightsPerFrame*           m_LightsPerFrame;
    sce::Gnm::Buffer            m_pCBPerObject;     //Mapped with cbLightsPerFrame
    cbLightsPerFrame*           m_pPerObject;
#endif

    D3DShaderResourceView*      m_pTexture2DDiffuseMap;  //Mapped with gDiffuseMap
};

#endif