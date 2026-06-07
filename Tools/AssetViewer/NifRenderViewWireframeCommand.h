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

// NifRenderViewWireframeCommand.h

#ifndef NIFRENDERVIEWWIREFRAMECOMMAND_H
#define NIFRENDERVIEWWIREFRAMECOMMAND_H

#include "NifRenderViewUICommand.h"

class CNifRenderView;

class CNifRenderViewWireframeCommand : public CNifRenderViewUICommand
{    
    public:
        CNifRenderViewWireframeCommand();
        ~CNifRenderViewWireframeCommand();
        bool Execute(CNifRenderView* pkView);
        virtual char* GetIDString() {return "Wireframe";};
        
        static NiWireframeProperty* GetWireframeProperty(NiNode* scene);
    protected:
        NiCameraPtr m_spCamera;
        NiNodePtr m_spScene;
        CNifRenderView* m_pkView;
};

#endif  // #ifndef NIFRENDERVIEWWIREFRAMECOMMAND_H
