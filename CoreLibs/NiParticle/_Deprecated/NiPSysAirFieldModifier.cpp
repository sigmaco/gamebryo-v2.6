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

#include "NiPSysAirFieldModifier.h"
#include <NiBool.h>

NiImplementRTTI(NiPSysAirFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysAirFieldModifier::NiPSysAirFieldModifier() : NiPSysFieldModifier()
{
    SetDirection(-NiPoint3::UNIT_X);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysAirFieldModifier);
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Direction
    m_kDirection.LoadBinary(kStream);

    // Set Internal Variables
    SetDirection(m_kDirection);

    NiStreamLoadBinary(kStream, m_fAirFriction);

    NiStreamLoadBinary(kStream, m_fInheritVelocity);

    NiBool kBool;
    NiStreamLoadBinary(kStream, kBool);
    m_bInheritRotation = kBool ? true : false;

    NiStreamLoadBinary(kStream, kBool);
    m_bComponentOnly = kBool ? true : false;

    NiStreamLoadBinary(kStream, kBool);
    m_bEnableSpread = kBool ? true : false;

    NiStreamLoadBinary(kStream, m_fSpread);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Direction
    m_kDirection.SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fAirFriction);

    NiStreamSaveBinary(kStream, m_fInheritVelocity);

    NiBool kBool = m_bInheritRotation;
    NiStreamSaveBinary(kStream, kBool);

    kBool = m_bComponentOnly;
    NiStreamSaveBinary(kStream, kBool);

    kBool = m_bEnableSpread;
    NiStreamSaveBinary(kStream, kBool);

    NiStreamSaveBinary(kStream, m_fSpread);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysAirFieldModifier* pkDest = (NiPSysAirFieldModifier*) pkObject;

    if (pkDest->m_kDirection != m_kDirection)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
