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

#include "NiPSysColorModifier.h"

NiImplementRTTI(NiPSysColorModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysColorModifier::NiPSysColorModifier()
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysColorModifier);
//---------------------------------------------------------------------------
void NiPSysColorModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spColorData
}
//---------------------------------------------------------------------------
void NiPSysColorModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_spColorData = (NiColorData*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysColorModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spColorData)
    {
        m_spColorData->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysColorModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_spColorData);
}
//---------------------------------------------------------------------------
bool NiPSysColorModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysColorModifier* pkDest = (NiPSysColorModifier*) pkObject;

    if (!m_spColorData->IsEqual(pkDest->m_spColorData))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
