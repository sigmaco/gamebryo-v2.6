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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10Requirements.h"
#include "NSBD3D10Utility.h"
#include "NSBD3D10Shader.h"

#include <NiD3D10ShaderFactory.h>

//---------------------------------------------------------------------------
NSBD3D10Requirements::NSBD3D10Requirements() :
    m_uiVSVersion(0), 
    m_uiGSVersion(0), 
    m_uiPSVersion(0), 
    m_uiUserVersion(0), 
    m_uiPlatformFlags(0),
    m_bUsesNiRenderState(false),
    m_bUsesNiLightState(false), // Not used for D3D10 - streamed for legacy.
    m_uiBonesPerPartition(0), 
    m_uiBoneMatrixRegisters(0), 
    m_eBoneCalcMethod(NiD3D10BinaryShader::BONECALC_SKIN),
    m_eBinormalTangentMethod(NiShaderRequirementDesc::NBT_METHOD_NONE),
    m_uiBinormalTangentUVSource(NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT),
    m_bSoftwareVPAcceptable(false),
    m_bSoftwareVPRequired(false)
{
}
//---------------------------------------------------------------------------
NSBD3D10Requirements::~NSBD3D10Requirements()
{
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetVSVersion() const
{
    return m_uiVSVersion;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetVSVersion(unsigned int uiVersion)
{
    m_uiVSVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetGSVersion() const
{
    return m_uiGSVersion;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetGSVersion(unsigned int uiVersion)
{
    m_uiGSVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetPSVersion() const
{
    return m_uiPSVersion;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetPSVersion(unsigned int uiVersion)
{
    m_uiPSVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetUserVersion() const
{
    return m_uiUserVersion;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetUserVersion(unsigned int uiVersion)
{
    m_uiUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetPlatformFlags() const
{
    return m_uiPlatformFlags;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetPlatformFlags(unsigned int uiFlags)
{
    m_uiPlatformFlags = uiFlags;
}
//---------------------------------------------------------------------------
bool NSBD3D10Requirements::UsesNiRenderState() const
{
    return m_bUsesNiRenderState;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetUsesNiRenderState(bool bUses)
{
    m_bUsesNiRenderState = bUses;
}
//---------------------------------------------------------------------------
// Not used for D3D10, but maintained for legacy streaming purposes.
bool NSBD3D10Requirements::UsesNiLightState() const
{
    return m_bUsesNiLightState;
}
//---------------------------------------------------------------------------
// Not used for D3D10, but maintained for legacy streaming purposes.
void NSBD3D10Requirements::SetUsesNiLightState(bool bUses)
{
    m_bUsesNiLightState = bUses;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetBonesPerPartition() const
{
    return m_uiBonesPerPartition;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetBonesPerPartition(unsigned int uiBones)
{
    m_uiBonesPerPartition = uiBones;
}
//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetBoneMatrixRegisters() const
{
    return m_uiBoneMatrixRegisters;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetBoneMatrixRegisters(unsigned int uiRegisters)
{
    m_uiBoneMatrixRegisters = uiRegisters;
}
//---------------------------------------------------------------------------
NiD3D10BinaryShader::BoneMatrixCalcMethod
    NSBD3D10Requirements::GetBoneCalcMethod()
    const
{
    return m_eBoneCalcMethod;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetBoneCalcMethod(
    NiD3D10BinaryShader::BoneMatrixCalcMethod eMethod)
{
    m_eBoneCalcMethod = eMethod;
}
//---------------------------------------------------------------------------
NiShaderRequirementDesc::NBTFlags
    NSBD3D10Requirements::GetBinormalTangentMethod() const
{
    return m_eBinormalTangentMethod;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetBinormalTangentMethod(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    m_eBinormalTangentMethod = eNBTMethod;
}

//---------------------------------------------------------------------------
unsigned int NSBD3D10Requirements::GetBinormalTangentUVSource() const
{
    return m_uiBinormalTangentUVSource;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetBinormalTangentUVSource(unsigned int uiSource)
{
    m_uiBinormalTangentUVSource = uiSource;
}
//---------------------------------------------------------------------------
bool NSBD3D10Requirements::GetSoftwareVPAcceptable() const
{
    return m_bSoftwareVPAcceptable;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetSoftwareVPAcceptable(bool bSoftwareVP)
{
    m_bSoftwareVPAcceptable = bSoftwareVP;
}
//---------------------------------------------------------------------------
bool NSBD3D10Requirements::GetSoftwareVPRequired() const
{
    return m_bSoftwareVPRequired;
}
//---------------------------------------------------------------------------
void NSBD3D10Requirements::SetSoftwareVPRequired(bool bSoftwareVP)
{
    m_bSoftwareVPRequired = bSoftwareVP;
}
//---------------------------------------------------------------------------
bool NSBD3D10Requirements::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiPSVersion);
    NiStreamSaveBinary(kStream, m_uiVSVersion);
    NiStreamSaveBinary(kStream, m_uiGSVersion);
    NiStreamSaveBinary(kStream, m_uiUserVersion);
    NiStreamSaveBinary(kStream, m_uiPlatformFlags);

    unsigned int uiValue = m_bUsesNiRenderState ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    uiValue = m_bUsesNiLightState ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_uiBonesPerPartition);
    NiStreamSaveBinary(kStream, m_uiBoneMatrixRegisters);
    
    uiValue = (unsigned int)m_eBoneCalcMethod;
    NiStreamSaveBinary(kStream, uiValue);
    
    uiValue = (unsigned int)m_eBinormalTangentMethod;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_uiBinormalTangentUVSource);

    uiValue = m_bSoftwareVPAcceptable ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    uiValue = m_bSoftwareVPRequired ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10Requirements::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiPSVersion);
    NiStreamLoadBinary(kStream, m_uiVSVersion);

    if (NSBD3D10Shader::GetReadVersion() >= 0x00010013)
    {
        // Version 1.13 added D3D10 and geometry shader support.
        NiStreamLoadBinary(kStream, m_uiGSVersion);
    }

    NiStreamLoadBinary(kStream, m_uiUserVersion);
    NiStreamLoadBinary(kStream, m_uiPlatformFlags);
    unsigned int uiValue;

    NiStreamLoadBinary(kStream, uiValue);
    m_bUsesNiRenderState = (uiValue != 0);

    NiStreamLoadBinary(kStream, uiValue);
    m_bUsesNiLightState = (uiValue != 0);

    NiStreamLoadBinary(kStream, m_uiBonesPerPartition);
    NiStreamLoadBinary(kStream, m_uiBoneMatrixRegisters);

    NiStreamLoadBinary(kStream, uiValue);
    m_eBoneCalcMethod = (NiD3D10BinaryShader::BoneMatrixCalcMethod)uiValue;

    NiStreamLoadBinary(kStream, uiValue);
    m_eBinormalTangentMethod = (NiShaderRequirementDesc::NBTFlags)uiValue;

    // Version 1.11 added support for NBT Source UV sets
    if (NSBD3D10Shader::GetReadVersion() >= 0x00010011)
        NiStreamLoadBinary(kStream, m_uiBinormalTangentUVSource);

    // Version 1.7 added better support for software vertex processing
    if (NSBD3D10Shader::GetReadVersion() >= 0x00010007)
    {
        NiStreamLoadBinary(kStream, uiValue);
        m_bSoftwareVPAcceptable = (uiValue != 0);

        NiStreamLoadBinary(kStream, uiValue);
        m_bSoftwareVPRequired = (uiValue != 0);
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10Requirements::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "Requirements:\n");
    NSBD3D10Utility::IndentInsert();
    NSBD3D10Utility::Dump(pf, true, "VS        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiVSVersion),
        D3DSHADER_VERSION_MINOR(m_uiVSVersion));
    NSBD3D10Utility::Dump(pf, true, "GS        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiGSVersion),
        D3DSHADER_VERSION_MINOR(m_uiGSVersion));
    NSBD3D10Utility::Dump(pf, true, "PS        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiPSVersion),
        D3DSHADER_VERSION_MINOR(m_uiPSVersion));
    NSBD3D10Utility::Dump(pf, true, "US        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiUserVersion),
        D3DSHADER_VERSION_MINOR(m_uiUserVersion));
    NSBD3D10Utility::Dump(pf, true, "Platform  : 0x%08x\n",
        m_uiPlatformFlags);
    NSBD3D10Utility::Dump(pf, true, "UsesNiRS  : %s\n", 
        m_bUsesNiRenderState ? "true" : "false");
    NSBD3D10Utility::Dump(pf, true, "UsesNiLS  : %s\n", 
        m_bUsesNiLightState ? "true" : "false");
    NSBD3D10Utility::Dump(pf, true, "Bone/Part : %d\n",
        m_uiBonesPerPartition);
    NSBD3D10Utility::Dump(pf, true, "NBTMethod : 0x%08x\n", 
        (unsigned int)m_eBinormalTangentMethod);
    NSBD3D10Utility::Dump(pf, true, "NBTUVSource : 0x%08x\n", 
        (unsigned int)m_uiBinormalTangentUVSource);
    NSBD3D10Utility::Dump(pf, true, "SWVP OK   : %s\n", 
        m_bSoftwareVPAcceptable ? "true" : "false");
    NSBD3D10Utility::Dump(pf, true, "SWVP Req  : 0s\n", 
        m_bSoftwareVPRequired ? "true" : "false");
    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
