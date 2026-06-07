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

// Precompiled Header
#include "NiD3DRendererPCH.h"

#include "NiD3DShaderProgramCreator.h"

D3DDevicePtr NiD3DShaderProgramCreator::ms_pkD3DDevice = NULL;
NiD3DRenderer* NiD3DShaderProgramCreator::ms_pkD3DRenderer = NULL;

//---------------------------------------------------------------------------
void NiD3DShaderProgramCreator::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    ms_pkD3DRenderer = pkD3DRenderer;
    if (ms_pkD3DRenderer)
        SetD3DDevice(ms_pkD3DRenderer->GetD3DDevice());
    else 
        SetD3DDevice(0);
}
//---------------------------------------------------------------------------
void NiD3DShaderProgramCreator::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (ms_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(ms_pkD3DDevice);
    ms_pkD3DDevice = pkD3DDevice;
    if (ms_pkD3DDevice)
        D3D_POINTER_REFERENCE(ms_pkD3DDevice);
}
//---------------------------------------------------------------------------
