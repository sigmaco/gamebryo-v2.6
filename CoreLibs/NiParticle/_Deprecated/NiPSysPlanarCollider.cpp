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
#include <NiParticlePCH.h>

#include "NiPSysPlanarCollider.h"
#include <NiAVObject.h>

NiImplementRTTI(NiPSysPlanarCollider, NiPSysCollider);

//---------------------------------------------------------------------------
NiPSysPlanarCollider::NiPSysPlanarCollider() :
    m_pkColliderObj(NULL),
    m_fWidth(1.0f),
    m_fHeight(1.0f),
    m_kXAxis(NiPoint3::UNIT_X),
    m_kYAxis(NiPoint3::UNIT_Y)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysPlanarCollider);
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::LoadBinary(NiStream& kStream)
{
    NiPSysCollider::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkColliderObj
    NiStreamLoadBinary(kStream, m_fWidth);
    NiStreamLoadBinary(kStream, m_fHeight);
    m_kXAxis.LoadBinary(kStream);
    m_kYAxis.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::LinkObject(NiStream& kStream)
{
    NiPSysCollider::LinkObject(kStream);

    m_pkColliderObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysPlanarCollider::RegisterStreamables(NiStream& kStream)
{
    return NiPSysCollider::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysPlanarCollider::SaveBinary(NiStream& kStream)
{
    NiPSysCollider::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkColliderObj);
    NiStreamSaveBinary(kStream, m_fWidth);
    NiStreamSaveBinary(kStream, m_fHeight);
    m_kXAxis.SaveBinary(kStream);
    m_kYAxis.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysPlanarCollider::IsEqual(NiObject* pkObject)
{
    if (!NiPSysCollider::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysPlanarCollider* pkDest = (NiPSysPlanarCollider*) pkObject;

    if ((m_pkColliderObj && !pkDest->m_pkColliderObj) ||
        (!m_pkColliderObj && pkDest->m_pkColliderObj) ||
        (m_pkColliderObj && pkDest->m_pkColliderObj &&
            !m_pkColliderObj->IsEqual(pkDest->m_pkColliderObj)))
    {
        return false;
    }

    if (m_fWidth != pkDest->m_fWidth ||
        m_fHeight != pkDest->m_fHeight ||
        m_kXAxis != pkDest->m_kXAxis ||
        m_kYAxis != pkDest->m_kYAxis)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
