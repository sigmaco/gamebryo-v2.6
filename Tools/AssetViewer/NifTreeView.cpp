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

// NifTreeView.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifTreeView.h"
#include "NifPropertyWindowManager.h"
#include "NifDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifTreeView
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifTreeView, CTreeView)
//---------------------------------------------------------------------------
CNifTreeView::CNifTreeView()
{
    m_pilImages = NULL;
}
//---------------------------------------------------------------------------
CNifTreeView::~CNifTreeView()
{
    delete m_pilImages;
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifTreeView, CTreeView)
    //{{AFX_MSG_MAP(CNifTreeView)
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_TREEITEM_REMOVENIF, OnRemoveNif)
    ON_COMMAND(ID_TREEITEM_EXPANDALL, OnExpandAll)
    ON_COMMAND(ID_TREEITEM_COLLAPSEALL, OnCollapseAll)
    ON_COMMAND(ID_TREEITEM_PROPERTIES, OnProperties)
    ON_COMMAND(ID_TREEITEM_SHOWHIDE, OnToggleAppCulled)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
    ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifTreeView drawing
//---------------------------------------------------------------------------
void CNifTreeView::OnDraw(CDC* pDC)
{
    // Draw code can go here if needed
}
//---------------------------------------------------------------------------
// CNifTreeView diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CNifTreeView::AssertValid() const
{
    CTreeView::AssertValid();
}
//---------------------------------------------------------------------------
void CNifTreeView::Dump(CDumpContext& dc) const
{
    CTreeView::Dump(dc);
}
//---------------------------------------------------------------------------
CNifDoc* CNifTreeView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNifDoc)));
    return (CNifDoc*)m_pDocument;
}
#endif //_DEBUG
//---------------------------------------------------------------------------
// CNifTreeView message handlers
//---------------------------------------------------------------------------
void CNifTreeView::PopulateTreeCtrl()
{
    GetTreeCtrl().DeleteAllItems();

    GetDocument()->Lock();

    NiNode* pkScene = GetDocument()->GetSceneGraph();
    if (pkScene)
    {
        FillTreeRecursive(TVI_ROOT, pkScene);
        GetTreeCtrl().Expand(m_hSceneRoot, TVE_EXPAND);
    }

    GetDocument()->UnLock();
}
//---------------------------------------------------------------------------
void CNifTreeView::FillTreeRecursive(HTREEITEM hParent, NiNode* pkNode,
    HTREEITEM hInsertAfter)
{
    const char* pcRTTIName = pkNode->GetRTTI()->GetName();
    const char* pcObjectName = pkNode->GetName();
    if (!pcObjectName)
    {
        pcObjectName = "";
    }
    if (g_strBoundShapeRootName == pcObjectName)
    {
        return;
    }
    CString strItemName;
    strItemName.Format("%s \"%s\"", pcRTTIName, pcObjectName);

    int iImageIndex;
    if (hParent == TVI_ROOT)
    {
        iImageIndex = IM_WORLD;
    }
    else
    {
        iImageIndex = IM_NODE;
    }

    HTREEITEM hItem = GetTreeCtrl().InsertItem(strItemName, iImageIndex,
        iImageIndex, hParent, hInsertAfter);

    GetTreeCtrl().SetItemData(hItem, (DWORD) PtrToUlong(pkNode));

    if (hParent == TVI_ROOT)
    {
        m_hSceneRoot = hItem;
    }

    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                FillTreeRecursive(hItem, (NiNode*) pkChild);
            }
            else
            {
                pcRTTIName = pkChild->GetRTTI()->GetName();
                pcObjectName = pkChild->GetName();
                if (!pcObjectName)
                {
                    pcObjectName = "";
                }
                strItemName.Empty();
                strItemName.Format("%s \"%s\"", pcRTTIName, pcObjectName);

                iImageIndex = -1;
                if (NiIsKindOf(NiLight, pkChild))
                {
                    iImageIndex = IM_LIGHT;
                }
                else if (NiIsKindOf(NiCamera, pkChild))
                {
                    iImageIndex = IM_CAMERA;
                }
                else if (NiIsKindOf(NiGeometry, pkChild))
                {
                    iImageIndex = IM_GEOMETRY;
                }
                else if (NiIsKindOf(NiMesh, pkChild))
                {
                    iImageIndex = IM_GEOMETRY;
                }

                 HTREEITEM hChildItem = GetTreeCtrl().InsertItem(strItemName,
                     iImageIndex, iImageIndex, hItem);

                GetTreeCtrl().SetItemData(hChildItem, (DWORD) PtrToUlong(pkChild));
            }
        }
    }
}
//---------------------------------------------------------------------------
BOOL CNifTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
        TVS_SHOWSELALWAYS;

    return CTreeView::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------
void CNifTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    WORD wLoWord = LOWORD(lHint);
    switch (wLoWord)
    {
        case NIF_DESTROYSCENE:
            GetTreeCtrl().DeleteAllItems();
            break;
        case NIF_CREATESCENE:
            PopulateTreeCtrl();
            break;
        case NIF_ADDNIF:
        {
            unsigned int uiIndex = (unsigned int) HIWORD(lHint);
            AddNifAtIndex(uiIndex);
            break;
        }
        case NIF_REMOVENIF:
        {
            unsigned int uiIndex = (unsigned int) HIWORD(lHint);
            RemoveNifAtIndex(uiIndex);
            break;
        }
        case NIF_UPDATEDEFAULTNODE:
            UpdateDefaultNode();
            break;
        case NIF_SCENECHANGED:
            RepopulateTreeCtrl();
            break;
        case NIF_SELECTEDOBJECTCHANGED:
        {
            NiAVObject* pkObject = (NiAVObject*) pHint;
            HighlightObject(m_hSceneRoot, pkObject);
            break;
        }
        default:
            break;
    }

    //CTreeView::OnUpdate(pSender, lHint, pHint);
}
//---------------------------------------------------------------------------
void CNifTreeView::AddNifAtIndex(unsigned int uiIndex)
{
    GetDocument()->Lock();

    NiNode* pkScene = GetDocument()->GetSceneGraph();
    if (pkScene && uiIndex < pkScene->GetArrayCount())
    {
        unsigned int uiInsertAt;
        for (uiInsertAt = 0; uiInsertAt < pkScene->GetArrayCount();
            uiInsertAt++)
        {
            if (pkScene->GetAt(uiInsertAt))
            {
                if (uiIndex-- == 0)
                {
                    break;
                }
            }
        }

        NiNode* pkNewNode = NiDynamicCast(NiNode, pkScene->GetAt(uiInsertAt));
        if (pkNewNode)
        {
            FillTreeRecursive(m_hSceneRoot, pkNewNode);
        }
    }

    GetDocument()->UnLock();
}
//---------------------------------------------------------------------------
void CNifTreeView::RemoveNifAtIndex(unsigned int uiIndex)
{
    HTREEITEM hItem = GetTreeCtrl().GetChildItem(m_hSceneRoot);
    for (unsigned int ui = 0; ui < uiIndex; ui++)
    {
        hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
    }
    GetTreeCtrl().DeleteItem(hItem);
}
//---------------------------------------------------------------------------
void CNifTreeView::UpdateDefaultNode()
{
    GetDocument()->Lock();

    // Update default node (first child of scene root).
    HTREEITEM hDefaultNode = GetTreeCtrl().GetChildItem(m_hSceneRoot);
    UINT uiState = GetTreeCtrl().GetItemState(hDefaultNode, TVIS_EXPANDED);
    GetTreeCtrl().DeleteItem(hDefaultNode);
    FillTreeRecursive(m_hSceneRoot, GetDocument()->GetDefaultNode(),
        TVI_FIRST);
    hDefaultNode = GetTreeCtrl().GetChildItem(m_hSceneRoot);
    GetTreeCtrl().SetItemState(hDefaultNode, uiState, TVIS_EXPANDED);

    // Toggle the scene root node to refresh properly.
    GetTreeCtrl().Expand(m_hSceneRoot, TVE_TOGGLE);
    GetTreeCtrl().Expand(m_hSceneRoot, TVE_TOGGLE);

    GetDocument()->UnLock();
}
//---------------------------------------------------------------------------
void CNifTreeView::OnInitialUpdate() 
{
    CTreeView::OnInitialUpdate();

    if (!m_pilImages)
    {
        m_pilImages = new CImageList;
        m_pilImages->Create(IDB_TREEIMAGES, 16, 1, RGB (255, 0, 255));
        GetTreeCtrl().SetImageList(m_pilImages, TVSIL_NORMAL);
    }
}
//--------------------------------------------------------------------------
void CNifTreeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
    HTREEITEM hHitItem = GetTreeCtrl().HitTest(point);
    if (hHitItem)
    {
        CPoint ptScreenPt = point;
        ClientToScreen(&ptScreenPt);
        GetTreeCtrl().SelectItem(hHitItem);

        CMenu menuPopup;
        menuPopup.CreatePopupMenu();

        if (GetTreeCtrl().GetParentItem(hHitItem) == m_hSceneRoot &&
            GetTreeCtrl().GetItemText(hHitItem) != "NiNode \"Default Node\"")
        {
            menuPopup.AppendMenu(MF_STRING, ID_TREEITEM_REMOVENIF,
                "&Remove NIF File");
            menuPopup.AppendMenu(MF_SEPARATOR);
        }

        if (GetTreeCtrl().ItemHasChildren(hHitItem))
        {
            menuPopup.AppendMenu(MF_STRING, ID_TREEITEM_EXPANDALL,
                "E&xpand All");
            menuPopup.AppendMenu(MF_STRING, ID_TREEITEM_COLLAPSEALL,
                "&Collapse All");
            menuPopup.AppendMenu(MF_SEPARATOR);
        }

        menuPopup.AppendMenu(MF_STRING, ID_TREEITEM_PROPERTIES,
            "&Properties");


        DWORD kData = (DWORD)GetTreeCtrl().GetItemData(hHitItem);
        NiAVObject* pkObject = (NiAVObject*) UlongToPtr(kData);
        
        if (pkObject->GetAppCulled())
        {
            menuPopup.AppendMenu(MF_STRING, ID_TREEITEM_SHOWHIDE,
                "&Show");
        }
        else
        {
            menuPopup.AppendMenu(MF_STRING, ID_TREEITEM_SHOWHIDE,
                "&Hide");
        }

        menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |
                TPM_RIGHTBUTTON, ptScreenPt.x, ptScreenPt.y,
                this);
    }

    CTreeView::OnRButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifTreeView::OnExpandAll() 
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();
    if (hCurSelItem)
    {
        ExpandRecursive(hCurSelItem, TVE_EXPAND);
        GetTreeCtrl().EnsureVisible(hCurSelItem);
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::OnCollapseAll() 
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();
    if (hCurSelItem)
    {
        ExpandRecursive(hCurSelItem, TVE_COLLAPSE);
        GetTreeCtrl().EnsureVisible(hCurSelItem);
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::OnRemoveNif()
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();
    if (hCurSelItem)
    {
        NiNode* pkRoot = (NiNode*) GetTreeCtrl().GetItemData(hCurSelItem);
        GetDocument()->Lock();
        GetDocument()->RemoveFile(pkRoot);
        GetDocument()->UnLock();
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::ExpandRecursive(HTREEITEM hItem, UINT nCode)
{
    GetTreeCtrl().Expand(hItem, nCode);

    HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hItem);
    while (hChildItem)
    {
        ExpandRecursive(hChildItem, nCode);
        hChildItem = GetTreeCtrl().GetNextSiblingItem(hChildItem);
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::OnProperties() 
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();

    if (hCurSelItem)
    {
        DWORD kData = (DWORD)GetTreeCtrl().GetItemData(hCurSelItem);
        GetTreeCtrl().EnsureVisible(hCurSelItem);
        NiAVObject* pkObject = (NiAVObject*) UlongToPtr(kData);
        CNifPropertyWindowManager* pkManager =  
            CNifPropertyWindowManager::GetPropertyWindowManager();
        pkManager->CreatePropertyWindow(pkObject);
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::OnToggleAppCulled() 
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();

    if (hCurSelItem)
    {
        DWORD kData = (DWORD)GetTreeCtrl().GetItemData(hCurSelItem);
        GetTreeCtrl().EnsureVisible(hCurSelItem);
        NiAVObject* pkObject = (NiAVObject*) UlongToPtr(kData);
        pkObject->SetAppCulled(!pkObject->GetAppCulled());

        // Mirror the app culled flag on the proxy camera.
        if (NiIsKindOf(NiCamera, pkObject))
        {
            NiCamera* pkCam = NiVerifyStaticCast(NiCamera, pkObject);
            CNifDoc* pkDoc = GetDocument();
            if (!pkDoc)
                return;

            pkDoc->Lock();

            CNifCameraList* pkCamList = pkDoc->GetCameraList();
            if (!pkCamList)
            {
                pkDoc->UnLock();
                return;
            }

            NiCamera* pkProxyCam = pkCamList->GetCameraFromOriginal(pkCam);
            if (pkProxyCam)
                pkProxyCam->SetAppCulled(pkObject->GetAppCulled());

            pkDoc->UnLock();
        }
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();
    
    if (hCurSelItem)
    {
        HTREEITEM hChild = GetTreeCtrl().GetChildItem(hCurSelItem);
        if (!hChild)
        {
            NiAVObject* pkObject = (NiAVObject*)
                GetTreeCtrl().GetItemData(hCurSelItem);
            CNifPropertyWindowManager* pkManager =
                CNifPropertyWindowManager::GetPropertyWindowManager();
            CNifPropertyWindow* pkWindow = 
                pkManager->CreatePropertyWindow(pkObject);
            if (pkWindow)
            {
                // For some reason, double-clicking in the listview
                // causes the listview to regain active status
                // after this method ends. In order to bring our 
                // window to the top of the z-order, we need to 
                // post a message to the queue that will be processed
                // later.
                pkWindow->PostMessage(WM_ACTIVATE, 
                    MAKEWPARAM(WA_ACTIVE,0), NULL);
            }
        }
    }

    *pResult = 0;
}
//---------------------------------------------------------------------------
void CNifTreeView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();
    if (hCurSelItem)
    {
        NiAVObject* pkObject = NULL;

        if (hCurSelItem != m_hSceneRoot && GetTreeCtrl().GetItemText(
            hCurSelItem) != "NiNode \"Default Node\"")
        {
            pkObject = (NiAVObject*) GetTreeCtrl().GetItemData(hCurSelItem);
        }

        // Highlight pkObject in render view.
        GetDocument()->HighlightObject(pkObject);
    }
}
//---------------------------------------------------------------------------
bool CNifTreeView::HighlightObject(HTREEITEM hItem, NiAVObject* pkObject)
{
    if (!pkObject)
    {
        return false;
    }

    NiAVObject* pkTreeObject = (NiAVObject*) GetTreeCtrl().GetItemData(hItem);
    if (pkTreeObject == pkObject)
    {
        GetTreeCtrl().SelectItem(hItem);
        GetTreeCtrl().EnsureVisible(hItem);
        return true;
    }

    HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hItem);
    while (hChildItem)
    {
        if (HighlightObject(hChildItem, pkObject))
        {
            return true;
        }
        hChildItem = GetTreeCtrl().GetNextSiblingItem(hChildItem);
    }

    return false;
}
//---------------------------------------------------------------------------
void CNifTreeView::RepopulateTreeCtrl()
{
    NiAVObject* pkObject = NULL;
    HTREEITEM hCurSelItem = GetTreeCtrl().GetSelectedItem();
    if (hCurSelItem)
    {
        pkObject = (NiAVObject*) GetTreeCtrl().GetItemData(hCurSelItem);
    }

    GetTreeCtrl().DeleteAllItems();
    PopulateTreeCtrl();

    if (pkObject)
    {
        HighlightObject(m_hSceneRoot, pkObject);
    }
}
//---------------------------------------------------------------------------
void CNifTreeView::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
    if (GetAsyncKeyState(VK_MENU) & 0x8000)
    {
        OnProperties();
    }

    *pResult = 0;
}
//---------------------------------------------------------------------------
