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

#ifndef NID3D10EFFECTTECHNIQUEDESC_H
#define NID3D10EFFECTTECHNIQUEDESC_H

#include "NiD3D10EffectShaderLibType.h"
#include <NiD3D10Headers.h>
#include <NiGeometryData.h>
#include <NiGPUProgram.h>
#include <NiSemanticAdapterTable.h>

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectTechniqueDesc : public NiMemObject
{
public:
    NiD3D10EffectTechniqueDesc();

    NiFixedString m_kShaderName;
    NiFixedString m_kTechniqueName;
    NiFixedString m_kDesc;

    NiUInt32 m_uiImplementation;
    NiUInt32 m_uiVSVersion;
    NiUInt32 m_uiGSVersion;
    NiUInt32 m_uiPSVersion;
    NiUInt32 m_uiUserVersion;

    NiUInt32 m_uiBonesPerPartition;
    NiUInt32 m_uiNBTSourceUV;

    NiGeometryData::DataFlags m_eNBTMethod;
    bool m_bUsesNiRenderState;

    // This is an arbitrary constant - if its shaders with more passes need
    // to be parsed, then this value can be increased without causing problems.
    enum 
    {
        MAX_PASS_COUNT = 8
    };

    NiUInt32 m_uiPassCount;
    NiUInt8 m_auiBufferCount[MAX_PASS_COUNT][NiGPUProgram::PROGRAM_MAX];

    NiFixedString m_aaakConstantBuffers
        [MAX_PASS_COUNT]
        [NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];

    NiSemanticAdapterTable m_kAdapterTable;
};

#endif //NID3D10EFFECTTECHNIQUEDESC_H
