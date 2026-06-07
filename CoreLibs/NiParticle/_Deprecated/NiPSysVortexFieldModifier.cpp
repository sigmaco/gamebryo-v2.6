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

#include "NiPSysVortexFieldModifier.h"

NiImplementRTTI(NiPSysVortexFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysVortexFieldModifier::NiPSysVortexFieldModifier() :
    m_kDirection(NiPoint3::ZERO)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysVortexFieldModifier);
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    m_kDirection.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysVortexFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysVortexFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysVortexFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysVortexFieldModifier* pkDest = (NiPSysVortexFieldModifier*) pkObject;

    if (pkDest->m_kDirection != m_kDirection)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
