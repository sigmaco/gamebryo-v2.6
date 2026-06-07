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

#include "NiPSysGravityFieldModifier.h"

NiImplementRTTI(NiPSysGravityFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysGravityFieldModifier::NiPSysGravityFieldModifier() :
    m_kDirection(-NiPoint3::UNIT_Y)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysGravityFieldModifier);
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Direction
    m_kDirection.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGravityFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Direction
    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGravityFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysGravityFieldModifier* pkDest =
        (NiPSysGravityFieldModifier*) pkObject;

    if (pkDest->m_kDirection != m_kDirection)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
