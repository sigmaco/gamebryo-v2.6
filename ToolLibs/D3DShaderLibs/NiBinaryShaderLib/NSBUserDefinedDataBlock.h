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

#ifndef NSBUSERDEFINEDDATABLOCK_H
#define NSBUSERDEFINEDDATABLOCK_H

#include "NSBConstantMap.h"
#include "NSBUtility.h"

class NiShaderConstantMap;
class NiShaderDesc;

class NIBINARYSHADERLIB_ENTRY NSBUserDefinedDataBlock :
    public NSBConstantMap
{
public:
    inline NSBUserDefinedDataBlock(const char* pcName);
    inline virtual ~NSBUserDefinedDataBlock();

    inline const char* GetName() const;
    inline void SetName(const char* pcName);

    virtual bool AddEntry(char* pcKey, unsigned int uiFlags, 
        unsigned int uiSize, unsigned int uiStride, void* pvSource, 
        bool bCopyData = true);

    virtual NiPlatformShaderConstantMap* GetPixelConstantMap(
        NiShaderDesc* pkShaderDesc);
    virtual NiPlatformShaderConstantMap* GetVertexConstantMap(
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

#include "NSBUserDefinedDataBlock.inl"

#endif  //NSBUSERDEFINEDDATABLOCK_H
