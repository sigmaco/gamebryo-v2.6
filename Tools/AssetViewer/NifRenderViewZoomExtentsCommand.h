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

// NifRenderViewZoomExtentsCommand.h

#ifndef NIFRENDERVIEWZOOMEXTENTSCOMMAND_H
#define NIFRENDERVIEWZOOMEXTENTSCOMMAND_H

#include "NifRenderViewUICommand.h"

class CNifRenderView;

class CNifRenderViewZoomExtentsCommand : public CNifRenderViewUICommand
{    
    public:
        enum ExtentsType { EXTENTS_ALL, EXTENTS_SELECTED};

        CNifRenderViewZoomExtentsCommand(ExtentsType eType = EXTENTS_ALL);
        ~CNifRenderViewZoomExtentsCommand();
        bool Execute(CNifRenderView* pkView);
        virtual char* GetIDString() {return "CameraZoomExtents";};

    protected:
        void CalcCameraTransform();
        void CalcCameraFrustum();
        NiCameraPtr m_spCamera;
        NiNodePtr m_spScene;
        CNifRenderView* m_pkView;
        ExtentsType m_eType;


};

#endif  // #ifndef NIFRENDERVIEWZOOMEXTENTSCOMMAND_H
