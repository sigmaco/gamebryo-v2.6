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
inline const NiFixedString& NiShaderDeclaration::UsageToString(
    NiShaderDeclaration::ShaderParameterUsage eUsage)
{
    return ms_akUsageStrings[eUsage];
}
//---------------------------------------------------------------------------
inline NiShaderDeclaration::ShaderParameterUsage 
    NiShaderDeclaration::StringToUsage(const NiFixedString& kUsage)
{
    // Unfortunately, fixed string handles are not allocated in a fully
    // predicatable manner so we have to scan the existing array.
    for (unsigned int ui = 0; ui < NiShaderDeclaration::SPUSAGE_COUNT; ++ui)
    {
        if (ms_akUsageStrings[ui] == kUsage)
            return (NiShaderDeclaration::ShaderParameterUsage)ui;
    }

    return NiShaderDeclaration::SPUSAGE_POSITION;
}
//---------------------------------------------------------------------------
inline const NiShaderDeclaration::ShaderRegisterEntry* 
    NiShaderDeclaration::GetEntry(unsigned int uiEntry, 
    unsigned int uiStream) const
{
    if (uiStream >= m_uiStreamCount)
        return NULL;

    if (!m_pkStreamEntries || 
        uiEntry >= m_pkStreamEntries[uiStream].m_uiEntryCount)
    {
        return NULL;
    }
    return m_pkStreamEntries[uiStream].m_pkEntries + uiEntry;
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderDeclaration::GetStreamCount() const
{
    return m_uiStreamCount;
}
//---------------------------------------------------------------------------
inline unsigned int NiShaderDeclaration::GetEntryCount(
    unsigned int uiStream) const
{
    if (!m_pkStreamEntries || uiStream >= m_uiStreamCount)
        return NULL;
    return m_pkStreamEntries[uiStream].m_uiEntryCount;
}
//---------------------------------------------------------------------------
