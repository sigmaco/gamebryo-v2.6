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

#include "NiPSysModifierBoolCtlr.h"
#include <NiBoolData.h>
#include <NiBoolTimelineInterpolator.h>

NiImplementRTTI(NiPSysModifierBoolCtlr, NiPSysModifierCtlr);

//---------------------------------------------------------------------------
NiPSysModifierBoolCtlr::NiPSysModifierBoolCtlr()
{
}
//---------------------------------------------------------------------------
NiEvaluator* NiPSysModifierBoolCtlr::CreatePoseEvaluator(
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return NULL;
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysModifierBoolCtlr::CreatePoseInterpolator(
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::SynchronizePoseInterpolator(NiInterpolator*,
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSysModifierBoolCtlr::CreateBlendInterpolator(
    unsigned short,
    bool,
    float,
    unsigned char) const
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::Update(float)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
bool NiPSysModifierBoolCtlr::InterpolatorIsCorrectType(
    NiInterpolator*, unsigned short) const
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
    return false;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spBoolData
    }
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierCtlr::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiBoolData* pkBoolData = (NiBoolData*) kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiBoolTimelineInterpolator(pkBoolData);
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierBoolCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysModifierBoolCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
