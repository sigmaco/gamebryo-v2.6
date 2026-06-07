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

// NifRenderViewOverdrawIndicatorCommand.h

#ifndef NIFRENDERVIEWOVERDRAWINDICATORCOMMAND_H
#define NIFRENDERVIEWOVERDRAWINDICATORCOMMAND_H

#include "NifRenderViewUICommand.h"
#include <NiMeshScreenElements.h>

class CNifRenderView;

class CNifRenderViewOverdrawIndicatorCommand : public CNifRenderViewUICommand
{    
public:
    CNifRenderViewOverdrawIndicatorCommand();
    ~CNifRenderViewOverdrawIndicatorCommand();
    bool Execute(CNifRenderView* pkView);
    virtual char* GetIDString() {return "OverdrawIndicator";};
    static void CleanUp();
    
protected:
    void CreateOverdrawIndicator();
    void SetOverdraw(bool bEnable);
    static NiWindowRef m_kWnd;
    static int m_iMsgID;

    NiStencilProperty* GetStencilProperty(NiNode* pkScene);
    
    // support for overdraw metering
    static NiMeshScreenElementsPtr ms_spBlueLevel;
    static NiMeshScreenElementsPtr ms_spGreenLevel;
    static NiMeshScreenElementsPtr ms_spYellowLevel;
    static NiMeshScreenElementsPtr ms_spRedLevel;
    static NiMeshScreenElementsPtr ms_spWarning;
    static NiStencilPropertyPtr ms_spStencil;

    NiCameraPtr m_spCamera;
    NiNodePtr m_spScene;
    CNifRenderView* m_pkView;
};

#endif  // #ifndef NIFRENDERVIEWOVERDRAWINDICATORCOMMAND_H
