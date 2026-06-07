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
inline const NiFixedString& NiPhysXClothMesh::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothMesh::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NxClothMesh* NiPhysXClothMesh::GetMesh() const
{
    return m_pkMesh;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothMesh::SetMesh(NxClothMesh* pkMesh)
{
    m_pkMesh = pkMesh;
}
//---------------------------------------------------------------------------
inline const bool NiPhysXClothMesh::GetStreamInline() const
{
    return m_bStreamInline;
}
//---------------------------------------------------------------------------
inline void NiPhysXClothMesh::SetStreamInline(const bool bVal)
{
    m_bStreamInline = bVal;
}
//---------------------------------------------------------------------------
inline NiPhysXMeshDesc* NiPhysXClothMesh::GetMeshDesc() const
{
    return m_pkMeshDesc;
}
//---------------------------------------------------------------------------
inline void  NiPhysXClothMesh::SetMeshDesc(NiPhysXMeshDesc* pkDesc)
{
    m_pkMeshDesc = pkDesc;
}
//---------------------------------------------------------------------------

