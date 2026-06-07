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

#ifndef NISTRIPIFYGEOMETRYDIALOG_H
#define NISTRIPIFYGEOMETRYDIALOG_H

/// This class implements the options dialog for scene graph optimization.
class NiStripifyGeometryDialog : public NiDialog
{
public:

    /// Construction for the dialog box.
    NiStripifyGeometryDialog(NiModuleRef hInstance, 
        NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo);

    ~NiStripifyGeometryDialog();

    /// Required virtual function overrides from NiDialog.
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);
    virtual int DoModal();

    NiPluginInfoPtr GetResults();

protected:

    /// Read out the options from the dialog into the Results
    void HandleOK();

    /// Initial values this dialog was started with.
    NiPluginInfoPtr m_spPluginInfoInitial;

    /// Final options settings from the dialog.
    NiPluginInfoPtr m_spPluginInfoResults;

};

#endif  // #ifndef NISTRIPIFYGEOMETRYDIALOG_H
