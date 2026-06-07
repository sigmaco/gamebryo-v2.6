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

#include "NiPSAirFieldForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSAirFieldForce, NiPSFieldForce);

const NiPoint3 NiPSAirFieldForce::ms_kUninitializedPosition(
    -NI_INFINITY,
    -NI_INFINITY,
    -NI_INFINITY);

//---------------------------------------------------------------------------
NiPSAirFieldForce::NiPSAirFieldForce(
    const NiFixedString& kName,
    NiAVObject* pkFieldObj,
    float fMagnitude,
    float fAttenuation,
    bool bUseMaxDistance,
    float fMaxDistance,
    const NiPoint3& kDirection,
    float fAirFriction,
    float fInheritedVelocity,
    bool bInheritRotation,
    bool bEnableSpread,
    float fSpread) :
    NiPSFieldForce(
        kName,
        NiPSForceDefinitions::FORCE_AIR_FIELD,
        &m_kInputData,
        pkFieldObj,
        fMagnitude,
        fAttenuation,
        bUseMaxDistance,
        fMaxDistance),
    m_kLastFieldPosition(ms_kUninitializedPosition),
    m_fLastUpdateTime(-NI_INFINITY)
{
    SetDirection(kDirection);
    SetAirFriction(fAirFriction);
    SetInheritedVelocity(fInheritedVelocity);
    SetInheritRotation(bInheritRotation);
    SetEnableSpread(bEnableSpread);
    SetSpread(fSpread);
}
//---------------------------------------------------------------------------
NiPSAirFieldForce::NiPSAirFieldForce() :
    m_kLastFieldPosition(ms_kUninitializedPosition),
    m_fLastUpdateTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
size_t NiPSAirFieldForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSAirFieldForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSAirFieldForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    if (!m_pkFieldObj)
    {
        return false;
    }

    // Get field to particle system transform.
    NiTransform kFieldToPSys;
    ComputeFieldToPSys(pkParticleSystem->GetWorldTransform(), kFieldToPSys);

    // Compute transformed position.
    m_kInputData.m_kTransformedPosition = kFieldToPSys.m_Translate;

    // Compute the air direction.
    NiPoint3 kAirDirection;
    if (m_kInputData.m_bInheritRotation)
    {
        kAirDirection = kFieldToPSys.m_Rotate * m_kInputData.m_kUnitDirection;
    }
    else
    {
        kAirDirection = m_kInputData.m_kUnitDirection;
    }

    // Compute the air field velocity.
    if (m_kInputData.m_fAttenuation == 0.0f &&
        m_kInputData.m_fInheritedVelocity == 0.0f &&
        !m_kInputData.m_bEnableSpread)
    {
        m_kInputData.m_kAirFieldVelocity = kAirDirection *
            m_kInputData.m_fMagnitude;
    }
    else
    {
        m_kInputData.m_kAirFieldVelocity = NiPoint3::ZERO;
        if (m_kLastFieldPosition != ms_kUninitializedPosition &&
            m_fLastUpdateTime != -NI_INFINITY &&
            m_fLastUpdateTime != fTime)
        {
            m_kInputData.m_kAirFieldVelocity =
                (m_kInputData.m_kTransformedPosition - m_kLastFieldPosition) *
                m_kInputData.m_fInheritedVelocity /
                (fTime - m_fLastUpdateTime);
        }

        // Apply the magnitude to the air velocity.
        m_kInputData.m_kAirFieldVelocity += kAirDirection *
            m_kInputData.m_fMagnitude;

        // Compute unit air field velocity.
        m_kInputData.m_kUnitAirFieldVelocity =
            m_kInputData.m_kAirFieldVelocity;
        m_kInputData.m_fAirFieldVelocityLength =
            m_kInputData.m_kUnitAirFieldVelocity.Unitize();

        // Calculate the CosSpread.
        m_kInputData.m_fCosSpread = m_kInputData.m_bEnableSpread ?
            NiCos(NI_HALF_PI * m_kInputData.m_fSpread) : 0.0f;
    }

    // Update the last field position and time.
    m_kLastFieldPosition = m_kInputData.m_kTransformedPosition;
    m_fLastUpdateTime = fTime;

    // Check for no air.
    if (m_kInputData.m_kAirFieldVelocity.SqrLength() == 0.0f)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSAirFieldForce);
//---------------------------------------------------------------------------
void NiPSAirFieldForce::CopyMembers(
    NiPSAirFieldForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSFieldForce::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyFieldData(pkDest);

    pkDest->SetDirection(GetDirection());
    pkDest->SetAirFriction(GetAirFriction());
    pkDest->SetInheritedVelocity(GetInheritedVelocity());
    pkDest->SetInheritRotation(GetInheritRotation());
    pkDest->SetEnableSpread(GetEnableSpread());
    pkDest->SetSpread(GetSpread());
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSAirFieldForce);
//---------------------------------------------------------------------------
void NiPSAirFieldForce::LoadBinary(NiStream& kStream)
{
    NiPSFieldForce::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadFieldData(kStream);

    NiPoint3 kValue;
    float fValue;
    NiBool bValue;

    kValue.LoadBinary(kStream);
    SetDirection(kValue);

    NiStreamLoadBinary(kStream, fValue);
    SetAirFriction(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetInheritedVelocity(fValue);

    NiStreamLoadBinary(kStream, bValue);
    SetInheritRotation(NIBOOL_IS_TRUE(bValue));

    NiStreamLoadBinary(kStream, bValue);
    SetEnableSpread(NIBOOL_IS_TRUE(bValue));

    NiStreamLoadBinary(kStream, fValue);
    SetSpread(fValue);
}
//---------------------------------------------------------------------------
void NiPSAirFieldForce::LinkObject(NiStream& kStream)
{
    NiPSFieldForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSAirFieldForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSFieldForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSAirFieldForce::SaveBinary(NiStream& kStream)
{
    NiPSFieldForce::SaveBinary(kStream);

    // Save base class data.
    SaveFieldData(kStream);

    GetDirection().SaveBinary(kStream);
    NiStreamSaveBinary(kStream, GetAirFriction());
    NiStreamSaveBinary(kStream, GetInheritedVelocity());
    NiStreamSaveBinary(kStream, NiBool(GetInheritRotation()));
    NiStreamSaveBinary(kStream, NiBool(GetEnableSpread()));
    NiStreamSaveBinary(kStream, GetSpread());
}
//---------------------------------------------------------------------------
bool NiPSAirFieldForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSFieldForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSAirFieldForce* pkDest = (NiPSAirFieldForce*) pkObject;

    if (pkDest->GetDirection() != GetDirection() ||
        pkDest->GetAirFriction() != GetAirFriction() ||
        pkDest->GetInheritedVelocity() != GetInheritedVelocity() ||
        pkDest->GetInheritRotation() != GetInheritRotation() ||
        pkDest->GetEnableSpread() != GetEnableSpread() ||
        pkDest->GetSpread() != GetSpread())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSAirFieldForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSFieldForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSAirFieldForce::ms_RTTI.GetName()));

    pkStrings->Add(GetDirection().GetViewerString("Direction"));
    pkStrings->Add(NiGetViewerString("AirFriction", GetAirFriction()));
    pkStrings->Add(NiGetViewerString("InheritedVelocity",
        GetInheritedVelocity()));
    pkStrings->Add(NiGetViewerString("InheritRotation", GetInheritRotation()));
    pkStrings->Add(NiGetViewerString("EnableSpread", GetEnableSpread()));
    pkStrings->Add(NiGetViewerString("Spread", GetSpread()));
}
//---------------------------------------------------------------------------
