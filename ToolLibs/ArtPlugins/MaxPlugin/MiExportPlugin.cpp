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
#include "MiExportPlugin.h"

#include <NiScriptInfoDialogs.h>
#include "MiUtils.h"
#include "MiResource.h"
#include <NiDefaultScriptWriter.h>
#include <direct.h> // for _mkdir
#include "NiMAXExportSettingsDialog.h"
#include <NiPluginHelpers.h>

NiScriptInfoSetPtr MiExportPlugin::ms_spAvailableExportScripts = NULL;
NiScriptInfoSetPtr MiExportPlugin::ms_spAvailableProcessScripts = NULL;
//---------------------------------------------------------------------------
class MiExportPluginClassDesc : public ClassDesc 
{
public:
    inline int             IsPublic() { return 1; }
    inline void*           Create(BOOL) 
                    { return NiExternalNew MiExportPlugin; }
    inline const TCHAR*    ClassName() 
                    { return GetString(IDS_NI_EXPORT_CLASS_NAME); }
    inline SClass_ID       SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
    inline Class_ID        ClassID() { return MI_OUTPUT_CLASS_ID; }
    inline const TCHAR*    Category() { return _T(""); }
};
//---------------------------------------------------------------------------

static MiExportPluginClassDesc NvExportPluginDesc;

