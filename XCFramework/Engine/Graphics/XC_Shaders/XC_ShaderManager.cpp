/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"

#if !defined(LOAD_SHADERS_FROM_DATA)
#include "Engine/Graphics/XC_Shaders/src/DefaultShader.h"
#include "Engine/Graphics/XC_Shaders/src/SolidColor.h"
#include "Engine/Graphics/XC_Shaders/src/LightTexture.h"
#include "Engine/Graphics/XC_Shaders/src/TerrainMultiTex.h"
#include "Engine/Graphics/XC_Shaders/src/CubeMap.h"
#include "Engine/Graphics/XC_Shaders/src/SkinnedCharacter.h"
#endif

#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"


XC_ShaderManager::XC_ShaderManager(ID3DDevice& device)
    : m_device(device)
{
}

XC_ShaderManager::~XC_ShaderManager(void)
{
}

void XC_ShaderManager::Init()
{
    m_Shaders.resize(SHADERTYPE_MAX);

#if defined(XCGRAPHICS_DX12) || defined(XCGRAPHICS_DX11)
    ZeroMemory(&m_rasterizerDesc, sizeof(D3D_RASTERIZER_DESC));
    ZeroMemory(&m_samplerDesc, sizeof(m_samplerDesc));
#endif

#if defined(XCGRAPHICS_DX12)
    //Initialize shader shared descriptor heap
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_sharedDescriptorHeap = (SharedDescriptorHeap*)&container.CreateNewSystem("SharedDescriptorHeap");

    m_sharedDescriptorHeap->Init(m_device,
#if defined(LOAD_SHADERS_FROM_DATA)
        100
#else
        SolidColorShader::NbOfDescriptors 
        + LightTextureShader::NbOfDescriptors 
        + TerrainMultiTex::NbOfDescriptors 
        + CubeMapShader::NbOfDescriptors 
        + SkinnedCharacterShader::NbOfDescriptors 
        + NbOfTextureResources 
#endif
        + 100);

#elif defined(XCGRAPHICS_DX11)
    LoadRasterizers();
#endif

    LoadShaders();
}

void XC_ShaderManager::LoadShaders()
{
    IShader* binShader;

    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadFile(SHADER_SCHEMA_FILEPATH);
    fbSystem.ParseAndLoadFile(SHADER_DATA_FILEPATH);

#if defined(LOAD_SHADERS_FROM_DATA)
    auto FBShadersRoot = GetFBRootShader(fbSystem.GetBufferFromLoadedData());

    for (auto it = FBShadersRoot->FBShaders()->begin(); it != FBShadersRoot->FBShaders()->end(); ++it)
    {
        binShader = new XCShaderHandle(m_device);
        binShader->load((void*) *it);
        m_Shaders[it->ShaderType()] = binShader;
    }
#else
    for (unsigned int shaderIndex = 0; shaderIndex < SHADERTYPE_MAX; shaderIndex++)
    {
        switch (shaderIndex)
        {
            case SHADERTYPE_DEFAULT:
                {
                    binShader = new DefaultShader(m_device);

                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_DEFAULT] = binShader;

                    break;
                }

            case SHADERTYPE_COLORTECH:
                {
                    binShader = new SolidColorShader(m_device);

                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_COLORTECH] = binShader;

                    break;
                }
            case SHADERTYPE_LIGHTTEXTURE:
                {
                    binShader = new LightTextureShader(m_device);

                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_LIGHTTEXTURE] = binShader;
                    break;
                }

            case SHADERTYPE_REFELECTED_LIGHTTEXTURE:
                {
                    binShader = new XCShaderHandle(m_device);
                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_REFELECTED_LIGHTTEXTURE] = binShader;
                    break;
                }

            case SHADERTYPE_TERRIANMULTITEXTURE:
                {
                    binShader = new TerrainMultiTex(m_device);

                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_TERRIANMULTITEXTURE] = binShader;
                    break;
                }

            case SHADERTYPE_SIMPLECUBEMAP:
                {
                    binShader = new CubeMapShader(m_device);

                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_SIMPLECUBEMAP] = binShader;
                    break;
                }

            case SHADERTYPE_SKINNEDCHARACTER:
                {
                    binShader = new SkinnedCharacterShader(m_device);

                    binShader->loadShader();
                    binShader->createBufferConstants();
                    m_Shaders[SHADERTYPE_SKINNEDCHARACTER] = binShader;
                    break;
                }

            case SHADERTYPE_MAX:
                break;

            default: 
                break;
        }
    }
#endif
}

#if defined(XCGRAPHICS_DX11)
void XC_ShaderManager::LoadRasterizers()
{

    m_rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    m_rasterizerDesc.CullMode = D3D11_CULL_BACK;

    m_device.CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerStates[RASTERIZERTYPE_FILL_WIREFRAME]);

    m_rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    m_rasterizerDesc.CullMode = D3D11_CULL_BACK;

    m_device.CreateRasterizerState(&m_rasterizerDesc, &m_rasterizerStates[RASTERIZERTYPE_FILL_SOLID]);

    m_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    m_samplerDesc.MinLOD = 0;
    m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ValidateResult(m_device.CreateSamplerState(&m_samplerDesc, &m_SamplerLinear));
}

void XC_ShaderManager::SetRasterizerState(ID3DDeviceContext& context, ERasterizer_Type type)
{
    context.RSSetState(m_rasterizerStates[type]);
}
#endif

void XC_ShaderManager::Destroy()
{
    for (unsigned int shaderIndex = 0; shaderIndex < SHADERTYPE_MAX; shaderIndex++)
    {
        if (m_Shaders[(SHADERTYPE)shaderIndex] != nullptr)
        {
            m_Shaders[(SHADERTYPE)shaderIndex]->destroy();
        }
    }
    m_Shaders.clear();

#if defined(XCGRAPHICS_DX12)
    m_sharedDescriptorHeap->Destroy();
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("SharedDescriptorHeap");
#endif
}

void XC_ShaderManager::ApplyShader(ID3DDeviceContext& context, SHADERTYPE _shaderType)
{
    m_Shaders[_shaderType]->applyShader(context);
#if defined(XCGRAPHICS_DX11)
    context.PSSetSamplers(0, 1, &m_SamplerLinear);
#endif
}

IShader* XC_ShaderManager::GetShader(SHADERTYPE _type)
{
    return m_Shaders[_type];
}

void XC_ShaderManager::DrawNonIndexed(ID3DDeviceContext& context, unsigned int vertexCount)
{
#if defined(XCGRAPHICS_DX12)
    context.DrawInstanced(vertexCount, 1, 0, 0);
#elif defined(XCGRAPHICS_DX11)
    context.Draw(vertexCount, 0);
#endif
}

void XC_ShaderManager::DrawIndexed(ID3DDeviceContext& context, unsigned int _indexCount, void* indexGpuAddr)
{
#if defined(XCGRAPHICS_DX12)
    context.DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
#elif defined(XCGRAPHICS_DX11)
    context.DrawIndexed(_indexCount, 0, 0);
#endif
}

void XC_ShaderManager::ClearShaderAndRenderStates(ID3DDeviceContext& context)
{
    for (unsigned int shaderIndex = 0; shaderIndex < SHADERTYPE_MAX; shaderIndex++)
    {
        if (m_Shaders[(SHADERTYPE)shaderIndex] != nullptr)
        {
            m_Shaders[(SHADERTYPE)shaderIndex]->reset(context);
        }
    }
}