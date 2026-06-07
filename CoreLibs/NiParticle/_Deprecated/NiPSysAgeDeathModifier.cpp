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

#include "NiPSysAgeDeathModifier.h"
#include <NiStream.h>
#include <NiBool.h>
#include "NiPSysSpawnModifier.h"

NiImplementRTTI(NiPSysAgeDeathModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysAgeDeathModifier::NiPSysAgeDeathModifier() :
    m_bSpawnOnDeath(false),
    m_pkSpawnModifier(NULL)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysAgeDeathModifier);
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiBool bSpawnOnDeath;
    NiStreamLoadBinary(kStream, bSpawnOnDeath);
    m_bSpawnOnDeath = (bSpawnOnDeath == 0) ? false : true;
    kStream.ReadLinkID();   // m_pkSpawnModifier
}
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkSpawnModifier = (NiPSysSpawnModifier*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysAgeDeathModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAgeDeathModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, NiBool(m_bSpawnOnDeath));
    kStream.SaveLinkID(m_pkSpawnModifier);
}
//---------------------------------------------------------------------------
bool NiPSysAgeDeathModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysAgeDeathModifier* pkDest = (NiPSysAgeDeathModifier*) pkObject;

    if (pkDest->m_bSpawnOnDeath != m_bSpawnOnDeath)
    {
        return false;
    }

    if ((pkDest->m_pkSpawnModifier && !m_pkSpawnModifier) ||
        (!pkDest->m_pkSpawnModifier && m_pkSpawnModifier) ||
        (pkDest->m_pkSpawnModifier && m_pkSpawnModifier &&
            !pkDest->m_pkSpawnModifier->IsEqual(m_pkSpawnModifier)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
