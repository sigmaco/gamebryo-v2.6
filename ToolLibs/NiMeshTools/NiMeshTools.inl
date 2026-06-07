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
inline NiMeshTools::WeightSort::WeightSort(const float fWeight, 
    const NiUInt32 uiIndex) 
{
    m_fWeight = fWeight;
    m_uiIndex = uiIndex;
}
//---------------------------------------------------------------------------
inline NiMeshTools::VertexWeightData::VertexWeightData() 
{
    m_uiRefCount = 0;
    m_fWeight = 0.0f;
    m_uiCacheIndex = 0xffffffff;
    m_uiOldIndex = 0xffffffff;
    m_uiNewIndex = 0xffffffff;
}
//---------------------------------------------------------------------------
inline NiMeshTools::PrimitiveWeightData::PrimitiveWeightData() 
{
    m_bIsAdded = false;
    for (NiUInt32 ui = 0; ui < 3; ui++)
    {
        m_apVerts[ui] = NULL;
    }
}
//---------------------------------------------------------------------------
inline float NiMeshTools::PrimitiveWeightData::GetTotalWeight()
{
    float fWeight = 0.0f;
    for (NiUInt32 ui = 0; ui < 3; ui++)
    {
        fWeight += m_apVerts[ui]->m_fWeight;
    }
    return fWeight;
}
//---------------------------------------------------------------------------
template <typename T>
inline T NiMeshTools::InterpolateTriangle(float fBarycentricU,
    float fBarycentricV, unsigned int uiV1, unsigned int uiV2,
    unsigned int uiV3, NiTStridedRandomAccessIterator<T>& kIter)
{
    return InterpolateTriangle(fBarycentricU, fBarycentricV,
        kIter[uiV1], kIter[uiV2], kIter[uiV3]);
}
//---------------------------------------------------------------------------
template <typename T>
inline T NiMeshTools::InterpolateTriangle(
    float fBarycentricU, float fBarycentricV, const T& kV1, const T& kV2,
    const T& kV3)
{
    return kV1 * (1.0f - (fBarycentricU + fBarycentricV)) +
        kV2 * fBarycentricU + kV3 * fBarycentricV;
}
//---------------------------------------------------------------------------
