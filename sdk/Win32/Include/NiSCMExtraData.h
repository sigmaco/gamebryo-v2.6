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

#ifndef NISCMEXTRADATA_H
#define NISCMEXTRADATA_H

#include "NiExtraData.h"
#include "NiGPUProgram.h"

class NIMAIN_ENTRY NiSCMExtraData : public NiExtraData
{
    NiDeclareRTTI;

public:
    class SCMExtraDataEntry : public NiMemObject
    {
    public:
        unsigned int m_uiIndex;
        NiExtraData* m_pkExtraData;
    };

    NiSCMExtraData(const NiFixedString& kName, unsigned int uiNumVertexEntries,
        unsigned int uiNumGeometryEntries, unsigned int uiNumPixelEntries);
    virtual ~NiSCMExtraData();

    virtual bool IsStreamable() const;
    virtual bool IsCloneable() const;

    inline void AddEntry(unsigned int uiConstantIndex, unsigned int uiPass,
        NiGPUProgram::ProgramType eType, NiExtraData* pkExtraData,
        bool bGlobal = false);
    inline NiExtraData* GetNextEntry(unsigned int uiConstantIndex, 
        unsigned int uiPass, NiGPUProgram::ProgramType eType, 
        bool bGlobal = false);
    inline void Reset();

protected:
    static const unsigned int m_uiPassShift;

    // Using matched arrays here. 0 holds vertex info and 1 holds pixel info.
    unsigned int m_auiNumEntries[NiGPUProgram::PROGRAM_MAX];
    unsigned int m_auiCurrentEntry[NiGPUProgram::PROGRAM_MAX];
    SCMExtraDataEntry* m_apkEntries[NiGPUProgram::PROGRAM_MAX];
};

NiSmartPointer(NiSCMExtraData);

#include "NiSCMExtraData.inl"

#endif

