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

#ifndef NISTATS_H
#define NISTATS_H

#include <NiMemObject.h>
#include <NiViewRenderClick.h>
#include <NiConsole.h>

class NiRenderer;
class NiTexture;

class NiStats : public NiMemObject
{
public:
    NiStats(NiRenderer* pkRenderer, NiTexture* pkTexture,
        unsigned int uiCharacterWidth, unsigned int uiCharacterHeight,
        unsigned int uiColumns); 
    ~NiStats();

    void Update();
    bool GetActive();
    void SetActive(bool bActive);
    NiViewRenderClick* GetRenderClick();

protected:
    NiConsole* m_pkConsole;
    unsigned int m_uiBoxHeight;
    unsigned int m_uiBoxWidth;
};

//---------------------------------------------------------------------------
inline bool NiStats::GetActive()
{
    return m_pkConsole->GetRenderClick()->GetActive();   
}

//---------------------------------------------------------------------------
inline void NiStats::SetActive(bool bActive)
{
     m_pkConsole->GetRenderClick()->SetActive(bActive);
}

//---------------------------------------------------------------------------
inline NiViewRenderClick* NiStats::GetRenderClick()
{
    return m_pkConsole->GetRenderClick();   
}

#endif // NISTATS_H
