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

// NifRenderViewResetFrustumCommand.h

#ifndef NIFRENDERVIEWRESETFRUSTUMCOMMAND_H
#define NIFRENDERVIEWRESETFRUSTUMCOMMAND_H

#include "NifRenderViewUICommand.h"

class CNifRenderView;

class CNifRenderViewResetFrustumCommand : public CNifRenderViewUICommand
{    
    public:
        CNifRenderViewResetFrustumCommand();
        ~CNifRenderViewResetFrustumCommand();
        bool Execute(CNifRenderView* pkView);
        virtual char* GetIDString() {return "CameraResetFrustum";};

    protected:
        void CalcCameraFrustum();
        NiCameraPtr m_spCamera;
        NiNodePtr m_spScene;
        CNifRenderView* m_pkView;
        float m_fNearDist;
        float m_fFarDist;
        bool m_bIsDefaultCamera;
};

#endif  // #ifndef NIFRENDERVIEWRESETFRUSTUMCOMMAND_H
