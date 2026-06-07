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

#ifndef ASSETANALYZERDIALOG_H
#define ASSETANALYZERDIALOG_H

/// This class implements the options dialog for the SampleProcessPlugin.
class AssetAnalyzerDialog : public NiDialog
{
public:
    /// Construction for the dialog box.
    AssetAnalyzerDialog(NiModuleRef hInstance, NiWindowRef hWndParent,
        NiPluginInfo* pkPluginInfo);
    virtual ~AssetAnalyzerDialog();

    /// Required virtual function overrides from NiDialog.
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

    /// Read out the options from the dialog into the results.
    NiPluginInfoPtr GetResults();

protected:
    /// For processing the OK button.
    void HandleOK();

    void SetDialogCheckBox(char* pcPluginInfo, int iButton, int iField);

    void GetDialogCheckBox(char* pcPluginInfo, int iButton);

    void SetDialogIntField(char* pcPluginInfo, int iField, int iUnits = 1);

    void GetDialogIntField(char* pcPluginInfo, int iField, int iUnits = 1);

    void SetDialogFloatField(char* pcPluginInfo, int iField);

    void GetDialogFloatField(char* pcPluginInfo, int iField);

    void ToggleCheckBoxes(int iWParamLow);

    void AddRequiredObject();
    void RemoveRequiredObject();

    void LoadRequiredObjects();
    void SaveRequiredObjects();

    void LoadTextureSize();
    void SaveTextureSize();

    /// Initial values this dialog was started with.
    NiPluginInfoPtr m_spPluginInfoInitial;

    /// Final options settings from the dialog.
    NiPluginInfoPtr m_spPluginInfoResults;
};

#endif  // #ifndef ASSETANALYZERDIALOG_H
