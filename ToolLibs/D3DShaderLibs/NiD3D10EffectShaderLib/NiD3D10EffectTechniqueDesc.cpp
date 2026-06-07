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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectTechniqueDesc.h"

//---------------------------------------------------------------------------
NiD3D10EffectTechniqueDesc::NiD3D10EffectTechniqueDesc() :
    m_uiImplementation(0),
    m_uiVSVersion(0),
    m_uiGSVersion(0),
    m_uiPSVersion(0),
    m_uiUserVersion(0),
    m_uiBonesPerPartition(0),
    m_uiNBTSourceUV(0),
    m_eNBTMethod(NiShaderRequirementDesc::NBT_METHOD_NONE),
    m_bUsesNiRenderState(false),
    m_uiPassCount(0)
{
    for (NiUInt32 i = 0; i < MAX_PASS_COUNT; i++)
    {
        for (NiUInt32 j = 0; j < NiGPUProgram::PROGRAM_MAX; j++)
        {
            m_auiBufferCount[i][j] = 0;

            for (NiUInt32 k = 0; 
                k < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; k++)
            {
                m_aaakConstantBuffers[i][j][k] = NULL;
            }
        }
    }
}
//---------------------------------------------------------------------------
