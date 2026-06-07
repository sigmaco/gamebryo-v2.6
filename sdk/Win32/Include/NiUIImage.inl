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
inline void NiUIImage::EnterPressed(unsigned char)
{
}
//---------------------------------------------------------------------------
inline void NiUIImage::LeftMouseButton(unsigned char)
{
}
//---------------------------------------------------------------------------
inline void NiUIImage::Draw(NiRenderer*)
{
}
//---------------------------------------------------------------------------
inline void NiUIImage::SetTextureCoords(NiRect<float> kTextureCoords)
{
    m_kTextureCoords.m_left = kTextureCoords.m_left;
    m_kTextureCoords.m_right = kTextureCoords.m_right;
    m_kTextureCoords.m_top = kTextureCoords.m_top;
    m_kTextureCoords.m_bottom = kTextureCoords.m_bottom;
}
//---------------------------------------------------------------------------
inline unsigned int NiUIImage::NumQuadsRequired()
{
    return 1;
}
//---------------------------------------------------------------------------
