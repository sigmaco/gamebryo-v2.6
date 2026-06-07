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

#ifndef NINAVORBITCONTROLLER_H
#define NINAVORBITCONTROLLER_H

#include "NiUserInterfaceLibType.h"
#include "NiNavBaseController.h"
#include "NiUISlot.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiNavOrbitController : public NiNavBaseController
{
public:
    NiNavOrbitController(NiAVObject* pkObject, const char* pcName, 
        float fRadius, NiPoint3 kUpVector = NiPoint3::ZERO);
    virtual ~NiNavOrbitController();

    enum Controls
    {
        RADIUS_DOLLY = 0,
        LATITUDE_ORBIT,
        LONGITUDE_ORBIT,
        PAN_RIGHT_LEFT,
        PAN_UP_DOWN,
        FLY,

        ORBIT_NUM_CONTROLS
    };

    virtual bool SetActive(bool bActive, NavState& kCurState);

    void ResetKeyboardHotkey(Controls eControlId, bool bPositiveDir, 
        NiInputKeyboard::KeyCode eKey, 
        NiInputKeyboard::Modifiers eModifier = NiInputKeyboard::KMOD_NONE);
    void ResetMouseHotkey(Controls eControlId, bool bPositiveDir, 
        NiInputMouse::Button eButton, 
        NiInputMouse::Modifiers eModifier = NiInputMouse::MMOD_NONE);
    void ResetMouseAxis(Controls eControlId, NiInputMouse::Axes eMouseAxis);
    void ResetGamePadHotkey(Controls eControlId, bool bPositiveDir, 
        NiInputGamePad::Button eButton, 
        NiInputGamePad::ButtonMask eModifier = NiInputGamePad::NIGP_MASK_NONE);
    void ResetGamePadAxis(Controls eControlId, 
        NiInputGamePad::DefaultAxis eAxis);
    inline void SetObject(NiAVObject* pkObject);

    inline void SetUpDir(const NiPoint3& kNewUpDir);
    inline void SetControlActive(bool bActive, Controls eControlId);
    inline void SetRadius(float fRadius);

    virtual const char* GetControlName(unsigned int uiControlId) const;

protected:
    static const char* ms_pcElementName[ORBIT_NUM_CONTROLS];
    static const float ms_fRadiusChangeRate;
    static const float ms_fAngleChangeRate;
    static const float ms_fPanChangeRate;
    static const float ms_fMinRadius;

    virtual void MouseInput(unsigned int uiControlId, float fDelta);
    virtual void GamePadInput(unsigned int uiControlId, float fDelta);
    virtual void UpdatePose();

    inline void RadiusPosChange(unsigned char ucButtonState);
    inline void RadiusNegChange(unsigned char ucButtonState);
    inline void LatitudePosChange(unsigned char ucButtonState);
    inline void LatitudeNegChange(unsigned char ucButtonState);
    inline void LongitudePosChange(unsigned char ucButtonState);
    inline void LongitudeNegChange(unsigned char ucButtonState);
    // RL is right/left while UD is up/down
    inline void PanRLPosChange(unsigned char ucButtonState);
    inline void PanRLNegChange(unsigned char ucButtonState);
    inline void PanUDPosChange(unsigned char ucButtonState);
    inline void PanUDNegChange(unsigned char ucButtonState);
    inline void FlyPosChange(unsigned char ucButtonState);
    inline void FlyNegChange(unsigned char ucButtonState);

    //InputBinding m_akBindings[ORBIT_NUM_CONTROLS * 2];
    float m_afDeltaValues[ORBIT_NUM_CONTROLS];
    //bool m_abIsActive[ORBIT_NUM_CONTROLS];
    NiPoint3 m_kCenterPt;
    // If this is NiPoint3::ZERO, there are no constraints on motion
    NiPoint3 m_kUpVector;
    NiAVObjectPtr m_spControlledObject;
    float m_fRadius;
};
NiSmartPointer(NiNavOrbitController);
//---------------------------------------------------------------------------
#include "NiNavOrbitController.inl"
//---------------------------------------------------------------------------

#endif
