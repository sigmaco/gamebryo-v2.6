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
inline NiAlphaSortProcessor::NiAlphaSortProcessor(bool bObserveNoSortHint,
    bool bSortByClosestPoint) :
    NiBackToFrontSortProcessor(bSortByClosestPoint),
    m_bObserveNoSortHint(bObserveNoSortHint)
{
}
//---------------------------------------------------------------------------
inline void NiAlphaSortProcessor::SetObserveNoSortHint(bool bObserveNoSortHint)
{
    m_bObserveNoSortHint = bObserveNoSortHint;
}
//---------------------------------------------------------------------------
inline bool NiAlphaSortProcessor::GetObserveNoSortHint() const
{
    return m_bObserveNoSortHint;
}
//---------------------------------------------------------------------------
inline bool NiAlphaSortProcessor::IsTransparent(const NiRenderObject& kObject)
{
    // Get property state.
    const NiPropertyState* pkPropState = kObject.GetPropertyState();
    NIASSERT(pkPropState);

    // Get alpha property.
    const NiAlphaProperty* pkAlphaProp = pkPropState->GetAlpha();
    NIASSERT(pkAlphaProp);

    // If alpha blending is turned on and the no sort hint is not specified
    // (if observed), return true.
    if (pkAlphaProp->GetAlphaBlending() &&
        !(m_bObserveNoSortHint && pkAlphaProp->GetNoSorter()) &&
        kObject.GetSortObject())
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
