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
#include "AssetAnalyzerDialog.h"
#include "AssetAnalyzerPluginDefines.h"
#include "resource.h"

//---------------------------------------------------------------------------
AssetAnalyzerDialog::AssetAnalyzerDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_PROCESS_DIALOG, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
AssetAnalyzerDialog::~AssetAnalyzerDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::InitDialog()
{
    // Max Triangle Count
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT, 
        IDC_USE_MAX_TRIANGLE_COUNT, IDC_MAX_TRIANGLE_COUNT);

    SetDialogIntField(ASSET_ANALYZER_MAX_TRIANGLE_COUNT,
        IDC_MAX_TRIANGLE_COUNT);

    // Max Scene Memory Size
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE, 
        IDC_USE_MAX_MEMORY_SIZE, IDC_MAX_MEMORY_SIZE);

    SetDialogIntField(ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE,
        IDC_MAX_MEMORY_SIZE, 1024);

    // Max Total Texture Size
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE, 
        IDC_USE_MAX_SCENE_TEXTURE_SIZE, IDC_MAX_SCENE_TEXTURE_SIZE);

    SetDialogIntField(ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE,
        IDC_MAX_SCENE_TEXTURE_SIZE, 1024);

    // Max Object Count
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAX_OBJECT_COUNT, 
        IDC_USE_MAX_SCENE_OBJECT_COUNT, IDC_MAX_SCENE_OBJECT_COUNT);

    SetDialogIntField(ASSET_ANALYZER_MAX_OBJECT_COUNT,
        IDC_MAX_SCENE_OBJECT_COUNT);

    // Min Triangle to Mesh Ratio
    SetDialogCheckBox(ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_MESH_RATIO, 
        IDC_USE_TRIANGLE_MESH_RATIO, IDC_TRIANGLE_TO_MESH_RATIO);

    SetDialogFloatField(ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_MESH_RATIO,
        IDC_TRIANGLE_TO_MESH_RATIO);

    // Multi Sub Object Warings
    SetDialogCheckBox(ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS, 
        IDC_USE_MULTI_SUBOBJECT_WARNING, IDC_MULTI_SUB_OBJECT_WARNINGS);

    SetDialogIntField(ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS,
        IDC_MULTI_SUB_OBJECT_WARNINGS);

    // Max Morphing Vertex Per Object        
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT, 
        IDC_USE_MAX_MORPHING_VERTEX_COUNT, IDC_MAX_MORPHING_VERTEX_COUNT);

    SetDialogIntField(ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT,
        IDC_MAX_MORPHING_VERTEX_COUNT);

    // Max Triangle Count Per Object
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT, 
        IDC_USE_MAX_TRIANGLES_PER_OBJECT, IDC_MAX_TRIANGLES_PER_OBJECT);

    SetDialogIntField(ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT,
        IDC_MAX_TRIANGLES_PER_OBJECT);

    // Max Scene Graph Depth
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH, 
        IDC_USE_MAX_SCENE_GRAPH_DEPTH, IDC_MAX_SCENE_GRAPH_DEPTH);

    SetDialogIntField(ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH,
        IDC_MAX_SCENE_GRAPH_DEPTH);

    // Max Lights Per Object
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT, 
        IDC_USE_MAX_LIGHTS_PER_OBJECT, IDC_MAX_LIGHTS_PER_OBJECT);

    SetDialogIntField(ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT,
        IDC_MAX_LIGHTS_PER_OBJECT);



    // SETUP STUFF FOR THE REQUIRED OBJECTS
    if (m_spPluginInfoInitial->GetBool(ASSET_ANALYZER_USE_REQUIRED_OBJECTS))
    {
        CheckDlgButton(m_hWnd, IDC_USE_REQUIRED_OBJECTS, BST_CHECKED);

        EnableWindow(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_NEW_REQUIRED_OBJECT), true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_ADD_REQUIRED_OBJECT), true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), true);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_USE_REQUIRED_OBJECTS, BST_UNCHECKED);
        
        EnableWindow(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_NEW_REQUIRED_OBJECT), false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_ADD_REQUIRED_OBJECT), false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), false);
    }

    LoadRequiredObjects();

    LoadTextureSize();

    // Setup the Light Options
    CheckDlgButton(m_hWnd, IDC_NO_SPOT_LIGHTS, 
        m_spPluginInfoInitial->GetBool(ASSET_ANALYZER_NO_SPOT_LIGHTS));

    CheckDlgButton(m_hWnd, IDC_NO_POINT_LIGHTS, 
        m_spPluginInfoInitial->GetBool(ASSET_ANALYZER_NO_POINT_LIGHTS));

    CheckDlgButton(m_hWnd, IDC_NO_DIRECTIONAL_LIGHTS, 
        m_spPluginInfoInitial->GetBool(ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS));

    CheckDlgButton(m_hWnd, IDC_NO_AMBIENT_LIGHTS, 
        m_spPluginInfoInitial->GetBool(ASSET_ANALYZER_NO_AMBIENT_LIGHTS));


}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::SetDialogCheckBox(char* pcPluginInfo, int iButton,
    int iField)
{
    if (m_spPluginInfoInitial->GetBool(pcPluginInfo))
    {
        CheckDlgButton(m_hWnd, iButton, BST_CHECKED);

        if (iField != 0)
        {
            EnableWindow(GetDlgItem(m_hWnd, iField), true);
        }
    }
    else
    {
        CheckDlgButton(m_hWnd, iButton, BST_UNCHECKED);

        if (iField != 0)
        {
            EnableWindow(GetDlgItem(m_hWnd, iField), false);
        }
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::GetDialogCheckBox(char* pcPluginInfo, int iButton)
{
    m_spPluginInfoResults->SetBool(pcPluginInfo, IsDlgButtonChecked(m_hWnd,
        iButton) == BST_CHECKED);
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::SetDialogIntField(char* pcPluginInfo, int iField,
    int iUnits /*= 1*/)
{
    NiString kTemp = NiString::FromInt(
        m_spPluginInfoInitial->GetInt(pcPluginInfo) / iUnits);

    SetWindowText(GetDlgItem(m_hWnd, iField), kTemp);
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::GetDialogIntField(char* pcPluginInfo, int iField,
    int iUnits /*= 1*/)
{
    // Get the string from windows
    char pcBuffer[128];
    GetWindowText(GetDlgItem(m_hWnd, iField), pcBuffer, 128);

    // Convert to an integer
    NiString kTemp = pcBuffer;
    int iIntValue = 0;

    if (kTemp.ToInt(iIntValue))
    {
        m_spPluginInfoResults->SetInt(pcPluginInfo, iIntValue * iUnits);
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::SetDialogFloatField(char* pcPluginInfo, int iField)
{
    NiString kTemp = NiString::FromFloat(
        m_spPluginInfoInitial->GetFloat(pcPluginInfo));

    SetWindowText(GetDlgItem(m_hWnd, iField), kTemp);
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::GetDialogFloatField(char* pcPluginInfo, int iField)
{
    // Get the string from windows
    char pcBuffer[128];
    GetWindowText(GetDlgItem(m_hWnd, iField), pcBuffer, 128);

    // Convert to an integer
    NiString kTemp = pcBuffer;
    float fFloatValue = 0.0f;

    if (kTemp.ToFloat(fFloatValue))
    {
        m_spPluginInfoResults->SetFloat(pcPluginInfo, fFloatValue);
    }
}
//---------------------------------------------------------------------------
BOOL AssetAnalyzerDialog::OnCommand(int iWParamLow, int iWParamHigh,
    long)
{
    switch (iWParamLow)
    {
    case IDC_USE_MAX_TRIANGLE_COUNT:
    case IDC_USE_MAX_TEXTURE_SIZE:
    case IDC_USE_MAX_MEMORY_SIZE:
    case IDC_USE_MAX_SCENE_TEXTURE_SIZE:
    case IDC_USE_MAX_SCENE_OBJECT_COUNT:
    case IDC_USE_TRIANGLE_MESH_RATIO:
    case IDC_USE_MULTI_SUBOBJECT_WARNING:
    case IDC_USE_MAX_MORPHING_VERTEX_COUNT:
    case IDC_USE_MAX_TRIANGLES_PER_OBJECT:
    case IDC_USE_MAX_SCENE_GRAPH_DEPTH:
    case IDC_USE_MAX_LIGHTS_PER_OBJECT:
    case IDC_USE_REQUIRED_OBJECTS:
        ToggleCheckBoxes(iWParamLow);
        break;
    case IDC_ADD_REQUIRED_OBJECT:
        AddRequiredObject();
        break;
    case IDC_REQUIRED_OBJECTS:
        
        if (iWParamHigh == LBN_DBLCLK)
        {
            RemoveRequiredObject();
        }
        break;
    case IDOK:

        // Handle Enter from the Add Require Object
        if (GetFocus() == GetDlgItem(m_hWnd,IDC_NEW_REQUIRED_OBJECT))
        {
            AddRequiredObject();
            return FALSE;
        }

        // Handle the OK button.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        // Handle the cancel button.
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    // If you have any logic for updating any of the GUI, add the handlers
    // here. Return true for anything that you handle.
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::ToggleCheckBoxes(int iWParamLow)
{
    bool bChecked = IsDlgButtonChecked(m_hWnd, iWParamLow) == BST_CHECKED;

    switch (iWParamLow)
    {
    case IDC_USE_MAX_TRIANGLE_COUNT:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_TRIANGLE_COUNT), bChecked);
        break;
    case IDC_USE_MAX_TEXTURE_SIZE:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), bChecked);
        break;
    case IDC_USE_MAX_MEMORY_SIZE:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_MEMORY_SIZE), bChecked);
        break;
    case IDC_USE_MAX_SCENE_TEXTURE_SIZE:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_SCENE_TEXTURE_SIZE), bChecked);
        break;
    case IDC_USE_MAX_SCENE_OBJECT_COUNT:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_SCENE_OBJECT_COUNT), bChecked);
        break;
    case IDC_USE_TRIANGLE_MESH_RATIO:
        EnableWindow(GetDlgItem(m_hWnd, IDC_TRIANGLE_TO_MESH_RATIO), 
            bChecked);
        break;
    case IDC_USE_MULTI_SUBOBJECT_WARNING:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MULTI_SUB_OBJECT_WARNINGS), 
            bChecked);
        break;
    case IDC_USE_MAX_MORPHING_VERTEX_COUNT:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_MORPHING_VERTEX_COUNT), 
            bChecked);
        break;
    case IDC_USE_MAX_TRIANGLES_PER_OBJECT:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_TRIANGLES_PER_OBJECT), 
            bChecked);
        break;
    case IDC_USE_MAX_SCENE_GRAPH_DEPTH:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_SCENE_GRAPH_DEPTH), bChecked);
        break;
    case IDC_USE_MAX_LIGHTS_PER_OBJECT:
        EnableWindow(GetDlgItem(m_hWnd, IDC_MAX_LIGHTS_PER_OBJECT), bChecked);
        break;
    case IDC_USE_REQUIRED_OBJECTS:
    // SETUP STUFF FOR THE REQUIRED OBJECTS
        EnableWindow(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), bChecked);
        EnableWindow(GetDlgItem(m_hWnd, IDC_NEW_REQUIRED_OBJECT), bChecked);
        EnableWindow(GetDlgItem(m_hWnd, IDC_ADD_REQUIRED_OBJECT), bChecked);
        EnableWindow(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), bChecked);
        break;
    }
}
//---------------------------------------------------------------------------
NiPluginInfoPtr AssetAnalyzerDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::HandleOK()
{


    // Max Triangle Count
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT, 
        IDC_USE_MAX_TRIANGLE_COUNT);

    GetDialogIntField(ASSET_ANALYZER_MAX_TRIANGLE_COUNT,
        IDC_MAX_TRIANGLE_COUNT);

    // Max Scene Memory Size
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE, 
        IDC_USE_MAX_MEMORY_SIZE);

    GetDialogIntField(ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE,
        IDC_MAX_MEMORY_SIZE, 1024);

    // Max Total Texture Size
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE, 
        IDC_USE_MAX_SCENE_TEXTURE_SIZE);

    GetDialogIntField(ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE,
        IDC_MAX_SCENE_TEXTURE_SIZE, 1024);

    // Max Object Count
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAX_OBJECT_COUNT, 
        IDC_USE_MAX_SCENE_OBJECT_COUNT);

    GetDialogIntField(ASSET_ANALYZER_MAX_OBJECT_COUNT,
        IDC_MAX_SCENE_OBJECT_COUNT);

    // Min Triangle to Mesh Ratio
    GetDialogCheckBox(ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_MESH_RATIO, 
        IDC_USE_TRIANGLE_MESH_RATIO);

    GetDialogFloatField(ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_MESH_RATIO,
        IDC_TRIANGLE_TO_MESH_RATIO);

    // Multi Sub Object Warings
    GetDialogCheckBox(ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS, 
        IDC_USE_MULTI_SUBOBJECT_WARNING);

    GetDialogIntField(ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS,
        IDC_MULTI_SUB_OBJECT_WARNINGS);

    // Max Morphing Vertex Per Object        
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT, 
        IDC_USE_MAX_MORPHING_VERTEX_COUNT);

    GetDialogIntField(ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT,
        IDC_MAX_MORPHING_VERTEX_COUNT);

    // Max Triangle Count Per Object
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT, 
        IDC_USE_MAX_TRIANGLES_PER_OBJECT);

    GetDialogIntField(ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT,
        IDC_MAX_TRIANGLES_PER_OBJECT);

    // Max Scene Graph Depth
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH, 
        IDC_USE_MAX_SCENE_GRAPH_DEPTH);

    GetDialogIntField(ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH,
        IDC_MAX_SCENE_GRAPH_DEPTH);

    // Max Lights Per Object
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT, 
        IDC_USE_MAX_LIGHTS_PER_OBJECT);

    GetDialogIntField(ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT,
        IDC_MAX_LIGHTS_PER_OBJECT);


    GetDialogCheckBox(ASSET_ANALYZER_USE_REQUIRED_OBJECTS, 
        IDC_USE_REQUIRED_OBJECTS);


    SaveRequiredObjects();

    SaveTextureSize();

    // Handle the Light Options
    GetDialogCheckBox(ASSET_ANALYZER_NO_SPOT_LIGHTS, IDC_NO_SPOT_LIGHTS);
    GetDialogCheckBox(ASSET_ANALYZER_NO_POINT_LIGHTS, IDC_NO_POINT_LIGHTS);
    GetDialogCheckBox(ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS, 
        IDC_NO_DIRECTIONAL_LIGHTS);
    GetDialogCheckBox(ASSET_ANALYZER_NO_AMBIENT_LIGHTS, IDC_NO_AMBIENT_LIGHTS);
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::AddRequiredObject()
{
    // Get the Current Text
    char acBuffer[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_NEW_REQUIRED_OBJECT), acBuffer, 256);

    // Clear the Text
    SetWindowText(GetDlgItem(m_hWnd, IDC_NEW_REQUIRED_OBJECT), "");

    SendMessage(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), 
        LB_ADDSTRING, 0, PtrToLong(acBuffer));

}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::RemoveRequiredObject()
{

    // See What is selected
    int iSelect = (int)SendMessage(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), 
        LB_GETCURSEL, 0, 0);

    // Delete that Item
    SendMessage(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), LB_DELETESTRING,
            iSelect, 0); 

}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::LoadRequiredObjects()
{
    int iNumRequiredObjects = m_spPluginInfoInitial->GetInt(
        ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS);
    
    for(int iLoop = 0; iLoop < iNumRequiredObjects; iLoop++)
    {
        // Create the Key
        NiString kKey(ASSET_ANALYZER_REQUIRED_OBJECTS);
        kKey += NiString::FromInt(iLoop);

        // Read the Value
        NiString kValue = m_spPluginInfoInitial->GetValue(kKey);

        // Add to the List Box
        SendMessage(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), 
            LB_ADDSTRING, 0, PtrToLong((const char*)kValue));
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::SaveRequiredObjects()
{
    // Remove the old Require Objects
    int iNumRequiredObjects = m_spPluginInfoInitial->GetInt(
        ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS);
    
    int iLoop = 0;
    for(; iLoop < iNumRequiredObjects; iLoop++)
    {
        // Create the Key
        NiString kKey(ASSET_ANALYZER_REQUIRED_OBJECTS);
        kKey += NiString::FromInt(iLoop);

        m_spPluginInfoResults->RemoveParameter(kKey);
    }

    // Add the New Required Objects
    iNumRequiredObjects = (int)SendMessage(GetDlgItem(m_hWnd,
        IDC_REQUIRED_OBJECTS), LB_GETCOUNT, 0, 0);

    m_spPluginInfoResults->SetInt(
        ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS, iNumRequiredObjects);

    for(iLoop = 0; iLoop < iNumRequiredObjects; iLoop++)
    {
        // Create the Key
        NiString kKey(ASSET_ANALYZER_REQUIRED_OBJECTS);
        kKey += NiString::FromInt(iLoop);

        char acObject[512];
        SendMessage(GetDlgItem(m_hWnd, IDC_REQUIRED_OBJECTS), 
            LB_GETTEXT, iLoop, PtrToLong(acObject));

        m_spPluginInfoResults->AddParameter(kKey, acObject);
    }


}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::LoadTextureSize()
{
    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("32"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("64"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("128"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("256"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("512"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("1024"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("2048"));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_ADDSTRING, 0, PtrToLong("4096"));


    // Max Texture Size
    SetDialogCheckBox(ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE, 
        IDC_USE_MAX_TEXTURE_SIZE, IDC_MAX_TEXTURE_SIZE);


    NiString kValue = NiString::FromInt(m_spPluginInfoInitial->GetInt(
        ASSET_ANALYZER_MAX_TEXTURE_SIZE));

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_SELECTSTRING, 0, PtrToLong((const char*)kValue));


}
//---------------------------------------------------------------------------
void AssetAnalyzerDialog::SaveTextureSize()
{
    // Max Texture Size
    GetDialogCheckBox(ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE, 
        IDC_USE_MAX_TEXTURE_SIZE);


    int iIndex = (int)SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_GETCURSEL, 0, 0);

    char acBuffer[128];

    SendMessage(GetDlgItem(m_hWnd, IDC_MAX_TEXTURE_SIZE), 
        CB_GETLBTEXT, iIndex, PtrToLong(acBuffer));

    NiString kString(acBuffer);
    int iTextureSize = 0;
    
    kString.ToInt(iTextureSize);

    m_spPluginInfoResults->SetInt(ASSET_ANALYZER_MAX_TEXTURE_SIZE, 
        iTextureSize);
}
//---------------------------------------------------------------------------
