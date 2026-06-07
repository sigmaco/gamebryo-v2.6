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

#ifndef NICONTROLLEREXTRACTORDIALOG_H
#define NICONTROLLEREXTRACTORDIALOG_H

/// This class implements the options dialog for the controller extractor.
class NiControllerExtractorDialog : public NiDialog
{
public:

    /// Construction for the dialog box.
    NiControllerExtractorDialog(NiModuleRef hInstance, 
        NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo);
    virtual ~NiControllerExtractorDialog();

    /// Required virtual function overrides from NiDialog.
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);
    virtual BOOL OnMessage(unsigned int uiMessage, int iWParamLow, 
        int iWParamHigh, long lParam);

    /// Read out the options from the dialog into the Results
    NiPluginInfoPtr GetResults();
    
    static const char* ms_pcPSResetControllerType;

protected:
    void HandleOK();
    void VerifyCorrectSeparator();
    void EnableFileNameOptions(BOOL bEnabled);
    void ResetExampleFileNames();
    void EnableKFMOptions(BOOL bEnabled);
    
    NiKFMTool::TransitionType GetSyncTransType();
    NiKFMTool::TransitionType GetNonSyncTransType();
    void SetSyncTransType(NiKFMTool::TransitionType eType);
    void SetNonSyncTransType(NiKFMTool::TransitionType eType);
    NiString TransTypeToString(NiKFMTool::TransitionType eType);

    /// Initial values this dialog was started with.
    NiPluginInfoPtr m_spPluginInfoInitial;

    /// Final options settings from the dialog.
    NiPluginInfoPtr m_spPluginInfoResults;
};

#endif  // #ifndef NICONTROLLEREXTRACTORDIALOG_H
