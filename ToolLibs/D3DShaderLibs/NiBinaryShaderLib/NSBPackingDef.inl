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
// NSBPackingDef inline functions
//---------------------------------------------------------------------------
inline NSBPackingDef::NSBPDEntry::NSBPDEntry() :
    m_uiStream(0), 
    m_uiRegister(0), 
    m_uiInput((unsigned int)NiShaderDeclaration::SHADERPARAM_INVALID), 
    m_eType(NID3DPDT_INVALID), 
    m_eTesselator(NiShaderDeclaration::SPTESS_DEFAULT), 
    m_eUsage(NiShaderDeclaration::SPUSAGE_POSITION), 
    m_uiUsageIndex(0)
{
}
//---------------------------------------------------------------------------
inline NSBPackingDef::NSBPDEntry::~NSBPDEntry()
{
}
//---------------------------------------------------------------------------
inline unsigned int NSBPackingDef::NSBPDEntry::GetStream()
{
    return m_uiStream;
}
//---------------------------------------------------------------------------
inline unsigned int NSBPackingDef::NSBPDEntry::GetRegister()
{
    return m_uiRegister;
}
//---------------------------------------------------------------------------
inline unsigned int NSBPackingDef::NSBPDEntry::GetInput()
{
    return m_uiInput;
}
//---------------------------------------------------------------------------
inline NSBPackingDef::NiD3DPackingDefType 
    NSBPackingDef::NSBPDEntry::GetType()
{
    return m_eType;
}
//---------------------------------------------------------------------------
inline NiShaderDeclaration::ShaderParameterTesselator 
    NSBPackingDef::NSBPDEntry::GetTesselator()
{
    return m_eTesselator;
}
//---------------------------------------------------------------------------
inline NiShaderDeclaration::ShaderParameterUsage 
    NSBPackingDef::NSBPDEntry::GetUsage()
{
    return m_eUsage;
}
//---------------------------------------------------------------------------
inline unsigned int NSBPackingDef::NSBPDEntry::GetUsageIndex()
{
    return m_uiUsageIndex;
}
//---------------------------------------------------------------------------
inline void NSBPackingDef::NSBPDEntry::SetStream(unsigned int uiStream)
{
    m_uiStream = uiStream;
}
//---------------------------------------------------------------------------
inline void NSBPackingDef::NSBPDEntry::SetRegister(unsigned int uiRegister)
{
    m_uiRegister = uiRegister;
}
//---------------------------------------------------------------------------
inline void NSBPackingDef::NSBPDEntry::SetInput(unsigned int uiInput)
{
    m_uiInput = uiInput;
}
//---------------------------------------------------------------------------
inline void NSBPackingDef::NSBPDEntry::SetType(NiD3DPackingDefType eType)
{
    m_eType = eType;
}
//---------------------------------------------------------------------------
inline void NSBPackingDef::NSBPDEntry::SetTesselator(
    NiShaderDeclaration::ShaderParameterTesselator eTesselator)
{
    m_eTesselator = eTesselator;
}
//---------------------------------------------------------------------------
inline void NSBPackingDef::NSBPDEntry::SetUsage(
    NiShaderDeclaration::ShaderParameterUsage eUsage)
{
    m_eUsage = eUsage;
}
//---------------------------------------------------------------------------
inline void 
NSBPackingDef::NSBPDEntry::SetUsageIndex(unsigned int uiUsageIndex)
{
    m_uiUsageIndex = uiUsageIndex;
}
//---------------------------------------------------------------------------
