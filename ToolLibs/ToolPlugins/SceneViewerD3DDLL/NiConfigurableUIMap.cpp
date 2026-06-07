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

#include "stdafx.h"  
#include "NiConfigurableUIMap.h"

//---------------------------------------------------------------------------
NiConfigurableUIMap::NiConfigurableUIMap()
{
    m_pcName = NiAlloc(char, 256);
    NiStrcpy(m_pcName, 256, "New Map");
    m_pkKeyboardMap = NiNew NiSceneCommandList();
    m_pkMouseMap= NiNew NiSceneCommandList();
    m_pkExternalMap = NiNew NiSceneCommandQueue(50);
    m_uiPriorityListSizeKeyboard = 0;
    m_uiPriorityListSizeMouse = 0;
    m_iLowestPriorityValue = -99999;
    m_pkCommandFactory = NiNew NiSceneCommandFactory(this);
    m_aiLastPriorityLevelKeyboard = NULL;
    m_aiLastPriorityLevelMouse = NULL;
    m_pkKeyboard = NULL;
    m_pkMouse = NULL;
    m_fDeadZonePercentage = 0.25f;
}
//---------------------------------------------------------------------------
NiConfigurableUIMap::~NiConfigurableUIMap()
{
    NiDelete m_pkKeyboardMap;
    NiDelete m_pkMouseMap;
    NiDelete m_pkExternalMap;
    NiFree(m_aiLastPriorityLevelKeyboard);
    NiFree(m_aiLastPriorityLevelMouse);
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
NiSceneCommandList* NiConfigurableUIMap::GetKeyboardList()
{
    return m_pkKeyboardMap;
}
//---------------------------------------------------------------------------
NiSceneCommandList* NiConfigurableUIMap::GetMouseList()
{
    return m_pkMouseMap;
}

//---------------------------------------------------------------------------
NiSceneCommandQueue* NiConfigurableUIMap::CreateMouseCommands()
{
    if (m_pkMouse == NULL)
        return NULL;

    int iX = 0;
    int iY = 0;
    m_pkMouse->GetCurrentMouseLocation(iX,iY);
    int iPrevX = 0;
    int iPrevY = 0;
    m_pkMouse->GetPreviousMouseLocation(iPrevX, iPrevY);
    if (iPrevX == -1 || iPrevY == 0)
        iPrevX = iX;
    if (iPrevY == -1 || iPrevY == 0)
        iPrevY = iY;
    
    int iDx = iX - iPrevX;
    int iDy = iY - iPrevY;

    m_pkMouse->UpdatePreviousMousePosition(iX, iY);

    short sWheelDelta = m_pkMouse->GetWheelDelta();
    long lModifiers = m_pkMouse->GetModifiers();

    for (unsigned int ui = 0; ui <  m_uiPriorityListSizeMouse; ui++)
        m_aiLastPriorityLevelMouse[ui] = m_iLowestPriorityValue;

    NiTList<NiSceneCommandInfo*>* pkList = m_pkMouseMap->GetList();
    NiTListIterator pkIterator = pkList->GetHeadPos();
    NiSceneMouseCommandInfo* pkCommandInfo = NULL;
    NiSceneCommand* pkCommand = NULL;
    //bool bAddedCommands = false;

    NiSceneCommandQueue* pkQueue = NiNew NiSceneCommandQueue(25);
    while (pkIterator != NULL)
    {
        pkCommandInfo = (NiSceneMouseCommandInfo*)(NiSceneCommandInfo*)
            (pkList->Get(pkIterator));
        if (pkCommandInfo != NULL)
        {
            unsigned int uiMouseButtons = pkCommandInfo->m_lModifiers;// >> 4;
            bool bModifiersAreASubset = 
                ((lModifiers & pkCommandInfo->m_lModifiers) == 
                pkCommandInfo->m_lModifiers);
            if (MouseButtonsMatchStates(uiMouseButtons,
                pkCommandInfo->m_eDeviceState) && 
                (bModifiersAreASubset || 
                (pkCommandInfo->m_eDeviceState == NiDevice::DOUBLE_CLICKED)))
            {
                if ( GetMaxLevelForMouseButtons(uiMouseButtons) <= 
                    pkCommandInfo->m_iPriority)
                {
                    //NiOutputDebugString("MOUSE MATCH FOUND!\n");
                    SetLevelForMouseButtons(uiMouseButtons, 
                        pkCommandInfo->m_iPriority);
                    pkCommand = m_pkCommandFactory->CreateSceneMouseCommand(
                        pkCommandInfo, iDx, iDy, iX, iY, sWheelDelta);
                    if (pkCommand != NULL)
                        pkQueue->Add(pkCommand);
                    //bAddedCommands = true;

                }
            }
            
        }
        pkIterator = pkList->GetNextPos(pkIterator);
    }

    if (pkQueue->IsEmpty())
    {
        NiDelete pkQueue;
        return NULL;
    }
    else
    {
        return pkQueue;
    }
}
//---------------------------------------------------------------------------
NiSceneCommandQueue* NiConfigurableUIMap::CreateKeyboardCommands()
{
    if (m_pkKeyboard == NULL)
        return NULL;

    long lModifiers = m_pkKeyboard->GetModifiers();

    for (unsigned int ui = 0; ui <  m_uiPriorityListSizeKeyboard; ui++)
        m_aiLastPriorityLevelKeyboard[ui] = m_iLowestPriorityValue;

    NiTList<NiSceneCommandInfo*>* pkList = m_pkKeyboardMap->GetList();
    NiTListIterator pkIterator = pkList->GetHeadPos();
    NiSceneKeyboardCommandInfo* pkCommandInfo = NULL;
    NiSceneCommand* pkCommand = NULL;
    bool bAddedCommands = false;

    NiSceneCommandQueue* pkQueue = NiNew NiSceneCommandQueue(25);
    while (pkIterator != NULL)
    {
        pkCommandInfo = (NiSceneKeyboardCommandInfo*)(NiSceneCommandInfo*)
            (pkList->Get(pkIterator));
        if (pkCommandInfo != NULL)
        {
            bool bModifiersAreASubset = 
                ((lModifiers & pkCommandInfo->m_lModifiers) == 
                pkCommandInfo->m_lModifiers);
            if (m_pkKeyboard->StatesMatch(pkCommandInfo->m_eKey, 
                    pkCommandInfo->m_eDeviceState) && bModifiersAreASubset) 
            {
                if (m_aiLastPriorityLevelKeyboard[pkCommandInfo->m_eKey] <= 
                    pkCommandInfo->m_iPriority)
                {
                    m_aiLastPriorityLevelKeyboard[pkCommandInfo->m_eKey] =
                        pkCommandInfo->m_iPriority;
                    pkCommand = m_pkCommandFactory->CreateSceneKeyboardCommand(
                        pkCommandInfo);
                    if (pkCommand != NULL)
                        pkQueue->Add(pkCommand);
                    bAddedCommands = true;
                }
            }
        }
        pkIterator = pkList->GetNextPos(pkIterator);
    }

    if (pkQueue->IsEmpty())
    {
        NiDelete pkQueue;
        return NULL;
    }
    else
    {
        return pkQueue;
    }
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SetKeyboard(NiSceneKeyboard* pkKeyboard)
{
    if (pkKeyboard == NULL)
        return;
    NiUIMap::SetKeyboard(pkKeyboard);
    m_uiPriorityListSizeKeyboard = pkKeyboard->GetDeviceStateArraySize();
    NiFree(m_aiLastPriorityLevelKeyboard);
    m_aiLastPriorityLevelKeyboard = NiAlloc(int, 
        m_uiPriorityListSizeKeyboard);
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SetMouse(NiSceneMouse* pkMouse)
{
    if (pkMouse == NULL)
        return;
    
    NiUIMap::SetMouse(pkMouse);
    m_uiPriorityListSizeMouse = pkMouse->GetDeviceStateArraySize();
    NiFree(m_aiLastPriorityLevelMouse);
    m_aiLastPriorityLevelMouse = NiAlloc(int, m_uiPriorityListSizeMouse);
}
//---------------------------------------------------------------------------
NiSceneCommandFactoryPtr NiConfigurableUIMap::GetCommandFactory()
{
    return m_pkCommandFactory;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SetCommandFactory(
    NiSceneCommandFactoryPtr spFactory)
{
    if (spFactory != NULL)
    {
        m_pkCommandFactory = spFactory;
        m_pkCommandFactory->SetParent(this);
    }
}
//---------------------------------------------------------------------------
bool NiConfigurableUIMap::MouseButtonsMatchStates(unsigned int uiMouseButtons,
    NiDevice::DeviceState eState)
{
    bool bAllDown = true;

    if (uiMouseButtons == NiSceneMouse::MMOD_NONE)
    {
        bAllDown = m_pkMouse->StatesMatch(NiSceneMouse::SCENEMOUSE_NOBUTTON,
            eState);
    }
    
    if ((uiMouseButtons & NiSceneMouse::MMOD_LEFT) == 
        NiSceneMouse::MMOD_LEFT)
    {
        bAllDown = bAllDown && 
            m_pkMouse->StatesMatch(NiSceneMouse::SCENEMOUSE_LBUTTON, eState);
    }
    
    if ((uiMouseButtons & NiSceneMouse::MMOD_MIDDLE) == 
        NiSceneMouse::MMOD_MIDDLE)
    {
        bAllDown = bAllDown && 
            m_pkMouse->StatesMatch(NiSceneMouse::SCENEMOUSE_MBUTTON, eState);
    }

    if ((uiMouseButtons & NiSceneMouse::MMOD_RIGHT) == 
        NiSceneMouse::MMOD_RIGHT)
    {
        bAllDown = bAllDown && 
            m_pkMouse->StatesMatch(NiSceneMouse::SCENEMOUSE_RBUTTON, eState);
    }

    if ((uiMouseButtons & NiSceneMouse::MMOD_WHEEL) == 
        NiSceneMouse::MMOD_WHEEL)
    {
        bAllDown = bAllDown && 
            m_pkMouse->StatesMatch(NiSceneMouse::SCENEMOUSE_WHEEL, eState);
    }
    return bAllDown;
}
//---------------------------------------------------------------------------
int NiConfigurableUIMap::GetMaxLevelForMouseButtons(
    unsigned int uiMouseButtons)
{
    int iMax = m_iLowestPriorityValue;
    
    if (uiMouseButtons == NiSceneMouse::MMOD_NONE)
    {
        iMax = NiMax(iMax, 
            m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_NOBUTTON]);
    }
    
    if ((uiMouseButtons & NiSceneMouse::MMOD_LEFT) == 
        NiSceneMouse::MMOD_LEFT)
    {
        iMax = NiMax(iMax, 
            m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_LBUTTON]);
    }
    
    if ((uiMouseButtons & NiSceneMouse::MMOD_MIDDLE) == 
        NiSceneMouse::MMOD_MIDDLE)
    {
        iMax = NiMax(iMax, 
            m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_MBUTTON]);
    }

    if ((uiMouseButtons & NiSceneMouse::MMOD_RIGHT) == 
        NiSceneMouse::MMOD_RIGHT)
    {
        iMax = NiMax(iMax, 
            m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_RBUTTON]);
    }

    if ((uiMouseButtons & NiSceneMouse::MMOD_WHEEL) == 
        NiSceneMouse::MMOD_WHEEL)
    {
        iMax = NiMax(iMax, 
            m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_WHEEL]);
    }    

    return iMax;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SetLevelForMouseButtons(unsigned int uiMouseButtons, 
    int iLevel)
{
    if (uiMouseButtons == NiSceneMouse::MMOD_NONE)
        m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_NOBUTTON] = iLevel;
        
    if ((uiMouseButtons & NiSceneMouse::MMOD_LEFT) == 
        NiSceneMouse::MMOD_LEFT)
        m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_LBUTTON] = iLevel;
    
    if ((uiMouseButtons & NiSceneMouse::MMOD_MIDDLE) == 
        NiSceneMouse::MMOD_MIDDLE)
        m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_MBUTTON] = iLevel;

    if ((uiMouseButtons & NiSceneMouse::MMOD_RIGHT) == 
        NiSceneMouse::MMOD_RIGHT)
        m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_RBUTTON] = iLevel;

    if ((uiMouseButtons & NiSceneMouse::MMOD_WHEEL) == 
        NiSceneMouse::MMOD_WHEEL)
        m_aiLastPriorityLevelMouse[NiSceneMouse::SCENEMOUSE_WHEEL] = iLevel;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SetDeadzone(float fDeadZonePercentage)
{
    if (fDeadZonePercentage > 0 && fDeadZonePercentage < 1)
        m_fDeadZonePercentage = fDeadZonePercentage;
}
//---------------------------------------------------------------------------
float NiConfigurableUIMap::GetDeadzone()
{
    return m_fDeadZonePercentage;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SetName(const char* pcName)
{
    if (pcName != NULL)
    {
        NiFree(m_pcName);
        size_t stLength = strlen(pcName)+1;
        m_pcName = NiAlloc(char, stLength);
        m_pcName = NiStrcpy(m_pcName, stLength, pcName);
    }
}
//---------------------------------------------------------------------------
const char* NiConfigurableUIMap::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
NiSceneCommandQueue* NiConfigurableUIMap::CreateExternalCommands()
{
    if (m_pkExternalMap != NULL)
    {
        NiSceneCommandQueue* pkQueue = NiNew NiSceneCommandQueue(5);
        if (m_pkExternalMap->GetSize() != 0)
        {
            while (m_pkExternalMap->GetSize() != 0)
            {
                pkQueue->Add(m_pkExternalMap->Remove());
            }
        }
        return pkQueue;
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiConfigurableUIMap::SendCommand(NiSceneCommand* pkCommand)
{
    if (m_pkExternalMap != NULL)
    {
        m_pkExternalMap->Add(pkCommand);
    }
}
//---------------------------------------------------------------------------
        
