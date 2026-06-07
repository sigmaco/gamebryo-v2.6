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
#include "CoordinateSystemConverterDialog.h"
#include "CoordinateSystemConverter_resource.h"
#include "CoordinateSystemConverterDLLDefines.h"

//---------------------------------------------------------------------------
CoordinateSystemConverterDialog::CoordinateSystemConverterDialog(
    NiModuleRef hInstance, NiWindowRef hWndParent, 
    NiPluginInfo* pkPluginInfo) : 
    NiDialog(IDD_OPT_COORDINATE_SYSTEM_CONVERTER_OPTIONS, hInstance, 
        hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();

}
//---------------------------------------------------------------------------
CoordinateSystemConverterDialog::~CoordinateSystemConverterDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
int CoordinateSystemConverterDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void CoordinateSystemConverterDialog::InitDialog()
{
    NiString strValue = m_spPluginInfoInitial->GetValue(COORDINATE_SYSTEM);

    int idValue = IDC_MAYA_TO_MAX;

    if (strValue == MAYA_TO_MAX)
        idValue = IDC_MAYA_TO_MAX;
    
    if (strValue == MAX_TO_MAYA)
        idValue = IDC_MAX_TO_MAYA;

    CheckRadioButton(m_hWnd, IDC_MAYA_TO_MAX, IDC_MAX_TO_MAYA, idValue);

}
//---------------------------------------------------------------------------
BOOL CoordinateSystemConverterDialog::OnCommand(int iWParamLow, 
                                                int,
                                                long)
{
    switch(iWParamLow)
    {
    case IDOK:
    {
            // Save options.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    }
    case IDCANCEL:
    {
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr CoordinateSystemConverterDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void CoordinateSystemConverterDialog::HandleOK()
{
    // Get the coordinate system. Default is Right handed.
    if (IsDlgButtonChecked(m_hWnd, IDC_MAYA_TO_MAX) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetValue(COORDINATE_SYSTEM, MAYA_TO_MAX);
    }
    else
    {
        m_spPluginInfoResults->SetValue(COORDINATE_SYSTEM, MAX_TO_MAYA);
    }
}
//---------------------------------------------------------------------------
