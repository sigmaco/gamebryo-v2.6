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

#ifndef NSBD3D10USERDEFINEDDATABLOCK_H
#define NSBD3D10USERDEFINEDDATABLOCK_H

#include "NSBD3D10ConstantMap.h"
#include "NSBD3D10Utility.h"

class NiD3D10ShaderConstantMap;
class NiShaderDesc;

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10UserDefinedDataBlock :
    public NSBD3D10ConstantMap
{
public:
    NSBD3D10UserDefinedDataBlock(const char* pcName);
    virtual ~NSBD3D10UserDefinedDataBlock();

    inline const char* GetName() inline const;
    inline void SetName(const char* pcName);

    virtual bool AddEntry(char* pcKey, unsigned int uiFlags, 
        unsigned int uiSize, unsigned int uiStride, void* pvSource, 
        bool bCopyData = true);

    virtual NiD3D10ShaderConstantMap* GetPixelConstantMap(
        NiShaderDesc* pkShaderDesc);
    virtual NiD3D10ShaderConstantMap* GetVertexConstantMap(
        NiShaderDesc* pkShaderDesc);

    // *** begin Emergent internal use only
    virtual bool SaveBinary(NiBinaryStream& kStream);
    virtual bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    virtual void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    virtual bool SaveBinaryEntries(NiBinaryStream& kStream);
    virtual bool LoadBinaryEntries(NiBinaryStream& kStream);

    char* m_pcName;
};

#include "NSBD3D10UserDefinedDataBlock.inl"

#endif  //NSBD3D10USERDEFINEDDATABLOCK_H
