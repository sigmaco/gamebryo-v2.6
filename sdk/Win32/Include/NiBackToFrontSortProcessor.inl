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
inline NiBackToFrontSortProcessor::NiBackToFrontSortProcessor(
    bool bSortByClosestPoint) : m_pfDepths(NULL), m_uiAllocatedDepths(0),
    m_bSortByClosestPoint(bSortByClosestPoint)
{
}
//---------------------------------------------------------------------------
inline void NiBackToFrontSortProcessor::SetSortByClosestPoint(
    bool bSortByClosestPoint)
{
    m_bSortByClosestPoint = bSortByClosestPoint;
}
//---------------------------------------------------------------------------
inline bool NiBackToFrontSortProcessor::GetSortByClosestPoint() const
{
    return m_bSortByClosestPoint;
}
//---------------------------------------------------------------------------
inline float NiBackToFrontSortProcessor::ComputeDepth(
    const NiRenderObject& kGeometry, const NiPoint3& kDir)
{
    const NiBound& kWorldBound = kGeometry.GetWorldBound();
    float fDepth = kWorldBound.GetCenter() * kDir;
    if (m_bSortByClosestPoint)
    {
        fDepth -= kWorldBound.GetRadius();
    }

    return fDepth;
}
//---------------------------------------------------------------------------
