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

#ifndef NISDMMACROS_H
#define NISDMMACROS_H

#define NiDeclareSDM(libname, entrytag) \
    class entrytag libname##SDM \
    { \
    public: \
        libname##SDM(); \
        static void Init(); \
        static void Shutdown(); \
    protected: \
        static bool ms_bInitialized; \
    }; 

#define NiImplementSDMConstructor(libname, dependencies) \
    bool libname##SDM::ms_bInitialized = false; \
    libname##SDM::libname##SDM() \
    { \
        static int siCounter = 0; \
        if (siCounter++ == 0) \
        { \
            NiStaticDataManager::AddLibrary(#libname, \
                libname##SDM::Init, libname##SDM::Shutdown, dependencies); \
        } \
    }

#define NiImplementSDMInitCheck() \
    if (!ms_bInitialized) \
    { \
        ms_bInitialized = true; \
    } \
    else \
    { \
        return; \
    }

#define NiImplementSDMShutdownCheck() \
    if (ms_bInitialized) \
    { \
        ms_bInitialized = false; \
    } \
    else \
    { \
        return; \
    }

#define NiImplementDllMain(libname) \
    HMODULE gs_h##libname##Handle = 0; \
    BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID)\
    { \
        switch (ulReason) \
        { \
            case DLL_PROCESS_ATTACH: \
                gs_h##libname##Handle = hModule; \
                NiStaticDataManager::ProcessAccumulatedLibraries(); \
                break; \
            case DLL_PROCESS_DETACH: \
                gs_h##libname##Handle = 0; \
                NiStaticDataManager::RemoveLibrary(#libname); \
                break; \
        } \
        return TRUE; \
     }

#endif  // #ifndef NISDMMACROS_H
