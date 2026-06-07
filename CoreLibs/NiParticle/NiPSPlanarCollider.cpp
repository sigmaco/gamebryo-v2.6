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

#include "NiPSPlanarCollider.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSPlanarCollider, NiPSCollider);

//---------------------------------------------------------------------------
NiPSPlanarCollider::NiPSPlanarCollider(
    float fBounce,
    bool bSpawnOnCollide,
    bool bDieOnCollide,
    NiPSSpawner* pkSpawner,
    NiAVObject* pkColliderObj,
    float fWidth,
    float fHeight,
    const NiPoint3& kXAxis,
    const NiPoint3& kYAxis) :
    NiPSCollider(
        NiPSColliderDefinitions::COLLIDER_PLANAR,
        &m_kInputData,
        fBounce,
        bSpawnOnCollide,
        bDieOnCollide,
        pkSpawner),
    m_pkColliderObj(pkColliderObj)
{
    SetWidth(fWidth);
    SetHeight(fHeight);
    SetXAxis(kXAxis);
    SetYAxis(kYAxis);

    m_kInputData.m_kPlane = NiPlane(NiPoint3::ZERO, 0.0f);
    m_kInputData.m_kColliderPosition = NiPoint3::ZERO;
    m_kInputData.m_kColliderRotation = NiMatrix3::IDENTITY;
    m_kInputData.m_kRotatedXAxis = kXAxis;
    m_kInputData.m_kRotatedYAxis = kYAxis;
    m_kInputData.m_fHalfSqScaledWidth = 0.5f;
    m_kInputData.m_fHalfSqScaledHeight = 0.5f;

    m_kLastColliderTransform.MakeIdentity();
    m_kLastPSysTransform.MakeIdentity();
}
//---------------------------------------------------------------------------
size_t NiPSPlanarCollider::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSPlanarCollider::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSPlanarCollider::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Call base class update function.
    NiPSCollider::Update(pkParticleSystem, fTime);

    // Only compute m_kPlane if it has never been computed or the collider
    // object's world transform has changed.
    if (m_kInputData.m_kPlane == NiPlane(NiPoint3::ZERO, 0.0f) ||
        (m_pkColliderObj &&
           (m_kLastColliderTransform != m_pkColliderObj->GetWorldTransform() ||
            m_kLastPSysTransform != pkParticleSystem->GetWorldTransform())))
    {
        m_kInputData.m_kColliderPosition = NiPoint3::ZERO;
        m_kInputData.m_kColliderRotation = NiMatrix3::IDENTITY;
        NiPoint3 kNormal = m_kInputData.m_kXAxis.Cross(m_kInputData.m_kYAxis);
        kNormal.Unitize();
        float fScaledWidth = m_kInputData.m_fWidth;
        float fScaledHeight = m_kInputData.m_fHeight;

        if (m_pkColliderObj)
        {
            NiTransform kCollider = m_pkColliderObj->GetWorldTransform();
            NiTransform kPSys = pkParticleSystem->GetWorldTransform();
            NiTransform kInvPSys;
            kPSys.Invert(kInvPSys);
            NiTransform kColliderToPSys = kInvPSys * kCollider;

            m_kInputData.m_kColliderPosition = kColliderToPSys.m_Translate;
            m_kInputData.m_kColliderRotation = kColliderToPSys.m_Rotate;
            kNormal = m_kInputData.m_kColliderRotation * kNormal;
            kNormal.Unitize();
            fScaledWidth *= kColliderToPSys.m_fScale;
            fScaledHeight *= kColliderToPSys.m_fScale;

            m_kLastColliderTransform = kCollider;
            m_kLastPSysTransform = kPSys;
        }

        // Find the half width and height because we will be using distance
        // from the plane center not the total width.
        fScaledWidth *= 0.5f;
        fScaledHeight *= 0.5f;

        m_kInputData.m_fHalfSqScaledWidth = fScaledWidth * fScaledWidth;
        m_kInputData.m_fHalfSqScaledHeight = fScaledHeight * fScaledHeight;
        m_kInputData.m_kPlane = NiPlane(kNormal,
            m_kInputData.m_kColliderPosition);

        m_kInputData.m_kRotatedXAxis = m_kInputData.m_kColliderRotation *
            m_kInputData.m_kXAxis;
        m_kInputData.m_kRotatedYAxis = m_kInputData.m_kColliderRotation *
            m_kInputData.m_kYAxis;
    }

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSPlanarCollider);
//---------------------------------------------------------------------------
void NiPSPlanarCollider::CopyMembers(
    NiPSPlanarCollider* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSCollider::CopyMembers(pkDest, kCloning);

    // Copy base class m_pkInputData pointer here, as it cannot be copied from
    // the base class.
    pkDest->m_pkInputData = &pkDest->m_kInputData;

    // Copy base class data.
    CopyColliderData(pkDest);

    pkDest->SetWidth(GetWidth());
    pkDest->SetHeight(GetHeight());
    pkDest->SetXAxis(GetXAxis());
    pkDest->SetYAxis(GetYAxis());
}
//---------------------------------------------------------------------------
void NiPSPlanarCollider::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSCollider::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSPlanarCollider* pkDest = (NiPSPlanarCollider*) pkObject;

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
NiImplementCreateObject(NiPSPlanarCollider);
//---------------------------------------------------------------------------
void NiPSPlanarCollider::LoadBinary(NiStream& kStream)
{
    NiPSCollider::LoadBinary(kStream);

    // Set base class m_pkInputData pointer here, as it cannot be set when
    // streaming the base class.
    m_pkInputData = &m_kInputData;

    // Load base class data.
    LoadColliderData(kStream);

    float fValue;
    NiPoint3 kValue;

    NiStreamLoadBinary(kStream, fValue);
    SetWidth(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetHeight(fValue);

    kValue.LoadBinary(kStream);
    SetXAxis(kValue);

    kValue.LoadBinary(kStream);
    SetYAxis(kValue);

    m_pkColliderObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSPlanarCollider::LinkObject(NiStream& kStream)
{
    NiPSCollider::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSPlanarCollider::RegisterStreamables(NiStream& kStream)
{
    return NiPSCollider::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSPlanarCollider::SaveBinary(NiStream& kStream)
{
    NiPSCollider::SaveBinary(kStream);

    // Save base class data.
    SaveColliderData(kStream);

    NiStreamSaveBinary(kStream, GetWidth());
    NiStreamSaveBinary(kStream, GetHeight());
    GetXAxis().SaveBinary(kStream);
    GetYAxis().SaveBinary(kStream);

    kStream.SaveLinkID(m_pkColliderObj);
}
//---------------------------------------------------------------------------
bool NiPSPlanarCollider::IsEqual(NiObject* pkObject)
{
    if (!NiPSCollider::IsEqual(pkObject))
    {
        return false;
    }

    NiPSPlanarCollider* pkDest = (NiPSPlanarCollider*) pkObject;

    if ((pkDest->m_pkColliderObj && !m_pkColliderObj) ||
        (!pkDest->m_pkColliderObj && m_pkColliderObj))
    {
        return false;
    }

    if (pkDest->GetWidth() != GetWidth() ||
        pkDest->GetHeight() != GetHeight() ||
        pkDest->GetXAxis() != GetXAxis() ||
        pkDest->GetYAxis() != GetYAxis())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSPlanarCollider::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSCollider::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSPlanarCollider::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Collider Object", m_pkColliderObj));
    pkStrings->Add(NiGetViewerString("Width", GetWidth()));
    pkStrings->Add(NiGetViewerString("Height", GetHeight()));
    pkStrings->Add(GetXAxis().GetViewerString("XAxis"));
    pkStrings->Add(GetYAxis().GetViewerString("YAxis"));
}
//---------------------------------------------------------------------------
