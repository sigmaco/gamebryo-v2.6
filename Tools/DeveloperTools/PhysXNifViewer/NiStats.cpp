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

#include "NiStats.h"
#include "NiConsole.h"

#include <NiRenderTargetGroup.h>

//---------------------------------------------------------------------------
NiStats::NiStats(NiRenderer* pkRenderer, NiTexture* pkTexture,
    unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
    unsigned int uiColumns)
{
    const NiRenderTargetGroup* pkRTGroup =
        pkRenderer->GetDefaultRenderTargetGroup();

    unsigned int uiScreenHeight = pkRTGroup->GetHeight(0);

    m_uiBoxHeight = 1;
    m_uiBoxWidth = 10;

    // uiPadY should be approximately the number of lines that get chopped off
    // the bottom of the display.
    const unsigned int uiPadY = 32;

    const unsigned int uiPosX = 50;
    unsigned int uiPosY = uiScreenHeight - uiCharacterHeight * m_uiBoxHeight -
        uiPadY;

    m_pkConsole = NiNew NiConsole(pkTexture, uiCharacterWidth,
        uiCharacterHeight, uiColumns, uiPosX, uiPosY, '!', 'z');

    m_pkConsole->GetRenderClick()->SetActive(false);
}

//---------------------------------------------------------------------------
NiStats::~NiStats()
{
    NiDelete m_pkConsole;
}

//---------------------------------------------------------------------------
void NiStats::Update()
{
    m_pkConsole->Clear();

    NiColorA kColor(0.0f, 0.0f, 0.0f, 0.25f);

    m_pkConsole->AddRectangle(-1, 0, m_uiBoxWidth, m_uiBoxHeight, kColor);

    char acString[128];
    unsigned int uiRow = 0;
    unsigned int uiCol = 0;

    static float sfLastTime = 0.0f;
    float fTime = NiGetCurrentTimeInSec();
    int iFPS = (int) (1.0f / (fTime - sfLastTime) + 0.5f);
    sfLastTime = fTime;

    NiSprintf(acString, 128, "FPS: %d", iFPS);

    m_pkConsole->AddString(uiCol, uiRow++, acString);
}
