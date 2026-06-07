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

#include "NiTerrainPCH.h"
#include "NiTerrainConfiguration.h"

NiTerrainConfiguration::NiTerrainConfiguration()
{
#if defined(_WII)
    m_bMorphingDataEnabled = false;
    m_bTangentDataEnabled = false;
    m_bLightingDataCompressionEnabled = false;
#else
    m_bMorphingDataEnabled = true;
    m_bTangentDataEnabled = true;
    m_bLightingDataCompressionEnabled = true;
#endif

    ValidateConfiguration();
}
//---------------------------------------------------------------------------
bool NiTerrainConfiguration::ValidateConfiguration()
{
    bool bValid = true;

#if defined(_WII)
    if (m_bMorphingDataEnabled || m_bTangentDataEnabled || 
        m_bLightingDataCompressionEnabled)
    {
        bValid = false;
    }
#else
    if (m_bMorphingDataEnabled && !m_bLightingDataCompressionEnabled)
    {
        bValid = false;
    }
#endif

    NIASSERT(bValid);
    return bValid;
}
//---------------------------------------------------------------------------