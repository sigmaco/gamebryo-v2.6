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

#include "NiPSDragFieldForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSDragFieldForce, NiPSFieldForce);

//---------------------------------------------------------------------------
NiPSDragFieldForce::NiPSDragFieldForce(
    const NiFixedString& kName,
    NiAVObject* pkFieldObj,
    float fMagnitude,
    float fAttenuation,
    bool bUseMaxDistance,
    float fMaxDistance,
    bool bUseDirection,
    const NiPoint3& kDirection) :
    NiPSFieldForce(
        kName,
        NiPSForceDefinitions::FORCE_DRAG_FIELD,
        &m_kInputData,
        pkFieldObj,
        fMagnitude,
        fAttenuation,
        bUseMaxDistance,
        fMaxDistance)
{
    SetUseDirection(bUseDirection);
    SetDirection(kDirection);
}
//---------------------------------------------------------------------------
NiPSDragFieldForce::NiPSDragFieldForce()
{
}
//---------------------------------------------------------------------------
size_t NiPSDragFieldForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSDragFieldForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSDragFieldForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float)
{
    if (!m_pkFieldObj || m_kInputData.m_fMagnitude == 0.0f)
    {
        return false;
    }

    if (!m_kInputData.m_bUseDirection &&
        m_kInputData.m_fAttenuation == 0.0f &&
        !m_kInputData.m_bUseMaxDistance)
    {
        // This case does not require use of m_pkDragObj. Just return true.
        return true;
    }

    // Get field to particle system transform.
    NiTransform kFieldToPSys;
    ComputeFieldToPSys(pkParticleSystem->GetWorldTransform(), kFieldToPSys);

    m_kInputData.m_kPosition = kFieldToPSys.m_Translate;
    if (m_kInputData.m_bUseDirection &&
        m_kInputData.m_kDirection != NiPoint3::ZERO)
    {
        m_kInputData.m_kDragAxis = kFieldToPSys.m_Rotate *
            m_kInputData.m_kDirection;
        m_kInputData.m_kDragAxis.Unitize();
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSDragFieldForce);
//---------------------------------------------------------------------------
void NiPSDragFieldForce::CopyMembers(
    NiPSDragFieldForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSFieldForce::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyFieldData(pkDest);

    pkDest->SetUseDirection(GetUseDirection());
    pkDest->SetDirection(GetDirection());
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSDragFieldForce);
//---------------------------------------------------------------------------
void NiPSDragFieldForce::LoadBinary(NiStream& kStream)
{
    NiPSFieldForce::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadFieldData(kStream);

    NiBool bValue;
    NiPoint3 kValue;

    NiStreamLoadBinary(kStream, bValue);
    SetUseDirection(NIBOOL_IS_TRUE(bValue));

    kValue.LoadBinary(kStream);
    SetDirection(kValue);
}
//---------------------------------------------------------------------------
void NiPSDragFieldForce::LinkObject(NiStream& kStream)
{
    NiPSFieldForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSDragFieldForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSFieldForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSDragFieldForce::SaveBinary(NiStream& kStream)
{
    NiPSFieldForce::SaveBinary(kStream);

    // Save base class data.
    SaveFieldData(kStream);

    NiStreamSaveBinary(kStream, NiBool(GetUseDirection()));
    GetDirection().SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSDragFieldForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSFieldForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSDragFieldForce* pkDest = (NiPSDragFieldForce*) pkObject;

    if (pkDest->GetUseDirection() != GetUseDirection() ||
        pkDest->GetDirection() != GetDirection())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSDragFieldForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSFieldForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSDragFieldForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("UseDirection", GetUseDirection()));
    pkStrings->Add(GetDirection().GetViewerString("Direction"));
}
//---------------------------------------------------------------------------
