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
#include <NiParticlePCH.h>

#include "NiPSysUpdateCtlr.h"
#include <NiObjectNET.h>

NiImplementRTTI(NiPSysUpdateCtlr, NiTimeController);

//---------------------------------------------------------------------------
NiPSysUpdateCtlr::NiPSysUpdateCtlr()
{
    m_fLoKeyTime = m_fHiKeyTime = 0.0f;
}
//---------------------------------------------------------------------------
void NiPSysUpdateCtlr::Update(float)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
void NiPSysUpdateCtlr::SetTarget(NiObjectNET* pkTarget)
{
    NiTimeController::SetTarget(pkTarget);

    if (m_pkTarget)
    {
        // Ensure that this controller is the last one in m_pkTarget's
        // controller list. NiTimeController::SetTarget ensures that this is
        // the first controller in the list.
        NiTimeController* pkCtlr = m_pkTarget->GetControllers();
        NIASSERT(pkCtlr);
        while (pkCtlr->GetNext())
        {
            pkCtlr = pkCtlr->GetNext();
        }
        if (pkCtlr != this)
        {
            pkCtlr->SetNext(this);
            m_pkTarget->SetControllers(m_spNext);
            m_spNext = NULL;
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysUpdateCtlr::TargetIsRequiredType() const
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return false;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysUpdateCtlr);
//---------------------------------------------------------------------------
void NiPSysUpdateCtlr::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysUpdateCtlr::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysUpdateCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiTimeController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysUpdateCtlr::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysUpdateCtlr::IsEqual(NiObject* pkObject)
{
    return NiTimeController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
