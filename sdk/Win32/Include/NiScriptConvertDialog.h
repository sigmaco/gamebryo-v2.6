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

#ifndef NISCRIPTCONVERTDIALOG_H
#define NISCRIPTCONVERTDIALOG_H

#include "NiDialog.h"
#include <NiString.h>

class NIPLUGINTOOLKIT_ENTRY NiScriptConvertDialog : public NiDialog
{
public:
    NiScriptConvertDialog(NiString strTitle);

    // Method for creating a completely modal dialog
    virtual int DoModal();

    static bool GetModifyScript();
    static bool GetYesToAll();

protected:

    NiString m_strTitle;

    static bool ms_bDoNotModifyScript;
    static bool ms_bYesToAll;

    NiScriptConvertDialog() {};
    NiScriptConvertDialog(NiScriptConvertDialog&){};

    virtual void InitDialog();

    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

    virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
        int iWParamHigh, long lParam);

    virtual void OnClose();

    virtual void OnDestroy();

};

#endif
