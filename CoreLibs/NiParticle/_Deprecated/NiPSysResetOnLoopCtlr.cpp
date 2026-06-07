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

#include "NiPSysResetOnLoopCtlr.h"

NiImplementRTTI(NiPSysResetOnLoopCtlr, NiTimeController);

//---------------------------------------------------------------------------
NiPSysResetOnLoopCtlr::NiPSysResetOnLoopCtlr()
{
}
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::Update(float)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
bool NiPSysResetOnLoopCtlr::TargetIsRequiredType() const
{
     NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return false;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysResetOnLoopCtlr);
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysResetOnLoopCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiTimeController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysResetOnLoopCtlr::IsEqual(NiObject* pkObject)
{
    return NiTimeController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
