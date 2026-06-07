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

#include "NiPhysXFluidUpdateModifier.h"
#include "NiPhysXFluidSystem.h"
#include "fluids/NxFluid.h"

NiImplementRTTI(NiPhysXFluidUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPhysXFluidUpdateModifier::NiPhysXFluidUpdateModifier()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidUpdateModifier);
//---------------------------------------------------------------------------
bool NiPhysXFluidUpdateModifier::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::LoadBinary(NiStream& kStream)
{
   NiPSysModifier::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPhysXFluidUpdateModifier::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
