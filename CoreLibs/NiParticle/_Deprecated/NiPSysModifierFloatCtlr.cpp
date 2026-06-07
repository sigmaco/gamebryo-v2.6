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

#include "NiPSysModifierFloatCtlr.h"
#include <NiFloatData.h>
#include <NiFloatInterpolator.h>

NiImplementRTTI(NiPSysModifierFloatCtlr, NiPSysModifierCtlr);

//---------------------------------------------------------------------------
NiPSysModifierFloatCtlr::NiPSysModifierFloatCtlr()
{
}
//---------------------------------------------------------------------------
NiEvaluator* NiPSysModifierFloatCtlr::CreatePoseEvaluator(
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return NULL;
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysModifierFloatCtlr::CreatePoseInterpolator(
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::SynchronizePoseInterpolator(
    NiInterpolator*, unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSysModifierFloatCtlr::CreateBlendInterpolator(
    unsigned short, bool,
    float,
    unsigned char) const
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::Update(float)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
bool NiPSysModifierFloatCtlr::InterpolatorIsCorrectType(
    NiInterpolator*, unsigned short) const
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return false;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spData
    }
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierCtlr::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiFloatData* pkFloatData = (NiFloatData*)
            kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiFloatInterpolator(pkFloatData);
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierFloatCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysModifierFloatCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
