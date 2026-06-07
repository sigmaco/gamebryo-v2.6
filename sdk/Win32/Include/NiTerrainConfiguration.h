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

#ifndef NITERRAINCONFIGURATION_H
#define NITERRAINCONFIGURATION_H

#include <NiMemObject.h>
#include <NiUniversalTypes.h>

/**
    The terrain configuration class contains various settings used to determine how the terrain 
    data is packed and rendered at run-time depending on the current platform.
    @note The configuration is set per platform and cannot be modified.
*/
class NITERRAIN_ENTRY NiTerrainConfiguration : public NiMemObject
{
public:
    
    /**
        Default constructor. The constructor sets the configuration flags automatically depending
        on what platform the application is currently running.
    */
    NiTerrainConfiguration();

    /// @cond EMERGENT_INTERNAL

    inline void EnableMorphingData(bool bMorphingDataEnabled);
    inline void EnableTangentData(bool bTangentDataEnabled);
    inline void EnableLightingDataCompression(
        bool bLightingDataCompressionEnabled);

    inline bool IsMorphingDataEnabled() const;
    inline bool IsTangentDataEnabled() const;
    inline bool IsLightingDataCompressionEnabled() const;

    inline NiUInt32 GetNumPositionComponents() const;
    inline NiUInt32 GetNumTangentComponents() const;
    inline NiUInt32 GetNumNormalComponents() const;

    bool ValidateConfiguration();

    /// @endcond

private:

    bool m_bMorphingDataEnabled;
    bool m_bTangentDataEnabled;
    bool m_bLightingDataCompressionEnabled;

};

#include "NiTerrainConfiguration.inl"

#endif