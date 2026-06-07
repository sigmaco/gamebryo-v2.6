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
inline const NiFixedString& NiPhysXConvexMesh::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiPhysXConvexMesh::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NxConvexMesh* NiPhysXConvexMesh::GetMesh() const
{
    return m_pkMesh;
}
//---------------------------------------------------------------------------
inline void NiPhysXConvexMesh::SetMesh(NxConvexMesh* pkMesh)
{
    m_pkMesh = pkMesh;
}
//---------------------------------------------------------------------------
inline const bool NiPhysXConvexMesh::GetStreamInline() const
{
    return m_bStreamInline;
}
//---------------------------------------------------------------------------
inline void NiPhysXConvexMesh::SetStreamInline(const bool bVal)
{
    m_bStreamInline = bVal;
}
//---------------------------------------------------------------------------
inline NiPhysXMeshDesc* NiPhysXConvexMesh::GetMeshDesc() const
{
    return m_pkMeshDesc;
}
//---------------------------------------------------------------------------
inline void  NiPhysXConvexMesh::SetMeshDesc(NiPhysXMeshDesc* pkDesc)
{
    m_pkMeshDesc = pkDesc;
}
//---------------------------------------------------------------------------

