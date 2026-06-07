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

#include "NiLightDimmerController.h"
#include "NiFloatInterpolator.h"

NiImplementRTTI(NiLightDimmerController,NiFloatInterpController);

//---------------------------------------------------------------------------
NiLightDimmerController::NiLightDimmerController()
{ 
}

//---------------------------------------------------------------------------
NiLightDimmerController::~NiLightDimmerController()
{ /* */ }

//---------------------------------------------------------------------------
bool NiLightDimmerController::InterpTargetIsCorrectType(NiObjectNET* 
    pkTarget) const
{
    return (NiIsKindOf(NiLight, pkTarget));
}
//---------------------------------------------------------------------------
void NiLightDimmerController::Update(float fTime)
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
        float fValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, fValue))
        {
            if (m_pkTarget)
            {
                NiLight* pLight = NiDynamicCast(NiLight, m_pkTarget);
                pLight->SetDimmer(fValue);
            }
        }
    }
    
}
//---------------------------------------------------------------------------
bool NiLightDimmerController::UpdateValue(float, float fFloat, 
    unsigned short)
{
    NIASSERT(GetManagerControlled());
    if (m_pkTarget)
    {
        NiLight* pLight = NiDynamicCast(NiLight, m_pkTarget);
        pLight->SetDimmer(fFloat);
    }
    return true;
}
//---------------------------------------------------------------------------
void NiLightDimmerController::GetTargetFloatValue(float& fValue)
{
    if (m_pkTarget)
    {
        NiLight* pLight = NiSmartPointerCast(NiLight, m_pkTarget);
        fValue = pLight->GetDimmer();
    }
        
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLightDimmerController);
//---------------------------------------------------------------------------
void NiLightDimmerController::CopyMembers(NiLightDimmerController* pkDest,
    NiCloningProcess& kCloning)
{
    NiFloatInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLightDimmerController);
//---------------------------------------------------------------------------
void NiLightDimmerController::LoadBinary(NiStream& kStream)
{
    NiFloatInterpController::LoadBinary(kStream);  
}
//---------------------------------------------------------------------------
void NiLightDimmerController::LinkObject(NiStream& kStream)
{
    NiFloatInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLightDimmerController::RegisterStreamables(NiStream& kStream)
{
    if (!NiFloatInterpController::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiLightDimmerController::SaveBinary(NiStream& kStream)
{
    NiFloatInterpController::SaveBinary(kStream);

}
//---------------------------------------------------------------------------
bool NiLightDimmerController::IsEqual(NiObject* pkObject)
{
    if (!NiFloatInterpController::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiLightDimmerController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiFloatInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiLightDimmerController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

