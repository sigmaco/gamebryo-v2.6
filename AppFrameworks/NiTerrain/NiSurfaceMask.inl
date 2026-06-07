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
inline NiSurfaceMask::NiSurfaceMask() :
    m_pucMask(0),
    m_uiMaskIndex(0),
    m_uiWidth(0)
{
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSurfaceMask::GetWidth() const
{
    return m_uiWidth;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiSurfaceMask::GetAt(NiUInt32 uiX, NiUInt32 uiY) const
{
    NIASSERT(uiX < m_uiWidth && uiY < m_uiWidth);
    return m_pucMask[uiX + uiY * m_uiWidth];
}
//---------------------------------------------------------------------------
inline void NiSurfaceMask::SetAt(NiUInt32 uiX, NiUInt32 uiY,
    NiUInt8 ucValue)
{
    NIASSERT(uiX < m_uiWidth && uiY < m_uiWidth);
    m_pucMask[uiX + uiY * m_uiWidth] = ucValue;
}
