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
inline int NiUnionBV::Type() const
{
    return UNION_BV;
}
//---------------------------------------------------------------------------
inline unsigned int NiUnionBV::GetSize() const
{
    return m_kBoundingVolume.GetSize();
}
//---------------------------------------------------------------------------
inline const NiBoundingVolume* NiUnionBV::GetBoundingVolume(
    unsigned int i) const
{
    if (i < m_kBoundingVolume.GetSize())
        return m_kBoundingVolume.GetAt(i);
    else
        return 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiUnionBV::WhichObjectIntersect() const
{
    return m_uiWhichObjectIntersect;
}
//---------------------------------------------------------------------------
