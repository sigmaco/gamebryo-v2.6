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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

//---------------------------------------------------------------------------
#include "NSBStateGroup.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"

#include <NiD3DUtility.h>
#include <NiD3DTextureStage.h>
#include <NiDX9Renderer.h>
//---------------------------------------------------------------------------
bool NSBTextureStage::SetupTextureStage(NiD3DTextureStage& kStage)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    kStage.SetStage(m_uiStage);
    kStage.SetTextureFlags(m_uiTextureFlags);
    kStage.SetTextureTransformFlags(m_uiTextureTransformFlags,
        m_pcGlobalEntry);
    kStage.SetObjTextureFlags(m_usObjTextureFlags);

    NiD3DTextureStageGroup* pkStageGroup = kStage.GetTextureStageGroup();
    if (m_pkTextureStageGroup)
    {
        if (m_pkTextureStageGroup->GetStateCount())
        {
            NIASSERT(pkStageGroup);
            if (!m_pkTextureStageGroup->SetupTextureStageGroup(
                *pkStageGroup))
            {
                NiDelete pkStageGroup;
                return false;
            }
        }
    }

    if (m_pkSamplerStageGroup)
    {
        if (m_pkSamplerStageGroup->GetStateCount())
        {
            NIASSERT(pkStageGroup);
            if (!m_pkSamplerStageGroup->SetupTextureSamplerGroup(
                *pkStageGroup))
            {
                NiDelete pkStageGroup;
                return false;
            }
        }
    }

    kStage.SetTextureTransformation(m_kTextureTransformation);

    return true;
}
//---------------------------------------------------------------------------
