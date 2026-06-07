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
#include "NiCursorPCH.h"

#include "NiSystemCursor.h"

#include <NiRenderTargetGroup.h>

//---------------------------------------------------------------------------
NiImplementRTTI(NiSystemCursor, NiCursor);
//---------------------------------------------------------------------------
NiSystemCursorPtr NiSystemCursor::ms_spSystemCursor = 0;
NiSystemCursor::SystemCursorInfo NiSystemCursor::ms_akInfo[SYSTEM_COUNT] = 
{
    { BULLSEYE,     15, 16,   0,   0 },
    { CLOSEHAND,    15, 15,  32,   0 },
    { CROSS,        15, 15,  64,   0 },
    { CROSSHAIR,    16, 15,  96,   0 },
    { DIAG_RESIZE,  15, 15, 128,   0 },
    { HAND,         11,  5, 160,   0 },
    { HORZ_RESIZE,  15, 15, 192,   0 },
    { HOURGLASS,    15, 15, 224,   0 },
    { IBEAM,        15, 15,   0,  32 },
    { MAGNIFY,      13, 14,  32,  32 },
    { MOVE,         15, 15,  64,  32 },
    { PEN,           5,  3,  96,  32 },
    { QUESTION,      8,  6, 128,  32 },
    { STANDARD,     10,  6, 160,  32 },
    { STOP,         15, 15, 192,  32 },
    { UNAVAILABLE,   4,  2, 224,  32 },
    { VERT_RESIZE,  15, 15,   0,  64 },
    { WAIT,          4,  1,  32,  64 }
};
//---------------------------------------------------------------------------
NiSystemCursor::~NiSystemCursor()
{
    //** Nothing yet... **//
}
//---------------------------------------------------------------------------
NiCursor* NiSystemCursor::Create(NiRenderer* pkRenderer, 
    NiRect<int>& kRect, SystemCursor eType,
    const char* pcImageFilename/* = NULL*/, 
    const NiRenderTargetGroup* pkTarget/* = NULL*/)
{
    if (ms_spSystemCursor)
    {
        NIASSERT(!"SystemCursor already created!");
        return ms_spSystemCursor;
    }

    NiCursor* pkCursor = 0;

    NiSystemCursor* pkSysCursor = NiNew NiSystemCursor(pkRenderer);
    if (pkSysCursor)
    {
        pkSysCursor->SetRect(kRect);
        pkSysCursor->SetRenderTargetGroup(pkTarget);

        // Use a default path if one isn't provided
        const char* pcFilename = pcImageFilename;
        if (!pcFilename)
        {
#if defined(_XENON)
            pcFilename = "D:/Data/SystemCursors.tga";
#else   //#if defined(_XENON)
            pcFilename = "../../Data/SystemCursors.tga";
#endif  //#if defined(_XENON)
        }
        
        // Set the image
        if (!pkSysCursor->SetImage(pcFilename))
        {
            NiDelete pkSysCursor;
            return 0;
        }

        pkSysCursor->m_uiDrawWidth = 32;
        pkSysCursor->m_uiDrawHeight = 32;
        pkSysCursor->SetType(eType);

        pkSysCursor->ResetScreenGeometry();

        ms_spSystemCursor = pkSysCursor;
        pkCursor = (NiCursor*)pkSysCursor;
    }

    return pkCursor;
}
//---------------------------------------------------------------------------
void NiSystemCursor::Shutdown()
{
    ms_spSystemCursor = 0;
}
//---------------------------------------------------------------------------
NiSystemCursor* NiSystemCursor::GetCursor(SystemCursor eType)
{
    if (ms_spSystemCursor)
    {
        ms_spSystemCursor->SetType(eType);
        return ms_spSystemCursor;
    }
    return 0;
}
//---------------------------------------------------------------------------
NiSystemCursor::SystemCursor NiSystemCursor::SetType(SystemCursor eType)
{
    if (m_eType == eType)
        return m_eType;

    SystemCursor eOldType = m_eType;

    m_eType = eType;

    m_uiCurrentOffsetTop = ms_akInfo[eType].m_uiTextureOffsetTop;
    m_uiCurrentOffsetLeft = ms_akInfo[eType].m_uiTextureOffsetLeft;
    SetHotSpot(ms_akInfo[eType].m_uiHotSpotX, ms_akInfo[eType].m_uiHotSpotY);

    // Force a 'redraw'
    ResetScreenGeometry();

    return eOldType;
}
//---------------------------------------------------------------------------
bool NiSystemCursor::ResetScreenGeometry()
{
    if (!m_spElements)
        return false;

    // Adjust for hot-spot changes.
    const NiRenderTargetGroup* pkRTGroup = m_pkRenderTarget;
    if (pkRTGroup == NULL)
        pkRTGroup = m_spRenderer->GetDefaultRenderTargetGroup();
    float fInvScrW = 1.0f / pkRTGroup->GetWidth(0);
    float fInvScrH = 1.0f / pkRTGroup->GetHeight(0);
    float fLeft = -fInvScrW * (float)m_uiHotSpotX;
    float fTop = -fInvScrH * (float)m_uiHotSpotY;
    float fWidth = 32.0f * fInvScrW;
    float fHeight = 32.0f * fInvScrH;

    m_spElements->SetRectangle(0, fLeft, fTop, fWidth, fHeight);
    m_spElements->UpdateBound();
    m_spElements->Update(0.0f);

    // Calculate the proper UV set.
    unsigned int uiTextureWidth = m_spImage->GetWidth();
    unsigned int uiTextureHeight = m_spImage->GetHeight();

    fLeft = (float)m_uiCurrentOffsetLeft / (float)uiTextureWidth;
    fTop = (float)m_uiCurrentOffsetTop / (float)uiTextureHeight;
    float fRight = fLeft + 32.0f / (float)uiTextureWidth;
    float fBottom = fTop + 32.0f / (float)uiTextureHeight;
    m_spElements->SetTextures(0, 0, fLeft, fTop, fRight, fBottom);

    return true;
}
//---------------------------------------------------------------------------
void NiSystemCursor::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    pStrings->Add(NiGetViewerString(NiSystemCursor::ms_RTTI.GetName()));
    NiCursor::GetViewerStrings(pStrings);
}
//---------------------------------------------------------------------------
