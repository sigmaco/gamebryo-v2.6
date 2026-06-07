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

#ifndef NIVERSION_H
#define NIVERSION_H

// GAMEBRYO VERSION NUMBERS (to be changed by Emergent Developers only)
#define GAMEBRYO_MAJOR_VERSION 2
#define GAMEBRYO_MINOR_VERSION 6
#define GAMEBRYO_PATCH_VERSION 0
#define GAMEBRYO_BUILD_VERSION 0

#define GAMEBRYO_SDK_VERSION \
    (GAMEBRYO_MAJOR_VERSION << 24) | (GAMEBRYO_MINOR_VERSION << 16) | \
    (GAMEBRYO_PATCH_VERSION << 8) | (GAMEBRYO_BUILD_VERSION << 0) 

// This value represents the date of the current Emergent-provided build
#define GAMEBRYO_BUILD_DATE_DAY 20
#define GAMEBRYO_BUILD_DATE_MONTH 10
#define GAMEBRYO_BUILD_DATE_YEAR 2008
#define GAMEBRYO_BUILD_DATE \
    (GAMEBRYO_BUILD_DATE_YEAR << 16) + (GAMEBRYO_BUILD_DATE_MONTH << 8) + \
    (GAMEBRYO_BUILD_DATE_DAY)

// Need the extra level of indirection so the values of the macros are
// concatenated, not the names.
#define CONCAT2(A, B) A##B
#define CONCAT(A, B) CONCAT2(A, B)

// NIF VERSION NUMBERS (also to be changed by Emergent Developers only)
#define NIF_MAJOR_VERSION CONCAT(GAMEBRYO_MAJOR_VERSION, 0)
#define NIF_MINOR_VERSION GAMEBRYO_MINOR_VERSION
#define NIF_PATCH_VERSION 0
#define NIF_INTERNAL_VERSION 0

// Without the extra level of macro indirection, GAMEBRYO_*_VERSION_STRING 
// would end up containing the names of the *_VERSION macros instead of their
// values.
#define NI_STR2(A) #A
#define NI_STR(B) NI_STR2(B)

#define GAMEBRYO_SDK_VERSION_STRING \
    NI_STR(GAMEBRYO_MAJOR_VERSION) "." NI_STR(GAMEBRYO_MINOR_VERSION) "." \
    NI_STR(GAMEBRYO_PATCH_VERSION) "." NI_STR(GAMEBRYO_BUILD_VERSION)

#define GAMEBRYO_NIF_VERSION_STRING \
    NI_STR(NIF_MAJOR_VERSION) "." NI_STR(NIF_MINOR_VERSION) "." \
    NI_STR(NIF_PATCH_VERSION) "." NI_STR(NIF_INTERNAL_VERSION)

#define GAMEBRYO_MODULE_VERSION_STRING(module) \
    #module" Gamebryo Version "GAMEBRYO_SDK_VERSION_STRING

#define GAMEBRYO_BUILD_DATE_STRING \
    NI_STR(GAMEBRYO_BUILD_DATE_MONTH) "-" \
    NI_STR(GAMEBRYO_BUILD_DATE_DAY) "-" \
    NI_STR(GAMEBRYO_BUILD_DATE_YEAR)

#if defined(NIDEBUG)
    #define NI_DLL_CONFIG_SUFFIX "D"
#elif defined(NIRELEASE)
    #define NI_DLL_CONFIG_SUFFIX "R"
#elif defined(NISHIPPING)
    #define NI_DLL_CONFIG_SUFFIX "S"
#endif

#if defined(WIN32) || defined(_XENON)
    #if (_MSC_VER == 1500) //VC9.0
        #define NI_SHORT_DLL_SUFFIX NI_STR(GAMEBRYO_MAJOR_VERSION) \
            NI_STR(GAMEBRYO_MINOR_VERSION) "VC90"
    #elif (_MSC_VER == 1400) //VC8.0
        #define NI_SHORT_DLL_SUFFIX NI_STR(GAMEBRYO_MAJOR_VERSION) \
            NI_STR(GAMEBRYO_MINOR_VERSION) "VC80"
    #elif (_MSC_VER == 1310) //VC7.1
        #define NI_SHORT_DLL_SUFFIX NI_STR(GAMEBRYO_MAJOR_VERSION) \
            NI_STR(GAMEBRYO_MINOR_VERSION) "VC71"
    #else
        #error Unsupported version of Visual Studio
    #endif
#else // #if defined(WIN32) || defined(_XENON)
    #define NI_SHORT_DLL_SUFFIX ""
#endif // #if defined(WIN32) || defined(_XENON) 

// Gamebryo DLLs are in the form of $(ProjectName)$(NI_DLL_SUFFIX).dll
#define NI_DLL_SUFFIX NI_SHORT_DLL_SUFFIX NI_DLL_CONFIG_SUFFIX

#endif
