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

#ifndef NICONSOLE_H
#define NICONSOLE_H

#include <NiColor.h>
#include <NiViewRenderClick.h>
#include <NiMeshScreenElements.h>

class NiRenderer;
class NiTexture;

class NiConsole : public NiMemObject
{
public:
    NiConsole(NiTexture* pkSourceTexture,
        unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
        unsigned int uiColumns, unsigned int uiPosX, unsigned int uiPosY,
        int iFirstCharacter, int iLastCharacter, 
        const NiFixedString& kRenderClickName);
    ~NiConsole();

    void SetColor(const NiColorA& kColor);
    void GetColor(NiColorA& kColor) const;
    void AddString(unsigned int uiX, unsigned int uiY, const char* pcString);
    void AddRectangle(int iX, int iY, unsigned int uiWidth,
        unsigned int uiHeight, const NiColorA& kColor);
    void AddLine(int iX, int iY, unsigned int uiWidth, const NiColorA& kColor);
    NiMeshScreenElementsPtr GetScreenTexture() const;
    NiViewRenderClick* GetRenderClick() const;

    void Clear();
    void Render(NiRenderer* pkRenderer);

protected:
    NiMeshScreenElementsPtr m_spScreenTexture;
    NiViewRenderClickPtr m_spScreenTextureRenderClick;
    unsigned int m_auiCharacterSize[2];
    unsigned int m_auiOrigin[2];
    unsigned int m_uiColumns;
    int m_iFirstCharacter;
    int m_iLastCharacter;
    NiColorA m_kColor;
};

//---------------------------------------------------------------------------
inline void NiConsole::SetColor(const NiColorA& kColor)
{
    m_kColor = kColor;
}

//---------------------------------------------------------------------------
inline void NiConsole::GetColor(NiColorA& kColor) const
{
    kColor = m_kColor;
}

//---------------------------------------------------------------------------
inline NiViewRenderClick* NiConsole::GetRenderClick() const
{
    return m_spScreenTextureRenderClick;
}

//---------------------------------------------------------------------------
inline NiMeshScreenElementsPtr NiConsole::GetScreenTexture() const
{
    return m_spScreenTexture;
}

#endif  // NICONSOLE_H
