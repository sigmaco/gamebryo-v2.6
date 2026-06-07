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

#include "NiPSysBombModifier.h"
#include <NiAVObject.h>

NiImplementRTTI(NiPSysBombModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysBombModifier::NiPSysBombModifier() :
    m_pkBombObj(NULL),
    m_kBombAxis(NiPoint3::UNIT_X),
    m_fDecay(0.0f),
    m_fDeltaV(0.0f),
    m_eDecayType(NONE),
    m_eSymmType(SPHERICAL)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysBombModifier);
//---------------------------------------------------------------------------
void NiPSysBombModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkBombObj
    m_kBombAxis.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fDecay);
    NiStreamLoadBinary(kStream, m_fDeltaV);
    NiStreamLoadEnum(kStream, m_eDecayType);
    NiStreamLoadEnum(kStream, m_eSymmType);
}
//---------------------------------------------------------------------------
void NiPSysBombModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkBombObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysBombModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysBombModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkBombObj);
    m_kBombAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fDecay);
    NiStreamSaveBinary(kStream, m_fDeltaV);
    NiStreamSaveEnum(kStream, m_eDecayType);
    NiStreamSaveEnum(kStream, m_eSymmType);
}
//---------------------------------------------------------------------------
bool NiPSysBombModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysBombModifier* pkDest = (NiPSysBombModifier*) pkObject;

    if ((m_pkBombObj && !pkDest->m_pkBombObj) ||
        (!m_pkBombObj && pkDest->m_pkBombObj) ||
        (m_pkBombObj && pkDest->m_pkBombObj &&
            !m_pkBombObj->IsEqual(pkDest->m_pkBombObj)))
    {
        return false;
    }

    if (m_kBombAxis != pkDest->m_kBombAxis ||
        m_fDecay != pkDest->m_fDecay ||
        m_fDeltaV != pkDest->m_fDeltaV ||
        m_eDecayType != pkDest->m_eDecayType ||
        m_eSymmType != pkDest->m_eSymmType)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
