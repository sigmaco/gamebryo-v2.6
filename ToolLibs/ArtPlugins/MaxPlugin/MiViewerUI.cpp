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
#include <NiMemStream.h>
#include <NiViewerPluginInfo.h>

#include "MiViewerPlugin.h"
#include "MiUtils.h"

#include "MiDlgProcAbout.h"
#include "MiDlgProcRegister.h"
#include "MiDlgProcRender.h"
#include "MiDlgProcInteraction.h"

#include "NiMAXAnimationConverter.h"
#include "NiMAXCameraConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXLODConverter.h"
#include "NiMAXMeshConverter.h"
#include "NiMAXMorphConverter.h"
#include "NiMAXLightConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXTextureConverter.h"

bool MiViewerPlugin::ms_bViewButtonVisible = true;

#define NIUTIL_PARAM_MAIN_LENGTH 0
#define NIUTIL_PARAM_ABOUT_LENGTH 0
#define NIUTIL_PARAM_QUICKSAVE_LENGTH 0

static int NvRendererIDs[] = 
{
    IDC_DX9_VIEWER,
    IDC_SCENEIMMERSE_VIEWER,
    IDC_ANIMATION_VIEWER,
    IDC_CUSTOM_VIEWER,
    IDC_XENON_VIEWER, 
    IDC_PS3_VIEWER, 
    IDC_WII_VIEWER, 
    IDC_PHYSX_VIEWER,
    IDC_D3D10_VIEWER
};

static ParamUIDesc NvDescParamRenderer[] = 
{
    ParamUIDesc(PB_RENDERTYPE, TYPE_RADIO,
            NvRendererIDs, 9),
    ParamUIDesc(PB_WINDOWED, TYPE_SINGLECHEKBOX,
            IDC_WINDOWED),
    ParamUIDesc(PB_WINDOW_X, EDITTYPE_INT,
            IDC_SP_WINDOW_X, IDC_SP_WINDOW_X_SPIN,
            1.0f, 2000.0f, 1.0f),   
    ParamUIDesc(PB_WINDOW_Y, EDITTYPE_INT,
            IDC_SP_WINDOW_Y, IDC_SP_WINDOW_Y_SPIN,
            1.0f, 2000.0f, 1.0f), 
    ParamUIDesc(PB_STRIPIFY_FOR_VIEWER, TYPE_SINGLECHEKBOX,
            IDC_STRIPIFYCHECK)
};
#define NIUTIL_PARAM_RENDERER_LENGTH 5

static ParamUIDesc NvDescParamInteraction[] = 
{
    ParamUIDesc(PB_CS_AS_KEYFRAME, TYPE_SINGLECHEKBOX, IDC_CS_AS_KEYFRAME)
};

#define NIUTIL_PARAM_INTERACTION_LENGTH 1

static ParamBlockDescID NvDescIDsv0[] = 
{
{ TYPE_INT, NULL, FALSE, PB_RENDERTYPE },     // renderer type
{ TYPE_INT, NULL, FALSE, PB_WINDOWED },       // windowed
{ TYPE_INT, NULL, FALSE, PB_WINDOW_X },       // window x resolution
{ TYPE_INT, NULL, FALSE, PB_WINDOW_Y },       // window y resolution
{ TYPE_INT, NULL, FALSE, PB_IKSOLVER },       // ik solver
{ TYPE_INT, NULL, FALSE, PB_IKSOLVEMODE },    // ik solver mode
{ TYPE_INT, NULL, FALSE, PB_CS_AS_KEYFRAME }, // convert CS animation as 
                                              // keyframe
{ TYPE_INT, NULL, FALSE, PB_STRIPIFY_FOR_VIEWER } // Stripify for viewer
};

#define NUM_OLDVERSIONS 0

// Current version
#define NI_CURRENT_VERSION 0
static ParamVersionDesc NvCurVersion(NvDescIDsv0,
    sizeof(NvDescIDsv0) / sizeof(NvDescIDsv0[0]),
    NI_CURRENT_VERSION);
