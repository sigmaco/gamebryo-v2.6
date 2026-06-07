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
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectTechniqueDesc.h"

//---------------------------------------------------------------------------
NiD3DXEffectTechniqueDesc::NiD3DXEffectTechniqueDesc() :
    m_pcShaderName(0),
    m_pcTechniqueName(0),
    m_pcDesc(0),
    m_uiImplementation(0),
    m_uiPSVersion(0),
    m_uiVSVersion(0),
    m_uiUserVersion(0),
    m_uiBonesPerPartition(0),
    m_eNBTMethod(NiShaderRequirementDesc::NBT_METHOD_NONE),
    m_uiNBTSourceUV(0),
    m_bUsesNiRenderState(false),
    m_bUsesNiLightState(false),
    m_bBlendIndicesAsD3DColor(false),
    m_bSoftwareVPRequired(false),
    m_bSoftwareVPAcceptible(false)
{
}
//---------------------------------------------------------------------------
