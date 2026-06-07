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

#ifndef NISCENEGRAPHOPTIMIZATIONDIALOG_H
#define NISCENEGRAPHOPTIMIZATIONDIALOG_H

/// This class implements the options dialog for scene graph optimization.
class NiSceneGraphOptimizationDialog : public NiDialog
{
public:
    /// Construction for the dialog box.
    NiSceneGraphOptimizationDialog(NiModuleRef hInstance, 
        NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo);

    ~NiSceneGraphOptimizationDialog();

    /// Required virtual function overrides from NiDialog.
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

    NiPluginInfoPtr GetResults();

protected:

    /// Initialize the skin and bone options based on the current plaform.
    void SetSkinAndBonesOptionsForPlatform();

    /// Reset the dialog back to it's default options
    void ResetDefaultDialogOptions();

    /// Clear the dialog removeing all optimizations
    void UncheckAllDialogOptions();

    /// Read out the options from the dialog into the Results
    void HandleOK();

    /// Initial values this dialog was started with.
    NiPluginInfoPtr m_spPluginInfoInitial;

    /// Final options settings from the dialog.
    NiPluginInfoPtr m_spPluginInfoResults;
};

#endif  // #ifndef NISCENEGRAPHOPTIMIZATIONDIALOG_H
