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
#include <NiMatrix3.h>

NiImplementRTTI(NiAudioListener,NiAVObject);
//---------------------------------------------------------------------------
NiAudioListener::NiAudioListener()
{
    //
    // These are initialized this way to avoid any pre-main initialization
    // ordering problems, i.e. to avoid the possibility of the pre-main
    // created listener from having invalid vectors/transforms because
    // things like NiPoint3::ZERO and NiMatrix3::IDENTITY haven't been 
    // initialized yet.
    //
    m_kDirection = NiPoint3(1, 0, 0);
    m_kUp = NiPoint3(0, 1, 0);  

    m_kLocal.m_Translate = NiPoint3(0, 0, 0);

    NiMatrix3 diagMatrix;
    diagMatrix.MakeDiagonal (1.0, 1.0, 1.0);
    m_kLocal.m_Rotate = diagMatrix;
    m_kLocalVelocity = NiPoint3(0, 0, 0);

    m_pvListener = 0;
}
//---------------------------------------------------------------------------
NiAudioListener::~NiAudioListener() 
{
    Release();
}
//---------------------------------------------------------------------------
void NiAudioListener::Release()
{ 
}
//---------------------------------------------------------------------------
void NiAudioListener::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateDownwardPass(kUpdate);
    UpdateAudioData();
}
//---------------------------------------------------------------------------
void NiAudioListener::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateSelectedDownwardPass(kUpdate);
    UpdateAudioData();
}
//---------------------------------------------------------------------------
void NiAudioListener::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateRigidDownwardPass(kUpdate);
    UpdateAudioData();
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiObject* NiAudioListener::CreateObject(const char**,
    unsigned int)
{
    return NiAudioSystem::GetAudioSystem()->GetListener();
}
//---------------------------------------------------------------------------
void NiAudioListener::LoadBinary (NiStream& stream)
{
    NiAVObject::LoadBinary(stream);

    m_kDirection.LoadBinary(stream);
    m_kUp.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiAudioListener::LinkObject(NiStream& stream)
{
    NiAVObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiAudioListener::RegisterStreamables(NiStream& stream)
{
    if (! NiAVObject::RegisterStreamables(stream))
        return false;

    //
    // Register the sound system -- this call is the only place the
    // stream knows about the sound system.  NiStream will handle 
    // the rest.
    //
    if (! NiAudioSystem::GetAudioSystem()->RegisterStreamables(stream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiAudioListener::SaveBinary(NiStream& stream)
{
    NiAVObject::SaveBinary(stream);

    m_kDirection.SaveBinary(stream);
    m_kUp.SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiAudioListener::IsEqual (NiObject* pObject)
{
    if (! NiAVObject::IsEqual(pObject))
        return false;

    NiAudioListener* pListen = (NiAudioListener*)pObject;

    if (m_kDirection != pListen->m_kDirection)
        return false;

    if (m_kUp != pListen->m_kUp)
        return false;

    return true;
}
//---------------------------------------------------------------------------
const NiPoint3& NiAudioListener::GetDirectionVector() const
{
    return m_kDirection;
}
//---------------------------------------------------------------------------
const NiPoint3& NiAudioListener::GetUpVector() const
{
    return m_kUp;
}
//---------------------------------------------------------------------------
