// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIDX9DEFINES_H
#define NIDX9DEFINES_H

// Contains some helper mappings/definitions for cross-D3D development.
//*** This header file is intended for DX9 only! ***//
// This will cause an error to flag when a build attempts to include this 
// file on any platform besides DX9.
#if defined(_XENON)
    #error This header is for DX9 builds only!
#endif  //#if defined(_XENON)

#include "NiDX9Headers.h"
#include <D3DX9Math.h>

#include <NiOS.h>
#include <NiRTLib.h>
#include <NiSmartPointer.h>

#include "NiDX9RendererLibType.h"
#define NID3D_ENTRY NIDX9RENDERER_ENTRY

#include "NiD3DError.h"

//*** Class Declarations
class NiDX9Renderer;
class NiDX9RenderState;
class NiDX9PaletteData;
class NiDX9TextureData;
class NiDX9SourceTextureData;
class NiDX9MaterialManager;

//*** Mappings for platform agnostic code
// D3D Mappings
typedef LPDIRECT3DRESOURCE9 D3DResourcePtr;
typedef LPDIRECT3DDEVICE9 D3DDevicePtr;
typedef LPDIRECT3DSURFACE9 D3DSurfacePtr;
typedef LPDIRECT3DBASETEXTURE9 D3DBaseTexturePtr;
typedef LPDIRECT3DTEXTURE9 D3DTexturePtr;
typedef LPDIRECT3DVERTEXBUFFER9 D3DVertexBufferPtr;
typedef LPDIRECT3DINDEXBUFFER9 D3DIndexBufferPtr;

// Gamebryo Mappings
typedef NiDX9Renderer NiD3DRenderer;
typedef NiDX9RenderState NiD3DRenderState;
typedef NiDX9PaletteData NiD3DPaletteData;
typedef NiDX9TextureData NiD3DTextureData;
typedef NiDX9SourceTextureData NiD3DSourceTextureData;
typedef NiDX9MaterialManager NiD3DMaterialManager;

// Shader 'handle'
typedef LPDIRECT3DVERTEXDECLARATION9 NiD3DVertexDeclaration;
typedef LPDIRECT3DVERTEXSHADER9 NiD3DVertexShaderHandle;
typedef LPDIRECT3DPIXELSHADER9 NiD3DPixelShaderHandle;

// This is to hide the implementation of D3DVECTOR4
#ifndef D3DVECTOR4_DEFINED
typedef struct _D3DVECTOR4 {
    float x;
    float y;
    float z;
    float w;
} D3DVECTOR4;
#define D3DVECTOR4_DEFINED
#endif  //#ifndef D3DVECTOR4_DEFINED

// Align D3DXMATRIX
typedef __declspec(align(16)) D3DXMATRIX    D3DXALIGNEDMATRIX;

#endif  //#ifndef NIDX9DEFINES_H
