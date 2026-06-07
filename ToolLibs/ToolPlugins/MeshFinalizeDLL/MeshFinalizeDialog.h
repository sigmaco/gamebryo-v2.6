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

#ifndef MESHFINALIZEDIALOG_H
#define MESHFINALIZEDIALOG_H

#include "MeshFinalize_resource.h"

// These keys are defined in this header file as well as in the
// NiWiiMeshFinalize tool plug-in. Any changes must be reflected
// in both places.
#define WII_INDEX_STREAM_REMOVAL_KEY "WiiIndexRemoval"
#define WII_CONVERT_SKINNING_STREAMS_KEY "WiiConvertSkinning"
#define WII_CREATE_DISPLAY_LIST_STREAM_KEY "WiiDisplayListCreation"

/// This class implements the options dialog for scene graph optimization.
class MeshFinalizeDialog : public NiDialog
{
public:

    /// Construction for the dialog box.
    MeshFinalizeDialog(WORD wResourceId, NiModuleRef hInstance, 
        NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo);

    ~MeshFinalizeDialog();

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

#endif  // #ifndef MESHFINALIZE_H
