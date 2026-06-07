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
inline void NiSCMExtraData::AddEntry(
    unsigned int uiConstantIndex, unsigned int uiPass,
    NiGPUProgram::ProgramType eType, NiExtraData* pkExtraData,
    bool bGlobal)
{
    NIASSERT(m_auiCurrentEntry[eType] < m_auiNumEntries[eType]);
    // Set pass to 255 if bGlobal is true since it is highly unlikely any
    // shader will have 255 passes.
    if (bGlobal)
        uiPass = 255;
    unsigned int uiIndex = uiConstantIndex | uiPass << m_uiPassShift;

    m_apkEntries[eType][m_auiCurrentEntry[eType]].m_uiIndex = uiIndex;
    m_apkEntries[eType][m_auiCurrentEntry[eType]++].m_pkExtraData = 
        pkExtraData;
}
//---------------------------------------------------------------------------
inline NiExtraData* NiSCMExtraData::GetNextEntry(
    unsigned int uiConstantIndex, unsigned int uiPass,
    NiGPUProgram::ProgramType eType, bool)
{
    NIASSERT(m_auiCurrentEntry[eType] < m_auiNumEntries[eType]);
    unsigned int uiIndex = uiConstantIndex | uiPass << m_uiPassShift;

    // Make sure the indices match as a simple error check.
    if (m_apkEntries[eType][m_auiCurrentEntry[eType]].m_uiIndex == uiIndex)
        return m_apkEntries[eType][m_auiCurrentEntry[eType]++].m_pkExtraData;
       
    return 0;
}
//---------------------------------------------------------------------------
inline void NiSCMExtraData::Reset()
{
    // Reset the walk through the list.
    m_auiCurrentEntry[NiGPUProgram::PROGRAM_VERTEX] = 0;
    m_auiCurrentEntry[NiGPUProgram::PROGRAM_GEOMETRY] = 0;
    m_auiCurrentEntry[NiGPUProgram::PROGRAM_PIXEL] = 0;
}
//---------------------------------------------------------------------------
