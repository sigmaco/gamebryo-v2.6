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

#include "NiPSysSphericalCollider.h"
#include <NiAVObject.h>

NiImplementRTTI(NiPSysSphericalCollider, NiPSysCollider);

//---------------------------------------------------------------------------
NiPSysSphericalCollider::NiPSysSphericalCollider() :
    m_pkColliderObj(NULL),
    m_fRadius(1.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysSphericalCollider);
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::LoadBinary(NiStream& kStream)
{
    NiPSysCollider::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkColliderObj
    NiStreamLoadBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::LinkObject(NiStream& kStream)
{
    NiPSysCollider::LinkObject(kStream);

    m_pkColliderObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysSphericalCollider::RegisterStreamables(NiStream& kStream)
{
    return NiPSysCollider::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysSphericalCollider::SaveBinary(NiStream& kStream)
{
    NiPSysCollider::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkColliderObj);
    NiStreamSaveBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
bool NiPSysSphericalCollider::IsEqual(NiObject* pkObject)
{
    if (!NiPSysCollider::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysSphericalCollider* pkDest = (NiPSysSphericalCollider*) pkObject;

    if ((m_pkColliderObj && !pkDest->m_pkColliderObj) ||
        (!m_pkColliderObj && pkDest->m_pkColliderObj) ||
        (m_pkColliderObj && pkDest->m_pkColliderObj &&
            !m_pkColliderObj->IsEqual(pkDest->m_pkColliderObj)))
    {
        return false;
    }

    if (m_fRadius != pkDest->m_fRadius)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
