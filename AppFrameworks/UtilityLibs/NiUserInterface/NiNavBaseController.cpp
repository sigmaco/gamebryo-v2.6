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
#include "NiUserInterfacePCH.h"

#include "NiNavBaseController.h"
#include "NiUIManager.h"
#include "NiNavManager.h"

//---------------------------------------------------------------------------
NiNavBaseController::NiNavBaseController(const char* pcName) :
    m_bActive(false),
    m_bHaveMouseMotion(false),
    m_bHaveMouseZAxis(false),
    m_fMouseScalar(0.0008f),
    m_fMouseAccelPow(2.0f),
    m_fMouseAccelMul(0.5f),
    m_fGamePadAccelPow(2.0f),
    m_fGamePadAccelMul(1.0f)
{
    m_kMouseSlot.Initialize(this, &NiNavBaseController::MouseLeftButton);

    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < NiInputGamePad::NIGP_NUM_DEFAULT_AXIS; ++uiIdx)
    {
        m_bHaveGamePadAxis[uiIdx] = false;
    }

    if (pcName != NULL)
    {
        m_kName = pcName;
    }
    else
    {
        m_kName = '\0';
    }
}
//---------------------------------------------------------------------------
NiNavBaseController::~NiNavBaseController()
{
    // This unreserves any reserved hotkeys
    NavState kNavState;
    kNavState.bIgnoreThis = true;
    if (m_bActive)
        SetActive(false, kNavState);

    unsigned int uiIdx;
    unsigned int uiSize = m_kSlots.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        NiDelete m_kSlots.GetAt(uiIdx);
    }
}
//---------------------------------------------------------------------------
bool NiNavBaseController::SetActive(bool bActive, NavState&)
{
    unsigned int uiIdx;
    NiUIManager* pkUIManager = NiUIManager::GetUIManager();
    if (pkUIManager == NULL)
        return false;

    m_bActive = bActive;

    // If we only want ISPRESSED events we should change the next line
    unsigned char ucReservedStates = NiUIManager::ISPRESSED | 
        NiUIManager::WASPRESSED | NiUIManager::WASRELEASED;
    InputBinding kPosKey, kNegKey;

    bool bKeyboardConnected, bMouseConnected, bGamePadConnected;
    bKeyboardConnected = pkUIManager->KeyboardExists();
    bMouseConnected = pkUIManager->MouseExists();
    bGamePadConnected = pkUIManager->GamePadExists();
    bool bGotMouseAlready = false;

    bool bReturnValue = true;
    // Note that this code can return false for one of two reasons:
    // 1) No NiUIManager
    // 2) An inability to reserve hotkeys (when bActive == true)
    // This means that if SetActive(true, ...) fails, this may still have
    // reserved a number of keys and SetActive(false, ...) should be called.
    // 
    // This was designed thus so that derived classes could override this
    // function, call this function, check the return value and if false
    // check for 1.  They could then choose to return true (if 1  
    // is true), effectively overriding 2.
    // 
    // Otherwise, the code in this loop is organized very similar to that in 
    // the above function NiNavBaseController::InitializeUIElements.  The
    // only addition is that the innermost "ifs" had to be added to check if
    // we were to reserve or unreserve the hotkeys
    unsigned int uiNumControls = GetControlCount();
    for (uiIdx = 0; uiIdx < uiNumControls; ++uiIdx)
    {
        if (!m_kIsActive.GetAt(uiIdx))
            continue;

        kPosKey = GetBinding(uiIdx, POSITIVE_AXIS);
        kNegKey = GetBinding(uiIdx, NEGATIVE_AXIS);

        if (bKeyboardConnected && (kPosKey.SupportsKeyboard()))
        {
            if (bActive)
            {
                bReturnValue = bReturnValue && 
                    pkUIManager->ReserveKeyboardButton(kPosKey.eKeyboardKey, 
                    m_kSlots.GetAt(GetIdx(uiIdx, true)), ucReservedStates,
                    kPosKey.eKeyboardModifier);
            }
            else
            {
                pkUIManager->UnreserveKeyboardButton(kPosKey.eKeyboardKey, 
                    m_kSlots.GetAt(GetIdx(uiIdx, true)), 
                    kPosKey.eKeyboardModifier);
            }
        }
        if (bKeyboardConnected && (kNegKey.SupportsKeyboard()))
        {
            if (bActive)
            {
                bReturnValue = bReturnValue && 
                    pkUIManager->ReserveKeyboardButton(kNegKey.eKeyboardKey,
                    m_kSlots.GetAt(GetIdx(uiIdx, false)), ucReservedStates,
                    kNegKey.eKeyboardModifier);
            }
            else
            {
                pkUIManager->UnreserveKeyboardButton(kNegKey.eKeyboardKey,
                    m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                    kNegKey.eKeyboardModifier);
            }
        }
        if (bMouseConnected && (kPosKey.SupportsMouseAxis()))
        {
            NIASSERT(kNegKey.SupportsMouseAxis());
            NIASSERT(kNegKey.eMouseAxis == kPosKey.eMouseAxis);
            if (bActive)
            {
                if (!bGotMouseAlready)
                {
                    bool tmp = pkUIManager->ReserveMouse(m_kMouseSlot);
                    bReturnValue = bReturnValue && tmp;
                    bGotMouseAlready = tmp;
                }
                if (kPosKey.eMouseAxis == NiInputMouse::NIM_AXIS_Z)
                    m_bHaveMouseZAxis = bGotMouseAlready;
            }
            else
            {
                if (!bGotMouseAlready)
                {
                    pkUIManager->UnreserveMouse(m_kMouseSlot);
                    bGotMouseAlready = true;
                }
                if (kPosKey.eMouseAxis == NiInputMouse::NIM_AXIS_Z)
                    m_bHaveMouseZAxis = bGotMouseAlready;
            }
        }
        else if (bMouseConnected)
        {
            if (kPosKey.SupportsMouseButton())
            {
                if (bActive)
                {
                    bReturnValue = bReturnValue && 
                        pkUIManager->ReserveMouseButton(kPosKey.eMouseButton, 
                        m_kSlots.GetAt(GetIdx(uiIdx, true)), ucReservedStates,
                        kPosKey.eMouseModifier);
                }
                else
                {
                    pkUIManager->UnreserveMouseButton(kPosKey.eMouseButton,
                        m_kSlots.GetAt(GetIdx(uiIdx, true)), 
                        kPosKey.eMouseModifier);
                }
            }
            if (kNegKey.SupportsMouseButton())
            {
                if (bActive)
                {
                    bReturnValue = bReturnValue && 
                        pkUIManager->ReserveMouseButton(kNegKey.eMouseButton, 
                        m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                        ucReservedStates, kNegKey.eMouseModifier);
                }
                else
                {
                    pkUIManager->UnreserveMouseButton(kNegKey.eMouseButton,
                        m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                        kNegKey.eMouseModifier);
                }
            }
        }
        if (bGamePadConnected && (kPosKey.SupportsGamePadAxis()))
        {
            NIASSERT(kNegKey.SupportsGamePadAxis());
            NIASSERT(kNegKey.eGamePadAxis == kPosKey.eGamePadAxis);
            if (bActive)
            {
                m_bHaveGamePadAxis[kPosKey.eGamePadAxis] = 
                    pkUIManager->ReserveGamePadAxis(kPosKey.eGamePadAxis,
                    m_kSlots.GetAt(GetIdx(uiIdx, true)));
                bReturnValue = bReturnValue && m_bHaveGamePadAxis[
                    kPosKey.eGamePadAxis];
            }
            else
            {
                pkUIManager->UnreserveGamePadAxis(kPosKey.eGamePadAxis, 
                    m_kSlots.GetAt(GetIdx(uiIdx, true)));
            }
        }
        else if (bGamePadConnected)
        {
            if (kPosKey.SupportsGamePadButton())
            {
                if (bActive)
                {
                    bReturnValue = bReturnValue && 
                        pkUIManager->ReserveGamePadButton(
                            kPosKey.eGamePadButton, 
                            m_kSlots.GetAt(GetIdx(uiIdx, true)), 
                            ucReservedStates, 
                            kPosKey.eGamePadModifier);

                    if (kPosKey.eGamePadOptionalModifier !=
                        NiInputGamePad::NIGP_MASK_NONE)
                    {
                        bReturnValue = bReturnValue && 
                            pkUIManager->ReserveGamePadButton(
                                kPosKey.eGamePadButton, 
                                m_kSlots.GetAt(GetIdx(uiIdx, true)), 
                                ucReservedStates, 
                                kPosKey.eGamePadOptionalModifier);
                    }
                }
                else
                {
                    pkUIManager->UnreserveGamePadButton(
                        kPosKey.eGamePadButton,
                        m_kSlots.GetAt(GetIdx(uiIdx, true)), 
                        kPosKey.eGamePadModifier);

                    if (kPosKey.eGamePadOptionalModifier !=
                        NiInputGamePad::NIGP_MASK_NONE)
                    {
                        pkUIManager->UnreserveGamePadButton(
                            kPosKey.eGamePadButton,
                            m_kSlots.GetAt(GetIdx(uiIdx, true)), 
                            kPosKey.eGamePadOptionalModifier);
                    }
                }
            }
            if (kNegKey.SupportsGamePadButton())
            {
                if (bActive)
                {
                    bReturnValue = bReturnValue && 
                        pkUIManager->ReserveGamePadButton(
                            kNegKey.eGamePadButton, 
                            m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                            ucReservedStates, 
                            kNegKey.eGamePadModifier);

                    if (kPosKey.eGamePadOptionalModifier !=
                        NiInputGamePad::NIGP_MASK_NONE)
                    {
                        bReturnValue = bReturnValue && 
                            pkUIManager->ReserveGamePadButton(
                            kNegKey.eGamePadButton, 
                            m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                            ucReservedStates, 
                            kNegKey.eGamePadOptionalModifier);
                    }
                }
                else
                {
                    pkUIManager->UnreserveGamePadButton(
                        kNegKey.eGamePadButton,
                        m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                        kNegKey.eGamePadModifier);

                    if (kPosKey.eGamePadOptionalModifier !=
                        NiInputGamePad::NIGP_MASK_NONE)
                    {
                        pkUIManager->UnreserveGamePadButton(
                            kNegKey.eGamePadButton,
                            m_kSlots.GetAt(GetIdx(uiIdx, false)), 
                            kNegKey.eGamePadOptionalModifier);
                    }
                }
            }
        }
    }
    return bReturnValue;
}
//---------------------------------------------------------------------------
void NiNavBaseController::SampleAndUpdate()
{
    if (!m_bActive)
        return;
    if (NiUIManager::GetUIManager() == NULL)
        return;

    unsigned int uiIdx;
    InputBinding kPosKey, kNegKey;
 
    // Note that this code only cares about the GamePad and Mouse axes
    unsigned int uiNumControls = GetControlCount();
    int iDelta = 0;
    int iLow, iHigh;
    for (uiIdx = 0; uiIdx < uiNumControls; ++uiIdx)
    {
        if (!m_kIsActive.GetAt(uiIdx))
            continue;

        kPosKey = GetBinding(uiIdx, POSITIVE_AXIS);
        kNegKey = GetBinding(uiIdx, NEGATIVE_AXIS);

        if (NiUIManager::GetUIManager()->MouseExists() && 
            kPosKey.SupportsMouseAxis())
        {
            // Note that the assert makes sure that the negative direction 
            // also uses a mouse axis and that they both use the same axis  
            NIASSERT(kNegKey.SupportsMouseAxis());
            NIASSERT(kNegKey.eMouseAxis == kPosKey.eMouseAxis);

            NiInputMouse* pkMouse = NiUIManager::GetUIManager()->GetMouse();
            NIASSERT(pkMouse != NULL);
            if (((kPosKey.eMouseAxis == NiInputMouse::NIM_AXIS_Z) && 
                m_bHaveMouseZAxis) || m_bHaveMouseMotion)
            {
                pkMouse->GetPositionDelta(kPosKey.eMouseAxis, iDelta);
                float fFrameTime = 
                    NiNavManager::GetNavManager()->GetFrameTime();
                if (iDelta != 0 && fFrameTime > 0)
                {
                    // Normalize the mouse delta into a velocity
                    float fVelocity = iDelta * m_fMouseScalar / fFrameTime;
                    bool bNegative = fVelocity < 0;
                    float fAbsVelocity = 
                        NiSelect(bNegative, -fVelocity, fVelocity);
                    
                    // Accelerate / Exaggerate mouse velocity by:
                    // adding an exponential to the linear value.
                    // This retains the linear value 
                    // (exponential alone decreases value if in 0-1 range).
                    float fExaggeratedVelocity = 
                        fAbsVelocity + NiPow(fAbsVelocity, m_fMouseAccelPow);
                    // Use a multiplier over result for ease of tuning
                    fExaggeratedVelocity *= m_fMouseAccelMul;

                    // Restore negative sign
                    float fDelta = 
                        NiSelect(bNegative, 
                        -fExaggeratedVelocity, 
                        fExaggeratedVelocity);

                    MouseInput(uiIdx, fDelta);
                }
            }
        }
        if ((NiUIManager::GetUIManager()->GamePadExists()) &&
            (kPosKey.SupportsGamePadAxis()) && 
            m_bHaveGamePadAxis[kPosKey.eGamePadAxis])
        {
            // Note that the assert makes sure that the negative direction
            // also uses the Gamebryo axis and that they use the same axis.
            NIASSERT(kNegKey.SupportsGamePadAxis());
            NIASSERT(kNegKey.eGamePadAxis == kPosKey.eGamePadAxis);

            for (unsigned int uiPad = 0; 
                uiPad < NiInputSystem::MAX_GAMEPADS; 
                uiPad++)
            {
                NiInputGamePad* pkGamePad = 
                    NiUIManager::GetUIManager()->
                    GetInputSystem()->
                    GetGamePad(uiPad);

                if (pkGamePad)
                {
                    iDelta = pkGamePad->GetAxisValue(kPosKey.eGamePadAxis);
                    pkGamePad->GetAxisRange(iLow, iHigh);
                    // This scales it to the 0..1 range
                    float fDelta = 
                        ((float)(iDelta - iLow)) 
                        / ((float)(iHigh - iLow));
                    // This moves it to the -1..1 range
                    fDelta = (fDelta - 0.5f) * 2.0f;

                    // Accelerate gamepad movement.
                    bool bNegative = fDelta < 0;
                    float fDeltaAbs = NiAbs(fDelta);
                    fDeltaAbs = m_fGamePadAccelMul * 
                        NiPow(fDeltaAbs, m_fGamePadAccelPow);

                    // restore sign
                    fDelta = NiSelect(bNegative, -fDeltaAbs, fDeltaAbs);
                    if (fabs(fDelta) > (float)1e-4)
                        GamePadInput(uiIdx, fDelta);
                }
            }
        }
    }

    UpdatePose();
}
//---------------------------------------------------------------------------
void NiNavBaseController::MouseLeftButton(unsigned char ucState)
{
    if (ucState & NiUIManager::WASPRESSED)
        m_bHaveMouseMotion = true;
    else if (ucState & NiUIManager::WASRELEASED)
        m_bHaveMouseMotion = false;
}
//---------------------------------------------------------------------------
