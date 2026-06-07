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
#include "NiSceneKeyboardCommandInfo.h"
//---------------------------------------------------------------------------

NiSceneKeyboardCommandInfo::NiSceneKeyboardCommandInfo()
{
    NiSceneCommandInfo::NiSceneCommandInfo();
    m_eKey = NiSceneKeyboard::KEY_NOKEY;
    m_eType = KEYBOARD;
}
//---------------------------------------------------------------------------
NiSceneKeyboardCommandInfo::NiSceneKeyboardCommandInfo(
    NiSceneCommandInfo* pkOldInfo)
{
    m_uiClassID = pkOldInfo->m_uiClassID;
    m_eDeviceState = pkOldInfo->m_eDeviceState;
    m_fSystemDependentScaleFactor = pkOldInfo->m_fSystemDependentScaleFactor;
    m_iPriority = pkOldInfo->m_iPriority;
    m_lModifiers = pkOldInfo->m_lModifiers;
    if (pkOldInfo->m_pcName != NULL)
    {
        size_t stLen = strlen(pkOldInfo->m_pcName) + 1;
        m_pcName = NiAlloc(char, stLen);
        NiStrcpy(m_pcName, stLen, pkOldInfo->m_pcName);
    }
    if (pkOldInfo->m_kParamList.GetSize() != 0)
    {
        m_kParamList.SetSize(pkOldInfo->m_kParamList.GetSize());
        for (unsigned int ui = 0; ui < pkOldInfo->m_kParamList.GetSize(); ui++)
            m_kParamList.SetAt( ui, pkOldInfo->m_kParamList.GetAt(ui));
    }
    m_eKey = NiSceneKeyboard::KEY_NOKEY;
    m_eType = KEYBOARD;
}
//---------------------------------------------------------------------------
NiSceneCommandInfo* NiSceneKeyboardCommandInfo::Clone()
{
    NiSceneKeyboardCommandInfo* pkInfo = 
        NiNew NiSceneKeyboardCommandInfo();
    pkInfo->m_eKey = m_eKey;
    pkInfo->m_lModifiers = m_lModifiers;
    pkInfo->m_eDeviceState = m_eDeviceState;
    pkInfo->m_uiClassID = m_uiClassID;
    pkInfo->m_fSystemDependentScaleFactor = 
        m_fSystemDependentScaleFactor;
    for (unsigned int i = 0; i < m_kParamList.GetSize(); i++)
    {
        pkInfo->m_kParamList.Add(m_kParamList.GetAt(i));
    }
    pkInfo->m_iPriority = m_iPriority;
    pkInfo->SetName(m_pcName);
    return pkInfo;
}
//---------------------------------------------------------------------------

