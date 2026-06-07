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

#include "NiBSplineInterpolator.h"
#include <NiMath.h>
NiImplementRTTI(NiBSplineInterpolator, NiInterpolator);

//---------------------------------------------------------------------------
NiBSplineInterpolator::NiBSplineInterpolator(NiBSplineData* pkData, 
    NiBSplineBasisData* pkBasisData) :  m_fStartTime(NI_INFINITY),
    m_fEndTime(-NI_INFINITY), m_spData(pkData), m_spBasisData(pkBasisData)
{
}
//---------------------------------------------------------------------------
NiBSplineInterpolator::~NiBSplineInterpolator()
{
}
//---------------------------------------------------------------------------
void NiBSplineInterpolator::GetActiveTimeRange(float& fBeginTime,
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
void NiBSplineInterpolator::Collapse()
{
}
//---------------------------------------------------------------------------
void NiBSplineInterpolator::GuaranteeTimeRange(float, float)
{
    NIASSERT(!"GuaranteeTimeRange should not be called on "
       "NiBSplineInterpolator.");
}
//---------------------------------------------------------------------------
NiInterpolator* NiBSplineInterpolator::GetSequenceInterpolator(
    float, float)
{
    NIASSERT(!"GetSequenceInterpolator should not be called on "
        "NiBSplineInterpolator.");
    return NULL;
}
//---------------------------------------------------------------------------
void NiBSplineInterpolator::SetTimeRange(float fStart, float fEnd)
{
    m_fStartTime = fStart;
    m_fEndTime = fEnd;
}
//---------------------------------------------------------------------------
NiBSplineData* NiBSplineInterpolator::GetData() const
{
    return m_spData;
}
//---------------------------------------------------------------------------
void NiBSplineInterpolator::SetData(NiBSplineData* pkData, 
    NiBSplineBasisData* pkBasisData)
{
    m_spData = pkData; 
    m_spBasisData = pkBasisData;
}
//---------------------------------------------------------------------------
NiBSplineBasisData* NiBSplineInterpolator::GetBasisData() const
{
    return m_spBasisData;
}
//---------------------------------------------------------------------------
bool NiBSplineInterpolator::UsesCompressedControlPoints() const
{
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineInterpolator::GetAllocatedSize(
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
unsigned int NiBSplineInterpolator::GetControlPointCount(
    unsigned short usChannel) const
{
    if(usChannel >= GetChannelCount())
        return 0;

    if (m_spData == 0 || NiBSplineData::INVALID_HANDLE == 
        GetControlHandle(usChannel))
    {
        return 0;
    }

    NiBSplineBasis<float, 3>& kBasis = 
        m_spBasisData->GetDegree3Basis();

    return kBasis.GetQuantity();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiBSplineInterpolator::CopyMembers(NiBSplineInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpolator::CopyMembers(pkDest, kCloning);
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
void NiBSplineInterpolator::LoadBinary(NiStream& kStream)
{
    NiInterpolator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fStartTime);
    NiStreamLoadBinary(kStream, m_fEndTime);
    m_spData = (NiBSplineData*) kStream.ResolveLinkID();
    m_spBasisData = (NiBSplineBasisData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiBSplineInterpolator::LinkObject(NiStream& kStream)
{
    NiInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiInterpolator::RegisterStreamables(kStream))
        return false;

    if (m_spData)
        m_spData->RegisterStreamables(kStream);

    if (m_spBasisData)
        m_spBasisData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiBSplineInterpolator::SaveBinary(NiStream& kStream)
{
    NiInterpolator::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fStartTime);
    NiStreamSaveBinary(kStream, m_fEndTime);
    kStream.SaveLinkID(m_spData);
    kStream.SaveLinkID(m_spBasisData);
}
//---------------------------------------------------------------------------
bool NiBSplineInterpolator::IsEqual(NiObject* pkOther)
{
    if (!NiInterpolator::IsEqual(pkOther))
        return false;

    const NiBSplineInterpolator* pkOtherInterp = (const NiBSplineInterpolator*)
        pkOther;
    if (m_fStartTime != pkOtherInterp->m_fStartTime ||
        m_fEndTime != pkOtherInterp->m_fEndTime)
    {
        return false;
    }
    
    if ((m_spData && !pkOtherInterp->m_spData) ||
        (!m_spData && pkOtherInterp->m_spData) ||
        (m_spData && !m_spData->IsEqual(pkOtherInterp->m_spData)))
    {
        return false;
    }

    if ((m_spBasisData && !pkOtherInterp->m_spBasisData) ||
        (!m_spBasisData && pkOtherInterp->m_spBasisData) ||
        (m_spBasisData && 
        !m_spBasisData->IsEqual(pkOtherInterp->m_spBasisData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineInterpolator::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_fStartTime",
        m_fStartTime));
    pkStrings->Add(NiGetViewerString("m_fEndTime",
        m_fEndTime));
    
    if (m_spData)
        m_spData->GetViewerStrings(pkStrings);

    if (m_spBasisData)
        m_spBasisData->GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
