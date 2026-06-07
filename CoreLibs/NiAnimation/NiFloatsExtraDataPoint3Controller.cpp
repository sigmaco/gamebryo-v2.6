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

#include "NiFloatsExtraDataPoint3Controller.h"
#include <NiFloatsExtraData.h>
#include "NiBlendPoint3Interpolator.h"
#include "NiFloatData.h"
#include "NiPoint3Interpolator.h"
#include "NiConstPoint3Evaluator.h"

NiImplementRTTI(NiFloatsExtraDataPoint3Controller, NiExtraDataController);

//---------------------------------------------------------------------------
NiFloatsExtraDataPoint3Controller::NiFloatsExtraDataPoint3Controller(
    const NiFixedString& kFloatsExtraDataName, int iFloatsExtraDataIndex) :
    NiExtraDataController(kFloatsExtraDataName),
    m_iFloatsExtraDataIndex(iFloatsExtraDataIndex) 
{
}
//---------------------------------------------------------------------------
NiFloatsExtraDataPoint3Controller::NiFloatsExtraDataPoint3Controller() :
    m_iFloatsExtraDataIndex(-1)
{
}
//---------------------------------------------------------------------------
NiFloatsExtraDataPoint3Controller::~NiFloatsExtraDataPoint3Controller()
{
}
//---------------------------------------------------------------------------
NiEvaluator* NiFloatsExtraDataPoint3Controller::CreatePoseEvaluator(
#ifdef NIDEBUG
    unsigned short usIndex)
#else
    unsigned short)
#endif
{
    NIASSERT(usIndex == 0);
    NiFloatsExtraData* pkFloatsED = NiSmartPointerCast(NiFloatsExtraData,
        m_spExtraData);
    NiPoint3 kPt (pkFloatsED->GetValue(m_iFloatsExtraDataIndex),
        pkFloatsED->GetValue(m_iFloatsExtraDataIndex + 1),
        pkFloatsED->GetValue(m_iFloatsExtraDataIndex + 2));
    return NiNew NiConstPoint3Evaluator(kPt);
}
//---------------------------------------------------------------------------
NiInterpolator* NiFloatsExtraDataPoint3Controller::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    NiFloatsExtraData* pkFloatsED = NiSmartPointerCast(NiFloatsExtraData,
        m_spExtraData);
    NiPoint3 kPt (pkFloatsED->GetValue(m_iFloatsExtraDataIndex),
        pkFloatsED->GetValue(m_iFloatsExtraDataIndex + 1),
        pkFloatsED->GetValue(m_iFloatsExtraDataIndex + 2));
    return NiNew NiPoint3Interpolator(kPt);
}
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::SynchronizePoseInterpolator(
    NiInterpolator* pkInterp, 
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NiPoint3Interpolator* pkFloatInterp = 
        NiDynamicCast(NiPoint3Interpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkFloatInterp);
    NiFloatsExtraData* pkFloatsED = NiSmartPointerCast(NiFloatsExtraData,
        m_spExtraData);
    NiPoint3 kPt (pkFloatsED->GetValue(m_iFloatsExtraDataIndex),
        pkFloatsED->GetValue(m_iFloatsExtraDataIndex + 1),
        pkFloatsED->GetValue(m_iFloatsExtraDataIndex + 2));
    pkFloatInterp->SetPoseValue(kPt);
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiFloatsExtraDataPoint3Controller::
    CreateBlendInterpolator(
    unsigned short usIndex,
    bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    return NiNew NiBlendPoint3Interpolator(
        bManagerControlled, fWeightThreshold, ucArraySize);
}
//---------------------------------------------------------------------------
const char* NiFloatsExtraDataPoint3Controller::GetCtlrID()
{
    if (m_kCtlrID.Exists())
        return m_kCtlrID;

    m_kCtlrID = NULL;
    const char* pcExtraDataName = GetExtraDataName();
    if (pcExtraDataName)
    {
        size_t iLength = strlen(pcExtraDataName) + 15;
        char* acString = NiAlloc(char,iLength);
        NiSprintf(acString, iLength, "%s[%d]", pcExtraDataName, 
            m_iFloatsExtraDataIndex);
        m_kCtlrID = acString;
        NiFree(acString);
    }
    return m_kCtlrID;
}  
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::Update(float fTime)
{
    if (GetManagerControlled())
    {
        return;
    }

    if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        NiPoint3 kValue;
            
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, kValue))
        {
            if (m_spExtraData)
            {
                NIASSERT(NiIsKindOf(NiFloatsExtraData, m_spExtraData));
                NiFloatsExtraData* pkFloatsED =
                    NiSmartPointerCast(NiFloatsExtraData, m_spExtraData);
                pkFloatsED->SetValue(m_iFloatsExtraDataIndex, kValue.x);
                pkFloatsED->SetValue(m_iFloatsExtraDataIndex + 1, kValue.y);
                pkFloatsED->SetValue(m_iFloatsExtraDataIndex + 2, kValue.z);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiFloatsExtraDataPoint3Controller::UpdateValue(float, 
    const NiPoint3& kPoint3, unsigned short)
{
    NIASSERT(GetManagerControlled());
    if (m_spExtraData)
    {
        NIASSERT(NiIsKindOf(NiFloatsExtraData, m_spExtraData));
        NiFloatsExtraData* pkFloatsED =
            NiSmartPointerCast(NiFloatsExtraData, m_spExtraData);
        pkFloatsED->SetValue(m_iFloatsExtraDataIndex, kPoint3.x);
        pkFloatsED->SetValue(m_iFloatsExtraDataIndex + 1, kPoint3.y);
        pkFloatsED->SetValue(m_iFloatsExtraDataIndex + 2, kPoint3.z);
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiFloatsExtraDataPoint3Controller::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, 
    unsigned short usIndex) const
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsPoint3ValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFloatsExtraDataPoint3Controller);
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::CopyMembers(
    NiFloatsExtraDataPoint3Controller* pkDest, NiCloningProcess& kCloning)
{
    NiExtraDataController::CopyMembers(pkDest, kCloning);

    pkDest->m_iFloatsExtraDataIndex = m_iFloatsExtraDataIndex;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFloatsExtraDataPoint3Controller);
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::LoadBinary(NiStream& kStream)
{
    NiExtraDataController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_iFloatsExtraDataIndex);
}
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::LinkObject(NiStream& kStream)
{
    NiExtraDataController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiFloatsExtraDataPoint3Controller::RegisterStreamables(NiStream& kStream)
{
    return NiExtraDataController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::SaveBinary(NiStream& kStream)
{
    NiExtraDataController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_iFloatsExtraDataIndex);
}
//---------------------------------------------------------------------------
bool NiFloatsExtraDataPoint3Controller::IsEqual(NiObject* pkObject)
{
    if (!NiExtraDataController::IsEqual(pkObject))
    {
        return false;
    }

    NiFloatsExtraDataPoint3Controller* pkDest = 
        (NiFloatsExtraDataPoint3Controller*) pkObject;

    if (m_iFloatsExtraDataIndex != pkDest->m_iFloatsExtraDataIndex)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiFloatsExtraDataPoint3Controller::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiExtraDataController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiFloatsExtraDataPoint3Controller::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_iFloatsExtraDataIndex",
        m_iFloatsExtraDataIndex));
}
//---------------------------------------------------------------------------
