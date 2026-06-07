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
inline NiSetElementCountCommand::NiSetElementCountCommand(
    NiEntityPropertyInterface* pkPropertyInterface,
    const NiFixedString& kPropertyName, unsigned int uiNewCount) :
    m_spPropertyInterface(pkPropertyInterface), m_kPropertyName(kPropertyName),
    m_uiOldCount(0), m_uiNewCount(uiNewCount), m_bOldCountValid(false),
    m_bAttemptUndo(false)
{
    NIASSERT(m_spPropertyInterface && m_kPropertyName.Exists());

    char acCommandName[1024];
    NiSprintf(acCommandName, 1024, "Change the element count for the \"%s\" "
        "property on the \"%s\" entity/component to %d.",
        (const char*)m_kPropertyName,
        (const char*)m_spPropertyInterface->GetName(),
        m_uiNewCount);
    m_kCommandName = acCommandName;
}
//---------------------------------------------------------------------------
inline NiEntityPropertyInterface*
    NiSetElementCountCommand::GetPropertyInterface() const
{
    return m_spPropertyInterface;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiSetElementCountCommand::GetPropertyName() const
{
    return m_kPropertyName;
}
//---------------------------------------------------------------------------
inline unsigned int NiSetElementCountCommand::GetOldCount() const
{
    return m_uiOldCount;
}
//---------------------------------------------------------------------------
inline unsigned int NiSetElementCountCommand::GetNewCount() const
{
    return m_uiNewCount;
}
//---------------------------------------------------------------------------
