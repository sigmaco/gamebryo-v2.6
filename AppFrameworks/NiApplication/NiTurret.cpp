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

// Precompiled Header
#include "NiApplicationPCH.h"

#include "NiTurret.h"
#include "NiApplication.h"
#include "NiSystem.h"

//---------------------------------------------------------------------------
NiTurret::NiTurret(TurretInput eInputDevice)
{
    if (eInputDevice == TUR_DEFAULT)
    {
#if defined(WIN32) && !defined(_XENON)
        m_eInputDevice = TUR_KEYBOARD;
#else //#if defined(WIN32) && !defined(_XENON)
        m_eInputDevice = TUR_GAMEPAD;
#endif //#if defined(WIN32) && !defined(_XENON)
    }
    else
    {
        m_eInputDevice = eInputDevice;
    }

    for (int i = 0; i < 3; i++)
    {
        m_auiTrnKeyM[i] = 0;
        m_auiTrnKeyP[i] = 0;
        m_auiTrnKeyModifiers[i] = 0;
        m_abTrnAnalogControl[i] = false;
        m_aucTrnAnalogLast[i] = 0x00;
        m_afTrnSpeed[i] = 0.0f;
        m_aspTrnObject[i] = 0;
        m_auiRotKeyM[i] = 0;
        m_auiRotKeyP[i] = 0;
        m_auiRotKeyModifiers[i] = 0;
        m_abRotAnalogControl[i] = false;
        m_aucRotAnalogLast[i] = 0x00;
        m_afRotSpeed[i] = 0.0f;
        m_aspRotObject[i] = 0;
        m_abUseAxis[i] = false;
        m_abUpdateAxis[i] = false;
    }

    m_akAxis[0] = NiPoint3::UNIT_X;
    m_akAxis[1] = NiPoint3::UNIT_Y;
    m_akAxis[2] = NiPoint3::UNIT_Z;
}
//---------------------------------------------------------------------------
void NiTurret::SetAxes(const NiMatrix3& kRot)
{
    kRot.GetCol(0, m_akAxis[0]);
    kRot.GetCol(1, m_akAxis[1]);
    kRot.GetCol(2, m_akAxis[2]);
}
//---------------------------------------------------------------------------
void NiTurret::SetStandardTrn(float fSpeed, NiAVObject* pkObject)
{
    switch (m_eInputDevice)
    {
    case TUR_KEYBOARD:
        m_auiTrnKeyM[0] = NiInputKeyboard::KEY_DOWN;
        m_auiTrnKeyP[0] = NiInputKeyboard::KEY_UP;
        m_auiTrnKeyModifiers[0] = 0;
        m_abTrnAnalogControl[0] = false;
        m_auiTrnKeyM[1] = NiInputKeyboard::KEY_S;
        m_auiTrnKeyP[1] = NiInputKeyboard::KEY_W;
        m_auiTrnKeyModifiers[1] = 0;
        m_abTrnAnalogControl[1] = false;
        m_auiTrnKeyM[2] = NiInputKeyboard::KEY_A;
        m_auiTrnKeyP[2] = NiInputKeyboard::KEY_D;
        m_auiTrnKeyModifiers[2] = 0;
        m_abTrnAnalogControl[2] = false;
        break;
    case TUR_MOUSE:
        m_auiTrnKeyM[0] = NiInputMouse::NIM_LEFT;
        m_auiTrnKeyP[0] = NiInputMouse::NIM_RIGHT;
        m_auiTrnKeyModifiers[0] = 0;
        m_abTrnAnalogControl[0] = false;
        m_auiTrnKeyM[1] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiTrnKeyP[1] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiTrnKeyModifiers[1] = 0;
        m_abTrnAnalogControl[1] = false;
        m_auiTrnKeyM[2] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiTrnKeyP[2] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiTrnKeyModifiers[2] = 0;
        m_abTrnAnalogControl[2] = false;
        break;
    case TUR_GAMEPAD:
        m_auiTrnKeyM[0] = NiInputGamePad::NIGP_STICK_RIGHT;
        m_auiTrnKeyP[0] = NiInputGamePad::NIGP_STICK_AXIS_V;
        m_auiTrnKeyModifiers[0] = 0;
        m_abTrnAnalogControl[0] = true;
        m_auiTrnKeyM[1] = NiInputGamePad::NIGP_STICK_LEFT;
        m_auiTrnKeyP[1] = NiInputGamePad::NIGP_STICK_AXIS_V;
        m_auiTrnKeyModifiers[1] = 0;
        m_abTrnAnalogControl[1] = true;
        m_auiTrnKeyM[2] = NiInputGamePad::NIGP_STICK_LEFT;
        m_auiTrnKeyP[2] = NiInputGamePad::NIGP_STICK_AXIS_H;
        m_auiTrnKeyModifiers[2] = 0;
        m_abTrnAnalogControl[2] = true;
        break;

    default:
        break;
    }

    m_afTrnSpeed[0] = fSpeed;
    m_aspTrnObject[0] = pkObject;
    m_afTrnSpeed[1] = fSpeed;
    m_aspTrnObject[1] = pkObject;
    m_afTrnSpeed[2] = fSpeed;
    m_aspTrnObject[2] = pkObject;
}
//---------------------------------------------------------------------------
void NiTurret::SetStandardRot(float fSpeed, NiAVObject* pkTrnObject, 
    NiAVObject* pkRotObject)
{
    switch(m_eInputDevice)
    {
    case TUR_KEYBOARD:
        m_auiRotKeyM[0] = NiInputKeyboard::KEY_NOKEY;
        m_auiRotKeyP[0] = NiInputKeyboard::KEY_NOKEY;
        m_auiRotKeyModifiers[0] = 0;
        m_abRotAnalogControl[0] = false;
        m_auiRotKeyM[1] = NiInputKeyboard::KEY_LEFT;
        m_auiRotKeyP[1] = NiInputKeyboard::KEY_RIGHT;
        m_auiRotKeyModifiers[1] = 0;
        m_abRotAnalogControl[1] = false;
        m_auiRotKeyM[2] = NiInputKeyboard::KEY_PRIOR;
        m_auiRotKeyP[2] = NiInputKeyboard::KEY_NEXT;
        m_auiRotKeyModifiers[2] = 0;
        m_abRotAnalogControl[2] = false;
        break;
    case TUR_MOUSE:
        m_auiRotKeyM[0] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiRotKeyP[0] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiRotKeyModifiers[0] = 0;
        m_abRotAnalogControl[0] = false;
        m_auiRotKeyM[1] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiRotKeyP[1] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiRotKeyModifiers[1] = 0;
        m_abRotAnalogControl[1] = false;
        m_auiRotKeyM[2] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiRotKeyP[2] = (unsigned int)NiInputMouse::NIM_NONE;
        m_auiRotKeyModifiers[2] = 0;
        m_abRotAnalogControl[2] = false;
        break;
    case TUR_GAMEPAD:
        m_auiRotKeyM[0] = NiInputGamePad::NIGP_STICK_RIGHT;
        m_auiRotKeyP[0] = NiInputGamePad::NIGP_STICK_AXIS_V;
        m_auiRotKeyModifiers[0] = NiInputGamePad::NIGP_MASK_R2;
        m_abRotAnalogControl[0] = true;
        m_auiRotKeyM[1] = NiInputGamePad::NIGP_STICK_LEFT;
        m_auiRotKeyP[1] = NiInputGamePad::NIGP_STICK_AXIS_H;
        m_auiRotKeyModifiers[1] = NiInputGamePad::NIGP_MASK_R2;
        m_abRotAnalogControl[1] = true;
        m_auiRotKeyM[2] = NiInputGamePad::NIGP_STICK_LEFT;
        m_auiRotKeyP[2] = NiInputGamePad::NIGP_STICK_AXIS_V;
        m_auiRotKeyModifiers[2] = NiInputGamePad::NIGP_MASK_R2;
        m_abRotAnalogControl[2] = true;
        break;

    default:
        break;
    }

    m_afRotSpeed[0] = fSpeed;
    m_abUseAxis[0] = false;
    m_abUpdateAxis[0] = false;
    m_aspRotObject[0] = NULL;
    m_afRotSpeed[1] = fSpeed;
    m_abUseAxis[1] = true;
    m_abUpdateAxis[1] = true;
    m_aspRotObject[1] = pkTrnObject;
    m_afRotSpeed[2] = fSpeed;
    m_abUseAxis[2] = false;
    m_abUpdateAxis[2] = false;
    m_aspRotObject[2] = pkRotObject;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnSpeed(int iIndex, float fSpeed)
{
    m_afTrnSpeed[iIndex] = fSpeed;
}
//---------------------------------------------------------------------------
float NiTurret::GetTrnSpeed(int iIndex) const
{
    return m_afTrnSpeed[iIndex];
}
//---------------------------------------------------------------------------
void NiTurret::SetRotSpeed(int iIndex, float fSpeed)
{
    m_afRotSpeed[iIndex] = fSpeed;
}
//---------------------------------------------------------------------------
float NiTurret::GetRotSpeed(int iIndex) const
{
    return m_afRotSpeed[iIndex];
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnButtonsKB(int iIndex, NiInputKeyboard::KeyCode eKeyM, 
    NiInputKeyboard::KeyCode eKeyP)
{
    NIASSERT(m_eInputDevice == TUR_KEYBOARD);
    m_auiTrnKeyM[iIndex] = eKeyM;
    m_auiTrnKeyP[iIndex] = eKeyP;
    m_abTrnAnalogControl[iIndex] = false;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnButtonsM(int iIndex, NiInputMouse::Button eButM, 
    NiInputMouse::Button eButP)
{
    NIASSERT(m_eInputDevice == TUR_MOUSE);
    m_auiTrnKeyM[iIndex] = eButM;
    m_auiTrnKeyP[iIndex] = eButP;
    m_abTrnAnalogControl[iIndex] = false;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnButtonsGP(int iIndex, NiInputGamePad::Button eButM, 
    NiInputGamePad::Button eButP)
{
    NIASSERT(m_eInputDevice == TUR_GAMEPAD);
    m_auiTrnKeyM[iIndex] = eButM;
    m_auiTrnKeyP[iIndex] = eButP;
    m_abTrnAnalogControl[iIndex] = false;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnButtonsStickDirGP(int iIndex, 
    NiInputGamePad::Sticks eStick, NiInputGamePad::Sticks eDir)
{
    NIASSERT(m_eInputDevice == TUR_GAMEPAD);
    
    //  The Minus value indicates the stick, the Plus the direction
    m_auiTrnKeyM[iIndex] = eStick;
    m_auiTrnKeyP[iIndex] = eDir;
    m_abTrnAnalogControl[iIndex] = true;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotButtonsKB(int iIndex, NiInputKeyboard::KeyCode eKeyM, 
    NiInputKeyboard::KeyCode eKeyP)
{
    NIASSERT(m_eInputDevice == TUR_KEYBOARD);
    m_auiRotKeyM[iIndex] = eKeyM;
    m_auiRotKeyP[iIndex] = eKeyP;
    m_abRotAnalogControl[iIndex] = false;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotButtonsM(int iIndex, NiInputMouse::Button eButM, 
    NiInputMouse::Button eButP)
{
    NIASSERT(m_eInputDevice == TUR_MOUSE);
    m_auiRotKeyM[iIndex] = eButM;
    m_auiRotKeyP[iIndex] = eButP;
    m_abRotAnalogControl[iIndex] = false;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotButtonsGP(int iIndex, NiInputGamePad::Button eButM, 
    NiInputGamePad::Button eButP)
{
    NIASSERT(m_eInputDevice == TUR_GAMEPAD);
    m_auiRotKeyM[iIndex] = eButM;
    m_auiRotKeyP[iIndex] = eButP;
    m_abRotAnalogControl[iIndex] = false;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotButtonsStickDirGP(int iIndex, 
    NiInputGamePad::Sticks eStick, NiInputGamePad::Sticks eDir)
{
    NIASSERT(m_eInputDevice == TUR_GAMEPAD);
    
    //  The Minus value indicates the stick, the Plus the direction
    m_auiRotKeyM[iIndex] = eStick;
    m_auiRotKeyP[iIndex] = eDir;
    m_abRotAnalogControl[iIndex] = true;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnModifiers(int iIndex, long lModifiers)
{
    m_auiTrnKeyModifiers[iIndex] = lModifiers;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotModifiers(int iIndex, long lModifiers)
{
    m_auiRotKeyModifiers[iIndex] = lModifiers;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnObject(NiAVObject* pkObject)
{
    m_aspTrnObject[0] = pkObject;
    m_aspTrnObject[1] = pkObject;
    m_aspTrnObject[2] = pkObject;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotObject(NiAVObject* pkObject)
{
    m_aspRotObject[0] = pkObject;
    m_aspRotObject[1] = pkObject;
    m_aspRotObject[2] = pkObject;
}
//---------------------------------------------------------------------------
void NiTurret::SetTrnObject(int iIndex, NiAVObject* pkObject)
{
    m_aspTrnObject[iIndex] = pkObject;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotObject(int iIndex, NiAVObject* pkObject)
{
    m_aspRotObject[iIndex] = pkObject;
}
//---------------------------------------------------------------------------
void NiTurret::SetRotUseUpdateAxes(int iIndex, bool bUseAxis, 
    bool bUpdateAxis)
{
    m_abUseAxis[iIndex] = bUseAxis;
    m_abUpdateAxis[iIndex] = bUpdateAxis;
}
//---------------------------------------------------------------------------
bool NiTurret::ReadTrn(int i)
{
    if (m_aspTrnObject[i] && m_afTrnSpeed[i] != 0.0f)
    {
        float fSpeed = 0.0f;
        long lModifiers = m_auiTrnKeyModifiers[i];

        if (m_eInputDevice == TUR_GAMEPAD)
        {
            if (m_abTrnAnalogControl[i])
            {
                ReadTrnAnalog(i, lModifiers, fSpeed);
            }
            else
            {
                if (ReadButton(m_auiTrnKeyM[i]))
                    fSpeed = -m_afTrnSpeed[i];
                else if (ReadButton(m_auiTrnKeyP[i]))
                    fSpeed = +m_afTrnSpeed[i];
            }
        }
        else if ( m_eInputDevice == TUR_KEYBOARD )
        {
            if (ReadButton(m_auiTrnKeyM[i]))
                fSpeed = -m_afTrnSpeed[i];
            else if (ReadButton(m_auiTrnKeyP[i]))
                fSpeed = +m_afTrnSpeed[i];
        }

        if (fSpeed != 0.0f)
        {
            NiPoint3 trn = m_aspTrnObject[i]->GetTranslate();
            NiPoint3 kIncr = fSpeed * m_akAxis[i];
            trn += kIncr;
            m_aspTrnObject[i]->SetTranslate(trn);
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiTurret::ReadTrnAnalog(int i, long, float& fSpeed)
{
    bool bResult = false;

    for (unsigned int iGamePad = 0; 
        iGamePad < NiInputSystem::MAX_GAMEPADS;
        iGamePad++
        )
    {
        NiInputGamePad* pkGamePad = NiApplication::ms_pkApplication->
            GetInputSystem()->GetGamePad(iGamePad);

        if (!pkGamePad)
            continue;

        //  Check stick - consoles only have 2, so...
        if ((m_auiTrnKeyM[i] != NiInputGamePad::NIGP_STICK_LEFT) &&
            (m_auiTrnKeyM[i] != NiInputGamePad::NIGP_STICK_RIGHT))
        {
            continue;
        }

        //  Check direction
        if ((m_auiTrnKeyP[i] != NiInputGamePad::NIGP_STICK_AXIS_H) &&
            (m_auiTrnKeyP[i] != NiInputGamePad::NIGP_STICK_AXIS_V))
        {
            continue;
        }

        if (m_auiTrnKeyModifiers[i] != pkGamePad->GetModifiers())
            continue;

        int iHorz = 0;
        int iVert = 0;
        int iVal  = 0;

        //  Read value    
        pkGamePad->GetStickValue((NiInputGamePad::Sticks)m_auiTrnKeyM[i], 
            iHorz, iVert);
        switch(m_auiTrnKeyP[i])
        {
        case NiInputGamePad::NIGP_STICK_AXIS_H:
            iVal = iHorz;
            break;
        case NiInputGamePad::NIGP_STICK_AXIS_V:
            iVal = iVert;
            // Up is negative on a game pad.
            iVal *= -1;
            break;
        default:
            continue;
        }

        // remap value to [-1, 1]
        int iLow = 0;
        int iHigh = 0;
        pkGamePad->GetAxisRange(iLow, iHigh);
        float fValue = 0;
        if (iHigh - iLow != 0)
            fValue = (iVal - iLow) / (float)(iHigh - iLow) * 2 - 1; 

        fSpeed += fValue * m_afTrnSpeed[i];
        bResult = true;
    }
        
    return bResult;
}
//---------------------------------------------------------------------------
bool NiTurret::ReadRot(int i)
{
    if (m_aspRotObject[i] && m_afRotSpeed[i] != 0.0f)
    {
        float fSpeed = 0.0f;
        long lModifiers = m_auiRotKeyModifiers[i];

        if (m_eInputDevice == TUR_GAMEPAD)
        {
            if (m_abRotAnalogControl[i])
            {
                ReadRotAnalog(i, lModifiers, fSpeed);
            }
            else
            {
                if (ReadButton(m_auiRotKeyM[i]))
                    fSpeed = -m_afRotSpeed[i];
                else if (ReadButton(m_auiRotKeyP[i]))
                    fSpeed = +m_afRotSpeed[i];
            }
        }
        else if (m_eInputDevice == TUR_KEYBOARD)
        {
            if (ReadButton(m_auiRotKeyM[i]))
                fSpeed = -m_afRotSpeed[i];
            else if (ReadButton(m_auiRotKeyP[i]))
                fSpeed = +m_afRotSpeed[i];
        }

        if (fSpeed != 0.0f)
        {
            NiMatrix3 kRot = m_aspRotObject[i]->GetRotate();
            NiMatrix3 kIncr;
            if (m_abUseAxis[i])
            {
                kIncr.MakeRotation(fSpeed, m_akAxis[i]);
                if (m_abUpdateAxis[i])
                {
                    for (int j = 0; j < 3; j++)
                    {
                        if (j != i)
                            m_akAxis[j] = kIncr*m_akAxis[j];
                    }
                }
            }
            else
            {
                NiPoint3 kAxis;
                kRot.GetCol(i, kAxis);
                kIncr.MakeRotation(fSpeed, kAxis);
            }
            kRot = kIncr * kRot;
            m_aspRotObject[i]->SetRotate(kRot);

            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiTurret::ReadRotAnalog(int i, long, float& fSpeed)
{
    bool bResult = false;

    for (unsigned int iGamePad = 0; 
        iGamePad < NiInputSystem::MAX_GAMEPADS;
        iGamePad++
        )
    {
        NiInputGamePad* pkGamePad = NiApplication::ms_pkApplication->
            GetInputSystem()->GetGamePad(iGamePad);

        if (!pkGamePad)
            continue;

        //  Check stick
        if ((m_auiRotKeyM[i] !=(int)NiInputGamePad::NIGP_STICK_LEFT) &&
            (m_auiRotKeyM[i] !=(int)NiInputGamePad::NIGP_STICK_RIGHT))
        {
            continue;
        }

        //  Check direction
        if ((m_auiRotKeyP[i] !=(int)NiInputGamePad::NIGP_STICK_AXIS_H) &&
            (m_auiRotKeyP[i] !=(int)NiInputGamePad::NIGP_STICK_AXIS_V))
        {
            continue;
        }         

        if (m_auiRotKeyModifiers[i] != pkGamePad->GetModifiers())
            continue;

        int iHorz = 0;
        int iVert = 0;
        int iVal  = 0;

        //  Read value    
        pkGamePad->GetStickValue((NiInputGamePad::Sticks)m_auiRotKeyM[i], 
            iHorz, iVert);

        switch(m_auiRotKeyP[i])
        {
        case NiInputGamePad::NIGP_STICK_AXIS_H:
            iVal = iHorz;
            break;
        case NiInputGamePad::NIGP_STICK_AXIS_V:
            iVal = iVert;
            break;
        default:
            continue;
        }

        // remap value to [-1, 1]
        int iLow = 0;
        int iHigh = 0;
        pkGamePad->GetAxisRange(iLow, iHigh);
        float fValue = 0;
        if (iHigh - iLow != 0)
            fValue = (iVal - iLow) / (float)(iHigh - iLow) * 2 - 1; 

        fSpeed += fValue * m_afRotSpeed[i];
        bResult = true;
    }
        
    return bResult;
}
//---------------------------------------------------------------------------
bool NiTurret::ReadButton(int iKey, long* plModifiers)
{
    NiInputSystem* pkInput = 
        NiApplication::ms_pkApplication->GetInputSystem();
    NIASSERT(pkInput);

    switch (m_eInputDevice)
    {
    case TUR_KEYBOARD:
        {
            NiInputKeyboard* pkKeyboard = pkInput->GetKeyboard();
            NIASSERT(pkKeyboard);
            return pkKeyboard->KeyIsDown((NiInputKeyboard::KeyCode)iKey);
        }
    case TUR_MOUSE:
        {
            NiInputMouse* pkMouse = pkInput->GetMouse();
            NIASSERT(pkMouse);
            return pkMouse->ButtonIsDown((NiInputMouse::Button)iKey);
        }
    case TUR_GAMEPAD:
        {
            for (unsigned int iGamePad = 0; 
                iGamePad < NiInputSystem::MAX_GAMEPADS;
                iGamePad++
                )
            {
                NiInputGamePad* pkGamePad = pkInput->GetGamePad(iGamePad);
                if (!pkGamePad)
                    continue;
                if (plModifiers)
                {
                    unsigned int lModifiers = *plModifiers;
                    if (pkGamePad->ButtonIsDown(
                        (NiInputGamePad::Button)iKey, 
                        lModifiers))
                    {
                        return true;
                    }
                }
                else
                {
                    if (pkGamePad->ButtonIsDown(
                        (NiInputGamePad::Button)iKey))
                    {
                        return true;
                    }
                }
            }
        }

    default:
        break;

    }
    return false;
}
//---------------------------------------------------------------------------
bool NiTurret::Read()
{
    bool bTD = ReadTrn(0);
    bool bTU = ReadTrn(1);
    bool bTR = ReadTrn(2);
    bool bRD = ReadRot(0);
    bool bRU = ReadRot(1);
    bool bRR = ReadRot(2);

    return bTD || bTU || bTR || bRD || bRU || bRR;
}
//---------------------------------------------------------------------------
NiTurret::TurretInput NiTurret::GetInputDevice(void)
{
    return m_eInputDevice;
}
//---------------------------------------------------------------------------
void NiTurret::SetInputDevice(NiTurret::TurretInput eInputDevice)
{
    m_eInputDevice = eInputDevice;
}
//---------------------------------------------------------------------------
