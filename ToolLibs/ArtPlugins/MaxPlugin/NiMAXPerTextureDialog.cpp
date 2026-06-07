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
#include "NiMAXPerTextureDialog.h"
#include "NiMAX.h"

//
// Static data member initializations :
//

NiMAXPerTextureDialog* NiMAXPerTextureDialog::theDlg = 0;


const char* 
NiMAXPerTextureDialog::ms_aTextureExportSettingStrings[
    NI_NUM_EXPORT_SETTINGS] = 
{
    "Global settings",
    "Image Defaults",
    "High Color (16 bit)",
    "Compressed (DXT1)",
    "Palettized (8 bit)"   
};

const int
NiMAXPerTextureDialog::m_aiTextureFilenameWinHandles[NI_NUM_MAP_TYPES] =
{
    IDC_FILE_BASE,
    IDC_FILE_DARK,
    IDC_FILE_DETAIL,
    IDC_FILE_DECAL1,
    IDC_FILE_DECAL2,
    IDC_FILE_GLOW,
    IDC_FILE_GLOSS
};

const int 
NiMAXPerTextureDialog::m_aiTextureExportSettingsHandles[NI_NUM_MAP_TYPES] =
{
    sk_base_export,
    sk_dark_export,
    sk_detail_export,
    sk_decal1_export,
    sk_decal2_export,
    sk_glow_export,
    sk_gloss_export
};

const int NiMAXPerTextureDialog::ms_aIDC_TEXTURE_EXPORT[NI_NUM_MAP_TYPES] = 
{
    IDC_TEXTURE_EXPORT_BASE,
    IDC_TEXTURE_EXPORT_DARK,
    IDC_TEXTURE_EXPORT_DETAIL,
    IDC_TEXTURE_EXPORT_DECAL1,
    IDC_TEXTURE_EXPORT_DECAL2,
    IDC_TEXTURE_EXPORT_GLOW,
    IDC_TEXTURE_EXPORT_GLOSS
};

//---------------------------------------------------------------------------
NiMAXPerTextureDialog::NiMAXPerTextureDialog(ShaderParamDlg * Parent, 
    IParamBlock2* pblock)
{ 
    m_pkParent = Parent;
    m_pblock = pblock;
} 
 
static const int aiMapsToMAXChannels[] = 
{   //BASE  DARK  DETAIL DECAL1 BUMP   GLOSS  GLOW   DECAL2
        0,   1,     2,    3,      -1,     6,    5  ,    -1
};

//---------------------------------------------------------------------------

NiMAXPerTextureDialog::~NiMAXPerTextureDialog()
{ 
    m_pkParent = 0;
} 
//---------------------------------------------------------------------------

int NiMAXPerTextureDialog::GetMaxMapType()
{ 
    return NI_NUM_MAP_TYPES;
} 

//---------------------------------------------------------------------------

void NiMAXPerTextureDialog::SetupListBoxItems(HWND hwndCS)
{
    SendMessage(hwndCS, CB_RESETCONTENT, 0L, 0L);

    for (int j = 0; j < NI_NUM_EXPORT_SETTINGS; j++) 
    {
        SendMessage(hwndCS, CB_ADDSTRING, 
                    0L, (LPARAM)(ms_aTextureExportSettingStrings[j]) );
    }
}


//---------------------------------------------------------------------------

int NiMAXPerTextureDialog::GetTextureSettingForMap(MapType map)
{ 
    return m_pblock->GetInt((ParamID)m_aiTextureExportSettingsHandles[map], 0);
} 

//---------------------------------------------------------------------------

void NiMAXPerTextureDialog::SetTextureSettingForMap(MapType map, 
    NiMAXShader::TextureExportSetting setting)
{ 
    if(map < 0 || setting < 0)
        return;
    else
        m_pblock->SetValue((ParamID)m_aiTextureExportSettingsHandles[map], 0, setting);
} 

//---------------------------------------------------------------------------

void NiMAXPerTextureDialog::SetTextureMapName(HWND hDlg, MapType map, 
    char* filename)
{
    if (map < NI_NUM_MAP_TYPES)
        SetDlgItemText(hDlg, m_aiTextureFilenameWinHandles[map], filename);
}

//---------------------------------------------------------------------------
int  NiMAXPerTextureDialog::IDC_ToIndex(int idc, const int ms_aIDCTable[])
{
    for (int i = 0; i < NI_NUM_MAP_TYPES; i++)
    {
        if (idc == ms_aIDCTable[i])
            return i;
    }
    NIASSERT(0);
    return -1;
}
//---------------------------------------------------------------------------
// Our static message handler
// In order to avoid making everything static, I keep a ptr to the actual
// instance of the texture export settings dialog window
BOOL NiMAXPerTextureDialog::PanelProc(HWND hDlg, UINT iMsg, WPARAM wParam, 
    LPARAM lParam)
{
    if(theDlg)
    {
        theDlg->HandleMessages(hDlg,iMsg,wParam,lParam);
    }

    switch (iMsg)
    {
        case WM_INITDIALOG:
            theDlg = (NiMAXPerTextureDialog*) lParam;
            CenterWindow(hDlg, GetWindow(hDlg, GW_OWNER));
            theDlg->HandleMessages(hDlg,iMsg,wParam,lParam);
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_OK_BUTTON:
                case WM_CLOSE:
                case WM_DESTROY: 
                   theDlg = NULL;
                   EndDialog(hDlg, TRUE); 
                   break;
            }
    }
    return FALSE;
}

