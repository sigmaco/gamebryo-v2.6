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
inline NiBlendInterpolator::InterpArrayItem::InterpArrayItem()
{
    ClearValues();
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::InterpArrayItem::ClearValues()
{
    m_spInterpolator = NULL;
    m_fWeight = 0.0f;
    m_fNormalizedWeight = 0.0f;
    m_cPriority = 0;
    m_fEaseSpinner = 0.0f;
    m_fUpdateTime = INVALID_TIME;
}
//---------------------------------------------------------------------------
inline unsigned char NiBlendInterpolator::GetArraySize() const
{
    return m_ucArraySize;
}
//---------------------------------------------------------------------------
inline unsigned char NiBlendInterpolator::GetArrayGrowBy() 
{
    return ms_ucArrayGrowBy;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetArrayGrowBy(unsigned char ucArrayGrowBy)
{
    ms_ucArrayGrowBy = ucArrayGrowBy;
}
//---------------------------------------------------------------------------
inline NiInterpolator* NiBlendInterpolator::GetInterpolator(
    unsigned char ucIndex) const
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Get the chached interpolator for a single interpolator.
        return m_pkSingleInterpolator;
    }

    return m_pkInterpArray[ucIndex].m_spInterpolator;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetInterpolator(
    NiInterpolator* pkInterpolator, unsigned char ucIndex)
{
    NIASSERT(pkInterpolator && ucIndex < m_ucArraySize);
    NIASSERT(InterpolatorIsCorrectType(pkInterpolator));
    m_pkInterpArray[ucIndex].m_spInterpolator = pkInterpolator;
    SetComputeNormalizedWeights(true);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Set the cached interpolator for a single interpolator.
        m_pkSingleInterpolator = pkInterpolator;
    }
}
//---------------------------------------------------------------------------
inline float NiBlendInterpolator::GetWeight(unsigned char ucIndex) const
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Do not get the weight for a single interpolator.
        return 1.0f;
    }

    return m_pkInterpArray[ucIndex].m_fWeight;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetWeight(float fWeight,
    unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_ucArraySize);
    NIASSERT(fWeight >= 0.0f);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Do not set the weight for a single interpolator.
        return;
    }

    if (m_pkInterpArray[ucIndex].m_fWeight == fWeight)
    {
        return;
    }

    m_pkInterpArray[ucIndex].m_fWeight = fWeight;
    ClearWeightSums();
    SetComputeNormalizedWeights(true);
}
//---------------------------------------------------------------------------
inline char NiBlendInterpolator::GetPriority(unsigned char ucIndex) const
{
    NIASSERT(ucIndex < m_ucArraySize);
    return m_pkInterpArray[ucIndex].m_cPriority;
}
//---------------------------------------------------------------------------
inline float NiBlendInterpolator::GetEaseSpinner(unsigned char ucIndex) const
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Do not get the ease spinner for a single interpolator.
        return 1.0f;
    }

    return m_pkInterpArray[ucIndex].m_fEaseSpinner;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetEaseSpinner(float fEaseSpinner,
    unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_ucArraySize);
    NIASSERT(fEaseSpinner >= 0.0f && fEaseSpinner <= 1.0f);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Do not set the ease spinner for a single interpolator.
        return;
    }

    if (m_pkInterpArray[ucIndex].m_fEaseSpinner == fEaseSpinner)
    {
        return;
    }

    m_pkInterpArray[ucIndex].m_fEaseSpinner = fEaseSpinner;
    ClearWeightSums();
    SetComputeNormalizedWeights(true);
}
//---------------------------------------------------------------------------
inline float NiBlendInterpolator::GetTime(unsigned char ucIndex) const
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Return the cached time for a single interpolator.
        return m_fSingleTime;
    }

    return m_pkInterpArray[ucIndex].m_fUpdateTime;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetTime(float fTime, unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Set the cached time for a single interpolator.
        m_fSingleTime = fTime;
        return;
    }

    m_pkInterpArray[ucIndex].m_fUpdateTime = fTime;
}
//---------------------------------------------------------------------------
inline float NiBlendInterpolator::GetWeightThreshold() const
{
    return m_fWeightThreshold;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetWeightThreshold(float fWeightThreshold)
{
    m_fWeightThreshold = fWeightThreshold;
    if (m_fWeightThreshold < 0.0f)
    {
        m_fWeightThreshold = 0.0f;
    }
}
//---------------------------------------------------------------------------
inline float NiBlendInterpolator::GetNormalizedWeight(unsigned char ucIndex)
    const
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx)
    {
        // Do not get the normalized weight for a single interpolator.
        return 1.0f;
    }

    return m_pkInterpArray[ucIndex].m_fNormalizedWeight;
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::ClearWeightSums()
{
    m_fHighSumOfWeights = -NI_INFINITY;
    m_fNextHighSumOfWeights = -NI_INFINITY;
    m_fHighEaseSpinner = -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline bool NiBlendInterpolator::GetSingleUpdateTime(float& fTime)
{
    NIASSERT(m_ucSingleIdx != INVALID_INDEX && 
        m_pkSingleInterpolator != NULL);
    
    if (GetManagerControlled())
    {
        fTime = m_fSingleTime;
    }

    if (fTime == INVALID_TIME)
    {
        // The time for this interpolator has not been set. Do
        // not update the interpolator.
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiBlendInterpolator::GetUpdateTimeForItem(float& fTime, 
    InterpArrayItem& kItem)
{
    NiInterpolator* pkInterpolator = kItem.m_spInterpolator;
    if (pkInterpolator && kItem.m_fNormalizedWeight != 0.0f)
    {
        if (GetManagerControlled())
        {
            fTime = kItem.m_fUpdateTime;
        }

        if (fTime == INVALID_TIME)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline bool NiBlendInterpolator::GetOnlyUseHighestWeight() const
{
    return GetBit(ONLY_USE_HIGHEST_WEIGHT_MASK);
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetOnlyUseHighestWeight(
    bool bOnlyUseHighestWeight)
{
    SetBit(bOnlyUseHighestWeight, ONLY_USE_HIGHEST_WEIGHT_MASK);
}
//---------------------------------------------------------------------------
inline bool NiBlendInterpolator::GetManagerControlled() const
{
    return GetBit(MANAGER_CONTROLLED_MASK);
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetManagerControlled(
    bool bManagerControlled)
{
    SetBit(bManagerControlled, MANAGER_CONTROLLED_MASK);
}
//---------------------------------------------------------------------------
inline bool NiBlendInterpolator::GetComputeNormalizedWeights() const
{
    return GetBit(COMPUTE_NORMALIZED_WEIGHTS_MASK);
}
//---------------------------------------------------------------------------
inline void NiBlendInterpolator::SetComputeNormalizedWeights(
    bool bComputeNormalizedWeights)
{
    SetBit(bComputeNormalizedWeights, COMPUTE_NORMALIZED_WEIGHTS_MASK);
}
//---------------------------------------------------------------------------
