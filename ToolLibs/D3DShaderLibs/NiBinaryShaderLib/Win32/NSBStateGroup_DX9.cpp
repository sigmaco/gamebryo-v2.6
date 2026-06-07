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
#include "NSBRenderStates.h"
#include "NSBStageAndSamplerStates.h"

#include <NiD3DUtility.h>
#include <NiD3DRenderStateGroup.h>
#include <NiD3DTextureStageGroup.h>
#include <NiDX9Renderer.h>

//---------------------------------------------------------------------------
bool NSBStateGroup::SetupRenderStateGroup(
    NiD3DRenderStateGroup& kRSGroup)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    if (m_kStateList.GetSize() == 0)
        return false;

    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and stuff it in the RenderStateGroup
            D3DRENDERSTATETYPE eRS;
            
            if (NSBRenderStates::GetD3DRenderStateType(
                (NSBRenderStates::NiD3DRenderState)pkEntry->GetState(), eRS))
            {
                unsigned int uiValue; 
                if (NSBRenderStates::ConvertNSBRenderStateValue(
                    (NSBRenderStates::NiD3DRenderState)pkEntry->GetState(),
                    pkEntry->GetValue(), uiValue))
                {
                    if (eRS != 0x7fffffff)
                    {
                        kRSGroup.SetRenderState(eRS, uiValue, 
                            pkEntry->IsSaved());
                    }
                }
            }
        }
        else
        {
            // Not support yet!
            NIASSERT(!"NSBStateGroup::GetRenderStateGroup> Attribute State"
                " support not yet implemented!");
        }

        pkEntry = GetNextState();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::SetupTextureStageGroup(
    NiD3DTextureStageGroup& kTSGroup)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    if (m_kStateList.GetSize() == 0)
        return false;

    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and stuff it in the TextureStateGroup
            unsigned int uiTSST;

            if (NSBStageAndSamplerStates::GetD3DTextureStageState(
                (NSBStageAndSamplerStates::NiD3DTextureStageState)
                    pkEntry->GetState(), 
                uiTSST))
            {
                unsigned int uiValue; 
                if (NSBStageAndSamplerStates::ConvertNSBTextureStageStateValue(
                    (NSBStageAndSamplerStates::NiD3DTextureStageState)
                        pkEntry->GetState(), pkEntry->GetValue(), uiValue))
                {
                    if (uiTSST != 0x7fffffff)
                    {
                        kTSGroup.SetStageState(uiTSST, uiValue, 
                            pkEntry->IsSaved());
                    }
                }
            }
        }
        else
        {
            // Not support yet!
            NIASSERT(!"NSBStateGroup::GetRenderStateGroup> Attribute State"
                " support not yet implemented!");
        }

        pkEntry = GetNextState();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NSBStateGroup::SetupTextureSamplerGroup(
    NiD3DTextureStageGroup& kTSGroup)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    if (m_kStateList.GetSize() == 0)
        return false;

    NSBSGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and stuff it in the TextureStateGroup
            unsigned int uiValue = 0x7fffffff;
            if (pkEntry->UsesMapValue() ||
                NSBStageAndSamplerStates::ConvertNSBTextureSamplerStateValue(
                (NSBStageAndSamplerStates::NiD3DTextureSamplerState)
                    pkEntry->GetState(), pkEntry->GetValue(), uiValue))
            {
                if (pkEntry->GetState() != 0x7fffffff)
                {
                    kTSGroup.SetSamplerState(pkEntry->GetState(), 
                        uiValue, pkEntry->IsSaved(), pkEntry->UsesMapValue());
                }
            }
        }
        else
        {
            // Not support yet!
            NIASSERT(!"NSBStateGroup::GetRenderStateGroup> Attribute State"
                " support not yet implemented!");
        }

        pkEntry = GetNextState();
    }

    return true;
}
//---------------------------------------------------------------------------
