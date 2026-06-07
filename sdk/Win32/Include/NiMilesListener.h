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

#ifndef NIMILESLISTENER_H
#define NIMILESLISTENER_H

#include <NiAudioListener.h>

//
// IMPORTANT!  There can be only active instance of NiListener.  This
// listener is created as part of the startup process of NiAudioSystem
// and is destroyed with the shutdown process.
//

class NIAUDIO_ENTRY NiMilesListener : public NiAudioListener
{
    NiDeclareRTTI;
    NiDeclareStream;

public:

    //
    // The listener requires a coordinate frame to describe its 
    // orientation, so like a camera, the "listener space"
    // direction and kUp vectors can be set.
    //
    virtual void  SetDirectionVector(const NiPoint3& kDir);
    virtual const NiPoint3& GetDirectionVector() const;

    virtual void  SetUpVector(const NiPoint3& kUp);
    virtual const NiPoint3& GetUpVector() const;

    //
    // Update SoundSystem with the current status of the listener
    //
    virtual void Update();
    virtual void UpdateAudioData();

    //
    // Query the internal SoundSystem renderer state.  Typically
    // theGamebryo state information should be sufficient,
    // but these methods give Miles's internal values.
    // Note: These return values are intentionally 
    // NOT references.
    //
    virtual NiPoint3 GetPosition();
    virtual NiPoint3 GetVelocity();
    virtual void GetOrientation(NiPoint3& kDir, NiPoint3& kUp);
    
    inline virtual NiPoint3 GetWorldVelocity() { return m_kLocalVelocity; };

protected:
    //
    // The constructor and destructor are protected because only 
    // the SoundSystem can create the sole instance of NiListener.
    //
    NiMilesListener();
    virtual ~NiMilesListener();

    virtual void Release();

    friend class NiMilesAudioSystem;

private:

    virtual void Startup();     

};

typedef NiPointer<NiMilesListener> NiMilesListenerPtr;


#endif

