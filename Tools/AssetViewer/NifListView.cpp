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

// NifListView.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifListView.h"
#include "NifDoc.h"
#include "NifPropertyWindowManager.h"
#include "ListOptionsDlg.h"

#include <NiMesh.h>
#include <NiMeshHWInstance.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifListView
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifListView, CView)
//---------------------------------------------------------------------------
CNifListView::CNifListView()
{
    m_pkFont = new CFont;
    m_pkFont->CreatePointFont(80, "MS Sans Serif");
    m_bListExists = false;

    m_bDisplayNodes = true;
    m_bDisplayLights = true;
    m_bDisplayCameras = true;
    m_bDisplayGeometry = true;
    m_bDisplayCustom = false;
    m_bDisplayTimeControllers = false;
    m_bDisplayProperties = false;
    m_bDisplayDynamicEffects = false;
    m_bDisplayVisible = true;
    m_bDisplayHidden = true;    
}
//---------------------------------------------------------------------------
CNifListView::~CNifListView()
{
    delete m_pkFont;
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifListView, CView)
    //{{AFX_MSG_MAP(CNifListView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_CBN_SELCHANGE(IDC_LIST_SCENE_GRAPH, OnSelChange)
    ON_CBN_DBLCLK(IDC_LIST_SCENE_GRAPH, OnDblClick)
    ON_BN_CLICKED(IDC_BUTTON_OPTIONS, OnOptionsButtonClicked)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifListView drawing
