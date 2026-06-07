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
#include "AssetAnalyzerResultsDialog.h"
#include "AssetAnalyzerPluginDefines.h"
#include "resource.h"

//---------------------------------------------------------------------------
AssetAnalyzerResultsDialog::AssetAnalyzerResultsDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiTPointerList<NiString*>* pkErrors) :
    NiDialog(IDD_RESULTS_DIALOG, hInstance, hWndParent)
{
    m_pkErrors = pkErrors;
}
//---------------------------------------------------------------------------
AssetAnalyzerResultsDialog::~AssetAnalyzerResultsDialog()
{
}
//---------------------------------------------------------------------------
void AssetAnalyzerResultsDialog::InitDialog()
{
    // Convert to a single string
    NiString kErrorText;
    NiString kTempString;

    NiTListIterator kIter = m_pkErrors->GetHeadPos();

    while (kIter != NULL)
    {
        // Convert any Tabs you find.
        kTempString = *m_pkErrors->GetNext(kIter);
        kTempString.Replace("\t", "    ");

        // Append into the Text String
        kErrorText += kTempString;
        kErrorText += "\r\n";
    }


    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RESULTS), kErrorText);
}
//---------------------------------------------------------------------------
BOOL AssetAnalyzerResultsDialog::OnCommand(int iWParamLow, int,
    long)
{
    switch (iWParamLow)
    {
    case IDOK:
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
