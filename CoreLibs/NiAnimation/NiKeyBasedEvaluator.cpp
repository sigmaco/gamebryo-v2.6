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
#include "NiAnimationPCH.h"

#include "NiKeyBasedEvaluator.h"
#include <NiMath.h>
#include "NiEulerRotKey.h"

NiImplementRTTI(NiKeyBasedEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiKeyBasedEvaluator::NiKeyBasedEvaluator()
{
}
//---------------------------------------------------------------------------
NiKeyBasedEvaluator::~NiKeyBasedEvaluator()
{
}
//---------------------------------------------------------------------------
void NiKeyBasedEvaluator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;

    for (unsigned short us = 0; us < GetKeyChannelCount(); us++)
    {
        NiAnimationKey* pkBeginKey = GetKeyAt(0, us);
        NiAnimationKey* pkEndKey = GetKeyAt(GetKeyCount(us) - 1, us);
        if(pkBeginKey && pkEndKey)
        {
            if (pkBeginKey->GetTime() < fBeginKeyTime)
                fBeginKeyTime = pkBeginKey->GetTime();
            if (pkEndKey->GetTime() > fEndKeyTime)
                fEndKeyTime = pkEndKey->GetTime();
        }
    }

    if (fBeginKeyTime == NI_INFINITY && fEndKeyTime == -NI_INFINITY)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
unsigned int NiKeyBasedEvaluator::GetAllocatedSize(unsigned short 
    usChannel) const
{
    NIASSERT(usChannel < GetKeyChannelCount());
    if (GetKeyType(usChannel) == NiAnimationKey::EULERKEY)
    {
        NiEulerRotKey* pkEulerKey = (NiEulerRotKey*) GetKeyArray(usChannel);
        unsigned int uiSize = 0;
        if (pkEulerKey)
        {
            uiSize = sizeof(NiEulerRotKey);
            for (unsigned char uc = 0; uc < 3; uc++)
            {
                unsigned int uiNumKeys = pkEulerKey->GetNumKeys(uc);
                unsigned int uiKeySize = pkEulerKey->GetKeySize(uc);
                uiSize += uiNumKeys * uiKeySize;
            }           
        }
        return uiSize;
    }
    return GetKeyCount(usChannel) * GetKeyStride(usChannel);
}
//---------------------------------------------------------------------------
NiAnimationKey* NiKeyBasedEvaluator::GetKeyAt(unsigned int uiWhichKey, 
    unsigned short usChannel) const
{
    NIASSERT(usChannel < GetKeyChannelCount());
    return GetKeyArray(usChannel)->GetKeyAt(uiWhichKey,
        GetKeyStride(usChannel));
}
//---------------------------------------------------------------------------
NiAnimationKey* NiKeyBasedEvaluator::GetAnim(unsigned int &uiNumKeys, 
    NiAnimationKey::KeyType &eType, unsigned char &ucSize,
    unsigned short usChannel) const
{
    NIASSERT(usChannel < GetKeyChannelCount());
    uiNumKeys = GetKeyCount(usChannel);
    eType = GetKeyType(usChannel);
    ucSize = GetKeyStride(usChannel);
    return GetKeyArray(usChannel);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiKeyBasedEvaluator::CopyMembers(NiKeyBasedEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiKeyBasedEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiKeyBasedEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiKeyBasedEvaluator::RegisterStreamables(NiStream& kStream)
{
    return NiEvaluator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiKeyBasedEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiKeyBasedEvaluator::IsEqual(NiObject* pkOther)
{
    return NiEvaluator::IsEqual(pkOther);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiKeyBasedEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiKeyBasedEvaluator::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