//---------------------------------------------------------------------------
void CNifListView::OnDraw(CDC* pDC)
{
    // Draw code can go here if needed
}
//---------------------------------------------------------------------------
// CNifListView diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CNifListView::AssertValid() const
{
    CView::AssertValid();
}
//---------------------------------------------------------------------------
void CNifListView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
//---------------------------------------------------------------------------
CNifDoc* CNifListView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNifDoc)));
    return (CNifDoc*)m_pDocument;
}
#endif //_DEBUG
//---------------------------------------------------------------------------
// CNifListView message handlers
//---------------------------------------------------------------------------
int CNifListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_kSceneGraphList.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        CBS_AUTOHSCROLL | CBS_SIMPLE | CBS_SORT | CBS_HASSTRINGS |
        CBS_DISABLENOSCROLL | CBS_NOINTEGRALHEIGHT,
        CRect(0, 0, 100, 100), this, IDC_LIST_SCENE_GRAPH);
    m_kSceneGraphList.SetFont(m_pkFont);
    m_bListExists = true;

    m_kOptionsButton.Create("Options... (Ctrl+F6)", WS_CHILD | WS_VISIBLE,
        CRect(0, 0, 100, 100), this, IDC_BUTTON_OPTIONS);
    m_kOptionsButton.SetFont(m_pkFont);

    return 0;
}
//---------------------------------------------------------------------------
void CNifListView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);

    if (m_bListExists)
    {
        m_kSceneGraphList.MoveWindow(0, 0, cx, cy - 20);
        m_kOptionsButton.MoveWindow(0, cy - 20, cx, 20);
    }
}
//---------------------------------------------------------------------------
void CNifListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    WORD wLoWord = LOWORD(lHint);
    switch (wLoWord)
    {
        case NIF_DESTROYSCENE:
            m_kSceneGraphList.ResetContent();
            break;
        case NIF_CREATESCENE:
            break;
        case NIF_ADDNIF:
        {
            unsigned int uiIndex = (unsigned int) HIWORD(lHint);
            AddNifAtIndex(uiIndex);
            break;
        }
        case NIF_REMOVENIF:
        case NIF_SCENECHANGED:
            RePopulateSceneGraphList();
            break;
        case NIF_SELECTEDOBJECTCHANGED:
        {
            NiAVObject* pkObject = (NiAVObject*) pHint;
            HighlightObject(pkObject);
            break;
        }
        case ID_LIST_OPTIONS:
            OnOptionsButtonClicked();
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
void CNifListView::AddNifAtIndex(unsigned int uiIndex)
{
    GetDocument()->Lock();

    NiNode* pkScene = GetDocument()->GetSceneGraph();
    if (pkScene && uiIndex < pkScene->GetArrayCount())
    {
        unsigned int uiInsertAt = 0;
        for (unsigned int ui = 0; ui < pkScene->GetArrayCount(); ui++)
        {
            if (pkScene->GetAt(ui))
            {
                if (uiInsertAt == uiIndex)
                {
                    break;
                }
                else
                {
                    uiInsertAt++;
                }
            }
        }

        NiNode* pkNewNode = NiDynamicCast(NiNode, pkScene->GetAt(uiInsertAt));
        if (pkNewNode)
        {
            AddToListRecursive(pkNewNode);
        }
    }

    GetDocument()->UnLock();
}
//---------------------------------------------------------------------------
void CNifListView::AddToListRecursive(NiAVObject* pkObject)
{
    CString strName = pkObject->GetName();
    if (strName.IsEmpty())
    {
        strName.Format(" Unnamed %s", pkObject->GetRTTI()->GetName());
        NiNode* pkParent = pkObject->GetParent();
        if (pkParent && pkParent->GetName())
        {
            strName += CString(" (Child of \"") +
                CString(pkParent->GetName()) + "\")";
        }
    }
    else
    {
        strName += CString(" [") + CString(pkObject->GetRTTI()->GetName()) +
            "]";
    }

    bool bCustomOK = false;
    if (m_bDisplayCustom)
    {
        if (m_strCustomString == pkObject->GetRTTI()->GetName())
        {
            bCustomOK = true;
        }
        else
        {
            NiTimeController* pkCtlr = pkObject->GetControllers();
            while (pkCtlr)
            {
                if (m_strCustomString == pkCtlr->GetRTTI()->GetName())
                {
                    bCustomOK = true;
                    break;
                }
                pkCtlr = pkCtlr->GetNext();
            }

            if (!bCustomOK)
            {
                NiPropertyList* pkPropList = &pkObject->GetPropertyList();
                NiTListIterator kIter = pkPropList->GetHeadPos();
                while (kIter)
                {
                    NiProperty* pkProp = pkPropList->GetNext(kIter);
                    if (pkProp &&
                        m_strCustomString == pkProp->GetRTTI()->GetName())
                    {
                        bCustomOK = true;
                        break;
                    }
                }
            }

            if (!bCustomOK && NiIsKindOf(NiNode, pkObject))
            {
                const NiDynamicEffectList* pkEffectList =
                    &((NiNode*) pkObject)->GetEffectList();
                NiTListIterator kIter = pkEffectList->GetHeadPos();
                while (kIter)
                {
                    NiDynamicEffect* pkEffect = pkEffectList->GetNext(kIter);
                    if (pkEffect &&
                        m_strCustomString == pkEffect->GetRTTI()->GetName())
                    {
                        bCustomOK = true;
                        break;
                    }
                }
            }
        }
    }

    if ((m_bDisplayCustom && bCustomOK) || (!m_bDisplayCustom && (
        (m_bDisplayNodes && NiIsKindOf(NiNode, pkObject)) ||
        (m_bDisplayLights && NiIsKindOf(NiLight, pkObject)) ||
        (m_bDisplayCameras && NiIsKindOf(NiCamera, pkObject)) ||
        (m_bDisplayGeometry && NiIsKindOf(NiMesh, pkObject)) ||
        (m_bDisplayGeometry && NiIsKindOf(NiMeshHWInstance, pkObject)))))
    {
        if ((m_bDisplayTimeControllers && !pkObject->GetControllers()) ||
            (m_bDisplayProperties &&
                !pkObject->GetPropertyList().GetSize()) ||
            (m_bDisplayDynamicEffects && !(NiIsKindOf(NiNode, pkObject) &&
                ((NiNode*) pkObject)->GetEffectList().GetSize())))
        {
            // Don't add object.
        }
        else if ((m_bDisplayVisible && !pkObject->GetAppCulled()) ||
            (m_bDisplayHidden && pkObject->GetAppCulled()))
        {
            // Add object to list box.
            int iIndex = m_kSceneGraphList.AddString(strName);
            m_kSceneGraphList.SetItemData(iIndex, (DWORD) PtrToUlong(pkObject));
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                AddToListRecursive(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void CNifListView::RePopulateSceneGraphList()
{
    int iIndex = m_kSceneGraphList.GetCurSel();
    NiAVObject* pkObject = (NiAVObject*) m_kSceneGraphList.GetItemData(
        iIndex);

    GetDocument()->Lock();

    m_kSceneGraphList.ResetContent();

    NiNode* pkScene = GetDocument()->GetSceneGraph();
    if (pkScene)
    {
        for (unsigned int ui = 1; ui < pkScene->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkScene->GetAt(ui);
            if (pkChild)
            {
                AddToListRecursive(pkChild);
            }
        }
    }

    GetDocument()->UnLock();

    HighlightObject(pkObject);
}
//---------------------------------------------------------------------------
void CNifListView::OnSelChange()
{
    int iIndex = m_kSceneGraphList.GetCurSel();
    NiAVObject* pkObject = (NiAVObject*) m_kSceneGraphList.GetItemData(
        iIndex);
    GetDocument()->HighlightObject(pkObject);
}
//---------------------------------------------------------------------------
void CNifListView::HighlightObject(NiAVObject* pkObject)
{
    if (!pkObject)
    {
        return;
    }

    for (int i = 0; i < m_kSceneGraphList.GetCount(); i++)
    {
        NiAVObject* pkListObject = (NiAVObject*)
            m_kSceneGraphList.GetItemData(i);
        if (pkListObject == pkObject)
        {
            m_kSceneGraphList.SetCurSel(i);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void CNifListView::OnDblClick()
{
    int iIndex = m_kSceneGraphList.GetCurSel();
    if (iIndex != CB_ERR)
    {
        NiAVObject* pkObject = (NiAVObject*) m_kSceneGraphList.GetItemData(
            iIndex);
        CNifPropertyWindowManager* pkManager = 
            CNifPropertyWindowManager::GetPropertyWindowManager();
        pkManager->CreatePropertyWindow(pkObject);
    }
}
//---------------------------------------------------------------------------
void CNifListView::OnOptionsButtonClicked()
{
    CListOptionsDlg dlg;
    dlg.m_bNodes = m_bDisplayNodes;
    dlg.m_bLights = m_bDisplayLights;
    dlg.m_bCameras = m_bDisplayCameras;
    dlg.m_bGeometry = m_bDisplayGeometry;
    dlg.m_bCustom = m_bDisplayCustom;
    dlg.m_strCustom = m_strCustomString;
    dlg.m_bTimeControllers = m_bDisplayTimeControllers;
    dlg.m_bProperties = m_bDisplayProperties;
    dlg.m_bDynamicEffects = m_bDisplayDynamicEffects;
    dlg.m_bVisible = m_bDisplayVisible;
    dlg.m_bHidden = m_bDisplayHidden;
    if (dlg.DoModal() == IDOK)
    {
        m_bDisplayNodes = (dlg.m_bNodes ? true : false);
        m_bDisplayLights = (dlg.m_bLights ? true : false);
        m_bDisplayCameras = (dlg.m_bCameras ? true : false);
        m_bDisplayGeometry = (dlg.m_bGeometry ? true : false);
        m_bDisplayCustom = (dlg.m_bCustom ? true : false);
        m_strCustomString = dlg.m_strCustom;
        m_bDisplayTimeControllers = (dlg.m_bTimeControllers ? true : false);
        m_bDisplayProperties = (dlg.m_bProperties ? true : false);
        m_bDisplayDynamicEffects = (dlg.m_bDynamicEffects ? true : false);
        m_bDisplayVisible = (dlg.m_bVisible ? true : false);
        m_bDisplayHidden = (dlg.m_bHidden ? true : false);

        RePopulateSceneGraphList();
    }
}
//---------------------------------------------------------------------------
void CNifListView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (!(nFlags & 0x4000))
    {
        if (nChar == VK_RETURN)
        {
            OnDblClick();
        }
    }
}
//---------------------------------------------------------------------------
BOOL CNifListView::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
        OnKeyDown((UINT)pMsg->wParam, 
            LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
    }

    return CView::PreTranslateMessage(pMsg);
}
//---------------------------------------------------------------------------
