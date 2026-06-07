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

#ifndef NISTRIPIFYINTERFACE_H
#define NISTRIPIFYINTERFACE_H

#include "NiStripifyLibType.h"
#include <NiMain.h>
#include <NiSystem.h>

#include "NiStripify.h"

class NISTRIPIFY_ENTRY NiStripifyInterface : public NiMemObject
{
public:
    enum Platform
    {
        PLATFORM_WIN32,  // PC
        PLATFORM_PS3,    // Sony PlayStation3
        PLATFORM_XBOX360,   // Microsoft Xbox 360
        PLATFORM_WII,    // Nintendo Wii
        PLATFORM_COUNT
    };

    NiStripifyInterface(Platform ePlatform);
    void Stripify_Object(NiNode* pkNode);

    inline void SetInterfaceCacheSize(unsigned int uiCacheSize);

    Platform GetPlatform() const;
    inline unsigned int GetCacheSize() const;

    static unsigned int GetDefaultCacheSize(Platform ePlatform);

protected:
    void Stripify_NextGeneration(NiNode* pkNode);

    class Platform_Settings
    {
    public:
        Platform m_ePlatform;
        unsigned int m_uiCacheSize;
    };

    static Platform_Settings ms_akDefaultSettings[PLATFORM_COUNT];

    Platform_Settings m_kSettings;
};

#include "NiStripifyInterface.inl"

#endif  //#ifndef NISTRIPIFYINTERFACE_H
