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

//---------------------------------------------------------------------------
inline const NiSystemDesc& NiSystemDesc::GetSystemDesc() 
{
    NIASSERT(ms_pkSystemDesc != NULL);
    return *(ms_pkSystemDesc);
}
//---------------------------------------------------------------------------
inline bool NiSystemDesc::IsLittleEndian() const
{
    return true;
}
//---------------------------------------------------------------------------
inline unsigned int NiSystemDesc::GetPhysicalProcessorCount() const
{
    return m_uiNumPhysicalProcessors;
}
//---------------------------------------------------------------------------
inline unsigned int NiSystemDesc::GetPhysicalCoreCount() const
{
    return m_uiNumPhysicalCores;
}
//---------------------------------------------------------------------------
inline unsigned int NiSystemDesc::GetLogicalProcessorCount() const
{
    return m_uiNumLogicalProcessors;
}
//---------------------------------------------------------------------------
inline NiSystemDesc::PlatformID NiSystemDesc::GetPlatformID() const
{
    return NI_WIN32;
}
//---------------------------------------------------------------------------
inline float NiSystemDesc::GetPerformanceCounterHz() const
{
    return m_fPCCyclesPerSecond;
}
//---------------------------------------------------------------------------
inline bool NiSystemDesc::GetToolMode() const
{
    return m_bToolMode;
}
//---------------------------------------------------------------------------
inline void NiSystemDesc::SetToolMode(const bool bToolMode) const
{
    m_bToolMode = bToolMode;
}
//---------------------------------------------------------------------------
inline NiSystemDesc::RendererID NiSystemDesc::GetToolModeRendererID() const
{
    NIASSERT(m_bToolMode);
    return m_eToolModeRendererID;
}
//---------------------------------------------------------------------------
inline void NiSystemDesc::SetToolModeRendererID(
    const NiSystemDesc::RendererID eToolModeRendererID) const
{
    NIASSERT(m_bToolMode);
    m_eToolModeRendererID = eToolModeRendererID;
}
//---------------------------------------------------------------------------

