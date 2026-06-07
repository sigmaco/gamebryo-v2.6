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

#include "NiPSBombForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSBombForce, NiPSForce);

//---------------------------------------------------------------------------
NiPSBombForce::NiPSBombForce(
    const NiFixedString& kName,
    NiAVObject* pkBombObj,
    NiPoint3 kBombAxis,
    float fDecay,
    float fDeltaV,
    NiPSForceDefinitions::BombData::DecayType eDecayType,
    NiPSForceDefinitions::BombData::SymmType eSymmType) :
    NiPSForce(kName, NiPSForceDefinitions::FORCE_BOMB),
    m_pkBombObj(pkBombObj)
{
    SetBombAxis(kBombAxis);
    SetDecay(fDecay);
    SetDeltaV(fDeltaV);
    SetDecayType(eDecayType);
    SetSymmType(eSymmType);
}
//---------------------------------------------------------------------------
NiPSBombForce::NiPSBombForce() : m_pkBombObj(NULL)
{
}
//---------------------------------------------------------------------------
size_t NiPSBombForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSBombForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSBombForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float)
{
    if (!m_pkBombObj)
    {
        return false;
    }

    NiTransform kBomb = m_pkBombObj->GetWorldTransform();
    NiTransform kPSys = pkParticleSystem->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kBombToPSys = kInvPSys * kBomb;

    m_kInputData.m_kPosition = kBombToPSys.m_Translate;
    m_kInputData.m_kDirection = kBombToPSys.m_Rotate *
        m_kInputData.m_kBombAxis;
    m_kInputData.m_kDirection.Unitize();

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSBombForce);
//---------------------------------------------------------------------------
void NiPSBombForce::CopyMembers(
    NiPSBombForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForce::CopyMembers(pkDest, kCloning);

    pkDest->SetBombAxis(GetBombAxis());
    pkDest->SetDecay(GetDecay());
    pkDest->SetDeltaV(GetDeltaV());
    pkDest->SetDecayType(GetDecayType());
    pkDest->SetSymmType(GetSymmType());
}
//---------------------------------------------------------------------------
void NiPSBombForce::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSForce::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSBombForce* pkDest = (NiPSBombForce*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkBombObj, pkObject))
    {
        pkDest->m_pkBombObj = (NiAVObject*) pkObject;
    }
    else
    {
        pkDest->m_pkBombObj = m_pkBombObj;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSBombForce);
//---------------------------------------------------------------------------
void NiPSBombForce::LoadBinary(NiStream& kStream)
{
    NiPSForce::LoadBinary(kStream);

    NiPoint3 kValue;
    float fValue;
    NiPSForceDefinitions::BombData::DecayType eDecayType;
    NiPSForceDefinitions::BombData::SymmType eSymmType;

    kValue.LoadBinary(kStream);
    SetBombAxis(kValue);

    NiStreamLoadBinary(kStream, fValue);
    SetDecay(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetDeltaV(fValue);

    NiStreamLoadEnum(kStream, eDecayType);
    SetDecayType(eDecayType);

    NiStreamLoadEnum(kStream, eSymmType);
    SetSymmType(eSymmType);

    m_pkBombObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSBombForce::LinkObject(NiStream& kStream)
{
    NiPSForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSBombForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSBombForce::SaveBinary(NiStream& kStream)
{
    NiPSForce::SaveBinary(kStream);

    GetBombAxis().SaveBinary(kStream);
    NiStreamSaveBinary(kStream, GetDecay());
    NiStreamSaveBinary(kStream, GetDeltaV());
    NiStreamSaveEnum(kStream, GetDecayType());
    NiStreamSaveEnum(kStream, GetSymmType());

    kStream.SaveLinkID(m_pkBombObj);
}
//---------------------------------------------------------------------------
bool NiPSBombForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSBombForce* pkDest = (NiPSBombForce*) pkObject;

    if ((pkDest->m_pkBombObj && !m_pkBombObj) ||
        (!pkDest->m_pkBombObj && m_pkBombObj))
    {
        return false;
    }

    if (pkDest->GetBombAxis() != GetBombAxis() ||
        pkDest->GetDecay() != GetDecay() ||
        pkDest->GetDeltaV() != GetDeltaV() ||
        pkDest->GetDecayType() != GetDecayType() ||
        pkDest->GetSymmType() != GetSymmType())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSBombForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSBombForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Bomb Object", m_pkBombObj));
    pkStrings->Add(GetBombAxis().GetViewerString("BombAxis"));
    pkStrings->Add(NiGetViewerString("Decay", GetDecay()));
    pkStrings->Add(NiGetViewerString("DeltaV", GetDeltaV()));

    const char* pcDecayType = NULL;
    switch (GetDecayType())
    {
        case NiPSForceDefinitions::BombData::DECAY_NONE:
            pcDecayType = "DECAY_NONE";
            break;
        case NiPSForceDefinitions::BombData::DECAY_LINEAR:
            pcDecayType = "DECAY_LINEAR";
            break;
        case NiPSForceDefinitions::BombData::DECAY_EXPONENTIAL:
            pcDecayType = "DECAY_EXPONENTIAL";
            break;
        default:
            NIASSERT(!"Unknown DecayType!");
            break;
    }
    NIASSERT(pcDecayType);
    pkStrings->Add(NiGetViewerString("DecayType", pcDecayType));

    const char* pcSymmType = NULL;
    switch (GetSymmType())
    {
    case NiPSForceDefinitions::BombData::SYMM_SPHERICAL:
            pcSymmType = "SYMM_SPHERICAL";
            break;
        case NiPSForceDefinitions::BombData::SYMM_CYLINDRICAL:
            pcSymmType = "SYMM_CYLINDRICAL";
            break;
        case NiPSForceDefinitions::BombData::SYMM_PLANAR:
            pcSymmType = "SYMM_PLANAR";
            break;
        default:
            NIASSERT(!"Unknown SymmType!");
            break;
    }
    NIASSERT(pcSymmType);
    pkStrings->Add(NiGetViewerString("SymmType", pcSymmType));
}
//---------------------------------------------------------------------------
