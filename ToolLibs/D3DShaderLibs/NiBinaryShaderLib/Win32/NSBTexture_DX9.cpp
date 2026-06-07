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
#include "NiBinaryShaderLibPCH.h"

//---------------------------------------------------------------------------
#include "NSBTexture.h"
#include "NSBUtility.h"

#include <NiD3DUtility.h>
#include <NiD3DTextureStage.h>
#include <NiDX9Renderer.h>

//---------------------------------------------------------------------------
bool NSBTexture::SetupTextureStage(NiD3DTextureStage& kStage)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    NIASSERT(kStage.GetStage() == m_uiStage);

    // Override all texture flags except for 
    // NiD3DTextureStage::TSTF_MAP_USE_INDEX
    unsigned int uiCurrentTextureFlags = kStage.GetTextureFlags();
    kStage.SetTextureFlags(
        (uiCurrentTextureFlags & NiD3DTextureStage::TSTF_MAP_USE_INDEX) |
        (m_uiTextureFlags & ~NiD3DTextureStage::TSTF_MAP_USE_INDEX));

    // Override object texture flags
    kStage.SetObjTextureFlags(m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
