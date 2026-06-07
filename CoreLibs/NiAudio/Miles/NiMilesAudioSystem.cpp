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

#include <NiSystem.h>
#include <NiVersion.h>
#include "NiMilesListener.h"
#include "NiMilesAudioSystem.h"
#include "NiMilesSource.h"
#include <NiBool.h>
#include <NiMaterialProperty.h>
#include <NiNode.h>
#include <Mss.h>

#ifdef _XENON
#include <xaudio.h>
#endif
#ifdef _PS3
#include <cell/audio.h>
#include <sysutil/sysutil_sysparam.h>
#endif
#ifdef _WII
#include <revolution/mix.h>
#endif

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiAudio);
//---------------------------------------------------------------------------

NiImplementRTTI(NiMilesAudioSystem, NiAudioSystem);

//---------------------------------------------------------------------------
NiMilesAudioSystem* NiMilesAudioSystem::GetAudioSystem()
{
    return (NiMilesAudioSystem*)ms_pAudioSystem;
}
//---------------------------------------------------------------------------
NiMilesAudioSystem::NiMilesAudioSystem() : 
    m_uFlags(0),
    m_kWnd(0), 
    m_pDIG(0)
{
    m_fUnitsPerMeter = 1.0f;     // set units to meters
    m_spListener = NiNew NiMilesListener;
    NIASSERT(m_spListener);
    NIASSERT(m_pSources);
}
//---------------------------------------------------------------------------
NiMilesAudioSystem::~NiMilesAudioSystem()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::SetHWnd(NiWindowRef kWnd)
{
    m_kWnd = kWnd;
}
//---------------------------------------------------------------------------
NiWindowRef NiMilesAudioSystem::GetHWnd()
{
    return m_kWnd;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::Startup(const char* pcDirectoryname)
{
#if defined(_XENON)
    // By Default only XMADec and MP3Dec are registered to Miles. If other RIBs
    // are desired, such as OggDec, Voice, SRS, DolbySurround, or DSP, register
    // them here. For each RIB be sure that the appropriate library to is in 
    // the application's dependency list. See the Miles documentation for more
    // information.
    Register_RIB(XMADec);
    Register_RIB(MP3Dec);

    XAUDIOENGINEINIT EngineInit = { 0 };
    EngineInit.pEffectTable = &XAudioDefaultEffectTable;
    XAudioInitialize( &EngineInit );

#elif defined(_PS3)
    // By Default only MP3Dec is registered to Miles. If other RIBs
    // are desired, such as OggDec, Voice, SRS, DolbySurround, or DSP, register
    // them here. For each RIB be sure that the appropriate library to is in 
    // the application's dependency list. See the Miles documentation for more
    // information.
    Register_RIB(MP3Dec);

    //
    // initialize libAudio
    //
    int iReturn = cellAudioInit();
    if (iReturn != CELL_OK &&
        iReturn != CELL_AUDIO_ERROR_ALREADY_INIT)
    {
        return false;
    }

    int iNumSpeakers;
    CellAudioOutConfiguration a_config;
    int i;
    int dev;
    CellAudioOutDeviceInfo    a_info;
    (void)memset(&a_config, 0, sizeof(CellAudioOutConfiguration));
    (void)memset(&a_info, 0, sizeof(CellAudioOutDeviceInfo));

    a_config.channel = 2;  // at least 2ch audio is always available

    // check the number of audio device 
    dev = cellAudioOutGetNumberOfDevice(CELL_AUDIO_OUT_PRIMARY);
    for (i = 0; i < dev; i ++)
    {
        // get the device information 
        iReturn = 
            cellAudioOutGetDeviceInfo(CELL_AUDIO_OUT_PRIMARY, i, &a_info);
        if (iReturn == CELL_OK)
        {
            if (a_info.portType == CELL_AUDIO_OUT_PORT_HDMI)
            {
                int j;
                for (j = 0; j < 16; j ++)
                {
                    // Game environment: using Linear PCM 48kHz
                    if ((a_info.availableModes [j].type == 
                        CELL_AUDIO_OUT_CODING_TYPE_LPCM) &&
                        (a_info.availableModes [j].fs & 
                        CELL_AUDIO_OUT_FS_48KHZ))
                    {
                        if (a_config.channel < 
                            a_info.availableModes[j].channel)
                        {
                            a_config.channel = 
                                a_info.availableModes[j].channel;
                        }
                    }
                }
            }
        }
    }

    a_config.downMixer = CELL_AUDIO_OUT_DOWNMIXER_NONE;

    iReturn = 
        cellAudioOutConfigure(CELL_AUDIO_OUT_PRIMARY, &a_config, NULL, 0);
    if (iReturn != CELL_OK)
    {
        iNumSpeakers = 2;
    }

    iNumSpeakers = a_config.channel;

    AIL_mem_use_malloc(NiMallocProxy);
    AIL_mem_use_free(NiFreeProxy);

    AIL_startup();

    m_pDIG = AIL_open_digital_driver( 48000, 16, iNumSpeakers, 0 );
#elif defined(_WII)
    // By Default only MP3Dec is registered to Miles. If other RIBs
    // are desired, such as OggDec, Voice, SRS, DolbySurround, or DSP, register
    // them here. For each RIB be sure that the appropriate library to is in 
    // the application's dependency list. See the Miles documentation for more
    // information.
    Register_RIB(MP3Dec);

    AIInit(0);
    AXInit();  
    MIXInit();
    AXSetCompressor(AX_COMPRESSOR_OFF);

    AIL_mem_use_malloc(NiMallocProxy);
    AIL_mem_use_free(NiFreeProxy);

    AIL_startup();

    // Wii hardware is 32028
    m_pDIG = AIL_open_digital_driver(32028, 16, 2, 0);
#else
    // Set Miles redistribution directory.  A redistribution directory 
    // is not needed for 360, PS3, or Wii because the libraries are 
    // statically linked.

    AIL_set_redist_directory(pcDirectoryname);
#endif

#if !defined(_PS3) && !defined(_WII)
    AIL_mem_use_malloc(NiMallocProxy);
    AIL_mem_use_free(NiFreeProxy);

    if (!AIL_quick_startup(1, 0, 44100, 16, MSS_MC_USE_SYSTEM_CONFIG))
        return false;    

    AIL_quick_handles((HDIGDRIVER*)&m_pDIG, NULL, NULL);
#endif
    
    return true;
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::Shutdown() 
{
    // Unload all of the Sources    
    if (m_pSources)
    {
        //
        // Any sources that have already released themselves will have been
        // removed from this list.
        //
        NiTListIterator pos = m_pSources->GetHeadPos();
        while (pos)
        {
            NiMilesSource* pSource = (NiMilesSource*)m_pSources->GetNext(pos);
            pSource->Unload();
        }

        m_pSources->RemoveAll();
    }

    //
    // Release and decrement/destroy the listener
    //
    if (m_spListener)
        NiSmartPointerCast(NiMilesListener,m_spListener)->Release();

    AIL_close_digital_driver((HDIGDRIVER)m_pDIG);

    AIL_quick_shutdown();
    AIL_shutdown();

#ifdef _XENON
     XAudioShutDown();
#endif

#ifdef _PS3
     cellAudioQuit();
#endif

#ifdef _WII
    // Nothing to do.
#endif
}
//---------------------------------------------------------------------------
NiMilesListener* NiMilesAudioSystem::GetListener()
{
    // Overload to cast to the correct type
    return (NiMilesListener*)NiAudioSystem::GetListener();
}
//---------------------------------------------------------------------------
NiAudioSource* NiMilesAudioSystem::CreateSource(
    unsigned int uiType /* = NiAudioSource::TYPE_DEFAULT*/)
{
    return (NiAudioSource*)NiNew NiMilesSource(uiType);
}
//---------------------------------------------------------------------------
NiAudioSystem::SpeakerType NiMilesAudioSystem::GetSpeakerType()
{
    // As it is most likely that the speaker type that was used when miles 
    // started up was MSS_MC_USE_SYSTEM_CONFIG (end user config via Windows 
    // control panel) we will return the lowest possible configuration as we
    // can not be certain of the actual configuration being used.

    // Miles Doc: 'It is best not to rely solely on the OS to report a 
    // meaningful speaker-configuration value; we strongly recommend offering 
    // the user the ability to select an MSS_MC_ configuration directly as 
    // part of your application's sound-options dialog. So, default to 
    // MSS_MS_USE_SYSTEM_CONFIG, but give the user the ability to override.'

    return NiAudioSystem::TYPE_3D_2_SPEAKER;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::SetSpeakerType(unsigned int)
{
    // The speaker type cannot be adjusted after miles has been started.
    // To specify a different speaker type other than the default when calling
    // AIL_quick_startup or AIL_open_digital_driver.

    // Miles Doc: 'It is best not to rely solely on the OS to report a 
    // meaningful speaker-configuration value; we strongly recommend offering 
    // the user the ability to select an MSS_MC_ configuration directly as 
    // part of your application's sound-options dialog. So, default to 
    // MSS_MS_USE_SYSTEM_CONFIG, but give the user the ability to override.' 

    return false;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::SetBestSpeakerTypeAvailable()
{
    // The speaker type cannot be adjusted after miles has been started.
    // To specify a different speaker type other than the default when calling
    // AIL_quick_startup or AIL_open_digital_driver.

    // Miles Doc: 'It is best not to rely solely on the OS to report a 
    // meaningful speaker-configuration value; we strongly recommend offering 
    // the user the ability to select an MSS_MC_ configuration directly as 
    // part of your application's sound-options dialog. So, default to 
    // MSS_MS_USE_SYSTEM_CONFIG, but give the user the ability to override.'

    return false;
}
//---------------------------------------------------------------------------
char* NiMilesAudioSystem::GetLastError() 
{
    char* ptemp = AIL_last_error();
    return ptemp;  
}
//---------------------------------------------------------------------------  
NiMilesSource* NiMilesAudioSystem::GetFirstSource(NiTListIterator& iter)
{
    // Overload to cast directly to the correctly type
    return (NiMilesSource*)NiAudioSystem::GetFirstSource(iter);
}
//---------------------------------------------------------------------------  
NiMilesSource* NiMilesAudioSystem::GetNextSource(NiTListIterator& iter)
{
    // Overload to cast directly to the correctly type
    return (NiMilesSource*)NiAudioSystem::GetNextSource(iter);
}
//---------------------------------------------------------------------------
NiMilesSource* NiMilesAudioSystem::FindDuplicateSource(
    NiAudioSource* pkOriginal)
{
    // Check to see if there is already a source with the same filename.
    // If so, duplicate the source rather than create a new one.  
    // Unlike the base class Miles Duplicates need to be of the same type
    NiTListIterator iter;
    NiAudioSource* pkSource = GetFirstSource(iter);

    while (pkSource)
    {
        if ((pkOriginal != pkSource) && pkSource->GetAllowSharing() &&
            (pkSource->GetType() == pkOriginal->GetType()) &&
            pkSource->GetLocalName() && 
            (!strcmp(pkOriginal->GetLocalName(), pkSource->GetLocalName())))
        {
            return (NiMilesSource*)pkSource;
        }
        pkSource = GetNextSource(iter);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::IsUniqueSource(NiMilesSource* pkOriginal)
{
    // Check to see if there is a source which uses the same File Image
    NiTListIterator iter;
    NiMilesSource* pkSource = (NiMilesSource*)GetFirstSource(iter);

    while (pkSource)
    {
        if ((pkOriginal != pkSource) && 
            pkOriginal->GetFileImage() == pkSource->GetFileImage())
        {
            return false;
        }
        pkSource = (NiMilesSource*)GetNextSource(iter);
    }

    return true;
}
//---------------------------------------------------------------------------  
bool NiMilesAudioSystem::SetUnitsPerMeter(float fUnits)
{
    if (!NiAudioSystem::SetUnitsPerMeter(fUnits))
        return false;

    AIL_set_3D_distance_factor((HDIGDRIVER)m_pDIG, 1.0f / fUnits);

    return true;
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::SetDriverProperty(char* cPreferenceName, 
    void* PreferenceValue)
{
    AIL_output_filter_driver_property((HDIGDRIVER)m_pDIG, cPreferenceName,
        0, PreferenceValue, 0);
}
//---------------------------------------------------------------------------  
void NiMilesAudioSystem::GetDriverProperty(char* cPreferenceName, 
    void* PreferenceValue)
{
    AIL_output_filter_driver_property((HDIGDRIVER)m_pDIG, cPreferenceName,
        PreferenceValue, 0, 0);
}
//---------------------------------------------------------------------------  
bool NiMilesAudioSystem::GetReverbAvailable()
{
    return (AIL_room_type((HDIGDRIVER)m_pDIG) != -1);
}
//---------------------------------------------------------------------------  
bool NiMilesAudioSystem::SetCurrentRoomReverb(unsigned int dwPreset)
{
    if (!GetReverbAvailable())
        return false;

    // Set current reverb
    AIL_set_room_type((HDIGDRIVER)m_pDIG, dwPreset);

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiMilesAudioSystem::GetCurrentRoomReverb()
{
    // Get current reverb
    return AIL_room_type((HDIGDRIVER)m_pDIG);
}
//---------------------------------------------------------------------------
void* NiMilesAudioSystem::GetDigitalDriver()
{
    return m_pDIG;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiObject* NiMilesAudioSystem::CreateObject(const char**,
    unsigned int)
{
    return NiMilesAudioSystem::GetAudioSystem();
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::LoadBinary (NiStream& stream)
{
    NiObject::LoadBinary(stream);
    float fVal1;
    NiStreamLoadBinary(stream, fVal1);
    SetUnitsPerMeter(fVal1);
    NiStreamLoadBinary(stream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::RegisterStreamables(NiStream& stream)
{
    if (!NiObject::RegisterStreamables(stream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);
    NiStreamSaveBinary(stream, m_fUnitsPerMeter);
    NiStreamSaveBinary(stream, m_uFlags);
}
//---------------------------------------------------------------------------
bool NiMilesAudioSystem::IsEqual(NiObject* pkObject)
{
    if (!NiAudioSystem::IsEqual(pkObject))
        return false;

    NiMilesAudioSystem* pkMiles = (NiMilesAudioSystem*)pkObject;

    // Compare members 
    if (m_kWnd != pkMiles->m_kWnd)
        return false;

    if (m_pDIG != pkMiles->m_pDIG)
        return false;

    if (m_uFlags != pkMiles->m_uFlags)
        return false;

     return true;
}
//---------------------------------------------------------------------------
const unsigned int NiMilesAudioSystem::GetNumberActiveSamples()
{
    return AIL_active_sample_count((HDIGDRIVER)m_pDIG);
}
//---------------------------------------------------------------------------
void* NiMilesAudioSystem::NiMallocProxy(UINTa uiSizeInBytes)
{
    return NiAlignedMalloc2(uiSizeInBytes, 16, NiMemHint::IS_BULK | NiMemHint::USAGE_AUDIO);
}
//---------------------------------------------------------------------------
void NiMilesAudioSystem::NiFreeProxy(void* pvMem)
{
    NiAlignedFree(pvMem);
}
//---------------------------------------------------------------------------
