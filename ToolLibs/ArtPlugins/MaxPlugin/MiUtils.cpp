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

#include "MaxImmerse.h"
#include "MiUtils.h"
#include "NiMAXConverter.h"
#include "MiResource.h"
#include "MiViewerPlugin.h"

//---------------------------------------------------------------------------
void MessageBox(int iString1, int iString2) 
{
    TSTR str1(GetString(iString1));
    TSTR str2(GetString(iString2));
    MessageBox(GetActiveWindow(), str1.data(), str2.data(), MB_OK);
}

//---------------------------------------------------------------------------
TCHAR* GetString(int iID)
{
    static TCHAR buf[256];

    if (hInstance)
        return LoadString(hInstance, iID, buf, sizeof(buf)) ? buf : NULL;

    return NULL;
}
//---------------------------------------------------------------------------
void HandleError(int iStatus)
{
#if 0
IDS_NI_ERROR_FCLOSE
IDS_NI_ERROR_WRITE
IDS_NI_ERROR_CANT_CREATE
IDS_NI_ERROR_INVALID_FILE
IDS_NI_ERROR_READ
IDS_NI_ERROR_OPENING_FILE
IDS_NI_ERROR_NOT_IN_PATH
IDS_NI_ERROR_NO_RENDERER
#endif

    switch (iStatus)
    {
    case W3D_STAT_MISSING_DEMO:
        MessageBox(IDS_NI_ERROR_MISSING_DEMO, IDS_NI_UTIL_ERR_TITLE);
        break;
    case W3D_STAT_NO_MEMORY:
        MessageBox(IDS_NI_ERROR_MEM_FAIL, IDS_NI_UTIL_ERR_TITLE);
        break;
    case W3D_STAT_NO_RENDERER:
        MessageBox(IDS_NI_ERROR_NO_RENDERER, IDS_NI_UTIL_ERR_TITLE);
        break;
    case W3D_STAT_FAILED:
    default:
        MessageBox(IDS_NI_ERROR_GENERIC, IDS_NI_UTIL_ERR_TITLE);
        break;
    }
}
#define NI_CONFIG_FILE_NAME _T("mimmerse.ini")
//---------------------------------------------------------------------------
void GetCfgFilename(Interface* pIntf, TCHAR* pFileName, 
    unsigned int uiFileNameSize)
{

    NiSprintf(pFileName, uiFileNameSize, "%s%s", 
        GetCOREInterface()->GetDir(APP_MAX_SYS_ROOT_DIR),
        "plugins\\MaxImmerse\\");

    int iLen = (int)_tcslen(pFileName);
    if (iLen)
    {
        if (_tcscmp(&pFileName[iLen-1], _T("\\")))
#if defined(_MSC_VER) && _MSC_VER >= 1400
            _tcscat_s(pFileName, uiFileNameSize, _T("\\"));
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            _tcscat(pFileName, _T("\\"));
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _tcscat_s(pFileName, uiFileNameSize, NI_CONFIG_FILE_NAME);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    _tcscat(pFileName, NI_CONFIG_FILE_NAME);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

#if defined(_MSC_VER) && _MSC_VER >= 1400
    FILE* pkFile = NULL;
    fopen_s(&pkFile, pFileName, "r");
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    FILE* pkFile = fopen(pFileName, "r");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (!pkFile)
    {
        GetUserCfgFilename(pIntf, pFileName, uiFileNameSize);
        return;
    }

    fclose(pkFile);
    pkFile = NULL;
}
//---------------------------------------------------------------------------
void GetUserCfgFilename(Interface* pIntf, TCHAR* pFileName, 
    unsigned int uiFileNameSize)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _tcscpy_s(pFileName, uiFileNameSize, pIntf->GetDir(APP_PLUGCFG_DIR));
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    _tcscpy(pFileName, pIntf->GetDir(APP_PLUGCFG_DIR));
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iLen = (int)_tcslen(pFileName);
    if (iLen)
    {
        if (_tcscmp(&pFileName[iLen-1], _T("\\")))
#if defined(_MSC_VER) && _MSC_VER >= 1400
            _tcscat_s(pFileName, uiFileNameSize, _T("\\"));
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            _tcscat(pFileName, _T("\\"));
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _tcscat_s(pFileName, uiFileNameSize, NI_CONFIG_FILE_NAME);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    _tcscat(pFileName, NI_CONFIG_FILE_NAME);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
bool ToBool(int i)
{
    if(i == 0)
        return false;
    else
        return true;
}
//---------------------------------------------------------------------------
Control * GetControlFromAnimatableName(Animatable *anim, char *name)
{
    int w;
    Control *c1, *c2;

    if (!anim) 
        return(NULL);

    for (w=0; w < anim->NumSubs(); w++)
    {
        if (strcmp(anim->SubAnimName(w), name) == 0)
        {
            c1=GetControlInterface(anim->SubAnim(w));
            return(c1);
        }
    
        c2=GetControlFromAnimatableName(anim->SubAnim(w), name);
        if (c2) 
            return(c2);
    }
    return(NULL);
}
//---------------------------------------------------------------------------
