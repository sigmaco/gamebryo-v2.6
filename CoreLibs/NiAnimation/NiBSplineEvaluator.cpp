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

#include "NiBSplineEvaluator.h"
#include <NiMath.h>

NiImplementRTTI(NiBSplineEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiBSplineEvaluator::NiBSplineEvaluator(NiBSplineData* pkData, 
    NiBSplineBasisData* pkBasisData) :  m_fStartTime(NI_INFINITY),
    m_fEndTime(-NI_INFINITY), m_spData(pkData), m_spBasisData(pkBasisData)
{
}
//---------------------------------------------------------------------------
NiBSplineEvaluator::~NiBSplineEvaluator()
{
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::GetActiveTimeRange(float& fBeginTime,
    float& fEndTime) const
{
    if (fBeginTime == NI_INFINITY && fEndTime == -NI_INFINITY)
    {
        fBeginTime = 0.0f;
        fEndTime = 0.0f;
    }
    else
    {
        fBeginTime = m_fStartTime;
        fEndTime = m_fEndTime;
    }
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::Collapse()
{
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::GuaranteeTimeRange(float, 
        float)
{
    NIASSERT(!"GuaranteeTimeRange should not be called on "
       "NiBSplineEvaluator.");
}
//---------------------------------------------------------------------------
NiEvaluator* NiBSplineEvaluator::GetSequenceEvaluator(
    float, float)
{
    NIASSERT(!"GetSequenceEvaluator should not be called on "
        "NiBSplineEvaluator.");
    return NULL;
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::SetTimeRange(float fStart, float fEnd)
{
    m_fStartTime = fStart;
    m_fEndTime = fEnd;
}
//---------------------------------------------------------------------------
NiBSplineData* NiBSplineEvaluator::GetData() const
{
    return m_spData;
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::SetData(NiBSplineData* pkData, 
    NiBSplineBasisData* pkBasisData)
{
    m_spData = pkData; 
    m_spBasisData = pkBasisData;
}
//---------------------------------------------------------------------------
NiBSplineBasisData* NiBSplineEvaluator::GetBasisData() const
{
    return m_spBasisData;
}
//---------------------------------------------------------------------------
bool NiBSplineEvaluator::UsesCompressedControlPoints() const
{
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineEvaluator::GetAllocatedSize(
    unsigned short usChannel) const
{
    NIASSERT(usChannel < GetChannelCount());
    if (UsesCompressedControlPoints())
    {
        return GetControlPointCount(usChannel) * GetDimension(usChannel) * 
            sizeof(short);
    }
    else
    {
        return GetControlPointCount(usChannel) * GetDimension(usChannel) * 
            sizeof(float);
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplineEvaluator::GetControlPointCount(
    unsigned short usChannel) const
{
    if (usChannel >= GetChannelCount())
        return 0;

    if (m_spData == 0 || NiBSplineData::INVALID_HANDLE == 
        GetControlHandle(usChannel))
    {
        return 0;
    }

    NiBSplineBasis<float, 3>& kBasis = m_spBasisData->GetDegree3Basis();

    return kBasis.GetQuantity();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiBSplineEvaluator::CopyMembers(NiBSplineEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);
    pkDest->m_fEndTime = m_fEndTime;
    pkDest->m_fStartTime = m_fStartTime;    
    pkDest->m_spData = m_spData;

    // Because the basis data caches values, it should not be shared
    // across objects. There is a chance that the two new scene graphs
    // could be updated in separate threads. Therefore, the basis data
    // should be cloned, but only once per scene graph clone operation.
    if (m_spBasisData)
    {
        pkDest->m_spBasisData = (NiBSplineBasisData*)m_spBasisData->
            CreateSharedClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiBSplineEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fStartTime);
    NiStreamLoadBinary(kStream, m_fEndTime);
    m_spData = (NiBSplineData*) kStream.ResolveLinkID();
    m_spBasisData = (NiBSplineBasisData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
        return false;

    if (m_spData)
        m_spData->RegisterStreamables(kStream);

    if (m_spBasisData)
        m_spBasisData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiBSplineEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fStartTime);
    NiStreamSaveBinary(kStream, m_fEndTime);
    kStream.SaveLinkID(m_spData);
    kStream.SaveLinkID(m_spBasisData);
}
//---------------------------------------------------------------------------
bool NiBSplineEvaluator::IsEqual(NiObject* pkOther)
{
    if (!NiEvaluator::IsEqual(pkOther))
        return false;

    const NiBSplineEvaluator* pkOtherEval = (const NiBSplineEvaluator*)
        pkOther;
    if (m_fStartTime != pkOtherEval->m_fStartTime ||
        m_fEndTime != pkOtherEval->m_fEndTime)
    {
        return false;
    }
    
    if ((m_spData && !pkOtherEval->m_spData) ||
        (!m_spData && pkOtherEval->m_spData) ||
        (m_spData && !m_spData->IsEqual(pkOtherEval->m_spData)))
    {
        return false;
    }

    if ((m_spBasisData && !pkOtherEval->m_spBasisData) ||
        (!m_spBasisData && pkOtherEval->m_spBasisData) ||
        (m_spBasisData && 
        !m_spBasisData->IsEqual(pkOtherEval->m_spBasisData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineEvaluator::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_fStartTime", m_fStartTime));
    pkStrings->Add(NiGetViewerString("m_fEndTime", m_fEndTime));
    
    if (m_spData)
        m_spData->GetViewerStrings(pkStrings);

    if (m_spBasisData)
        m_spBasisData->GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
