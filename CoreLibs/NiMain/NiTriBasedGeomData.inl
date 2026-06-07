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
//  NiTriBasedGeomData inline functions

//---------------------------------------------------------------------------
inline unsigned short NiTriBasedGeomData::GetTriangleCount() const
{
    return m_usTriangles;
}
//---------------------------------------------------------------------------
inline void NiTriBasedGeomData::SetTriangleCount(unsigned short usTriangles)
{
    // Unless a derived class overrides this, all triangles are considered
    // active
    m_usTriangles = usTriangles;
    m_usActiveTriangles = usTriangles;
}
//---------------------------------------------------------------------------
inline void NiTriBasedGeomData::SetActiveTriangleCount(
    unsigned short /*usActive*/)
{
    // stub for derived classes
}
//---------------------------------------------------------------------------
inline unsigned short NiTriBasedGeomData::GetActiveTriangleCount() const
{
    return m_usActiveTriangles;
}
//---------------------------------------------------------------------------
