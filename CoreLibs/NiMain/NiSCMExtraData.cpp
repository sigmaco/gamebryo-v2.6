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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiSCMExtraData.h"

NiImplementRTTI(NiSCMExtraData, NiExtraData);

const unsigned int NiSCMExtraData::m_uiPassShift = 16;

//---------------------------------------------------------------------------
NiSCMExtraData::NiSCMExtraData(const NiFixedString& kName, 
    unsigned int uiNumVertexEntries, unsigned int uiGeometryEntries, 
    unsigned int uiNumPixelEntries)
{
    SetName(kName);
    
    m_auiNumEntries[NiGPUProgram::PROGRAM_VERTEX] = uiNumVertexEntries;
    m_auiCurrentEntry[NiGPUProgram::PROGRAM_VERTEX] = 0;
    if (uiNumVertexEntries)
    {
        m_apkEntries[NiGPUProgram::PROGRAM_VERTEX] = 
            NiAlloc(SCMExtraDataEntry, uiNumVertexEntries);
    }
    else
    {
        m_apkEntries[NiGPUProgram::PROGRAM_VERTEX] = 0;
    }
    
    m_auiNumEntries[NiGPUProgram::PROGRAM_GEOMETRY] = uiGeometryEntries;
    m_auiCurrentEntry[NiGPUProgram::PROGRAM_GEOMETRY] = 0;
    if (uiNumVertexEntries)
    {
        m_apkEntries[NiGPUProgram::PROGRAM_GEOMETRY] = 
            NiAlloc(SCMExtraDataEntry, uiNumVertexEntries);
    }
    else 
    {
        m_apkEntries[NiGPUProgram::PROGRAM_GEOMETRY] = 0;
    }

    m_auiNumEntries[NiGPUProgram::PROGRAM_PIXEL] = uiNumPixelEntries;
    m_auiCurrentEntry[NiGPUProgram::PROGRAM_PIXEL] = 0;
    if (uiNumPixelEntries)
    {
        m_apkEntries[NiGPUProgram::PROGRAM_PIXEL] = 
            NiAlloc(SCMExtraDataEntry, uiNumPixelEntries);
    }
    else
    {
        m_apkEntries[NiGPUProgram::PROGRAM_PIXEL] = 0;
    }    
}
//---------------------------------------------------------------------------
NiSCMExtraData::~NiSCMExtraData()
{
    NiFree(m_apkEntries[NiGPUProgram::PROGRAM_VERTEX]);
    NiFree(m_apkEntries[NiGPUProgram::PROGRAM_GEOMETRY]);
    NiFree(m_apkEntries[NiGPUProgram::PROGRAM_PIXEL]);
}
//---------------------------------------------------------------------------

// By default we neither stream nor clone these extra data objects since
// they only exist to hold temporary cached values.

bool NiSCMExtraData::IsCloneable() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiSCMExtraData::IsStreamable() const
{
    return false;
}
//---------------------------------------------------------------------------
