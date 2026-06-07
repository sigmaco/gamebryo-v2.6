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
inline NiAddRemovePropertyCommand::NiAddRemovePropertyCommand(
    NiEntityPropertyInterface* pkPropertyInterface,
    const NiFixedString& kPropertyName, 
    const NiFixedString& kDisplayName,
    const NiFixedString& kPrimitiveType,
    const NiFixedString& kSemanticType,
    const NiFixedString& kDescription, 
    bool bAddProperty) 
    :
    m_spPropertyInterface(pkPropertyInterface),
    m_kPropertyName(kPropertyName), 
    m_kDisplayName(kDisplayName),
    m_kPrimitiveType(kPrimitiveType), 
    m_kSemanticType(kSemanticType),
    m_kDescription(kDescription), 
    m_bCollection(false),
    m_bAddProperty(bAddProperty),
    m_bSuccessfullyExecuted(false), 
    m_bShouldUndo(false)
{
    NIASSERT(m_spPropertyInterface && m_kPropertyName.Exists() &&
        m_kPrimitiveType.Exists() && m_kSemanticType.Exists());

    GenerateCommandName();
}
//---------------------------------------------------------------------------
inline NiAddRemovePropertyCommand::NiAddRemovePropertyCommand(
    NiEntityPropertyInterface* pkPropertyInterface,
    const NiFixedString& kPropertyName, 
    const NiFixedString& kDisplayName,
    const NiFixedString& kPrimitiveType,
    const NiFixedString& kSemanticType,
    bool bCollection,
    const NiFixedString& kDescription, 
    bool bAddProperty) 
    :
    m_spPropertyInterface(pkPropertyInterface),
    m_kPropertyName(kPropertyName), 
    m_kDisplayName(kDisplayName),
    m_kPrimitiveType(kPrimitiveType), 
    m_kSemanticType(kSemanticType),
    m_kDescription(kDescription), 
    m_bCollection(bCollection),
    m_bAddProperty(bAddProperty),
    m_bSuccessfullyExecuted(false), 
    m_bShouldUndo(false)
{
    NIASSERT(m_spPropertyInterface && m_kPropertyName.Exists() &&
        m_kPrimitiveType.Exists() && m_kSemanticType.Exists());

    GenerateCommandName();
}
//---------------------------------------------------------------------------
inline void NiAddRemovePropertyCommand::GenerateCommandName()
{
    // Generate command name.
    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "%s \"%s\" property %s \"%s\" entity/component", 
        m_bAddProperty ? "Add" : "Remove",
        (const char*)m_kPropertyName, 
        m_bAddProperty ? "to" : "from",
        (const char*)m_spPropertyInterface->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiEntityPropertyInterface* 
    NiAddRemovePropertyCommand::GetPropertyInterface() const
{
    return m_spPropertyInterface;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiAddRemovePropertyCommand::GetPropertyName()
    const
{
    return m_kPropertyName;
}
//---------------------------------------------------------------------------
inline NiBool NiAddRemovePropertyCommand::GetAddProperty() const
{
    return m_bAddProperty;
}
//---------------------------------------------------------------------------
