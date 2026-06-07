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

#include "NiPSysCollider.h"
#include "NiPSysSpawnModifier.h"
#include "NiPSysColliderManager.h"
#include <NiStream.h>
#include <NiBool.h>

NiImplementRTTI(NiPSysCollider, NiObject);

//---------------------------------------------------------------------------
NiPSysCollider::NiPSysCollider() :
    m_fBounce(1.0f),
    m_bSpawnOnCollide(false),
    m_bDieOnCollide(false),
    m_pkSpawnModifier(NULL),
    m_pkManager(NULL)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysCollider::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fBounce);
    NiBool bSpawnOnCollide;
    NiStreamLoadBinary(kStream, bSpawnOnCollide);
    m_bSpawnOnCollide = (bSpawnOnCollide == 0) ? false : true;
    NiBool bDieOnCollide;
    NiStreamLoadBinary(kStream, bDieOnCollide);
    m_bDieOnCollide = (bDieOnCollide == 0) ? false : true;
    kStream.ReadLinkID();   // m_pkSpawnModifier

    kStream.ReadLinkID();   // m_pkManager
    kStream.ReadLinkID();   // m_spNext
}
//---------------------------------------------------------------------------
void NiPSysCollider::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_pkSpawnModifier = (NiPSysSpawnModifier*) kStream.GetObjectFromLinkID();

    m_pkManager = (NiPSysColliderManager*) kStream.GetObjectFromLinkID();
    m_spNext = (NiPSysCollider*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysCollider::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spNext)
    {
        m_spNext->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysCollider::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fBounce);
    NiStreamSaveBinary(kStream, NiBool(m_bSpawnOnCollide));
    NiStreamSaveBinary(kStream, NiBool(m_bDieOnCollide));
    kStream.SaveLinkID(m_pkSpawnModifier);

    kStream.SaveLinkID(m_pkManager);
    kStream.SaveLinkID(m_spNext);
}
//---------------------------------------------------------------------------
bool NiPSysCollider::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysCollider* pkDest = (NiPSysCollider*) pkObject;

    if (m_fBounce != pkDest->m_fBounce ||
        m_bSpawnOnCollide != pkDest->m_bSpawnOnCollide ||
        m_bDieOnCollide != pkDest->m_bDieOnCollide)
    {
        return false;
    }

    if ((m_pkSpawnModifier && !pkDest->m_pkSpawnModifier) ||
        (!m_pkSpawnModifier && pkDest->m_pkSpawnModifier) ||
        (m_pkSpawnModifier && pkDest->m_pkSpawnModifier &&
            !m_pkSpawnModifier->IsEqual(pkDest->m_pkSpawnModifier)))
    {
        return false;
    }

    if ((m_pkManager && !pkDest->m_pkManager) ||
        (!m_pkManager && pkDest->m_pkManager))
    {
        return false;
    }

    if ((m_spNext && !pkDest->m_spNext) ||
        (!m_spNext && pkDest->m_spNext) ||
        (m_spNext && pkDest->m_spNext &&
            !m_spNext->IsEqual(pkDest->m_spNext)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
