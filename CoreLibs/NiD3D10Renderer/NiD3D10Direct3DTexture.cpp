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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Direct3DTexture.h"

NiImplementRTTI(NiD3D10Direct3DTexture, NiTexture);

//---------------------------------------------------------------------------
NiD3D10Direct3DTexture* NiD3D10Direct3DTexture::Create(NiRenderer* pkRenderer)
{
    if (!pkRenderer)
        return NULL;

    NiD3D10Direct3DTexture* pkThis = NiNew NiD3D10Direct3DTexture;

    return pkThis;
}
//---------------------------------------------------------------------------
NiD3D10Direct3DTexture::NiD3D10Direct3DTexture()
{
    /* */
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Direct3DTexture::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Direct3DTexture::GetHeight() const
{
    return m_uiHeight;
}
