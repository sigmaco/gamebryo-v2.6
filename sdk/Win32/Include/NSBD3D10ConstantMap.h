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

#ifndef NSBD3D10CONSTANTMAP_H
#define NSBD3D10CONSTANTMAP_H

#include <NiBinaryStream.h>
#include <NiTPointerList.h>

#include <NiShader.h>
#include <NiShaderConstantMapEntry.h>

#include "NiD3D10BinaryShaderLibLibType.h"

class NiD3D10ShaderConstantMap;
class NiShaderDesc;

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10ConstantMap : public NiRefObject
{
public:
    NSBD3D10ConstantMap();
    virtual ~NSBD3D10ConstantMap();

    // Usage
    inline unsigned int GetProgramType();
    inline void SetProgramType(unsigned int uiUsage);

    // Entry
    class NSBD3D10CM_Entry : public NiShaderConstantMapEntry
    {
    public:
        NSBD3D10CM_Entry();
        ~NSBD3D10CM_Entry();

        // *** begin Emergent internal use only
        bool SaveBinary(NiBinaryStream& kStream);
        bool LoadBinary(NiBinaryStream& kStream);
        // *** end Emergent internal use only
    };

    unsigned int GetGlobalEntryCount();
    unsigned int GetPlatformEntryCount(NiShader::Platform ePlatform);
    unsigned int GetTotalEntryCount();

    virtual bool AddEntry(char* pcKey, unsigned int uiFlags, 
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        char* pcVariableName, unsigned int uiSize = 0, 
        unsigned int uiStride = 0, void* pvSource = 0, 
        bool bCopyData = false);
    virtual bool AddPlatformSpecificEntry(unsigned int uiPlatformFlags, 
        char* pcKey, unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiReg, unsigned int uiCount, char* pcVariableName, 
        unsigned int uiSize = 0, unsigned int uiStride = 0, 
        void* pvSource = 0, bool bCopyData = false);

    NSBD3D10CM_Entry* GetFirstEntry();
    NSBD3D10CM_Entry* GetNextEntry();

    NSBD3D10CM_Entry* GetFirstPlatformEntry(NiShader::Platform ePlatform);
    NSBD3D10CM_Entry* GetNextPlatformEntry(NiShader::Platform ePlatform);

    NSBD3D10CM_Entry* GetEntryByKey(const char* pcKey);
    unsigned int GetEntryIndexByKey(const char* pcKey);

    NSBD3D10CM_Entry* GetPlatformEntryByKey(NiShader::Platform ePlatform, 
        const char* pcKey);
    unsigned int GetPlatformEntryIndexByKey(NiShader::Platform ePlatform, 
        const char* pcKey);

    virtual NiD3D10ShaderConstantMap* GetVertexConstantMap(
        NiShaderDesc* pkShaderDesc);
    virtual NiD3D10ShaderConstantMap* GetGeometryConstantMap(
        NiShaderDesc* pkShaderDesc);
    virtual NiD3D10ShaderConstantMap* GetPixelConstantMap(
        NiShaderDesc* pkShaderDesc);

    // *** begin Emergent internal use only
    virtual bool SaveBinary(NiBinaryStream& kStream);
    virtual bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    virtual void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NSBD3D10CM_Entry* CreateEntry(char* pcKey, 
        unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
        unsigned int uiCount, char* pcVariableName, unsigned int uiSize,
        unsigned int uiStride, void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_DX9(char* pcKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        char* pcVariableName, unsigned int uiSize, unsigned int uiStride,
        void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_Xenon(char* pcKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        char* pcVariableName, unsigned int uiSize, unsigned int uiStride,
        void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_PS3(char* pcKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        char* pcVariableName, unsigned int uiSize, unsigned int uiStride,
        void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_D3D10(char* pcKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        char* pcVariableName, unsigned int uiSize, unsigned int uiStride,
        void* pvSource, bool bCopyData);

    bool GetPlatformListPointers(NiShader::Platform ePlatform,
        NiTListIterator*& pkIter, 
        NiTPointerList<NSBD3D10CM_Entry*>*& pkEntryList);

    bool ProcessMapEntry(NiShaderDesc* pkShaderDesc,
        NSBD3D10CM_Entry* pkEntry, NiD3D10ShaderConstantMap* pkSCMV);

    virtual bool SaveBinaryEntries(NiBinaryStream& kStream);
    virtual bool LoadBinaryEntries(NiBinaryStream& kStream);

    unsigned int m_uiProgramType;
    NiTListIterator m_kListIter;
    NiTPointerList<NSBD3D10CM_Entry*> m_kEntryList;
    NiTListIterator m_kListIter_DX9;
    NiTPointerList<NSBD3D10CM_Entry*> m_kEntryList_DX9;
    NiTListIterator m_kListIter_Xenon;
    NiTPointerList<NSBD3D10CM_Entry*> m_kEntryList_Xenon;
    NiTListIterator m_kListIter_PS3;
    NiTPointerList<NSBD3D10CM_Entry*> m_kEntryList_PS3;
    NiTListIterator m_kListIter_D3D10;
    NiTPointerList<NSBD3D10CM_Entry*> m_kEntryList_D3D10;
};

NiSmartPointer(NSBD3D10ConstantMap);

#include "NSBD3D10ConstantMap.inl"

#endif  //NSBD3D10CONSTANTMAP_H
