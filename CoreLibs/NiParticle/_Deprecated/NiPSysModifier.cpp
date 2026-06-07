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

#include "NiPSysModifier.h"
#include <NiParticleSystem.h>

NiImplementRTTI(NiPSysModifier, NiObject);

//---------------------------------------------------------------------------
NiPSysModifier::NiPSysModifier() :
    m_uiOrder(0),
    m_pkTarget(NULL),
    m_bActive(true)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysModifier::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kName);
    }
    else
    {
        kStream.LoadFixedString(m_kName);
    }

    NiStreamLoadBinary(kStream, m_uiOrder);
    kStream.ReadLinkID();   // m_pkTarget
    NiBool bActive;
    NiStreamLoadBinary(kStream, bActive);
    m_bActive = (bActive == 0) ? false : true;
}
//---------------------------------------------------------------------------
void NiPSysModifier::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    kStream.GetObjectFromLinkID();  // m_pkTarget
}
//---------------------------------------------------------------------------
bool NiPSysModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    kStream.RegisterFixedString(m_kName);

    return true;
}
//---------------------------------------------------------------------------
void NiPSysModifier::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NIASSERT(m_kName.Exists());
    kStream.SaveFixedString(m_kName);
    NiStreamSaveBinary(kStream, m_uiOrder);
    kStream.SaveLinkID(m_pkTarget);
    NiStreamSaveBinary(kStream, NiBool(m_bActive));
}
//---------------------------------------------------------------------------
bool NiPSysModifier::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysModifier* pkDest = (NiPSysModifier*) pkObject;
    if (m_kName != pkDest->m_kName)
    {
        return false;
    }

    if (m_uiOrder != pkDest->m_uiOrder ||
        m_bActive != pkDest->m_bActive)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
