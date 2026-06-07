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

#include "NiPhysXPSFluidEmitter.h"

NiImplementRTTI(NiPhysXPSFluidEmitter, NiPSEmitter);

//---------------------------------------------------------------------------
NiPhysXPSFluidEmitter::NiPhysXPSFluidEmitter(const NiFixedString& kName,
    NiRGBA kInitialColor, float fInitialRadius, float fRadiusVar) :
    NiPSEmitter(kName, 0.0f, 0.0f, 0.0f,0.0f, 0.0f, 0.0f, kInitialColor, 
    fInitialRadius, fRadiusVar,NI_INFINITY, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
    false, NiPoint3::ZERO, false)
{
}
//---------------------------------------------------------------------------
NiPhysXPSFluidEmitter::NiPhysXPSFluidEmitter()
{
}
//---------------------------------------------------------------------------
bool NiPhysXPSFluidEmitter::ComputeInitialPositionAndVelocity(
    NiPSParticleSystem*, NiPoint3&, 
    NiPoint3&)
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSFluidEmitter);
//---------------------------------------------------------------------------
bool NiPhysXPSFluidEmitter::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidEmitter::LoadBinary(NiStream& kStream)
{
    NiPSEmitter::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidEmitter::LinkObject(NiStream& kStream)
{
    NiPSEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSFluidEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSFluidEmitter::SaveBinary(NiStream& kStream)
{
    NiPSEmitter::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSFluidEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSEmitter::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSFluidEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPhysXPSFluidEmitter::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
