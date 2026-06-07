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
#include "FurGeneratorDialog.h"
#include "FurGeneratorDefines.h"
#include "FurGenerator_resource.h"

//---------------------------------------------------------------------------
FurGeneratorDialog::FurGeneratorDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_OPT_FURGENERATOR, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();

}
//---------------------------------------------------------------------------
FurGeneratorDialog::~FurGeneratorDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
int FurGeneratorDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void FurGeneratorDialog::InitDialog()
{
    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_LENGTH), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_LENGTH));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_LENGTH_RANDOMNESS), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_LENGTH_RANDOMNESS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_TEXTURE_TILES_PER_FIN), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_TEXTURE_TILES_PER_FIN_RANDOMNESS), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN_RANDOMNESS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_SHELL_TEXTURE_SIZE), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_SHELL_TEXTURE_SIZE));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_SHELL_SIZE_FACTOR), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_SHELL_SIZE_FACTOR));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_NUM_SHELLS), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_NUM_SHELLS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_DENSITY), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_DENSITY));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_R), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_TINT_COLOR_R));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_G), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_TINT_COLOR_G));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_B), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_TINT_COLOR_B));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_A), m_spPluginInfoInitial
        ->GetValue(OPT_FURGENERATOR_FUR_TINT_COLOR_A));

    CheckDlgButton(m_hWnd, IDC_CHECK_GENERATE_DENSE_FINS, 
        m_spPluginInfoInitial->GetBool(OPT_FURGENERATOR_GENERATE_DENSE_FINS));
}
//---------------------------------------------------------------------------
BOOL FurGeneratorDialog::OnCommand(int iWParamLow, int, 
    long)
{
    switch(iWParamLow)
    {
    case IDOK:
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr FurGeneratorDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void FurGeneratorDialog::HandleOK()
{
    char acText[100];
    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_LENGTH), &acText[0], 100);
    float fFurLength = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(OPT_FURGENERATOR_FUR_LENGTH, fFurLength);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_LENGTH_RANDOMNESS), &acText[0], 100);
    float fFurLengthRandomness = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(OPT_FURGENERATOR_FUR_LENGTH_RANDOMNESS,
        fFurLengthRandomness);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_TEXTURE_TILES_PER_FIN), &acText[0], 100);
    float fTextureTilesPerFin = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN, fTextureTilesPerFin);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_TEXTURE_TILES_PER_FIN_RANDOMNESS), &acText[0], 100);
    float fTextureTilesPerFinRandomness = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_TEXTURE_TILES_PER_FIN_RANDOMNESS,
        fTextureTilesPerFinRandomness);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_NUM_SHELLS), &acText[0], 100);
    int nNumShells = atoi(acText);
    m_spPluginInfoResults->SetInt(OPT_FURGENERATOR_NUM_SHELLS, nNumShells);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_SHELL_SIZE_FACTOR), &acText[0], 100);
    float fShellSizeFactor = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(OPT_FURGENERATOR_SHELL_SIZE_FACTOR,
        fShellSizeFactor);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_SHELL_TEXTURE_SIZE), &acText[0], 100);
    int nShellTextureSize = atoi(acText);
    m_spPluginInfoResults->SetInt(OPT_FURGENERATOR_SHELL_TEXTURE_SIZE,
        nShellTextureSize);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_DENSITY), &acText[0], 100);
    float fFurDensity = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_FUR_DENSITY,
        fFurDensity);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_R), &acText[0], 100);
    float fFurTint = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_FUR_TINT_COLOR_R,
        fFurTint);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_G), &acText[0], 100);
    fFurTint = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_FUR_TINT_COLOR_G,
        fFurTint);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_B), &acText[0], 100);
    fFurTint = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_FUR_TINT_COLOR_B,
        fFurTint);

    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_FUR_TINT_A), &acText[0], 100);
    fFurTint = (float)atof(acText);
    m_spPluginInfoResults->SetFloat(
        OPT_FURGENERATOR_FUR_TINT_COLOR_A,
        fFurTint);

    bool bGenerateDenseFins = 
        (IsDlgButtonChecked(m_hWnd, IDC_CHECK_GENERATE_DENSE_FINS)) ? 
        true : false;
    m_spPluginInfoResults->SetBool(OPT_FURGENERATOR_GENERATE_DENSE_FINS,
        bGenerateDenseFins);
}
//---------------------------------------------------------------------------
