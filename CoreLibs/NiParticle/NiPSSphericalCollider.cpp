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

#include "NiPSSphericalCollider.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSSphericalCollider, NiPSCollider);

//---------------------------------------------------------------------------
NiPSSphericalCollider::NiPSSphericalCollider(
    float fBounce,
    bool bSpawnOnCollide,
    bool bDieOnCollide,
    NiPSSpawner* pkSpawner,
    NiAVObject* pkColliderObj,
    float fRadius) :
    NiPSCollider(
        NiPSColliderDefinitions::COLLIDER_SPHERICAL,
        &m_kInputData,
        fBounce,
        bSpawnOnCollide,
        bDieOnCollide,
        pkSpawner),
    m_pkColliderObj(pkColliderObj)
{
    SetRadius(fRadius);

    m_kInputData.m_kColliderPosition = NiPoint3::ZERO;
    m_kInputData.m_fScaledRadius = m_kInputData.m_fRadius;
    m_kInputData.m_fRadiusSq = m_kInputData.m_fScaledRadius *
        m_kInputData.m_fScaledRadius;

    // Initialize last transforms to invalid values.
    m_kLastColliderTransform.m_Translate = m_kLastPSysTransform.m_Translate =
        NiPoint3(-NI_INFINITY, -NI_INFINITY, -NI_INFINITY);
    m_kLastColliderTransform.m_Rotate = m_kLastPSysTransform.m_Rotate =
        NiMatrix3::ZERO;
    m_kLastColliderTransform.m_fScale = m_kLastPSysTransform.m_fScale =
        -NI_INFINITY;
}
//---------------------------------------------------------------------------
size_t NiPSSphericalCollider::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSSphericalCollider::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSSphericalCollider::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Call base class update function.
    NiPSCollider::Update(pkParticleSystem, fTime);

    // Only compute variables if the collider object's world transform has
    // changed.
    if (m_pkColliderObj &&
        (m_kLastColliderTransform != m_pkColliderObj->GetWorldTransform() ||
         m_kLastPSysTransform != pkParticleSystem->GetWorldTransform()))
    {
        NiTransform kCollider = m_pkColliderObj->GetWorldTransform();
        NiTransform kPSys = pkParticleSystem->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        NiTransform kColliderToPSys = kInvPSys * kCollider;

        m_kInputData.m_kColliderPosition = kColliderToPSys.m_Translate;
        m_kInputData.m_fScaledRadius = kColliderToPSys.m_fScale *
            m_kInputData.m_fRadius;
        m_kInputData.m_fRadiusSq = m_kInputData.m_fScaledRadius *
            m_kInputData.m_fScaledRadius;

        m_kLastPSysTransform = kPSys;
        m_kLastColliderTransform = kCollider;
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSphericalCollider);
//---------------------------------------------------------------------------
void NiPSSphericalCollider::CopyMembers(
    NiPSSphericalCollider* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSCollider::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyColliderData(pkDest);

    pkDest->SetRadius(GetRadius());

    pkDest->m_kInputData = m_kInputData;
}
//---------------------------------------------------------------------------
void NiPSSphericalCollider::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSCollider::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSSphericalCollider* pkDest = (NiPSSphericalCollider*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkColliderObj, pkObject))
    {
        pkDest->m_pkColliderObj = (NiAVObject*) pkObject;
    }
    else
    {
        pkDest->m_pkColliderObj = m_pkColliderObj;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSphericalCollider);
//---------------------------------------------------------------------------
void NiPSSphericalCollider::LoadBinary(NiStream& kStream)
{
    NiPSCollider::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadColliderData(kStream);

    float fValue;

    NiStreamLoadBinary(kStream, fValue);
    SetRadius(fValue);

    m_pkColliderObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSSphericalCollider::LinkObject(NiStream& kStream)
{
    NiPSCollider::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSphericalCollider::RegisterStreamables(NiStream& kStream)
{
    return NiPSCollider::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSSphericalCollider::SaveBinary(NiStream& kStream)
{
    NiPSCollider::SaveBinary(kStream);

    // Save base class data.
    SaveColliderData(kStream);

    NiStreamSaveBinary(kStream, GetRadius());

    kStream.SaveLinkID(m_pkColliderObj);
}
//---------------------------------------------------------------------------
bool NiPSSphericalCollider::IsEqual(NiObject* pkObject)
{
    if (!NiPSCollider::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSphericalCollider* pkDest = (NiPSSphericalCollider*) pkObject;

    if ((pkDest->m_pkColliderObj && !m_pkColliderObj) ||
        (!pkDest->m_pkColliderObj && m_pkColliderObj))
    {
        return false;
    }

    if (pkDest->GetRadius() != GetRadius())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSphericalCollider::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSCollider::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSphericalCollider::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Collider Object", m_pkColliderObj));
    pkStrings->Add(NiGetViewerString("Radius", GetRadius()));
}
//---------------------------------------------------------------------------
