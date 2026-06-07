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
// Precompiled Header
#include "NiInputPCH.h"

#include "NiAction.h"
#include "NiInputGamePad.h"
#include "NiInputMouse.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiAction::NiAction() :
    m_pcName(0), 
    m_uiAppData(0), 
    m_uiSemantic(0), 
    m_uiFlags(0), 
    m_uiDeviceID(0), 
    m_iRangeLow(0),
    m_iRangeHigh(0), 
    m_pvContext(0), 
    m_uiKeyModifiers(0), 
    m_uiMouseModifiers(0), 
    m_uiPadModifiers(0), 
    m_uiControl(0), 
    m_uiHow(0)
{
}
//---------------------------------------------------------------------------
NiAction::NiAction(const char* pcName, unsigned int uiAppData, 
    unsigned int uiSemantic, unsigned int uiFlags, unsigned int uiDeviceID, 
    int iRangeLow, int iRangeHigh, void* pvContext) :
    m_pcName(0), 
    m_uiAppData(uiAppData), 
    m_uiSemantic(uiSemantic), 
    m_uiFlags(uiFlags), 
    m_uiDeviceID(uiDeviceID), 
    m_iRangeLow(iRangeLow), 
    m_iRangeHigh(iRangeHigh), 
    m_pvContext(pvContext), 
    m_uiKeyModifiers(0), 
    m_uiMouseModifiers(0), 
    m_uiPadModifiers(0), 
    m_uiControl(0), 
    m_uiHow(0)
{
    SetName(pcName);
}
//---------------------------------------------------------------------------
NiAction::~NiAction()
{
    SetName(0);
}
//---------------------------------------------------------------------------
void NiAction::SetName(const char* pcName)
{
    if (m_pcName && pcName && (strcmp(m_pcName, pcName) == 0))
        return;
    
    if (m_pcName)
    {
        NiFree(m_pcName);
        m_pcName = 0;
    }

    if (!pcName)
        return;

    size_t stLen = strlen(pcName) + 1;
    m_pcName = NiAlloc(char, stLen);
    NIASSERT(m_pcName);
    NiStrcpy(m_pcName, stLen, pcName);
}
//---------------------------------------------------------------------------
unsigned int NiAction::SemanticToDeviceControl(unsigned int uiSemantic)
{
    if (IS_GP_MAPPING(uiSemantic))
        return SemanticToDeviceControl_GamePad(uiSemantic);
    else
    if (IS_MOUSE_MAPPING(uiSemantic))
        return SemanticToDeviceControl_Mouse(uiSemantic);
    else
    if (IS_KEY_MAPPING(uiSemantic))
        return SemanticToDeviceControl_Keyboard(uiSemantic);

    return 0xffffffff;
}
//---------------------------------------------------------------------------
unsigned int NiAction::SemanticToDeviceControl_GamePad(
    unsigned int uiSemantic)
{
    switch (uiSemantic)
    {
        //*** GAME PAD MAPPINGS ***//
    case GP_AXIS_LEFT_H:            return NIGP_STICK_LEFT_H;
    case GP_AXIS_LEFT_V:            return NIGP_STICK_LEFT_V;
    case GP_AXIS_RIGHT_H:           return NIGP_STICK_RIGHT_H;
    case GP_AXIS_RIGHT_V:           return NIGP_STICK_RIGHT_V;
/***
    case GP_AXIS_X1_H:
    case GP_AXIS_X1_V:
    case GP_AXIS_X2_H:
    case GP_AXIS_X2_V:
***/
    case GP_BUTTON_LUP:             return NiInputGamePad::NIGP_LUP;
    case GP_BUTTON_LDOWN:           return NiInputGamePad::NIGP_LDOWN;
    case GP_BUTTON_LLEFT:           return NiInputGamePad::NIGP_LLEFT;
    case GP_BUTTON_LRIGHT:          return NiInputGamePad::NIGP_LRIGHT;
    case GP_BUTTON_L1:              return NiInputGamePad::NIGP_L1;
    case GP_BUTTON_L2:              return NiInputGamePad::NIGP_L2;
    case GP_BUTTON_R1:              return NiInputGamePad::NIGP_R1;
    case GP_BUTTON_R2:              return NiInputGamePad::NIGP_R2;
    case GP_BUTTON_RUP:             return NiInputGamePad::NIGP_RUP;
    case GP_BUTTON_RDOWN:           return NiInputGamePad::NIGP_RDOWN;
    case GP_BUTTON_RLEFT:           return NiInputGamePad::NIGP_RLEFT;
    case GP_BUTTON_RRIGHT:          return NiInputGamePad::NIGP_RRIGHT;
    case GP_BUTTON_A:               return NiInputGamePad::NIGP_A;
    case GP_BUTTON_B:               return NiInputGamePad::NIGP_B;
    case GP_BUTTON_START:           return NiInputGamePad::NIGP_START;
    case GP_BUTTON_SELECT:          return NiInputGamePad::NIGP_SELECT;
/***
    case GP_DPAD:
***/
/***
    // Any Axis
    GP_AXIS_ANY_1           = 0xff010000,
    GP_AXIS_ANY_2           = 0xff010001,
    GP_AXIS_ANY_3           = 0xff010002,
    GP_AXIS_ANY_4           = 0xff010003,
    GP_AXIS_ANY_5           = 0xff001004,
    GP_AXIS_ANY_6           = 0xff001005,
    GP_AXIS_ANY_7           = 0xff001006,
    GP_AXIS_ANY_8           = 0xff001007,
    // Any Button
    GP_BUTTON_ANY_BASE      = 0xff010100,
***/
    }

    return 0xffffffff;
}
//---------------------------------------------------------------------------
unsigned int NiAction::SemanticToDeviceControl_Mouse(
    unsigned int uiSemantic)
{
    switch (uiSemantic)
    {
        //*** MOUSE MAPPINGS ***//
/***
    MOUSE_MASK              = 0xfd000000,
***/
    case MOUSE_AXIS_X:              return NiInputMouse::NIM_AXIS_X;
    case MOUSE_AXIS_Y:              return NiInputMouse::NIM_AXIS_Y;
    case MOUSE_AXIS_Z:              return NiInputMouse::NIM_AXIS_Z;
    case MOUSE_BUTTON_LEFT:         return NiInputMouse::NIM_LEFT;
    case MOUSE_BUTTON_RIGHT:        return NiInputMouse::NIM_RIGHT;
    case MOUSE_BUTTON_MIDDLE:       return NiInputMouse::NIM_MIDDLE;
    case MOUSE_BUTTON_X1:           return NiInputMouse::NIM_X1;
    case MOUSE_BUTTON_X2:           return NiInputMouse::NIM_X2;
    case MOUSE_BUTTON_X3:           return NiInputMouse::NIM_X3;
    case MOUSE_BUTTON_X4:           return NiInputMouse::NIM_X4;
    case MOUSE_BUTTON_X5:           return NiInputMouse::NIM_X5;
/***
        // *** Any axis mapping
    MOUSE_AXIS_ANY_1        = 0xfd010001,
    MOUSE_AXIS_ANY_2        = 0xfd010002,
    MOUSE_AXIS_ANY_3        = 0xfd010003,
        // *** Any button mapping
    MOUSE_BUTTON_ANY_BASE   = 0xfd010100
***/
    }

    return 0xffffffff;
}
//---------------------------------------------------------------------------
unsigned int NiAction::SemanticToDeviceControl_Keyboard(
    unsigned int uiSemantic)
{
    if (!IS_KEY_ANY(uiSemantic))
    {
        return (uiSemantic & ~KEY_MASK);
    }
    else
    {
//    KEY_ANY_BASE            = 0xfe010000,
    }

    return 0xffffffff;
}
//---------------------------------------------------------------------------
