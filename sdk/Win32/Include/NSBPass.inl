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
inline const char* NSBPass::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetShaderProgramFile(
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType) const
{
    return m_akInfo[eRenderer][eType].m_pcProgramFile;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetShaderProgramFile(const char* pcProgramFileName, 
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType)
{
    NiRendererUtility::SetString(m_akInfo[eRenderer][eType].m_pcProgramFile, 
        0, pcProgramFileName);
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetShaderProgramEntryPoint(
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType) const
{
    return m_akInfo[eRenderer][eType].m_pcEntryPoint;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetShaderProgramEntryPoint(const char* pcEntryPoint, 
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType)
{
    NiRendererUtility::SetString(m_akInfo[eRenderer][eType].m_pcEntryPoint, 
        0, pcEntryPoint);
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetShaderProgramShaderTarget(
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType) const
{
    return m_akInfo[eRenderer][eType].m_pcTarget;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetShaderProgramShaderTarget(const char* pcShaderTarget, 
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType)
{
    NiRendererUtility::SetString(m_akInfo[eRenderer][eType].m_pcTarget, 0,
        pcShaderTarget);
}
//---------------------------------------------------------------------------
inline bool NSBPass::GetSoftwareVertexProcessing() const
{
    return m_bSoftwareVP;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetSoftwareVertexProcessing(bool bSoftwareVP)
{
    m_bSoftwareVP = bSoftwareVP;
}
//---------------------------------------------------------------------------
inline NSBUserDefinedDataSet* NSBPass::GetUserDefinedDataSet()
{
    return m_spUserDefinedDataSet;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetUserDefinedDataSet(NSBUserDefinedDataSet* pkUDDSet)
{
    m_spUserDefinedDataSet = pkUDDSet;
}
//---------------------------------------------------------------------------
inline unsigned int NSBPass::GetVertexConstantMapCount()
{
    return m_kVertexShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NSBPass::GetGeometryConstantMapCount()
{
    return m_kGeometryShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NSBPass::GetPixelConstantMapCount()
{
    return m_kPixelShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline NiStreamOutSettings& NSBPass::GetStreamOutSettings() 
{ 
    return m_kStreamOutSettings; 
}
//---------------------------------------------------------------------------
