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

#ifndef NIBASERENDEREROPTIONSVIEW_H
#define NIBASERENDEREROPTIONSVIEW_H

#include <NiMemObject.h>
#include "NiBaseRendererDesc.h"

// Forward declarations
class NiBaseRendererDesc;
class NiRendererSettings;

class NiBaseRendererOptionsView : public NiMemObject
{
public:
    NiBaseRendererOptionsView();
    virtual ~NiBaseRendererOptionsView();

    // Dialog creation and controls initialization function
    virtual NiWindowRef InitDialog(NiWindowRef pParentWnd);

    // Returns name of a renderer
    virtual char* GetName() = 0;

    // This functions are called when renderer is selected / deselected
    virtual void Activate();
    virtual void Deactivate();
    virtual bool IsActive();

    // Height changing functions for basic / advanced views
    virtual unsigned int SetBasicHeight();
    virtual unsigned int SetAdvancedHeight();

protected:
    NiWindowRef m_pDlgHandle;
    NiRendererSettings* m_pkRendererSettings;
    bool m_bViewAdvanced;
    unsigned int m_uiBasicHeight;
    unsigned int m_uiAdvancedHeight;

    // Functions, that initialize controls in dialog
    virtual void InitDialogControls();
    virtual void InitAdapterDependentControls();
    virtual void InitDeviceDependentControls();

    // Returns an instance of NiXXXRendererDesc
    virtual NiBaseRendererDesc* GetRendDesc() = 0;

    // Helper function, that fills a combo box from array of strings
    void FillComboboxFromArray(
        NiWindowRef pWnd,
        int iControlId,
        NiTObjectArray<NiFixedString>& kList,
        unsigned int uiCurSel);

    // Functions to react on user input
    bool ChangePerfHUD();
    bool ChangeAdapter();
    bool ChangeDeviceType(bool bRefDevice);
    bool ChangePureDevice();
    bool ChangeVertexprocessing(
        NiRendererSettings::Vertexprocessing eVertex);
    bool ChangeFullScreen();
    bool ChangeStencil();
    bool ChangeScreenSizeFilter(
        NiBaseRendererDesc::ResolutionFilters eSize);
    bool ChangeResolution();
    bool ChangeMultisample();
    bool ChangeRTFormat();
    bool ChangeDSFormat();
    bool ChangeVSync();

private:
    // Message processing functions
    bool ProcessCommand(NiWindowRef pDlgHandle, WORD wID, WORD wNotifyCode);
    static BOOL CALLBACK RendererOptionsViewWndProc(
        HWND pDlg,
        UINT uiMsg,
        WPARAM wParam,
        LPARAM lParam);
};

#include "NiBaseRendererOptionsView.inl"

#endif // NIBASERENDEREROPTIONSVIEW_H