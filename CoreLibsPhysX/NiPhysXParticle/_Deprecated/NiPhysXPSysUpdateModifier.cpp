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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXPSysUpdateModifier.h"

#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXPSysUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPhysXPSysUpdateModifier::NiPhysXPSysUpdateModifier()
{
}
//---------------------------------------------------------------------------
NiPhysXPSysUpdateModifier::~NiPhysXPSysUpdateModifier()
{
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSysUpdateModifier::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysUpdateModifier);
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

