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

#include "NiMaterialColorController.h"
#include "NiPosData.h"
#include "NiPoint3Interpolator.h"

NiImplementRTTI(NiMaterialColorController,NiPoint3InterpController);


//---------------------------------------------------------------------------
NiMaterialColorController::NiMaterialColorController()
{
    m_uFlags = 0;
    SetType(AMB);
}
//---------------------------------------------------------------------------
NiMaterialColorController::~NiMaterialColorController()
{
}
//---------------------------------------------------------------------------
bool NiMaterialColorController::InterpTargetIsCorrectType(NiObjectNET* 
    pkTarget) const
{
    return (NiIsKindOf(NiMaterialProperty, pkTarget));
}
//---------------------------------------------------------------------------
void NiMaterialColorController::GetTargetPoint3Value(NiPoint3& kValue)
{
    NiMaterialProperty* pkMaterial = (NiMaterialProperty*) m_pkTarget;
    NiColor kColor;

    switch (GetType())
    {
        case AMB:
            kColor = pkMaterial->GetAmbientColor();
            break;
        case DIFF:
            kColor = pkMaterial->GetDiffuseColor();
            break;
        case SPEC:
            kColor = pkMaterial->GetSpecularColor();
            break;
        case SELF_ILLUM:
            kColor = pkMaterial->GetEmittance();
            break;
    }
    kValue.x = kColor.r;
    kValue.y = kColor.g;
    kValue.z = kColor.b;
}
//---------------------------------------------------------------------------
void NiMaterialColorController::Update(float fTime)
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
        NiPoint3 kValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, kValue))
        {
            UpdateValue(fTime, kValue, 0);
        }
    }
}
//---------------------------------------------------------------------------
bool NiMaterialColorController::UpdateValue(float, 
    const NiPoint3& kPoint3, unsigned short)
{
    NiMaterialProperty* pkMaterial = (NiMaterialProperty*) m_pkTarget;
    NiColor kColor(kPoint3.x, kPoint3.y, kPoint3.z);

    if (kColor.r < 0.0f)
        kColor.r = 0.0f;
    else if (kColor.r > 1.0f)
        kColor.r = 1.0f;

    if (kColor.g < 0.0f)
        kColor.g = 0.0f;
    else if (kColor.g > 1.0f)
        kColor.g = 1.0f;

    if (kColor.b < 0.0f)
        kColor.b = 0.0f;
    else if (kColor.b > 1.0f)
        kColor.b = 1.0f;

    switch (GetType())
    {
        case AMB:
            pkMaterial->SetAmbientColor(kColor);
            break;
        case DIFF:
            pkMaterial->SetDiffuseColor(kColor);
            break;
        case SPEC:
            pkMaterial->SetSpecularColor(kColor);
            break;
        case SELF_ILLUM:
            pkMaterial->SetEmittance(kColor);
            break;
    }

    return true;
}
//---------------------------------------------------------------------------
const char* NiMaterialColorController::GetCtlrID()
{
    switch(GetType())
    {
        case AMB:
            return "AMB";
        case DIFF:
            return "DIFF";
        case SPEC:
            return "SPEC";
        case SELF_ILLUM:
            return "SELF_ILLUM";
        default:
            return NULL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMaterialColorController);
//---------------------------------------------------------------------------
void NiMaterialColorController::CopyMembers(NiMaterialColorController* pkDest,
    NiCloningProcess& kCloning)
{
    NiPoint3InterpController::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMaterialColorController);
//---------------------------------------------------------------------------
void NiMaterialColorController::LoadBinary(NiStream& kStream)
{
    NiPoint3InterpController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiMaterialColorController::LinkObject(NiStream& kStream)
{
    NiPoint3InterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMaterialColorController::RegisterStreamables(NiStream& kStream)
{
    if (!NiPoint3InterpController::RegisterStreamables(kStream))
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiMaterialColorController::SaveBinary(NiStream& kStream)
{
    NiPoint3InterpController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiMaterialColorController::IsEqual(NiObject* pkObject)
{
    if (!NiPoint3InterpController::IsEqual(pkObject))
        return false;

    NiMaterialColorController* pkCtrl = (NiMaterialColorController*) pkObject;

    if (GetType() != pkCtrl->GetType())
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiMaterialColorController::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPoint3InterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiMaterialColorController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

