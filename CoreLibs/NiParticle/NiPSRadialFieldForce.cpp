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

#include "NiPSRadialFieldForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSRadialFieldForce, NiPSFieldForce);

//---------------------------------------------------------------------------
NiPSRadialFieldForce::NiPSRadialFieldForce(
    const NiFixedString& kName,
    NiAVObject* pkFieldObj,
    float fMagnitude,
    float fAttenuation,
    bool bUseMaxDistance,
    float fMaxDistance,
    float fRadialType) :
    NiPSFieldForce(
        kName,
        NiPSForceDefinitions::FORCE_RADIAL_FIELD,
        &m_kInputData,
        pkFieldObj,
        fMagnitude,
        fAttenuation,
        bUseMaxDistance,
        fMaxDistance)
{
    SetRadialType(fRadialType);
}
//---------------------------------------------------------------------------
NiPSRadialFieldForce::NiPSRadialFieldForce()
{
}
//---------------------------------------------------------------------------
size_t NiPSRadialFieldForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSRadialFieldForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSRadialFieldForce::Update(
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

    m_kInputData.m_kTransformedPosition = kFieldToPSys.m_Translate;

    m_kInputData.m_fHalfAttenuation = m_kInputData.m_fAttenuation * 0.5f;

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSRadialFieldForce);
//---------------------------------------------------------------------------
void NiPSRadialFieldForce::CopyMembers(
    NiPSRadialFieldForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSFieldForce::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyFieldData(pkDest);

    pkDest->SetRadialType(GetRadialType());
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSRadialFieldForce);
//---------------------------------------------------------------------------
void NiPSRadialFieldForce::LoadBinary(NiStream& kStream)
{
    NiPSFieldForce::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadFieldData(kStream);

    float fValue;

    NiStreamLoadBinary(kStream, fValue);
    SetRadialType(fValue);
}
//---------------------------------------------------------------------------
void NiPSRadialFieldForce::LinkObject(NiStream& kStream)
{
    NiPSFieldForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSRadialFieldForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSFieldForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSRadialFieldForce::SaveBinary(NiStream& kStream)
{
    NiPSFieldForce::SaveBinary(kStream);

    // Save base class data.
    SaveFieldData(kStream);

    NiStreamSaveBinary(kStream, GetRadialType());
}
//---------------------------------------------------------------------------
bool NiPSRadialFieldForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSFieldForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSRadialFieldForce* pkDest = (NiPSRadialFieldForce*) pkObject;

    if (pkDest->GetRadialType() != GetRadialType())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSRadialFieldForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSFieldForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSRadialFieldForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("RadialType", GetRadialType()));
}
//---------------------------------------------------------------------------
