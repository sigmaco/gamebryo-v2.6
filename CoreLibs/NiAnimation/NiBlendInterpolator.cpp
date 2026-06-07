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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiBlendInterpolator.h"
#include <NiCloningProcess.h>
#include <NiBool.h>

NiImplementRTTI(NiBlendInterpolator, NiInterpolator);

const unsigned char NiBlendInterpolator::INVALID_INDEX = (unsigned char)
    UCHAR_MAX;
const float NiBlendInterpolator::INVALID_TIME = -FLT_MAX;
unsigned char NiBlendInterpolator::ms_ucArrayGrowBy = 2;
//---------------------------------------------------------------------------
NiBlendInterpolator::NiBlendInterpolator(bool bManagerControlled,
    float fWeightThreshold, unsigned char ucArraySize) :
    m_uFlags(0),
    m_ucArraySize(ucArraySize), 
    m_ucInterpCount(0),
    m_ucSingleIdx(INVALID_INDEX), 
    m_cHighPriority(SCHAR_MIN),
    m_cNextHighPriority(SCHAR_MIN), 
    m_pkInterpArray(NULL),
    m_pkSingleInterpolator(NULL),
    m_fWeightThreshold(fWeightThreshold),
    m_fSingleTime(INVALID_TIME), 
    m_fHighSumOfWeights(-NI_INFINITY),
    m_fNextHighSumOfWeights(-NI_INFINITY), 
    m_fHighEaseSpinner(-NI_INFINITY)
{
    SetManagerControlled(bManagerControlled);
    if (m_ucArraySize > 0)
    {
        // Allocate memory for array.
        m_pkInterpArray = NiNew InterpArrayItem[m_ucArraySize];
    }
}
//---------------------------------------------------------------------------
NiBlendInterpolator::NiBlendInterpolator() : 
    m_uFlags(0),
    m_ucArraySize(0),
    m_ucInterpCount(0), 
    m_ucSingleIdx(INVALID_INDEX),
    m_cHighPriority(SCHAR_MIN), 
    m_cNextHighPriority(SCHAR_MIN),
    m_pkInterpArray(NULL), 
    m_pkSingleInterpolator(NULL), 
    m_fWeightThreshold(0.0f), 
    m_fSingleTime(INVALID_TIME),
    m_fHighSumOfWeights(-NI_INFINITY), 
    m_fNextHighSumOfWeights(-NI_INFINITY),
    m_fHighEaseSpinner(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
NiBlendInterpolator::~NiBlendInterpolator()
{
    NiDelete[] m_pkInterpArray;
    m_pkInterpArray = NULL;
    m_pkSingleInterpolator = NULL;
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::SetPriority(char cPriority, unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_ucArraySize);

    // Only set priority if it differs from the current priority.
    if (m_pkInterpArray[ucIndex].m_cPriority == cPriority)
    {
        return;
    }

    m_pkInterpArray[ucIndex].m_cPriority = cPriority;

    if (cPriority > m_cHighPriority)
    {
        m_cNextHighPriority = m_cHighPriority;
        m_cHighPriority = cPriority;
    }
    else
    {
        // Determine highest priority.
        m_cHighPriority = m_cNextHighPriority = SCHAR_MIN;
        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            InterpArrayItem& kTempItem = m_pkInterpArray[uc];
            if (kTempItem.m_spInterpolator != NULL)
            {
                if (kTempItem.m_cPriority > m_cNextHighPriority)
                {
                    if (kTempItem.m_cPriority > m_cHighPriority)
                    {
                        m_cNextHighPriority = m_cHighPriority;
                        m_cHighPriority = kTempItem.m_cPriority;
                    }
                    else if (kTempItem.m_cPriority < m_cHighPriority)
                    {
                        m_cNextHighPriority = kTempItem.m_cPriority;
                    }
                }
            }
        }
    }

    ClearWeightSums();
    SetComputeNormalizedWeights(true);
}
//---------------------------------------------------------------------------
unsigned char NiBlendInterpolator::AddInterpInfo(
    NiInterpolator* pkInterpolator, float fWeight, char cPriority,
    float fEaseSpinner)
{
    NIASSERT(pkInterpolator && InterpolatorIsCorrectType(pkInterpolator));

    // Get new interp array index.
    unsigned char ucIndex;
    for (ucIndex = 0; ucIndex < m_ucArraySize; ucIndex++)
    {
        if (m_pkInterpArray[ucIndex].m_spInterpolator == NULL)
        {
            break;
        }
    }
    if (ucIndex == m_ucArraySize && !ReallocateArrays())
    {
        return INVALID_INDEX;
    }

    // Set new interp array data.
    InterpArrayItem& kNewItem = m_pkInterpArray[ucIndex];
    kNewItem.m_spInterpolator = pkInterpolator;
    kNewItem.m_fWeight = fWeight;
    kNewItem.m_cPriority = cPriority;
    kNewItem.m_fEaseSpinner = fEaseSpinner;

    // Update high priority.
    if (cPriority > m_cNextHighPriority)
    {
        if (cPriority > m_cHighPriority)
        {
            m_cNextHighPriority = m_cHighPriority;
            m_cHighPriority = cPriority;
        }
        else if (cPriority != m_cHighPriority)
        {
            m_cNextHighPriority = cPriority;
        }
    }

    // Set single index.
    if (++m_ucInterpCount == 1)
    {
        m_ucSingleIdx = ucIndex;
        m_pkSingleInterpolator = pkInterpolator;
    }
    else
    {
        m_ucSingleIdx = INVALID_INDEX;
        m_pkSingleInterpolator = NULL;
        m_fSingleTime = INVALID_TIME;
    }

    ClearWeightSums();    
    SetComputeNormalizedWeights(true);

    return ucIndex;
}
//---------------------------------------------------------------------------
NiInterpolatorPtr NiBlendInterpolator::RemoveInterpInfo(
    unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_ucArraySize);
    InterpArrayItem& kItem = m_pkInterpArray[ucIndex];

    // Remove interp info.
    NiInterpolatorPtr spInterpolator = kItem.m_spInterpolator;
    char cPriority = kItem.m_cPriority;
    kItem.ClearValues();

    // Update high priorities and weight sums.
    if (cPriority == m_cHighPriority ||
        cPriority == m_cNextHighPriority)
    {
        // Determine highest priority.
        m_cHighPriority = m_cNextHighPriority = SCHAR_MIN;
        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            InterpArrayItem& kTempItem = m_pkInterpArray[uc];
            if (kTempItem.m_spInterpolator != NULL)
            {
                if (kTempItem.m_cPriority > m_cNextHighPriority)
                {
                    if (kTempItem.m_cPriority > m_cHighPriority)
                    {
                        m_cNextHighPriority = m_cHighPriority;
                        m_cHighPriority = kTempItem.m_cPriority;
                    }
                    else if (kTempItem.m_cPriority < m_cHighPriority)
                    {
                        m_cNextHighPriority = kTempItem.m_cPriority;
                    }
                }
            }
        }
    }

    // Set single index.
    if (--m_ucInterpCount == 1)
    {
        if (m_ucArraySize == 2)
        {
            if (ucIndex == 0)
            {
                m_ucSingleIdx = 1;
            }
            else
            {
                m_ucSingleIdx = 0;
            }
        }
        else
        {
            for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
            {
                if (m_pkInterpArray[uc].m_spInterpolator != NULL)
                {
                    m_ucSingleIdx = uc;
                }
            }
        }
        m_pkSingleInterpolator = m_pkInterpArray[m_ucSingleIdx]
            .m_spInterpolator;
        m_fSingleTime = m_pkInterpArray[m_ucSingleIdx].m_fUpdateTime;
    }
    else if (m_ucInterpCount == 0)
    {
        m_ucSingleIdx = INVALID_INDEX;
        m_pkSingleInterpolator = NULL;
        m_fSingleTime = INVALID_TIME;
    }

    ClearWeightSums();
    SetComputeNormalizedWeights(true);

    return spInterpolator;
}
//---------------------------------------------------------------------------
unsigned char NiBlendInterpolator::GetHighestWeightedIndex() const
{
    if (m_ucInterpCount == 1)
    {
        return m_ucSingleIdx;
    }

    float fHighestWeight = 0.0f;
    unsigned char ucHighIndex = UCHAR_MAX;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        if (m_pkInterpArray[uc].m_fNormalizedWeight > fHighestWeight)
        {
            ucHighIndex = uc;
            fHighestWeight = m_pkInterpArray[uc].m_fNormalizedWeight;
        }
    }

    if (ucHighIndex != UCHAR_MAX)
    {
        return ucHighIndex;
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
bool NiBlendInterpolator::ReallocateArrays()
{
    if (ms_ucArrayGrowBy == 0)
    {
        return false;
    }

    // Allocate new array.
    unsigned char ucNewArraySize = m_ucArraySize + ms_ucArrayGrowBy;
    
    // There is a chance that the math could overflow. This assert should
    // catch the overflow condition.
    NIASSERT(ucNewArraySize > m_ucArraySize);
    
    InterpArrayItem* pkNewInterpArray = NiNew InterpArrayItem[ucNewArraySize];

    // Copy existing data.
    unsigned char uc;
    for (uc = 0; uc < m_ucArraySize; uc++)
    {
        pkNewInterpArray[uc] = m_pkInterpArray[uc];
    }

    // Delete existing data.
    NiDelete[] m_pkInterpArray;

    // Update variables.
    m_ucArraySize = ucNewArraySize;
    m_pkInterpArray = pkNewInterpArray;

    return true;
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::ComputeNormalizedWeights()
{
    if (!GetComputeNormalizedWeights())
    {
        return;
    }
        
    SetComputeNormalizedWeights(false);

    if (m_ucInterpCount == 1)
    {
        m_pkInterpArray[m_ucSingleIdx].m_fNormalizedWeight = 1.0f;
        return;
    }
    else if (m_ucInterpCount == 2)
    {
        ComputeNormalizedWeightsFor2();
        return;
    }

    unsigned char uc;

    if (m_fHighSumOfWeights == -NI_INFINITY)
    {
        // Compute sum of weights for highest and next highest priorities,
        // along with highest ease spinner for the highest priority.
        m_fHighSumOfWeights = 0.0f;
        m_fNextHighSumOfWeights = 0.0f;
        m_fHighEaseSpinner = 0.0f;
        for (uc = 0; uc < m_ucArraySize; uc++)
        {
            InterpArrayItem& kItem = m_pkInterpArray[uc];
            if (kItem.m_spInterpolator != NULL)
            {
                float fRealWeight = kItem.m_fWeight * kItem.m_fEaseSpinner;
                if (kItem.m_cPriority == m_cHighPriority)
                {
                    m_fHighSumOfWeights += fRealWeight;
                    if (kItem.m_fEaseSpinner > m_fHighEaseSpinner)
                    {
                        m_fHighEaseSpinner = kItem.m_fEaseSpinner;
                    }
                }
                else if (kItem.m_cPriority == m_cNextHighPriority)
                {
                    m_fNextHighSumOfWeights += fRealWeight;
                }
            }
        }
    }

    NIASSERT(m_fHighEaseSpinner >= 0.0f && m_fHighEaseSpinner <= 1.0f);
    float fOneMinusHighEaseSpinner = 1.0f - m_fHighEaseSpinner;
    float fTotalSumOfWeights = m_fHighEaseSpinner * m_fHighSumOfWeights +
        fOneMinusHighEaseSpinner * m_fNextHighSumOfWeights;
    float fOneOverTotalSumOfWeights = 
        (fTotalSumOfWeights > 0.0f) ? (1.0f / fTotalSumOfWeights) : 0.0f;

    // Compute normalized weights.
    for (uc = 0; uc < m_ucArraySize; uc++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[uc];
        if (kItem.m_spInterpolator != NULL)
        {
            if (kItem.m_cPriority == m_cHighPriority)
            {
                kItem.m_fNormalizedWeight = m_fHighEaseSpinner *
                    kItem.m_fWeight * kItem.m_fEaseSpinner *
                    fOneOverTotalSumOfWeights;
            }
            else if (kItem.m_cPriority == m_cNextHighPriority)
            {
                kItem.m_fNormalizedWeight = fOneMinusHighEaseSpinner *
                    kItem.m_fWeight * kItem.m_fEaseSpinner *
                    fOneOverTotalSumOfWeights;
            }
            else
            {
                kItem.m_fNormalizedWeight = 0.0f;
            }
        }
    }

    // Exclude weights below threshold, computing new sum in the process.
    float fSumOfNormalizedWeights = 1.0f;
    if (m_fWeightThreshold > 0.0f)
    {
        fSumOfNormalizedWeights = 0.0f;
        for (uc = 0; uc < m_ucArraySize; uc++)
        {
            InterpArrayItem& kItem = m_pkInterpArray[uc];
            if (kItem.m_spInterpolator != NULL &&
                kItem.m_fNormalizedWeight != 0.0f)
            {
                if (kItem.m_fNormalizedWeight < m_fWeightThreshold)
                {
                    kItem.m_fNormalizedWeight = 0.0f;
                }
                fSumOfNormalizedWeights += kItem.m_fNormalizedWeight;
            }
        }
    }

    // Renormalize weights if any were excluded earlier.
    if (fSumOfNormalizedWeights != 1.0f)
    {
        // Renormalize weights.
        float fOneOverSumOfNormalizedWeights = 
            (fSumOfNormalizedWeights > 0.0f) ?
            (1.0f / fSumOfNormalizedWeights) : 0.0f;

        for (uc = 0; uc < m_ucArraySize; uc++)
        {
            InterpArrayItem& kItem = m_pkInterpArray[uc];
            if (kItem.m_fNormalizedWeight != 0.0f)
            {
                kItem.m_fNormalizedWeight = kItem.m_fNormalizedWeight *
                    fOneOverSumOfNormalizedWeights;
            }
        }
    }

    // Only use the highest weight, if so directed.
    if (GetOnlyUseHighestWeight())
    {
        float fHighest = -1.0f;
        unsigned char ucHighIndex = INVALID_INDEX;
        for (uc = 0; uc < m_ucArraySize; uc++)
        {
            if (m_pkInterpArray[uc].m_fNormalizedWeight > fHighest)
            {
                ucHighIndex = uc;
                fHighest = m_pkInterpArray[uc].m_fNormalizedWeight;
            }
            m_pkInterpArray[uc].m_fNormalizedWeight = 0.0f;
        }

        NIASSERT(ucHighIndex != INVALID_INDEX);
        // Set the highest index to 1.0
        m_pkInterpArray[ucHighIndex].m_fNormalizedWeight = 1.0f;
    }
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::ComputeNormalizedWeightsFor2()
{
    NIASSERT(m_ucInterpCount == 2);

    // Get pointers to the two items.
    InterpArrayItem* pkItem1 = NULL;
    InterpArrayItem* pkItem2 = NULL;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        if (m_pkInterpArray[uc].m_spInterpolator)
        {
            if (!pkItem1)
            {
                pkItem1 = &m_pkInterpArray[uc];
                continue;
            }
            else if (!pkItem2)
            {
                pkItem2 = &m_pkInterpArray[uc];
                break;
            }
        }
    }
    NIASSERT(pkItem1 && pkItem2);

    // Calculate the real weight of each item.
    float fRealWeight1 = pkItem1->m_fWeight * pkItem1->m_fEaseSpinner;
    float fRealWeight2 = pkItem2->m_fWeight * pkItem2->m_fEaseSpinner;
    if (fRealWeight1 == 0.0f && fRealWeight2 == 0.0f)
    {
        pkItem1->m_fNormalizedWeight = 0.0f;
        pkItem2->m_fNormalizedWeight = 0.0f;
        return;
    }

    // Compute normalized weights.
    if (pkItem1->m_cPriority > pkItem2->m_cPriority)
    {
        if (pkItem1->m_fEaseSpinner == 1.0f)
        {
            pkItem1->m_fNormalizedWeight = 1.0f;
            pkItem2->m_fNormalizedWeight = 0.0f;
            return;
        }

        float fOneMinusEaseSpinner = 1.0f - pkItem1->m_fEaseSpinner;
        float fOneOverSumOfWeights = 1.0f / (pkItem1->m_fEaseSpinner *
            fRealWeight1 + fOneMinusEaseSpinner * fRealWeight2);
        pkItem1->m_fNormalizedWeight = pkItem1->m_fEaseSpinner * fRealWeight1
            * fOneOverSumOfWeights;
        pkItem2->m_fNormalizedWeight = fOneMinusEaseSpinner * fRealWeight2 *
            fOneOverSumOfWeights;
    }
    else if (pkItem1->m_cPriority < pkItem2->m_cPriority)
    {
        if (pkItem2->m_fEaseSpinner == 1.0f)
        {
            pkItem1->m_fNormalizedWeight = 0.0f;
            pkItem2->m_fNormalizedWeight = 1.0f;
            return;
        }

        float fOneMinusEaseSpinner = 1.0f - pkItem2->m_fEaseSpinner;
        float fOneOverSumOfWeights = 1.0f / (pkItem2->m_fEaseSpinner *
            fRealWeight2 + fOneMinusEaseSpinner * fRealWeight1);
        pkItem1->m_fNormalizedWeight = fOneMinusEaseSpinner * fRealWeight1 *
            fOneOverSumOfWeights;
        pkItem2->m_fNormalizedWeight = pkItem2->m_fEaseSpinner * fRealWeight2
            * fOneOverSumOfWeights;
    }
    else
    {
        float fOneOverSumOfWeights = 1.0f / (fRealWeight1 + fRealWeight2);
        pkItem1->m_fNormalizedWeight = fRealWeight1 * fOneOverSumOfWeights;
        pkItem2->m_fNormalizedWeight = fRealWeight2 * fOneOverSumOfWeights;
    }

    // Only use the highest weight, if so desired.
    if (GetOnlyUseHighestWeight())
    {
        if (pkItem1->m_fNormalizedWeight >= pkItem2->m_fNormalizedWeight)
        {
            pkItem1->m_fNormalizedWeight = 1.0f;
            pkItem2->m_fNormalizedWeight = 0.0f;
        }
        else
        {
            pkItem1->m_fNormalizedWeight = 0.0f;
            pkItem2->m_fNormalizedWeight = 1.0f;
        }
        return;
    }

    // Exclude weights below threshold.
    if (m_fWeightThreshold > 0.0f)
    {
        bool bReduced1 = false;
        if (pkItem1->m_fNormalizedWeight < m_fWeightThreshold)
        {
            pkItem1->m_fNormalizedWeight = 0.0f;
            bReduced1 = true;
        }

        bool bReduced2 = false;
        if (pkItem2->m_fNormalizedWeight < m_fWeightThreshold)
        {
            pkItem2->m_fNormalizedWeight = 0.0f;
            bReduced1 = true;
        }

        if (bReduced1 && bReduced2)
        {
            return;
        }
        else if (bReduced1)
        {
            pkItem2->m_fNormalizedWeight = 1.0f;
            return;
        }
        else if (bReduced2)
        {
            pkItem1->m_fNormalizedWeight = 1.0f;
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::Collapse()
{
    if (m_ucInterpCount == 1)
    {
        // Use the cached interpolator pointer for a single interpolator.
        m_pkSingleInterpolator->Collapse();
        return;
    }

    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[uc];
        if (kItem.m_spInterpolator != NULL)
        {
            kItem.m_spInterpolator->Collapse();
        }
    }
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_ucInterpCount == 1)
    {
        // Use the cached interpolator pointer for a single interpolator.
        m_pkSingleInterpolator->GuaranteeTimeRange(fStartTime, fEndTime);
        return;
    }

    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[uc];
        if (kItem.m_spInterpolator != NULL)
        {
            kItem.m_spInterpolator->GuaranteeTimeRange(fStartTime, fEndTime);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiBlendInterpolator::CopyMembers(NiBlendInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
    pkDest->m_ucArraySize = m_ucArraySize;
    pkDest->m_pkInterpArray = NiNew InterpArrayItem[m_ucArraySize];
    pkDest->m_fWeightThreshold = m_fWeightThreshold;

    if (!GetManagerControlled())
    {
        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            InterpArrayItem& kItem = m_pkInterpArray[uc];
            InterpArrayItem& kDestItem = pkDest->m_pkInterpArray[uc];
            if (kItem.m_spInterpolator)
            {
                kDestItem.m_spInterpolator = (NiInterpolator*)
                    kItem.m_spInterpolator->CreateClone(kCloning);
            }
            kDestItem.m_fWeight = kItem.m_fWeight;
            kDestItem.m_fNormalizedWeight = kItem.m_fNormalizedWeight;
            kDestItem.m_cPriority = kItem.m_cPriority;
            kDestItem.m_fEaseSpinner = kItem.m_fEaseSpinner;
            kDestItem.m_fUpdateTime = kItem.m_fUpdateTime;
        }

        pkDest->m_ucInterpCount = m_ucInterpCount;
        pkDest->m_ucSingleIdx = m_ucSingleIdx;
        pkDest->m_fSingleTime = m_fSingleTime;

        pkDest->m_cHighPriority = m_cHighPriority;
        pkDest->m_cNextHighPriority = m_cNextHighPriority;
    }
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::ProcessClone(NiCloningProcess& kCloning)
{
    NiInterpolator::ProcessClone(kCloning);

    NiObject* pkClone = 0;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiBlendInterpolator* pkDest = (NiBlendInterpolator*) pkClone;

    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        NiInterpolator* pkInterpolator = m_pkInterpArray[uc].m_spInterpolator;
        if (pkInterpolator)
        {
            pkInterpolator->ProcessClone(kCloning);
        }
    }

    if (m_pkSingleInterpolator)
    {
        NiInterpolator* pkClonedInterpolator;
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkSingleInterpolator,
            pkClone);
        if (bCloned)
        {
            pkClonedInterpolator = (NiInterpolator*) pkClone;
        }
        else
        {
            pkClonedInterpolator = m_pkSingleInterpolator;
        }
        pkDest->m_pkSingleInterpolator = pkClonedInterpolator;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiBlendInterpolator::InterpArrayItem::LoadBinary(NiStream& kStream)
{
    m_spInterpolator = (NiInterpolator*) kStream.ResolveLinkID();
    NiStreamLoadBinary(kStream, m_fWeight);
    NiStreamLoadBinary(kStream, m_fNormalizedWeight);
    NiStreamLoadBinary(kStream, m_cPriority);

    NiStreamLoadBinary(kStream, m_fEaseSpinner);
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::InterpArrayItem::SaveBinary(NiStream& kStream)
{
    kStream.SaveLinkID(m_spInterpolator);
    NiStreamSaveBinary(kStream, m_fWeight);
    NiStreamSaveBinary(kStream, m_fNormalizedWeight);
    NiStreamSaveBinary(kStream, m_cPriority);
    NiStreamSaveBinary(kStream, m_fEaseSpinner);
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::InterpArrayItem::RegisterStreamables(
    NiStream& kStream)
{
    if (m_spInterpolator)
    {
        m_spInterpolator->RegisterStreamables(kStream);
    }
}
//---------------------------------------------------------------------------
bool NiBlendInterpolator::InterpArrayItem::IsEqual(InterpArrayItem& kDest)
{
    if ((m_spInterpolator && !kDest.m_spInterpolator) ||
        (!m_spInterpolator && kDest.m_spInterpolator))
    {
        return false;
    }
    if (m_spInterpolator &&
        !m_spInterpolator->IsEqual(kDest.m_spInterpolator))
    {
        return false;
    }

    if (m_fWeight != kDest.m_fWeight ||
        m_fNormalizedWeight != kDest.m_fNormalizedWeight ||
        m_cPriority != kDest.m_cPriority ||
        m_fEaseSpinner != kDest.m_fEaseSpinner)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::LoadBinary(NiStream& kStream)
{
    NiInterpolator::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 112))
    {
        NiStreamLoadBinary(kStream, m_uFlags);

        // The ComputeNormalizedWeights flag really should not be streamed.
        // However, it is part of m_uFlags. Thus, override its streamed value
        // here.
        SetComputeNormalizedWeights(false);
    }

    NiStreamLoadBinary(kStream, m_ucArraySize);

    m_pkInterpArray = NiNew InterpArrayItem[m_ucArraySize];
    
    NiStreamLoadBinary(kStream, m_fWeightThreshold);

    if (!GetManagerControlled())
    {
        NiStreamLoadBinary(kStream, m_ucInterpCount);
        NiStreamLoadBinary(kStream, m_ucSingleIdx);
        NiStreamLoadBinary(kStream, m_cHighPriority);
        NiStreamLoadBinary(kStream, m_cNextHighPriority);

        NiStreamLoadBinary(kStream, m_fSingleTime);
        NiStreamLoadBinary(kStream, m_fHighSumOfWeights);
        NiStreamLoadBinary(kStream, m_fNextHighSumOfWeights);
        NiStreamLoadBinary(kStream, m_fHighEaseSpinner);

        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            m_pkInterpArray[uc].LoadBinary(kStream);
        }
        m_pkSingleInterpolator = (NiInterpolator*) kStream
            .ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::LinkObject(NiStream& kStream)
{
    NiInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (!GetManagerControlled())
    {
        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            m_pkInterpArray[uc].RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBlendInterpolator::SaveBinary(NiStream& kStream)
{
    NiInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_ucArraySize);
    NiStreamSaveBinary(kStream, m_fWeightThreshold);

    if (!GetManagerControlled())
    {
        NiStreamSaveBinary(kStream, m_ucInterpCount);
        NiStreamSaveBinary(kStream, m_ucSingleIdx);
        NiStreamSaveBinary(kStream, m_cHighPriority);
        NiStreamSaveBinary(kStream, m_cNextHighPriority);

        NiStreamSaveBinary(kStream, m_fSingleTime);
        NiStreamSaveBinary(kStream, m_fHighSumOfWeights);
        NiStreamSaveBinary(kStream, m_fNextHighSumOfWeights);
        NiStreamSaveBinary(kStream, m_fHighEaseSpinner);

        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            m_pkInterpArray[uc].SaveBinary(kStream);
        }
        kStream.SaveLinkID(m_pkSingleInterpolator);
    }
}
//---------------------------------------------------------------------------
bool NiBlendInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendInterpolator* pkDest = (NiBlendInterpolator*) pkObject;

    if (m_ucArraySize != pkDest->m_ucArraySize ||
        ms_ucArrayGrowBy != pkDest->ms_ucArrayGrowBy)
    {
        return false;
    }

    unsigned char us;
    for (us = 0; us < m_ucArraySize; us++)
    {
        if (!m_pkInterpArray[us].IsEqual(pkDest->m_pkInterpArray[us]))
        {
            return false;
        }
    }

    if (m_uFlags != pkDest->m_uFlags ||
        m_fWeightThreshold != pkDest->m_fWeightThreshold ||
        m_ucInterpCount != pkDest->m_ucInterpCount ||
        m_ucSingleIdx != pkDest->m_ucSingleIdx ||
        ((m_pkSingleInterpolator != NULL &&
            !m_pkSingleInterpolator->IsEqual(
            pkDest->m_pkSingleInterpolator)) ||
            m_pkSingleInterpolator != pkDest->m_pkSingleInterpolator ) ||
        m_fSingleTime != pkDest->m_fSingleTime)
    {
        return false;
    }

    if (m_cHighPriority != pkDest->m_cHighPriority ||
        m_cNextHighPriority != pkDest->m_cNextHighPriority)
    {
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendInterpolator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_ucArraySize", m_ucArraySize));
    pkStrings->Add(NiGetViewerString("ms_ucArrayGrowBy", ms_ucArrayGrowBy));
    pkStrings->Add(NiGetViewerString("ManagerControlled",
        GetManagerControlled()));

    pkStrings->Add(NiGetViewerString("m_fWeightThreshold",
        m_fWeightThreshold));
    pkStrings->Add(NiGetViewerString("m_bOnlyUseHighestWeight",
        GetOnlyUseHighestWeight()));

    pkStrings->Add(NiGetViewerString("m_ucInterpCount", m_ucInterpCount));
    pkStrings->Add(NiGetViewerString("m_ucSingleIdx", m_ucSingleIdx));
}
//---------------------------------------------------------------------------
