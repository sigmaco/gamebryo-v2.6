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

#include "NiAudioSystem.h"
#include "NiAudioSource.h"
#include <NiSystem.h>
#include <NiFilename.h>

NiImplementRTTI(NiAudioSource,NiAVObject);

//---------------------------------------------------------------------------
NiAudioSource::NiAudioSource(unsigned int uiType)
{
    m_uFlags = 0;

    // Initialize the member data
    Initialize();

    m_uiInitialType = uiType;
    NiAudioSystem::GetAudioSystem()->AddSource(this);   
}
//---------------------------------------------------------------------------
NiAudioSource::~NiAudioSource()
{
    Release();

    NiFree(m_pcFilename);
    NiFree(m_pcLocalName);

    NiAudioSystem::GetAudioSystem()->RemoveSource(this);
}
//--------------------------------------------------------------------------- 
void NiAudioSource::Initialize()
{
    m_uiInitialType = TYPE_DEFAULT;
 
    SetStreamed(false);
    
    // Audio parameters
    m_pcFilename  = 0;
    m_pcLocalName = 0;
    
    m_iLoopCount = LOOP_SINGLE;
    
    // Cone Data
    m_fConeAngle1Deg = 0.0f;
    m_fConeAngle2Deg = 0.0f;
    m_fConeGain = 0.0f;

    m_kDirection = -NiPoint3::UNIT_X;
    m_kUp = NiPoint3::UNIT_Y;
    m_kLocalVelocity = NiPoint3::ZERO;
    
    m_fMinDistance = 0.0f;
    m_fMaxDistance = 0.0f;

    m_fGain = 0.0f;

    m_lPlaybackRate = 0;
    m_fPlayTime = 0.0f;
    m_uiPlayPosition = 0;

    m_fRoomEffectLevel = 0.0f;
    m_fOcclusionFactor = 0.0f;
    m_fObstructionFactor = 0.0f;


    SetAllowSharing(false);
    SetLoaded(false);
    SetCone(false);
    
    // Fade variables
    m_fFadeInitialGain = 0.0f;
    m_fFadeDestinationGain = 0.0f;
    m_fFadeStartTime = NOT_FADING;
    m_fFadeDuration = 0.0f;
}
//--------------------------------------------------------------------------- 
void NiAudioSource::SetFilename(const char* pFilename) 
{
    NIASSERT(pFilename);

    // First check to see if incoming filename is the same as the 
    // existing one.  If they are identical then, assuming the A3dSource 
    // exists, the function can return.
    if (m_pcFilename)
    {
        if (!strcmp(m_pcFilename, pFilename))
        {
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

}
//--------------------------------------------------------------------------- 
bool NiAudioSource::Load() 
{
    // Returns false if data is invalid OR if data is already loaded
    if (!m_pcFilename || GetLoaded())
    {
        NiOutputDebugString("Invalid filename or data already loaded\n");
        return false;
    }

    // return false if SetFilename was not called previously 
    //  unless not a 3D object
    if (!m_pcFilename && m_uiInitialType == TYPE_3D)
    {
        NiOutputDebugString("Must call SetFilename before Load\n");
        return false;
    }

    NiAudioSystem* pkSound = NiAudioSystem::GetAudioSystem();
        
    NiAudioSource* pkDup = NULL;

    if (GetAllowSharing() && !GetStreamed())
    {
        // Check to see if there is already a source with the same filename.
        // If so, duplicate the source rather than create a new one.  
        NiTListIterator iter;
        NiAudioSource* pkSource = pkSound->GetFirstSource(iter);
        while (pkSource)
        {
            if ((this != pkSource) && pkSource->m_pcLocalName && 
                (!strcmp(m_pcLocalName, pkSource->m_pcLocalName)))
             {
                 pkDup = pkSource;
                 break;
             }
             pkSource = pkSound->GetNextSource(iter);
         }
        if (pkDup)
        {
            if (pkDup->GetStreamed())
                pkDup = 0;
        }
     }

    NIMETRICS_AUDIO_ADDTOLOADEDSOURCES(1);

    return true;
}
//---------------------------------------------------------------------------
bool NiAudioSource::Unload()
{
    if (!GetLoaded()) 
        return false;

    // avoid potential problems by releasing handle/stopping sound
    Release();

    NIMETRICS_AUDIO_ADDTOLOADEDSOURCES(-1);

    return true;
}
//---------------------------------------------------------------------------
void NiAudioSource::Release()
{
}
//---------------------------------------------------------------------------
bool NiAudioSource::FadeToGain(float fGain, float fStartTime, 
    float fDuration)
{
    m_fFadeInitialGain = GetGain();
    m_fFadeDestinationGain = fGain;
    m_fFadeStartTime = fStartTime;
    m_fFadeDuration = fDuration;
    return true;
}
//---------------------------------------------------------------------------
void NiAudioSource::Update(float fTime)
{
    NiAVObject::Update(fTime);

    UpdateAudioData(fTime);
}
//---------------------------------------------------------------------------
void NiAudioSource::UpdateAudioData(float fTime)
{
    // Handle the gain when fading
    if ((m_fFadeStartTime != NOT_FADING) && (fTime >= m_fFadeStartTime))
    {
        if (fTime <= (m_fFadeStartTime + m_fFadeDuration))
        {
            float fPercentComplete = 
                (fTime - m_fFadeStartTime) / m_fFadeDuration;
            SetGain(m_fFadeInitialGain + 
                ((m_fFadeDestinationGain - m_fFadeInitialGain) * 
                 fPercentComplete));
        }
        else
        {
            SetGain(m_fFadeDestinationGain);
            
            // Clear fading time values
            m_fFadeStartTime = NOT_FADING;
            m_fFadeDuration = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
void NiAudioSource::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateDownwardPass(kUpdate);
    UpdateAudioData(kUpdate.GetTime());
}
//---------------------------------------------------------------------------
void NiAudioSource::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateSelectedDownwardPass(kUpdate);
    UpdateAudioData(kUpdate.GetTime());
}
//---------------------------------------------------------------------------
void NiAudioSource::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    NiAVObject::UpdateRigidDownwardPass(kUpdate);
    UpdateAudioData(kUpdate.GetTime());
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetConeData(float fAngle1Deg, float fAngle2Deg, 
    float fGain)
{
    // Set the internal cone variables
    m_fConeAngle1Deg = fAngle1Deg;
    m_fConeAngle2Deg = fAngle2Deg;
    m_fConeGain = fGain;
    SetCone(true);
    return true;
}
//---------------------------------------------------------------------------
void NiAudioSource::GetConeData(float& fAngle1Deg, float& fAngle2Deg, 
    float& fGain)
{
    // Get the internal cone variables
    fAngle1Deg = m_fConeAngle1Deg;
    fAngle2Deg = m_fConeAngle1Deg;
    fGain = m_fConeGain;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetMinMaxDistance(float fMin, float fMax)
{
    // Set the Minimum and maximum distance of a sound. Sounds at or inside
    // the minimum distance will be a full volume
    m_fMinDistance = fMin;
    m_fMaxDistance = fMax;
    return true;
}
//---------------------------------------------------------------------------
void NiAudioSource::GetMinMaxDistance(float& fMin, float& fMax)
{
    fMin = m_fMinDistance;
    fMax = m_fMaxDistance;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetGain(float fGain)
{
    m_fGain = fGain;
    return true;
}
//---------------------------------------------------------------------------
float NiAudioSource::GetGain()
{
    return m_fGain;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetPlaybackRate(long lRate)
{
    m_lPlaybackRate = lRate;
    return true;
}
//---------------------------------------------------------------------------
long NiAudioSource::GetPlaybackRate()
{
    return m_lPlaybackRate;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetPlayTime(float fTime)
{
    m_fPlayTime = fTime;
    return true;    
}
//---------------------------------------------------------------------------
float NiAudioSource::GetPlayTime()
{
    return m_fPlayTime;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetPlayPosition(unsigned int Pos)
{
    m_uiPlayPosition = Pos;
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiAudioSource::GetPlayPosition()
{
    return m_uiPlayPosition;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetRoomEffectLevel(float fLevel)
{
    m_fRoomEffectLevel = fLevel;
    return true;
}
//---------------------------------------------------------------------------
float NiAudioSource::GetRoomEffectLevel()
{
    return m_fRoomEffectLevel;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetOcclusionFactor(float fLevel)
{
    m_fOcclusionFactor = fLevel;
    return true;
}
//---------------------------------------------------------------------------
float NiAudioSource::GetOcclusionFactor()
{
    return m_fOcclusionFactor;
}
//---------------------------------------------------------------------------
bool NiAudioSource::SetObstructionFactor(float fLevel)
{
    m_fObstructionFactor = fLevel;
    return true;
}
//---------------------------------------------------------------------------
float NiAudioSource::GetObstructionFactor()
{
    return m_fObstructionFactor;
}
//---------------------------------------------------------------------------
void NiAudioSource::SetVelocity(NiPoint3& kVelocity)
{
    m_kLocalVelocity = kVelocity;
}
//---------------------------------------------------------------------------
NiPoint3 NiAudioSource::GetVelocity()
{
    return m_kLocalVelocity;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiAudioSource::CopyMembers(NiAudioSource* pkDest,
    NiCloningProcess& kCloning)
{
    NiAVObject::CopyMembers(pkDest, kCloning);

    // Don't need to copy m_uFlags

    //
    // Note: pkDest is not loaded by CopyMembers
    // Also, when the source is loaded, Gamebryo will identify
    // a duplicate filename with an existing source and will try
    // to Sharing the source
    //
    pkDest->m_uiInitialType = m_uiInitialType;
    pkDest->SetFilename(m_pcFilename);

    pkDest->SetStreamed(GetStreamed());
    pkDest->SetAllowSharing(GetAllowSharing());
    pkDest->SetLoaded(GetLoaded());

    // Copy cone variables
    pkDest->m_kDirection = m_kDirection;
    pkDest->m_kUp = m_kUp;

    float fVal1;
    float fVal2;
    float fVal3;

    if (GetCone())
    {
        GetConeData(fVal1, fVal2, fVal3);
        pkDest->SetConeData(fVal1, fVal2, fVal3);
    }

    GetMinMaxDistance(fVal1, fVal2);
    pkDest->SetMinMaxDistance(fVal1, fVal2);
    pkDest->SetGain(GetGain());

    // Copy the playback variables
    pkDest->m_iLoopCount = m_iLoopCount;
    pkDest->m_lPlaybackRate = m_lPlaybackRate;
    pkDest->m_fPlayTime = m_fPlayTime;
    pkDest->m_uiPlayPosition = m_uiPlayPosition;

    // Copy the Reverb settings
    pkDest->m_fRoomEffectLevel = m_fRoomEffectLevel;
    pkDest->m_fOcclusionFactor = m_fOcclusionFactor;
    pkDest->m_fObstructionFactor = m_fObstructionFactor;


    pkDest->m_fFadeInitialGain = m_fFadeInitialGain;
    pkDest->m_fFadeDestinationGain = m_fFadeDestinationGain;
    pkDest->m_fFadeStartTime = m_fFadeStartTime;
    pkDest->m_fFadeDuration = m_fFadeDuration;

    pkDest->SetVelocity(m_kLocalVelocity);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiObject* NiAudioSource::CreateObject(const char**,
    unsigned int)
{
// MSS
//    return new NiAudioSource;
    return NULL;
}
//---------------------------------------------------------------------------
void NiAudioSource::LoadBinary (NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    NiStreamLoadBinary(kStream, m_uiInitialType);
    char* pcName = 0;
    kStream.LoadCString(pcName);

    SetFilename(pcName);
    
    NiFree(pcName);

    bool bVal;
    NiStreamLoadBinary(kStream, bVal);
    SetStreamed(bVal);
    NiStreamLoadBinary(kStream, bVal);
    SetAllowSharing(bVal);

    
    m_kDirection.LoadBinary(kStream);
    m_kUp.LoadBinary(kStream);
    float fVal1;
    float fVal2;
    float fVal3;
    NiStreamLoadBinary(kStream, fVal1);
    NiStreamLoadBinary(kStream, fVal2);
    NiStreamLoadBinary(kStream, fVal3);

    if (GetCone())
        SetConeData(fVal1, fVal2, fVal3);

    NiStreamLoadBinary(kStream, fVal1);
    NiStreamLoadBinary(kStream, fVal2);
    SetMinMaxDistance(fVal1, fVal2);
    NiStreamLoadBinary(kStream, fVal1);
    SetGain(fVal1);

    NiStreamLoadBinary(kStream, m_iLoopCount);
    NiStreamLoadBinary(kStream, m_lPlaybackRate);
    NiStreamLoadBinary(kStream, m_fPlayTime);
    NiStreamLoadBinary(kStream, m_uiPlayPosition);

    NiStreamLoadBinary(kStream, m_fRoomEffectLevel);
    NiStreamLoadBinary(kStream, m_fOcclusionFactor);
    NiStreamLoadBinary(kStream, m_fObstructionFactor);

    NiStreamLoadBinary(kStream, m_fFadeInitialGain);
    NiStreamLoadBinary(kStream, m_fFadeDestinationGain);
    NiStreamLoadBinary(kStream, m_fFadeStartTime);
    NiStreamLoadBinary(kStream, m_fFadeDuration);

    NiStreamLoadBinary(kStream, m_kLocalVelocity);

}
//---------------------------------------------------------------------------
void NiAudioSource::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiAudioSource::RegisterStreamables(NiStream& kStream)
{
    return NiAVObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiAudioSource::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    NiStreamSaveBinary(kStream, m_uiInitialType);
    kStream.SaveCString(m_pcFilename);

    NiStreamSaveBinary(kStream, GetStreamed());
    NiStreamSaveBinary(kStream, GetAllowSharing());

    m_kDirection.SaveBinary(kStream);
    m_kUp.SaveBinary(kStream);
    float fVal1;
    float fVal2;
    float fVal3;
    GetConeData(fVal1, fVal2, fVal3);
    NiStreamSaveBinary(kStream, fVal1);
    NiStreamSaveBinary(kStream, fVal2);
    NiStreamSaveBinary(kStream, fVal3);

    GetMinMaxDistance(fVal1, fVal2);
    NiStreamSaveBinary(kStream, fVal1);
    NiStreamSaveBinary(kStream, fVal2);
    NiStreamSaveBinary(kStream, GetGain());

    NiStreamSaveBinary(kStream, m_iLoopCount);
    NiStreamSaveBinary(kStream, m_lPlaybackRate);
    NiStreamSaveBinary(kStream, m_fPlayTime);
    NiStreamSaveBinary(kStream, m_uiPlayPosition);

    NiStreamSaveBinary(kStream, m_fRoomEffectLevel);
    NiStreamSaveBinary(kStream, m_fOcclusionFactor);
    NiStreamSaveBinary(kStream, m_fObstructionFactor);

    NiStreamSaveBinary(kStream, m_fFadeInitialGain);
    NiStreamSaveBinary(kStream, m_fFadeDestinationGain);
    NiStreamSaveBinary(kStream, m_fFadeStartTime);
    NiStreamSaveBinary(kStream, m_fFadeDuration);

    NiStreamSaveBinary(kStream, m_kLocalVelocity);
}
//---------------------------------------------------------------------------
bool NiAudioSource::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;

    NiAudioSource* pkSource = (NiAudioSource*)pkObject;

    if (m_uiInitialType != pkSource->m_uiInitialType)
        return false;

    if (strcmp(m_pcFilename, pkSource->m_pcFilename))
        return false;

    if (GetStreamed() != pkSource->GetStreamed())
        return false;

    if (GetAllowSharing() != pkSource->GetAllowSharing())
        return false;

    if (GetLoaded() != pkSource->GetLoaded())
        return false;

    if (m_kDirection != pkSource->m_kDirection ||
        m_kUp != pkSource->m_kUp ||
        GetLoopCount() != pkSource->GetLoopCount())
    {
        return false;
    }


    if (GetCone() != pkSource->GetCone())
        return false;

    float fVal01;
    float fVal02;
    float fVal03;
    float fVal11;
    float fVal12;
    float fVal13;

    if (GetCone())
    {
        GetConeData(fVal01, fVal02, fVal03);
        pkSource->GetConeData(fVal11, fVal12, fVal13);
        
        if (!CloseTo(fVal01, fVal11, fVal01 * 0.001f) ||
            !CloseTo(fVal02, fVal12, fVal02 * 0.001f) ||
            !CloseTo(fVal03, fVal13, fVal03 * 0.001f))
        {
            return false;
        }
    }

    GetMinMaxDistance(fVal01, fVal02);
    pkSource->GetMinMaxDistance(fVal11, fVal12);
    if (!CloseTo(fVal01, fVal11, fVal01 * 0.001f) ||
        !CloseTo(fVal02, fVal12, fVal02 * 0.001f))
    {
        return false;
    }

    if (!CloseTo(GetGain(), pkSource->GetGain(), GetGain() * 0.001f))
    {
        return false;
    }

    // Check the Play variables
    if (m_iLoopCount != pkSource->m_iLoopCount)
        return false;

    if (m_lPlaybackRate != pkSource->m_lPlaybackRate)
        return false;

    if (!CloseTo(m_fPlayTime, pkSource->m_fPlayTime, m_fPlayTime * 0.001f))
        return false;

    if (m_uiPlayPosition != pkSource->m_uiPlayPosition)
        return false;

    // Check the Reverb variables
    if (!CloseTo(m_fRoomEffectLevel, pkSource->m_fRoomEffectLevel, 
        m_fRoomEffectLevel * 0.001f))
        return false;

    if (!CloseTo(m_fOcclusionFactor, pkSource->m_fOcclusionFactor, 
        m_fOcclusionFactor * 0.001f))
        return false;

    if (!CloseTo(m_fObstructionFactor, pkSource->m_fObstructionFactor, 
        m_fObstructionFactor * 0.001f))
        return false;

    // Check the fade variables
    if (!CloseTo(m_fFadeInitialGain, pkSource->m_fFadeInitialGain, 
        m_fFadeInitialGain * 0.001f))
        return false;

    if (!CloseTo(m_fFadeDestinationGain, pkSource->m_fFadeDestinationGain, 
        m_fFadeDestinationGain * 0.001f))
        return false;

    if (!CloseTo(m_fFadeStartTime, pkSource->m_fFadeStartTime, 
        m_fFadeStartTime * 0.001f))
        return false;

    if (!CloseTo(m_fFadeDuration, pkSource->m_fFadeDuration, 
        m_fFadeDuration * 0.001f))
        return false;

    // Check the Velocity
    if (!CloseTo(m_kLocalVelocity.x, pkSource->GetVelocity().x, 
        m_kLocalVelocity.x * 0.001f))
        return false;

    if (!CloseTo(m_kLocalVelocity.y, pkSource->GetVelocity().y, 
        m_kLocalVelocity.y * 0.001f))
        return false;

    if (!CloseTo(m_kLocalVelocity.z, pkSource->GetVelocity().z, 
        m_kLocalVelocity.z * 0.001f))
        return false;

    return true;
}
//---------------------------------------------------------------------------
const char* NiAudioSource::GetFilename() 
{ 
    return m_pcFilename; 
}
//---------------------------------------------------------------------------
const char* NiAudioSource::GetLocalName()
{
    return m_pcLocalName;
}
//---------------------------------------------------------------------------
void NiAudioSource::SetDirectionVector(const NiPoint3& kDir)
{
    m_kDirection = kDir;
}
//---------------------------------------------------------------------------
const NiPoint3& NiAudioSource::GetDirectionVector() const
{
    return m_kDirection;
}
//---------------------------------------------------------------------------
void NiAudioSource::SetUpVector(const NiPoint3& kUp)
{
    m_kUp = kUp;
}
//---------------------------------------------------------------------------
const NiPoint3& NiAudioSource::GetUpVector() const
{
    return m_kUp;
}
//---------------------------------------------------------------------------
void NiAudioSource::SetLoopCount(int iLoopCount)
{
    m_iLoopCount = iLoopCount;
}
//---------------------------------------------------------------------------
int NiAudioSource::GetLoopCount()
{
    return m_iLoopCount;
}
//---------------------------------------------------------------------------
void NiAudioSource::SetLoaded(bool bLoaded) 
{
    SetBit(bLoaded, LOADED_MASK);
}
//---------------------------------------------------------------------------
bool NiAudioSource::GetLoaded() const
{ 
    return GetBit(LOADED_MASK);
}
//---------------------------------------------------------------------------
void NiAudioSource::SetCone(bool bCone) 
{
    SetBit(bCone, CONE_MASK);
}
//---------------------------------------------------------------------------
bool NiAudioSource::GetCone() const
{ 
    return GetBit(CONE_MASK);
}
//---------------------------------------------------------------------------
void NiAudioSource::SetStreamed(bool bStream) 
{
    SetBit(bStream, STREAM_MASK);
}
//---------------------------------------------------------------------------
bool NiAudioSource::GetStreamed() const
{ 
    return GetBit(STREAM_MASK);
}
//---------------------------------------------------------------------------
void NiAudioSource::SetAllowSharing(bool bAllowSharing) 
{
    SetBit(bAllowSharing, ALLOW_SHARING_MASK);
}
//---------------------------------------------------------------------------
bool NiAudioSource::GetAllowSharing() const
{ 
    return GetBit(ALLOW_SHARING_MASK);
}
//---------------------------------------------------------------------------
unsigned int NiAudioSource::GetType()
{ 
    return m_uiInitialType;
}
//---------------------------------------------------------------------------
bool NiAudioSource::CloseTo(float fA, float fB, float fEpsilon)
{
    // Determines if float fA is within fEpsilon of float fB.

    // Returns: true, if fA is within fEpsilon of fB.
    //          false, otherwise.

    return ((float) fabs(fA - fB) <= fabs(fEpsilon));
}
//---------------------------------------------------------------------------
