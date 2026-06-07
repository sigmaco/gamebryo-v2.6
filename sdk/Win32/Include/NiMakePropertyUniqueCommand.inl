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
inline NiMakePropertyUniqueCommand::NiMakePropertyUniqueCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface,
    const NiFixedString& kPropertyName) 
    :
    m_spEntityPropertyInterface(pkEntityPropertyInterface),
    m_kPropertyName(kPropertyName), 
    m_bMadeUnique(false)
{
    NiFixedString kTemp;
    NIASSERT(m_spEntityPropertyInterface && m_kPropertyName.Exists() &&
        m_spEntityPropertyInterface->GetDisplayName(m_kPropertyName, kTemp));

    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, 
        "Make \"%s\" property unique on \"%s\" entity", 
        (const char*)m_kPropertyName, 
        (const char*)m_spEntityPropertyInterface->GetName());
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiEntityPropertyInterface*
    NiMakePropertyUniqueCommand::GetEntityPropertyInterface() const
{
    return m_spEntityPropertyInterface;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiMakePropertyUniqueCommand::GetPropertyName()
    const
{
    return m_kPropertyName;
}
//---------------------------------------------------------------------------
