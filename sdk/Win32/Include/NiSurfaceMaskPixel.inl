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
inline NiSurfaceMaskPixel::NiSurfaceMaskPixel() : 
    m_kPosition(), 
    m_pkMask(0),
    m_pkFirstContainingLeaf(0)
{
}
//---------------------------------------------------------------------------
inline NiSurfaceMaskPixel::NiSurfaceMaskPixel(
    NiTerrainDataLeaf* pkFirstContainingLeaf,
    const NiSurfaceMask* pkMask, 
    NiUInt32 uiX, NiUInt32 uiY) :
    m_kPosition(uiX, uiY),
    m_pkMask(pkMask),
    m_pkFirstContainingLeaf(pkFirstContainingLeaf)
{
}
//---------------------------------------------------------------------------
inline NiSurfaceMaskPixel::NiSurfaceMaskPixel(
    NiTerrainDataLeaf* pkFirstContainingLeaf, 
    const NiSurfaceMask* pkMask, NiIndex kPosition) :
    m_kPosition(kPosition),
    m_pkMask(pkMask),
    m_pkFirstContainingLeaf(pkFirstContainingLeaf)
{
}    
//---------------------------------------------------------------------------
inline NiSurfaceMaskPixel::~NiSurfaceMaskPixel()
{
    m_pkMask = 0;
}
//---------------------------------------------------------------------------
inline NiUInt8 NiSurfaceMaskPixel::GetValue() const
{
    NIASSERT(m_pkMask);
    return m_pkMask->GetAt(m_kPosition.x, m_kPosition.y);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSurfaceMaskPixel::GetX() const
{
    return m_kPosition.x;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSurfaceMaskPixel::GetY() const
{
    return m_kPosition.y;
}
//---------------------------------------------------------------------------
inline void NiSurfaceMaskPixel::SetX(NiUInt32 uiX)
{
    m_kPosition.x = uiX;
}
//---------------------------------------------------------------------------
inline void NiSurfaceMaskPixel::SetY(NiUInt32 uiY)
{
    m_kPosition.y = uiY;
}
//---------------------------------------------------------------------------
inline const NiSurfaceMask* NiSurfaceMaskPixel::GetMask() const
{
    return m_pkMask;
}
//---------------------------------------------------------------------------
inline void NiSurfaceMaskPixel::SetMask(const NiSurfaceMask* pkMask)
{
    m_pkMask = pkMask;
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiSurfaceMaskPixel::GetFirstContainingLeaf()
{
    return m_pkFirstContainingLeaf;
}
//---------------------------------------------------------------------------
inline const NiTerrainDataLeaf* NiSurfaceMaskPixel::GetFirstContainingLeaf()
    const
{
    return m_pkFirstContainingLeaf;
}
//---------------------------------------------------------------------------
inline void NiSurfaceMaskPixel::SetFirstContainingLeaf(
    NiTerrainDataLeaf* pkFirstContainingLeaf)
{
    m_pkFirstContainingLeaf = pkFirstContainingLeaf;
}