//---------------------------------------------------------------------------
void MiViewerPlugin::EnableView()
{
    ms_bViewButtonVisible = true;
    if(m_pRendererPMap != NULL)
    {
       SendMessage(m_pRendererPMap->GetHWnd(), WM_HIDEVIEW,0,0);
    }
}
//---------------------------------------------------------------------------
void MiViewerPlugin::DisableView()
{
    ms_bViewButtonVisible = false;
    if(m_pRendererPMap != NULL)
    {
       SendMessage(m_pRendererPMap->GetHWnd(), WM_HIDEVIEW,0,0);
    }
}
//---------------------------------------------------------------------------
void MiViewerPlugin::View(ViewerType eViewerType, bool bViewSelected) 
{
    CHECK_MEMORY();
    NiMAXOptions::ReadOptions(GetCOREInterface());
    NiSystemDesc::RendererID eRenderer = NiSystemDesc::RENDERER_GENERIC;
    

    if (eViewerType <= MAX_USE_OPTIONS_VIEWER)
        eViewerType = (ViewerType) NiMAXOptions::GetInt(NI_VIEWER_TYPE);
    
    if (eViewerType >= MAX_VIEWER_TYPES)
    {
        NiMessageBox("You tried to launch an unknown viewer.",
            "Gamebryo Viewer Plugin Error!");
        return;
    }

    NiScriptInfoPtr spScript = NULL;

    if (eViewerType == MAX_DX9_VIEWER)
    {
        spScript = NiMAXOptions::GetDX9ViewerScript();
        eRenderer = NiSystemDesc::RENDERER_GENERIC;
    }
    else if (eViewerType == MAX_D3D10_VIEWER)
    {
        spScript = NiMAXOptions::GetD3D10ViewerScript();
        eRenderer = NiSystemDesc::RENDERER_D3D10;
    }
    else if (eViewerType == MAX_SCENEIMMERSE_VIEWER)
    {
        // Since we use the AssetViewer for both DX9 and D3D10
        // we use the selected D3D10 radio button to break the tie,
        ViewerType Viewer = (ViewerType) NiMAXOptions::GetInt(NI_VIEWER_TYPE);
        if ( Viewer == MAX_D3D10_VIEWER )
        {                                            
            spScript = NiMAXOptions::GetD3D10SceneImmerseViewerScript();
            eRenderer = NiSystemDesc::RENDERER_D3D10;
        }
        else
        {                                             
            spScript = NiMAXOptions::GetSceneImmerseViewerScript();
            eRenderer = NiSystemDesc::RENDERER_GENERIC;
        }
    }
    else if (eViewerType == MAX_ANIMATION_VIEWER)
    {
        spScript = NiMAXOptions::GetAnimationViewerScript();
        eRenderer = NiSystemDesc::RENDERER_GENERIC;
    }
    else if (eViewerType == MAX_XENON_VIEWER)
    {
        spScript = NiMAXOptions::GetXenonViewerScript();
        eRenderer = NiSystemDesc::RENDERER_XENON;
    }
    else if (eViewerType == MAX_PS3_VIEWER)
    {
        spScript = NiMAXOptions::GetPS3ViewerScript();
        eRenderer = NiSystemDesc::RENDERER_PS3;
    }
    else if (eViewerType == MAX_WII_VIEWER)
    {
        spScript = NiMAXOptions::GetWiiViewerScript();
        eRenderer = NiSystemDesc::RENDERER_WII;
    }
    else if (eViewerType == MAX_PHYSX_VIEWER)
    {
        spScript = NiMAXOptions::GetPhysXViewerScript();
        eRenderer = NiSystemDesc::RENDERER_DX9;
    }
    else if (eViewerType == MAX_CUSTOM_VIEWER)
    {
        spScript = NiMAXOptions::GetCustomViewerScript();
        if (spScript)
        {
            for (unsigned int ui = 0; ui < spScript->GetPluginInfoCount(); 
                ui++)
            {
                NiPluginInfo* pkInfo = spScript->GetPluginInfoAt(ui);
                if (NiIsKindOf(NiViewerPluginInfo, pkInfo))
                    eRenderer = ((NiViewerPluginInfo*)pkInfo)->GetRenderer();
            }
        }
    }
    else 
    {
        return;
    }

    if (!spScript)
        return;

    // Set the render id
    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(eRenderer);
    
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    if (pkManager && !pkManager->VerifyScriptInfo(spScript))
    {
        char acString[1024];
        NiSprintf(acString, 1024, "The viewer, \"%s\", cannot execute.\n"
            "This could be due to missing DLL's, invalid parameters, or "
            "missing files.", (const char*) spScript->GetName());
        
        NiMessageBox(acString, "Viewer Error");
        return;
    }

    CHECK_MEMORY();
    NiMAXExporter::ExportScene(GetCOREInterface(), NiMAXExporter::TO_VIEWER,
        spScript, bViewSelected);
    CHECK_MEMORY();

    NiMAXOptions::WriteOptions(GetCOREInterface()); 
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------

BOOL MiViewerPlugin::SetValue(int i, TimeValue, int v)
{
    CHECK_MEMORY();
    bool bWriteOptions = true;
    switch (i)
    {
    case PB_RENDERTYPE: 
        NiMAXOptions::SetInt(NI_VIEWER_TYPE, v); 
        break;
    case PB_WINDOWED:
        NiMAXOptions::SetBool(NI_WINDOWED, v != 0 ? true : false);
        break;
    case PB_WINDOW_X:
        NiMAXOptions::SetInt(NI_RESX, v);
        break;
    case PB_WINDOW_Y:
        NiMAXOptions::SetInt(NI_RESY, v);
        break;
    case PB_CS_AS_KEYFRAME:
        NiMAXOptions::SetBool(NI_CONVERT_CSASKEYFRAMES, v != 0 ? true : 
            false);
        break;
    case PB_STRIPIFY_FOR_VIEWER:
        NiMAXOptions::SetBool(NI_VIEWER_STRIPIFY, v != 0 ? true : false);
        break;
    default:
        bWriteOptions = false;
        break;
    }

    if (bWriteOptions)
        NiMAXOptions::WriteOptions(GetCOREInterface());

   CHECK_MEMORY();
   return(TRUE);
}

//---------------------------------------------------------------------------
BOOL MiViewerPlugin::GetValue(int i, TimeValue, int &v, Interval &)
{
    switch (i)
    {
    case PB_RENDERTYPE: 
        v = NiMAXOptions::GetInt(NI_VIEWER_TYPE);
        break;
    case PB_WINDOWED:
        v = (int) NiMAXOptions::GetBool(NI_WINDOWED);
        break;
    case PB_WINDOW_X:
        v = NiMAXOptions::GetInt(NI_RESX);
        break;
    case PB_WINDOW_Y:
        v = NiMAXOptions::GetInt(NI_RESY);
        break;
    case PB_CS_AS_KEYFRAME:
        v = (int) NiMAXOptions::GetBool(NI_CONVERT_CSASKEYFRAMES);
        break;
    case PB_STRIPIFY_FOR_VIEWER:
        v = (int) NiMAXOptions::GetBool(NI_VIEWER_STRIPIFY);
        break;
    }
    return(TRUE);
}

//---------------------------------------------------------------------------
BOOL MiViewerPlugin::SetValue(int i, TimeValue t, float v)
{
    return SetValue(i, t, (int) v);
}

//---------------------------------------------------------------------------
BOOL MiViewerPlugin::GetValue(int i, TimeValue t, float& v, Interval& iValid)
{
    int iVal = (int) v;
    BOOL b = GetValue(i, t, iVal, iValid);
    v = (float) iVal;
    return(b);
}

//---------------------------------------------------------------------------
void MiViewerPlugin::BeginEditParams(Interface* pIntf, IUtil*) 
{

    CHECK_MEMORY();
    m_pIntf = pIntf;

    NiMAXOptions::ReadOptions(GetCOREInterface());
    m_pAboutPMap = CreateCPParamMap(
        NULL,
        NIUTIL_PARAM_ABOUT_LENGTH,
        this,
        pIntf,
        hInstance,
        MAKEINTRESOURCE(IDD_ABOUT_PANEL),
        GetString(IDS_NI_ABOUT_LABEL),
        (m_iFlags & ROLLUP11_OPEN) ? 0 : APPENDROLL_CLOSED);

#ifdef QUICKSAVE
    m_pQuickSavePMap = CreateCPParamMap(
        NULL,   
        NIUTIL_PARAM_QUICKSAVE_LENGTH,
        this,
        pIntf,
        hInstance,
        MAKEINTRESOURCE(IDD_QUICKSAVE_PANEL),
        GetString(IDS_NI_QUICKSAVE_LABEL),
        (m_iFlags & ROLLUP2_OPEN) ? 0 : APPENDROLL_CLOSED);
#endif

    m_pRendererPMap = CreateCPParamMap(
        NvDescParamRenderer,
        NIUTIL_PARAM_RENDERER_LENGTH,
        this,
        pIntf,
        hInstance,
        MAKEINTRESOURCE(IDD_RENDERER_PANEL),
        GetString(IDS_NI_VIEWER_TYPE_LABEL),
        (m_iFlags & ROLLUP3_OPEN) ? 0 : APPENDROLL_CLOSED);

    m_pInteractionPMap = CreateCPParamMap(
        NvDescParamInteraction,
        NIUTIL_PARAM_INTERACTION_LENGTH,
        this,
        pIntf,
        hInstance,
        MAKEINTRESOURCE(IDD_INTERACTION_PANEL),
        GetString(IDS_NI_INTERACTION_LABEL),
        (m_iFlags & ROLLUP4_OPEN) ? 0 : APPENDROLL_CLOSED);
    

    if (m_pAboutPMap)
        m_pAboutPMap->SetUserDlgProc(NiExternalNew MiDlgProcAbout(this));

#ifdef QUICKSAVE
    if (m_pQuickSavePMap)
    {
        m_pQuickSavePMap->SetUserDlgProc(
            NiExternalNew MiDlgProcQuickSave(pIntf));
    }
#endif
    if (m_pRendererPMap)
        m_pRendererPMap->SetUserDlgProc(NiExternalNew MiDlgProcRenderer(this));
    if (m_pInteractionPMap)
    {
        m_pInteractionPMap->SetUserDlgProc(NiExternalNew
           MiDlgProcInteraction(this));
    }
    CHECK_MEMORY();

}
    
//---------------------------------------------------------------------------
void MiViewerPlugin::EndEditParams(Interface*, IUtil*) 
{
    CHECK_MEMORY();
    m_iFlags = 0;

    NiMAXOptions::WriteOptions(GetCOREInterface());

    if (m_pAboutPMap)
    {
        if (IsRollupPanelOpen(m_pAboutPMap->GetHWnd()))
            m_iFlags |= ROLLUP11_OPEN;
        DestroyCPParamMap(m_pAboutPMap); 
        m_pAboutPMap = NULL;
    }

#ifdef QUICKSAVE
    if (m_pQuickSavePMap)
    {
        if (IsRollupPanelOpen(m_pQuickSavePMap->GetHWnd()))
            m_iFlags |= ROLLUP2_OPEN;
        DestroyCPParamMap(m_pQuickSavePMap); 
        m_pQuickSavePMap = NULL;
    }
#endif

    if (m_pRendererPMap)
    {
        if (IsRollupPanelOpen(m_pRendererPMap->GetHWnd()))
            m_iFlags |= ROLLUP3_OPEN;
        DestroyCPParamMap(m_pRendererPMap); 
        m_pRendererPMap = NULL;
    }

    if (m_pInteractionPMap)
    {
        if (IsRollupPanelOpen(m_pInteractionPMap->GetHWnd()))
            m_iFlags |= ROLLUP4_OPEN; 
        DestroyCPParamMap(m_pInteractionPMap); 
        m_pInteractionPMap = NULL;
    }
    m_pIntf = NULL;
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void MiViewerPlugin::CleanUp()
{
}


//---------------------------------------------------------------------------
Value* DoGamebryoView_cf(Value** arg_list, int count) 
{
    // check we have 1 arg and that it's a string
    check_arg_count(DoGamebryoView, 1, count);
    type_check(arg_list[0], Integer, "DoGamebryoView ViewerType");
    int iViewerType = arg_list[0]->to_int();

    bool bViewSelected = false;

    MiViewerPlugin::View((MiViewerPlugin::ViewerType) iViewerType, 
        bViewSelected);
    return &ok;
}

def_visible_primitive( DoGamebryoView,
                      "DoGamebryoView");
//---------------------------------------------------------------------------
Value* DoGamebryoViewSelected_cf(Value** arg_list, int count) 
{
    // check we have 1 arg and that it's a string
    check_arg_count(DoGamebryoViewSelected, 1, count);
    type_check(arg_list[0], Integer, "DoGamebryoViewSelected ViewerType");
    int iViewerType = arg_list[0]->to_int();

    bool bViewSelected = true;

    MiViewerPlugin::View((MiViewerPlugin::ViewerType) iViewerType, 
        bViewSelected);
    return &ok;
}

def_visible_primitive( DoGamebryoViewSelected,
                      "DoGamebryoViewSelected");
//---------------------------------------------------------------------------

