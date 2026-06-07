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

#ifndef NIMAXSCRIPTUTILS_H
#define NIMAXSCRIPTUTILS_H

#include "MiUtils.h"

BOOL MAXScriptEvaluate(const char *command);
void MAXScriptAddNoteKey(DefNoteTrack* pkTrack, float fTime);

//---------------------------------------------------------------------------
class NiMAXScriptUtils
{
public:

    NiMAXScriptUtils();
    ~NiMAXScriptUtils();

    static void Init();
    static void Shutdown();

    static Value* GetShadowTechniqueNames();

private:

    char* 
        m_apShadowTechniqueNames[NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES];
};

#endif
