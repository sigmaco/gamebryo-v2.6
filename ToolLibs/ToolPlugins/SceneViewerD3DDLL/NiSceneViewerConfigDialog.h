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
#ifndef NISCENEVIEWERCONFIGDIALOG_H
#define NISCENEVIEWERCONFIGDIALOG_H

#include <NiPluginToolkit.h>
#include <NiDialog.h>
#include <NiScriptInfo.h>
#include <NiScriptInfoSet.h>
#include "SceneViewerDLL.h"

/// This class implements the options dialog for scene graph optimization.
class NiSceneViewerConfigDialog : public NiDialog
{
public:

    /// Construction for the dialog box.
    NiSceneViewerConfigDialog(NiModuleRef hInstance, 
        NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo,
        NiString strPluginDataPath, CSceneViewerDll* pkSceneViewerDLL);

    ~NiSceneViewerConfigDialog();

    /// Required virtual function overrides from NiDialog.
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);
    virtual int DoModal();

    NiPluginInfoPtr GetResults();

protected:
    void AddFile(HWND handle, char* pcFilename);

    /// Read out the options from the dialog into the Results
    void HandleOK();

    bool m_bInit;
    int m_iUISelection;
    NiString m_strPluginDataPath;

    /// Initial values this dialog was started with.
    NiPluginInfoPtr m_spPluginInfoInitial;

    /// Final options settings from the dialog.
    NiPluginInfoPtr m_spPluginInfoResults;

    NiTObjectArray<NiString> m_kUIMArray;
    NiString m_strCustomUIMapFilename;
    CSceneViewerDll* m_pkSceneViewerDLL;


};

#endif //NISCENEVIEWERCONFIGDIALOG_H