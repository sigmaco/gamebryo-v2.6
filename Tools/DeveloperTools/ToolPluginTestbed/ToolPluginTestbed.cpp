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

#include "stdafx.h"
#include "ToolPluginTestbed.h"
#include "ToolPluginTestbedDlg.h"
#include <NiMaterialToolkit.h>
#include <NiMeshProfileProcessor.h>
#include <NiMeshProfileXMLParser.h>
#include <NiMeshUtilities.h>
#include <NiDevImageQuantizer.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CToolPluginTestbedApp
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CToolPluginTestbedApp, CWinApp)
    //{{AFX_MSG_MAP(CToolPluginTestbedApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CToolPluginTestbedApp construction
//---------------------------------------------------------------------------
CToolPluginTestbedApp::CToolPluginTestbedApp()
{
}
//---------------------------------------------------------------------------
CToolPluginTestbedApp::~CToolPluginTestbedApp()
{

}
//---------------------------------------------------------------------------
// The one and only CToolPluginTestbedApp object
//---------------------------------------------------------------------------
CToolPluginTestbedApp theApp;
//---------------------------------------------------------------------------
// Utility function for recursively copying one key to another.  Assumes
// that hOldKey and hNewKey have been opened successfully.
//
// Returns true if no errors, false otherwise.
//---------------------------------------------------------------------------
inline bool RegCopyTreeEx(HKEY hOldKey, int, HKEY hNewKey)
{
    DWORD dwIndex = 0;
    char acNameBuffer[1024];
    char acClassBuffer[256];

    bool bDone = false;

    // Loop through subkeys of hOldKey and recurse through them
    while(!bDone)
    {
        DWORD dwNameSize = (DWORD)sizeof(acNameBuffer);
        DWORD dwClassSize = (DWORD)sizeof(acClassBuffer);

        LONG lRet = RegEnumKeyEx(hOldKey, dwIndex++, acNameBuffer,
            &dwNameSize, 0, acClassBuffer, &dwClassSize,
            NULL);

        switch (lRet)
        {
        case ERROR_SUCCESS:
            {
                // copy child keys recursively
                HKEY hOldSubKey;
                HKEY hNewSubKey;

                LONG lRetOld = RegOpenKeyEx(hOldKey, (LPCTSTR)acNameBuffer,
                    0, KEY_ALL_ACCESS, &hOldSubKey);
                LONG lRetNew = RegCreateKeyEx(hNewKey, (LPCTSTR)acNameBuffer,
                    0, acClassBuffer, 0, KEY_ALL_ACCESS, NULL, &hNewSubKey,
                    NULL);
 
                if (lRetOld == ERROR_SUCCESS && lRetNew == ERROR_SUCCESS)
                {
                    if (!RegCopyTreeEx(hOldSubKey, 0, hNewSubKey))
                        return false;
                }
                else
                {
                    return false;
                }
                break;
            }

        case ERROR_NO_MORE_ITEMS:
            // all done
            bDone = true;
            break;

        case ERROR_MORE_DATA:
            // shouldn't happen
            NIASSERT(!"Static array not big enough.");
            return false;

        default:
            // unhandled error
            return false;
        }
    }

    // Now copy all the values.
    dwIndex = 0;
    bDone = false;
    BYTE aDataBuffer[1024];

    while (!bDone)
    {
        DWORD dwNameLen = sizeof(acNameBuffer);
        DWORD dwDataLen = sizeof(aDataBuffer);
        DWORD dwType;

        LONG lRet = RegEnumValue(hOldKey, dwIndex++, acNameBuffer,
            &dwNameLen, 0, &dwType, aDataBuffer, &dwDataLen);

        switch (lRet)
        {
        case ERROR_SUCCESS:
            {
                // copy key
                LONG lRet = RegSetValueEx(hNewKey, acNameBuffer, 0,
                    dwType, aDataBuffer, dwDataLen);

                if (lRet != ERROR_SUCCESS)
                    return false;
            }
            break;

        case ERROR_NO_MORE_ITEMS:
            bDone = true;
            break;

        default:
            // unhandled error
            return false;
        }
    }

    // success
    return true;
}
//---------------------------------------------------------------------------
// Utility function for recursively copying one registry key to another.
// Copies hOldRoot/pcOldKey into hNewRoot/pcNewKey if and only if
// hNewRoot/pcNewKey does not exist and hOldRoot/pcOldKey does.
//---------------------------------------------------------------------------
inline void CopyIfNotExist(HKEY hOldRoot, const char* pcOldKey,
    HKEY hNewRoot, const char* pcNewKey)
{
    // Determine if the new settings exist.
    HKEY hNewKey;
    LONG lRet = RegOpenKeyEx(hNewRoot, (LPCTSTR) pcNewKey, 0, KEY_ALL_ACCESS,
        &hNewKey);

    // if new not found
    if (lRet != ERROR_SUCCESS)
    {
        HKEY hOldKey;

        lRet = RegOpenKeyEx(hOldRoot, (LPCTSTR)pcOldKey,
            0, KEY_ALL_ACCESS, &hOldKey);

        // If old found...
        if (lRet == ERROR_SUCCESS)
        {
            // ...then copy tree.
            DWORD dwDisposition;
            lRet = RegCreateKeyEx(hNewRoot, (LPCTSTR)pcNewKey,
                0, NULL, 0, KEY_ALL_ACCESS, NULL, &hNewKey,
                &dwDisposition);
            NIASSERT(lRet == ERROR_SUCCESS &&
                dwDisposition == REG_CREATED_NEW_KEY);
            RegCopyTreeEx(hOldKey, 0, hNewKey);

            RegCloseKey(hNewKey);
            RegCloseKey(hOldKey);
        }
    }
    else
    {
        RegCloseKey(hNewKey);
    }
}
//---------------------------------------------------------------------------
// CToolPluginTestbedApp initialization
//---------------------------------------------------------------------------
BOOL CToolPluginTestbedApp::InitInstance()
{
    // Get plug-in directory.
#if defined(_MSC_VER) && _MSC_VER >= 1400
    char acPluginPath[NI_MAX_PATH];
    unsigned int uiPathLen = 0;
    const char* pcPluginPath = NULL;
    if (getenv_s(&uiPathLen, acPluginPath, NI_MAX_PATH, 
        "EGB_TOOL_PLUGIN_PATH") != 0 || uiPathLen > NI_MAX_PATH)
    {
        acPluginPath[0] = '\0';
    }
    else
    {
        pcPluginPath = acPluginPath;
    }
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    const char* pcPluginPath = getenv("EGB_TOOL_PLUGIN_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (!pcPluginPath)
    {
        MessageBox(NULL, "The environment variable EGB_TOOL_PLUGIN_PATH "
            "must\nbe defined for this application to properly execute.",
            "Missing Environment Variable", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need.

    CopyIfNotExist(HKEY_CURRENT_USER, 
        "Software\\Numerical Design Ltd\\Gamebryo Tool Plug-in Testbed",
        HKEY_CURRENT_USER,
        "Software\\Emergent Game Technologies\\Gamebryo Tool Plug-in Testbed");

    // Set the registry key.
    SetRegistryKey("Emergent Game Technologies");

    // Get the current application directory.
    char acAppPath[_MAX_PATH];
    NiStrcpy(acAppPath, _MAX_PATH, __targv[0]);
    char* pcPtr = strrchr(acAppPath, '\\');
    NIASSERT(pcPtr);
    pcPtr += 1;
    *pcPtr = '\0';

    NiFramework::InitFramework(acAppPath, "NiPluginToolkit Testbed",
        GAMEBRYO_SDK_VERSION_STRING);

    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        NiSystemDesc::RENDERER_GENERIC);

    NiShadowManager::Initialize();

    // Use the tool image converter
    NiImageConverter::SetImageConverter(NiDevImageQuantizer::Create());

    {
        NiStringTokenizer strTok(pcPluginPath);
        while (!strTok.IsEmpty())
        {
            NiString strPath = strTok.GetNextToken(";");
            int iPtr = strPath.FindReverse('\\');
            NIASSERT(iPtr >= 0);
            if ((unsigned int) iPtr < strPath.Length() - 1)
            {
                strPath += '\\';
            }

    #if _MSC_VER == 1310 //VC7.1
            strPath += "Lib\\VC71\\";
    #elif _MSC_VER == 1400 //VC8.0
            strPath += "Lib\\VC80\\";
    #elif _MSC_VER == 1500 //VC9.0
            strPath += "Lib\\VC90\\";
    #else
        #error Unsupported version of Visual Studio
    #endif

            NiPluginManager::GetInstance()->AddPluginDirectory(strPath);
        }
    }

    NiMaterialToolkit::CreateToolkit();

#if defined(_MSC_VER) && _MSC_VER >= 1400
    char acTemp[NI_MAX_PATH];
    size_t stBufferLength = 0;
    if (getenv_s(&stBufferLength, NULL, 0, "EGB_SHADER_LIBRARY_PATH") == 0 &&
        stBufferLength > 0)
    {
        getenv_s(&stBufferLength, acTemp, NI_MAX_PATH, 
            "EGB_SHADER_LIBRARY_PATH");
    }
    else
    {
        acTemp[0] = '\0';
    }
    CString strShaderPath(acTemp);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    CString strShaderPath = getenv("EGB_SHADER_LIBRARY_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (strShaderPath.IsEmpty())
    {
        NiMessageBox("The environment variable EGB_SHADER_LIBRARY_PATH "
            "must\nbe defined for this application to properly execute.",
            "Missing Environment Variable");
    }
    else
    {
        NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
        pkToolkit->LoadFromDLL(strShaderPath);

        CString strShaderDir = strShaderPath + "\\Data\\" +
            "DX9\\";
        pkToolkit->SetMaterialDirectory(strShaderDir);
    }


    // Create the mesh profile data
    NiMeshProfileProcessor::CreateMeshProfileProcessor();
    NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor();

    {
        CToolPluginTestbedDlg dlg;
        m_pMainWnd = &dlg;
        dlg.DoModal();
    }

    // Destory the mesh profile data
    NiMeshProfileProcessor::DestroyMeshProfileProcessor();

    NiFramework::GetFramework().GetSharedDataList().Lock();
    NiFramework::GetFramework().GetSharedDataList().DeleteAll();
    NiFramework::GetFramework().GetSharedDataList().Unlock();

    NiMaterialToolkit::UnloadShaders();
    NiMaterialToolkit::DestroyToolkit();
    NiShadowManager::Shutdown();
    NiFramework::ShutdownFramework();

    // Since the dialog has been closed, return FALSE so that we exit the
    // application, rather than start the application's message pump.
    return FALSE;
}
//---------------------------------------------------------------------------
