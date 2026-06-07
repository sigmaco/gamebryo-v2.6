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

#include "NiPSCollider.h"
#include "NiPSParticleSystem.h"
#include <NiCloningProcess.h>
#include <NiBool.h>

NiImplementRTTI(NiPSCollider, NiObject);

//---------------------------------------------------------------------------
NiPSCollider::NiPSCollider(
    NiPSColliderDefinitions::ColliderType eType,
    NiPSColliderDefinitions::ColliderData* pkInputData,
    float fBounce,
    bool bSpawnOnCollide,
    bool bDieOnCollide,
    NiPSSpawner* pkSpawner) :
    m_pkInputData(pkInputData),
    m_pkSpawner(pkSpawner),
    m_eType(eType),
    m_bActive(true)
{
    NIASSERT(m_pkInputData);

    SetBounce(fBounce);
    SetSpawnOnCollide(bSpawnOnCollide);
    SetDieOnCollide(bDieOnCollide);
}
//---------------------------------------------------------------------------
NiPSCollider::NiPSCollider() :
    m_pkInputData(NULL),
    m_pkSpawner(NULL),
    m_eType(NiPSColliderDefinitions::COLLIDER_PLANAR),
    m_bActive(true)
{
}
//---------------------------------------------------------------------------
bool NiPSCollider::Update(NiPSParticleSystem* pkParticleSystem, float)
{
    NIASSERT(m_pkInputData);

    if (m_pkSpawner)
    {
        m_pkInputData->m_ucSpawnerID =
            pkParticleSystem->GetSpawnerID(m_pkSpawner);
    }
    else
    {
        m_pkInputData->m_ucSpawnerID = NiPSSpawner::INVALID_ID;
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSCollider::CopyColliderData(NiPSCollider* pkDest)
{
    pkDest->SetBounce(GetBounce());
    pkDest->SetSpawnOnCollide(GetSpawnOnCollide());
    pkDest->SetDieOnCollide(GetDieOnCollide());
}
//---------------------------------------------------------------------------
void NiPSCollider::CopyMembers(
    NiPSCollider* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // m_pkInputData must be set directly by derived class CopyMembers
    // functions. This is because the pointer is passed into the main
    // protected constructor and cannot be known when cloning this object.

    pkDest->m_eType = m_eType;
    pkDest->m_bActive = m_bActive;
}
//---------------------------------------------------------------------------
void NiPSCollider::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSCollider* pkDest = (NiPSCollider*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkSpawner, pkObject))
    {
        pkDest->m_pkSpawner = (NiPSSpawner*) pkObject;
    }
    else
    {
        pkDest->m_pkSpawner = m_pkSpawner;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSCollider::LoadColliderData(NiStream& kStream)
{
    float fValue;
    NiBool bValue;

    NiStreamLoadBinary(kStream, fValue);
    SetBounce(fValue);

    NiStreamLoadBinary(kStream, bValue);
    SetSpawnOnCollide(NIBOOL_IS_TRUE(bValue));

    NiStreamLoadBinary(kStream, bValue);
    SetDieOnCollide(NIBOOL_IS_TRUE(bValue));
}
//---------------------------------------------------------------------------
void NiPSCollider::SaveColliderData(NiStream& kStream)
{
    NiStreamSaveBinary(kStream, GetBounce());
    NiStreamSaveBinary(kStream, NiBool(GetSpawnOnCollide()));
    NiStreamSaveBinary(kStream, NiBool(GetDieOnCollide()));
}
//---------------------------------------------------------------------------
void NiPSCollider::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    // m_pkInputData must be set directly by derived class LoadBinary
    // functions. This is because the pointer is passed into the main
    // protected constructor and cannot be known when streaming this object.
    // The data pointed to is also loaded by the derived class.

    m_pkSpawner = (NiPSSpawner*) kStream.ResolveLinkID();
    NiStreamLoadEnum(kStream, m_eType);
    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    m_bActive = NIBOOL_IS_TRUE(bValue);
}
//---------------------------------------------------------------------------
void NiPSCollider::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSCollider::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSCollider::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    // m_pkInputData is not saved. This pointer points to a member variable
    // in a derived class and as such cannot be streamed. The derived class
    // will stream this data.

    kStream.SaveLinkID(m_pkSpawner);
    NiStreamSaveEnum(kStream, m_eType);
    NiStreamSaveBinary(kStream, NiBool(m_bActive));
}
//---------------------------------------------------------------------------
bool NiPSCollider::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSCollider* pkDest = (NiPSCollider*) pkObject;

    if (pkDest->GetBounce() != GetBounce() ||
        pkDest->GetSpawnOnCollide() != GetSpawnOnCollide() ||
        pkDest->GetDieOnCollide() != GetDieOnCollide())
    {
        return false;
    }

    if ((pkDest->m_pkSpawner && !m_pkSpawner) ||
        (!pkDest->m_pkSpawner && m_pkSpawner))
    {
        return false;
    }

    if (pkDest->m_eType != m_eType ||
        pkDest->m_bActive != m_bActive)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSCollider::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSCollider::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Bounce", GetBounce()));
    pkStrings->Add(NiGetViewerString("SpawnOnCollide", GetSpawnOnCollide()));
    pkStrings->Add(NiGetViewerString("DieOnCollide", GetDieOnCollide()));
    pkStrings->Add(NiGetViewerString("Spawner", m_pkSpawner));

    const char* pcColliderType = NULL;
    switch (m_eType)
    {
        case NiPSColliderDefinitions::COLLIDER_PLANAR:
            pcColliderType = "COLLIDER_PLANAR";
            break;
        case NiPSColliderDefinitions::COLLIDER_SPHERICAL:
            pcColliderType = "COLLIDER_SPHERICAL";
            break;
        default:
            NIASSERT(!"Unknown ColliderType!");
            break;
    }
    NIASSERT(pcColliderType);
    pkStrings->Add(NiGetViewerString("Type", pcColliderType));
    pkStrings->Add(NiGetViewerString("Active", m_bActive));
}
//---------------------------------------------------------------------------
