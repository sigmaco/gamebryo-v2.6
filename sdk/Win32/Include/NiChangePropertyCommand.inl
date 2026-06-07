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
inline NiChangePropertyCommand::NiChangePropertyCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface, 
    const NiFixedString& kPropertyName,
    unsigned int uiPropertyIndex) 
    :
    m_spEntityPropertyInterface(pkEntityPropertyInterface),
    m_kPropertyName(kPropertyName),
    m_uiPropertyIndex(uiPropertyIndex), 
    m_bOldDataValid(false),
    m_bIndexInRange(false),
    m_bShouldBeReset(false)
{
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, "Change data for \"%s\" property in "
        "\"%s\" entity/component", 
        (const char*)m_kPropertyName,
        (const char*)m_spEntityPropertyInterface->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiChangePropertyCommand::~NiChangePropertyCommand()
{
}
//---------------------------------------------------------------------------
inline NiEntityPropertyInterface*
    NiChangePropertyCommand::GetEntityPropertyInterface() const
{
    return m_spEntityPropertyInterface;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiChangePropertyCommand::GetPropertyName() const
{
    return m_kPropertyName;
}
//---------------------------------------------------------------------------
inline unsigned int NiChangePropertyCommand::GetPropertyIndex() const
{
    return m_uiPropertyIndex;
}
//---------------------------------------------------------------------------
