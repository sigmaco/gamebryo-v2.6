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

#include "NiParticleSystem.h"
#include "NiPSysModifier.h"

NiImplementRTTI(NiParticleSystem, NiParticles);

//---------------------------------------------------------------------------
NiParticleSystem::NiParticleSystem() :
    m_bWorldSpace(true),
    m_fLastTime(-NI_INFINITY),
    m_bResetSystem(false),
    m_bDynamicBounds(false)
{
}
//---------------------------------------------------------------------------
void NiParticleSystem::AddModifier(NiPSysModifier* pkModifier)
{
    NIASSERT(pkModifier && pkModifier->GetName());

    // Insert the modifier into the list in the proper order.
    bool bInserted = false;
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    if (!kIter)
    {
        m_kModifierList.AddHead(pkModifier);
        bInserted = true;
    }
    while (kIter)
    {
        NiPSysModifier* pkCurModifier = m_kModifierList.Get(kIter);
        if (pkCurModifier->GetOrder() > pkModifier->GetOrder())
        {
            m_kModifierList.InsertBefore(kIter, pkModifier);
            bInserted = true;
            break;
        }
        kIter = m_kModifierList.GetNextPos(kIter);
    }
    if (!bInserted)
    {
        m_kModifierList.AddTail(pkModifier);
    }

    // Set target of modifier.
    pkModifier->SetSystemPointer(this);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiParticleSystem);
//---------------------------------------------------------------------------
void NiParticleSystem::LoadBinary(NiStream& kStream)
{
    NiParticles::LoadBinary(kStream);

    NiBool bWorldSpace;
    NiStreamLoadBinary(kStream, bWorldSpace);
    m_bWorldSpace = NIBOOL_IS_TRUE(bWorldSpace);

    kStream.ReadMultipleLinkIDs();  // m_kModifierList
}
//---------------------------------------------------------------------------
void NiParticleSystem::LinkObject(NiStream& kStream)
{
    NiParticles::LinkObject(kStream);

    unsigned int uiNumModifiers = kStream.GetNumberOfLinkIDs();
    for (unsigned int ui = 0; ui < uiNumModifiers; ui++)
    {
        NiPSysModifier* pkModifier = (NiPSysModifier*)
            kStream.GetObjectFromLinkID();
        AddModifier(pkModifier);
    }
}
//---------------------------------------------------------------------------
bool NiParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiParticles::RegisterStreamables(kStream))
    {
        return false;
    }

    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkModifier->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiParticleSystem::SaveBinary(NiStream& kStream)
{
    NiParticles::SaveBinary(kStream);

    NiBool bWorldSpace = m_bWorldSpace;
    NiStreamSaveBinary(kStream, bWorldSpace);

    NiStreamSaveBinary(kStream, m_kModifierList.GetSize());
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        kStream.SaveLinkID(pkModifier);
    }
}
//---------------------------------------------------------------------------
bool NiParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiParticles::IsEqual(pkObject))
    {
        return false;
    }

    NiParticleSystem* pkDest = (NiParticleSystem*) pkObject;

    if (m_bWorldSpace != pkDest->m_bWorldSpace ||
        m_bDynamicBounds != pkDest->m_bDynamicBounds)
    {
        return false;
    }

    NiTListIterator kListIter = m_kModifierList.GetHeadPos();
    while (kListIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kListIter);
        NiPSysModifier* pkDestModifier = pkDest->GetModifierByName(
            pkModifier->GetName());
        if (!pkModifier->IsEqual(pkDestModifier))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
