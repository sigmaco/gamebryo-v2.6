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

#ifndef NIRENDERERTABCONTROLLER_H
#define NIRENDERERTABCONTROLLER_H

#include "NiBaseTabController.h"

// Forward declarations
class NiBaseRendererOptionsView;

class NiRendererTabController: public NiBaseTabController
{
public:
    NiRendererTabController();
    virtual ~NiRendererTabController();

    // Creates and initializes a dialog
    virtual NiWindowRef InitDialog(NiWindowRef pParentWnd);

    // Returns name of a tab
    virtual char* GetCaption();

    // Functions for changing a tab height
    virtual unsigned int SetBasicHeight();
    virtual unsigned int SetAdvancedHeight();

protected:
    NiTPrimitiveArray<NiBaseRendererOptionsView*> m_kViewArray;
    unsigned int m_uiCurrentViewIdx;
    unsigned int m_uiChildDlgOffset;
    NiWindowRef m_pDlgHandle;

    // NiXXXOptionsView's creating / initializing / deleting
    bool CreateOptionsViews();
    bool InitOptionsView(NiBaseRendererOptionsView* pkOV);
    void DeleteOptionsViews();
    void InitDialogControls();

    // Message processing functions
    bool ProcessCommand(NiWindowRef pDlgHandle, WORD wID, WORD wNotifyCode);
    static BOOL CALLBACK RendererTabWndProc(
        HWND pDlg,
        UINT uiMsg,
        WPARAM wParam,
        LPARAM lParam);
};

#include "NiRendererTabController.inl"

#endif // NIRENDERERTABCONTROLLER_H
