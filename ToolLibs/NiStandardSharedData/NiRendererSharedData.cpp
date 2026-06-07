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

#include "NiRendererSharedData.h"

NiImplementRTTI(NiRendererSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiRendererSharedData::NiRendererSharedData()
{
    m_spRenderer = NULL; 
}
//---------------------------------------------------------------------------
NiRendererSharedData::~NiRendererSharedData()
{
    m_spRenderer = 0;
}
//---------------------------------------------------------------------------
NiRendererPtr NiRendererSharedData::GetRenderer()
{
    return m_spRenderer;
}
//---------------------------------------------------------------------------
void NiRendererSharedData::SetRenderer(NiRenderer* pkRenderer)
{
    NIASSERT(pkRenderer);
    m_spRenderer = pkRenderer;
}
//---------------------------------------------------------------------------
NiColor NiRendererSharedData::GetBackgroundColor()
{
    return m_kBackgroundColor;
}
//---------------------------------------------------------------------------

void NiRendererSharedData::SetBackgroundColor(NiColor kColor)
{
    m_kBackgroundColor = kColor;
}
//---------------------------------------------------------------------------

bool NiRendererSharedData::GetFullscreen()
{
    return m_bFullscreen;
}
//---------------------------------------------------------------------------

void NiRendererSharedData::SetFullscreen(bool bFullscreen)
{
    m_bFullscreen = bFullscreen;
}
//---------------------------------------------------------------------------


NiPoint2 NiRendererSharedData::GetWindowSize()
{
    return m_kWindowSize;
}
//---------------------------------------------------------------------------

void NiRendererSharedData::SetWindowSize(NiPoint2 kSize)
{
    m_kWindowSize = kSize;
}
//---------------------------------------------------------------------------
void NiRendererSharedData::PurgeRendererData(NiAVObject* pkObject)
{
    if (pkObject && m_spRenderer)
        m_spRenderer->PurgeAllRendererData(pkObject);
}