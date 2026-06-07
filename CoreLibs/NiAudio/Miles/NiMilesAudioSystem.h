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

#ifndef NIMILESAUDIOSYSTEM_H
#define NIMILESAUDIOSYSTEM_H

#include <NiAudioSystem.h>
#include "NiAudioLibType.h"
#include "NiAudioListener.h"
#include <NiObject.h>
#include <NiStream.h>
#include <NiTPointerList.h>

#include <mss.h>

class NiMilesSource;
class NiMilesListener;
NiSmartPointer(NiAudioListener);

class NIAUDIO_ENTRY NiMilesAudioSystem : public NiAudioSystem
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareFlags(unsigned char);
    
public:
    

    //
    // Access to the unique sound system
    //
    static NiMilesAudioSystem* GetAudioSystem();

    //
    // Get/Set window handle, for use by clients if needed.
    //
    // DirectSound on Win32 needs a correct HWND before initialization.
    // Gamebryo currently uses MSS_MC_USE_SYSTEM_CONFIG to select the output 
    // configuration that Miles will use. This allows the user to specify the 
    // output configuration through the Windows control panel. 
    // It is not known if DirectSound will be used, so the HWND is not passed 
    // into AIL_set_DirectSound_HWND(). If a client application wants to 
    // force DirectSound to be used they would provide MSS_MC_DIRECTSOUND3D 
    // as the selected output configuration and then provide the HWND 
    // through AIL_set_DirectSound_HWND.
    //
    virtual void SetHWnd(NiWindowRef hWnd);
    virtual NiWindowRef GetHWnd();
    
    virtual bool Startup(const char* pcDirectoryname);
    virtual void Shutdown();

    // Access to the single sound system's unique listener.  
    // Note, the application cannot "Set" the listener, 
    // it can only get it. Overload to cast to the correct type
    NiMilesListener* GetListener();

    // The NiAudioSystem contains a factory for creating sources
    // Sources can be created manually but the factory will abstract
    // creation of sources of the current implementation. ie. NiMileSource, 
    // NiSensauraSource
    virtual NiAudioSource* CreateSource(
        unsigned int uiType = NiAudioSource::TYPE_DEFAULT);

    // SetSpeakerType - Will always return false as the speaker type is chosen
    // when Miles is started up. See the Get/SetSpeakerType declarations for 
    // more comments.
    virtual SpeakerType GetSpeakerType();
    virtual bool SetSpeakerType(unsigned int uiType = TYPE_3D_2_SPEAKER);
    virtual bool SetBestSpeakerTypeAvailable();

    // A pointer to a string describing last error.  First byte 0 if no error.
    virtual char* GetLastError();  
    
    const unsigned int GetNumberActiveSamples();

    // Overload to cast directly to the correctly type
    NiMilesSource* GetFirstSource(NiTListIterator& iter);
    NiMilesSource* GetNextSource(NiTListIterator& iter);   

    NiMilesSource* FindDuplicateSource(NiAudioSource* pkOriginal);

    bool IsUniqueSource(NiMilesSource* pkOriginal);
    virtual bool SetUnitsPerMeter(float fUnits);

    void SetDriverProperty(char * cPreferenceName, void * PreferenceValue);
    void GetDriverProperty(char * cPreferenceName, void * PreferenceValue);

    /////////////////////////////////////////////////////////
    // BEGIN REVERB SPECIFIC FUNCTIONS.
    //

    // returns true if reverb is available
    virtual bool GetReverbAvailable();
    
    //
    // Reverb functions - if reverb is supported by the current driver
    //  then SetCurrentRoomReverb will return true.. it will return false
    //  if the driver pointer is zero or if reverb is not supported.
    virtual bool  SetCurrentRoomReverb(unsigned int dwPreset);
    virtual unsigned int GetCurrentRoomReverb();

    void* GetDigitalDriver();

    // Miles requires function pointers if we want to use Gamebryo
    // memory management, but NiMalloc and NiFree are macros.
    // We proxy the macros here so we have something to pass to Miles.
    static void* AILCALLBACK NiMallocProxy(UINTa uiSizeInBytes);
    static void AILCALLBACK NiFreeProxy(void* pvMem);
protected:       
    NiMilesAudioSystem();
    ~NiMilesAudioSystem();
    

    NiWindowRef m_kWnd;
    void* m_pDIG;            // digital driver for Miles
                
    friend class NiMilesAudioSDM;
    friend class NiMilesSource;
};


#endif
