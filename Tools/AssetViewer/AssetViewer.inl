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

// AssetViewer.inl

//---------------------------------------------------------------------------
inline int CAssetViewerApp::GetSettingsParamInt(CString strParam)
{
    return GetProfileInt("Settings", strParam, g_iParamNotFound);
}
//---------------------------------------------------------------------------
inline void CAssetViewerApp::SetSettingsParamInt(CString strParam, int iValue)
{
    WriteProfileInt("Settings", strParam, iValue);
}
//---------------------------------------------------------------------------
inline CString CAssetViewerApp::GetSettingsParamString(CString strParam)
{
    return GetProfileString("Settings", strParam, g_strParamNotFound);
}
//---------------------------------------------------------------------------
inline void CAssetViewerApp::SetSettingsParamString(CString strParam,
    CString strValue)
{
    WriteProfileString("Settings", strParam, strValue);
}
//---------------------------------------------------------------------------
inline float CAssetViewerApp::GetSettingsParamFloat(CString strParam)
{
    float fValue;
    CString strValue = GetProfileString("Settings", strParam,
        g_strParamNotFound);
    if (strValue == g_strParamNotFound)
    {
        fValue = g_fParamNotFound;
    }
    else
    {
        fValue = (float) atof(strValue);
    }

    return fValue;
}
//---------------------------------------------------------------------------
inline void CAssetViewerApp::SetSettingsParamFloat(CString strParam,
    float fValue)
{
    CString strValue;
    strValue.Format("%f", fValue);
    WriteProfileString("Settings", strParam, strValue);
}
//---------------------------------------------------------------------------
inline CString CAssetViewerApp::GetApplicationDirectory()
{
    return m_kApplicationDirectory;
}
//---------------------------------------------------------------------------
