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

#include "NiUIManager.h"
//---------------------------------------------------------------------------
NiUIManager* NiUIManager::ms_pkUIManager = NULL;
const unsigned int NiUIManager::ms_auiResolutionLevels[2] = {
    600, 768};
//---------------------------------------------------------------------------
NiUIManager::NiUIManager() : 
    m_iCurUIGroup(-1),
    m_kAtlasMap(),
    m_spFont(NULL),
    m_spInputSystem(NULL),
    m_spCursor(NULL),
    m_kMaxCharDimensionsInNSC(0.0f, 0.0f),
    m_bMouseExists(false),
    m_bKeyboardExists(false),
    m_bGamePadExists(false),
    m_bMouseReserved(false),
    m_bSearchWithMouse(true),
    m_bEnterIsPressed(false),
    m_bMouseOverButton(false),
    m_bEmitMouseSignal(false),
    m_bVisible(true)
{
    m_kMoveDown.Initialize(this, &NiUIManager::MoveDown);
    m_kMoveUp.Initialize(this, &NiUIManager::MoveUp);
    m_kEnterPressed.Initialize(this, &NiUIManager::EnterPressed);
    m_kLeftMouseBtn.Initialize(this, &NiUIManager::LeftMouseButton);
    m_kUsePointerNavigation.Initialize(this, &NiUIManager::UsePointerNavigation);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    m_kSafeFrameInNDC = pkRenderer->GetSafeZone();

    unsigned int uiWidth, uiHeight;
    pkRenderer->ConvertFromNDCToPixels(1.0f, 1.0f, uiWidth, uiHeight);

    m_fDisplayWidthInPixels = (float) uiWidth;
    m_fDisplayHeightInPixels = (float) uiHeight;

}
//---------------------------------------------------------------------------
NiUIManager::~NiUIManager()
{
    m_spFont = NULL;
    m_spInputSystem = NULL;
    m_spCursor = NULL;

    unsigned int uiIdx, uiSize;
    uiSize = m_kMouseActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kMouseActionSet.GetAt(uiIdx) = NULL;
    }
    uiSize = m_kGamePadActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kGamePadActionSet.GetAt(uiIdx) = NULL;
    }
    uiSize = m_kKeyboardActionSet.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kKeyboardActionSet.GetAt(uiIdx) = NULL;
    }
    uiSize = m_kUIGroups.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kUIGroups.SetAt(uiIdx, NULL);
    }
    uiSize = m_kUIRenderGroups.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kUIRenderGroups.SetAt(uiIdx, NULL);
    }
}
//---------------------------------------------------------------------------
void NiUIManager::Create()
{
    if (NiRenderer::GetRenderer() == NULL)
    {
        NiOutputDebugString("NiUIManager::Create() attempted before the "
            "NiRenderer was created.  This is explicitly not allowed.");
        return;
    }

    if (ms_pkUIManager != NULL)
        return;

    ms_pkUIManager = NiNew NiUIManager();
}
//---------------------------------------------------------------------------
NiUIManager* NiUIManager::GetUIManager()
{
    return ms_pkUIManager;
}
//---------------------------------------------------------------------------
void NiUIManager::Shutdown()
{
    NiDelete ms_pkUIManager;
    ms_pkUIManager = NULL;
}
//---------------------------------------------------------------------------
bool NiUIManager::Initialize(NiInputSystem* pkInputSystem,
    const char* pcAtlasFilename, NiCursor* pkCursor)
{
    if (pkInputSystem == NULL)
    {
        NiOutputDebugString("NiUIManager::Initialize failed due to no NiInput"
            " system initialized.");
        return false;
    }
    if (pkInputSystem->GetMode() != NiInputSystem::IMMEDIATE)
    {
        NiOutputDebugString("NiUIManager::Initialize failed due to improper "
            "NiInput initialization.  NiInputSystem must be in immediate "
            "mode.");
        return false;
    }
    if (pcAtlasFilename == NULL)
    {
        NiOutputDebugString("NiUIManager::Initialize failed due invalid"
            "\"Look and Feel\" texture filename.");
        return false;
    }
    if (!m_kAtlasMap.ReadFile(pcAtlasFilename))
    {
        NiOutputDebugString("NiUIManager::Initialize failed because \"Look "
            "and Feel\" texture file did not open properly.");
        return false;
    }

    // First I need to get the path from pcAtlasFilename
    NiString kFilename(pcAtlasFilename);
    unsigned int uiFrontSlash = kFilename.FindReverse('/');
    unsigned int uiBackSlash = kFilename.FindReverse('\\');
    if ((uiFrontSlash != NiString::INVALID_INDEX) && 
        (uiBackSlash != NiString::INVALID_INDEX))
    {
        if (uiFrontSlash > uiBackSlash)
        {
            kFilename = kFilename.GetSubstring(0, uiFrontSlash + 1);
        }
        else
        {
            kFilename = kFilename.GetSubstring(0, uiBackSlash + 1);
        }
    }
    else if (uiFrontSlash != NiString::INVALID_INDEX)
    {
        kFilename = kFilename.GetSubstring(0, uiFrontSlash + 1);
    }
    else if (uiBackSlash != NiString::INVALID_INDEX)
    {
        kFilename = kFilename.GetSubstring(0, uiBackSlash + 1);
    }
    else
    {
        kFilename = NiString("");
    }
    // Depending on the screen resolution, get the correct font
    unsigned int uiWidth, uiHeight;   
    NiRenderer::GetRenderer()->ConvertFromNDCToPixels(1.0f, 1.0f, 
        uiWidth, uiHeight);
    if (uiHeight <= ms_auiResolutionLevels[0])
    {
        // Low res
        kFilename += m_kAtlasMap.GetTextFilename(
            NiUIAtlasMap::TEXT_FOR_LOW_RESOLUTION);
    }
    else if (uiHeight <= ms_auiResolutionLevels[1])
    {
        // medium res
        kFilename += m_kAtlasMap.GetTextFilename(
            NiUIAtlasMap::TEXT_FOR_MED_RESOLUTION);
    }
    else
    {
        // high res
        kFilename += m_kAtlasMap.GetTextFilename(
            NiUIAtlasMap::TEXT_FOR_HIGH_RESOLUTION);
    }

    m_spFont = NiFont::Create(NiRenderer::GetRenderer(), kFilename);
    if (m_spFont == NULL)
    {
        NiOutputDebugString("NiUIManager::Initialize failed because it "
            "couldn\'t find the NFF file for the NiFont.");
        return false;
    }

    float fMaxCharWidth = 0.0f;
    float fMaxCharHeight = 0.0f;
    for (char c = 33; c < 127; c++)
    {
        float fCharWidth = 0.0f;
        float fCharHeight = 0.0f;
        m_spFont->GetCharExtent(c, fCharWidth, fCharHeight);

        fMaxCharWidth = NiMax(fMaxCharWidth, fCharWidth);
        fMaxCharHeight = NiMax(fMaxCharHeight, fCharHeight);
    }

    // Compute the NSC dimensions of the maximum character value.
    m_kMaxCharDimensionsInNSC = ConvertPixelDimensionsToNSCDimensions(
        (unsigned int)fMaxCharWidth, (unsigned int)fMaxCharHeight);
    
    m_spInputSystem = pkInputSystem;

    unsigned int uiNumDevices = m_spInputSystem->GetAvailableDeviceCount();
    unsigned int uiIdx;
    NiInputDevice::Description* kDevDescription;
    for (uiIdx = 0; uiIdx < uiNumDevices; ++uiIdx)
    {
        if (uiIdx == 0)
            kDevDescription = m_spInputSystem->GetFirstDeviceDesc();
        else
            kDevDescription = m_spInputSystem->GetNextDeviceDesc();

        switch (kDevDescription->GetType())
        {
        case (NiInputDevice::NIID_KEYBOARD):
            m_bKeyboardExists = true;
            break;
        case (NiInputDevice::NIID_MOUSE):
            m_bMouseExists = true;
            break;
        case (NiInputDevice::NIID_GAMEPAD):
            m_bGamePadExists = true;
            break;
        case (NiInputDevice::NIID_NULL):
        default:
            break;

        }
    }
#if defined(_XENON) || defined(_PS3) || defined(_WII)
    m_bGamePadExists = true;
#endif
    if (!m_bKeyboardExists && !m_bMouseExists && !m_bGamePadExists)
    {
        NiOutputDebugString("NiUIManager::Initialize failed because NiInput "
            "could identify no valid input devices.");
        return false;
    }

    ReserveGamePadButton(NiInputGamePad::NIGP_LDOWN, &m_kMoveDown, 
        WASPRESSED);
    ReserveGamePadButton(NiInputGamePad::NIGP_LUP, &m_kMoveUp, WASPRESSED);
    ReserveGamePadButton(NiInputGamePad::NIGP_RDOWN, &m_kEnterPressed,
        WASPRESSED | ISPRESSED | WASRELEASED);
#if defined(_WII)
    ReserveGamePadButton(NiInputGamePad::NIGP_RRIGHT, &m_kUsePointerNavigation,
        WASPRESSED | ISPRESSED | WASRELEASED);
#endif
    ReserveKeyboardButton(NiInputKeyboard::KEY_TAB, &m_kMoveDown, 
        WASPRESSED);
    ReserveKeyboardButton(NiInputKeyboard::KEY_TAB, &m_kMoveUp, WASPRESSED,
        NiInputKeyboard::KMOD_LSHIFT);
    ReserveKeyboardButton(NiInputKeyboard::KEY_TAB, &m_kMoveUp, WASPRESSED,
        NiInputKeyboard::KMOD_RSHIFT);
    ReserveKeyboardButton(NiInputKeyboard::KEY_RETURN, &m_kEnterPressed, 
        WASPRESSED | ISPRESSED | WASRELEASED);
    ReserveMouseButton(NiInputMouse::NIM_LEFT, &m_kLeftMouseBtn, 
        WASPRESSED | ISPRESSED | WASRELEASED);

    m_spCursor = pkCursor;

    return true;
}
//---------------------------------------------------------------------------
void NiUIManager::PrepareRenderer()
{
    // First we need to sort them by reverse depth.  Painter's algorithm will
    // work as long as the smallest depth is first drawn.
    //
    // Sort is implemented as selection sort at the moment.  If initialization
    // speed becomes important and there are many NiUIRenderGroups for some
    // reason, use a O(n log n) search.
    unsigned int uiIdx1, uiIdx2, uiSize;
    unsigned char ucMinDepth;
    unsigned int uiMinPos;
    uiSize = m_kUIRenderGroups.GetSize();
    for (uiIdx1 = 0; uiIdx1 < (uiSize - 1); ++uiIdx1)
    {
        uiMinPos = uiIdx1;
        ucMinDepth = m_kUIRenderGroups.GetAt(uiIdx1)->GetDepth();
        for (uiIdx2 = (uiIdx1 + 1); uiIdx2 < uiSize; ++uiIdx2)
        {
            if (m_kUIRenderGroups.GetAt(uiIdx2)->GetDepth() < ucMinDepth)
            {
                uiMinPos = uiIdx2;
                ucMinDepth = m_kUIRenderGroups.GetAt(uiIdx2)->GetDepth();
            }
        }
        NiUIRenderGroupPtr spTmp = m_kUIRenderGroups.GetAt(uiIdx1);
        m_kUIRenderGroups.SetAt(uiIdx1, m_kUIRenderGroups.GetAt(uiMinPos));
        m_kUIRenderGroups.SetAt(uiMinPos, spTmp);
        spTmp = NULL;
    }

    // Now we need to tell each of the children to initialize their 
    // render quads
    for (uiIdx1 = 0; uiIdx1 < uiSize; ++uiIdx1)
    {
        m_kUIRenderGroups.GetAt(uiIdx1)->InitializeScreenElements();
    }
}
//---------------------------------------------------------------------------
void NiUIManager::UpdateUI()
{
    if (m_spInputSystem == NULL)
    {
        return;
    }

    unsigned int uiIdx, uiSize, uiModifiers;
    unsigned char ucButtonState;

    // Mouse Events
    NiInputMouse* pkInputMouse = m_spInputSystem->GetMouse();
    if (pkInputMouse)
    {
        NiMouseHotkeySignal* pkMouseHS;

        // First handle the button
        for (uiIdx = 0; uiIdx < m_kMouseActionSet.GetSize(); ++uiIdx)
        {
            ucButtonState = 0;
            pkMouseHS = m_kMouseActionSet.GetAt(uiIdx);
            if ((pkMouseHS->m_ucState & ISPRESSED) && 
                (pkInputMouse->ButtonIsDown(pkMouseHS->m_eHotkey, 
                uiModifiers)))
            {
                if (uiModifiers == (unsigned int)pkMouseHS->m_eModifier)
                    ucButtonState |= ISPRESSED;
            }
            if ((pkMouseHS->m_ucState & WASPRESSED) && 
                (pkInputMouse->ButtonWasPressed(pkMouseHS->m_eHotkey, 
                uiModifiers)))
            {
                if (uiModifiers == (unsigned int)pkMouseHS->m_eModifier)
                    ucButtonState |= WASPRESSED;
            }
            if ((pkMouseHS->m_ucState & WASRELEASED) && 
                (pkInputMouse->ButtonWasReleased(pkMouseHS->m_eHotkey, 
                uiModifiers)))
            {
                if (uiModifiers == (unsigned int)pkMouseHS->m_eModifier)
                    ucButtonState |= WASRELEASED;
            }

            if (ucButtonState)
            {
                pkMouseHS->m_kSignal.EmitSignal(ucButtonState);
            }
        }

        if ((m_spCursor != NULL) && m_bSearchWithMouse && m_bVisible)
        {
            int iMouseX, iMouseY;
            m_spCursor->GetScreenSpacePosition(iMouseX, iMouseY);
            unsigned int uiMouseX = (iMouseX > 0) ? iMouseX : 0;
            unsigned int uiMouseY = (iMouseY > 0) ? iMouseY : 0;
            m_bMouseOverButton = false;
            uiSize = m_kUIGroups.GetSize();
            for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
            {
                if (m_kUIGroups.GetAt(uiIdx)->IsInside(uiMouseX, uiMouseY))
                {
                    if ((m_iCurUIGroup >= 0) && 
                        ((unsigned int)m_iCurUIGroup != uiIdx))
                    {
                        m_kUIGroups.GetAt(m_iCurUIGroup)->SetFocus(false);
                    }
                    m_iCurUIGroup = uiIdx;
                    m_bMouseOverButton = true;

                    m_kUIGroups.GetAt(m_iCurUIGroup)->SetFocus(true);
                    break;
                }
            }
        }
    }

    // Keyboard Events
    NiInputKeyboard* pkInputKeyboard = m_spInputSystem->GetKeyboard();
    if (pkInputKeyboard)
    {
        NiKeyboardHotkeySignal* pkKeyboardHS;

        for (uiIdx = 0; uiIdx < m_kKeyboardActionSet.GetSize(); ++uiIdx)
        {
            ucButtonState = 0;
            pkKeyboardHS = m_kKeyboardActionSet.GetAt(uiIdx);
            if ((pkKeyboardHS->m_ucState & ISPRESSED) &&
                (pkInputKeyboard->KeyIsDown(pkKeyboardHS->m_eHotkey,
                uiModifiers)))
            {
                if (uiModifiers == (unsigned int)pkKeyboardHS->m_eModifier)
                    ucButtonState |= ISPRESSED;
            }
            if ((pkKeyboardHS->m_ucState & WASPRESSED) &&
                (pkInputKeyboard->KeyWasPressed(pkKeyboardHS->m_eHotkey,
                uiModifiers)))
            {
                if (uiModifiers == (unsigned int)pkKeyboardHS->m_eModifier)
                    ucButtonState |= WASPRESSED;
            }
            if ((pkKeyboardHS->m_ucState & WASRELEASED) &&
                (pkInputKeyboard->KeyWasReleased(pkKeyboardHS->m_eHotkey,
                uiModifiers)))
            {
                if (uiModifiers == (unsigned int)pkKeyboardHS->m_eModifier)
                    ucButtonState |= WASRELEASED;
            }

            if (ucButtonState)
            {
                pkKeyboardHS->m_kSignal.EmitSignal(ucButtonState);
            }
        }
    }

    // GamePad Events
    for (unsigned int uiPad = 0; uiPad < NiInputSystem::MAX_GAMEPADS; uiPad++)
    {
        NiInputGamePad* pkInputGamePad = m_spInputSystem->GetGamePad(uiPad);
        if (pkInputGamePad)
        {
            NiGamePadHotkeySignal* pkGamePadHS;

            for (uiIdx = 0; uiIdx < m_kGamePadActionSet.GetSize(); ++uiIdx)
            {
                ucButtonState = 0;
                pkGamePadHS = m_kGamePadActionSet.GetAt(uiIdx);
                if (!pkGamePadHS->m_bIsHotkey)
                    continue;
                if ((pkGamePadHS->m_ucState & ISPRESSED) &&
                    (pkInputGamePad->ButtonIsDown(pkGamePadHS->m_eHotkey, 
                    uiModifiers)))
                {
                    // While holding the button down, it itself may become a modifier,
                    // so we need to check for that.
                    unsigned int uiButtonMask = 1 << pkGamePadHS->m_eHotkey;
                    if (uiModifiers == pkGamePadHS->m_uiModifier ||
                        uiModifiers == (pkGamePadHS->m_uiModifier | uiButtonMask))
                    {
                        ucButtonState |= ISPRESSED;
                    }
                }
                if ((pkGamePadHS->m_ucState & WASPRESSED) &&
                    (pkInputGamePad->ButtonWasPressed(pkGamePadHS->m_eHotkey,
                    uiModifiers)))
                {
                    // While holding the button down, it itself may become a modifier,
                    // so we need to check for that.
                    unsigned int uiButtonMask = 1 << pkGamePadHS->m_eHotkey;
                    if (uiModifiers == pkGamePadHS->m_uiModifier ||
                        uiModifiers == (pkGamePadHS->m_uiModifier | uiButtonMask))
                    {
                        ucButtonState |= WASPRESSED;
                    }
                }
                if ((pkGamePadHS->m_ucState & WASRELEASED) &&
                    (pkInputGamePad->ButtonWasReleased(pkGamePadHS->m_eHotkey,
                    uiModifiers)))
                {
                    // While holding the button down, it itself may become a modifier,
                    // so we need to check for that.
                    unsigned int uiButtonMask = 1 << pkGamePadHS->m_eHotkey;
                    if (uiModifiers == pkGamePadHS->m_uiModifier ||
                        uiModifiers == (pkGamePadHS->m_uiModifier | uiButtonMask))
                    {
                        ucButtonState |= WASRELEASED;
                    }
                }

                if (ucButtonState)
                {
                    pkGamePadHS->m_kSignal.EmitSignal(ucButtonState);
                }
            }
        }
    }

#if defined(_WII)
    // On the Wii, a Wii Remote drives the cursor that was passed
    // in, which we use like a mouse for the menus.  If enter is held
    // down, don't switch to a new group.
    if (m_bSearchWithMouse  && !m_bEnterIsPressed && m_spCursor != NULL)
    {
        int iX, iY;
        m_spCursor->GetScreenSpacePosition(iX, iY);
        unsigned int uiX = (iX > 0) ? iX : 0;
        unsigned int uiY = (iY > 0) ? iY : 0;
        
        uiSize = m_kUIGroups.GetSize();
        for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
        {
            if (m_kUIGroups.GetAt(uiIdx)->IsInside(uiX, uiY))
            {
                if ((m_iCurUIGroup >= 0) && 
                    ((unsigned int)m_iCurUIGroup != uiIdx))
                {
                    m_kUIGroups.GetAt(m_iCurUIGroup)->SetFocus(false);
                }
                m_iCurUIGroup = uiIdx;
                m_kUIGroups.GetAt(m_iCurUIGroup)->SetFocus(true);
                break;
            }
        }
    }
#endif
}
//---------------------------------------------------------------------------
void NiUIManager::Draw(NiRenderer* pkRenderer)
{
    if (!m_bVisible)
        return;

    unsigned int uiIdx, uiSize;
    uiSize = m_kUIRenderGroups.GetSize();
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        m_kUIRenderGroups.GetAt(uiIdx)->Draw(pkRenderer);
    }
}
//---------------------------------------------------------------------------
void NiUIManager::AddUIGroup(NiUIGroup* pkUIGroup)
{
    unsigned int uiIdx, uiSize;
    uiSize = m_kUIGroups.GetSize();
    
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (m_kUIGroups.GetAt(uiIdx) == pkUIGroup)
        {
            return;
        }
    }

    m_kUIGroups.Add(pkUIGroup);
    if (uiSize == 0)
    {
        m_iCurUIGroup = 0;
        pkUIGroup->SetFocus(true);
    }

    OrderGroups();
}
//---------------------------------------------------------------------------
void NiUIManager::OrderGroups()
{
    unsigned int uiIdx1, uiIdx2, uiMinIdx, uiSize;
    uiSize = m_kUIGroups.GetSize();
    NiPoint2 kPos, kCurMin;

    // Implemented as a selection sort now for simplicity
    // Could be reimplemented later for a O(n log n) sort instead of O(n^2)
    for (uiIdx1 = 0; uiIdx1 < uiSize; ++uiIdx1)
    {
        uiMinIdx = uiIdx1;
        kCurMin = m_kUIGroups.GetAt(uiMinIdx)->GetOffset();
        for (uiIdx2 = uiIdx1 + 1; uiIdx2 < uiSize; ++uiIdx2)
        {
            kPos = m_kUIGroups.GetAt(uiIdx2)->GetOffset();
            if (IsLower(kPos, kCurMin))
            {
                kCurMin = kPos;
                uiMinIdx = uiIdx2;
            }
        }
        NiUIGroupPtr tmp = m_kUIGroups.GetAt(uiMinIdx);
        m_kUIGroups.SetAt(uiMinIdx, m_kUIGroups.GetAt(uiIdx1));
        m_kUIGroups.SetAt(uiIdx1, tmp);
    }
}
//---------------------------------------------------------------------------
void NiUIManager::RemoveUIGroup(NiUIGroup* pkUIGroup)
{
    unsigned int uiSize, uiIdx;
    uiSize = m_kUIGroups.GetSize();
    bool bWasFound = false;
    NiUIGroupPtr tmp;

    // This runs through the array, leaving order the same except when the
    // element is found.  At that point the last element has been removed
    // from the end and placed in the current position (and bWasFound = true).
    // 
    // From that point on, the loop shifts the items over one to maintain 
    // ordering.  If the item was found and the list is over greater size 
    // remaining is more than 1, the last item must be copied in at the end.
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        if (!bWasFound)
        {
            if (m_kUIGroups.GetAt(uiIdx) == pkUIGroup)
            {
                bWasFound = true;
                m_kUIGroups.RemoveAtAndFill(uiIdx);

                // This is guaranteed to be at least 0 because otherwise
                // this line would not be reached
                uiSize--;

                // This if necessary for the case where the indexed element
                // in the list was just removed
                if (uiSize != uiIdx)
                {
                    tmp = m_kUIGroups.GetAt(uiIdx);
                }

            }
        }
        else
        {
            // uiIdx-1 guaranteed to be at least 0 because
            // uiIdx must be at least 1
            m_kUIGroups.SetAt(uiIdx-1, m_kUIGroups.GetAt(uiIdx));
        }
    }
    if ((uiSize > 1) && (bWasFound))
    {
        // This places at the end what had been at the end before
        m_kUIGroups.SetAt(uiSize-1, tmp);
    }
}
//---------------------------------------------------------------------------
void NiUIManager::MoveDown(unsigned char)
{
    if (!m_bSearchWithMouse)
        return;

    unsigned int uiSize = m_kUIGroups.GetSize();
    if (uiSize == 0)
        return;

    if ((m_iCurUIGroup == -1) || 
        (((unsigned int)m_iCurUIGroup) >= uiSize))
    {
        m_iCurUIGroup = 0;
    }

    unsigned int uiNumPasses = 0;
    while (!m_kUIGroups.GetAt(m_iCurUIGroup)->AdvanceFocus())
    {
        ++m_iCurUIGroup;

        if (((unsigned int)m_iCurUIGroup) >= uiSize)
        {
            m_iCurUIGroup = 0;
            ++uiNumPasses;
            if (uiNumPasses > 1)
            {
                m_iCurUIGroup = -1;
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiUIManager::MoveUp(unsigned char)
{
    if (!m_bSearchWithMouse)
        return;

    unsigned int uiSize = m_kUIGroups.GetSize();
    if (uiSize == 0)
        return;

    if ((m_iCurUIGroup == -1) || (((unsigned int)m_iCurUIGroup) >= uiSize))
    {
         // guaranteed to be >= 0 by if above
        m_iCurUIGroup = uiSize - 1;
    }

    unsigned int uiNumPasses = 0;
    while (!m_kUIGroups.GetAt(m_iCurUIGroup)->RegressFocus())
    {
        --m_iCurUIGroup;

        if (m_iCurUIGroup < 0)
        {
            // guaranteed to be >= 0 by first if
            m_iCurUIGroup = uiSize - 1; 
            ++uiNumPasses;
            if (uiNumPasses > 1)
            {
                m_iCurUIGroup = -1;
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiUIManager::EnterPressed(unsigned char ucValue)
{
    if (!m_bSearchWithMouse || (m_iCurUIGroup == -1) || 
        (m_kUIGroups.GetSize() == 0))
    {
        return;
    }
    
    if (!m_bVisible)
        return;

    if (ucValue & ISPRESSED)
    {
        m_bEnterIsPressed = true;
    }
    else
    {
        m_bEnterIsPressed = false;
    }

    m_kUIGroups.GetAt(m_iCurUIGroup)->EnterPressed(ucValue);
}
//---------------------------------------------------------------------------
void NiUIManager::LeftMouseButton(unsigned char ucValue)
{
    if (ucValue & WASPRESSED)
    {
        m_bSearchWithMouse = false;
    }
    else if (ucValue & WASRELEASED && m_bVisible)
    {
        m_bSearchWithMouse = true;
    }

    if ((!m_bMouseOverButton && (ucValue & WASPRESSED)) || 
        m_bEmitMouseSignal || (m_iCurUIGroup == -1) || 
        (m_kUIGroups.GetSize() == 0))
    {
        m_bEmitMouseSignal = true;
        m_kLeftMouseSignal.EmitSignal(ucValue);
        if (ucValue & WASRELEASED)
            m_bEmitMouseSignal = false;
    }
    else
    {
        if (!m_bVisible)
            return;

        m_bEmitMouseSignal = false;
        m_kUIGroups.GetAt(m_iCurUIGroup)->LeftMouseButton(ucValue);
    }
}
//---------------------------------------------------------------------------
void NiUIManager::UsePointerNavigation(unsigned char ucValue)
{
    if (ucValue & WASPRESSED)
    {
        m_bSearchWithMouse = false;
    }
    else if (ucValue & WASRELEASED)
    {
        m_bSearchWithMouse = true;
    }
}
//---------------------------------------------------------------------------
NiRect<float> NiUIManager::ConvertNSCToNDC(const NiRect<float>& kNSCRect)
{
    NiRect<float> kNDCRect = m_kSafeFrameInNDC;
    kNDCRect.m_left   += kNSCRect.m_left   * m_kSafeFrameInNDC.GetWidth();
    kNDCRect.m_right  -= (1.0f - kNSCRect.m_right) * 
        m_kSafeFrameInNDC.GetWidth();
    kNDCRect.m_top    += kNSCRect.m_top    * m_kSafeFrameInNDC.GetHeight();
    kNDCRect.m_bottom -= (1.0f - kNSCRect.m_bottom) * 
        m_kSafeFrameInNDC.GetHeight();
    return kNDCRect;
}
//---------------------------------------------------------------------------
NiRect<unsigned int> NiUIManager::ConvertNSCToPixels(
    const NiRect<float>& kNSCRect)
{
    NiRect<float> kNDCRect = ConvertNSCToNDC(kNSCRect);
    NiRect<unsigned int> kPixelRect;
    kPixelRect.m_left   = (unsigned int) (kNDCRect.m_left   * 
        m_fDisplayWidthInPixels);
    kPixelRect.m_right  = (unsigned int) (kNDCRect.m_right  * 
        m_fDisplayWidthInPixels);
    kPixelRect.m_top    = (unsigned int) (kNDCRect.m_top    * 
        m_fDisplayHeightInPixels);
    kPixelRect.m_bottom = (unsigned int) (kNDCRect.m_bottom * 
        m_fDisplayHeightInPixels);
    
    return kPixelRect;
}
//---------------------------------------------------------------------------
NiPoint2 NiUIManager::ConvertPixelDimensionsToNSCDimensions(
    unsigned int uiWidth, unsigned int uiHeight)
{
    NiPoint2 kPixelDimensions((float) uiWidth, (float) uiHeight);
    NiPoint2 kSafeFrameDimensionsInPixels;
    kSafeFrameDimensionsInPixels.x = 
        m_kSafeFrameInNDC.GetWidth() * m_fDisplayWidthInPixels;
    kSafeFrameDimensionsInPixels.y = 
        m_kSafeFrameInNDC.GetHeight() * m_fDisplayHeightInPixels;

    NiPoint2 kNSCDimensions;
    kNSCDimensions.x = kPixelDimensions.x / kSafeFrameDimensionsInPixels.x;
    kNSCDimensions.y = kPixelDimensions.y / kSafeFrameDimensionsInPixels.y;

    return kNSCDimensions;

}
//---------------------------------------------------------------------------
NiPoint2 NiUIManager::ConvertNDCDimensionsToNSCDimensions(
    float fWidth, float fHeight)
{
    NiPoint2 kNSCDimensions;
    kNSCDimensions.x = fWidth / m_kSafeFrameInNDC.GetWidth();
    kNSCDimensions.y = fHeight / m_kSafeFrameInNDC.GetHeight();

    return kNSCDimensions;

}
//---------------------------------------------------------------------------
