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

#ifndef NIINFODIALOG_H
#define NIINFODIALOG_H

#include "NiDialog.h"
#include "NiString.h"

class NIPLUGINTOOLKIT_ENTRY NiInfoDialog : public NiDialog
{
    public:
        NiInfoDialog(NiString strTitle);

        // Method for creating a completely modal dialog
        virtual int DoModal();

        void AppendText(const char* pcString, bool bAutoDisplay = false);
        void SetText(const char* pcText, bool bAutoDisplay = false);
        NiString GetText();
        bool IsEmpty();

        void DisplayText();

    protected:
        NiString m_strTitle;
        NiString m_strText;

        NiInfoDialog() {};
        NiInfoDialog(NiInfoDialog&){};

        virtual void InitDialog();

        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

        virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
            int iWParamHigh, long lParam);

        virtual void OnClose();

        virtual void OnDestroy();

};

NiSmartPointer(NiInfoDialog);

#endif
