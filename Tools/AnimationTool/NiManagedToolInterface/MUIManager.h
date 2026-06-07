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

// NifInputManager.h

#ifndef MUIMANAGER_H
#define MUIMANAGER_H
#include "MCamera.h"
#include "MOrbitPoint.h"
#include "MUIState.h"
#include "MUICommand.h"

namespace NiManagedToolInterface
{
    public __gc class MUIManager
    {
    public:
        MUIManager();
        void DeleteContents();

        __property bool get_InvertPan();
        __property void set_InvertPan(bool bInvert);
        __property MCamera::UpAxisType get_UpAxis();
        __property void set_UpAxis(MCamera::UpAxisType eType);
        __property MOrbitPoint* get_OrbitPoint();
        __property MUIState* get_ActiveUIState();
        __property MUIState::UIType get_ActiveUIType();
        __property void set_ActiveUIType(MUIState::UIType eActiveType);
        __property float get_BackgroundHeight();
        __property void set_BackgroundHeight(float fHeight);

        __event void OnActiveUITypeChanged(MUIState::UIType eActiveType);

        void Update();
        void InitWindowHandle(IntPtr pWindowHandle);
        void Init();
        void RefreshData();
        void Shutdown();
        
        void Lock();
        void Unlock();

        void SubmitCommand(MUICommand::CommandType eType);
        MUICommand* GetCommand(MUICommand::CommandType eType);

        void SetCamera(MCamera* pkMCamera);

    protected:
        MUIState* m_aUIState[];
        MUICommand* m_aCommands[];
        MUIState::UIType m_eActiveUIType;
        unsigned int m_uiLockCount;
        bool m_bInvertPan;
        MCamera::UpAxisType m_eUpAxis;
        float m_fBackgroundHeight;
        MCamera* m_pkMCamera;
        MOrbitPoint* m_pkMOrbitPoint;
    };
}

#endif  // #ifndef NIFTIMEMANAGER_H
