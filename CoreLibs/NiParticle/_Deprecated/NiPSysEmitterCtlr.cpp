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

#include "NiPSysEmitterCtlr.h"
#include "NiPSysEmitterCtlrData.h"
#include <NiFloatInterpolator.h>
#include <NiBlendFloatInterpolator.h>
#include <NiBoolInterpolator.h>
#include <NiBlendBoolInterpolator.h>

NiImplementRTTI(NiPSysEmitterCtlr, NiPSysModifierCtlr);

#define BIRTH_RATE_INDEX 0
#define EMITTER_ACTIVE_INDEX 1

//---------------------------------------------------------------------------
NiPSysEmitterCtlr::NiPSysEmitterCtlr()
{
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::Update(float)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::InterpolatorIsCorrectType(NiInterpolator* pkInterp,
    unsigned short usIndex) const
{
    if (usIndex == BIRTH_RATE_INDEX && 
        (NiIsKindOf(NiFloatInterpolator, pkInterp) ||
        NiIsKindOf(NiBlendFloatInterpolator, pkInterp)))
    {
        return true;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX && 
        (NiIsKindOf(NiBoolInterpolator, pkInterp) ||
        NiIsKindOf(NiBlendBoolInterpolator, pkInterp)))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned short NiPSysEmitterCtlr::GetInterpolatorCount() const
{
    return 2;
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysEmitterCtlr::GetInterpolator(unsigned short usIndex)
    const
{
    if (usIndex == BIRTH_RATE_INDEX)
    {
        return m_spInterpolator;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        return m_spEmitterActiveInterpolator;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SetInterpolator(NiInterpolator* pkInterpolator,
    unsigned short usIndex)
{
    if (usIndex == BIRTH_RATE_INDEX)
    {
        NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
            usIndex));
        m_spInterpolator = pkInterpolator;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
            usIndex));
        m_spEmitterActiveInterpolator = pkInterpolator;
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiPSysEmitterCtlr::CreatePoseEvaluator(
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");

    return NULL;
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysEmitterCtlr::CreatePoseInterpolator(
    unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SynchronizePoseInterpolator(
    NiInterpolator*, unsigned short)
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSysEmitterCtlr::CreateBlendInterpolator(
    unsigned short, bool,
    float,
    unsigned char) const
{
    NIASSERT(!"This class is deprecated and should only be used for "
        "streaming!");

    return NULL;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spData
        return;
    }
    kStream.ReadLinkID();       //m_spEmitterActiveInterpolator
        
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierCtlr::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiPSysEmitterCtlrDataPtr spData = (NiPSysEmitterCtlrData*) 
            kStream.GetObjectFromLinkID();
        if (spData)
        {
            NiFloatInterpolator* pkFloatInterp = NiNew NiFloatInterpolator();
            unsigned int uiNumKeys;
            NiFloatKey::KeyType eBirthRateKeyType;
            unsigned char ucSize;
            NiFloatKey* pkBirthRateKeys = spData->GetBirthRateKeys(uiNumKeys,
                eBirthRateKeyType, ucSize);
            pkFloatInterp->ReplaceKeys(pkBirthRateKeys, uiNumKeys, 
                eBirthRateKeyType);
            spData->SetBirthRateKeys(NULL, 0, NiFloatKey::NOINTERP); 
            SetInterpolator(pkFloatInterp, BIRTH_RATE_INDEX);
            
            NiBoolInterpolator* pkBoolInterp = NiNew NiBoolInterpolator();
            
            NiBoolKey::KeyType eEmitterActiveKeyType;
            NiBoolKey* pkEmitterActiveKeys = 
                spData->GetEmitterActiveKeys(uiNumKeys, 
                eEmitterActiveKeyType, ucSize);
            pkBoolInterp->ReplaceKeys(pkEmitterActiveKeys,
                uiNumKeys, eEmitterActiveKeyType);
            spData->SetEmitterActiveKeys(NULL, 0, NiAnimationKey::NOINTERP); 
            SetInterpolator(pkBoolInterp, EMITTER_ACTIVE_INDEX);
        }
        return;
    }
    m_spEmitterActiveInterpolator = (NiInterpolator*) 
        kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifierCtlr::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spEmitterActiveInterpolator)
         m_spEmitterActiveInterpolator->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::SaveBinary(kStream);

    kStream.SaveLinkID(m_spEmitterActiveInterpolator);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifierCtlr::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysEmitterCtlr* pkDest = (NiPSysEmitterCtlr*) pkObject;

    if ((pkDest->m_spEmitterActiveInterpolator == NULL &&
        m_spEmitterActiveInterpolator != NULL) ||
        (pkDest->m_spEmitterActiveInterpolator != NULL &&
        m_spEmitterActiveInterpolator == NULL))
    {
        return false;
    }
    else if (pkDest->m_spEmitterActiveInterpolator &&
        !pkDest->m_spEmitterActiveInterpolator->IsEqual(
        m_spEmitterActiveInterpolator))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
