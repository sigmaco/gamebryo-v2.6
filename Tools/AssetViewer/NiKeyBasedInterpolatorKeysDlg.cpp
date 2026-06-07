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

// NiKeyBasedInterpolatorKeysDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiKeyBasedInterpolatorKeysDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiKeyBasedInterpolatorKeysDlg dialog
//---------------------------------------------------------------------------
CNiKeyBasedInterpolatorKeysDlg::CNiKeyBasedInterpolatorKeysDlg(
    CWnd* pParent /*=NULL*/) :
    CNiObjectDlg(CNiKeyBasedInterpolatorKeysDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiKeyBasedInterpolatorKeysDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_uiIndex = 0;
}
//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorKeysDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiKeyBasedInterpolatorKeysDlg)
    DDX_Control(pDX, IDC_ANIMATION_KEYS_LIST, m_wndAnimationKeysList);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_PREVIOUS_BUTTON, m_wndPrevButton);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_NEXT_BUTTON, m_wndNextButton);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_KEYCONTENT, m_wndContentEdit);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_CHANNELPOSED, m_wndPoseEdit);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_KEYCOUNT, m_wndCountEdit);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_KEYCHANNEL, m_wndChannelEdit);
    DDX_Control(pDX, IDC_NIANIMATIONKEY_KEYTYPE, m_wndTypeEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiKeyBasedInterpolatorKeysDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiKeyBasedInterpolatorKeysDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_ANIMATION_KEYS_LIST, OnDblclkAnimationKeyList)
    ON_BN_CLICKED(IDC_NIANIMATIONKEY_NEXT_BUTTON, OnNextButton)
    ON_BN_CLICKED(IDC_NIANIMATIONKEY_PREVIOUS_BUTTON, OnPrevButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiKeyBasedInterpolatorKeysDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiKeyBasedInterpolatorKeysDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(m_pkObj != NULL);
    ASSERT(NiIsKindOf(NiKeyBasedInterpolator,m_pkObj ));
    
    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorKeysDlg::OnDblclkAnimationKeyList(NMHDR* pNMHDR,
    LRESULT* pResult)
{
}
//---------------------------------------------------------------------------
bool CNiKeyBasedInterpolatorKeysDlg::DoUpdate()
{
    NiKeyBasedInterpolator* pkObj = (NiKeyBasedInterpolator*) m_pkObj;

    if (m_uiIndex == 0)
        m_wndPrevButton.EnableWindow(FALSE);
    else
        m_wndPrevButton.EnableWindow(TRUE);

    if (m_uiIndex == (unsigned int) pkObj->GetKeyChannelCount() - 1 )
        m_wndNextButton.EnableWindow(FALSE);
    else
        m_wndNextButton.EnableWindow(TRUE);


    char acString[1024];
    NiSprintf(acString, 1024, "%d of %d", m_uiIndex + 1, 
        pkObj->GetKeyChannelCount());
    m_wndChannelEdit.SetWindowText(acString);

    m_wndCountEdit.SetWindowText(GetCount(pkObj, m_uiIndex));
    m_wndTypeEdit.SetWindowText(GetType(pkObj, m_uiIndex));
    m_wndContentEdit.SetWindowText(GetContentType(pkObj, m_uiIndex));
    m_wndPoseEdit.SetWindowText(GetPose(pkObj, m_uiIndex));

    FillInList(m_wndAnimationKeysList, pkObj, m_uiIndex);

    return true;
}
//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorKeysDlg::OnPrevButton() 
{
    m_uiIndex--;
    DoUpdate();
}
//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorKeysDlg::OnNextButton() 
{
    m_uiIndex++;
    DoUpdate();
}
//---------------------------------------------------------------------------
unsigned int CNiKeyBasedInterpolatorKeysDlg::GetNumberOfColumns(
    NiKeyBasedInterpolator* pkInterp, unsigned short usIndex)
{
    NiAnimationKey::KeyType eType = pkInterp->GetKeyType(usIndex);
    switch (eType)
    {
        case NiAnimationKey::NOINTERP:
            return 2;
        case NiAnimationKey::LINKEY:
            return 2;
        case NiAnimationKey::BEZKEY: 
            return 2;
        case NiAnimationKey::TCBKEY: 
            return 2;
        case NiAnimationKey::EULERKEY:
            return 2;
        case NiAnimationKey::STEPKEY:
            return 2;
        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
CString CNiKeyBasedInterpolatorKeysDlg::GetContentType(
    NiKeyBasedInterpolator* pkInterp, unsigned short usIndex)
{
    NiAnimationKey::KeyContent eContent = pkInterp->GetKeyContent(usIndex);
    switch (eContent)
    {
        case NiAnimationKey::FLOATKEY:
            return "FLOATKEY";  
        case NiAnimationKey::POSKEY:
            return "POSKEY";    
        case NiAnimationKey::ROTKEY:
            return "ROTKEY";
        case NiAnimationKey::COLORKEY:
            return "COLORKEY";
        case NiAnimationKey::TEXTKEY:
            return "TEXTKEY";
        case NiAnimationKey::BOOLKEY:
            return "BOOLKEY";
        default:
            return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
CString CNiKeyBasedInterpolatorKeysDlg::GetType(
    NiKeyBasedInterpolator* pkInterp, unsigned short usIndex)
{
    NiAnimationKey::KeyType eType = pkInterp->GetKeyType(usIndex);
    switch (eType)
    {
        case NiAnimationKey::NOINTERP:
            return "NOINTERP";
        case NiAnimationKey::LINKEY:
            return "LINKEY";
        case NiAnimationKey::BEZKEY: 
            return "BEZKEY";
        case NiAnimationKey::TCBKEY: 
            return "TCBKEY";
        case NiAnimationKey::EULERKEY:
            return "EULERKEY";
        case NiAnimationKey::STEPKEY:
            return "STEPKEY";
        default:
            return "UNKNOWN";
    }
}
//---------------------------------------------------------------------------
CString CNiKeyBasedInterpolatorKeysDlg::GetCount(
    NiKeyBasedInterpolator* pkInterp, unsigned short usIndex)
{
    char acString[256];
    NiSprintf(acString, 256, "%d", pkInterp->GetKeyCount(usIndex));
    CString temp = acString;
    return temp;
}
//---------------------------------------------------------------------------
CString CNiKeyBasedInterpolatorKeysDlg::GetPose(
    NiKeyBasedInterpolator* pkInterp, unsigned short usIndex)
{
    if (pkInterp->GetChannelPosed(usIndex))
        return "TRUE";
    else
        return "FALSE";
}
//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorKeysDlg::FillInList(CListCtrl& kList, 
    NiKeyBasedInterpolator* pkInterp, unsigned short usIndex)
{
    int nColumnCount = kList.GetHeaderCtrl()->GetItemCount();
    // Delete all of the columns.
    int i;
    for (i=0; i < nColumnCount; i++)
    {
        kList.DeleteColumn(0);
    }

    NiAnimationKey::KeyType eType = pkInterp->GetKeyType(usIndex);
    NiAnimationKey::KeyContent eContent = pkInterp->GetKeyContent(usIndex);
    char acValueColumn[256];
    switch (eContent)
    {
        case NiAnimationKey::FLOATKEY:
        case NiAnimationKey::TEXTKEY:
        case NiAnimationKey::BOOLKEY:
            NiSprintf(acValueColumn, 256, "VALUE");
            break;
        case NiAnimationKey::POSKEY:
            NiSprintf(acValueColumn, 256, "VALUE (X, Y, Z)");
            break;
        case NiAnimationKey::ROTKEY:
            if (eType == NiAnimationKey::EULERKEY)
                NiSprintf(acValueColumn, 256, "VALUE");
            else
                NiSprintf(acValueColumn, 256, "VALUE (X, Y, Z, W)");
            break;
        case NiAnimationKey::COLORKEY:
            NiSprintf(acValueColumn, 256, "VALUE (R, G, B, A)");
            break;
    }

    int iColumns = GetNumberOfColumns(pkInterp, usIndex) + 1;
    kList.InsertColumn(0, "ID");
    kList.InsertColumn(1, "TIME");
    kList.InsertColumn(2, acValueColumn);
    
    CRect rect;
    kList.GetWindowRect(&rect);
    int cx = rect.Width()/iColumns;

    kList.SetColumnWidth(0, 25);
    kList.SetColumnWidth(1, 70);
    switch (eContent)
    {
        case NiAnimationKey::FLOATKEY:
        case NiAnimationKey::TEXTKEY:
        case NiAnimationKey::BOOLKEY:
            cx = 70;
            break;
        case NiAnimationKey::POSKEY:
            cx = 190;
            break;
        case NiAnimationKey::ROTKEY:
        case NiAnimationKey::COLORKEY:
            cx = 250;
            break;
    }

    for (i = 2; i < iColumns; i++)
        kList.SetColumnWidth(i, cx);

    kList.DeleteAllItems();
    int nItem = 0;

    if (pkInterp->GetChannelPosed(usIndex))
    {
        char acIndex[256];
        char acValue[512];
        char acTime[512];
        NiSprintf(acIndex, 256, "0");
        NiSprintf(acTime, 512, "POSED");
        int i = m_wndAnimationKeysList.InsertItem( nItem++, acIndex);
        m_wndAnimationKeysList.SetItemText(i, 1, acTime);
        
        switch (eContent)
        {
            case NiAnimationKey::FLOATKEY:
                {
                    float fValue;
                    if (NiIsKindOf(NiTransformInterpolator, pkInterp))
                    {
                        NiQuatTransform kTransform;
                        pkInterp->Update(0.0f, NULL, kTransform);
                        fValue = kTransform.GetScale();
                    }
                    else
                    {
                        pkInterp->Update(0.0f, NULL, fValue);
                    }
                    NiSprintf(acValue, 512, "%f", fValue);
                }
                break;
            case NiAnimationKey::POSKEY:
                {
                    NiPoint3 kValue;
                    if (NiIsKindOf(NiTransformInterpolator, pkInterp))
                    {
                        NiQuatTransform kTransform;
                        pkInterp->Update(0.0f, NULL, kTransform);
                        kValue = kTransform.GetTranslate();
                    }
                    else
                    {
                        pkInterp->Update(0.0f, NULL, kValue);
                    }

                    NiSprintf(acValue, 512, "(%f, %f, %f)", 
                        kValue.x, kValue.y, kValue.z);
                }
                break;
            case NiAnimationKey::ROTKEY:
                {
                    NiQuaternion kValue;
                    if (NiIsKindOf(NiTransformInterpolator, pkInterp))
                    {
                        NiQuatTransform kTransform;
                        pkInterp->Update(0.0f, NULL, kTransform);
                        kValue = kTransform.GetRotate();
                    }
                    else
                    {
                        pkInterp->Update(0.0f, NULL, kValue);
                    }
                    NiSprintf(acValue, 512, "(%f, %f, %f, %f)", 
                        kValue.GetX(), kValue.GetY(), kValue.GetZ(), 
                        kValue.GetW());
                }
                break;
            case NiAnimationKey::COLORKEY:
                {
                    NiColorA kColor;
                    pkInterp->Update(0.0f, NULL, kColor);
                    NiSprintf(acValue, 512, "(%f, %f, %f, %f)", 
                        kColor.r, kColor.g, kColor.b, kColor.a);
                }
                break;
            case NiAnimationKey::TEXTKEY:
                acValue[0] = '\0';;
                break;
            case NiAnimationKey::BOOLKEY:
                {
                    bool bValue;
                    pkInterp->Update(0.0f, NULL, bValue);
                    
                    if (bValue)
                        NiSprintf(acValue, 512, "true");
                    else
                        NiSprintf(acValue, 512, "false");
                }
                break;
        }
        m_wndAnimationKeysList.SetItemText(i, 2, acValue);
        return;
    }


    // Check for Euler Rotation Keys
    if ((eContent == NiAnimationKey::ROTKEY) && 
        (eType == NiAnimationKey::EULERKEY))
    {
        // Get the Euler Rotation Key, there will be only one because it hold
        // the other rotation keys.
        NiEulerRotKey* pkEKey = 
            (NiEulerRotKey*)pkInterp->GetKeyAt(0, usIndex);

        if (pkEKey)
        {
            // Loop over the X, Y, and Z rotations.
            int iKeyType;
            for (iKeyType = 0; iKeyType < 3; iKeyType++)
            {
                int iCurrentKey;
                for (iCurrentKey = 0; 
                    iCurrentKey < (int)pkEKey->GetNumKeys(iKeyType); 
                    iCurrentKey++)
                {
                    // Create the Index Name
                    char acIndex[256];
                    switch (iKeyType)
                    {
                    case 0:
                        NiSprintf(acIndex, 256, "X = %d", iCurrentKey);
                        break;
                    case 1:
                        NiSprintf(acIndex, 256, "Y = %d", iCurrentKey);
                        break;
                    case 2:
                        NiSprintf(acIndex, 256, "Z = %d", iCurrentKey);
                        break;
                    }

                    // Get the animation key
                    NiAnimationKey* pkKey = NULL;

                    switch (pkEKey->GetType(iKeyType))
                    {
                    case NiAnimationKey::BEZKEY:
                        pkKey =
                            (NiAnimationKey*)(&((NiBezFloatKey*)
                                pkEKey->GetKeys(iKeyType))[iCurrentKey]);
                        break;
                    case NiAnimationKey::LINKEY:
                        pkKey = (NiAnimationKey*)(&((NiLinFloatKey*)
                            pkEKey->GetKeys(iKeyType))[iCurrentKey]);
                        break;
                    default:
                        NIASSERT(0);
                        break;
                    }

                    InsertAnimationKey(nItem++, acIndex, pkKey,
                        NiAnimationKey::FLOATKEY);
                }

                // Create a seperator
                int i = m_wndAnimationKeysList.InsertItem( nItem++, "-----");
                m_wndAnimationKeysList.SetItemText(i, 1, "---------");
                m_wndAnimationKeysList.SetItemText(i, 2, "---------");

            }
        }
    }
    else
    {
        for (unsigned int ui = 0; ui < pkInterp->GetKeyCount(usIndex); ui++)
        {
            NiAnimationKey* pkKey = pkInterp->GetKeyAt(ui, usIndex); 
            if(pkKey)
            {
                char acIndex[256];
                NiSprintf(acIndex, 256, "%d", ui);
                InsertAnimationKey(nItem++, acIndex, pkKey, eContent);
            }
        }
    }
}
//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorKeysDlg::InsertAnimationKey(int iListIndex, 
    char* pcIndexName, NiAnimationKey* pkKey,
    NiAnimationKey::KeyContent eContent)
{
    char acValue[512];
    char acTime[512];
    NiSprintf(acTime, 512, "%f", pkKey->GetTime());
    int i = m_wndAnimationKeysList.InsertItem( iListIndex++, pcIndexName);
    m_wndAnimationKeysList.SetItemText(i, 1, acTime);

    switch (eContent)
    {
    case NiAnimationKey::FLOATKEY:
        NiSprintf(acValue, 512, "%f", ((NiFloatKey*)pkKey)->GetValue());
        break;
    case NiAnimationKey::POSKEY:
        NiSprintf(acValue, 512, "(%f, %f, %f)",
            ((NiPosKey*)pkKey)->GetPos().x,
            ((NiPosKey*)pkKey)->GetPos().y,
            ((NiPosKey*)pkKey)->GetPos().z);
        break;
    case NiAnimationKey::ROTKEY:
        NiSprintf(acValue, 512, "(%f, %f, %f, %f)", 
            ((NiRotKey*)pkKey)->GetQuaternion().GetX(), 
            ((NiRotKey*)pkKey)->GetQuaternion().GetY(), 
            ((NiRotKey*)pkKey)->GetQuaternion().GetZ(),
            ((NiRotKey*)pkKey)->GetQuaternion().GetW());
        break;
    case NiAnimationKey::COLORKEY:
        NiSprintf(acValue, 512, "(%f, %f, %f, %f)", 
            ((NiColorKey*)pkKey)->GetColor().r, 
            ((NiColorKey*)pkKey)->GetColor().g, 
            ((NiColorKey*)pkKey)->GetColor().b,
            ((NiColorKey*)pkKey)->GetColor().a);
        break;
    case NiAnimationKey::TEXTKEY:
        NiSprintf(acValue, 512, "");
        break;
    case NiAnimationKey::BOOLKEY:
        if (((NiBoolKey*)pkKey)->GetBool())
            NiSprintf(acValue, 512, "true");
        else
            NiSprintf(acValue, 512, "false");
        break;
    }

    m_wndAnimationKeysList.SetItemText(i, 2, acValue);
}
//---------------------------------------------------------------------------