//---------------------------------------------------------------------------
BOOL NiMAXPerTextureDialog::HandleMessages(HWND hDlg, UINT iMsg, 
    WPARAM wParam, LPARAM)
{
    CHECK_MEMORY();
    int id = LOWORD(wParam);

    int i = 0;

    switch (iMsg)
    {
        case WM_INITDIALOG:
            LoadParams(hDlg);
            return TRUE;
            break;
        case WM_COMMAND:
            int iSel;
            switch(id)
            {
                case IDC_TEXTURE_EXPORT_BASE:
                case IDC_TEXTURE_EXPORT_DARK:
                case IDC_TEXTURE_EXPORT_DETAIL:
                case IDC_TEXTURE_EXPORT_DECAL1:
                case IDC_TEXTURE_EXPORT_DECAL2:
                case IDC_TEXTURE_EXPORT_GLOSS:
                case IDC_TEXTURE_EXPORT_GLOW:
                    // Get the index from the windows id code
                    i = IDC_ToIndex(id, ms_aIDC_TEXTURE_EXPORT);
                    iSel = (int)SendDlgItemMessage(hDlg, id, CB_GETCURSEL, 0, 0);
                    SetTextureSettingForMap((MapType)i, 
                        (NiMAXShader::TextureExportSetting)iSel);
                    break;
                case IDC_OK_BUTTON:
                case WM_CLOSE:
                case WM_DESTROY: 
                   SaveParams(hDlg);
                   break;

            }
            break;
        case CC_SPINNER_BUTTONDOWN:
            theHold.Begin();
            break;

        case WM_CUSTEDIT_ENTER:
        case CC_SPINNER_BUTTONUP: 
            if (HIWORD(wParam) || iMsg==WM_CUSTEDIT_ENTER) 
                theHold.Accept(GetString(IDS_PARAMCHG));
            else 
                theHold.Cancel();
            break;
    }
    return FALSE;
}


//---------------------------------------------------------------------------
void  NiMAXPerTextureDialog::ShowDialog(HINSTANCE hInstance, 
    IParamBlock2* pblock)
{
    m_pblock = pblock;
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_TEXTURE_EXPORT), 
            GetActiveWindow(), (DLGPROC)PanelProc , (LPARAM) this);
}

//---------------------------------------------------------------------------
void  NiMAXPerTextureDialog::SaveParams(HWND)
{

}

//---------------------------------------------------------------------------
// This class exists as a callback that runs through the enumerated 
// dependents, looking for a Material. It stores the Material if it finds it
// and returns. This is done to fit in with Max's scheme of finding 
// dependents
class CheckForMapsDlg : public DependentEnumProc 
{
    public:     
        BOOL bfound;
        ReferenceMaker *m_pkme;
        ReferenceMaker *m_pkmap;
        CheckForMapsDlg(ReferenceMaker *m) 
        {
            bfound = FALSE;
            m_pkme = m;
            m_pkmap = NULL;
        }
        inline int proc(ReferenceMaker *rmaker) 
        {
            if (rmaker==m_pkme) 
                return 0;
            if (rmaker->SuperClassID()==SClass_ID(MATERIAL_CLASS_ID)) 
            {
                bfound = TRUE;
                m_pkmap = rmaker;
                return 1;
            }
            return 0;
        }
};
 
void  NiMAXPerTextureDialog::LoadParams(HWND hDlg)
{
    // Setup the ComboBoxes
    HWND hwndCS;
    for(int i = 0; i <  NI_NUM_MAP_TYPES; i++)
    {
        hwndCS = GetDlgItem(hDlg, ms_aIDC_TEXTURE_EXPORT[i]);
        SetupListBoxItems(hwndCS);
        SendMessage(hwndCS, CB_SETCURSEL, 
            GetTextureSettingForMap((MapType) i), 0L);
    }
    
    Shader* pks = m_pkParent->GetShader() ;
    CheckForMapsDlg kcheck(pks);

    pks->DoEnumDependents(&kcheck);

    // Make sure that we have found the map in the enumerations
    // Make sure that the map is not equal to null
    if(kcheck.bfound && kcheck.m_pkmap != NULL )
    {
        // Should probably check to make sure this is a StdMat 
        // before we cast
        StdMat2* pkmaterial = ((StdMat2*) (kcheck.m_pkmap));
        // Make sure we survived the cast
        if(pkmaterial)
        {
            SetTextureMapName(hDlg, DECAL2, "None");
            for(int i = 0; i <  NI_NUM_MAP_TYPES; i++)
            {
                
                if(pkmaterial->GetMapState(i) == 2)
                // 0 = no map, 1 = disable, 2 = mapon
                {
                    TSTR filename = pkmaterial->GetMapName(i);
                    if(filename != NULL)
                    {
                        if(aiMapsToMAXChannels[i] == DECAL1)
                        {
                            Texmap *pktmap = pkmaterial->GetSubTexmap(
                                S_DECAL);
                            Class_ID id = pktmap->ClassID();

                            if (( id == Class_ID(MIX_CLASS_ID, 0) ||
                                  id == Class_ID(COMPOSITE_CLASS_ID, 0) ||
                                  id == Class_ID(RGBMULT_CLASS_ID, 0)) &&
                                  pktmap->NumSubTexmaps() > 1) 
                            {
                                Texmap* pksubtmap = pktmap->GetSubTexmap(0);
                                filename = pksubtmap->GetName();
                                pksubtmap = pktmap->GetSubTexmap(1);
                                SetTextureMapName(hDlg, DECAL2, 
                                                pksubtmap->GetName());
                               

                            }
                            else
                                SetTextureMapName(hDlg, DECAL2, "None");
                        }
                        

                        SetTextureMapName(hDlg,(MapType) 
                            aiMapsToMAXChannels[i], filename);
                    }
                }
                else if(aiMapsToMAXChannels[i] != -1)
                {
                    SetTextureMapName(hDlg,(MapType)
                        aiMapsToMAXChannels[i], "None");
                }
            }
        }
    }
}
