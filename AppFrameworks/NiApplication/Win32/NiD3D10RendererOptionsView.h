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

#ifndef NID3D10RENDEREROPTIONSVIEW_H
#define NID3D10RENDEREROPTIONSVIEW_H

#include "NiBaseRendererOptionsView.h"
#include "NiD3D10RendererDesc.h"

// Forward declarations
class NiD3D10RendererDesc;

class NiD3D10RendererOptionsView : public NiBaseRendererOptionsView
{
public:
    NiD3D10RendererOptionsView();
    virtual ~NiD3D10RendererOptionsView();

    // Returns name of a renderer
    virtual char* GetName();

    // Dialog creation and controls initialization function
    virtual NiWindowRef InitDialog(NiWindowRef pParentWnd);

    // Height changing functions for basic / advanced views
    virtual unsigned int SetBasicHeight();
    virtual unsigned int SetAdvancedHeight();

protected:
    NiWindowRef m_pD3D10DlgHandle;
    NiD3D10RendererDesc* m_pkD3D10RendererDesc;

    // Returns an instance of NiDX9RendererDesc
    virtual NiBaseRendererDesc* GetRendDesc();

    // Init controls in dialog
    virtual void InitAdapterDependentControls();
    void InitD3D10DialogControls();

    // Functions to react on user input
    bool ChangeDisplay();

private:
    // Message processing functions
    bool ProcessD3D10Command(
        NiWindowRef pDlgHandle,
        WORD wID,
        WORD wNotifyCode);
    static BOOL CALLBACK D3D10RendererOptionsViewWndProc(
        HWND pDlg,
        UINT uiMsg,
        WPARAM wParam,
        LPARAM lParam);
};

#include "NiD3D10RendererOptionsView.inl"

#endif // NID3D10RENDEREROPTIONSVIEW_H