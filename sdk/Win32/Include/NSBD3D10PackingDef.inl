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
// NSBD3D10PackingDef inline functions
//---------------------------------------------------------------------------
inline NSBD3D10PackingDef::NSBD3D10PDEntry::NSBD3D10PDEntry() :
    m_uiStream(0), 
    m_uiRegister(0), 
    m_uiInput((unsigned int)NiShaderDeclaration::SHADERPARAM_INVALID), 
    m_eType(NID3DPDT_INVALID), 
    m_eUsage(NiShaderDeclaration::SPUSAGE_POSITION), 
    m_uiUsageIndex(0)
{
}
//---------------------------------------------------------------------------
inline NSBD3D10PackingDef::NSBD3D10PDEntry::~NSBD3D10PDEntry()
{
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10PackingDef::NSBD3D10PDEntry::GetStream()
{
    return m_uiStream;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10PackingDef::NSBD3D10PDEntry::GetRegister()
{
    return m_uiRegister;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10PackingDef::NSBD3D10PDEntry::GetInput()
{
    return m_uiInput;
}
//---------------------------------------------------------------------------
inline NSBD3D10PackingDef::NiD3D10PackingDefType 
    NSBD3D10PackingDef::NSBD3D10PDEntry::GetType()
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline NiShaderDeclaration::ShaderParameterUsage 
    NSBD3D10PackingDef::NSBD3D10PDEntry::GetUsage()
{
    return m_eUsage;
}
//---------------------------------------------------------------------------
inline unsigned int NSBD3D10PackingDef::NSBD3D10PDEntry::GetUsageIndex()
{
    return m_uiUsageIndex;
}
//---------------------------------------------------------------------------
inline void NSBD3D10PackingDef::NSBD3D10PDEntry::SetStream(
    unsigned int uiStream)
{
    m_uiStream = uiStream;
}
//---------------------------------------------------------------------------
inline void NSBD3D10PackingDef::NSBD3D10PDEntry::SetRegister(
    unsigned int uiRegister)
{
    m_uiRegister = uiRegister;
}
//---------------------------------------------------------------------------
inline void NSBD3D10PackingDef::NSBD3D10PDEntry::SetInput(
    unsigned int uiInput)
{
    m_uiInput = uiInput;
}
//---------------------------------------------------------------------------
inline void NSBD3D10PackingDef::NSBD3D10PDEntry::SetType(
    NiD3D10PackingDefType eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline void NSBD3D10PackingDef::NSBD3D10PDEntry::SetUsage(
    NiShaderDeclaration::ShaderParameterUsage eUsage)
{
    m_eUsage = eUsage;
}
//---------------------------------------------------------------------------
inline void NSBD3D10PackingDef::NSBD3D10PDEntry::SetUsageIndex(
    unsigned int uiUsageIndex)
{
    m_uiUsageIndex = uiUsageIndex;
}
//---------------------------------------------------------------------------
