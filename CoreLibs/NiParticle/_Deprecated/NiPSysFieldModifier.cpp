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

#include "NiPSysFieldModifier.h"
#include <NiAVObject.h>
#include <NiBool.h>

NiImplementRTTI(NiPSysFieldModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysFieldModifier::NiPSysFieldModifier() : 
    m_pkFieldObj(NULL),
    m_fMagnitude(0.0f),
    m_fAttenuation(0.0f), 
    m_bUseMaxDistance(false),
    m_fMaxDistance(0.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkFieldObj
    NiStreamLoadBinary(kStream, m_fMagnitude);
    NiStreamLoadBinary(kStream, m_fAttenuation);

    NiBool kBool;
    NiStreamLoadBinary(kStream, kBool);
    m_bUseMaxDistance = (kBool != 0);

    NiStreamLoadBinary(kStream, m_fMaxDistance);
}
//---------------------------------------------------------------------------
void NiPSysFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkFieldObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkFieldObj);
    NiStreamSaveBinary(kStream, m_fMagnitude);
    NiStreamSaveBinary(kStream, m_fAttenuation);

    NiBool kBool = m_bUseMaxDistance;
    NiStreamSaveBinary(kStream, kBool);

    NiStreamSaveBinary(kStream, m_fMaxDistance);
}
//---------------------------------------------------------------------------
bool NiPSysFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysFieldModifier* pkDest = (NiPSysFieldModifier*) pkObject;

    if ((m_pkFieldObj && !pkDest->m_pkFieldObj) ||
        (!m_pkFieldObj && pkDest->m_pkFieldObj) ||
        (m_pkFieldObj && pkDest->m_pkFieldObj &&
            !m_pkFieldObj->IsEqual(pkDest->m_pkFieldObj)))
    {
        return false;
    }

    if (pkDest->m_fMagnitude != m_fMagnitude ||
        pkDest->m_fAttenuation != m_fAttenuation ||
        pkDest->m_bUseMaxDistance != m_bUseMaxDistance ||
        pkDest->m_fMaxDistance != m_fMaxDistance )
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
