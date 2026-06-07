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

#ifndef NI_MAX_PER_TEXTURE_DIALOG
#define NI_MAX_PER_TEXTURE_DIALOG

#include "NiMAXShader.h"
#include "NiMAX.h"
#include "MiResource.h"

#define MAXIMMERSE_CHANNELS 7
#define NI_NUM_EXPORT_SETTINGS 5
#define NI_PER_TEXTURE_VERSION 1

class NiMAXPerTextureDialog 
{
public:

    // Enumeration of map types used by the Dialog
    enum MapType
    {
        BASE, 
        DARK,
        DETAIL,
        DECAL1,
        DECAL2,
        GLOW,
        GLOSS,
        NI_NUM_MAP_TYPES
    };

    NiMAXPerTextureDialog(ShaderParamDlg* Parent, IParamBlock2* pblock);
    ~NiMAXPerTextureDialog();
    void ShowDialog(HINSTANCE hInstance, IParamBlock2* pblock);
    
    int GetMaxMapType(); // The max number of map types available

    int  GetTextureSettingForMap(MapType map); 
    void SetTextureSettingForMap(MapType map,
        NiMAXShader::TextureExportSetting setting);
    void SetTextureMapName(HWND hDlg,MapType map, char* filename);

protected:

    // Windows wants a static function pointer for use with its message 
    // handlers so we use the following PanelProc and theDlg to handle 
    // messages Panel Proc is called statically and then calls the 
    // HandleMessages method of theDlg
    static BOOL PanelProc(HWND hDlg, UINT iMsg, WPARAM wParam, 
        LPARAM lParam);
    static NiMAXPerTextureDialog* theDlg;

    ShaderParamDlg* m_pkParent; // Shader dialog window that spawned us
    IParamBlock2* m_pblock;     // Param block from spawning dialog window

    // Called from the static PanelProc to handle all class intrinsic
    // messages
    BOOL HandleMessages(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
    void LoadParams(HWND hDlg); // Setup and load paramater from paramblock
    void SaveParams(HWND hDlg); // Unused at the moment
    void SetupListBoxItems(HWND hwndCS); // Populates the list boxes with 
    //items
    
    // Converts the windows idcode to the actual channel index
    int IDC_ToIndex(int idc, const int ms_aIDCTable[]);

    // Convenience arrays holding the windows id numbers for components and
    // the max id numbers for the same components, allows for easy iteration
    static const char* 
        ms_aTextureExportSettingStrings[NI_NUM_EXPORT_SETTINGS];
    static const int m_aiTextureExportSettingsHandles[NI_NUM_MAP_TYPES];
    static const int m_aiTextureFilenameWinHandles[NI_NUM_MAP_TYPES];
    static const int ms_aIDC_TEXTURE_EXPORT[NI_NUM_MAP_TYPES];
};

#endif