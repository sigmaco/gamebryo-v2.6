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

// ViewerStringsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ViewerStringsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CViewerStringsDlg dialog
//---------------------------------------------------------------------------
CViewerStringsDlg::CViewerStringsDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CViewerStringsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CViewerStringsDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_kStringArray.SetSize(20);
    m_kStringArray.SetGrowBy(25);
    m_eShowType = CViewerStringsDlg::FULL;
}
//---------------------------------------------------------------------------
void CViewerStringsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CViewerStringsDlg)
    DDX_Control(pDX, IDC_VIEWER_STRING_LIST, m_wndViewerStringsList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CViewerStringsDlg, CDialog)
    //{{AFX_MSG_MAP(CViewerStringsDlg)
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CViewerStringsDlg message handlers
//---------------------------------------------------------------------------
BOOL CViewerStringsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(m_pkObj != NULL);
    SetStrings();

    return TRUE;
}
//---------------------------------------------------------------------------
void CViewerStringsDlg::SetStrings()
{
    unsigned int ui = 0;
    for(; ui < m_kStringArray.GetSize(); ui++)
    {
        NiFree(m_kStringArray.RemoveAt(ui));
    }
    m_wndViewerStringsList.ResetContent();


    switch(m_eShowType)
    {
        case LIMITED:
            m_pkObj->AddViewerStrings(&m_kStringArray);
            break;
        case FULL:
        default:
            m_pkObj->GetViewerStrings(&m_kStringArray);
            break;
    }




    CDC* pDC = m_wndViewerStringsList.GetDC();
    CSize TextSize;
    int iMaxWidth = 0;

    int iItem = 0;

    for(ui = 0; ui < m_kStringArray.GetSize(); ui++)
    {
        char* pcString = m_kStringArray.GetAt(ui);
        if(pcString != NULL)
        {
            m_wndViewerStringsList.InsertString(iItem, pcString);
            iItem++;

            // Get the Extent of the String
            TextSize = pDC->GetTextExtent(pcString);

            // Increase the max width if it is larger
            if (iMaxWidth < TextSize.cx)
                iMaxWidth = TextSize.cx;
        }
    }

    // Get the Text Metrics
    TEXTMETRIC   tm;
    pDC->GetTextMetrics(&tm);

     // Add the avg width to prevent clipping
     iMaxWidth += tm.tmAveCharWidth;

    // Set the Horizontal Extent of the List Box
    m_wndViewerStringsList.SetHorizontalExtent(iMaxWidth);

}
//---------------------------------------------------------------------------
void CViewerStringsDlg::OnDestroy() 
{
    CDialog::OnDestroy();
    
    for(unsigned int ui = 0; ui < m_kStringArray.GetSize(); ui++)
    {
        NiFree( m_kStringArray.RemoveAt(ui));
    }
    m_wndViewerStringsList.ResetContent();
    
}
//---------------------------------------------------------------------------
bool CViewerStringsDlg::DoUpdate()
{
    SetStrings();
    return true;
}
//---------------------------------------------------------------------------
