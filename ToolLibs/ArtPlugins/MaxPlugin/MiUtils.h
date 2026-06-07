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

#ifndef MIUTILS_H
#define MIUTILS_H

#include "NiMAX.h"

void MessageBox(int iString1, int iString2);
TCHAR* GetString(int iID);
void HandleError(int iStatus); 

void GetCfgFilename(Interface *pIntf, TCHAR *pFileName, 
    unsigned int uiFileNameSize);
void GetUserCfgFilename(Interface *pIntf, TCHAR *pFileName, 
    unsigned int uiFileNameSize);
bool ToBool(int i);

Control * GetControlFromAnimatableName(Animatable *anim, char *name);

#endif
