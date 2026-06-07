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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluidEmitter.h"

NiImplementRTTI(NiPhysXFluidEmitter, NiPSysEmitter);

//---------------------------------------------------------------------------
NiPhysXFluidEmitter::NiPhysXFluidEmitter()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidEmitter);
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitter::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysEmitter::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::LinkObject(NiStream& kStream)
{
    NiPSysEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysEmitter::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysEmitter::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
