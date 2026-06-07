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

#include "NiKeyBasedInterpolator.h"
#include "NiEulerRotKey.h"
#include <NiMath.h>

NiImplementRTTI(NiKeyBasedInterpolator, NiInterpolator);

//---------------------------------------------------------------------------
NiKeyBasedInterpolator::NiKeyBasedInterpolator()
{
}
//---------------------------------------------------------------------------
NiKeyBasedInterpolator::~NiKeyBasedInterpolator()
{
}
//---------------------------------------------------------------------------
void NiKeyBasedInterpolator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;

    for (unsigned int ui = 0; ui < GetKeyChannelCount(); ui++)
    {
        NiAnimationKey* pkBeginKey = GetKeyAt(0, (unsigned char)ui);
        NiAnimationKey* pkEndKey = 
            GetKeyAt(GetKeyCount((unsigned char)ui) - 1, (unsigned char)ui);
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
unsigned int NiKeyBasedInterpolator::GetAllocatedSize(unsigned short 
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
            for (unsigned int ui = 0; ui < 3; ui++)
            {
                unsigned int uiNumKeys =
                    pkEulerKey->GetNumKeys((unsigned char)ui);
                unsigned int uiKeySize = 
                    pkEulerKey->GetKeySize((unsigned char)ui);
                uiSize += uiNumKeys * uiKeySize;
            }           
        }
        return uiSize;
    }
    return GetKeyCount(usChannel) * GetKeyStride(usChannel);
}
//---------------------------------------------------------------------------
NiAnimationKey* NiKeyBasedInterpolator::GetKeyAt(unsigned int uiWhichKey, 
    unsigned short usChannel) const
{
    NIASSERT(usChannel < GetKeyChannelCount());
    return GetKeyArray(usChannel)->GetKeyAt(uiWhichKey,
        GetKeyStride(usChannel));
}
//---------------------------------------------------------------------------
NiAnimationKey* NiKeyBasedInterpolator::GetAnim(unsigned int &uiNumKeys, 
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
void NiKeyBasedInterpolator::FillDerivedValues(unsigned short usChannel)
{
    unsigned int uiNumKeys; 
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;

    NiAnimationKey* pkKeys = GetAnim(uiNumKeys, eType, ucSize,
        usChannel);

    NIASSERT(pkKeys);
    NiAnimationKey::FillDerivedValsFunction pDerived;

    pDerived = NiAnimationKey::GetFillDerivedFunction(
        GetKeyContent(usChannel), eType);
    NIASSERT(pDerived);

    (*pDerived)(pkKeys, uiNumKeys, ucSize);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiKeyBasedInterpolator::CopyMembers(NiKeyBasedInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpolator::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiKeyBasedInterpolator::LoadBinary(NiStream& kStream)
{
    NiInterpolator::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiKeyBasedInterpolator::LinkObject(NiStream& kStream)
{
    NiInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiKeyBasedInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiKeyBasedInterpolator::SaveBinary(NiStream& kStream)
{
    NiInterpolator::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiKeyBasedInterpolator::IsEqual(NiObject* pkOther)
{
    return NiInterpolator::IsEqual(pkOther);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiKeyBasedInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiKeyBasedInterpolator::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
