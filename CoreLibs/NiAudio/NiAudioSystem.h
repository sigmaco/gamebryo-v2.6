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

#ifndef NIAUDIOSYSTEM_H
#define NIAUDIOSYSTEM_H

#include "NiAudioLibType.h"
#include "NiAudioListener.h"
#include <NiObject.h>
#include <NiStream.h>
#include <NiTPointerList.h>
#include "NiAudioSource.h"
#include "NiAudioMetrics.h"

NiSmartPointer(NiAudioListener);

class NIAUDIO_ENTRY NiAudioSystem : public NiObject
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareFlags(unsigned char);
    
public:
    //
    // Access to the unique sound system
    //
    static NiAudioSystem* GetAudioSystem();
    
    virtual bool Startup(const char* pcDirectoryname) = 0;
    virtual void Shutdown();

    //
    // Access to the single sound system's unique listener.  
    // Note, the application cannot "Set" the listener, 
    // it can only get it.
    //
    NiAudioListener* GetListener();

    // 
    // The NiAudioSystem contains a factory for creating sources
    // Sources can be created manually but the factory will abstract
    // creation of sources of the current implementation. ie. NiMileSource, 
    // NiSensauraSource
    virtual NiAudioSource* CreateSource(unsigned int uiType = 
        NiAudioSource::TYPE_DEFAULT) = 0;

    enum SpeakerType
    { 
        TYPE_3D_2_SPEAKER,
        TYPE_3D_HEADPHONE,
        TYPE_3D_SURROUND,
        TYPE_3D_4_SPEAKER,
        TYPE_3D_5_1_SPEAKER,
        TYPE_3D_7_1_SPEAKER,
        TYPE_3D_SPEAKER_TYPE_COUNT
    };
    virtual SpeakerType GetSpeakerType() = 0;
    virtual bool SetSpeakerType(unsigned int uiType = TYPE_3D_2_SPEAKER) = 0;
    virtual bool SetBestSpeakerTypeAvailable() = 0;

    // a pointer to a string describing last error. First byte 0 if no error
    virtual char* GetLastError() = 0;  
    
    //
    // Update audio info  
    //
    virtual void Update(float fTime, bool bUpdateAll = false);
    
    // 
    // Play and update control for ALL sources
    //
    virtual void PlayAllSources();
    virtual void StopAllSources();
    virtual void UpdateAllSources(float fTime);
    
    //
    // Functions that allow iteration over the list of sources
    //
    // Example of iteration:
    //    NiTListIterator iter;
    //    NiAudioSource* pSource = GetFirstSource(iter);
    //    while (pSource)
    //    {
    //        <do something with pSource>;
    //        pSource = GetNextSource(iter);
    //    }
    //
    NiAudioSource* GetFirstSource(NiTListIterator& iter);
    NiAudioSource* GetNextSource(NiTListIterator& iter);   

    NiAudioSource* FindDuplicateSource(NiAudioSource* pkOriginal);

    //
    // UnitsPerMeter - =The SoundSystem uses meters, if the application
    //  uses units other than meters this function should be called (returns
    //  true if valid [positive nonzero] fUnits)
    //   
    virtual bool SetUnitsPerMeter(float fUnits);
    virtual float GetUnitsPerMeter();

    /////////////////////////////////////////////////////////
    // BEGIN REVERB SPECIFIC FUNCTIONS.
    //

    // returns true if reverb is available
    //  
    virtual bool GetReverbAvailable() = 0;
    
    // Miles enumerated environment types
    enum
    {
        ENVIRONMENT_GENERIC,                // factory default
        ENVIRONMENT_PADDEDCELL,
        ENVIRONMENT_ROOM,                   // standard environments
        ENVIRONMENT_BATHROOM,
        ENVIRONMENT_LIVINGROOM,
        ENVIRONMENT_STONEROOM,
        ENVIRONMENT_AUDITORIUM,
        ENVIRONMENT_CONCERTHALL,
        ENVIRONMENT_CAVE,
        ENVIRONMENT_ARENA,
        ENVIRONMENT_HANGAR,
        ENVIRONMENT_CARPETEDHALLWAY,
        ENVIRONMENT_HALLWAY,
        ENVIRONMENT_STONECORRIDOR,
        ENVIRONMENT_ALLEY,
        ENVIRONMENT_FOREST,
        ENVIRONMENT_CITY,
        ENVIRONMENT_MOUNTAINS,
        ENVIRONMENT_QUARRY,
        ENVIRONMENT_PLAIN,
        ENVIRONMENT_PARKINGLOT,
        ENVIRONMENT_SEWERPIPE,
        ENVIRONMENT_UNDERWATER,
        ENVIRONMENT_DRUGGED,
        ENVIRONMENT_DIZZY,
        ENVIRONMENT_PSYCHOTIC,

        ENVIRONMENT_COUNT           // total number of environments
    };

    //
    // Reverb functions - if reverb is supported 
    //  then SetCurrentRoomReverb will return true.. it will return false
    
    virtual bool  SetCurrentRoomReverb(unsigned int uiPreset) = 0;
    virtual unsigned int GetCurrentRoomReverb() = 0;

    
    // *** begin Emergent internal use only ***
    static NiAudioSystem* ms_pAudioSystem;

    virtual ~NiAudioSystem();

protected:       
    NiAudioSystem();
    
    //
    // Sources are only added by the NiAudioSource constructor
    //
    virtual void AddSource(NiAudioSource* pSource);

    //
    // A source must be removed from the sound system for it 
    // to be cleaned up.  Otherwise the reference count will
    // always be at least 1 because of the reference to it from
    // the internal list of sound system sources.
    //
    virtual void RemoveSource(NiAudioSource* pSource);

    float  m_fUnitsPerMeter;
        
    NiAudioListenerPtr m_spListener;
    NiTPointerList<NiAudioSource*>* m_pSources;
        
    friend class NiAudioSource;
};


#endif
