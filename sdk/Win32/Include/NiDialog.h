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

#ifndef NIDIALOG_H
#define NIDIALOG_H

#include "NiPluginToolkitDefinitions.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>

class NIPLUGINTOOLKIT_ENTRY NiDialog : public NiRefObject
{
    public:
        NiDialog(WORD wResourceId, NiModuleRef hInstance, 
            NiWindowRef hWndParent);
        virtual ~NiDialog();

        virtual int DoModal();

        void Create();
        void Destroy();

        NiWindowRef GetWindowRef();
    protected:
        NiDialog() {};
        NiDialog(NiDialog&){};
        static BOOL CALLBACK TheDialogProc(NiWindowRef hWnd, UINT message,
            WPARAM wParam,LPARAM lParam);

        virtual void InitDialog();

        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

        virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
            int iWParamHigh, long lParam);

        virtual void OnClose();

        virtual void OnDestroy();

        NiModuleRef m_hInstance;
        NiWindowRef m_hWndParent;
        WORD m_wResourceId;
        NiWindowRef m_hWnd;
        bool m_bIsModalDlg;

};

NiSmartPointer(NiDialog);

#endif
