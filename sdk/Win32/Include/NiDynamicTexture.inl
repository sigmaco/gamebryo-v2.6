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
inline NiDynamicTexture::NiDynamicTexture(unsigned int uiWidth,
    unsigned int uiHeight)
{
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
    m_bTiled = false;
}
//---------------------------------------------------------------------------
inline unsigned int NiDynamicTexture::GetWidth() const
{
    if (m_pkRendererData)
        return m_pkRendererData->GetWidth();
    else
        return m_uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiDynamicTexture::GetHeight() const
{
    if (m_pkRendererData)
        return m_pkRendererData->GetHeight();
    else
        return m_uiHeight;
}
//---------------------------------------------------------------------------
inline bool NiDynamicTexture::GetTiled() const
{
    return m_bTiled;
}
//---------------------------------------------------------------------------
