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
#include "NiBinaryShaderLibPCH.h"

#include "NSBRequirements.h"
#include "NSBUtility.h"
#include "NSBShader.h"

//---------------------------------------------------------------------------
NSBRequirements::NSBRequirements() :
    m_uiVSVersion(0), 
    m_uiGSVersion(0), 
    m_uiPSVersion(0), 
    m_uiUserVersion(0), 
    m_uiPlatformFlags(0),
    m_bUsesNiRenderState(false),
    m_bUsesNiLightState(false),
    m_uiBonesPerPartition(0), 
    m_uiBoneMatrixRegisters(0), 
    m_eBoneCalcMethod(NiBinaryShader::BONECALC_SKIN),
    m_eBinormalTangentMethod(NiShaderRequirementDesc::NBT_METHOD_NONE),
    m_uiBinormalTangentUVSource(NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT),
    m_bSoftwareVPAcceptable(false),
    m_bSoftwareVPRequired(false)
{
}
//---------------------------------------------------------------------------
NSBRequirements::~NSBRequirements()
{
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetVSVersion() const
{
    return m_uiVSVersion;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetVSVersion(unsigned int uiVersion)
{
    m_uiVSVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetGSVersion() const
{
    return m_uiGSVersion;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetGSVersion(unsigned int uiVersion)
{
    m_uiGSVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetPSVersion() const
{
    return m_uiPSVersion;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetPSVersion(unsigned int uiVersion)
{
    m_uiPSVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetUserVersion() const
{
    return m_uiUserVersion;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetUserVersion(unsigned int uiVersion)
{
    m_uiUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetPlatformFlags() const
{
    return m_uiPlatformFlags;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetPlatformFlags(unsigned int uiFlags)
{
    m_uiPlatformFlags = uiFlags;
}
//---------------------------------------------------------------------------
bool NSBRequirements::UsesNiRenderState() const
{
    return m_bUsesNiRenderState;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetUsesNiRenderState(bool bUses)
{
    m_bUsesNiRenderState = bUses;
}
//---------------------------------------------------------------------------
bool NSBRequirements::UsesNiLightState() const
{
    return m_bUsesNiLightState;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetUsesNiLightState(bool bUses)
{
    m_bUsesNiLightState = bUses;
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetBonesPerPartition() const
{
    return m_uiBonesPerPartition;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetBonesPerPartition(unsigned int uiBones)
{
    m_uiBonesPerPartition = uiBones;
}
//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetBoneMatrixRegisters() const
{
    return m_uiBoneMatrixRegisters;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetBoneMatrixRegisters(unsigned int uiRegisters)
{
    m_uiBoneMatrixRegisters = uiRegisters;
}
//---------------------------------------------------------------------------
NiBinaryShader::BoneMatrixCalcMethod NSBRequirements::GetBoneCalcMethod() 
    const
{
    return m_eBoneCalcMethod;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetBoneCalcMethod(
    NiBinaryShader::BoneMatrixCalcMethod eMethod)
{
    m_eBoneCalcMethod = eMethod;
}
//---------------------------------------------------------------------------
NiShaderRequirementDesc::NBTFlags NSBRequirements::GetBinormalTangentMethod()
    const
{
    return m_eBinormalTangentMethod;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetBinormalTangentMethod(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    m_eBinormalTangentMethod = eNBTMethod;
}

//---------------------------------------------------------------------------
unsigned int NSBRequirements::GetBinormalTangentUVSource() const
{
    return m_uiBinormalTangentUVSource;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetBinormalTangentUVSource(unsigned int uiSource)
{
    m_uiBinormalTangentUVSource = uiSource;
}
//---------------------------------------------------------------------------
bool NSBRequirements::GetSoftwareVPAcceptable() const
{
    return m_bSoftwareVPAcceptable;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetSoftwareVPAcceptable(bool bSoftwareVP)
{
    m_bSoftwareVPAcceptable = bSoftwareVP;
}
//---------------------------------------------------------------------------
bool NSBRequirements::GetSoftwareVPRequired() const
{
    return m_bSoftwareVPRequired;
}
//---------------------------------------------------------------------------
void NSBRequirements::SetSoftwareVPRequired(bool bSoftwareVP)
{
    m_bSoftwareVPRequired = bSoftwareVP;
}
//---------------------------------------------------------------------------
bool NSBRequirements::SaveBinary(NiBinaryStream& kStream)
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
bool NSBRequirements::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiPSVersion);
    NiStreamLoadBinary(kStream, m_uiVSVersion);

    if (NSBShader::GetReadVersion() >= 0x00010013)
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
    m_eBoneCalcMethod = (NiBinaryShader::BoneMatrixCalcMethod)uiValue;

    NiStreamLoadBinary(kStream, uiValue);
    m_eBinormalTangentMethod = (NiShaderRequirementDesc::NBTFlags)uiValue;

    // Version 1.11 added support for NBT Source UV sets
    if (NSBShader::GetReadVersion() >= 0x00010011)
        NiStreamLoadBinary(kStream, m_uiBinormalTangentUVSource);

    // Version 1.7 added better support for software vertex processing
    if (NSBShader::GetReadVersion() >= 0x00010007)
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
void NSBRequirements::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Requirements:\n");
    NSBUtility::IndentInsert();
#if defined(WIN32) || defined(_XENON)
    NSBUtility::Dump(pf, true, "VS        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiVSVersion),
        D3DSHADER_VERSION_MINOR(m_uiVSVersion));
    NSBUtility::Dump(pf, true, "GS        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiGSVersion),
        D3DSHADER_VERSION_MINOR(m_uiGSVersion));
    NSBUtility::Dump(pf, true, "PS        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiPSVersion),
        D3DSHADER_VERSION_MINOR(m_uiPSVersion));
    NSBUtility::Dump(pf, true, "US        : %d.%d\n",
        D3DSHADER_VERSION_MAJOR(m_uiUserVersion),
        D3DSHADER_VERSION_MINOR(m_uiUserVersion));
#else
    NSBUtility::Dump(pf, true, "VS        : %d\n", m_uiVSVersion);
    NSBUtility::Dump(pf, true, "GS        : %d\n", m_uiGSVersion);
    NSBUtility::Dump(pf, true, "PS        : %d\n", m_uiPSVersion);
    NSBUtility::Dump(pf, true, "US        : %d\n", m_uiUserVersion);
#endif
    NSBUtility::Dump(pf, true, "Platform  : 0x%08x\n", m_uiPlatformFlags);
    NSBUtility::Dump(pf, true, "UsesNiRS  : %s\n", 
        m_bUsesNiRenderState ? "true" : "false");
    NSBUtility::Dump(pf, true, "UsesNiLS  : %s\n", 
        m_bUsesNiLightState ? "true" : "false");
    NSBUtility::Dump(pf, true, "Bone/Part : %d\n", m_uiBonesPerPartition);
    NSBUtility::Dump(pf, true, "NBTMethod : 0x%08x\n", 
        (unsigned int)m_eBinormalTangentMethod);
    NSBUtility::Dump(pf, true, "NBTUVSource : 0x%08x\n", 
        (unsigned int)m_uiBinormalTangentUVSource);
    NSBUtility::Dump(pf, true, "SWVP OK   : %s\n", 
        m_bSoftwareVPAcceptable ? "true" : "false");
    NSBUtility::Dump(pf, true, "SWVP Req  : 0s\n", 
        m_bSoftwareVPRequired ? "true" : "false");
    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