//---------------------------------------------------------------------------
ClassDesc* GetMAXExportClassDesc()
{
    return &NvExportPluginDesc;
}
//---------------------------------------------------------------------------
MiExportPlugin::MiExportPlugin()
{
    CHECK_MEMORY();
    m_bOptionsChanged = false;
    m_spNewProcessScript = NULL;
    m_spNewExportScript = NULL;
    NiTObjectArray<NiString> kFileDescriptorStrings;
    NiTObjectArray<NiString> kFileTypeStrings;
    NiPluginManager::GetInstance()->GetExportStrings(kFileDescriptorStrings, 
        kFileTypeStrings);
    m_kFileTypeStrings.SetSize(kFileTypeStrings.GetSize());
    for (unsigned int ui = 0; ui < kFileTypeStrings.GetSize(); ui++)
    {
        NiString strExt = kFileTypeStrings.GetAt(ui);
        unsigned int uiFind = strExt.FindReverse('.');
        strExt = strExt.GetSubstring(uiFind + 1, strExt.Length());
        m_kFileTypeStrings.Add(strExt.MakeExternalCopy());
    }
}
//---------------------------------------------------------------------------
MiExportPlugin::~MiExportPlugin() 
{
    for (unsigned int ui = 0; ui < m_kFileTypeStrings.GetSize(); ui++)
    {
        char *pcTemp = m_kFileTypeStrings.RemoveAt(ui);
        if (pcTemp)
            NiFree(pcTemp);
    }
}
//---------------------------------------------------------------------------
int MiExportPlugin::ExtCount() 
{
    return m_kFileTypeStrings.GetEffectiveSize();
}
//---------------------------------------------------------------------------
// Extensions supported for import/export modules
const TCHAR* MiExportPlugin::Ext(int n) 
{
    if (n < (int) m_kFileTypeStrings.GetEffectiveSize())
    {
        return m_kFileTypeStrings.GetAt(n);
    }
    else
    {
        return _T("");
    }
}
//---------------------------------------------------------------------------
// Long ASCII description (i.e. "Targa 2.0 Image File")
const TCHAR* MiExportPlugin::LongDesc() 
{
    return GetString(IDS_NI_EXPORT_LONG_DESC);
}
//---------------------------------------------------------------------------
// Short ASCII description (i.e. "Targa")
const TCHAR* MiExportPlugin::ShortDesc() 
{
    return GetString(IDS_NI_EXPORT_SHORT_DESC);
}
//---------------------------------------------------------------------------
// ASCII Author name
const TCHAR* MiExportPlugin::AuthorName() 
{
    return GetString(IDS_ROB_PHILLIPS);
}
//---------------------------------------------------------------------------
// ASCII Copyright message
const TCHAR* MiExportPlugin::CopyrightMessage() 
{
    return GetString(IDS_COPYRIGHT_NDL);
}
//---------------------------------------------------------------------------
// Other message #1
const TCHAR* MiExportPlugin::OtherMessage1() 
{
    return _T("");
}
//---------------------------------------------------------------------------
// Other message #2
const TCHAR* MiExportPlugin::OtherMessage2() 
{
    return _T("");
}
//---------------------------------------------------------------------------
// Version number * 100 (i.e. v3.01 = 301)
unsigned int MiExportPlugin::Version() 
{
    return 500;
}
//---------------------------------------------------------------------------
// Optional
void MiExportPlugin::ShowAbout(HWND) 
{
}
//---------------------------------------------------------------------------
int MiExportPlugin::DoExport(const TCHAR* pName, ExpInterface* pEI,
    Interface* pkIntf, BOOL bSuppressPrompts, DWORD options)
{
    return Export(pName, pEI, pkIntf, bSuppressPrompts, options);
}
//---------------------------------------------------------------------------
int MiExportPlugin::DoNiExportDialog(Interface* pkIntf)
{
    NiMAXExportSettingsDialog kDlg(hInstance, GetActiveWindow());
    int result = kDlg.DoModal();
    if (result == IDCANCEL) 
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        return IMPEXP_SUCCESS;
    }

    NiMAXOptions::WriteOptions(pkIntf);
    return 0;
}
//---------------------------------------------------------------------------
BOOL MiExportPlugin::SupportsOptions(int, DWORD options)
{
    bool bParam = SCENE_EXPORT_SELECTED;
    if (options && bParam)
    {
        return true;
    }
    else if (options == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
int MiExportPlugin::Export(const TCHAR* pName, ExpInterface*,
    Interface* pkIntf, BOOL bSuppressPrompts, DWORD options)
{
    CHECK_MEMORY();
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    bool bExportSelected = false;
    bool bParam = SCENE_EXPORT_SELECTED;
    if (options && bParam)
    {
        bExportSelected = true;
    }

    NiMAXOptions::g_kOptionsCS.Lock();
    NiMAXOptions::ReadOptions(pkIntf);
    NiMAXOptions::SetValue(NI_FILENAME, pName);

    if (!bSuppressPrompts) 
    {
        CHECK_MEMORY();
        if (DoNiExportDialog(pkIntf) == IMPEXP_SUCCESS)
            return IMPEXP_SUCCESS;
        CHECK_MEMORY();
    }

    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        (NiSystemDesc::RendererID)NiMAXOptions::GetInt(NI_EXPORT_PLATFORM));

    CHECK_MEMORY();
    bool bSuccess = NiMAXExporter::ExportScene(pkIntf, 
        NiMAXExporter::TO_FILE,
        NiMAXOptions::GetExportScript(),
        bExportSelected);
    CHECK_MEMORY();

    NiMAXOptions::g_kOptionsCS.Unlock();
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    if (bSuccess)
        return IMPEXP_SUCCESS;
    else
        return 0;
}
//---------------------------------------------------------------------------
Value* DoGamebryoExport_cf(Value** arg_list, int count) 
{
    // check we have 1 arg and that it's a string
    check_arg_count(GamebryoExport, 1, count);
    type_check(arg_list[0], String, "GamebryoExport Path");
    NiString strExportPath = arg_list[0]->to_string();
    
    NiString strPath, strFile, strExtension;
    if (NiPluginHelpers::DoExportFileDialog(strPath, strFile, strExtension,
        NULL, 
        strExportPath))
    {
        strExportPath = strPath + strFile + strExtension;
    }
    else
    {
        return &ok;
    }

    MiExportPlugin::Export(strExportPath, NULL, GetCOREInterface());
    return &ok;
}

def_visible_primitive(DoGamebryoExport, "GamebryoExport");
//---------------------------------------------------------------------------
Value* DoGamebryoExportSelected_cf(Value** arg_list, int count) 
{
    // check we have 1 arg and that it's a string
    check_arg_count(GamebryoExportSelected, 1, count);
    type_check(arg_list[0], String, "GamebryoExportSelected Path");
    NiString strExportPath = arg_list[0]->to_string();
    
    NiString strPath, strFile, strExtension;
    if (NiPluginHelpers::DoExportFileDialog(strPath, strFile, strExtension,
        NULL, 
        strExportPath))
    {
        strExportPath = strPath + strFile + strExtension;
    }
    else
    {
        return &ok;
    }

    MiExportPlugin::Export(strExportPath, NULL, GetCOREInterface(), FALSE,
        SCENE_EXPORT_SELECTED);
    return &ok;
}

def_visible_primitive(DoGamebryoExportSelected, "GamebryoExportSelected");
//---------------------------------------------------------------------------
Value* DoGamebryoExportSettingsDialog_cf(Value**, int) 
{
    // check we have 1 arg and that it's a string
    Interface* pkIntf = GetCOREInterface();
    NiMAXOptions::g_kOptionsCS.Lock();
    NiMAXOptions::ReadOptions(pkIntf);
    MiExportPlugin::DoNiExportDialog(pkIntf);
    NiMAXOptions::g_kOptionsCS.Unlock();
    return &ok;
}

def_visible_primitive(DoGamebryoExportSettingsDialog,
    "DoGamebryoExportSettingsDialog");
//---------------------------------------------------------------------------
