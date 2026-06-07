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

#include "NiPSFieldForce.h"
#include <NiCloningProcess.h>
#include <NiBool.h>

NiImplementRTTI(NiPSFieldForce, NiPSForce);

//---------------------------------------------------------------------------
NiPSFieldForce::NiPSFieldForce(
    const NiFixedString& kName,
    NiPSForceDefinitions::ForceType eType,
    NiPSForceDefinitions::FieldData* pkInputData,
    NiAVObject* pkFieldObj,
    float fMagnitude,
    float fAttenuation,
    bool bUseMaxDistance,
    float fMaxDistance) :
    NiPSForce(kName, eType),
    m_pkInputData(pkInputData),
    m_pkFieldObj(pkFieldObj)
{
    NIASSERT(m_pkInputData);

    SetMagnitude(fMagnitude);
    SetAttenuation(fAttenuation);
    SetUseMaxDistance(bUseMaxDistance);
    SetMaxDistance(fMaxDistance);
}
//---------------------------------------------------------------------------
NiPSFieldForce::NiPSFieldForce() :
    m_pkInputData(NULL),
    m_pkFieldObj(NULL)
{
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSFieldForce::CopyFieldData(NiPSFieldForce* pkDest)
{
    pkDest->SetMagnitude(GetMagnitude());
    pkDest->SetAttenuation(GetAttenuation());
    pkDest->SetUseMaxDistance(GetUseMaxDistance());
    pkDest->SetMaxDistance(GetMaxDistance());
}
//---------------------------------------------------------------------------
void NiPSFieldForce::CopyMembers(
    NiPSFieldForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForce::CopyMembers(pkDest, kCloning);

    // m_pkInputData must be set directly by derived class CopyMembers
    // functions. This is because the pointer is passed into the main
    // protected constructor and cannot be known when cloning this object.
}
//---------------------------------------------------------------------------
void NiPSFieldForce::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSForce::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSFieldForce* pkDest = (NiPSFieldForce*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkFieldObj, pkObject))
    {
        pkDest->m_pkFieldObj = (NiAVObject*) pkObject;
    }
    else
    {
        pkDest->m_pkFieldObj = m_pkFieldObj;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSFieldForce::LoadFieldData(NiStream& kStream)
{
    float fValue;
    NiBool bValue;

    NiStreamLoadBinary(kStream, fValue);
    SetMagnitude(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetAttenuation(fValue);

    NiStreamLoadBinary(kStream, bValue);
    SetUseMaxDistance(NIBOOL_IS_TRUE(bValue));

    NiStreamLoadBinary(kStream, fValue);
    SetMaxDistance(fValue);
}
//---------------------------------------------------------------------------
void NiPSFieldForce::SaveFieldData(NiStream& kStream)
{
    NiStreamSaveBinary(kStream, GetMagnitude());
    NiStreamSaveBinary(kStream, GetAttenuation());
    NiStreamSaveBinary(kStream, NiBool(GetUseMaxDistance()));
    NiStreamSaveBinary(kStream, GetMaxDistance());
}
//---------------------------------------------------------------------------
void NiPSFieldForce::LoadBinary(NiStream& kStream)
{
    NiPSForce::LoadBinary(kStream);

    // m_pkInputData must be set directly by derived class LoadBinary
    // functions. This is because the pointer is passed into the main
    // protected constructor and cannot be known when streaming this object.
    // The data pointed to is also loaded by the derived class.

    m_pkFieldObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSFieldForce::LinkObject(NiStream& kStream)
{
    NiPSForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFieldForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSFieldForce::SaveBinary(NiStream& kStream)
{
    NiPSForce::SaveBinary(kStream);

    // m_pkInputData is not saved. This pointer points to a member variable
    // in a derived class and as such cannot be streamed. The derived class
    // will stream this data.

    kStream.SaveLinkID(m_pkFieldObj);
}
//---------------------------------------------------------------------------
bool NiPSFieldForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSFieldForce* pkDest = (NiPSFieldForce*) pkObject;

    if ((pkDest->m_pkFieldObj && !m_pkFieldObj) ||
        (!pkDest->m_pkFieldObj && m_pkFieldObj))
    {
        return false;
    }

    if (pkDest->GetMagnitude() != GetMagnitude() ||
        pkDest->GetAttenuation() != GetAttenuation() ||
        pkDest->GetUseMaxDistance() != GetUseMaxDistance() ||
        pkDest->GetMaxDistance() != GetMaxDistance())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSFieldForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSFieldForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Field Object", m_pkFieldObj));
    pkStrings->Add(NiGetViewerString("Magnitude", GetMagnitude()));
    pkStrings->Add(NiGetViewerString("Attenuation", GetAttenuation()));
    pkStrings->Add(NiGetViewerString("UseMaxDistance", GetUseMaxDistance()));
    pkStrings->Add(NiGetViewerString("MaxDistance", GetMaxDistance()));
}
//---------------------------------------------------------------------------
