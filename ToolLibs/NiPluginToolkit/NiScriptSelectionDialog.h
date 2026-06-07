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
#ifndef NISCRIPTSELECTIONDIALOG_H
#define NISCRIPTSELECTIONDIALOG_H
#include "NiDialog.h"
#include "NiScriptInfo.h"
#include "NiScriptInfoSet.h"

class NIPLUGINTOOLKIT_ENTRY NiScriptSelectionDialog : public NiDialog
{
    public:
        NiScriptSelectionDialog(NiModuleRef hInstance, NiScriptInfo* pkInfo,
            NiScriptInfoSet* pkInfoSet, NiWindowRef hWndParent,
            const char* pcTypes);
        ~NiScriptSelectionDialog();
        int DoModal();

        void AddScriptsToComboBox(int iComboBoxID);
        void PopulatePluginsList(int iListID, NiScriptInfo* pkInfo);

        virtual void InitDialog();
        virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

        NiString MakePluginString(NiPluginInfo* pkInfo);
        NiScriptInfoPtr ms_spInfo;
        NiScriptInfoSetPtr ms_spScriptSet;

        NiString m_strTypes;
        
};

#endif