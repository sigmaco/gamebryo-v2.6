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

#include "NiVisController.h"
#include <NiAVObject.h>
#include "NiBoolInterpolator.h"
#include "NiBoolData.h"

NiImplementRTTI(NiVisController, NiBoolInterpController);

//---------------------------------------------------------------------------
NiVisController::NiVisController() 
{
}
//---------------------------------------------------------------------------
bool NiVisController::InterpTargetIsCorrectType(NiObjectNET* pkTarget) const
{
    return (NiIsKindOf(NiAVObject, pkTarget));
}
//---------------------------------------------------------------------------
void NiVisController::GetTargetBoolValue(bool& bValue)
{
    bValue = !((NiAVObject*) m_pkTarget)->GetAppCulled();
}
//---------------------------------------------------------------------------
void NiVisController::Update(float fTime)
{
    if (GetManagerControlled())
    {
        return;
    }

    if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        bool bValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, bValue))
            ((NiAVObject*)m_pkTarget)->SetAppCulled(!bValue);
    }
}
//---------------------------------------------------------------------------
bool NiVisController::UpdateValue(float, bool bBool, 
    unsigned short)
{
    NIASSERT(GetManagerControlled());
    NIASSERT(m_pkTarget);
    ((NiAVObject*)m_pkTarget)->SetAppCulled(!bBool);
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiVisController);
//---------------------------------------------------------------------------
void NiVisController::CopyMembers(NiVisController* pDest,
    NiCloningProcess& kCloning)
{
    NiBoolInterpController::CopyMembers(pDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiVisController);
//---------------------------------------------------------------------------
void NiVisController::LoadBinary(NiStream& kStream)
{
    NiBoolInterpController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiVisController::LinkObject(NiStream& kStream)
{
    NiBoolInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiVisController::RegisterStreamables(NiStream& kStream)
{
    return NiBoolInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiVisController::SaveBinary(NiStream& kStream)
{
    NiBoolInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiVisController::IsEqual(NiObject* pkObject)
{
    return NiBoolInterpController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
void NiVisController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiBoolInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiVisController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
