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
// NiGPUProgram inline functions
//---------------------------------------------------------------------------
inline NiGPUProgram::NiGPUProgram(ProgramType eType) : m_eProgramType(eType)
{
}
//---------------------------------------------------------------------------
inline NiGPUProgram::ProgramType NiGPUProgram::GetProgramType()
{
    return m_eProgramType;
}
//---------------------------------------------------------------------------
inline unsigned int NiGPUProgram::GetVariableCount()
{
    return 0;
}
//---------------------------------------------------------------------------
inline const char* NiGPUProgram::GetVariableName(unsigned int)
{
    return NULL;
}
//---------------------------------------------------------------------------
