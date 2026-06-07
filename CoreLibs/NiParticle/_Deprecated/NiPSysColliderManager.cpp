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

#include "NiPSysColliderManager.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysColliderManager, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysColliderManager::NiPSysColliderManager()
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysColliderManager);
//---------------------------------------------------------------------------
void NiPSysColliderManager::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spColliders
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_spColliders = (NiPSysCollider*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysColliderManager::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spColliders)
    {
        m_spColliders->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_spColliders);
}
//---------------------------------------------------------------------------
bool NiPSysColliderManager::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysColliderManager* pkDest = (NiPSysColliderManager*) pkObject;

    if ((m_spColliders && !pkDest->m_spColliders) ||
        (!m_spColliders && pkDest->m_spColliders) ||
        (m_spColliders && pkDest->m_spColliders &&
            !m_spColliders->IsEqual(pkDest->m_spColliders)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
