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


inline void NiTerrainConfiguration::EnableMorphingData(
    bool bMorphingDataEnabled)
{
    m_bMorphingDataEnabled = bMorphingDataEnabled;
    NIASSERT(ValidateConfiguration());
}
//---------------------------------------------------------------------------
inline void NiTerrainConfiguration::EnableTangentData(bool bTangentDataEnabled)
{
    m_bTangentDataEnabled = bTangentDataEnabled;
    NIASSERT(ValidateConfiguration());
}
//---------------------------------------------------------------------------
inline void NiTerrainConfiguration::EnableLightingDataCompression(
    bool bLightingDataCompressionEnabled)
{
    m_bLightingDataCompressionEnabled = bLightingDataCompressionEnabled;
    NIASSERT(ValidateConfiguration());
}
//---------------------------------------------------------------------------
inline bool NiTerrainConfiguration::IsMorphingDataEnabled() const
{
    return m_bMorphingDataEnabled;
}
//---------------------------------------------------------------------------
inline bool NiTerrainConfiguration::IsTangentDataEnabled() const
{
    return m_bTangentDataEnabled;
}
//---------------------------------------------------------------------------
inline bool NiTerrainConfiguration::IsLightingDataCompressionEnabled() const
{
    return m_bLightingDataCompressionEnabled;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainConfiguration::GetNumPositionComponents() const
{
    if (!m_bMorphingDataEnabled)
        return 3;
    return 4;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainConfiguration::GetNumTangentComponents() const
{
    if (!m_bTangentDataEnabled)
        return 0;

    if (m_bLightingDataCompressionEnabled && m_bMorphingDataEnabled)
        return 4;

    if (m_bLightingDataCompressionEnabled && !m_bMorphingDataEnabled)
        return 2;

    // !m_bLightingDataCompressionEnabled && !m_bMorphingDataEnabled
    return 3;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainConfiguration::GetNumNormalComponents() const
{
    if (m_bLightingDataCompressionEnabled && m_bMorphingDataEnabled)
        return 4;

    if (m_bLightingDataCompressionEnabled && !m_bMorphingDataEnabled)
        return 2;

    // !m_bLightingDataCompressionEnabled && !m_bMorphingDataEnabled
    return 3;
}