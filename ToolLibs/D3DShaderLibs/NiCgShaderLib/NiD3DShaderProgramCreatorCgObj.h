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

#ifndef NID3DSHADERPROGRAMCREATORCGOBJ_H
#define NID3DSHADERPROGRAMCREATORCGOBJ_H

#include "NiCgShaderLibType.h"

#include "NiD3DShaderProgramCreatorCg.h"

class NICGSHADERLIB_ENTRY NiD3DShaderProgramCreatorCgObj : 
    public NiD3DShaderProgramCreatorCg
{
public:
    virtual ~NiD3DShaderProgramCreatorCgObj();

    // *** begin Emergent internal use only ***

    static void _SDMInit();
    static void _SDMShutdown();

    static NiD3DShaderProgramCreatorCgObj* GetInstance();
    static void Shutdown();

    // *** end Emergent internal use only ***

protected:
    NiD3DShaderProgramCreatorCgObj();

    virtual bool LoadShaderCodeFromFile(const char* pcFileName, 
        const char* pcEntryPoint, CGprofile eProfile, void*& pvCode, 
        unsigned int& uiCodeSize, void*& pvConstantData, 
        unsigned int& uiConstantSize, CGprogram& kProgram, bool bRecoverable);
    virtual bool LoadShaderCodeFromBuffer(const void* pvBuffer, 
        const char* pcEntryPoint, CGprofile eProfile, void*& pvCode, 
        unsigned int& uiCodeSize, void*& pvConstantData, 
        unsigned int& uiConstantSize, CGprogram& kProgram, bool bRecoverable);

    static NiD3DShaderProgramCreatorCgObj* ms_pkCreator;
};

#include "NiD3DShaderProgramCreatorCgObj.inl"

#endif  //NID3DSHADERPROGRAMCREATORCGOBJ_H
