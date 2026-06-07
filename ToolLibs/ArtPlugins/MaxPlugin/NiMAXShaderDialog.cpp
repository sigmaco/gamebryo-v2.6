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
#include "NiMAXShaderDialog.h"
#include "NiStringTokenizer.h"

void FillShaderDropdown(const char* pcShaderName, HWND hWndParent);
void FillShaderDescription(const char* pcShaderName, HWND hWndParent);
void FillTechniqueDropdowns(const char* pcShaderName, HWND hWndParent,
    unsigned int uiWhichViewer, unsigned int uiWhichExport);
void DoExtendedShaderDesc(const char* pcShaderName, HWND hWndParent);

const char *
NiMAXShaderDialog::ms_aTestModes[NiAlphaProperty::TEST_MAX_MODES] = 
{
    "ALWAYS",
    "LESS",
    "EQUAL",
    "LESSEQUAL",
    "GREATER",
    "NOTEQUAL",
    "GREATEREQUAL",
    "NEVER" 
};

const char *
NiMAXShaderDialog::ms_aAlphaModes[NiAlphaProperty::ALPHA_MAX_MODES] = 
{
    "ONE",
    "ZERO",
    "SRCCOLOR",
    "INVSRCCOLOR",
    "DESTCOLOR",
    "INVDESTCOLOR",
    "SRCALPHA",
    "INVSRCALPHA",
    "DESTALPHA",
    "INVDESTALPHA",
    "SRCALPHASAT"
};


const char *
NiMAXShaderDialog::ms_aApplyModes[NiTexturingProperty::APPLY_MAX_MODES] =
{ 
    "REPLACE",
    "DECAL",
    "MODULATE",
    "DEPRECATED",   // used to be HILIGHT (ps2 only)
    "DEPRECATED"    // used to be HILIGHT2 (ps2 only)
};

const char *
NiMAXShaderDialog::ms_aVertexModes[NiVertexColorProperty::SOURCE_NUM_MODES] =
{
    "IGNORE",
    "EMISSIVE",
    "AMB_DIFF"
};

const char *
NiMAXShaderDialog::ms_aNormalMapTechniques[SHADER_NORMALMAPTECH_COUNT] =
{
    "None",
    "NDL",
    "ATI",
    "MAX"
};

const char *
NiMAXShaderDialog::ms_aLightingModes[
    NiVertexColorProperty::LIGHTING_NUM_MODES] = 
{
    "E",
    "E_A_D"
};

static TCHAR* mapStates[] = { _T(" "), _T("m"),  _T("M") };



//---------------------------------------------------------------------------
BOOL CALLBACK GamebryoShaderDlgProc(HWND hwndDlg, 
                                      UINT msg, 
                                      WPARAM wParam, 
                                      LPARAM lParam) 
{
    CHECK_MEMORY();
    NiMAXShaderDialog *theDlg;

    if (msg == WM_INITDIALOG) 
    {
        theDlg = (NiMAXShaderDialog *) lParam;
        SetWindowLong(hwndDlg, GWL_USERDATA, (LONG)lParam);
    }
    else if (msg == WM_CLOSE || msg == WM_DESTROY) 
    {
        SetWindowLong(hwndDlg, GWL_USERDATA, NULL);
        return FALSE;
    }
    else 
    {
        theDlg = (NiMAXShaderDialog *) LongToPtr(GetWindowLong(hwndDlg, GWL_USERDATA));

        if (theDlg == NULL)
            return FALSE; 
    }

    theDlg->m_isActive = 1;
    BOOL res = theDlg->PanelProc(hwndDlg, msg, wParam, lParam);

    // we need to make sure we didn't accidentally cause the shader
    // dialog to die here
    theDlg = (NiMAXShaderDialog *) LongToPtr(GetWindowLong(hwndDlg, GWL_USERDATA));
    if (theDlg == NULL)
         return res; 
    theDlg->m_isActive = 0;
    return res;
}

//---------------------------------------------------------------------------
// Initialize all custom controls
NiMAXShaderDialog::NiMAXShaderDialog( HWND hwMtlEdit, IMtlParams *pParams)
{
    CHECK_MEMORY();
    m_pMtl = NULL;
    m_pShader = NULL;
    m_hwmEdit = hwMtlEdit;
    m_pMtlPar = pParams;
    m_dadMgr.Init(this);
    m_hRollup = NULL;
    m_curTime = 0;
    m_isActive = m_valid = FALSE;

    m_pShininessSpinner = NULL;
    m_pBumpMagnitudeSpinner = NULL;
    m_pParallaxOffsetSpinner = NULL;
    m_pAlphaSpinner = NULL;
    m_pTestRefSpinner2 = NULL;


    long i = 0;
    for( ; i < SHADER_NCOLBOX; ++i )
        m_colorSwatch[ i ] = NULL;

    for( i = 0; i < NMBUTS; ++i )
        m_texMBut[ i ] = NULL;

}

//---------------------------------------------------------------------------
// Destroy all custom controls
NiMAXShaderDialog::~NiMAXShaderDialog()
{
    CHECK_MEMORY();
    HDC hdc = GetDC(m_hRollup);
    GetGPort()->RestorePalette(hdc, m_hOldPal);
    ReleaseDC(m_hRollup, hdc);

    if ( m_pShader ) 
        m_pShader->SetParamDlg(NULL);

    for (long i=0; i < NMBUTS; i++ )
    {
        ReleaseICustButton( m_texMBut[i] );
        m_texMBut[i] = NULL; 
    }
    
    ReleaseISpinner(m_pShininessSpinner);
    ReleaseISpinner(m_pAlphaSpinner);
    ReleaseISpinner(m_pParallaxOffsetSpinner);
    ReleaseISpinner(m_pBumpMagnitudeSpinner);
    ReleaseISpinner(m_pTestRefSpinner2);

    SetWindowLong(m_hRollup, GWL_USERDATA, NULL);
    m_hRollup = NULL;
    CHECK_MEMORY();
}


