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

#include "NiPSysGravityModifier.h"
#include <NiAVObject.h>

NiImplementRTTI(NiPSysGravityModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysGravityModifier::NiPSysGravityModifier() :
    m_pkGravityObj(NULL),
    m_kGravityAxis(NiPoint3::UNIT_X),
    m_fDecay(0.0f),
    m_fStrength(1.0f),
    m_eType(FORCE_PLANAR),
    m_fTurbulence(0.0f),
    m_fScale(1.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysGravityModifier);
//---------------------------------------------------------------------------
void NiPSysGravityModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkGravityObj
    m_kGravityAxis.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fDecay);
    NiStreamLoadBinary(kStream, m_fStrength);
    NiStreamLoadEnum(kStream, m_eType);

    NiStreamLoadBinary(kStream, m_fTurbulence);
    NiStreamLoadBinary(kStream, m_fScale);
}
//---------------------------------------------------------------------------
void NiPSysGravityModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkGravityObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysGravityModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkGravityObj);
    m_kGravityAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fDecay);
    NiStreamSaveBinary(kStream, m_fStrength);
    NiStreamSaveEnum(kStream, m_eType);
    NiStreamSaveBinary(kStream, m_fTurbulence);
    NiStreamSaveBinary(kStream, m_fScale);
}
//---------------------------------------------------------------------------
bool NiPSysGravityModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysGravityModifier* pkDest = (NiPSysGravityModifier*) pkObject;

    if ((m_pkGravityObj && !pkDest->m_pkGravityObj) ||
        (!m_pkGravityObj && pkDest->m_pkGravityObj) ||
        (m_pkGravityObj && pkDest->m_pkGravityObj &&
            !m_pkGravityObj->IsEqual(pkDest->m_pkGravityObj)))
    {
        return false;
    }

    if (pkDest->m_kGravityAxis != m_kGravityAxis ||
        pkDest->m_fDecay != m_fDecay ||
        pkDest->m_fStrength != m_fStrength ||
        pkDest->m_eType != m_eType ||
        pkDest->m_fTurbulence != m_fTurbulence ||
        pkDest->m_fScale != m_fScale)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
