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
inline NiUInt32 NiTerrainSector::GetVertsInBound(const NiBound &kBound, 
    NiDeformablePointSet* pkPointSet,
    NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail) const
{
    NiUInt32 uiNumVerts = 0;

    // create the transformation matrix to go from world space to 
    // terrain model space:
    NiTransform kTerrainTransform = GetSectorData()->GetWorldTransform();
    const NiPoint3& kCenter =  kTerrainTransform.m_Rotate.Inverse() * (
        (kBound.GetCenter() - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale); 
    
    NiBound kLocalBound;
    kLocalBound.SetCenter(kCenter);
    kLocalBound.SetRadius(kBound.GetRadius() / kTerrainTransform.m_fScale);

    // Get the verts within the local bound
    NiTerrainUtils::GetVertsInBound(kLocalBound, 
        pkPointSet, uiNumVerts, pkOriginalPointPool,
        uiDetailLevel, bIncludeLowerDetail,
        m_pkQuadData);

    pkPointSet->ValidateData();

    return uiNumVerts;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetVertsInBound2D(const NiBound &kBound,
    NiDeformablePointSet* pkPointSet,
    NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail) const
{
    NiUInt32 uiNumVerts = 0;

    // create the transformation matrix to go from world space to 
    // terrain model space:
    NiTransform kTerrainTransform = GetSectorData()->GetWorldTransform();
    const NiPoint3& kCenter =  kTerrainTransform.m_Rotate.Inverse() * (
        (kBound.GetCenter() - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale); 
    
    NiBound kLocalBound;
    kLocalBound.SetCenter(kCenter);
    kLocalBound.SetRadius(kBound.GetRadius() / kTerrainTransform.m_fScale);

    // Get the verts within the local bound
    NiTerrainUtils::GetVertsInBound2D(kLocalBound, 
        pkPointSet, uiNumVerts, pkOriginalPointPool,
        uiDetailLevel, bIncludeLowerDetail, 
        m_pkQuadData);

    pkPointSet->ValidateData();

    return uiNumVerts;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetVertsInBound( 
    const NiBoundingVolume &, 
    NiDeformablePointSet*,
    NiTQuickReleaseObjectPool<NiTerrainVertex>*,
    NiUInt32,
    bool) const
{
    return 0;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetPixelsInBound(
    const NiBound &kBound,
    NiDeformablePointSet* pkPointSet, 
    NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool,
    const NiSurface* pkSurface,
    bool bCreateMasksIfNotExist,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NiUInt32 uiNumPixels = 0;

    // create the transformation matrix to go from world space to 
    // terrain model space:
    NiTransform kTerrainTransform = GetSectorData()->GetWorldTransform();
    const NiPoint3& kCenter =  kTerrainTransform.m_Rotate.Inverse() * (
        (kBound.GetCenter() - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale); 
    
    NiBound kLocalBound;
    kLocalBound.SetCenter(kCenter);
    kLocalBound.SetRadius(kBound.GetRadius() / kTerrainTransform.m_fScale);

    if (bCreateMasksIfNotExist)
    {
        // We need to ensure all blocks in the bound up to the specified level
        // have a mask for this surface
        if (NiTerrainUtils::TestBound(kLocalBound, m_pkQuadData))
        {
            EnsureMaskExists(kLocalBound, pkSurface, uiDetailLevel, 
                m_pkQuadData);
        }
    }

    NiTerrainUtils::GetPixelsInBound(
        kLocalBound, pkPointSet, uiNumPixels, 
        pkOriginalPointPool, pkSurface, uiDetailLevel, bIncludeLowerDetail, 
        m_pkQuadData);

    pkPointSet->ValidateData();

    return uiNumPixels;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSector::GetPixelsInBound2D(
    const NiBound &kBound,
    NiDeformablePointSet* pkPointSet, 
    NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool,
    const NiSurface* pkSurface,
    bool bCreateMasksIfNotExist,
    NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail)
{
    NiUInt32 uiNumPixels = 0;

    // create the transformation matrix to go from world space to 
    // terrain model space:
    NiTransform kTerrainTransform = GetSectorData()->GetWorldTransform();
    const NiPoint3& kCenter =  kTerrainTransform.m_Rotate.Inverse() * (
        (kBound.GetCenter() - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale); 
    
    NiBound kLocalBound;
    kLocalBound.SetCenter(kCenter);
    kLocalBound.SetRadius(kBound.GetRadius() / kTerrainTransform.m_fScale);

    if (bCreateMasksIfNotExist)
    {
        // We need to ensure all blocks in the bound up to the specified level
        // have a mask for this surface
        if (NiTerrainUtils::TestBound(kLocalBound, m_pkQuadData))
        {
            EnsureMaskExists(kLocalBound, pkSurface, uiDetailLevel, 
                m_pkQuadData);
        }
    }

    NiTerrainUtils::GetPixelsInBound2D(
        kLocalBound, pkPointSet, uiNumPixels, 
        pkOriginalPointPool, pkSurface, uiDetailLevel, bIncludeLowerDetail, 
        m_pkQuadData);

    pkPointSet->ValidateData();

    return uiNumPixels;
}
//---------------------------------------------------------------------------
inline bool NiTerrainSector::GetSurfaceOpacity(
    const NiSurface* pkSurface, const NiPoint3& kWorldLocation, 
    NiUInt8& ucValue, NiUInt32 uiDetailLevel,
    bool bSearchLowerDetail) const
{
    return NiTerrainUtils::GetSurfaceOpacity(
        pkSurface, kWorldLocation, ucValue, uiDetailLevel, bSearchLowerDetail, 
        m_pkQuadData);
}
