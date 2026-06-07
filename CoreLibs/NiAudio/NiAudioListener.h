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

#ifndef NIAUDIOLISTENER_H
#define NIAUDIOLISTENER_H

#include <NiAudioLibType.h>
#include <NiAVObject.h>

class NiNode;

//
// IMPORTANT!  There can be only active instance of NiAudioListener.  This
// listener is created as part of the startup process of NiAudioSystem
// and is destroyed with the shutdown process.
//

class NIAUDIO_ENTRY NiAudioListener : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareStream;

public:

    virtual void Startup() = 0;

    //
    // The listener requires a coordinate frame to describe its 
    // orientation, so like a camera, the "listener space"
    // direction and kUp vectors can be set.
    //
    virtual void  SetDirectionVector(const NiPoint3& kDir) = 0;
    virtual const NiPoint3& GetDirectionVector() const;

    virtual void  SetUpVector(const NiPoint3& kUp) = 0;
    virtual const NiPoint3& GetUpVector() const;

    //
    // Update SoundSystem with the current status of the listener
    //
    virtual void Update() = 0;
    virtual void UpdateAudioData() = 0;
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);

    //
    // Query the internal SoundSystem renderer state.  Typically
    // the Gamebryo state information should be sufficient,
    // but these methods give the sound system  internal values.
    // Note: These return values are intentionally 
    // NOT references.
    //
    virtual NiPoint3 GetPosition() = 0;
    virtual NiPoint3 GetVelocity() = 0;
    virtual void GetOrientation(NiPoint3& kDir, NiPoint3& kUp) = 0;
    
protected:
    //
    // The constructor and destructor are protected because only 
    // the SoundSystem can create the sole instance of NiAudioListener.
    //
    NiAudioListener();
    virtual ~NiAudioListener();

    virtual void Release();

    NiPoint3 m_kDirection;
    NiPoint3 m_kUp;
    void* m_pvListener;
    
    NiPoint3 m_kLocalVelocity;

    friend class NiAudioSystem;

};

typedef NiPointer<NiAudioListener> NiAudioListenerPtr;


#endif

