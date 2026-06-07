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

#include "NiPSysBoundUpdateModifier.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysBoundUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier() : m_sUpdateSkip(0)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysBoundUpdateModifier);
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    // Update Skip didn't previously allow for a AUTO_SKIP_UPDATE value
    if (kStream.GetFileVersion() <= NiStream::GetVersion(10, 1, 0, 100))
    {
        unsigned short usUpdateSkip;
        NiStreamLoadBinary(kStream, usUpdateSkip);

        SetUpdateSkip(usUpdateSkip);
    }
    else
    {
        NiStreamLoadBinary(kStream, m_sUpdateSkip);
        SetUpdateSkip(m_sUpdateSkip);
    }
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysBoundUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_sUpdateSkip);
}
//---------------------------------------------------------------------------
bool NiPSysBoundUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysBoundUpdateModifier* pkDest = (NiPSysBoundUpdateModifier*) pkObject;

    if (pkDest->m_sUpdateSkip != m_sUpdateSkip)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
