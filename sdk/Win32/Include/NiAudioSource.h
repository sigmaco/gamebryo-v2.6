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

#ifndef NIAUDIOSOURCE_H
#define NIAUDIOSOURCE_H

#include "NiAudioLibType.h"
#include <NiAVObject.h>

class NIAUDIO_ENTRY NiAudioSource : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiAudioSource);
    NiDeclareStream;
    NiDeclareFlags(unsigned short);

public:

    enum
    {
        TYPE_3D = 0x00000000,
        TYPE_AMBIENT = 0x00000001,
        TYPE_DEFAULT = TYPE_3D
    };

    NiAudioSource(unsigned int uiType = TYPE_DEFAULT);
    virtual ~NiAudioSource();

    virtual unsigned int GetType();
    
    //    
    // Wave loading and unloading
    //
    virtual void SetAllowSharing(bool bAllowSharing);
    virtual bool GetAllowSharing() const;
    
    virtual void SetStreamed(bool bStream);
    virtual bool GetStreamed() const;

    virtual void SetFilename(const char* pcFilename);

    virtual const char* GetFilename();
    virtual const char* GetLocalName();

    virtual bool Load();
    virtual bool Unload();
 
    //
    // Audio parameters --
    // Many of these functions access the SoundSystem interface directly.  
    // Those that do will have corresponding "Set" functions 
    // return a boolean indicating if it was possible to set the 
    // value. However, in most cases the return value only indicates
    // if we were able to make a call to the SoundSystem (valid sound handle)
    // as opposed to if the SoundSystem actually set the values correctly.
    //
    virtual bool SetConeData(float fAngle1Deg, 
        float fAngle2Deg, float fGain) = 0;
    virtual void GetConeData(float& fAngle1Deg, 
        float& fAngle2Deg, float& fGain) = 0;

    //
    // Any **cone** sources require a coordinate frame to describe 
    // their orientation, so like a camera, the "source space"
    // direction and up vectors can be set.
    //
    virtual void SetDirectionVector(const NiPoint3& kDir);
    virtual const NiPoint3& GetDirectionVector() const;
    
    virtual void SetUpVector(const NiPoint3& kUp);
    virtual const NiPoint3& GetUpVector() const;


    virtual bool SetMinMaxDistance(float fMin, float fMax) = 0;
    virtual void GetMinMaxDistance(float& fMin, float& fMax) = 0;
    
    virtual bool SetGain(float fGain) = 0;
    virtual float GetGain() = 0;

    virtual bool FadeToGain(float fGain, float fStartTime, float fDuration);

    virtual bool SetPlaybackRate(long lRate) = 0;
    virtual long GetPlaybackRate() = 0;
    
    //
    // Playback control
    //
    enum
    {
        LOOP_SINGLE = 1,
        LOOP_INFINITE = 0
    };

    virtual void SetLoopCount(int iLoopCount);
    virtual int GetLoopCount();
    
    virtual bool Play() = 0;
    virtual bool Stop() = 0;
    virtual void Rewind() = 0;
    
    enum Status
    {
        DONE,
        PLAYING,
        STOPPED,
        FREE,
        PLAYINGBUTRELEASED,
        LOADED,
        NOT_SET
    };
    virtual Status GetStatus() = 0;
    
    virtual bool SetPlayTime(float fTime) = 0;
    virtual float GetPlayTime() = 0;
    virtual bool GetPlayLength(float& fTime) = 0;
    
    virtual bool SetPlayPosition(unsigned int uiPos) = 0;
    virtual unsigned int GetPlayPosition() = 0;   
    
    //
    // Update
    //
    virtual void Update(float fTime);
    virtual void UpdateAudioData(float fTime);
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);

    //
    // Occlusion, obstruction and room effect
    //
    virtual bool SetRoomEffectLevel(float fLevel) = 0;
    virtual float GetRoomEffectLevel() = 0;

    virtual bool SetOcclusionFactor(float fLevel) = 0;
    virtual float GetOcclusionFactor() = 0;

    virtual bool SetObstructionFactor(float fLevel) = 0;
    virtual float GetObstructionFactor() = 0;
    
    //
    // Query the internal SoundSystem renderer state.  Typically
    // the Gamebryo state information should be sufficient,
    // but these methods give the SoundSystem's internal values.
    // Note: These return values are intentionally 
    // NOT references.
    //
    virtual NiPoint3 GetPosition() = 0;
    virtual void GetOrientation(NiPoint3& kDir, NiPoint3& kUp) = 0;

    virtual void SetVelocity(NiPoint3& kVelocity);
    virtual NiPoint3 GetVelocity();

protected:

    void Initialize();

    //
    // Get/set loaded bit
    //
    virtual void SetLoaded(bool bLoaded);
    virtual bool GetLoaded() const;
  
    //
    // Stop and release the source
    //
    virtual void Release();

    // --- Begin NIF conversion code
    // Deprecated flags - use new flag code
    enum
    {
        MAX_POS = 10
    };
    // --- End NIF conversion code

    // flags
    enum
    {
        CONE_MASK           = 0x0001,
        LOADED_MASK         = 0x0002,
        STREAM_MASK         = 0x0004,
        ALLOW_SHARING_MASK  = 0x0008
    };

    virtual void SetCone(bool bCone);
    virtual bool GetCone() const;

    bool CloseTo(float fA, float fB, float fEpsilon);

    enum
    {
        NOT_FADING = -1
    };

    char* m_pcFilename;
    char* m_pcLocalName;  
    unsigned int m_uiInitialType;
    int m_iLoopCount;

    // Cone Data
    float m_fConeAngle1Deg;
    float m_fConeAngle2Deg;
    float m_fConeGain;
   
    NiPoint3 m_kDirection;
    NiPoint3 m_kUp;

    float m_fMinDistance;
    float m_fMaxDistance;

    float m_fGain;

    long m_lPlaybackRate;
    float m_fPlayTime;
    unsigned int m_uiPlayPosition;

    float m_fRoomEffectLevel;
    float m_fOcclusionFactor;
    float m_fObstructionFactor;
    
    float m_fFadeInitialGain;
    float m_fFadeDestinationGain;
    float m_fFadeStartTime;
    float m_fFadeDuration;

    NiPoint3 m_kLocalVelocity;

    friend class NiAudioSystem;
};

typedef NiPointer<NiAudioSource> NiAudioSourcePtr;


#endif
