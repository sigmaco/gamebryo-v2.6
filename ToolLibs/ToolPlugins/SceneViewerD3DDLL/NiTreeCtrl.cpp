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
#include "SceneViewerDll.h"

#include "NiTreeCtrl.h"
#include "resource.h"
#include "NiMain.h"

char NiTreeCtrl::ms_acWindowClassName[] = "NiTreeCtrl";
HWND NiTreeCtrl::m_hTreeWnd = NULL;
HWND NiTreeCtrl::m_hTreeView = NULL;
int NiTreeCtrl::ms_aiImageIndex[NiTreeCtrl::TI_NUMIMAGES];
int NiTreeCtrl::ms_aiImageID[NiTreeCtrl::TI_NUMIMAGES] =
{
    IDB_WORLD,
    IDB_CAMERA,
    IDB_LIGHT,
    IDB_NODE,
    IDB_MESH,
    IDB_ATTRIBUTE,
    IDB_ATTRITEM,
    IDB_MESHDATA
};

//---------------------------------------------------------------------------
NiTreeCtrl::NiTreeCtrl(HINSTANCE hInstance, HWND hParentWnd,
    NiAVObject* pkObject, char* pcName, int iXPos, int iYPos, int iWidth,
    int iHeight)
{
    NIASSERT(pkObject && pcName);

    m_hInstance = hInstance;
    m_hParentWnd = hParentWnd;
    m_pkObject = pkObject;
    size_t stLen = strlen(pcName) + 1;
    m_pcName = NiAlloc(char, stLen);
    NiStrcpy(m_pcName, stLen, pcName);

    CreateWindows(iXPos, iYPos, iWidth, iHeight);
    CreateImageList();
    CreateTree();

    ShowWindow(m_hTreeWnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hTreeWnd);
}
//---------------------------------------------------------------------------
NiTreeCtrl::~NiTreeCtrl()
{
    NiFree(m_pcName);
    DestroyTree();
    DestroyImageList();
    DestroyWindows();
}
//---------------------------------------------------------------------------
void NiTreeCtrl::CreateWindows(int iXPos, int iYPos, int iWidth, int iHeight)
{
    // initial tree view classes
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC = ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&icc);

    // register the window class
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = ms_acWindowClassName;
    wc.lpszMenuName = 0;
    RegisterClass(&wc);

    // create the tree view window
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
        ;//| WS_SIZEBOX;
    
    m_hTreeWnd = CreateWindow(ms_acWindowClassName, m_pcName, dwStyle, iXPos,
        iYPos, iWidth, iHeight, 0, 0, m_hInstance, 0);

    // create the tree view
    dwStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD |
        TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;

    RECT rect;
    GetClientRect(m_hTreeWnd, &rect);

    m_hTreeView = CreateWindow(WC_TREEVIEW, "", dwStyle, 0, 0,
        rect.right - rect.left + 1, rect.bottom - rect.top + 1, m_hTreeWnd, 0,
        m_hInstance, 0);
}
//---------------------------------------------------------------------------
void NiTreeCtrl::CreateImageList()
{
    const int iBmpW = 16, iBmpH = 15;

    m_hImageList = ImageList_Create(iBmpW, iBmpH, ILC_COLOR, TI_NUMIMAGES, 1);

    HBITMAP hBitmap;
    for (int i = 0; i < TI_NUMIMAGES; i++)
    {
        hBitmap = (HBITMAP) LoadImage(m_hInstance,
            MAKEINTRESOURCE(ms_aiImageID[i]), IMAGE_BITMAP, iBmpW, iBmpH,
            LR_DEFAULTCOLOR);
        ms_aiImageIndex[i] = ImageList_Add(m_hImageList,hBitmap, 0);
        DeleteObject(hBitmap);
    }
}
//---------------------------------------------------------------------------
void NiTreeCtrl::CreateTree()
{
    TreeView_SetImageList(m_hTreeView, m_hImageList, TVSIL_NORMAL);

    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    // add root item
    tvs.hParent = TVI_ROOT;
    tvs.item.pszText = m_pcName;
    tvs.item.iImage = ms_aiImageIndex[TI_WORLD];
    tvs.item.iSelectedImage = tvs.item.iImage;
    m_hTreeRoot = TreeView_InsertItem(m_hTreeView, &tvs);

    CreateTreeRecursive(m_hTreeRoot, m_pkObject);
}
//---------------------------------------------------------------------------
void NiTreeCtrl::CreateTreeRecursive(HTREEITEM hParent, NiAVObject* pkObject)
{
    // items are to be inserted in-order
    TV_INSERTSTRUCT tvs;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

    // OBJECT
    //     POINTER_VALUE (for debugging)
    //     BASECLASS_0
    //         ATTRIBUTE_ITEMS
    //     :
    //     BASECLASS_N
    //         ATTRIBUTE_ITEMS
    //     CHILD_NODE_0 (if OBJECT is NiNode-derived)
    //     :
    //     CHILD_NODE_M

    // add object
    tvs.hParent = hParent;

    // Format name string: RTTIName "Object Name"
    char *pcRTTIName = (char *) pkObject->GetRTTI()->GetName();
    size_t stNameLength = strlen(pcRTTIName);
    const char *pcObjName = (const char *) pkObject->GetName();
    stNameLength += (pcObjName ? strlen(pcObjName) : 0) + 10;

    char *pcName = NiAlloc(char, stNameLength);
    NiSprintf(pcName, stNameLength, "%s \"%s\"", pcRTTIName, 
        pcObjName ? pcObjName : "");
    
    tvs.item.pszText = pcName;
    if (NiIsKindOf(NiNode, pkObject))
    {
        tvs.item.iImage = ms_aiImageIndex[TI_NODE];
    }
    else if (NiIsKindOf(NiCamera, pkObject))
    {
        tvs.item.iImage = ms_aiImageIndex[TI_CAMERA];
    }
    else if (NiIsKindOf(NiLight, pkObject))
    {
        tvs.item.iImage = ms_aiImageIndex[TI_LIGHT];
    }
    else
    {
        tvs.item.iImage = ms_aiImageIndex[TI_MESH];
    }
    tvs.item.iSelectedImage = tvs.item.iImage;
    tvs.hParent = TreeView_InsertItem(m_hTreeView, &tvs);
    NiFree(pcName);

    // add attributes icon
    tvs.item.pszText = "Attributes";
    tvs.item.iImage = ms_aiImageIndex[TI_ATTRIBUTE];
    tvs.item.iSelectedImage = tvs.item.iImage;
    hParent = tvs.hParent;  // save old parent
    tvs.hParent = TreeView_InsertItem(m_hTreeView, &tvs);

    AddViewerStrings(pkObject, tvs);

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                CreateTreeRecursive(hParent, pkChild);
        }
    }
    else if (NiIsKindOf(NiMesh, pkObject))
    {
        CreateTree(hParent, (NiMesh*) pkObject);
    }
}
//---------------------------------------------------------------------------
void NiTreeCtrl::CreateTree(HTREEITEM hParent, NiMesh* pkData)
{
    TV_INSERTSTRUCT tvs;

    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvs.hParent = hParent;
    tvs.item.pszText = (char *) pkData->GetRTTI()->GetName();
    tvs.item.iImage = ms_aiImageIndex[TI_MESHDATA];
    tvs.item.iSelectedImage = tvs.item.iImage;
    tvs.hParent = TreeView_InsertItem(m_hTreeView, &tvs);
    AddViewerStrings(pkData, tvs);
}
//---------------------------------------------------------------------------
void NiTreeCtrl::AddViewerStrings(NiObject* pkObject, TV_INSERTSTRUCT& tvs)
{
    NiTPrimitiveArray<char*>* pkStrings = NiNew 
        NiTPrimitiveArray<char*>(64,64);
    pkObject->GetViewerStrings(pkStrings);
    unsigned int i;
    for (i = 0; i < pkStrings->GetSize(); i++)
    {
        tvs.item.pszText = pkStrings->GetAt(i);
        tvs.item.iImage = ms_aiImageIndex[TI_ATTRITEM];
        tvs.item.iSelectedImage = tvs.item.iImage;
        TreeView_InsertItem(m_hTreeView, &tvs);
        NiFree(pkStrings->GetAt(i));
    }

    NiDelete pkStrings;
}
//---------------------------------------------------------------------------
void NiTreeCtrl::DestroyWindows()
{
    DestroyWindow(m_hTreeView);
    DestroyWindow(m_hTreeWnd);
}
//---------------------------------------------------------------------------
void NiTreeCtrl::DestroyImageList()
{
    ImageList_Destroy(m_hImageList);
}
//---------------------------------------------------------------------------
void NiTreeCtrl::DestroyTree()
{
    TreeView_DeleteAllItems(m_hTreeView);
}
//---------------------------------------------------------------------------
void NiTreeCtrl::OnIdle()
{
    MSG msg;
    if (PeekMessage(&msg, m_hTreeView, 0, 0, PM_REMOVE))
    {
        if (!TranslateAccelerator(m_hTreeView, 0, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
//---------------------------------------------------------------------------
LRESULT CALLBACK NiTreeCtrl::WindowProcedure(HWND hWnd, UINT uiMsg,
    WPARAM wParam, LPARAM lParam)
{
    /*if (uiMsg == WM_SIZE)
        SendMessage(m_hTreeView,WM_SIZE,wParam, lParam);
    if (uiMsg == WM_SIZING)
        SendMessage(m_hTreeView,WM_SIZING,wParam, lParam);*/
    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
