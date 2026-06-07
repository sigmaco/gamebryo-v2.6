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
//  NiMAXMeshConverter inline functions
//---------------------------------------------------------------------------
inline void NiMAXMeshConverter::SetExportVertWeights(bool)
{
    ms_bExportVertWeights = false;//bSaveVertWeights;
}
//---------------------------------------------------------------------------
inline bool NiMAXMeshConverter::GetExportVertWeights()
{
    return ms_bExportVertWeights;
}
//---------------------------------------------------------------------------
inline bool NiMAXMeshConverter::FaceUsesOtherMaterial(Mesh* pMesh, int iFace,
    const int iMaxAttr, const int iAttrId)
{
    MtlID mID = (MtlID) (pMesh->getFaceMtlIndex(iFace) % iMaxAttr);
        
    return (iAttrId != -1 && mID != iAttrId);
}
//---------------------------------------------------------------------------

