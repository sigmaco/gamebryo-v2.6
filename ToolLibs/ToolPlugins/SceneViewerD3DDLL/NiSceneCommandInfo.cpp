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

#include "NiSceneCommandInfo.h"
//---------------------------------------------------------------------------
NiSceneCommandInfo::NiSceneCommandInfo()
{
    m_lModifiers = 0;
    m_eDeviceState = NiDevice::DEVICE_ERROR;
    m_eType = UI_TYPE_NONE;
    m_fSystemDependentScaleFactor = 1.0f;
    m_iPriority = -1;
    m_uiClassID = 0;
    m_pcName = NULL;
}
//---------------------------------------------------------------------------
NiSceneCommandInfo::~NiSceneCommandInfo()
{
    m_kParamList.RemoveAll();
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
NiSceneCommandInfo* NiSceneCommandInfo::Clone()
{
    return NULL;
}
//---------------------------------------------------------------------------
void NiSceneCommandInfo::SetName(char* pcName)
{
    if (pcName == NULL)
        return;
    NiFree(m_pcName);
    m_pcName = NiAlloc(char, 256);
    NiStrcpy(m_pcName, 256, pcName);
}
//---------------------------------------------------------------------------

