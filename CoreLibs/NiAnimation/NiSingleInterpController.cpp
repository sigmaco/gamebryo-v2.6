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

#include "NiSingleInterpController.h"

NiImplementRTTI(NiSingleInterpController, NiInterpController);

//---------------------------------------------------------------------------
NiSingleInterpController::NiSingleInterpController()
{
}
//---------------------------------------------------------------------------
unsigned short NiSingleInterpController::GetInterpolatorCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
NiInterpolator* NiSingleInterpController::GetInterpolator(
    unsigned short usIndex) const
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    return m_spInterpolator;
}
//---------------------------------------------------------------------------
void NiSingleInterpController::SetInterpolator(NiInterpolator* pkInterpolator,
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
        usIndex));
    m_spInterpolator = pkInterpolator;
}
//---------------------------------------------------------------------------
const char* NiSingleInterpController::GetInterpolatorID(unsigned short) 
{
    return NULL;
}
//---------------------------------------------------------------------------
unsigned short NiSingleInterpController::GetInterpolatorIndex(
    const char*) const
{
    return 0;
}
//---------------------------------------------------------------------------
bool NiSingleInterpController::TargetIsRequiredType() const
{
    return InterpTargetIsCorrectType(m_pkTarget);
}
//---------------------------------------------------------------------------
void NiSingleInterpController::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spInterpolator)
    {
        m_spInterpolator->GuaranteeTimeRange(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
void NiSingleInterpController::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    NiInterpController::ResetTimeExtrema();
    if (m_spInterpolator)
    {
        m_spInterpolator->GetActiveTimeRange(m_fLoKeyTime, m_fHiKeyTime);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiSingleInterpController::CopyMembers(NiSingleInterpController* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpController::CopyMembers(pkDest, kCloning);

    if (m_spInterpolator)
    {
        pkDest->m_spInterpolator = (NiInterpolator*) m_spInterpolator
            ->CreateSharedClone(kCloning);
    }
}
//---------------------------------------------------------------------------
void NiSingleInterpController::ProcessClone(NiCloningProcess& kCloning)
{
    NiInterpController::ProcessClone(kCloning);

    if (m_spInterpolator)
    {
        m_spInterpolator->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiSingleInterpController::LoadBinary(NiStream& kStream)
{
    NiInterpController::LoadBinary(kStream);

    m_spInterpolator = (NiInterpolator*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiSingleInterpController::LinkObject(NiStream& kStream)
{
    NiInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiSingleInterpController::RegisterStreamables(NiStream& kStream)
{
    if (!NiInterpController::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spInterpolator)
    {
        m_spInterpolator->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiSingleInterpController::SaveBinary(NiStream& kStream)
{
    NiInterpController::SaveBinary(kStream);

    kStream.SaveLinkID(m_spInterpolator);
}
//---------------------------------------------------------------------------
bool NiSingleInterpController::IsEqual(NiObject* pkObject)
{
    if (!NiInterpController::IsEqual(pkObject))
    {
        return false;
    }

    NiSingleInterpController* pkDest = (NiSingleInterpController*) pkObject;

    if ((m_spInterpolator && !pkDest->m_spInterpolator) ||
        (!m_spInterpolator && pkDest->m_spInterpolator) ||
        (m_spInterpolator &&
            !m_spInterpolator->IsEqual(pkDest->m_spInterpolator)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiSingleInterpController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiSingleInterpController::ms_RTTI
        .GetName()));

    if (m_spInterpolator)
    {
        m_spInterpolator->GetViewerStrings(pkStrings);
    }
}
//---------------------------------------------------------------------------
