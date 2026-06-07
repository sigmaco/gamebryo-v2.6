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
inline const char* NSBD3D10Pass::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Pass::GetShaderProgramFile(
    NiSystemDesc::RendererID eRenderer,
    NiGPUProgram::ProgramType eType) const
{
    return m_akInfo[eRenderer][eType].m_pcProgramFile;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Pass::SetShaderProgramFile(const char* pcProgramFileName, 
    NiSystemDesc::RendererID eRenderer, NiGPUProgram::ProgramType eType)
{
    NSBD3D10Utility::SetString(m_akInfo[eRenderer][eType].m_pcProgramFile, 
        0, pcProgramFileName);
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Pass::GetShaderProgramEntryPoint(
    NiSystemDesc::RendererID eRenderer,
    NiGPUProgram::ProgramType eType) const
{
    return m_akInfo[eRenderer][eType].m_pcEntryPoint;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Pass::SetShaderProgramEntryPoint(
    const char* pcEntryPoint, NiSystemDesc::RendererID eRenderer,
    NiGPUProgram::ProgramType eType)
{
    NSBD3D10Utility::SetString(m_akInfo[eRenderer][eType].m_pcEntryPoint, 
        0, pcEntryPoint);
}
//---------------------------------------------------------------------------
inline const char* NSBD3D10Pass::GetShaderProgramShaderTarget(
    NiSystemDesc::RendererID eRenderer,
    NiGPUProgram::ProgramType eType) const
{
    return m_akInfo[eRenderer][eType].m_pcTarget;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Pass::SetShaderProgramShaderTarget(
    const char* pcShaderTarget, NiSystemDesc::RendererID eRenderer,
    NiGPUProgram::ProgramType eType)
{
    NSBD3D10Utility::SetString(m_akInfo[eRenderer][eType].m_pcTarget, 0,
        pcShaderTarget);
}
//---------------------------------------------------------------------------
inline bool NSBD3D10Pass::GetSoftwareVertexProcessing() const
{
    return m_bSoftwareVP;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Pass::SetSoftwareVertexProcessing(bool bSoftwareVP)
{
    m_bSoftwareVP = bSoftwareVP;
}
//---------------------------------------------------------------------------
inline NSBD3D10UserDefinedDataSet* NSBD3D10Pass::GetUserDefinedDataSet()
{
    return m_spUserDefinedDataSet;
}
//---------------------------------------------------------------------------
inline void NSBD3D10Pass::SetUserDefinedDataSet(
    NSBD3D10UserDefinedDataSet* pkUDDSet)
{
    m_spUserDefinedDataSet = pkUDDSet;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Pass::GetVertexConstantMapCount()
{
    return m_kVertexShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Pass::GetGeometryConstantMapCount()
{
    return m_kGeometryShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10Pass::GetPixelConstantMapCount()
{
    return m_kPixelShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
inline NiStreamOutSettings& NSBD3D10Pass::GetStreamOutSettings() 
{ 
    return m_kStreamOutSettings; 
}
//---------------------------------------------------------------------------
