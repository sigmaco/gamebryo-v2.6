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

#ifndef NITURRET_H
#define NITURRET_H

//---------------------------------------------------------------------------
// This class is designed to support moving an object where translation
// and rotation are not necessarily constrained to the same node.  In
// particular, the class supports a three-object system
//   TRANSLATION_NODE -> ROTATION_NODE -> CAMERA
// that allows the camera to be moved so that the translation node controls
// translation in x, y, z, and rotation about an "up" vector.  The rotation
// node controls "look-up/look-down", a rotation about the "right" vector.
// For example, the following code sets up a turret where
//   * the up/down arrow keys control translation in the forward/backward
//       direction, 
//   * the keys 'Z' and 'A' control vertical translation, 
//   * the keys 'X' and 'S' control horizontal translation (sliding), 
//   * the left/right arrow keys control turning about the up axis, and
//   * the PageUp/PageDown keys control looking up/down.
// The rotation about the direction vector (roll) is not enabled in this
// code sample.  The first parameter to SetTrn and SetRot indicates the
// axis that the keys control.  Index 0 refers to the forward/backward
// direction, index 1 refers to the up/down direction, and index 2 refers
// to the left/right direction.
//
// Code is typically in Initialize():
//
//   NiTurret turret;
//   NiCameraPtr spCamera;
//   NiPoint3 initialCamTrn = <whatever>;
//   NiMatrix3 initialCamRot = <whatever>;
//   float fDirTrnSpeed = <whatever>;
//   float fUpTrnSpeed = <whatever>;
//   float fRightTrnSpeed = <whatever>;
//   float fTurnRotSpeed = <whatever>;
//   float fLookRotSpeed = <whatever>;
//
//   NiNodePtr spTrnNode = new NiNode;
//   NiNodePtr spRotNode = new NiNode;
//   spTrnNode->SetTranslate(initialCamTrn);
//   spRotNode->SetRotate(initialCamRot);
//   turret.SetAxes(initialCamRot);
//   turret.SetStandardTrn(spTrnNode);
//   turret.SetTrnSpeed(0, fDirTrnSpeed);
//   turret.SetTrnSpeed(1, fUpTrnSpeed);
//   turret.SetTrnSpeed(2, fRightTrnSpeed);
//   turret.SetStandardRot(spTrnNode, spRotNode);
//   turret.SetRotSpeed(1, fTurnRotSpeed);
//   turret.SetRotSpeed(2, fLookRotSpeed);
//
//   spRotNode->AttachChild(spCamera);
//   spTrnNode->AttachChild(spRotNode);
//   spTrnNode->Update();
//
// In the OnIdle() command the turret is handled by
//
//       if (turret.Read())
//       {
//           spTrnNode->Update();
//           <application does whatever here>;
//       }
//
// In Terminate(), the turret is handled by
//
//   spTrnNode = 0;
//
// and the reference counts for spRotNode and spCamera are automatically
// adjusted by the recursive traversal of the three-object hierarchy rooted
// at spTrnNode.
//---------------------------------------------------------------------------

#include <NiMain.h>
#include <NiInputKeyboard.h>
#include <NiInputMouse.h>
#include <NiInputGamePad.h>

class NiTurret
{
public:
    enum TurretInput
    {
        TUR_DEFAULT, 
        TUR_KEYBOARD, 
        TUR_MOUSE, 
        TUR_GAMEPAD
    };
    
    NiTurret (TurretInput eInputDevice = TUR_DEFAULT);

    bool Read();

    void SetAxes(const NiMatrix3& kRot);

    void SetStandardTrn(float fSpeed, NiAVObject* pkObject);
    void SetStandardRot(float fSpeed, NiAVObject* pkTrnObject, 
        NiAVObject* pkRotObject);

    void SetTrnButtonsKB(int iIndex, NiInputKeyboard::KeyCode eKeyM, 
        NiInputKeyboard::KeyCode eKeyP);
    void SetTrnButtonsM(int iIndex, NiInputMouse::Button eButM, 
        NiInputMouse::Button eButP);
    void SetTrnButtonsGP(int iIndex, NiInputGamePad::Button eButM, 
        NiInputGamePad::Button eButP);
    void SetTrnButtonsStickDirGP(int iIndex, NiInputGamePad::Sticks eStick, 
        NiInputGamePad::Sticks eDir);
    void SetRotButtonsKB(int iIndex, NiInputKeyboard::KeyCode eKeyM, 
        NiInputKeyboard::KeyCode eKeyP);
    void SetRotButtonsM(int iIndex, NiInputMouse::Button eButM, 
        NiInputMouse::Button eButP);
    void SetRotButtonsGP(int iIndex, NiInputGamePad::Button eButM, 
        NiInputGamePad::Button eButP);
    void SetRotButtonsStickDirGP(int iIndex, NiInputGamePad::Sticks eStick, 
        NiInputGamePad::Sticks eDir);

    //  Modifiers
    void SetTrnModifiers(int iIndex, long lModifiers);
    void SetRotModifiers(int iIndex, long lModifiers);
        
    void SetTrnObject(NiAVObject* pkObject);
    void SetRotObject(NiAVObject* pkObject);
    void SetTrnObject(int iIndex, NiAVObject* pkObject);
    void SetRotObject(int iIndex, NiAVObject* pkObject);
    
    void SetRotUseUpdateAxes(bool bUseAxis, bool bUpdateAxis);
    void SetRotUseUpdateAxes(int iIndex, bool bUseAxis, bool bUpdateAxis);

    void SetTrnSpeed(int iIndex, float fSpeed);
    float GetTrnSpeed(int iIndex) const;
    void SetRotSpeed(int iIndex, float fSpeed);
    float GetRotSpeed(int iIndex) const;

    TurretInput GetInputDevice(void);
    void SetInputDevice(TurretInput eInputDevice);

protected:
    bool ReadTrn(int i);
    bool ReadTrnAnalog(int i, long lModifiers, float& fSpeed);
    bool ReadRot(int i);
    bool ReadRotAnalog(int i, long lModifiers, float& fSpeed);

    bool ReadButton(int iKey, long* plModifiers = NULL);

    TurretInput m_eInputDevice;
    
    NiPoint3 m_akAxis[3];
    bool m_abUseAxis[3];
    bool m_abUpdateAxis[3];

    unsigned int m_auiTrnKeyM[3];
    unsigned int m_auiTrnKeyP[3];
    unsigned int m_auiTrnKeyModifiers[3];
    bool m_abTrnAnalogControl[3];    
    unsigned char m_aucTrnAnalogLast[3];
    float m_afTrnSpeed[3];
    NiAVObjectPtr m_aspTrnObject[3];

    unsigned int m_auiRotKeyM[3];
    unsigned int m_auiRotKeyP[3];
    unsigned int m_auiRotKeyModifiers[3];
    bool m_abRotAnalogControl[3];    
    unsigned char m_aucRotAnalogLast[3];
    float m_afRotSpeed[3];
    NiAVObjectPtr m_aspRotObject[3];
};

#endif // NITURRET.H
