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

#include <stdafx.h>
#include <NiString.h>
#include "NiSceneGraphOptimizationDialog.h"
#include "NiSceneGraphOptimization_resource.h"
#include "NiSceneGraphOptimizationDefines.h"

//---------------------------------------------------------------------------
NiSceneGraphOptimizationDialog::NiSceneGraphOptimizationDialog(
    NiModuleRef hInstance, NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo)
    : NiDialog(IDD_OPTIMIZATION_OPTIONS, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
NiSceneGraphOptimizationDialog::~NiSceneGraphOptimizationDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::InitDialog()
{
    // Load options.
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_DUP_PROPERTIES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_BAD_TEXTURING_PROPS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_MULTIMTL_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_CHILDLESS_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_SINGLE_CHILD_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES,
        m_spPluginInfoInitial->GetBool(OPT_MERGE_SIBLING_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES,
        m_spPluginInfoInitial->GetBool(OPT_MERGE_SIBLING_TRISHAPES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA,
        m_spPluginInfoInitial->GetBool(OPT_NO_MERGE_ON_ALPHA));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES,
        m_spPluginInfoInitial->GetBool(OPT_OPTIMIZE_TRISHAPES));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_MESHSIZEMODIFIER), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_MESH_SIZE_MODIFIER));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_MESHNORMALMODIFIER), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_MESH_NORMAL_MODIFIER));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_MESHPOSITIONMODIFIER), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_MESH_POSITION_MODIFIER));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_DX9_MESHCACHESIZE), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_DX9_MESH_VERTEX_CACHE_SIZE));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_D3D10_MESHCACHESIZE), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_D3D10_MESH_VERTEX_CACHE_SIZE));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_XENON_MESHCACHESIZE), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_XENON_MESH_VERTEX_CACHE_SIZE));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_PS3_MESHCACHESIZE), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_PS3_MESH_VERTEX_CACHE_SIZE));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_WII_MESHCACHESIZE), m_spPluginInfoInitial
        ->GetValue(OPT_OPTIMIZE_WII_MESH_VERTEX_CACHE_SIZE));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_EXTRA_UVSETS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_UNNECESSARY_NORMALS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS,
        m_spPluginInfoInitial->GetBool(
        OPT_REMOVE_UNNECESSARY_VISCONTROLLERS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS,
        m_spPluginInfoInitial->GetBool(
        OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEMESH,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_HIDDEN_BONE_MESH));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS,
        m_spPluginInfoInitial->GetBool(OPT_REDUCE_ANIMATION_KEYS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), m_spPluginInfoInitial
        ->GetValue(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA,
            m_spPluginInfoInitial->GetBool(
            OPT_REMOVE_UNNECESSARY_EXTRA_DATA));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS,
        m_spPluginInfoInitial->GetBool(OPT_REMOVE_SGOKEEP_TAGS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP,
        m_spPluginInfoInitial->GetBool(OPT_SORT_BY_TEXTURING_PROP));

    if (m_spPluginInfoInitial->GetBool(OPT_REMOVE_NAMES))
    {
        CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
            IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_REMOVENAMES);
    }
    else 
    {
        if (m_spPluginInfoInitial->GetBool(OPT_NAME_UNNAMED_OBJECTS))
        {
            CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
                IDC_OPT_RADIO_LEAVENAMES,
                IDC_OPT_RADIO_NAMEUNNAMEDOBJECTS);
        }
        else
        {
            CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
                IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_LEAVENAMES);
        }
    }
    
    EnableWindow(
        GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA),
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_MERGESIBLINGTRISHAPES));
    EnableWindow(
        GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE),
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REDUCEANIMATIONKEYS));
    EnableWindow(
        GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS),
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_WELDSKIN,
        m_spPluginInfoInitial->GetBool(OPT_WELD_SKIN));
    
    if (m_spPluginInfoInitial->GetBool(OPT_EXPORT_SOFTWARE_SKIN))
    {
        CheckRadioButton(m_hWnd, IDC_OPT_RADIO_SKINHW,
            IDC_OPT_RADIO_SKINSW, IDC_OPT_RADIO_SKINSW);
    }
    else 
    {
        CheckRadioButton(m_hWnd, IDC_OPT_RADIO_SKINHW,
            IDC_OPT_RADIO_SKINSW, IDC_OPT_RADIO_SKINHW);
    }

    SetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
        m_spPluginInfoInitial->GetValue(OPT_BONES_PER_PARTITION));

    SetSkinAndBonesOptionsForPlatform();
}
//---------------------------------------------------------------------------
BOOL NiSceneGraphOptimizationDialog::OnCommand(int iWParamLow,
    int, long)
{
    switch(iWParamLow)
    {
    case IDOK:
        // Save options.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    case IDC_OPT_CHECK_MERGESIBLINGTRISHAPES:
        EnableWindow( GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA),
            IsDlgButtonChecked(m_hWnd,
            IDC_OPT_CHECK_MERGESIBLINGTRISHAPES) == 1);
        return TRUE;
    case IDC_OPT_CHECK_REDUCEANIMATIONKEYS:
        EnableWindow( GetDlgItem(m_hWnd,
            IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE),
            IsDlgButtonChecked(m_hWnd,
            IDC_OPT_CHECK_REDUCEANIMATIONKEYS) == 1);
        return TRUE;
    case IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA:
        EnableWindow( GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS),
            IsDlgButtonChecked(m_hWnd,
            IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA) == 1);
        return TRUE;
    case IDC_OPT_BUTTON_RESETDEFAULTS:
        ResetDefaultDialogOptions();
        return TRUE;
    case IDC_OPT_BUTTON_UNCHECKALL:
        // Uncheck all options.
        UncheckAllDialogOptions();
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiSceneGraphOptimizationDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::SetSkinAndBonesOptionsForPlatform()
{
    NiString strValue = m_spPluginInfoResults->GetValue("TexturePlatform");
    int iPlatform;
    if(strValue.ToInt(iPlatform))
    {
        switch (iPlatform)
        {
        case NiDevImageConverter::PLAYSTATION2:
            EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
                FALSE);
            break;
        default:
            EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
                TRUE);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::ResetDefaultDialogOptions()
{
    InitDialog();
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::UncheckAllDialogOptions()
{
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS,
        FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS,
        FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEMESH, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP, FALSE);
    CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
        IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_LEAVENAMES);
    EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA), FALSE);
    EnableWindow(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), FALSE);
    EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS), FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_WELDSKIN, FALSE);  
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::HandleOK()
{
    m_spPluginInfoResults->SetBool(OPT_REMOVE_DUP_PROPERTIES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_BAD_TEXTURING_PROPS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_MULTIMTL_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_CHILDLESS_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_SINGLE_CHILD_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_MERGE_SIBLING_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_MERGE_SIBLING_TRISHAPES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_NO_MERGE_ON_ALPHA,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_OPTIMIZE_TRISHAPES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES) ==
        BST_CHECKED);

    char acBuffer[64];
    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_MESHSIZEMODIFIER), acBuffer, 
        sizeof(acBuffer));
    float fValue = (float)atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_OPTIMIZE_MESH_SIZE_MODIFIER,
        fValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_MESHNORMALMODIFIER), 
        acBuffer, sizeof(acBuffer));
    fValue = (float)atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_OPTIMIZE_MESH_NORMAL_MODIFIER,
        fValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_MESHPOSITIONMODIFIER), 
        acBuffer, sizeof(acBuffer));
    fValue = (float)atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_OPTIMIZE_MESH_POSITION_MODIFIER,
        fValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_DX9_MESHCACHESIZE), acBuffer,
        sizeof(acBuffer));
    int iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_OPTIMIZE_DX9_MESH_VERTEX_CACHE_SIZE,
        iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_D3D10_MESHCACHESIZE), 
        acBuffer, sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_OPTIMIZE_D3D10_MESH_VERTEX_CACHE_SIZE,
        iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_XENON_MESHCACHESIZE), 
        acBuffer, sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_OPTIMIZE_XENON_MESH_VERTEX_CACHE_SIZE,
        iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_PS3_MESHCACHESIZE), acBuffer,
        sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_OPTIMIZE_PS3_MESH_VERTEX_CACHE_SIZE,
        iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_WII_MESHCACHESIZE), acBuffer,
        sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_OPTIMIZE_WII_MESH_VERTEX_CACHE_SIZE,
        iValue);

    m_spPluginInfoResults->SetBool(OPT_REMOVE_EXTRA_UVSETS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_NORMALS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_VISCONTROLLERS,
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS,
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_HIDDEN_BONE_MESH,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEMESH) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REDUCE_ANIMATION_KEYS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS) ==
        BST_CHECKED);

    char acTolerance[100];
    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), &acTolerance[0], 100);
    float fNewTolerance = (float) atof(acTolerance);
    if (fNewTolerance > 0.0f)
    {
        m_spPluginInfoResults->SetFloat(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE,
            fNewTolerance);
    }

    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_EXTRA_DATA,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA)
        == BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_SGOKEEP_TAGS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_SORT_BY_TEXTURING_PROP,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP) ==
        BST_CHECKED);

    if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_REMOVENAMES) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_REMOVE_NAMES, true);
        m_spPluginInfoResults->SetBool(OPT_NAME_UNNAMED_OBJECTS, false);
    }
    else if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_NAMEUNNAMEDOBJECTS) ==
        BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_REMOVE_NAMES, false);
        m_spPluginInfoResults->SetBool(OPT_NAME_UNNAMED_OBJECTS, true);
    }
    else
    {
        m_spPluginInfoResults->SetBool(OPT_REMOVE_NAMES, false);
        m_spPluginInfoResults->SetBool(OPT_NAME_UNNAMED_OBJECTS, false);
    }

    bool bWeld = (IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_WELDSKIN) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_WELD_SKIN, bWeld);

    if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_SKINSW) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_EXPORT_SOFTWARE_SKIN, true);
        m_spPluginInfoResults->SetBool(OPT_EXPORT_HARDWARE_SKIN, false);
    }
    else if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_SKINHW) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_EXPORT_SOFTWARE_SKIN, false);
        m_spPluginInfoResults->SetBool(OPT_EXPORT_HARDWARE_SKIN, true);
    }
    else
    {
        NIASSERT(!"UNKNOWN STATE");
    }
    
    char acBonesPerPartition[100];
    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
        &acBonesPerPartition[0], 100);
    unsigned int uiNewBPP = atoi(acBonesPerPartition);
    if (uiNewBPP > 3 && uiNewBPP <= 256)
    {
        m_spPluginInfoResults->SetInt(OPT_BONES_PER_PARTITION,
            (int) uiNewBPP);
    }
}
//---------------------------------------------------------------------------
