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

// ExtraDataDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ExtraDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CExtraDataDlg dialog
//---------------------------------------------------------------------------
CExtraDataDlg::CExtraDataDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CExtraDataDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CExtraDataDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CExtraDataDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CExtraDataDlg)
    DDX_Control(pDX, IDC_EXTRA_DATA_LIST, m_wndExtraDataList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CExtraDataDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CExtraDataDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CExtraDataDlg message handlers
//---------------------------------------------------------------------------
bool CExtraDataDlg::DoUpdate()
{
    ASSERT(NiIsKindOf(NiObjectNET, m_pkObj));
    NiObjectNET* pkObj = (NiObjectNET*) m_pkObj;

    NiTPrimitiveArray<char*> kArray(25);
    kArray.SetGrowBy(25);

    if (pkObj)
    {
        for (unsigned int i=0; i < pkObj->GetExtraDataSize(); i++)
        {
            NiExtraData* pkExtra = pkObj->GetExtraDataAt(i);

            pkExtra->GetViewerStrings(&kArray);
        }
    }

    CDC* pDC = m_wndExtraDataList.GetDC();
    CSize TextSize;
    int iMaxWidth = 0;


    int iItem = 0;
    m_wndExtraDataList.ResetContent();
    for(unsigned int ui = 0; ui < kArray.GetSize(); ui++)
    {
        char* pcString = kArray.GetAt(ui);
        if(pcString != NULL)
        {
            this->m_wndExtraDataList.InsertString( iItem , pcString);
            iItem++;

            // Get the Extent of the String
            TextSize = pDC->GetTextExtent(pcString);

            // Increase the max width if it is larger
            if (iMaxWidth < TextSize.cx)
                iMaxWidth = TextSize.cx;

        }
        NiFree(pcString);
    }

    // Get the Text Metrics
    TEXTMETRIC   tm;
    pDC->GetTextMetrics(&tm);

    // Add the avg width to prevent clipping
    iMaxWidth += tm.tmAveCharWidth;

    // Set the Horizontal Extent of the List Box
    m_wndExtraDataList.SetHorizontalExtent(iMaxWidth);

    return true;

}
//---------------------------------------------------------------------------
BOOL CExtraDataDlg::OnInitDialog() 
{
    CNiObjectDlg::OnInitDialog();
    
    DoUpdate();

    return TRUE;
}
//---------------------------------------------------------------------------
