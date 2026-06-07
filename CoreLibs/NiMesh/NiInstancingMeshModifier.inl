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
inline void NiInstancingMeshModifier::MarkInstanceNodeTranformsDirty()
{
    m_bDirtyNodeTransforms = true;
}
//---------------------------------------------------------------------------
inline bool NiInstancingMeshModifier::AreInstanceNodeTranformsDirty()
{
    return m_bDirtyNodeTransforms;
}
//---------------------------------------------------------------------------
inline bool NiInstancingMeshModifier::AreInstanceNodesUsed()
{
    return m_bInstanceNodes;
}
//---------------------------------------------------------------------------
inline bool NiInstancingMeshModifier::GetPerInstaceCulling()
{
    return m_bPerInstanceCulling;
}
//---------------------------------------------------------------------------
inline void NiInstancingMeshModifier::SetPerInstanceCulling(
    bool bPerInstanceCulling)
{
    m_bPerInstanceCulling = bPerInstanceCulling;
}
//---------------------------------------------------------------------------
inline bool NiInstancingMeshModifier::GetStaticBounds()
{
    return m_bStaticBounds;
}
//---------------------------------------------------------------------------
inline void NiInstancingMeshModifier::SetStaticBounds(bool bStaticBounds)
{
    m_bStaticBounds = bStaticBounds;
}
//---------------------------------------------------------------------------
inline NiMesh* NiInstancingMeshModifier::GetAffectedMesh()
{
    return m_pkAffectedMesh;
}
//---------------------------------------------------------------------------

