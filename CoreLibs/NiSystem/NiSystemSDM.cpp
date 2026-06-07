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
#include "NiSystemPCH.h"

#include "NiSystem.h"
#include "NiSystemSDM.h"

#if defined(_WII)
    #include "NiWiiHio2.h"
    #include "NiWiiLockedCache.h"
#endif

//CODEBLOCK(1) - DO NOT DELETE THIS LINE

bool NiSystemSDM::ms_bInitialized = false;

#ifdef NISYSTEM_EXPORT
BOOL APIENTRY DllMain(HANDLE, DWORD ulReason, LPVOID)
{
    switch (ulReason)
    {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#endif

//---------------------------------------------------------------------------
void NiSystemSDM::Init()
{
    NiImplementSDMInitCheck();

//CODEBLOCK(2) - DO NOT DELETE THIS LINE

#ifndef NI_LOGGER_DISABLE
    NiLogger::_SDMInit();
#endif // #ifndef NI_LOGGER_DISABLE

    NiMemManager::_SDMInit();

#if defined(_WII)
    NiWiiLockedCache::_SDMInit();
#endif

    NiSystemDesc::InitSystemDesc();
}
//---------------------------------------------------------------------------
void NiSystemSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

#if defined(_WII)
    NiWiiLockedCache::_SDMShutdown();
#endif
    
    NiSystemDesc::ShutdownSystemDesc();
 
    NiMemManager::_SDMShutdown();

#ifndef NI_LOGGER_DISABLE
    NiLogger::_SDMShutdown();
#endif // #ifndef NI_LOGGER_DISABLE

#if defined(_WII)
    NiWiiHio2::_SDMShutdown();
#endif // #if defined(_WII)
}
//---------------------------------------------------------------------------
