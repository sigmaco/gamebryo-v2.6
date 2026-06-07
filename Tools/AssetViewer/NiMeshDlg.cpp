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

// NiMeshDlg.cpp

#include "stdafx.h"
#include "commctrl.h"
#include "AssetViewer.h"
#include "NiMeshDlg.h"
#include "NiDataStreamDlg.h"
#include "NifDoc.h"
#include "NifPropertyWindowManager.h"

#include <NiMesh.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiMeshDlg dialog
//---------------------------------------------------------------------------
CNiMeshDlg::CNiMeshDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiMeshDlg::IDD, pParent)
    , m_pilImages(NULL)
    , m_pCachedTreeCtrl(NULL)
{
    //{{AFX_DATA_INIT(CNiMeshDlg)
    //}}AFX_DATA_INIT
}
CNiMeshDlg::~CNiMeshDlg()
{
    NiDelete m_pilImages;
}
//---------------------------------------------------------------------------
void CNiMeshDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiMeshDlg)
    DDX_Control(pDX, IDC_NIMESH_SHADERNAME, m_wndShaderName);
    DDX_Control(pDX, IDC_NIMESH_MODEL_BOUND_RADIUS_EDIT, 
        m_wndModelBoundRadiusEdit);
    DDX_Control(pDX, IDC_NIMESH_MODEL_BOUND_CENTER_Z_EDIT, 
        m_wndModelBoundCenterZEdit);
    DDX_Control(pDX, IDC_NIMESH_MODEL_BOUND_CENTER_Y_EDIT, 
        m_wndModelBoundCenterYEdit);
    DDX_Control(pDX, IDC_NIMESH_MODEL_BOUND_CENTER_X_EDIT, 
        m_wndModelBoundCenterXEdit);
    DDX_Control(pDX, IDC_NIMESH_MODIFIER_LIST, m_wndMeshModifierList);
    DDX_Control(pDX, IDC_NIDATASTREAMGROUP_LIST, m_wndDataStreamList);
    DDX_Control(pDX, IDC_NIDATASTREAMGROUP_TREE, m_wndTreeControl);
    DDX_Control(pDX, IDC_NIMESH_PRIMITIVETYPE, m_wndPrimitiveType);
    DDX_Control(pDX, IDC_NIMESH_SUBMESHCOUNT_EDIT, m_wndSubmeshCountEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiMeshDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiMeshDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_NIMESH_MODIFIER_LIST, OnDblclkNiMeshModifierList)
    ON_NOTIFY(NM_DBLCLK, IDC_NIDATASTREAMGROUP_LIST, 
        OnDblclkNiDataStreamGroupList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiMeshDlg message handlers
//---------------------------------------------------------------------------
bool CNiMeshDlg::DoUpdate()
{
    if(!NiIsKindOf(NiMesh, m_pkObj))
        return false;

    NiMesh* pkMesh = (NiMesh*) m_pkObj;
    
    char acString[256];

    m_wndPrimitiveType.SetWindowText(pkMesh->GetPrimitiveTypeString());

    NiSprintf(acString, 256, "%d", pkMesh->GetSubmeshCount());
    m_wndSubmeshCountEdit.SetWindowText(acString);

    const NiMaterial* pkMaterial = pkMesh->GetActiveMaterial();
    if (pkMaterial)
        NiSprintf(acString, 256, "%s", (const char*)pkMaterial->GetName());
    else
        NiSprintf(acString, 256, "Default Fixed-Function Pipeline");
    m_wndShaderName.SetWindowText(acString);

    NiBound kBound = pkMesh->GetModelBound();
    NiSprintf(acString, 256, "%.4f", kBound.GetRadius());
    m_wndModelBoundRadiusEdit.SetWindowText(acString);

    NiPoint3 kPoint = kBound.GetCenter();
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndModelBoundCenterZEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndModelBoundCenterYEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndModelBoundCenterXEdit.SetWindowText(acString);

    m_wndMeshModifierList.DeleteAllItems();

    for (NiUInt32 ui = 0; ui < pkMesh->GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = pkMesh->GetModifierAt(ui);
        if (pkModifier)
        {
            NiSprintf(acString, 256, "%d", ui);
            m_wndMeshModifierList.InsertItem(ui, acString);
            m_wndMeshModifierList.SetItemText(ui, 0, 
                pkModifier->GetRTTI()->GetName());
        }
    }

    m_wndDataStreamList.DeleteAllItems();

    NiUInt32 uiStreamCount = pkMesh->GetStreamRefCount();
    for (NiUInt32 ui = 0; ui < uiStreamCount; ui++)
    {
        NiDataStreamRef* pkStreamRef = pkMesh->GetStreamRefAt(ui);

        NiUInt32 uiElemCount = pkStreamRef->GetElementDescCount();
        for (NiUInt32 j = 0; j < uiElemCount ; j++)
        {            
            CString strName;
            strName.Format("Semantic: %s, SemanticIndex: %d",
                pkStreamRef->GetSemanticNameAt(j),
                pkStreamRef->GetSemanticIndexAt(j));

            m_wndDataStreamList.InsertItem(m_wndDataStreamList.GetItemCount(),
                strName);
        }
    }

    // Cache the current data stream group so we know if we really 
    // need to refresh the tree controll.  This is done so that 
    // during animation play back that the screen won't flash.
    if (m_pCachedTreeCtrl != pkMesh)
    {
        // Store the pointer to the stream group
        m_pCachedTreeCtrl = pkMesh;

        m_wndTreeControl.DeleteAllItems();

        //m_wndTreeControl.SetItemData(hItem, (DWORD)0);
        NiUInt32 uiStreamCount = pkMesh->GetStreamRefCount();
        for (NiUInt32 ui = 0; ui < uiStreamCount; ui++)
        {
            NiDataStreamRef* pkStreamRef = pkMesh->GetStreamRefAt(ui);
            NiDataStream* pkDataStream = pkStreamRef->GetDataStream();

            // Add parent element
            CString strName;
            strName.Format("StreamRef: %d", ui);
            HTREEITEM hParent = 
                m_wndTreeControl.InsertItem(strName, 5, 5, TVI_ROOT, TVI_LAST);

            // Add data stream info
            strName.Format("DataStream: Stride: %d, "
                "Size: %d, Data@:(0x%.8x)", 
                pkDataStream->GetStride(), pkDataStream->GetSize(), 
                pkDataStream);
            HTREEITEM hDataP = 
                m_wndTreeControl.InsertItem(strName, 4, 4, hParent, TVI_LAST);

            // Add Usage Info
            NiString kUsageString = NiDataStream::UsageToString(
                pkDataStream->GetUsage());
            strName.Format("Usage: %s", (const char*) kUsageString);
            m_wndTreeControl.InsertItem(strName, 7, 7, hDataP, TVI_LAST);

            // Add Access Info
            NiUInt8 uiAccessMask;
            if (!CNifDoc::GetDocument()->GetDataStreamInfo(pkDataStream,
                uiAccessMask))
            {
                // If this assert is hit, it means that the
                // NiTDataStreamFactoryProxy did not record the creation
                // of this stream via CNifDoc::SetDataStreamInfo.  If this
                // is the case, then the access mask will have
                // the values that AssetViewer is using (i.e. or-ing in
                // CPU_READ) rather than the original streamed values.
                NIASSERT(!"No mapping for this datastream.");
                uiAccessMask = pkDataStream->GetAccessMask();
            }
            NiString kAccessString = NiDataStream::AccessFlagsToString(
                uiAccessMask);
            strName.Format("Access: %s", (const char*) kAccessString);
            m_wndTreeControl.InsertItem(strName, 7, 7, hDataP, TVI_LAST);

            // Add Instance info
            if (pkStreamRef->IsPerInstance())
                strName = "Per Instance: YES";
            else
                strName = "Per Instance: NO";
            m_wndTreeControl.InsertItem(strName, 7, 7, hDataP, TVI_LAST);

            // Add elements array
            strName.Format("Elements");
            HTREEITEM hElemsP = 
                m_wndTreeControl.InsertItem(strName, 7, 7, hDataP, TVI_LAST);

            NiUInt32 uiElemCount = pkStreamRef->GetElementDescCount();
            for (NiUInt32 j = 0; j < uiElemCount ; j++)
            {
                NiDataStreamElement kElement = 
                    pkStreamRef->GetElementDescAt(j);
                
                strName.Format("%d: Semantic: %s, SemanticIndex: %d", 
                    j, 
                    pkStreamRef->GetSemanticNameAt(j),
                    pkStreamRef->GetSemanticIndexAt(j));

                HTREEITEM hThisElementP = m_wndTreeControl.InsertItem(
                    strName, 7, 7, hElemsP, TVI_LAST);

                strName.Format("Offset: %d, Format: %s "
                    "(Type: %s Count: %d Normalized: %d)",
                    kElement.GetOffset(), 
                    kElement.GetFormatString(),
                    NiDataStreamElement::GetTypeString(kElement.GetType()),
                    kElement.GetComponentCount(),
                    kElement.IsNormalized());

                m_wndTreeControl.InsertItem(
                    strName, 7, 7, hThisElementP, TVI_LAST);
                m_wndTreeControl.Expand(hThisElementP, TVE_EXPAND);
            }
            m_wndTreeControl.Expand(hElemsP, TVE_EXPAND);

            // Add Region data
            strName.Format("Regions");
            HTREEITEM hRegionsP = 
                m_wndTreeControl.InsertItem(strName, 5, 5, hDataP, TVI_LAST);

            NiUInt32 uiRegionCount = pkDataStream->GetRegionCount();
            for (NiUInt32 j = 0; j < uiRegionCount; j++)
            {
                NiDataStream::Region& kRegion = pkDataStream->GetRegion(j);
                strName.Format("%d: (StartIndex: %d, Range: %d)",
                    j,
                    kRegion.GetStartIndex(),
                    kRegion.GetRange());
                m_wndTreeControl.InsertItem(strName, 7, 7, hRegionsP,TVI_LAST);
            }
            m_wndTreeControl.Expand(hRegionsP, TVE_EXPAND);

             // Add Region data
            strName.Format("Submesh Bindings");
            HTREEITEM hSubmeshP = 
                m_wndTreeControl.InsertItem(strName, 5, 5, hDataP, TVI_LAST);

            NiUInt32 uiSubmeshCount = pkStreamRef->GetSubmeshRemapCount();
            NIASSERT(uiSubmeshCount == pkMesh->GetSubmeshCount());

            for (NiUInt32 j = 0; j < pkMesh->GetSubmeshCount(); j++)
            {
                NiDataStream::Region& kRegion = 
                    pkStreamRef->GetRegionForSubmesh(j);
                strName.Format("%d: Region Binding: %d (Start Index: %d, "
                    "Range: %d)",
                    j,
                    pkStreamRef->GetRegionIndexForSubmesh(j),
                    kRegion.GetStartIndex(),
                    kRegion.GetRange());
                m_wndTreeControl.InsertItem(strName, 7, 7, hSubmeshP, 
                    TVI_LAST);
            }
            m_wndTreeControl.Expand(hSubmeshP, TVE_EXPAND);

            m_wndTreeControl.Expand(hDataP, TVE_EXPAND);
            m_wndTreeControl.Expand(hParent, TVE_EXPAND);
        }

        m_wndTreeControl.SetScrollPos(SB_VERT, 0, true);
    }

    return true;

}
//---------------------------------------------------------------------------
BOOL CNiMeshDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    ASSERT(NiIsKindOf(NiMesh, m_pkObj));

    m_wndMeshModifierList.InsertColumn(0, "Name");
    CRect rect;
    m_wndMeshModifierList.GetWindowRect(&rect);
    m_wndMeshModifierList.SetColumnWidth(0, rect.Width()-4);

    m_wndDataStreamList.InsertColumn(0, "Data Streams");
    m_wndDataStreamList.GetWindowRect(&rect);
    m_wndDataStreamList.SetColumnWidth(0, rect.Width()-4);

    if (!m_pilImages)
    {
        m_pilImages = new CImageList;
        m_pilImages->Create(IDB_TREEIMAGES, 16, 1, RGB (255, 0, 255));
        m_wndTreeControl.SetImageList(m_pilImages, TVSIL_NORMAL);
    }    
    
    DoUpdate();
    return TRUE;  
    // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
void CNiMeshDlg::OnDblclkNiMeshModifierList(NMHDR* pNMHDR, 
    LRESULT* pResult) 
{
    *pResult = 0;

    if(!NiIsKindOf(NiMesh, m_pkObj))
        return;
    
    if(m_wndMeshModifierList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndMeshModifierList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiMesh* pkMesh = (NiMesh*) m_pkObj;
    for (NiUInt32 ui = 0; ui < pkMesh->GetModifierCount(); ui++)
    {
        NiMeshModifier* pkMeshModifier = pkMesh->GetModifierAt(ui);
        if (pkMeshModifier)
        {
            if(iMatchIndex == iIndex)
            {
                CNifPropertyWindowManager* pkManager = 
                    CNifPropertyWindowManager::GetPropertyWindowManager();

                pkManager->CreatePropertyWindow(pkMeshModifier);
                return;
            }

            iMatchIndex++;
        }
    }
}
//---------------------------------------------------------------------------

void CNiMeshDlg::OnDblclkNiDataStreamGroupList(NMHDR *pNMHDR, LRESULT *pResult)
{
    
    *pResult = 0;

    if(!NiIsKindOf(NiMesh, m_pkObj))
        return;
    
    if(m_wndDataStreamList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndDataStreamList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    CString kText = m_wndDataStreamList.GetItemText(iIndex, 0);

    int nStartSemanticPos = kText.Find("Semantic:");
    if (nStartSemanticPos < 0)
        return;
    nStartSemanticPos += 10;

    int nEndSemanticPos = kText.Find(",", nStartSemanticPos);
    if (nEndSemanticPos < 0)
        return;

    int nStartIndexPos = kText.Find("Index:");
    if (nStartIndexPos < 0)
        return;
    nStartIndexPos += 7;

    int nEndIndexPos = kText.Find(" ", nStartIndexPos);
    if (nEndIndexPos < 0)
        nEndIndexPos = kText.GetLength();

    NiFixedString kSemantic = (const char*)kText.Mid(nStartSemanticPos, 
        nEndSemanticPos - nStartSemanticPos);

    NiUInt32 uiIndex = atoi((const char*)kText.Mid(nStartIndexPos, 
        nEndIndexPos - nStartIndexPos));

    NiDataStreamObjectPtr pkDataStreamObject = 
        NiTCreate<NiDataStreamObject>();
    pkDataStreamObject->m_pkMesh = (NiMesh*) m_pkObj;
    pkDataStreamObject->m_kSemantic = kSemantic;
    pkDataStreamObject->m_uiSemanticIndex = uiIndex;

    CNifPropertyWindowManager* pkManager = 
        CNifPropertyWindowManager::GetPropertyWindowManager();
    pkManager->CreatePropertyWindow(pkDataStreamObject);

}
