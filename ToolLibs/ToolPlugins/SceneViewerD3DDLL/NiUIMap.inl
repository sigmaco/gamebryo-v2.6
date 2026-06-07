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
//---------------------------------------------------------------------------

inline NiUIMap::~NiUIMap()
{
}
//---------------------------------------------------------------------------
inline void NiUIMap::SetKeyboard(NiSceneKeyboard* pkKeyboard)
{
    m_pkKeyboard = pkKeyboard;
}
//---------------------------------------------------------------------------
inline void NiUIMap::SetMouse(NiSceneMouse* pkMouse)
{
    m_pkMouse = pkMouse;
}
//---------------------------------------------------------------------------
inline void NiUIMap::Initialize()
{
    RegisterCommandSet();
}
//---------------------------------------------------------------------------
inline void NiUIMap::RegisterCommandSet()
{
    NIASSERT(NiSceneCommandRegistry::IsInitialized());
    NiCameraRotateCommand::Register();
    NiCameraTranslateCommand::Register();
    NiCameraTumbleCommand::Register();
    NiCameraResetCommand::Register();
    NiCameraSwitchCommand::Register();
    NiCameraCutToCommand::Register();
    NiCameraZoomExtentsCommand::Register();
    NiScaleDeviceDeltaCommand::Register();
    NiWireframeCommand::Register();
    NiScaleTimeCommand::Register();
    NiAnimationFreezeCommand::Register();
    NiAnimationLoopCommand::Register();
    NiCameraAdjustFrustumCommand::Register();
    NiOverdrawIndicatorCommand::Register();
}
//---------------------------------------------------------------------------
inline void NiUIMap::SetViewportCenter(unsigned int uiX, unsigned int uiY)
{
    m_uiViewportCenterX = uiX;
    m_uiViewportCenterY = uiY;
}
//---------------------------------------------------------------------------
inline NiSceneCommandQueue* NiUIMap::CreateMouseCommands()
{
    return NULL;
}
//---------------------------------------------------------------------------
inline NiSceneCommandQueue* NiUIMap::CreateKeyboardCommands()
{
    return NULL;
}
//---------------------------------------------------------------------------
inline bool NiUIMap::UsesMouse()
{
    return m_pkMouse != NULL;
}
//---------------------------------------------------------------------------
inline bool NiUIMap::UsesKeyboard()
{
    return m_pkKeyboard != NULL;
}
//---------------------------------------------------------------------------
inline void NiUIMap::SendCommand(NiSceneCommand*)
{
}
//---------------------------------------------------------------------------

inline NiSceneCommandQueue* NiUIMap::CreateExternalCommands()
{
    return NULL;
}
//---------------------------------------------------------------------------
