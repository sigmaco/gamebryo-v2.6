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

// NiDataStreamDlg.cpp

#include "stdafx.h"
#include "commctrl.h"
#include "AssetViewer.h"
#include "NiDataStreamDlg.h"
#include "NifPropertyWindowManager.h"

#include <NiDataStream.h>
#include <NiFloat16.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// NiDataStreamObject object
//---------------------------------------------------------------------------
NiImplementRTTI(NiDataStreamObject, NiObject);
//---------------------------------------------------------------------------
NiDataStreamObject::NiDataStreamObject()
{
    m_pkMesh = NULL;
    m_kSemantic = NULL;
    m_uiSemanticIndex = 0;
}
//---------------------------------------------------------------------------
// CNiDataStreamDlg dialog
//---------------------------------------------------------------------------
CNiDataStreamDlg::CNiDataStreamDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiDataStreamDlg::IDD, pParent)
    , m_kDataStreamObject(NULL)
{
    //{{AFX_DATA_INIT(CNiDataStreamDlg)
    //}}AFX_DATA_INIT
}
CNiDataStreamDlg::~CNiDataStreamDlg()
{
}
//---------------------------------------------------------------------------
void CNiDataStreamDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiDataStreamDlg)
    DDX_Control(pDX, IDC_NIDATASTREAM_DATA_LIST, m_wndDataList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiDataStreamDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiDataStreamDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiDataStreamDlg message handlers
//---------------------------------------------------------------------------
#define ELEMENT_UPDATE(dataType, compCount) \
    { \
        NiUInt32 uiSubmeshCount = kElementLock.GetSubmeshCount(); \
        NiUInt32 uiItemCount = 0; \
        for (NiUInt32 ui = 0; ui < uiSubmeshCount; ui++) \
        { \
            uiItemCount += kElementLock.count(ui); \
        } \
        m_wndDataList.SetItemCount(uiItemCount); \
        NiUInt32 uiIndex = 0; \
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshCount; uiSubmesh++) \
        { \
            NiTStridedRandomAccessIterator<dataType> kIter = \
                kElementLock.begin<dataType>(uiSubmesh); \
            for (NiUInt32 ui = 0; ui < kElementLock.count(uiSubmesh); ui++) \
            { \
                NiString kValue; \
                kValue.Format("%d-%d: ", uiSubmesh, ui); \
                if (compCount == 1) \
                { \
                    kValue += NiString::From(kIter[ui]); \
                } \
                else \
                { \
                    dataType* pkData = &kIter[ui]; \
                    kValue += "("; \
                    kValue += NiString::From(pkData[0]); \
                    for (NiUInt32 uiComp = 1; uiComp < compCount; uiComp++) \
                    { \
                        kValue += ", "; \
                        kValue += NiString::From(pkData[uiComp]); \
                    } \
                    kValue += ")"; \
                } \
                if (uiIndex < (NiUInt32)m_wndDataList.GetItemCount()) \
                { \
                    m_wndDataList.SetItemText( \
                    uiIndex, 0, (const char*)kValue); \
                } \
                else \
                { \
                    m_wndDataList.InsertItem(uiIndex, (const char*)kValue); \
                } \
                uiIndex++; \
            } \
        } \
    }
//---------------------------------------------------------------------------
bool CNiDataStreamDlg::DoUpdate()
{
    if(!m_kDataStreamObject)
        return false;

    NiDataStreamRef* pkStreamRef = NULL;
    NiDataStreamElement kDataStreamElement;
    if (!m_kDataStreamObject->m_pkMesh->FindStreamRefAndElementBySemantic(
        m_kDataStreamObject->m_kSemantic,
        m_kDataStreamObject->m_uiSemanticIndex,
        NiDataStreamElement::F_UNKNOWN,
        pkStreamRef,
        kDataStreamElement))
    {
        m_wndDataList.DeleteAllItems();
        return false;
    }

    // Make sure it is valid
    if (!pkStreamRef)
    {
        m_wndDataList.DeleteAllItems();
        return false;
    }

    NiDataStreamElementLock kElementLock(
        m_kDataStreamObject->m_pkMesh,
        m_kDataStreamObject->m_kSemantic,
        m_kDataStreamObject->m_uiSemanticIndex,
        NiDataStreamElement::F_UNKNOWN,
        NiDataStream::LOCK_READ);

    if (!kElementLock.IsLocked())
    {
        m_wndDataList.DeleteAllItems();
        return false;
    }

    switch(kDataStreamElement.GetType())
    {
    case NiDataStreamElement::T_FLOAT16:
        ELEMENT_UPDATE(NiFloat16, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_FLOAT32:
        ELEMENT_UPDATE(float, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_INT16:
        ELEMENT_UPDATE(NiInt16, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_INT32:
        ELEMENT_UPDATE(NiInt32, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_INT8:
        ELEMENT_UPDATE(NiInt8, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_UINT16:
        ELEMENT_UPDATE(NiUInt16, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_UINT32:
        ELEMENT_UPDATE(NiUInt32, kDataStreamElement.GetComponentCount());
        break;
    case NiDataStreamElement::T_UINT8:
        ELEMENT_UPDATE(NiUInt8, kDataStreamElement.GetComponentCount());
        break;
    default:
        NIASSERT(0 && "Unknown format type.");
        return false;
    }
    return true;

}
//---------------------------------------------------------------------------
BOOL CNiDataStreamDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    ASSERT(m_kDataStreamObject);

    m_wndDataList.InsertColumn(0, "Data");
    CRect rect;
    m_wndDataList.GetWindowRect(&rect);
    m_wndDataList.SetColumnWidth(0, rect.Width());

    return DoUpdate();  
    // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
void CNiDataStreamDlg::SetNiObject(NiObject* pkObj)
{
    ASSERT(NiIsKindOf(NiDataStreamObject, pkObj));
    m_kDataStreamObject = NiDynamicCast(NiDataStreamObject, pkObj);
}
//---------------------------------------------------------------------------
