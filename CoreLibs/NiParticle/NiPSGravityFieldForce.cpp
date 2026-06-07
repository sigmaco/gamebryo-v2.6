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

#include "NiPSGravityFieldForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSGravityFieldForce, NiPSFieldForce);

//---------------------------------------------------------------------------
NiPSGravityFieldForce::NiPSGravityFieldForce(
    const NiFixedString& kName,
    NiAVObject* pkFieldObj,
    float fMagnitude,
    float fAttenuation,
    bool bUseMaxDistance,
    float fMaxDistance,
    const NiPoint3& kDirection) :
    NiPSFieldForce(
        kName,
        NiPSForceDefinitions::FORCE_GRAVITY_FIELD,
        &m_kInputData,
        pkFieldObj,
        fMagnitude,
        fAttenuation,
        bUseMaxDistance,
        fMaxDistance)
{
    SetDirection(kDirection);
}
//---------------------------------------------------------------------------
NiPSGravityFieldForce::NiPSGravityFieldForce()
{
}
//---------------------------------------------------------------------------
size_t NiPSGravityFieldForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSGravityFieldForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSGravityFieldForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float)
{
    if (!m_pkFieldObj || m_kInputData.m_fMagnitude == 0.0f)
    {
        return false;
    }

    // Get field to particle system transform.
    NiTransform kFieldToPSys;
    ComputeFieldToPSys(pkParticleSystem->GetWorldTransform(), kFieldToPSys);

    if (!m_kInputData.m_bUseMaxDistance && m_kInputData.m_fAttenuation == 0.0f)
    {
        m_kInputData.m_kTransformedDirection = kFieldToPSys.m_Rotate *
            m_kInputData.m_kDirection;
        m_kInputData.m_kTransformedDirection.Unitize();

        // Premultipy the direction by the magnitude because it is 
        // constant for all particles in this case.
        m_kInputData.m_kTransformedDirection *= m_kInputData.m_fMagnitude;
    }
    else
    {
        m_kInputData.m_kTransformedPosition = kFieldToPSys.m_Translate;
        m_kInputData.m_kTransformedDirection = kFieldToPSys.m_Rotate *
            m_kInputData.m_kUnitDirection;

        m_kInputData.m_fHalfAttenuation = m_kInputData.m_fAttenuation * 0.5f;
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSGravityFieldForce);
//---------------------------------------------------------------------------
void NiPSGravityFieldForce::CopyMembers(
    NiPSGravityFieldForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSFieldForce::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyFieldData(pkDest);

    pkDest->SetDirection(GetDirection());
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSGravityFieldForce);
//---------------------------------------------------------------------------
void NiPSGravityFieldForce::LoadBinary(NiStream& kStream)
{
    NiPSFieldForce::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadFieldData(kStream);

    NiPoint3 kValue;

    kValue.LoadBinary(kStream);
    SetDirection(kValue);
}
//---------------------------------------------------------------------------
void NiPSGravityFieldForce::LinkObject(NiStream& kStream)
{
    NiPSFieldForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSGravityFieldForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSFieldForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSGravityFieldForce::SaveBinary(NiStream& kStream)
{
    NiPSFieldForce::SaveBinary(kStream);

    // Save base class data.
    SaveFieldData(kStream);

    GetDirection().SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSGravityFieldForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSFieldForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSGravityFieldForce* pkDest = (NiPSGravityFieldForce*) pkObject;

    if (pkDest->GetDirection() != GetDirection())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSGravityFieldForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSFieldForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSGravityFieldForce::ms_RTTI
        .GetName()));

    pkStrings->Add(GetDirection().GetViewerString("Direction"));
}
//---------------------------------------------------------------------------
