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
#include "NiMilesAudioPCH.h"

#include "NiAudioListener.h"
#include "NiAudioSystem.h"
#include "NiAudioSource.h"
#include <NiBool.h>
#include <NiMaterialProperty.h>
#include <NiNode.h>

NiAudioSystem* NiAudioSystem::ms_pAudioSystem = NULL;

NiImplementRTTI(NiAudioSystem, NiObject);

//---------------------------------------------------------------------------
NiAudioSystem::NiAudioSystem()
{
    m_pSources = NiNew NiTPointerList<NiAudioSource*>;
    NIASSERT(m_pSources);

    m_fUnitsPerMeter = 1.0;     // set units to meters
    m_uFlags = 0;
}
//---------------------------------------------------------------------------
NiAudioSystem::~NiAudioSystem()
{
    Shutdown();

    // 
    // Decrement/destroy listener
    //
    m_spListener = 0;
    NiDelete m_pSources;
}
//---------------------------------------------------------------------------
NiAudioListener* NiAudioSystem::GetListener() 
{
    return m_spListener;
}
//---------------------------------------------------------------------------
void NiAudioSystem::Shutdown() 
{
    if (m_pSources)
    {
        //
        // Any sources that have already released themselves will have been
        // removed from this list.
        //
        NiTListIterator pos = m_pSources->GetHeadPos();
        while (pos)
        {
            NiAudioSource* pSource = m_pSources->GetNext(pos);
            pSource->Unload();
        }

        m_pSources->RemoveAll();
    }

    //
    // Release and decrement/destroy the listener
    //
    if (m_spListener)
        m_spListener->Release();

}
//---------------------------------------------------------------------------  
void NiAudioSystem::AddSource(NiAudioSource* pSource)
{
    m_pSources->AddTail(pSource);
}
//---------------------------------------------------------------------------  
void NiAudioSystem::RemoveSource(NiAudioSource* pSource)
{
    if (pSource->GetRefCount() == 0)
        m_pSources->Remove(pSource);
}
//---------------------------------------------------------------------------
void NiAudioSystem::Update(float fTime, bool bUpdateAll)
{
    if (!m_spListener)
        return;

    if (bUpdateAll)
    {
        //
        // Update Listener
        //
        m_spListener->Update();

        //
        // Update Sources 
        //
        UpdateAllSources(fTime);
    }

    NIMETRICS_AUDIO_RECORDMETRICS();
}
//---------------------------------------------------------------------------
void NiAudioSystem::PlayAllSources()
{
    NiTListIterator pos = m_pSources->GetHeadPos();
    while (pos)
    {
        NiAudioSource* pSource = m_pSources->GetNext(pos);
        pSource->Play();
    }
}
//---------------------------------------------------------------------------
void NiAudioSystem::StopAllSources()
{
    NiTListIterator pos = m_pSources->GetHeadPos();
    while (pos)
    {
        NiAudioSource* pSource = m_pSources->GetNext(pos);
        pSource->Stop();
    }
}
//---------------------------------------------------------------------------
void NiAudioSystem::UpdateAllSources(float fTime)
{
    NiTListIterator pos = m_pSources->GetHeadPos();
    while (pos)
    {
        NiAudioSource* pSource = m_pSources->GetNext(pos);
        pSource->Update(fTime);
    }
}
//---------------------------------------------------------------------------
NiAudioSource* NiAudioSystem::GetFirstSource(NiTListIterator& iter) 
{
    if (m_pSources)
    {
        iter = m_pSources->GetHeadPos();
        return (iter ? m_pSources->GetNext(iter) : 0);
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiAudioSource* NiAudioSystem::GetNextSource(NiTListIterator& iter)
{
    if (m_pSources)
        return (iter ? m_pSources->GetNext(iter) : 0);
    else
        return 0;
}
//---------------------------------------------------------------------------  
NiAudioSource* NiAudioSystem::FindDuplicateSource(NiAudioSource* pkOriginal)
{
    // Check to see if there is already a source with the same filename.
    // If so, duplicate the source rather than create a new one.  
    NiTListIterator iter;
    NiAudioSource* pkSource = GetFirstSource(iter);

    while (pkSource)
    {
        if ((pkOriginal != pkSource) && pkSource->GetAllowSharing() &&
            pkSource->GetLocalName() && 
            (!strcmp(pkOriginal->GetLocalName(), pkSource->GetLocalName())))
        {
            return pkSource;
        }
        pkSource = GetNextSource(iter);
    }

    return NULL;
}
//---------------------------------------------------------------------------  
bool NiAudioSystem::SetUnitsPerMeter(float fUnits)
{
    if (fUnits <= 0.0)
        return false;

    m_fUnitsPerMeter = fUnits;
    return true;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiObject* NiAudioSystem::CreateObject(const char**,
    unsigned int)
{
    return NiAudioSystem::GetAudioSystem();
}
//---------------------------------------------------------------------------
void NiAudioSystem::LoadBinary (NiStream& stream)
{
    NiObject::LoadBinary(stream);
    float fVal1;
    NiStreamLoadBinary(stream, fVal1);
    SetUnitsPerMeter(fVal1);
    NiStreamLoadBinary(stream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiAudioSystem::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiAudioSystem::RegisterStreamables(NiStream& stream)
{
    if (! NiObject::RegisterStreamables(stream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiAudioSystem::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);
    NiStreamSaveBinary(stream, m_fUnitsPerMeter);
    NiStreamSaveBinary(stream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiAudioSystem::IsEqual(NiObject* pObject)
{
    if (! NiObject::IsEqual(pObject))
        return false;

    NiAudioSystem* pSS = (NiAudioSystem*)pObject;

    if (m_fUnitsPerMeter != pSS->GetUnitsPerMeter()) 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiAudioSystem* NiAudioSystem::GetAudioSystem()
{
    return ms_pAudioSystem;
}
//---------------------------------------------------------------------------
float NiAudioSystem::GetUnitsPerMeter()
{
    return m_fUnitsPerMeter;
}
//---------------------------------------------------------------------------
