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

#include "NiDX9Direct3DTexture.h"

NiImplementRTTI(NiDX9Direct3DTexture, NiTexture);

//---------------------------------------------------------------------------
NiDX9Direct3DTexture* NiDX9Direct3DTexture::Create(NiRenderer* pkRenderer)
{
    if (!pkRenderer)
        return NULL;

    NiDX9Direct3DTexture* pkThis = NiNew NiDX9Direct3DTexture;
    
    return pkThis;
}
//---------------------------------------------------------------------------
