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

#include "NiMilesAudioSystem.h"
#include "NiMilesListener.h"
#include <NiMatrix3.h>
#include <Mss.h>

NiImplementRTTI(NiMilesListener,NiAudioListener);
//---------------------------------------------------------------------------
NiMilesListener::NiMilesListener()
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
NiMilesListener::~NiMilesListener() 
{
    Release();
}
//---------------------------------------------------------------------------
void NiMilesListener::Release() 
{
  /* ... */
}
//---------------------------------------------------------------------------
void NiMilesListener::Startup()
{
  /* ... */
}
//---------------------------------------------------------------------------
void NiMilesListener::Update() 
{
    UpdateAudioData();
}
//---------------------------------------------------------------------------
void NiMilesListener::UpdateAudioData()
{
    NiPoint3 worldLoc = GetWorldTranslate();
    NiPoint3 worldDir = GetWorldRotate() * m_kDirection;
    NiPoint3 worldUp =  GetWorldRotate() * m_kUp;
    NiPoint3 worldVel = GetWorldVelocity();

    // Miles wants meters per millisecond.. we want units per second
    float fUnitsPerMeter = NiAudioSystem::GetAudioSystem()->GetUnitsPerMeter();

    float fThousandUPM = (float) 1000.0 * fUnitsPerMeter;
    
    worldVel.x =  worldVel.x / fThousandUPM;
    worldVel.y =  worldVel.y / fThousandUPM;
    worldVel.z = -worldVel.z / fThousandUPM;    // also change from RH->LH

    // RH->LH coord. system for orientation
    worldDir.z *= -1.0;
    worldUp.z  *= -1.0;
    worldLoc.z *= -1.0;

    NiMilesAudioSystem* pSound = NiMilesAudioSystem::GetAudioSystem();
    
    AIL_set_listener_3D_position((HDIGDRIVER)pSound->GetDigitalDriver(),
        worldLoc.x, worldLoc.y, worldLoc.z);

    AIL_set_listener_3D_orientation((HDIGDRIVER)pSound->GetDigitalDriver(),
        worldDir.x, worldDir.y, worldDir.z, worldUp.x, worldUp.y, worldUp.z);

    AIL_set_listener_3D_velocity_vector((HDIGDRIVER)pSound->GetDigitalDriver(),
        worldVel.x, worldVel.y, worldVel.z);
}
//---------------------------------------------------------------------------
NiPoint3 NiMilesListener::GetPosition() 
{
    NiPoint3 pos;
    
    NiMilesAudioSystem* pSound = NiMilesAudioSystem::GetAudioSystem();
    AIL_listener_3D_position((HDIGDRIVER)pSound->GetDigitalDriver(), 
        &pos.x, &pos.y, &pos.z);

    // LH->RH coord.
    pos.z *= -1.0;

    return pos;
}
//---------------------------------------------------------------------------
// this is meters/millisecond
NiPoint3 NiMilesListener::GetVelocity() 
{
    NiPoint3 vel;

    NiMilesAudioSystem* pSound = NiMilesAudioSystem::GetAudioSystem();
    AIL_listener_3D_velocity((HDIGDRIVER)pSound->GetDigitalDriver(), 
        &vel.x, &vel.y, &vel.z);

    float fUnitsPerMeter = NiAudioSystem::GetAudioSystem()->GetUnitsPerMeter();

    float fThousandUPM = (float) 1000.0 * fUnitsPerMeter;

    // convert back to units per second
    vel.x *=  fThousandUPM;
    vel.y *=  fThousandUPM;
    vel.z *= -fThousandUPM; // LH->RH
  
    return vel;
}
//---------------------------------------------------------------------------
void NiMilesListener::GetOrientation(NiPoint3& dir, NiPoint3& up) 
{
    // get orientation from Miles in LH-coordinate
    NiMilesAudioSystem* pSound = NiMilesAudioSystem::GetAudioSystem();
    AIL_listener_3D_orientation((HDIGDRIVER)pSound->GetDigitalDriver(),
        &dir.x, &dir.y, &dir.z, &up.x, &up.y, &up.z);

    // invert Z coordinate to get back into RH-coordinate
    dir.z *= -1.0;
    up.z  *= -1.0;
}

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiObject* NiMilesListener::CreateObject(const char**,
    unsigned int)
{
    return NiAudioSystem::GetAudioSystem()->GetListener();
}
//---------------------------------------------------------------------------
void NiMilesListener::LoadBinary (NiStream& stream)
{
    NiAVObject::LoadBinary(stream);

    m_kDirection.LoadBinary(stream);
    m_kUp.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiMilesListener::LinkObject(NiStream& stream)
{
    NiAVObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiMilesListener::RegisterStreamables(NiStream& stream)
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
void NiMilesListener::SaveBinary(NiStream& stream)
{
    NiAVObject::SaveBinary(stream);

    m_kDirection.SaveBinary(stream);
    m_kUp.SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiMilesListener::IsEqual (NiObject* pObject)
{
    if (! NiAVObject::IsEqual(pObject))
        return false;

    NiMilesListener* pListen = (NiMilesListener*)pObject;

    if (m_kDirection != pListen->m_kDirection)
        return false;

    if (m_kUp != pListen->m_kUp)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiMilesListener::SetDirectionVector(const NiPoint3& kDir)
{
    m_kDirection = kDir;
}
//---------------------------------------------------------------------------
const NiPoint3& NiMilesListener::GetDirectionVector() const
{
    return m_kDirection;
}
//---------------------------------------------------------------------------
void NiMilesListener::SetUpVector(const NiPoint3& kUp)
{
    m_kUp = kUp;
}
//---------------------------------------------------------------------------
const NiPoint3& NiMilesListener::GetUpVector() const
{
    return m_kUp;
}
//---------------------------------------------------------------------------
