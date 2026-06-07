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

#include "NiMilesAudio.h"
#include "NiMilesAudioSDM.h"

NiImplementSDMConstructor(NiMilesAudio, "NiMain");

#ifdef NIAUDIO_EXPORT
NiImplementDllMain(NiMilesAudio);
#endif

//---------------------------------------------------------------------------
void NiMilesAudioSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiAudioListener);
    NiRegisterStream(NiAudioSource);
    NiRegisterStream(NiAudioSystem);
    NiRegisterStream(NiMilesListener);
    NiRegisterStream(NiMilesSource);
    NiRegisterStream(NiMilesAudioSystem);

    NiAudioSystem::ms_pAudioSystem = (NiAudioSystem*)NiNew NiMilesAudioSystem;

    NIASSERT(NiAudioSystem::ms_pAudioSystem);
}
//---------------------------------------------------------------------------
void NiMilesAudioSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiAudioListener);
    NiUnregisterStream(NiAudioSource);
    NiUnregisterStream(NiAudioSystem);
    NiUnregisterStream(NiMilesListener);
    NiUnregisterStream(NiMilesSource);
    NiUnregisterStream(NiMilesAudioSystem);

    NiDelete NiAudioSystem::ms_pAudioSystem;
    NiAudioSystem::ms_pAudioSystem = NULL;

}
//---------------------------------------------------------------------------
