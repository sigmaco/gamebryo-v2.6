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

#ifndef NSBCONSTANTMAP_H
#define NSBCONSTANTMAP_H

#include <NiBinaryStream.h>
#include <NiTPointerList.h>

#include <NiShader.h>
#include <NiShaderConstantMapEntry.h>

#include "NiBinaryShaderLibLibType.h"
#include "NSBUtility.h"

class NiShaderConstantMap;
class NiShaderDesc;

class NIBINARYSHADERLIB_ENTRY NSBConstantMap : public NiRefObject
{
public:
    NSBConstantMap();
    virtual ~NSBConstantMap();

    // Usage
    inline unsigned int GetProgramType();
    inline void SetProgramType(unsigned int uiUsage);

    // Entry
    class NSBCM_Entry : public NiShaderConstantMapEntry
    {
    public:
        NSBCM_Entry();
        ~NSBCM_Entry();

        // *** begin Emergent internal use only
        bool SaveBinary(NiBinaryStream& kStream);
        bool LoadBinary(NiBinaryStream& kStream);

        unsigned int GetComponentSize() const;
        // *** end Emergent internal use only
    };

    unsigned int GetGlobalEntryCount();
    unsigned int GetPlatformEntryCount(NiShader::Platform ePlatform);
    unsigned int GetTotalEntryCount();

    virtual bool AddEntry(const char* pcKey, unsigned int uiFlags, 
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        const char* pcVariableName, unsigned int uiSize = 0, 
        unsigned int uiStride = 0, void* pvSource = 0, 
        bool bCopyData = false);
    virtual bool AddPlatformSpecificEntry(unsigned int uiPlatformFlags, 
        const char* pcKey, unsigned int uiFlags, unsigned int uiExtra, 
        unsigned int uiReg, unsigned int uiCount, const char* pcVariableName, 
        unsigned int uiSize = 0, unsigned int uiStride = 0, 
        void* pvSource = 0, bool bCopyData = false);

    NSBCM_Entry* GetFirstEntry();
    NSBCM_Entry* GetNextEntry();

    NSBCM_Entry* GetFirstPlatformEntry(NiShader::Platform ePlatform);
    NSBCM_Entry* GetNextPlatformEntry(NiShader::Platform ePlatform);

    NSBCM_Entry* GetEntryByKey(const char* pcKey);
    unsigned int GetEntryIndexByKey(const char* pcKey);

    NSBCM_Entry* GetPlatformEntryByKey(NiShader::Platform ePlatform, 
        const char* pcKey);
    unsigned int GetPlatformEntryIndexByKey(NiShader::Platform ePlatform, 
        const char* pcKey);

    virtual NiPlatformShaderConstantMap* GetVertexConstantMap(
        NiShaderDesc* pkShaderDesc);
    virtual NiPlatformShaderConstantMap* GetGeometryConstantMap(
        NiShaderDesc* pkShaderDesc);
    virtual NiPlatformShaderConstantMap* GetPixelConstantMap(
        NiShaderDesc* pkShaderDesc);

    // *** begin Emergent internal use only
    virtual bool SaveBinary(NiBinaryStream& kStream);
    virtual bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    virtual void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    NSBCM_Entry* CreateEntry(const char* pcKey, 
        unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
        unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
        unsigned int uiStride, void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_DX9(const char* pcKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        const char* pcVariableName, unsigned int uiSize, unsigned int uiStride,
        void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_Xenon(const char* pcKey,
        unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
        unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
        unsigned int uiStride, void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_PS3(const char* pcKey, unsigned int uiFlags,
        unsigned int uiExtra, unsigned int uiReg, unsigned int uiCount,
        const char* pcVariableName, unsigned int uiSize,
        unsigned int uiStride, void* pvSource, bool bCopyData);
    bool AddPlatformSpecificEntry_D3D10(const char* pcKey,
        unsigned int uiFlags, unsigned int uiExtra, unsigned int uiReg,
        unsigned int uiCount, const char* pcVariableName, unsigned int uiSize,
        unsigned int uiStride, void* pvSource, bool bCopyData);

    bool GetPlatformListPointers(NiShader::Platform ePlatform,
        NiTListIterator*& pkIter, 
        NiTPointerList<NSBCM_Entry*>*& pkEntryList);

    bool ProcessMapEntry(NiShaderDesc* pkShaderDesc, NSBCM_Entry* pkEntry, 
        NiPlatformShaderConstantMap* pkSCMV);

    virtual bool SaveBinaryEntries(NiBinaryStream& kStream);
    virtual bool LoadBinaryEntries(NiBinaryStream& kStream);

    unsigned int m_uiProgramType;
    NiTListIterator m_kListIter;
    NiTPointerList<NSBCM_Entry*> m_kEntryList;
    NiTListIterator m_kListIter_DX9;
    NiTPointerList<NSBCM_Entry*> m_kEntryList_DX9;
    NiTListIterator m_kListIter_Xenon;
    NiTPointerList<NSBCM_Entry*> m_kEntryList_Xenon;
    NiTListIterator m_kListIter_PS3;
    NiTPointerList<NSBCM_Entry*> m_kEntryList_PS3;
    NiTListIterator m_kListIter_D3D10;
    NiTPointerList<NSBCM_Entry*> m_kEntryList_D3D10;
};

NiSmartPointer(NSBConstantMap);

#include "NSBConstantMap.inl"

#endif  //NSBCONSTANTMAP_H
