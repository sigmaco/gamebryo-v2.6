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

#include "NiPSTurbulenceFieldForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSTurbulenceFieldForce, NiPSFieldForce);

//---------------------------------------------------------------------------
NiPSTurbulenceFieldForce::NiPSTurbulenceFieldForce(
    const NiFixedString& kName,
    NiAVObject* pkFieldObj,
    float fMagnitude,
    float fAttenuation,
    bool bUseMaxDistance,
    float fMaxDistance,
    float fFrequency) :
    NiPSFieldForce(
        kName,
        NiPSForceDefinitions::FORCE_TURBULENCE_FIELD,
        &m_kInputData,
        pkFieldObj,
        fMagnitude,
        fAttenuation,
        bUseMaxDistance,
        fMaxDistance),
    m_fFrequencyUpdateTime(-NI_INFINITY),
    m_fLastUpdateTime(-NI_INFINITY)
{
    SetFrequency(fFrequency);
}
//---------------------------------------------------------------------------
NiPSTurbulenceFieldForce::NiPSTurbulenceFieldForce() :
    m_fFrequencyUpdateTime(-NI_INFINITY),
    m_fLastUpdateTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
size_t NiPSTurbulenceFieldForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSTurbulenceFieldForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSTurbulenceFieldForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    if (fTime >= m_fLastUpdateTime &&
        fTime < (m_fLastUpdateTime + m_fFrequencyUpdateTime))
    {
        return false;
    }

    // Update the last time.
    m_fLastUpdateTime = fTime;

    if (!m_pkFieldObj || m_kInputData.m_fMagnitude == 0.0f)
    {
        return false;
    }

    if (m_kInputData.m_fAttenuation == 0.0f &&
        !m_kInputData.m_bUseMaxDistance)
    {
        // This case does not need the field to particle system transform,
        // so don't compute it.
        return true;
    }

    // Get field to particle system transform.
    NiTransform kFieldToPSys;
    ComputeFieldToPSys(pkParticleSystem->GetWorldTransform(), kFieldToPSys);

    // Set field position.
    m_kInputData.m_kTransformedPosition = kFieldToPSys.m_Translate;

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSTurbulenceFieldForce);
//---------------------------------------------------------------------------
void NiPSTurbulenceFieldForce::CopyMembers(
    NiPSTurbulenceFieldForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSFieldForce::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyFieldData(pkDest);

    pkDest->SetFrequency(GetFrequency());
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSTurbulenceFieldForce);
//---------------------------------------------------------------------------
void NiPSTurbulenceFieldForce::LoadBinary(NiStream& kStream)
{
    NiPSFieldForce::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadFieldData(kStream);

    float fValue;

    NiStreamLoadBinary(kStream, fValue);
    SetFrequency(fValue);
}
//---------------------------------------------------------------------------
void NiPSTurbulenceFieldForce::LinkObject(NiStream& kStream)
{
    NiPSFieldForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSTurbulenceFieldForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSFieldForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSTurbulenceFieldForce::SaveBinary(NiStream& kStream)
{
    NiPSFieldForce::SaveBinary(kStream);

    // Save base class data.
    SaveFieldData(kStream);

    NiStreamSaveBinary(kStream, GetFrequency());
}
//---------------------------------------------------------------------------
bool NiPSTurbulenceFieldForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSFieldForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSTurbulenceFieldForce* pkDest = (NiPSTurbulenceFieldForce*) pkObject;

    if (pkDest->GetFrequency() != GetFrequency())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSTurbulenceFieldForce::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSFieldForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSTurbulenceFieldForce::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Frequency", GetFrequency()));
}
//---------------------------------------------------------------------------
