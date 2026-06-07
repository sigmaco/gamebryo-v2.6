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

#ifndef NIUIMAP_H
#define NIUIMAP_H

#include "NiSceneKeyboard.h"
#include "NiSceneMouse.h"
#include "NiSceneCommand.h"
#include "NiCameraRotateCommand.h"
#include "NiCameraTranslateCommand.h"
#include "NiCameraTumbleCommand.h"
#include "NiCameraResetCommand.h"
#include "NiCameraSwitchCommand.h"
#include "NiCameraCutToCommand.h"
#include "NiCameraZoomExtentsCommand.h"
#include "NiScaleDeviceDeltaCommand.h"
#include "NiWireframeCommand.h"
#include "NiScaleTimeCommand.h"
#include "NiAnimationFreezeCommand.h"
#include "NiAnimationLoopCommand.h"
#include "NiCameraAdjustFrustumCommand.h"
#include "NiOverdrawIndicatorCommand.h"
//#include <NiMain.h>
//#include "NiSceneCommandClasses.h"

typedef NiTQueue<NiSceneCommand*, NiTMallocInterface<NiSceneCommand*> >
    NiSceneCommandQueue;

class NiUIMap : public NiRefObject
{
public:
    virtual ~NiUIMap();
    virtual void SetKeyboard(NiSceneKeyboard* pkKeyboard);
    virtual void SetMouse(NiSceneMouse* pkMouse);

    virtual void SendCommand(NiSceneCommand* pkCommand);

    virtual bool UsesMouse();
    virtual bool UsesKeyboard();
    inline void SetViewportCenter(unsigned int uiX, unsigned int uiY);

    virtual void Initialize();
    virtual void RegisterCommandSet();

    virtual NiSceneCommandQueue* CreateMouseCommands();
    virtual NiSceneCommandQueue* CreateKeyboardCommands();
    virtual NiSceneCommandQueue* CreateExternalCommands();

protected:
    NiSceneKeyboard* m_pkKeyboard;
    NiSceneMouse* m_pkMouse;
    int m_iLastXPos;
    int m_iLastYPos;
    unsigned int m_uiViewportCenterX;
    unsigned int m_uiViewportCenterY;
};
NiSmartPointer(NiUIMap);
#include "NiUIMap.inl"
#endif
