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
inline NiFixedString::NiFixedString()
{
    m_kHandle = NiGlobalStringTable::NULL_STRING;
}
//---------------------------------------------------------------------------
inline NiFixedString::NiFixedString(const char* pcString)
{
    m_kHandle = NiGlobalStringTable::AddString(pcString);
}
//---------------------------------------------------------------------------
inline NiFixedString::NiFixedString(const NiFixedString& kString)
{
    NiGlobalStringTable::IncRefCount(
        (NiGlobalStringTable::GlobalStringHandle&)kString.m_kHandle);
    m_kHandle = kString.m_kHandle;
}
//---------------------------------------------------------------------------
inline NiFixedString::~NiFixedString()
{
    NiGlobalStringTable::DecRefCount(m_kHandle);
}
//---------------------------------------------------------------------------
inline NiFixedString::operator const char*() const
{
    return NiGlobalStringTable::GetString(m_kHandle);
}
//---------------------------------------------------------------------------
inline NiFixedString& NiFixedString::operator=(const NiFixedString& kString)
{
    if (m_kHandle != kString.m_kHandle)
    {
        NiGlobalStringTable::GlobalStringHandle kHandle = kString.m_kHandle;
        NiGlobalStringTable::IncRefCount(kHandle);
        NiGlobalStringTable::DecRefCount(m_kHandle);
        m_kHandle = kHandle;
    }
    return *this;
}
//---------------------------------------------------------------------------
inline NiFixedString& NiFixedString::operator=(const char* pcString)
{
    if (m_kHandle != pcString)
    {
        NiGlobalStringTable::GlobalStringHandle kHandle = m_kHandle;
        m_kHandle = NiGlobalStringTable::AddString(pcString);
        NiGlobalStringTable::DecRefCount(kHandle);
    }
    return *this;
}
//---------------------------------------------------------------------------
inline bool NiFixedString::Exists() const
{
    return m_kHandle != NiGlobalStringTable::NULL_STRING;
}
//---------------------------------------------------------------------------
inline size_t NiFixedString::GetLength() const
{
    return NiGlobalStringTable::GetLength(m_kHandle);
}
//---------------------------------------------------------------------------
inline unsigned int NiFixedString::GetRefCount() const
{
    return (unsigned int)NiGlobalStringTable::GetRefCount(m_kHandle);
}
//---------------------------------------------------------------------------
inline bool NiFixedString::Equals(const char* pcStr) const
{
    // Handle NULL checks and that this points to the exact same location
    // in memory.
    if (m_kHandle == pcStr)
        return true;

    // The previous check would return true if BOTH were NULL. Now check 
    // if the const char* is NULL
    if (pcStr == NULL  || m_kHandle == NULL)
        return false;

    return strcmp((const char*) m_kHandle, pcStr) == 0;
}
//---------------------------------------------------------------------------
inline bool NiFixedString::EqualsNoCase(const char* pcStr) const
{
    // Handle NULL checks and that this points to the exact same location
    // in memory.
    if (m_kHandle == pcStr)
        return true;

    // The previous check would return true if BOTH were NULL. Now check 
    // if the const char* is NULL
    if (pcStr == NULL  || m_kHandle == NULL)
        return false;

    return NiStricmp((const char*) m_kHandle, pcStr) == 0;
}
//---------------------------------------------------------------------------
inline bool NiFixedString::Contains(const char* pcStr) const
{
    if (m_kHandle == pcStr && pcStr != NULL)
        return true;

    // The previous check would return true if BOTH were NULL. Now check 
    // if the const char* is NULL
    if (pcStr == NULL  || m_kHandle == NULL || pcStr[0] == '\0' ||
        (const char*) m_kHandle == '\0')
    {
        return false;
    }

    return strstr((const char*) m_kHandle, pcStr) != NULL;
}
//---------------------------------------------------------------------------
inline bool NiFixedString::ContainsNoCase(const char* pcStr) const
{
    if (m_kHandle == pcStr && pcStr != NULL)
        return true;

    // The previous check would return true if BOTH were NULL. Now check 
    // if the const char* is NULL
    if (pcStr == NULL  || m_kHandle == NULL || pcStr[0] == '\0' ||
        (const char*) m_kHandle == '\0')
    {
        return false;
    }

    const char* pcMyString = (const char*) m_kHandle;
    size_t stOtherLength = strlen(pcStr);
    for (unsigned int uiMyIdx = 0; uiMyIdx < GetLength(); uiMyIdx++)
    {
        if (NiStrnicmp(&pcMyString[uiMyIdx], pcStr, stOtherLength) == 0)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
inline void NiFixedString::LoadCStringAsFixedString(NiBinaryStream& kStream,
    NiFixedString& kString)
{
    unsigned int uiLength;
    NiStreamLoadBinary(kStream, uiLength);
    if (uiLength >= 1024)
    {
        char* pcString = 0;
        pcString = NiAlloc(char, uiLength + 1);
        NIASSERT(pcString);

        kStream.Read(pcString, uiLength);
        pcString[uiLength] = 0;
        kString = pcString;

        NiFree(pcString);
    }
    else if (uiLength > 0)
    {
        char acString[1024];
        
        kStream.Read(acString, uiLength);
        acString[uiLength] = 0;
        kString = acString;
    }
    else
    {
        kString = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiFixedString::SaveFixedStringAsCString(NiBinaryStream& kStream,
    const NiFixedString& kString)
{
    kStream.WriteCString((const char*)kString);
}
//---------------------------------------------------------------------------
inline bool operator==(const NiFixedString& s1, 
    const NiFixedString& s2)
{
    return s1.m_kHandle == s2.m_kHandle;
}
//---------------------------------------------------------------------------
inline bool operator!=(const NiFixedString& s1, 
    const NiFixedString& s2)
{
    return s1.m_kHandle != s2.m_kHandle;
}
//---------------------------------------------------------------------------
inline bool operator==(const NiFixedString& s1, const char* s2)
{
    return s1.Equals(s2);
}
//---------------------------------------------------------------------------
inline bool operator!=(const NiFixedString& s1, const char* s2)
{
    return !(s1.Equals(s2));
}
//---------------------------------------------------------------------------
inline bool operator==(const char* s1, const NiFixedString& s2)
{
    return s2.Equals(s1);
}
//---------------------------------------------------------------------------
inline bool operator!=(const char* s1, const NiFixedString& s2)
{
    return !(s2.Equals(s1));
}
//---------------------------------------------------------------------------
inline void NiStandardizeFilePath(NiFixedString& kString)
{
    const char* pcString = (const char*) kString;
    char acTempPath[NI_MAX_PATH];
    NiStrcpy(acTempPath, NI_MAX_PATH, pcString);
    NiPath::Standardize(acTempPath);
    kString = acTempPath;
}
//---------------------------------------------------------------------------
