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

//  NiStripifyInterface inline functions

//---------------------------------------------------------------------------
inline void NiStripifyInterface::SetInterfaceCacheSize(
    unsigned int uiCacheSize)
{
    m_kSettings.m_uiCacheSize = uiCacheSize;
}
//---------------------------------------------------------------------------
inline NiStripifyInterface::Platform NiStripifyInterface::GetPlatform() const
{
    return m_kSettings.m_ePlatform;
}
//---------------------------------------------------------------------------
inline unsigned int NiStripifyInterface::GetCacheSize() const
{
    return m_kSettings.m_uiCacheSize;
}
//---------------------------------------------------------------------------
inline unsigned int NiStripifyInterface::GetDefaultCacheSize(
    Platform ePlatform)
{
    return ms_akDefaultSettings[ePlatform].m_uiCacheSize;
}
//---------------------------------------------------------------------------

