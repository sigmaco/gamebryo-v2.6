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
inline NiTerrainVertex::NiTerrainVertex() :
    m_kWorldLocation(NiPoint3::ZERO),
    m_fHeight(0.0f),
    m_pkFirstContainingLeaf(0),
    m_pkParentLeaf(0),
    m_kIndex(NiIndex::ZERO)    
{
}
//---------------------------------------------------------------------------
inline NiTerrainVertex::~NiTerrainVertex() 
{
}
//---------------------------------------------------------------------------
inline void NiTerrainVertex::SetHeight(float fHeight)
{
    m_fHeight = fHeight;
}
//---------------------------------------------------------------------------
inline const float NiTerrainVertex::GetHeight() const
{
    return m_fHeight;
}
//---------------------------------------------------------------------------
inline void NiTerrainVertex::SetIndex(const NiIndex& kIndex)
{
    m_kIndex = kIndex;
}
//---------------------------------------------------------------------------
inline const NiIndex& NiTerrainVertex::GetIndex() const
{
    return m_kIndex;    
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainVertex::GetFirstContainingLeaf() const
{
    return m_pkFirstContainingLeaf;
}
//---------------------------------------------------------------------------
inline void NiTerrainVertex::SetFirstContainingLeaf(
    NiTerrainDataLeaf* pkFirstContainingLeaf)
{
    m_pkFirstContainingLeaf = pkFirstContainingLeaf;
}
//---------------------------------------------------------------------------
inline NiTerrainDataLeaf* NiTerrainVertex::GetParentLeaf() const
{
    return m_pkParentLeaf;
}
//---------------------------------------------------------------------------
inline void NiTerrainVertex::SetParentLeaf(NiTerrainDataLeaf* pkParentLeaf)
{
    m_pkParentLeaf = pkParentLeaf;
}
