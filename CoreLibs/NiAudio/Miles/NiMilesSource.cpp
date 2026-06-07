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
#include "NiMilesSource.h"
#include <NiSystem.h>
#include <NiFilename.h>
#include <NiStopWatch.h>
#include <Mss.h>

NiImplementRTTI(NiMilesSource,NiAudioSource);

//---------------------------------------------------------------------------
NiMilesSource::NiMilesSource(unsigned int uiType) 
    : NiAudioSource(uiType)
{
    m_pvSource = 0;
    m_pvMemPtr = 0;

    // Initialize the status
    m_eDoneStatus = NOT_SET;

    // By default we want to Allow Sharing
    SetAllowSharing(true);
}
//---------------------------------------------------------------------------
NiMilesSource::~NiMilesSource()
{
    Unload();
}
//---------------------------------------------------------------------------
void NiMilesSource::Release()
{
    if (m_pvSource)
    {
        if (GetStreamed())
        {
            AIL_pause_stream((HSTREAM)m_pvSource, 1);
            AIL_close_stream((HSTREAM)m_pvSource);
        }
        else
        {
            AIL_release_sample_handle((HSAMPLE)m_pvSource);
        }
        
        m_pvSource = 0;
    }
}
//--------------------------------------------------------------------------- 
void NiMilesSource::SetAllowSharing(bool bAllowSharing)
{
    // Only change Allow Sharing when the sound isn't loaded
    NIASSERT(!GetLoaded());   

    // Only allow duplicate for non streamed
    if (GetStreamed())
        NiAudioSource::SetAllowSharing(false);
    else
        NiAudioSource::SetAllowSharing(bAllowSharing);
}
//--------------------------------------------------------------------------- 
void NiMilesSource::SetStreamed(bool bStream)
{
    // Only change Allow Sharing when the sound isn't loaded
    NIASSERT(!GetLoaded());   

    // Only allow Streamed for TYPE_AMBIENT
    if(GetType() != TYPE_AMBIENT)
        NiAudioSource::SetStreamed(false);
    else
    {
        NiAudioSource::SetStreamed(bStream);

            // Streamed sources can't share
        if (bStream)
            SetAllowSharing(false);
    }
}
//--------------------------------------------------------------------------- 
void NiMilesSource::SetFilename(const char* pFilename)
{
    NIASSERT(pFilename);

    // First check to see if incoming filename is the same as the 
    // existing one.  If they are identical then, assuming the Source 
    // exists, the function can return.
    if (m_pcFilename)
    {
        if (!strcmp(m_pcFilename, pFilename))
        {
            if (m_pvSource)
                return;            
        }
        else
        {
            Unload();
        }
    }

    // New filename will require loading
    SetLoaded(false);

    NiFree(m_pcFilename);
    NiFree(m_pcLocalName);
    m_pcFilename = 0;
    m_pcLocalName = 0;

    size_t stLen = strlen(pFilename) + 1;
    m_pcFilename = NiAlloc(char, stLen);
    NIASSERT(m_pcFilename);
    NiStrcpy(m_pcFilename, stLen, pFilename);

    // Now set filename only that can be used as a second check 
    // for creating Sharing sources.
    char acSearchPath[NI_MAX_PATH]; 

    NiFilename kFilename(m_pcFilename);
    kFilename.SetDir("");
    kFilename.SetDrive("");
    kFilename.SetPlatformSubDir("");
    kFilename.GetFullPath(acSearchPath, NI_MAX_PATH);

    stLen = strlen(acSearchPath) + 1;
    m_pcLocalName = NiAlloc(char, stLen);
    NIASSERT(m_pcLocalName);
    NiStrcpy(m_pcLocalName, stLen, acSearchPath);

    // Reset the Done Status
    m_eDoneStatus = NOT_SET;
}
//--------------------------------------------------------------------------- 
bool NiMilesSource::Load() 
{
    // Returns false if data is invalid OR if data is already loaded
    if (GetLoaded())
    {
        NiOutputDebugString("Data already loaded\n");
        return false;
    }

    if (!m_pcFilename)
    {
        NiOutputDebugString("Invalid filename\n");
        return false;
    }

    // Reset the Done Status
    m_eDoneStatus = NOT_SET;

    // Handle Sharing loading
    NiMilesAudioSystem* pSound = NiMilesAudioSystem::GetAudioSystem();
        
    NiMilesSource* pDup = NULL;

    if (GetAllowSharing() && !GetStreamed())
    {
        pDup = pSound->FindDuplicateSource(this);        

        if (pDup && !pDup->GetStreamed())
        {
            m_pvMemPtr = pDup->GetFileImage();
            SetLoaded(true);
            return true;
        }
    }


    switch(m_uiInitialType)
    {
    case TYPE_3D:
        {
            void* pSData;
            void* pDData;
            S32   uiType;
            
            AILSOUNDINFO Info;
            S32 uiSize;
            
            uiSize=AIL_file_size(m_pcFilename);
            pSData=AIL_file_read(m_pcFilename,0);
            
            if (pSData==0)
                return(0);
            
            uiType=AIL_file_type(pSData, uiSize);
            
            switch (uiType)
            {
            case AILFILETYPE_PCM_WAV: 
                m_pvMemPtr = pSData;
                SetLoaded(true);
                return true;

            case AILFILETYPE_ADPCM_WAV:   
                AIL_WAV_info(pSData,&Info);
                AIL_decompress_ADPCM(&Info,&pDData,0);
                AIL_mem_free_lock(pSData);
                m_pvMemPtr = pDData;
                SetLoaded(true);
                return true;

            case AILFILETYPE_MPEG_L3_AUDIO:
                AIL_decompress_ASI(pSData,uiSize,m_pcFilename,&pDData,0,0);
                AIL_mem_free_lock(pSData);
                m_pvMemPtr = pDData;
                SetLoaded(true);
                return true;
            default:
                AIL_mem_free_lock(pSData);
                m_pvMemPtr = 0;
                return false;
            };
        }
        break;
    case TYPE_AMBIENT:
        if (GetStreamed())
        {
            SetLoaded(true);
            return true;
        }
        else 
        {
            m_pvMemPtr = AIL_file_read(m_pcFilename, NULL);
            SetLoaded(true);
            return true;
        }
        break;
    default:
        NIASSERT(false);  // Unknown Type
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMilesSource::Unload()
{
    if (!GetLoaded()) 
        return false;

    // avoid potential problems by releasing handle/stopping sound
    Release();  

    // Only free the memory if it is not used by anyone
    if((m_pvMemPtr != NULL) &&
        NiMilesAudioSystem::GetAudioSystem()->IsUniqueSource(this))
    {
        AIL_mem_free_lock(m_pvMemPtr);
    }

    m_pvMemPtr = NULL;
    SetLoaded(false);
    return true;
}
//---------------------------------------------------------------------------
void* NiMilesSource::GetFileImage()
{
    return m_pvMemPtr;
}
//---------------------------------------------------------------------------
void NiMilesSource::SetLoopCount(int iLoopCount)
{
    NiAudioSource::SetLoopCount(iLoopCount);

    if (GetStatus() != PLAYING)
        return;

    // Adjust stuff if the sound is currently playing
    if (GetStreamed())
        AIL_set_stream_loop_count((HSTREAM)m_pvSource, m_iLoopCount);
    else
        AIL_set_sample_loop_count((HSAMPLE)m_pvSource, m_iLoopCount);

}
//---------------------------------------------------------------------------
bool NiMilesSource::SetConeData(float fAngle1Deg, float fAngle2Deg, 
    float fGain)
{
    NiAudioSource::SetConeData(fAngle1Deg, fAngle2Deg, fGain);

    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return false;

     AIL_set_sample_3D_cone((HSAMPLE)m_pvSource,
        fAngle1Deg, fAngle2Deg, fGain);

    return true;
}
//---------------------------------------------------------------------------
void NiMilesSource::GetConeData(float& fAngle1Deg, float& fAngle2Deg, 
    float& fGain)
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
    {
        NiAudioSource::GetConeData(fAngle1Deg, fAngle2Deg, fGain);
        return;
    }

    AIL_sample_3D_cone((HSAMPLE)m_pvSource,
        &fAngle1Deg, &fAngle2Deg, &fGain);
}
//--------------------------------------------------------------------------- 
bool NiMilesSource::SetMinMaxDistance(float fMin, float fMax)
{
    NiAudioSource::SetMinMaxDistance(fMin, fMax);

    if (!m_pvSource || m_uiInitialType != TYPE_3D)
    {
        return false;
    }

    AIL_set_sample_3D_distances((HSAMPLE)m_pvSource, fMax, fMin, 1);
    
    return true;

}
//---------------------------------------------------------------------------
void NiMilesSource::GetMinMaxDistance(float& fMin, float& fMax)
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
    {
        NiAudioSource::GetMinMaxDistance(fMin, fMax);
        return;
    }

    AIL_sample_3D_distances((HSAMPLE)m_pvSource, &fMax, &fMin, NULL);

    return;
}
//--------------------------------------------------------------------------- 
bool NiMilesSource::SetGain(float fGain)
{
    NiAudioSource::SetGain(fGain);

    if (!m_pvSource)
        return false;

    fGain = NiMax(0.0f, fGain);
    fGain = NiMin(1.0f, fGain);

    if (GetStreamed())
    {
        HSAMPLE kSample = AIL_stream_sample_handle((HSTREAM)m_pvSource);
        AIL_set_sample_volume_levels(kSample, fGain, fGain);
    }
    else
    {
        AIL_set_sample_volume_levels((HSAMPLE)m_pvSource, fGain, fGain); 
    }


    return true;
}
//---------------------------------------------------------------------------
float NiMilesSource::GetGain()
{
    if (!m_pvSource)
    {
        return NiAudioSource::GetGain();
    }

    float fLeftGain;
    float fRightGain;

    if (GetStreamed())
    {
        HSAMPLE kSample = AIL_stream_sample_handle((HSTREAM)m_pvSource);
        AIL_sample_volume_levels(kSample, &fLeftGain, &fRightGain);
    }
    else
    {
        AIL_sample_volume_levels((HSAMPLE)m_pvSource, &fLeftGain,
            &fRightGain); 
    }
    
    // Currently both the left and right sides are set equally
    return fLeftGain;
}
//---------------------------------------------------------------------------
bool NiMilesSource::SetPlaybackRate(long lRate)
{
    NiAudioSource::SetPlaybackRate(lRate);

    if (!m_pvSource)
        return false;

    if (GetStreamed())
    {
        HSAMPLE kSample = AIL_stream_sample_handle((HSTREAM)m_pvSource);
        AIL_set_sample_playback_rate(kSample, (S32)lRate);
    }
    else
    {
        AIL_set_sample_playback_rate((HSAMPLE)m_pvSource, (S32)lRate);
    }

    return true;
}
//---------------------------------------------------------------------------
long NiMilesSource::GetPlaybackRate()
{
    if (!m_pvSource)
        return NiAudioSource::GetPlaybackRate();

    S32 lRate;

    if (GetStreamed())
    {
        HSAMPLE kSample = AIL_stream_sample_handle((HSTREAM)m_pvSource);
        lRate = AIL_sample_playback_rate(kSample);
    }
    else
    {
        lRate = AIL_sample_playback_rate((HSAMPLE)m_pvSource);
    }

    return (long)lRate;
}
//---------------------------------------------------------------------------
void NiMilesSource::SetSamplePreference(char* cPreferenceName, 
    void* PreferenceValue)
{
    if (!GetLoaded() || !m_pvSource)
        return;

    SAMPLESTAGE kSampleStage;

#if (MSS_MAJOR_VERSION == 7 && MSS_MINOR_VERSION == 0 && MSS_SUB_VERSION < 18)
    kSampleStage = DP_OUTPUT;
#else
    kSampleStage = SP_OUTPUT;
#endif

    AIL_sample_stage_property((HSAMPLE)m_pvSource, kSampleStage, 
        cPreferenceName, 0, PreferenceValue, 0);

    return;
}
//---------------------------------------------------------------------------
void NiMilesSource::GetSamplePreference(char* cPreferenceName, 
    void* PreferenceValue)
{
    if (!GetLoaded() || !m_pvSource)
        return;

    SAMPLESTAGE kSampleStage;

#if (MSS_MAJOR_VERSION == 7 && MSS_MINOR_VERSION == 0 && MSS_SUB_VERSION < 18)
    kSampleStage = DP_OUTPUT;
#else
    kSampleStage = SP_OUTPUT;
#endif

    AIL_sample_stage_property ((HSAMPLE)m_pvSource, kSampleStage,
        cPreferenceName, PreferenceValue, 0, 0);

    return;
}
//---------------------------------------------------------------------------
bool NiMilesSource::Play()
{
    if (!GetLoaded())
        return false;

    CreateHandle();

    if (!m_pvSource)
        return false;
    // Set All of our values because we have a handle
    if (GetCone())
        SetConeData(m_fConeAngle1Deg, m_fConeAngle2Deg, m_fConeGain);

    if (m_fMinDistance > 0.0001f)
        SetMinMaxDistance(m_fMinDistance, m_fMaxDistance);

    SetGain(m_fGain);

    if (m_lPlaybackRate > 0)
        SetPlaybackRate(m_lPlaybackRate);

    if (m_fPlayTime > 0.0001f)
        SetPlayTime(m_fPlayTime);

    if (m_uiPlayPosition > 0)
        SetPlayPosition(m_uiPlayPosition);

    if (m_fRoomEffectLevel > 0.0001f)
        SetRoomEffectLevel(m_fRoomEffectLevel);

    if (m_fOcclusionFactor > 0.0001f)
        SetOcclusionFactor(m_fOcclusionFactor);

    if (m_fObstructionFactor > 0.0001f)
        SetObstructionFactor(m_fObstructionFactor);

    if (GetStreamed())
    {
        AIL_set_stream_loop_count((HSTREAM)m_pvSource, m_iLoopCount);
        AIL_start_stream((HSTREAM)m_pvSource);
    }
    else
    {
        if (GetType() == TYPE_3D)
            UpdateWorldTransfrom();

        AIL_set_sample_loop_count((HSAMPLE)m_pvSource, m_iLoopCount);

        // From Miles Audio documentation:
        // If a conventional single-buffered sample is already playing, 
        // it will be restarted from the beginning by this [AIL_start_sample]
        // function. 
        // To begin playback at a specific position, set the position with 
        // AIL_set_sample_position and call AIL_resume_sample instead 
        // of AIL_start_sample. 

        // Ensure the sample starts at the desired position
        if (m_uiPlayPosition > 0)
        {
            AIL_set_sample_position((HSAMPLE)m_pvSource, m_uiPlayPosition);
            AIL_resume_sample((HSAMPLE)m_pvSource);
        }
        else if (m_fPlayTime > 0.0001f)
        {
            // fTime is seconds but Miles wants milliseconds
            AIL_set_sample_ms_position((HSAMPLE)m_pvSource, 
                (S32)(m_fPlayTime * 1000.0f));
            AIL_resume_sample((HSAMPLE)m_pvSource);
        }
        else
        {
            AIL_start_sample((HSAMPLE)m_pvSource);
        }
    }

    NIMETRICS_AUDIO_ADDVALUE(PLAY_SOUND_LOOPS, m_iLoopCount);

    return true;
}
//---------------------------------------------------------------------------
bool NiMilesSource::Stop()
{
    if (!GetLoaded() || !m_pvSource)
        return false;

    if (GetStreamed())
        AIL_pause_stream((HSTREAM)m_pvSource, 1);
    else
        AIL_stop_sample((HSAMPLE)m_pvSource);


    // Release the m_pvSource because we stopped playing
    Release();
    return true;
}
//---------------------------------------------------------------------------
void NiMilesSource::Rewind()
{
    if (!GetLoaded() || !m_pvSource)
        return;


    if (GetStreamed())
        AIL_set_stream_ms_position((HSTREAM)m_pvSource, 0);
    else
        AIL_set_sample_ms_position((HSAMPLE)m_pvSource, 0);

    return;
}
//--------------------------------------------------------------------------- 
NiMilesSource::Status NiMilesSource::GetStatus()
{
    if (!GetLoaded())
        return NOT_SET;


    // Because we are freeing the handles when we are finished
    // we can't remember with the status that we have finish 
    // playing a song. This member will remember when we are
    // done playing a song
    if (!m_pvSource && (m_eDoneStatus == DONE))
        return DONE;
    else if (!m_pvSource)
        return NOT_SET;

    U32 uFlag = SMP_DONE;

    if (GetStreamed())
    {
        uFlag = AIL_stream_status((HSTREAM)m_pvSource);
    }
    else
    {
        uFlag = AIL_sample_status((HSAMPLE)m_pvSource);
    }
   
    switch (uFlag)
    {
    case SMP_DONE:
        return DONE;
    case SMP_PLAYING:
        return PLAYING;
    case SMP_STOPPED:
        return STOPPED;
    case SMP_FREE:
        return FREE;
    case SMP_PLAYINGBUTRELEASED:
        return PLAYINGBUTRELEASED;
    }

    return NOT_SET;
}
//--------------------------------------------------------------------------- 
bool NiMilesSource::SetPlayTime(float fTime)
{
    NiAudioSource::SetPlayTime(fTime);

    if (!GetLoaded() || !m_pvSource)
        return 0;

    // fTime is seconds but Miles wants milliseconds
    if (GetStreamed())
    {
        AIL_set_stream_ms_position((HSTREAM)m_pvSource, 
            (S32)(fTime * 1000.0f));
    }
    else
    {
        AIL_set_sample_ms_position((HSAMPLE)m_pvSource, 
            (S32)(fTime * 1000.0f));
    }

    return true;
}
//---------------------------------------------------------------------------
float NiMilesSource::GetPlayTime() 
{
    if (!GetLoaded() || !m_pvSource)
        return NiAudioSource::GetPlayTime();

    if (GetStreamed()) 
    {
        S32 iVal;
        AIL_stream_ms_position((HSTREAM)m_pvSource,0,&iVal);
        return (float) (iVal/1000.0);
    }
    else
    {
        S32 iVal;
        AIL_sample_ms_position((HSAMPLE)m_pvSource,0,&iVal);
        return (float) (iVal/1000.0);
    }

}
//---------------------------------------------------------------------------
bool NiMilesSource::GetPlayLength(float& fTime)
{
    if (!GetLoaded())
        return false;

    if (GetStreamed())
    {
        // Until we actually start playing we don't know
        // how long the sound is because it is streamed
        if(m_pvSource == 0)
            return false;

        S32 sOffset = 0;
        AIL_stream_ms_position((HSTREAM)m_pvSource, &sOffset, NULL);
        fTime = (float)sOffset / 1000.0f;
    }
    else
    {
        AILSOUNDINFO Info;
        AIL_WAV_info(m_pvMemPtr,&Info); 
            
        fTime =  (float)Info.samples / (float)Info.channels;
        fTime =  fTime / (float)Info.rate;
    }

    return true;
}
//--------------------------------------------------------------------------- 
bool NiMilesSource::SetPlayPosition(unsigned int dwPos)
{
    NiAudioSource::SetPlayPosition(dwPos);

    if (!GetLoaded() || !m_pvSource)
        return false;

    if (GetStreamed())
        AIL_set_stream_position((HSTREAM)m_pvSource, dwPos);
    else
        AIL_set_sample_position((HSAMPLE)m_pvSource, dwPos);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiMilesSource::GetPlayPosition()
{
    if (!GetLoaded() || !m_pvSource)
        return NiAudioSource::GetPlayPosition();

    U32 uiOffset;

    if (GetStreamed())
    {
        uiOffset = AIL_stream_position((HSTREAM)m_pvSource);
    }
    else
    {
        uiOffset = AIL_sample_position((HSAMPLE)m_pvSource);
    }

    return uiOffset;
}
//---------------------------------------------------------------------------
void NiMilesSource::UpdateAudioData(float)
{
    // Release the m_pvSource because we stopped playing
    // With the Miles 6.5c version 2D sounds never return
    // they are done they only return free
    if((GetStatus() == DONE) || (GetStatus() == FREE))
    {
        // Remember we finished playing this sound
        m_eDoneStatus = DONE;
        Release();
    }
    
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return;

    UpdateWorldTransfrom();
}
//---------------------------------------------------------------------------
void NiMilesSource::UpdateWorldTransfrom()
{
    NiPoint3 worldLoc = GetWorldTranslate();
    NiPoint3 worldVel = GetWorldVelocity();

    // Miles wants meters per millisecond.. 
    float fUnitsPerMeter = NiAudioSystem::GetAudioSystem()->GetUnitsPerMeter();

    float fThousandUPM = (float) 1000.0 * fUnitsPerMeter;

    worldVel.x =  worldVel.x / fThousandUPM;
    worldVel.y =  worldVel.y / fThousandUPM;
    worldVel.z = -worldVel.z / fThousandUPM;    // also change from RH->LH
  
    // RH->LH
    worldLoc.z *= -1.0;

    AIL_set_sample_3D_position((HSAMPLE)m_pvSource, 
        worldLoc.x, worldLoc.y, worldLoc.z);

    // velocity needs to be specified as m/ms
    AIL_set_sample_3D_velocity_vector((HSAMPLE)m_pvSource, 
        worldVel.x, worldVel.y, worldVel.z);

    if (GetCone())  
    {
        NiPoint3 worldDir = GetWorldRotate() * m_kDirection;
        NiPoint3 worldUp =  GetWorldRotate() * m_kUp;
        worldDir.z *= -1.0; // RH->LH
        worldUp.z  *= -1.0;

        AIL_set_sample_3D_orientation((HSAMPLE)m_pvSource, 
            worldDir.x, worldDir.y, worldDir.z, worldUp.x, worldUp.y, 
            worldUp.z);
    }
}
//---------------------------------------------------------------------------
bool NiMilesSource::SetRoomEffectLevel(float fLevel)
{
    NiAudioSource::SetRoomEffectLevel(fLevel);

    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return false; 

    AIL_set_sample_reverb_levels((HSAMPLE)m_pvSource, 1.0f, fLevel);
    return true;
}
//---------------------------------------------------------------------------
float NiMilesSource::GetRoomEffectLevel()
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return NiAudioSource::GetRoomEffectLevel(); 

    F32 fLevel;
    AIL_sample_reverb_levels((HSAMPLE)m_pvSource, 0, &fLevel);

    return fLevel;
}
//---------------------------------------------------------------------------
bool NiMilesSource::SetOcclusionFactor(float fLevel)
{
    NiAudioSource::SetOcclusionFactor(fLevel);

    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return false; 

    AIL_set_sample_occlusion((HSAMPLE)m_pvSource, fLevel);
    return true;
}
//---------------------------------------------------------------------------
float NiMilesSource::GetOcclusionFactor()
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return NiAudioSource::GetOcclusionFactor(); 

    F32 fLevel = AIL_sample_occlusion((HSAMPLE)m_pvSource);
    return fLevel;
}
//---------------------------------------------------------------------------
bool NiMilesSource::SetObstructionFactor(float fLevel)
{
    NiAudioSource::SetObstructionFactor(fLevel);

    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return false; 

    AIL_set_sample_obstruction((HSAMPLE)m_pvSource, fLevel);
    return true;
}
//---------------------------------------------------------------------------
float NiMilesSource::GetObstructionFactor()
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return NiAudioSource::GetObstructionFactor(); 

    F32 fLevel = AIL_sample_obstruction((HSAMPLE)m_pvSource);
    return fLevel;
}
//---------------------------------------------------------------------------
NiPoint3 NiMilesSource::GetPosition()
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return NiPoint3::ZERO;

    NiPoint3 pos;
    
    AIL_sample_3D_position((HSAMPLE)m_pvSource, &pos.x, &pos.y, &pos.z);
        
    pos.z *= -1.0;  //LH->RH
    return pos;
}
//---------------------------------------------------------------------------
void NiMilesSource::GetOrientation(NiPoint3& dir, NiPoint3& up)
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return;

    AIL_sample_3D_orientation((HSAMPLE)m_pvSource,
        &dir.x, &dir.y, &dir.z,&up.x, &up.y, &up.z);

    dir.z *= -1.0;  //LH->RH
    up.z  *= -1.0;
    return;
}
//---------------------------------------------------------------------------
NiPoint3 NiMilesSource::GetVelocity()
{
    if (!m_pvSource || m_uiInitialType != TYPE_3D)
        return NiPoint3::ZERO;

    NiPoint3 vel;

    AIL_sample_3D_velocity((HSAMPLE)m_pvSource,&vel.x, &vel.y, &vel.z);

    float fUnitsPerMeter = NiAudioSystem::GetAudioSystem()->GetUnitsPerMeter();

    float fThousandUPM = (float) 1000.0 * fUnitsPerMeter;

    // convert back to units per second
    vel.x *=  fThousandUPM;
    vel.y *=  fThousandUPM;
    vel.z *= -fThousandUPM; // LH->RH

    return vel;
}
//---------------------------------------------------------------------------
void* NiMilesSource::GetHandle()
{ 
    return m_pvSource;
}
//---------------------------------------------------------------------------
void NiMilesSource::CreateHandle()
{
    // If we already have a handle don't create another
    if (m_pvSource)
        return;

    // Can't Create Handles for Sounds not loaded
    if (!GetLoaded())
        return;

    // Attempt to create source in Miles -- will fail if sound system has
    // not been started.  

    switch(GetType())
    {
    case TYPE_3D:
        if(GetStreamed())
        {
            NIASSERT(false);     // Miles does not support streamed 3D sounds
            return;
        }
        
        // new source
        m_pvSource = 
            AIL_allocate_sample_handle(
            (HDIGDRIVER)NiMilesAudioSystem::GetAudioSystem()->
            GetDigitalDriver());

        // Check to see if no handle was created    ie out of system resources
        if(m_pvSource == NULL)
            return;

        if (!AIL_set_sample_file((HSAMPLE)m_pvSource, m_pvMemPtr, 0))
        {
            // Release the Handle because we couldn't set the sample file
            AIL_release_sample_handle((HSAMPLE)m_pvSource);
            m_pvSource = 0;
            return;
        }
        break;

    case TYPE_AMBIENT:
        if(GetStreamed())
        {
            NiMilesAudioSystem* pSound = NiMilesAudioSystem::GetAudioSystem();

            m_pvSource = 
                AIL_open_stream((HDIGDRIVER)pSound->GetDigitalDriver(),
                m_pcFilename, 0);
            if (!m_pvSource)
            {
                NILOG("Failed to create stream %s\n", m_pcFilename);
            }
        }
        else 
        {
            // new source
            m_pvSource = 
                AIL_allocate_sample_handle(
                (HDIGDRIVER)NiMilesAudioSystem::GetAudioSystem()->
                GetDigitalDriver());

            // Check to see if no handle was created ie out of system resources
            if(m_pvSource == NULL)
                return;

            if (!AIL_set_sample_file((HSAMPLE)m_pvSource, m_pvMemPtr, 0))
            {
                // Release the Handle because we couldn't set the sample file
                AIL_release_sample_handle((HSAMPLE)m_pvSource);
                m_pvSource = 0;
                return;
            }
        }
        break;
    default:
        NIASSERT(false);  // Invalid Sound Type
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiObject* NiMilesSource::CreateClone (
    NiCloningProcess& kCloning)
{
    NiMilesSource* pSource = NiNew NiMilesSource;
    NIASSERT(pSource);
    CopyMembers(pSource, kCloning);
    return pSource;
}
//---------------------------------------------------------------------------
void NiMilesSource::CopyMembers(NiMilesSource* pDest,
    NiCloningProcess& kCloning)
{
    NiAudioSource::CopyMembers(pDest, kCloning);

    pDest->m_pvMemPtr = m_pvMemPtr;    
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMilesSource);
//---------------------------------------------------------------------------
void NiMilesSource::LoadBinary (NiStream& kStream)
{
    NiAudioSource::LoadBinary(kStream);

    if ( kStream.GetFileVersion() <= NiStream::GetVersion(20, 2, 1, 8))
    {
        if (m_uiInitialType == TYPE_3D)
        {
            // Provider name is no longer needed. So simply read the data from
            // the file and ignore it.
            char* pcProviderName = 0;
            kStream.LoadCString(pcProviderName);
            NiFree(pcProviderName);
        }
    }
}
//---------------------------------------------------------------------------
void NiMilesSource::LinkObject(NiStream& kStream)
{
    NiAudioSource::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMilesSource::RegisterStreamables(NiStream& kStream)
{
    return NiAudioSource::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiMilesSource::SaveBinary(NiStream& kStream)
{
    NiAudioSource::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiMilesSource::IsEqual(NiObject* pObject)
{
    if (!NiAudioSource::IsEqual(pObject))
        return false;

    NiMilesSource* pSource = (NiMilesSource*)pObject;

    if (m_pvMemPtr != pSource->m_pvMemPtr)
        return false;

    if (m_pvSource != pSource->m_pvSource)
        return false;

    if (m_eDoneStatus != pSource->m_eDoneStatus)
        return false;

    return true;
}
//---------------------------------------------------------------------------
