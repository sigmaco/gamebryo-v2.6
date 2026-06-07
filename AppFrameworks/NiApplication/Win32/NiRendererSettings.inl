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
// Returns instance of a class or creates a new instance
//---------------------------------------------------------------------------
inline NiRendererSettings::~NiRendererSettings()
{
    ms_pkRendererSettings = NULL;
}

//---------------------------------------------------------------------------
inline NiRendererSettings* NiRendererSettings::GetInstance()
{
    if (ms_pkRendererSettings == NULL)
    {
        ms_pkRendererSettings = NiNew NiRendererSettings();
    }

    return ms_pkRendererSettings;
}

//---------------------------------------------------------------------------
// Initialization file name getting function
//---------------------------------------------------------------------------
inline const char* NiRendererSettings::GetIniFilename()
{
    return GetInstance()->m_kIniFileName;
}

//---------------------------------------------------------------------------
// Functions for reading / writing values to .ini file
//---------------------------------------------------------------------------
inline void NiRendererSettings::ReadUInt(
    const char* pcName,
    unsigned int& uiVal)
{
    uiVal = GetPrivateProfileInt("Renderer", pcName, uiVal, GetIniFilename());
}

//---------------------------------------------------------------------------
inline void NiRendererSettings::ReadBool(const char* pcName, bool& bVal)
{
    int iVal;
    iVal = GetPrivateProfileInt("Renderer", pcName, bVal, GetIniFilename());
    bVal = iVal ? true : false;
}

//---------------------------------------------------------------------------
inline void NiRendererSettings::WriteUInt(
    const char* pcName,
    unsigned int uiVal)
{
    char pcVal[64];
    NiSprintf(pcVal, 64, "%i", uiVal);
    WritePrivateProfileString("Renderer", pcName, pcVal, GetIniFilename());
}

//---------------------------------------------------------------------------
inline void NiRendererSettings::WriteBool(const char* pcName, bool bVal)
{
    char pcVal[64];
    NiSprintf(pcVal, 64, "%i", bVal);
    WritePrivateProfileString("Renderer", pcName, pcVal, GetIniFilename());
}