//---------------------------------------------------------------------------
// Called to set the custom controls with their current settings
void NiMAXShaderDialog::LoadDialog(BOOL) 
{
    CHECK_MEMORY();
    if (m_pShader && m_hRollup) 
    {
        m_pShininessSpinner->SetValue( 
                    m_pShader->GetShininess(0, FALSE) ,FALSE);
        m_pShininessSpinner->SetKeyBrackets(KeyAtCurTime(sk_shininess));
        
        m_pAlphaSpinner->SetValue(m_pShader->GetAlpha(0, FALSE), FALSE);
        m_pAlphaSpinner->SetKeyBrackets(KeyAtCurTime(sk_alpha));

        m_pBumpMagnitudeSpinner->SetValue(m_pShader->GetBumpMagnitude(),
            FALSE);
        m_pParallaxOffsetSpinner->SetValue(m_pShader->GetParallaxOffset(),
            FALSE);
        
        m_pTestRefSpinner2->SetValue(m_pShader->GetTestRef(0, FALSE), FALSE);
        m_pTestRefSpinner2->SetKeyBrackets(KeyAtCurTime(sk_test_ref));
        
        HWND hwndItem = GetDlgItem(m_hRollup, IDC_TEST_MODE2);
        SendMessage(hwndItem, CB_SETCURSEL, 
            m_pShader->GetAlphaTestMode(0, FALSE), 0);

        HWND hwndSrcBlend = GetDlgItem(m_hRollup, IDC_ALPHA_SRC_MODE);
        SendMessage(hwndSrcBlend, CB_SETCURSEL, 
            m_pShader->GetSrcBlend(0, FALSE), 0);

        HWND hwndDestBlend = GetDlgItem(m_hRollup, IDC_ALPHA_DEST_MODE);
        SendMessage(hwndDestBlend, CB_SETCURSEL, 
            m_pShader->GetDestBlend(0, FALSE), 0);

        hwndItem = GetDlgItem(m_hRollup, IDC_APPLY_MODE);
        SendMessage(hwndItem, CB_SETCURSEL, 
            m_pShader->GetTextureApplyMode(0, FALSE), 0);

        hwndItem = GetDlgItem(m_hRollup, IDC_SRC_MODE);
        SendMessage(hwndItem, CB_SETCURSEL, 
            m_pShader->GetSourceVertexMode(0, FALSE), 0);

        hwndItem = GetDlgItem(m_hRollup, IDC_NORMAL_MAP_COMBO);
        SendMessage(hwndItem, CB_SETCURSEL, 
            m_pShader->GetNormalMapTechnique(),0);

        hwndItem = GetDlgItem(m_hRollup, IDC_LIGHT_MODE);
        SendMessage(hwndItem, CB_SETCURSEL, 
            m_pShader->GetLightingMode(0, FALSE), 0);

        CheckDlgButton(m_hRollup, IDC_ALPHATESTING_ON, 
            m_pShader->GetAlphaTestOn(0, FALSE));
        CheckDlgButton(m_hRollup, IDC_VERTEXCOLORS_ON, 
            m_pShader->GetVertexColorsOn(0, FALSE));
        CheckDlgButton(m_hRollup, IDC_SPECULAR_ON, 
            m_pShader->GetSpecularOn(0, FALSE));
        CheckDlgButton(m_hRollup, IDC_NOSORTER, 
            m_pShader->GetNoSorter(0, FALSE));
        CheckDlgButton(m_hRollup, IDC_NORMAL_MAP_CHECK, 
            m_pShader->GetUseNormalMap());

        // enable/disable parameters depending on selected alpha mode
        switch (IDC_ALPHA_AUTOMATIC+m_pShader->GetAlphaMode(0, FALSE))
        {
        case IDC_ALPHA_NONE:
            // hide advanced src/dest mode
            ShowWindow(hwndSrcBlend, SW_HIDE);
            ShowWindow(hwndDestBlend, SW_HIDE);
            break;
        case IDC_ALPHA_STANDARD:
        case IDC_ALPHA_ADDITIVE:
        case IDC_ALPHA_MULT:
            // unhide advanced src/dest mode
            ShowWindow(hwndSrcBlend, SW_SHOW);
            ShowWindow(hwndDestBlend, SW_SHOW);
            // disable advanced src/dest mode
            EnableWindow(hwndSrcBlend, SW_HIDE);
            EnableWindow(hwndDestBlend, SW_HIDE);
            break;
        case IDC_ALPHA_ADVANCED:
        case IDC_ALPHA_AUTOMATIC:
        default:
            // unhide advanced src/dest mode
            ShowWindow(hwndSrcBlend, SW_SHOW);
            ShowWindow(hwndDestBlend, SW_SHOW);
            // enable advanced src/dest mode
            EnableWindow(hwndSrcBlend, SW_SHOW);
            EnableWindow(hwndDestBlend, SW_SHOW);
            break;
        }

        CheckRadioButton( m_hRollup, IDC_ALPHA_AUTOMATIC, IDC_ALPHA_ADVANCED,
            IDC_ALPHA_AUTOMATIC+m_pShader->GetAlphaMode(0, FALSE));

        UpdateColSwatches();

        NiString strShaderName = m_pShader->GetShaderName();
        unsigned int uiWhichExportTech = 
            m_pShader->GetExportShaderTechnique();
        unsigned int uiWhichViewerTech = 
            m_pShader->GetViewerShaderTechnique();

        FillShaderDropdown(strShaderName, m_hRollup);
        FillShaderDescription(strShaderName, m_hRollup);
        FillTechniqueDropdowns(strShaderName, m_hRollup, uiWhichViewerTech, 
            uiWhichExportTech);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXShaderDialog::UpdateMapButtons() 
{
    CHECK_MEMORY();

    for ( long i = 0; i < NMBUTS; ++i ) 
    {
        int nMap = texmapFromMBut[ i ];
        int state = m_pMtl->GetMapState( nMap );
        m_texMBut[i]->SetText( mapStates[ state ] );

        TSTR nm = m_pMtl->GetMapName( nMap );
        m_texMBut[i]->SetTooltip(TRUE,nm);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
// Necessary - but unused
void NiMAXShaderDialog::UpdateOpacity() 
{
}

//---------------------------------------------------------------------------
void NiMAXShaderDialog::UpdateColSwatches() 
{
    CHECK_MEMORY();
    m_colorSwatch[0]->SetKeyBrackets(m_pShader->KeyAtTime(sk_amb,m_curTime));
    m_colorSwatch[0]->SetColor( m_pShader->GetAmbientClr(0, FALSE) );

    m_colorSwatch[1]->SetKeyBrackets(m_pShader->KeyAtTime(sk_diff,m_curTime));
    m_colorSwatch[1]->SetColor( m_pShader->GetDiffuseClr(0, FALSE) );

    m_colorSwatch[2]->SetKeyBrackets(m_pShader->KeyAtTime(sk_spec,m_curTime));
    m_colorSwatch[2]->SetColor( m_pShader->GetSpecularClr(0, FALSE) );

    m_colorSwatch[3]->SetKeyBrackets(m_pShader->KeyAtTime(sk_emit,m_curTime));
    m_colorSwatch[3]->SetColor( m_pShader->GetEmittance(0, FALSE) );
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
// Translate a color IDC (control ID) to a color index
int NiMAXShaderDialog::ColorIDCToIndex(int idc) 
{
    CHECK_MEMORY();
    switch (idc) 
    {
        case IDC_AMB_COLOR: 
            return 0;
        case IDC_DIFF_COLOR: 
            return 1;
        case IDC_SPEC_COLOR: 
            return 2;
        case IDC_EMITTANCE: 
            return 3;
        default: 
            return 0;
    }
}


//---------------------------------------------------------------------------
// The dialog message handler
BOOL NiMAXShaderDialog::PanelProc(HWND hwndDlg, UINT msg, 
                                    WPARAM wParam, LPARAM ) 
{
    CHECK_MEMORY();
    int id = LOWORD(wParam);
    
    switch (msg) 
    {
        case WM_INITDIALOG:
           {
                // Initialize the dialog box with the values 
                // stored in the shader class
                HDC theHDC = GetDC(hwndDlg);
                m_hOldPal = GetGPort()->PlugPalette(theHDC);
                ReleaseDC(hwndDlg,theHDC);

                HWND hwndCS = GetDlgItem(hwndDlg, IDC_AMB_COLOR);
                m_colorSwatch[0] = 
                    GetIColorSwatch(hwndCS, 
                                    m_pShader->GetAmbientClr(0, FALSE), 
                                    GetString(IDS_AMB_COLOR) );

                hwndCS = GetDlgItem(hwndDlg, IDC_DIFF_COLOR);
                m_colorSwatch[1] = 
                    GetIColorSwatch(hwndCS, 
                                    m_pShader->GetDiffuseClr(0, FALSE), 
                                    GetString(IDS_DIFF_COLOR) );

                hwndCS = GetDlgItem(hwndDlg, IDC_SPEC_COLOR);
                m_colorSwatch[2] = 
                    GetIColorSwatch(hwndCS, 
                                    m_pShader->GetSpecularClr(0, FALSE), 
                                    GetString(IDS_SPEC_COLOR) );

                hwndCS = GetDlgItem(hwndDlg, IDC_EMITTANCE);
                m_colorSwatch[3] = 
                    GetIColorSwatch(hwndCS, 
                                    m_pShader->GetEmittance(0, FALSE), 
                                    GetString(IDS_EMITTANCE) );

                m_pShininessSpinner = 
                    SetupFloatSpinner(hwndDlg, 
                                      IDC_SHININESS_SPIN, 
                                      IDC_SHININESS_EDIT, 
                                      0.0f, 2500.0f, 
                                      m_pShader->GetShininess(0, FALSE));
                m_pShininessSpinner->SetScale(1.0f);

                m_pAlphaSpinner = 
                    SetupFloatSpinner(hwndDlg, 
                                      IDC_ALPHA_SPIN, 
                                      IDC_ALPHA_EDIT, 
                                      0.0f,1.0f, 
                                      m_pShader->GetAlpha(0, FALSE));
                m_pAlphaSpinner->SetScale(0.01f);

                m_pParallaxOffsetSpinner = 
                    SetupFloatSpinner(hwndDlg, 
                                      IDC_PARALLAX_OFFSET_SPIN, 
                                      IDC_PARALLAX_OFFSET_EDIT, 
                                      0.0f, 10.0f, 
                                      m_pShader->GetParallaxOffset());
                m_pParallaxOffsetSpinner->SetScale(0.01f);

                m_pBumpMagnitudeSpinner = 
                    SetupFloatSpinner(hwndDlg, 
                                      IDC_BUMP_MAGNITUDE_SPIN, 
                                      IDC_BUMP_MAGNITUDE_EDIT, 
                                      -2.0f, 2.0f, 
                                      m_pShader->GetBumpMagnitude());
                m_pBumpMagnitudeSpinner->SetScale(0.01f);

                m_pTestRefSpinner2 = 
                    SetupIntSpinner(hwndDlg, 
                                    IDC_TESTREF_SPIN, 
                                    IDC_TESTREF_EDIT, 
                                    0,255, 
                                    m_pShader->GetTestRef(0, FALSE));
                
                int j = 0;
                for (; j<NMBUTS; j++) 
                {

                    m_texMBut[j] = GetICustButton(
                        GetDlgItem(hwndDlg,texMButtonsIDC[j]));
                    NIASSERT( m_texMBut[j] );
                    m_texMBut[j]->SetRightClickNotify(TRUE);
                    m_texMBut[j]->SetDADMgr(&m_dadMgr);
                }


                //--
                hwndCS = GetDlgItem(hwndDlg, IDC_TEST_MODE2);


                SendMessage(hwndCS, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < NiAlphaProperty::TEST_MAX_MODES; j++) 
                {
                    SendMessage(hwndCS, CB_ADDSTRING, 
                                0L, (LPARAM)(ms_aTestModes[j]) );
                }

                //--
                HWND hwndSrcBlend = GetDlgItem(hwndDlg, IDC_ALPHA_SRC_MODE);
            
                SendMessage(hwndSrcBlend, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < NiAlphaProperty::ALPHA_MAX_MODES; j++) 
                {
                    SendMessage(hwndSrcBlend, CB_ADDSTRING, 
                                0L, (LPARAM)(ms_aAlphaModes[j]) );
                }

                //--
                HWND hwndDestBlend = GetDlgItem(hwndDlg, IDC_ALPHA_DEST_MODE);

                SendMessage(hwndDestBlend, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < NiAlphaProperty::ALPHA_MAX_MODES; j++) 
                {
                    SendMessage(hwndDestBlend, CB_ADDSTRING, 
                                0L, (LPARAM)(ms_aAlphaModes[j]) );
                }

                //--
                hwndCS = GetDlgItem(hwndDlg, IDC_APPLY_MODE);

                SendMessage(hwndCS, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < NiTexturingProperty::APPLY_MAX_MODES; j++) 
                {
                    if (strcmp(ms_aApplyModes[j], "DEPRECATED") != 0)
                    {
                        SendMessage(hwndCS, CB_ADDSTRING, 
                                    0L, (LPARAM)(ms_aApplyModes[j]) );
                    }
                }

                //--
                hwndCS = GetDlgItem(hwndDlg, IDC_SRC_MODE);

                SendMessage(hwndCS, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < NiVertexColorProperty::SOURCE_NUM_MODES; j++)
                {
                    SendMessage(hwndCS, CB_ADDSTRING, 
                                0L, (LPARAM)(ms_aVertexModes[j]) );
                }

                
                //--
                hwndCS = GetDlgItem(hwndDlg, IDC_NORMAL_MAP_COMBO);

                SendMessage(hwndCS, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < SHADER_NORMALMAPTECH_COUNT; j++) 
                {
                    SendMessage(hwndCS, CB_ADDSTRING, 
                                0L, (LPARAM)(ms_aNormalMapTechniques[j]) );
                }

                //--
                hwndCS = GetDlgItem(hwndDlg, IDC_LIGHT_MODE);

                SendMessage(hwndCS, CB_RESETCONTENT, 0L, 0L);
                for (j = 0; j < NiVertexColorProperty::LIGHTING_NUM_MODES;
                    j++) 
                {
                    SendMessage(hwndCS, CB_ADDSTRING, 
                                0L, (LPARAM)(ms_aLightingModes[j]) );
                }

                // switch to standard/additive/multiplicative if possible
                if (IDC_ALPHA_AUTOMATIC+m_pShader->GetAlphaMode(0, FALSE) ==
                    IDC_ALPHA_ADVANCED)
                {
                    int iSrcBlend = m_pShader->GetSrcBlend(0, FALSE);
                    int iDestBlend = m_pShader->GetDestBlend(0, FALSE);

                    if (iSrcBlend == NiAlphaProperty::ALPHA_ONE &&
                        iDestBlend == NiAlphaProperty::ALPHA_ONE)
                    {
                        // additive
                        m_pShader->SetAlphaMode(IDC_ALPHA_ADDITIVE -
                            IDC_ALPHA_AUTOMATIC, 0);
                    }
                    else if (iSrcBlend == NiAlphaProperty::ALPHA_ZERO &&
                        iDestBlend == NiAlphaProperty::ALPHA_SRCCOLOR)
                    {
                        // multiplicative
                        m_pShader->SetAlphaMode(IDC_ALPHA_MULT -
                            IDC_ALPHA_AUTOMATIC, 0);
                    }
                    else if (iSrcBlend == NiAlphaProperty::ALPHA_SRCALPHA &&
                        iDestBlend == NiAlphaProperty::ALPHA_INVSRCALPHA)
                    {
                        // standard
                        m_pShader->SetAlphaMode(IDC_ALPHA_STANDARD -
                            IDC_ALPHA_AUTOMATIC, 0);
                    }                    
                }

                CHECK_MEMORY();
                m_pTextureExportDlg = NiExternalNew NiMAXPerTextureDialog(
                    this, m_pShader->GetParamBlock(0));
                CHECK_MEMORY();
                LoadDialog(TRUE);
                CHECK_MEMORY();
            }
            break;

        case WM_COMMAND: 
            {
                CHECK_MEMORY();
                int iSel;

                for ( int i=0; i<NMBUTS; i++) 
                {
                    if (id == texMButtonsIDC[i]) 
                    {
                        PostMessage(m_hwmEdit,WM_TEXMAP_BUTTON, 
                                    texmapFromMBut[i],(LPARAM) m_pMtl );
                        UpdateMapButtons();
                        goto exit;
                    }
                }

                GetDlgItem(hwndDlg, IDC_ALPHA_SRC_MODE);
                GetDlgItem(hwndDlg, IDC_ALPHA_DEST_MODE);

                switch (id)
                {
                case IDC_NORMAL_MAP_COMBO:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_NORMAL_MAP_COMBO, CB_GETCURSEL, 0, 0);
                    m_pShader->SetNormalMapTechnique(
                        (NiMAXShader::NBT_Method) iSel);
                    break;
                case IDC_SRC_MODE:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_SRC_MODE, CB_GETCURSEL, 0, 0);
                    m_pShader->SetSourceVertexMode(iSel, 0);
                    break;
                case IDC_LIGHT_MODE:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_LIGHT_MODE, CB_GETCURSEL, 0, 0);
                    m_pShader->SetLightingMode(iSel, 0);
                    break;
                case IDC_ALPHA_SRC_MODE:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_ALPHA_SRC_MODE, CB_GETCURSEL, 0, 0);
                    m_pShader->SetSrcBlend(iSel, 0);
                    break;
                case IDC_ALPHA_DEST_MODE:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_ALPHA_DEST_MODE, CB_GETCURSEL, 0, 0);
                    m_pShader->SetDestBlend(iSel, 0);
                    break;
                case IDC_ALPHA_AUTOMATIC:
                case IDC_ALPHA_ADVANCED:
                case IDC_ALPHA_NONE:
                    m_pShader->SetAlphaMode(id - IDC_ALPHA_AUTOMATIC, 0);
                    break;
                case IDC_ALPHA_STANDARD:
                    m_pShader->SetAlphaMode(id - IDC_ALPHA_AUTOMATIC, 0);
                    m_pShader->SetSrcBlend(NiAlphaProperty::ALPHA_SRCALPHA,
                        0);
                    m_pShader->SetDestBlend(
                        NiAlphaProperty::ALPHA_INVSRCALPHA, 0);
                    break;
                case IDC_ALPHA_ADDITIVE:
                    m_pShader->SetAlphaMode(id - IDC_ALPHA_AUTOMATIC, 0);
                    m_pShader->SetSrcBlend(NiAlphaProperty::ALPHA_ONE,
                        0);
                    m_pShader->SetDestBlend(
                        NiAlphaProperty::ALPHA_ONE, 0);
                    break;
                case IDC_ALPHA_MULT:
                    m_pShader->SetAlphaMode(id - IDC_ALPHA_AUTOMATIC, 0);
                    m_pShader->SetSrcBlend(NiAlphaProperty::ALPHA_ZERO,
                        0);
                    m_pShader->SetDestBlend(
                        NiAlphaProperty::ALPHA_SRCCOLOR, 0);
                    break;
                case IDC_TEST_MODE2:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_TEST_MODE2, CB_GETCURSEL, 0, 0);
                    m_pShader->SetAlphaTestMode(iSel, 0);
                    break;
                case IDC_APPLY_MODE:
                    iSel = (int)SendDlgItemMessage(
                        hwndDlg, IDC_APPLY_MODE, CB_GETCURSEL, 0, 0);
                    m_pShader->SetTextureApplyMode(iSel, 0);
                    break;
                case IDC_ALPHATESTING_ON:
                    m_pShader->SetAlphaTestOn(
                        IsDlgButtonChecked(hwndDlg, IDC_ALPHATESTING_ON), 0);
                    break;
                case IDC_VERTEXCOLORS_ON:
                    m_pShader->SetVertexColorsOn(
                        IsDlgButtonChecked(hwndDlg, IDC_VERTEXCOLORS_ON), 0);
                    break;
                case IDC_NORMAL_MAP_CHECK:
                    m_pShader->SetUseNormalMap(
                        IsDlgButtonChecked(hwndDlg, IDC_NORMAL_MAP_CHECK));
                    break;
                case IDC_SPECULAR_ON:
                    m_pShader->SetSpecularOn(
                        IsDlgButtonChecked(hwndDlg, IDC_SPECULAR_ON), 0);
                    break;
                case IDC_NOSORTER:
                    m_pShader->SetNoSorter(
                        IsDlgButtonChecked(hwndDlg, IDC_NOSORTER), 0);
                    break;
                case IDC_TEXTURE_EXPORT_BUTTON:
                    m_pTextureExportDlg->ShowDialog(hInstance, 
                        m_pShader->GetParamBlock(0) );
                    break;
                case IDC_SHADER_COMBO:
                    if(HIWORD(wParam) == CBN_SELCHANGE )
                    {
                        CHECK_MEMORY();
                        HWND hWndViewerCombo = GetDlgItem(hwndDlg, 
                            IDC_VIEWER_TECHNIQUE_COMBO);
                        HWND hWndExportCombo = GetDlgItem(hwndDlg, 
                            IDC_EXPORT_TECHNIQUE_COMBO);
                        unsigned int uiSel = (unsigned int) SendDlgItemMessage(hwndDlg, 
                            IDC_SHADER_COMBO, CB_GETCURSEL, 0, 0); 
                        NIASSERT(uiSel != CB_ERR);
                        char acString[MAX_PATH];
                        SendDlgItemMessage(hwndDlg, IDC_SHADER_COMBO, 
                            CB_GETLBTEXT,
                            uiSel, (LPARAM) acString);  
                        FillShaderDescription(acString, hwndDlg);

                        if (strcmp(acString, m_pShader->GetShaderName()) 
                            == 0)
                        {
                            EnableWindow(hWndViewerCombo, SW_SHOW);
                            EnableWindow(hWndExportCombo, SW_SHOW);
                            EnableWindow(GetDlgItem(hwndDlg, 
                                IDC_APPLY_SHADER_BUTTON), SW_HIDE);
                            EnableWindow(GetDlgItem(hwndDlg, 
                                IDC_UPDATE_SHADER_BUTTON), SW_SHOW);
                        }
                        else
                        {
                            EnableWindow(hWndViewerCombo, SW_HIDE);
                            EnableWindow(hWndExportCombo, SW_HIDE);
                            EnableWindow(GetDlgItem(hwndDlg, 
                                IDC_APPLY_SHADER_BUTTON), SW_SHOW);
                            EnableWindow(GetDlgItem(hwndDlg, 
                                IDC_UPDATE_SHADER_BUTTON), SW_HIDE);
                        }
                        CHECK_MEMORY();
                    }
                    break;
                case IDC_APPLY_SHADER_BUTTON:
                    {
                        CHECK_MEMORY();
                        HWND hWndViewerCombo = GetDlgItem(hwndDlg, 
                            IDC_VIEWER_TECHNIQUE_COMBO);
                        HWND hWndExportCombo = GetDlgItem(hwndDlg, 
                            IDC_EXPORT_TECHNIQUE_COMBO);

                        unsigned int uiSel = (unsigned int) SendDlgItemMessage(hwndDlg, 
                            IDC_SHADER_COMBO, CB_GETCURSEL, 0, 0); 
                        char acString[MAX_PATH];
                        if (uiSel == CB_ERR)
                        {
                            MessageBox(hwndDlg, "There is not a valid shader "
                                "selected.\nUsing default shader instead.", 
                                "Invalid selection", MB_OK);
                            NiStrcpy(acString, MAX_PATH, 
                                NI_DEFAULT_SHADER_NAME);
                        }
                        else
                        {
                            SendDlgItemMessage(hwndDlg, IDC_SHADER_COMBO, 
                                CB_GETLBTEXT,
                                uiSel, (LPARAM) acString); 
                        }
                        CHECK_MEMORY();
                        m_pShader->SetShaderName(acString);
                        CHECK_MEMORY();
                        FillShaderDescription(acString, m_hRollup);
                        CHECK_MEMORY();
                        unsigned int uiWhichExportTech = 
                            m_pShader->GetExportShaderTechnique();
                        CHECK_MEMORY();
                        unsigned int uiWhichViewerTech = 
                            m_pShader->GetViewerShaderTechnique();
                        CHECK_MEMORY();
                        FillTechniqueDropdowns(acString, m_hRollup, 
                            uiWhichViewerTech, 
                            uiWhichExportTech);
                        CHECK_MEMORY();
                        EnableWindow(hWndViewerCombo, SW_SHOW);
                        EnableWindow(hWndExportCombo, SW_SHOW);
                        EnableWindow(GetDlgItem(hwndDlg, 
                            IDC_APPLY_SHADER_BUTTON), SW_HIDE);
                        EnableWindow(GetDlgItem(hwndDlg, 
                            IDC_UPDATE_SHADER_BUTTON), SW_SHOW);
                        CHECK_MEMORY();
                        m_pShader->UpdateShaderCustAttrib();
                        CHECK_MEMORY();
                    }
                    break;
                case IDC_SHADER_MOREINFO_BUTTON:
                    {
                        CHECK_MEMORY();
                        unsigned int uiSel = (unsigned int) SendDlgItemMessage(hwndDlg, 
                            IDC_SHADER_COMBO, CB_GETCURSEL, 0, 0); 
                        char acString[MAX_PATH];
                        if (uiSel == CB_ERR)
                        {
                            MessageBox(hwndDlg, "There is not a valid shader "
                                "selected.\nUsing default shader instead.", 
                                "Invalid selection", MB_OK);
                            NiStrcpy(acString, MAX_PATH, 
                                NI_DEFAULT_SHADER_NAME);
                        }
                        else
                        {
                            SendDlgItemMessage(hwndDlg, IDC_SHADER_COMBO, 
                                CB_GETLBTEXT,
                                uiSel, (LPARAM) acString); 
                        }
                        DoExtendedShaderDesc(acString, hwndDlg);
                        CHECK_MEMORY();
                    }
                    break;
                case IDC_UPDATE_SHADER_BUTTON:
                    {
                        CHECK_MEMORY();
                        int iReturn = MessageBox(hwndDlg, "This will reset "
                            "all of the values"
                            " of the current NiShader.\nDo you want to"
                            " continue?", 
                            "NiShader Reset Command confirmation", 
                            MB_OKCANCEL);

                        if (iReturn == IDOK)
                        {
                            HWND hWndViewerCombo = GetDlgItem(hwndDlg, 
                                IDC_VIEWER_TECHNIQUE_COMBO);
                            HWND hWndExportCombo = GetDlgItem(hwndDlg, 
                                IDC_EXPORT_TECHNIQUE_COMBO);
                            NiString strString = m_pShader->GetShaderName();
                            FillShaderDescription(strString, m_hRollup);
                            unsigned int uiWhichExportTech = 
                                m_pShader->GetExportShaderTechnique();
                            unsigned int uiWhichViewerTech = 
                                m_pShader->GetViewerShaderTechnique();
                            FillTechniqueDropdowns(strString, m_hRollup,
                                uiWhichViewerTech, 
                                uiWhichExportTech);
                            SendDlgItemMessage(hwndDlg, IDC_SHADER_COMBO,
                                CB_SELECTSTRING, (WPARAM) -1, 
                                (LPARAM) (const char*)strString);
                            EnableWindow(hWndViewerCombo, SW_SHOW);
                            EnableWindow(hWndExportCombo, SW_SHOW);
                            EnableWindow(GetDlgItem(hwndDlg, 
                                IDC_APPLY_SHADER_BUTTON), SW_HIDE);
                            EnableWindow(GetDlgItem(hwndDlg, 
                                IDC_UPDATE_SHADER_BUTTON), SW_SHOW);
                            CHECK_MEMORY();
                            m_pShader->RemoveShaderCustAttrib();
                            CHECK_MEMORY();
                            m_pShader->UpdateShaderCustAttrib();
                            CHECK_MEMORY();
                        }
                    }
                    break;
                case IDC_VIEWER_TECHNIQUE_COMBO:
                    {
                        if(HIWORD(wParam) == CBN_SELCHANGE )
                        {
                            HWND hWndViewerCombo = GetDlgItem(hwndDlg, 
                                IDC_VIEWER_TECHNIQUE_COMBO);
                            unsigned int uiSel = (unsigned int) SendMessage(hWndViewerCombo,
                                CB_GETCURSEL, 0, 0);
                            m_pShader->SetViewerShaderTechnique(uiSel);
                        }
                    }
                    break;
                case IDC_EXPORT_TECHNIQUE_COMBO:
                    {
                        if(HIWORD(wParam) == CBN_SELCHANGE )
                        {
                            HWND hWndExportCombo = GetDlgItem(hwndDlg, 
                                IDC_EXPORT_TECHNIQUE_COMBO);
                            unsigned int uiSel = (unsigned int) SendMessage(hWndExportCombo,
                                CB_GETCURSEL, 0, 0);
                            m_pShader->SetExportShaderTechnique(uiSel);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            break;

        case CC_COLOR_SEL: 
            {
                int id = LOWORD(wParam);
                SelectEditColor(ColorIDCToIndex(id));
            }           
            break;
        case CC_COLOR_DROP: 
            {
                int id = LOWORD(wParam);
                SelectEditColor(ColorIDCToIndex(id));
            }
            break;
        case CC_COLOR_BUTTONDOWN:
            theHold.Begin();
            break;
        case CC_COLOR_BUTTONUP:
            if (HIWORD(wParam)) 
                theHold.Accept(GetString(IDS_PARAMCHG));
            else 
                theHold.Cancel();
            break;
        case CC_COLOR_CHANGE: 
            {           
                int id = LOWORD(wParam);
                int buttonUp = HIWORD(wParam); 
                int n = ColorIDCToIndex(id);
                if (buttonUp) 
                    theHold.Begin();

                AColor curRGB = m_colorSwatch[n]->GetAColor();
                switch (n)
                {
                case 0:
                    m_pShader->SetAmbientClr(curRGB, m_curTime); 
                    break;
                case 1:
                    m_pShader->SetDiffuseClr(curRGB, m_curTime); 
                    break;
                case 2:
                    m_pShader->SetSpecularClr(curRGB, m_curTime); 
                    break;
                case 3:
                    m_pShader->SetEmittance(curRGB, m_curTime); 
                    break;
                default:
                    break;
                }

                if (buttonUp) 
                {
                    theHold.Accept(GetString(IDS_PARAMCHG));
                    UpdateMtlDisplay();             
                }
            } 
            break;
        case WM_PAINT: 
            if (!m_valid) 
            {
                m_valid = TRUE;
                ReloadDialog();
            }
            return FALSE;
        case WM_CLOSE:
        case WM_DESTROY: 
            break;

        case CC_SPINNER_CHANGE: 
            if (!theHold.Holding()) 
                theHold.Begin();

            switch (id) 
            {
                case IDC_SHININESS_SPIN: 
                    m_pShader->SetShininess(m_pShininessSpinner->GetFVal() , 
                        m_curTime);
                    break;
                case IDC_ALPHA_SPIN: 
                    m_pShader->SetAlpha(m_pAlphaSpinner->GetFVal() , 
                        m_curTime); 
                    break;
                case IDC_TESTREF_SPIN: 
                    m_pShader->SetTestRef((unsigned char)m_pTestRefSpinner2->GetIVal() , 
                        m_curTime); 
                    break;
                case IDC_BUMP_MAGNITUDE_SPIN:
                    m_pShader->SetBumpMagnitude(
                        m_pBumpMagnitudeSpinner->GetFVal());
                   break;
               case IDC_PARALLAX_OFFSET_SPIN:
                    m_pShader->SetParallaxOffset(
                        m_pParallaxOffsetSpinner->GetFVal());
                    break;
            }
            UpdateMtlDisplay();
        break;

        case CC_SPINNER_BUTTONDOWN:
            theHold.Begin();
            break;

        case WM_CUSTEDIT_ENTER:
        case CC_SPINNER_BUTTONUP: 
            if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
                theHold.Accept(GetString(IDS_PARAMCHG));
            else 
                theHold.Cancel();
            UpdateMtlDisplay();
            break;

    }
    exit:
    CHECK_MEMORY();
    return FALSE;
    }

//---------------------------------------------------------------------------

void FillShaderDropdown(const char* pcShaderName, HWND hWndParent)
{
    CHECK_MEMORY();
    HWND hWndCombo = GetDlgItem(hWndParent, IDC_SHADER_COMBO);
    SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);
    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();

    SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)NI_DEFAULT_SHADER_NAME);

    for (unsigned int ui = 0; ui < pkToolkit->GetLibraryCount(); ui++)
    {
        NiMaterialLibrary* pkLib = pkToolkit->GetLibraryAt(ui);
        if (pkLib)
        {   
            for (unsigned int uj = 0; uj < pkLib->GetMaterialCount(); uj++)
            {
                NiString strShaderName = pkLib->GetMaterialName(uj);

                if (strShaderName.IsEmpty())
                    continue;

                strShaderName = strShaderName.Left(MAX_PATH);
                SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM) 
                    (const char*)strShaderName);
            }
        }
    }

    SendMessage(hWndCombo, CB_SELECTSTRING, (WPARAM) -1, (LPARAM) pcShaderName);
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void FillShaderDescription(const char* pcShaderName, HWND hWndParent)
{
    CHECK_MEMORY();
    HWND hWndDesc = GetDlgItem(hWndParent, IDC_SHADER_NOTES);
    
    NiString strShaderName = pcShaderName;

    if (strShaderName.EqualsNoCase(NI_DEFAULT_SHADER_NAME))
    {
        SendMessage(hWndDesc, WM_SETTEXT, 0, (LPARAM)"The standard "
            "fixed-function pipeline.");
        return;
    }

    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
    NiShaderDesc* pkDesc = pkToolkit->GetMaterialDesc(pcShaderName);

    if (pkDesc)
    {
        NiString strDesc = pkDesc->GetDescription();
        strDesc.Replace("\n", "\r\x00A");
        if (!strDesc.IsEmpty())
        {
            SendMessage(hWndDesc, WM_SETTEXT, 0, (LPARAM) (const char*)
                strDesc);
            return;
        }
    }

    SendMessage(hWndDesc, WM_SETTEXT, 0, (LPARAM) "No information was "
        "provided for this shader.");
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void DoExtendedShaderDesc(const char* pcShaderName, HWND hWndParent)
{
    NiString strShaderName = pcShaderName;
    NiString strTitle = pcShaderName;
    strTitle += " Info";
    NiString strMessage = NULL;

    if (strShaderName.EqualsNoCase(NI_DEFAULT_SHADER_NAME))
    {
        strMessage += "The standard fixed-function pipeline.";
    }
    else
    {
        NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
        NiShaderDesc* pkDesc = pkToolkit->GetMaterialDesc(pcShaderName);

        if (pkDesc)
        {
            unsigned int uiMinMajor = 
                NiShaderRequirementDesc::GetMinorVertexShaderVersion(
                pkDesc->GetMinVertexShaderVersion());
            unsigned int uiMinMinor = 
                NiShaderRequirementDesc::GetMinorVertexShaderVersion(
                pkDesc->GetMinVertexShaderVersion());
            unsigned int uiMaxMajor = 
                NiShaderRequirementDesc::GetMinorVertexShaderVersion(
                pkDesc->GetMaxVertexShaderVersion());
            unsigned int uiMaxMinor = 
                NiShaderRequirementDesc::GetMinorVertexShaderVersion(
                pkDesc->GetMaxVertexShaderVersion());

            strMessage +="\nMin Vertex Shader Version: \t" + 
                NiString::FromInt(uiMinMajor) + "." + 
                NiString::FromInt(uiMinMinor);
            strMessage +="\nMax Vertex Shader Version: \t" + 
                NiString::FromInt(uiMaxMajor) + "." + 
                NiString::FromInt(uiMaxMinor);
            
            uiMinMajor = 
                NiShaderRequirementDesc::GetMajorPixelShaderVersion(
                pkDesc->GetMinPixelShaderVersion());
            uiMinMinor = 
                NiShaderRequirementDesc::GetMinorPixelShaderVersion(
                pkDesc->GetMinPixelShaderVersion());
            uiMaxMajor = 
                NiShaderRequirementDesc::GetMajorPixelShaderVersion(
                pkDesc->GetMaxPixelShaderVersion());
            uiMaxMinor = 
                NiShaderRequirementDesc::GetMinorPixelShaderVersion(
                pkDesc->GetMaxPixelShaderVersion());

            strMessage +="\n\nMin Pixel Shader Version: \t" + 
                NiString::FromInt(uiMinMajor) + "." + 
                NiString::FromInt(uiMinMinor);
            strMessage +="\nMax Pixel Shader Version: \t" + 
                NiString::FromInt(uiMaxMajor) + "." + 
                NiString::FromInt(uiMaxMinor);

            strMessage +="\n\n";

            strMessage += "Shader Description:\n";

            NiString strDesc = pkDesc->GetDescription();
            if (!strDesc.IsEmpty())
            {
                strDesc.Replace("\t", "");
                strDesc.Replace("\n", "\n\t");
                strMessage += "\t" + strDesc;
            }
            else
            {
                strMessage += "\tNo description was provided"
                " for this shader.";
            }


            for (unsigned int ui = 0; ui < 
                pkDesc->GetNumberOfImplementations(); ui++)
            {

                const NiShaderRequirementDesc* pkImplDesc = 
                    pkDesc->GetImplementationDescription(ui);
                strMessage += "\n\nTechnique \"";
                NiString strName = pkImplDesc->GetName();
                if (strName.IsEmpty())
                {
                    strName += "Implementation #";
                    strName += NiString::FromInt(ui);
                }
                strMessage += strName;
                strMessage += "\" Description:\n";

                strDesc = pkImplDesc->GetDescription();
                if (!strDesc.IsEmpty())
                {
                    strDesc.Replace("\t", "");
                    strDesc.Replace("\n", "\n\t");
                    strMessage += "\t" + strDesc;
                }
                else
                {
                    strMessage += "\tNo description was provided for"
                        " this technique.";
                }

                unsigned int uiMinMajor = 
                    NiShaderRequirementDesc::GetMajorVertexShaderVersion(
                    pkImplDesc->GetMinVertexShaderVersion());
                unsigned int uiMinMinor = 
                    NiShaderRequirementDesc::GetMinorVertexShaderVersion(
                    pkImplDesc->GetMinVertexShaderVersion());
                
                NIASSERT(pkImplDesc->GetMinVertexShaderVersion() == 
                    pkImplDesc->GetMaxVertexShaderVersion());
                strMessage +="\n\nVertex Shader Version: \t" + 
                    NiString::FromInt(uiMinMajor) + "." + 
                    NiString::FromInt(uiMinMinor);
                
                uiMinMajor = 
                    NiShaderRequirementDesc::GetMajorPixelShaderVersion(
                    pkImplDesc->GetMinPixelShaderVersion());
                uiMinMinor = 
                    NiShaderRequirementDesc::GetMinorPixelShaderVersion(
                    pkImplDesc->GetMinPixelShaderVersion());
                NIASSERT(pkImplDesc->GetMinPixelShaderVersion() == 
                    pkImplDesc->GetMaxPixelShaderVersion());
                
                strMessage +="\nPixel Shader Version: \t" + 
                    NiString::FromInt(uiMinMajor) + "." + 
                    NiString::FromInt(uiMinMinor);

                strMessage +="\nPlatforms: ";
                if (pkImplDesc->IsPlatformDX9())
                    strMessage +="DX9\t";
                if (pkImplDesc->IsPlatformD3D10())
                    strMessage +="D3D10\t";
                if (pkImplDesc->IsPlatformXenon())
                    strMessage +="Xenon\t";
                strMessage += "\n";


            }
        }
        else
        {
            strMessage += "No information was provided for this shader.";
        }
    }

    MessageBox(hWndParent, (const char*) strMessage, (const char*) strTitle, 
        MB_OK | MB_ICONINFORMATION);
                        
}
//---------------------------------------------------------------------------
void FillTechniqueDropdowns(const char* pcShaderName, HWND hWndParent, 
                            unsigned int uiWhichViewer, 
                            unsigned int uiWhichExport)
{
    CHECK_MEMORY();
    HWND hWndViewerCombo = GetDlgItem(hWndParent, IDC_VIEWER_TECHNIQUE_COMBO);
    SendMessage(hWndViewerCombo, CB_RESETCONTENT, 0, 0);

    HWND hWndExportCombo = GetDlgItem(hWndParent, IDC_EXPORT_TECHNIQUE_COMBO);
    SendMessage(hWndExportCombo, CB_RESETCONTENT, 0, 0);

    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();

    SendMessage(hWndViewerCombo, CB_ADDSTRING, 0, (LPARAM) 
        NI_DEFAULT_SHADER_TECHNIQUE_NAME);
    SendMessage(hWndExportCombo, CB_ADDSTRING, 0, (LPARAM)
        NI_DEFAULT_SHADER_TECHNIQUE_NAME);
    
    NiShaderDesc* pkDesc = pkToolkit->GetMaterialDesc(pcShaderName);

    if (pkDesc)
    {
        for (unsigned int ui = 0; ui < pkDesc->GetNumberOfImplementations(); 
            ui++)
        {
            const NiShaderDescBase* pkImplDesc = 
                pkDesc->GetImplementationDescription(ui);
            NiString strDesc(pkImplDesc->GetName());
            
            NiStringTokenizer strTok(strDesc);
            strDesc = strTok.GetNextToken(".!?\n\r\x00A");
            if (strDesc != NULL)
            {
                SendMessage(hWndViewerCombo, CB_ADDSTRING, 0, (LPARAM) 
                    (const char*)strDesc);
                SendMessage(hWndExportCombo, CB_ADDSTRING, 0, (LPARAM) 
                    (const char*)strDesc);
            }
            else
            {
                NiString strDesc;
                strDesc.Format("Implementation %i", ui);
                SendMessage(hWndViewerCombo, CB_ADDSTRING, 0, (LPARAM) 
                    (const char*)strDesc);
                SendMessage(hWndExportCombo, CB_ADDSTRING, 0, (LPARAM) 
                    (const char*)strDesc);
            }
        }
    }

    SendMessage(hWndViewerCombo, CB_SETCURSEL, uiWhichViewer, 0);
    SendMessage(hWndExportCombo, CB_SETCURSEL, uiWhichExport, 0);
    CHECK_MEMORY();

}
