//--------------------------------------------------------------------------------------
// File: DDSTextureLoader.h
//
// Functions for loading a DDS texture without using D3DX
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include <d3d11_x.h>

HRESULT CreateDDSTextureFromFile( __in ID3D11Device* pDev, __in_z const WCHAR* szFileName, __out_opt ID3D11ShaderResourceView** ppSRV, bool sRGB = false );