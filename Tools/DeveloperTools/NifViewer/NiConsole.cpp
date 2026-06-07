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

#include "NiConsole.h"
#include <NiMesh2DRenderView.h>

//---------------------------------------------------------------------------
NiConsole::NiConsole(NiTexture* pkSourceTexture,
    unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
    unsigned int uiColumns, unsigned int uiPosX, unsigned int uiPosY,
    int iFirstCharacter, int iLastCharacter, 
    const NiFixedString& kRenderClickName)
    : m_kColor(NiColorA::WHITE)
{
    m_auiCharacterSize[0] = uiCharacterWidth;
    m_auiCharacterSize[1] = uiCharacterHeight;
    m_uiColumns = uiColumns;
    m_auiOrigin[0] = uiPosX;
    m_auiOrigin[1] = uiPosY;
    m_iFirstCharacter = iFirstCharacter;
    m_iLastCharacter = iLastCharacter;

    // create the screen texture
    m_spScreenTexture = NiMeshScreenElements::Create(pkSourceTexture,
        NiTexturingProperty::APPLY_MODULATE);

    // attach it to a render click
    m_spScreenTextureRenderClick = NiNew NiViewRenderClick();
    m_spScreenTextureRenderClick->SetName(kRenderClickName);
    NiMesh2DRenderView* pkRenderView = NiNew NiMesh2DRenderView();
    pkRenderView->AppendScreenElement(m_spScreenTexture);
    m_spScreenTextureRenderClick->AppendRenderView(pkRenderView);
}

//---------------------------------------------------------------------------
NiConsole::~NiConsole()
{
    m_spScreenTexture = 0;
    m_spScreenTextureRenderClick = 0;
}

//---------------------------------------------------------------------------
void NiConsole::AddString(unsigned int uiX, unsigned int uiY, 
    const char* pcString)
{
    short sPixLeft = (short)(m_auiOrigin[0] + uiX * m_auiCharacterSize[0]);
    short sPixTop = (short)(m_auiOrigin[1] + uiY * m_auiCharacterSize[1]);
    int iChar;

    while ((iChar = *pcString) != 0)
    {
        if (iChar >= m_iFirstCharacter && iChar <= m_iLastCharacter)
        {
            iChar -= m_iFirstCharacter;

            unsigned short usTexTop = (unsigned short)((iChar / m_uiColumns) *
                m_auiCharacterSize[1]);
            unsigned short usTexLeft = (unsigned short)((iChar % m_uiColumns) *
                m_auiCharacterSize[0]);

            m_spScreenTexture->AddNewScreenRect(sPixTop, sPixLeft,
                (unsigned short)m_auiCharacterSize[0], 
                (unsigned short)m_auiCharacterSize[1],
                usTexTop, usTexLeft, m_kColor);
        }

        sPixLeft = (short)(sPixLeft + m_auiCharacterSize[0]);
        pcString++;
    }
}

//---------------------------------------------------------------------------
void NiConsole::AddRectangle(int iX, int iY, unsigned int uiWidth,
    unsigned int uiHeight, const NiColorA& kColor)
{
    short sPixLeft = (short)(m_auiOrigin[0] + iX * m_auiCharacterSize[0]);
    short sPixTop = (short)(m_auiOrigin[1] + iY * m_auiCharacterSize[1]);
    unsigned int uiPixelWidth = uiWidth * m_auiCharacterSize[0];
    unsigned int uiPixelHeight = uiHeight * m_auiCharacterSize[1];

    NiTexturingProperty* pkTexProp = NiDynamicCast(NiTexturingProperty,
        m_spScreenTexture->GetProperty(NiProperty::TEXTURING));
    NiTexture* pkTexture = pkTexProp->GetBaseTexture();

    // Font texture has an opaque white pixel in its bottom right corner. 
    // This function creates a rectangle that uses only that pixel. (The
    // texture coordinates are clamped, not wrapped.)

    m_spScreenTexture->AddNewScreenRect(sPixTop, sPixLeft, 
        (unsigned short)uiPixelWidth,
        (unsigned short)uiPixelHeight, 
        (unsigned short)(pkTexture->GetHeight() - 1), 
        (unsigned short)(pkTexture->GetWidth() - 1),
        kColor);
}

//---------------------------------------------------------------------------
void NiConsole::AddLine(int iX, int iY, unsigned int uiWidth,
    const NiColorA& kColor)
{
    short sPixLeft = (short)(m_auiOrigin[0] + iX * m_auiCharacterSize[0]);
    short sPixTop = (short)(m_auiOrigin[1] + iY * m_auiCharacterSize[1]);
    unsigned int uiPixelWidth = uiWidth * m_auiCharacterSize[0];
    const unsigned int uiPixelHeight = 1;

    NiTexturingProperty* pkTexProp = NiDynamicCast(NiTexturingProperty,
        m_spScreenTexture->GetProperty(NiProperty::TEXTURING));
    NiTexture* pkTexture = pkTexProp->GetBaseTexture();

    // Font texture has an opaque white pixel in its bottom right corner. 
    // This function creates a 1-pixel high rectangle that uses only that
    // pixel. (The texture coordinates are clamped, not wrapped.)

    m_spScreenTexture->AddNewScreenRect(sPixTop, sPixLeft, 
        (unsigned short)uiPixelWidth,
        (unsigned short)uiPixelHeight, 
        (unsigned short)(pkTexture->GetHeight() - 1),
        (unsigned short)(pkTexture->GetWidth() - 1),
        kColor);
}

//---------------------------------------------------------------------------
void NiConsole::Clear()
{
    m_spScreenTexture->RemoveAll();
}

//---------------------------------------------------------------------------
void NiConsole::Render(NiRenderer* pkRenderer)
{
    m_spScreenTexture->RenderImmediate(pkRenderer);
}
