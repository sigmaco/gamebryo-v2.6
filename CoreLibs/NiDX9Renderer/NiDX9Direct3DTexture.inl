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
inline NiDX9Direct3DTexture::NiDX9Direct3DTexture()
{
    /* */
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Direct3DTexture::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Direct3DTexture::GetHeight() const
{
    return m_uiHeight;
}
//---------------------------------------------------------------------------
inline void NiDX9Direct3DTexture::SetWidth(unsigned int uiWidth)
{
    m_uiWidth = uiWidth;
}
//---------------------------------------------------------------------------
inline void NiDX9Direct3DTexture::SetHeight(unsigned int uiHeight)
{
    m_uiHeight = uiHeight;
}
//---------------------------------------------------------------------------
