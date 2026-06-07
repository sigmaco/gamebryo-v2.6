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

#ifndef NISCRIPTNEWFROMTEMPLATEDIALOG_H
#define NISCRIPTNEWFROMTEMPLATEDIALOG_H

#include "NiDialog.h"
#include "NiScriptInfo.h"
#include "NiScriptInfoSet.h"

class NIPLUGINTOOLKIT_ENTRY NiScriptNewFromTemplateDialog : public NiDialog
{
    public:
        NiScriptNewFromTemplateDialog(NiModuleRef hInstance, NiWindowRef hWnd, 
            const char* pcName, const char* pcTitle, NiScriptInfoSetPtr pkSet);
        ~NiScriptNewFromTemplateDialog();
        int DoModal();
        virtual void InitDialog();
        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);
        
        char* m_pcName;
        char* m_pcTitle;
        NiScriptInfoSetPtr m_spScriptSet;

        unsigned int m_uiWhichScript;

        NiScriptInfo* GenerateNewScript();
        void AddScriptsToComboBox(int iComboBoxID);
};

#endif
