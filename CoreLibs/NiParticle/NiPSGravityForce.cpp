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
#include "NiParticlePCH.h"

#include "NiPSGravityForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSGravityForce, NiPSForce);

//---------------------------------------------------------------------------
NiPSGravityForce::NiPSGravityForce(
    const NiFixedString& kName,
    NiAVObject* pkGravityObj,
    const NiPoint3& kGravityAxis,
    float fDecay,
    float fStrength,
    NiPSForceDefinitions::GravityData::ForceType eForceType,
    float fTurbulence,
    float fTurbulenceScale) :
    NiPSForce(kName, NiPSForceDefinitions::FORCE_GRAVITY),
    m_pkGravityObj(pkGravityObj)
{
    SetGravityAxis(kGravityAxis);
    SetDecay(fDecay);
    SetStrength(fStrength);
    SetForceType(eForceType);
    SetTurbulence(fTurbulence);
    SetTurbulenceScale(fTurbulenceScale);
}
//---------------------------------------------------------------------------
NiPSGravityForce::NiPSGravityForce() : m_pkGravityObj(NULL)
{
}
//---------------------------------------------------------------------------
size_t NiPSGravityForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSGravityForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSGravityForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float)
{
    if (!m_pkGravityObj)
    {
        return false;
    }

    NiTransform kGravity = m_pkGravityObj->GetWorldTransform();
    NiTransform kPSys = pkParticleSystem->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kGravityToPSys = kInvPSys * kGravity;

    m_kInputData.m_kPosition = kGravityToPSys.m_Translate;
    m_kInputData.m_kDirection = kGravityToPSys.m_Rotate *
        m_kInputData.m_kGravityAxis;
    m_kInputData.m_kDirection.Unitize();

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSGravityForce);
//---------------------------------------------------------------------------
void NiPSGravityForce::CopyMembers(
    NiPSGravityForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForce::CopyMembers(pkDest, kCloning);

    pkDest->SetGravityAxis(GetGravityAxis());
    pkDest->SetDecay(GetDecay());
    pkDest->SetStrength(GetStrength());
    pkDest->SetForceType(GetForceType());
    pkDest->SetTurbulence(GetTurbulence());
    pkDest->SetTurbulenceScale(GetTurbulenceScale());
}
//---------------------------------------------------------------------------
void NiPSGravityForce::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSForce::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSGravityForce* pkDest = (NiPSGravityForce*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkGravityObj, pkObject))
    {
        pkDest->m_pkGravityObj = (NiAVObject*) pkObject;
    }
    else
    {
        pkDest->m_pkGravityObj = m_pkGravityObj;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSGravityForce);
//---------------------------------------------------------------------------
void NiPSGravityForce::LoadBinary(NiStream& kStream)
{
    NiPSForce::LoadBinary(kStream);

    NiPoint3 kValue;
    float fValue;
    NiPSForceDefinitions::GravityData::ForceType eForceType;

    kValue.LoadBinary(kStream);
    SetGravityAxis(kValue);

    NiStreamLoadBinary(kStream, fValue);
    SetDecay(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetStrength(fValue);

    NiStreamLoadEnum(kStream, eForceType);
    SetForceType(eForceType);

    NiStreamLoadBinary(kStream, fValue);
    SetTurbulence(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetTurbulenceScale(fValue);
    
    m_pkGravityObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSGravityForce::LinkObject(NiStream& kStream)
{
    NiPSForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSGravityForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSGravityForce::SaveBinary(NiStream& kStream)
{
    NiPSForce::SaveBinary(kStream);
    
    GetGravityAxis().SaveBinary(kStream);
    NiStreamSaveBinary(kStream, GetDecay());
    NiStreamSaveBinary(kStream, GetStrength());
    NiStreamSaveEnum(kStream, GetForceType());
    NiStreamSaveBinary(kStream, GetTurbulence());
    NiStreamSaveBinary(kStream, GetTurbulenceScale());

    kStream.SaveLinkID(m_pkGravityObj);
}
//---------------------------------------------------------------------------
bool NiPSGravityForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSGravityForce* pkDest = (NiPSGravityForce*) pkObject;

    if (pkDest->GetGravityAxis() != GetGravityAxis() ||
        pkDest->GetDecay() != GetDecay() ||
        pkDest->GetStrength() != GetStrength() ||
        pkDest->GetForceType() != GetForceType() ||
        pkDest->GetTurbulence() != GetTurbulence() ||
        pkDest->GetTurbulenceScale() != GetTurbulenceScale())
    {
        return false;
    }

    if ((pkDest->m_pkGravityObj && !m_pkGravityObj) ||
        (!pkDest->m_pkGravityObj && m_pkGravityObj))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSGravityForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSGravityForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Gravity Object", m_pkGravityObj));
    pkStrings->Add(GetGravityAxis().GetViewerString("GravityAxis"));
    pkStrings->Add(NiGetViewerString("Decay", GetDecay()));
    pkStrings->Add(NiGetViewerString("Strength", GetStrength()));

    const char* pcForceType = NULL;
    switch (GetForceType())
    {
        case NiPSForceDefinitions::GravityData::FORCE_PLANAR:
            pcForceType = "FORCE_PLANAR";
            break;
        case NiPSForceDefinitions::GravityData::FORCE_SPHERICAL:
            pcForceType = "FORCE_SPHERICAL";
            break;
        default:
            NIASSERT(!"Unknown ForceType!");
            break;
    }
    NIASSERT(pcForceType);
    pkStrings->Add(NiGetViewerString("ForceType", pcForceType));

    pkStrings->Add(NiGetViewerString("Turbulence", GetTurbulence()));
    pkStrings->Add(NiGetViewerString("TurbulenceScale", GetTurbulenceScale()));
}
//---------------------------------------------------------------------------
