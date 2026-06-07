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

// NiMeshModifierDlg.cpp

#include "stdafx.h"
#include "commctrl.h"
#include "AssetViewer.h"
#include "NiMeshModifierDlg.h"
#include "NifPropertyWindowManager.h"

#include <NiMeshModifier.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiMeshModifierDlg dialog
//---------------------------------------------------------------------------
CNiMeshModifierDlg::CNiMeshModifierDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiMeshModifierDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiMeshModifierDlg)
    //}}AFX_DATA_INIT
}
CNiMeshModifierDlg::~CNiMeshModifierDlg()
{
}
//---------------------------------------------------------------------------
void CNiMeshModifierDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiMeshModifierDlg)
    DDX_Control(pDX, IDC_NIMESHMODIFIER_NAME, m_wndModifierName);
    DDX_Control(pDX, IDC_NIMESHMODIFIER_SUBMIT_SYNCH, 
        m_wndSubmitSynchNames);
    DDX_Control(pDX, IDC_NIMESHMODIFIER_COMPLETE_SYNCH, 
        m_wndCompleteSynchNames);
    DDX_Control(pDX, IDC_NIMESHMODIFIER_VIEWER_STRINGS,
        m_wndViewerStringsList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiMeshModifierDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiMeshModifierDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiMeshModifierDlg message handlers
//---------------------------------------------------------------------------
bool CNiMeshModifierDlg::DoUpdate()
{
    if(!NiIsKindOf(NiMeshModifier, m_pkObj))
        return false;

    NiMeshModifier* pkMeshModifier = (NiMeshModifier*) m_pkObj;
    
    // Set the modifier name
    m_wndModifierName.SetWindowText(pkMeshModifier->GetRTTI()->GetName());

    // 
    NiString strWindowText = "None";
    bool bFirstEntry = true;

    // Set the supported sync points
    for (NiUInt32 ui = 0; ui < pkMeshModifier->GetSubmitSyncPointCount(); ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt =
            pkMeshModifier->GetSubmitSyncPoint(ui);

        if (bFirstEntry)
        {
            strWindowText = NiSyncArgs::GetSyncPointName(uiSyncPt);
            bFirstEntry = false;
        }
        else
        {
            strWindowText += " | ";
            strWindowText += NiSyncArgs::GetSyncPointName(uiSyncPt);
        }
    }

    m_wndSubmitSynchNames.SetWindowText((const char*)strWindowText);
    
    // Reset the window text
    strWindowText = "None";
    bFirstEntry = true;

    for (NiUInt32 ui = 0; ui < pkMeshModifier->GetCompleteSyncPointCount();
        ui++)
    {
        NiSyncArgs::SyncPoint uiSyncPt =
            pkMeshModifier->GetCompleteSyncPoint(ui);

        if (bFirstEntry)
        {
            strWindowText = NiSyncArgs::GetSyncPointName(uiSyncPt);
            bFirstEntry = false;
        }
        else
        {
            strWindowText += " | ";
            strWindowText += NiSyncArgs::GetSyncPointName(uiSyncPt);
        }
    }

    m_wndCompleteSynchNames.SetWindowText((const char*)strWindowText);

    // Reset the text and flag back to default
    strWindowText = "None";
    bFirstEntry = true;

    // Set the viewer strings
    NiViewerStringsArray kStrings;
    pkMeshModifier->GetViewerStrings(&kStrings);
    
    m_wndViewerStringsList.ResetContent();

    CDC* pDC = m_wndViewerStringsList.GetDC();
    CSize TextSize;
    int iMaxWidth = 0;
    int iItem = 0;

    for(NiUInt32 ui = 0; ui < kStrings.GetSize(); ui++)
    {
        char* pcString = kStrings.GetAt(ui);
        if(pcString != NULL)
        {
            m_wndViewerStringsList.InsertString(iItem, pcString);
            iItem++;

            // Get the Extent of the String
            TextSize = pDC->GetTextExtent(pcString);

            // Increase the max width if it is larger
            if (iMaxWidth < TextSize.cx)
                iMaxWidth = TextSize.cx;

            NiFree(pcString);
            kStrings.SetAt(ui, NULL);
        }
    }

    kStrings.RemoveAll();

    // Get the Text Metrics
    TEXTMETRIC   tm;
    pDC->GetTextMetrics(&tm);

     // Add the avg width to prevent clipping
     iMaxWidth += tm.tmAveCharWidth;

    // Set the Horizontal Extent of the List Box
    m_wndViewerStringsList.SetHorizontalExtent(iMaxWidth);

    return true;

}
//---------------------------------------------------------------------------
BOOL CNiMeshModifierDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    ASSERT(NiIsKindOf(NiMeshModifier, m_pkObj));  

    DoUpdate();
    return TRUE;  
}
//---------------------------------------------------------------------------
