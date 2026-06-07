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
#include "NiSceneCommandFactory.h"

//---------------------------------------------------------------------------
NiSceneCommandFactory::NiSceneCommandFactory(NiUIMap* pkParent)
{
    m_pkParent = pkParent;
    m_fScaleFactorGamePad = 1.0f;
    m_fScaleFactorMouse = 1.0f;
    m_fScaleFactorKeyboard = 1.0f;
    m_kSpecialCaseClassID = 0;
}
//---------------------------------------------------------------------------
void NiSceneCommandFactory::SetParent(NiUIMap* pkParent)
{
    m_pkParent = pkParent;
}
//---------------------------------------------------------------------------
NiSceneCommand* NiSceneCommandFactory::CreateSceneKeyboardCommand(
    NiSceneKeyboardCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
        return NULL;
    pkInfo->m_fSystemDependentScaleFactor = m_fScaleFactorKeyboard;
    pkInfo = (NiSceneKeyboardCommandInfo*)pkInfo->Clone();
    return CreateSceneCommand(pkInfo);
}
//---------------------------------------------------------------------------
NiSceneCommand* NiSceneCommandFactory::CreateSceneMouseCommand(
    NiSceneMouseCommandInfo* pkInfo, int iDx, int iDy, int iX, int iY,
    short sWheelDelta)
{
    // The mouse handles input a little differently, we use the 
    // DX and DY values to scale the parameter values

    if (pkInfo == NULL)
        return NULL;

    pkInfo = (NiSceneMouseCommandInfo*)pkInfo->Clone();
    
    if (pkInfo->m_iWheelDeltaAffectedParameterIndex != -1)
    {
        float fDx = pkInfo->m_kParamList.GetAt(
            pkInfo->m_iWheelDeltaAffectedParameterIndex);
        fDx *=(float)sWheelDelta;
        pkInfo->m_kParamList.SetAt(pkInfo->m_iWheelDeltaAffectedParameterIndex,
            fDx);
    }
    if (pkInfo->m_iDxAffectedParameterIndex != -1)
    {
        float fDx = pkInfo->m_kParamList.GetAt(
            pkInfo->m_iDxAffectedParameterIndex);
        fDx *=(float)iDx;
        pkInfo->m_kParamList.SetAt(pkInfo->m_iDxAffectedParameterIndex, fDx);
    }
    if (pkInfo->m_iDyAffectedParameterIndex != -1)
    {
        float fDy = pkInfo->m_kParamList.GetAt(
            pkInfo->m_iDyAffectedParameterIndex);
        fDy *= (float)iDy;
        pkInfo->m_kParamList.SetAt(pkInfo->m_iDyAffectedParameterIndex,
            fDy);
    }
    if (pkInfo->m_iXAffectedParameterIndex != -1)
    {
        float fX = pkInfo->m_kParamList.GetAt(
            pkInfo->m_iXAffectedParameterIndex);
        fX *=(float)iX;
        pkInfo->m_kParamList.SetAt(pkInfo->m_iXAffectedParameterIndex,
            fX);
    }
    if (pkInfo->m_iYAffectedParameterIndex != -1)
    {
        float fY = pkInfo->m_kParamList.GetAt(
            pkInfo->m_iYAffectedParameterIndex);
        fY *= (float)iY;
        pkInfo->m_kParamList.SetAt(pkInfo->m_iYAffectedParameterIndex,
            fY);
    }

    pkInfo->m_fSystemDependentScaleFactor = m_fScaleFactorMouse;
    return CreateSceneCommand(pkInfo);
}
//---------------------------------------------------------------------------
NiSceneCommand* NiSceneCommandFactory::CreateSceneCommand(
    NiSceneCommandInfo* pkInfo)
{
    NiSceneCommand* pkCommand = NULL;
    NiCommandID kClassID = (NiCommandID)pkInfo->m_uiClassID;
    
    if (m_kSpecialCaseClassID == 0)
    {
        m_kSpecialCaseClassID = 
            NiSceneCommandRegistry::GetCommandID("SCALE_DEVICE_DELTA");
    }
    
    if (kClassID == m_kSpecialCaseClassID)
    {
        if (pkInfo->m_kParamList.GetSize() == 1)
            AddToScaleFactor(pkInfo->m_eType, pkInfo->m_kParamList.GetAt(0));
    }
    else
    {
        pkCommand = NiSceneCommandRegistry::CreateCommand(kClassID, pkInfo);
    }
 
    NiDelete pkInfo;
    return pkCommand;
}
//---------------------------------------------------------------------------
void NiSceneCommandFactory::AddToScaleFactor(
    NiSceneCommandInfo::UserInterfaceType m_eType, float fScale)
{
    if (m_eType == NiSceneCommandInfo::KEYBOARD)
    {   
        fScale += m_fScaleFactorKeyboard;
        if (fScale > 0)
            m_fScaleFactorKeyboard = fScale;
    }
    else if (m_eType == NiSceneCommandInfo::MOUSE)
    {
        fScale += m_fScaleFactorMouse;
        if (fScale > 0)
            m_fScaleFactorMouse = fScale;
    }
    else if (m_eType == NiSceneCommandInfo::GAMEPAD)
    {
        fScale += m_fScaleFactorGamePad;
        if (fScale > 0)
            m_fScaleFactorGamePad = fScale;
    }
}
//---------------------------------------------------------------------------
