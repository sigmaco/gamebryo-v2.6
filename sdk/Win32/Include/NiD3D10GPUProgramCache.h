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

#ifndef NID3D10GPUPROGRAMCACHE_H
#define NID3D10GPUPROGRAMCACHE_H

#include "NiD3D10RendererLibType.h"

#include <NiGPUProgramCache.h>

class NID3D10RENDERER_ENTRY NiD3D10GPUProgramCache : public NiGPUProgramCache
{
public:
    NiD3D10GPUProgramCache(unsigned int uiVersion, const char* pcWorkingDir, 
        NiGPUProgram::ProgramType eType, const NiFixedString& kShaderProfile, 
        const char* pcMaterialIdentifier, bool bAutoWriteToDisk, 
        bool bWriteDebugHLSLFile, bool Locked, bool bLoad);

    virtual NiGPUProgram* GenerateProgram(const char* pcName, 
        const char* pcProgramText, 
        NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms);

    virtual const char* GetPlatformSpecificCodeID() const;
    virtual const char* GetPlatform() const;

protected:

    virtual bool SaveGPUProgram(NiBinaryStream& kStream, 
        NiGPUProgram* pkProgram);
    virtual bool LoadGPUProgram(NiBinaryStream& kStream, 
        const NiFixedString& kName, NiGPUProgramPtr& spProgram, bool bSkip);
    virtual bool SaveDebugHLSLFile(const char* pcFilename, 
        const char* pcProgramText);

    NiGPUProgram::ProgramType m_eType;
    bool m_bWriteDebugHLSLFile;
    NiFixedString m_kMaterialIdentifier;
};

#endif  //#ifndef NID3D10GPUPROGRAMCACHE_H
