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

#ifndef NIERRORPROMPTDIALOG_H
#define NIERRORPROMPTDIALOG_H

#include "NiDialog.h"
#include "NiString.h"

class NiErrorPromptDialog : public NiDialog
{
    public:
        NiErrorPromptDialog();

        // Method for creating a completely modal dialog
        virtual int DoModal();

       
    protected:
       
        NiErrorPromptDialog(NiErrorPromptDialog&){};
        bool m_bAskAgain;

        virtual void InitDialog();

        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

        virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
            int iWParamHigh, long lParam);

        virtual void OnClose();

        virtual void OnDestroy();

};

NiSmartPointer(NiErrorPromptDialog);

#endif