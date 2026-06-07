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

#include "NiShaderRequirementDesc.h"
#include "NiShader.h"
#include "NiShaderFactory.h"

//---------------------------------------------------------------------------
NiShaderRequirementDesc::NiShaderRequirementDesc() :
    NiShaderDescBase(),
    m_uiMinVertexShaderVersion(0xffffffff), 
    m_uiMaxVertexShaderVersion(0), 
    m_uiMinGeometryShaderVersion(0xffffffff), 
    m_uiMaxGeometryShaderVersion(0), 
    m_uiMinPixelShaderVersion(0xffffffff), 
    m_uiMaxPixelShaderVersion(0), 
    m_uiMinUserVersion(0xffffffff), 
    m_uiMaxUserVersion(0), 
    m_uiPlatformFlags(0), 
    m_uiBonesPerPartition(0), 
    m_eBinormalTangentMethod(NBT_METHOD_NONE),
    m_usNBTSourceUV(BINORMALTANGENTUVSOURCEDEFAULT),
    m_bConfigurableMaterial(false)
{
}
//---------------------------------------------------------------------------
NiShaderRequirementDesc::~NiShaderRequirementDesc()
{
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinVertexShaderVersion() const
{
    return m_uiMinVertexShaderVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMaxVertexShaderVersion() const
{
    return m_uiMaxVertexShaderVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinGeometryShaderVersion() const
{
    return m_uiMinGeometryShaderVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMaxGeometryShaderVersion() const
{
    return m_uiMaxGeometryShaderVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinPixelShaderVersion() const
{
    return m_uiMinPixelShaderVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMaxPixelShaderVersion() const
{
    return m_uiMaxPixelShaderVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinUserDefinedVersion() const
{
    return m_uiMinUserVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMaxUserDefinedVersion() const
{
    return m_uiMaxUserVersion;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetPlatformFlags() const
{
    return m_uiPlatformFlags;
}
//---------------------------------------------------------------------------
const bool NiShaderRequirementDesc::IsPlatformDX9() const
{
    if (m_uiPlatformFlags & NiShader::NISHADER_DX9)
        return true;
    return false;
}
//---------------------------------------------------------------------------
const bool NiShaderRequirementDesc::IsPlatformD3D10() const
{
    if (m_uiPlatformFlags & NiShader::NISHADER_D3D10)
        return true;
    return false;
}
//---------------------------------------------------------------------------
const bool NiShaderRequirementDesc::IsPlatformXenon() const
{
    if (m_uiPlatformFlags & NiShader::NISHADER_XENON)
        return true;
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiShaderRequirementDesc::GetBonesPerPartition() const
{
    return m_uiBonesPerPartition;
}
//---------------------------------------------------------------------------
NiShaderRequirementDesc::NBTFlags
    NiShaderRequirementDesc::GetBinormalTangentMethod() const
{
    return m_eBinormalTangentMethod;
}

//---------------------------------------------------------------------------
unsigned short NiShaderRequirementDesc::GetBinormalTangentUVSource() 
    const
{
    return m_usNBTSourceUV;
}
//---------------------------------------------------------------------------
bool NiShaderRequirementDesc::GetSoftwareVPAcceptable() const
{
    return m_bSoftwareVPAcceptable;
}
//---------------------------------------------------------------------------
bool NiShaderRequirementDesc::GetSoftwareVPRequired() const
{
    return m_bSoftwareVPRequired;
}
//---------------------------------------------------------------------------
bool NiShaderRequirementDesc::GetConfigurableMaterial() const
{
    return m_bConfigurableMaterial;
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinorVertexShaderVersion(
    const unsigned int uiVersion)
{
    return NiShaderFactory::GetInstance()->
        GetMinorVertexShaderVersion(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMajorVertexShaderVersion(
    const unsigned int uiVersion)
{
    return NiShaderFactory::GetInstance()->
        GetMajorVertexShaderVersion(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::CreateVertexShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    return NiShaderFactory::GetInstance()->
        CreateVertexShaderVersion(uiMajorVersion, uiMinorVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinorGeometryShaderVersion(
    const unsigned int uiVersion)
{
    return NiShaderFactory::GetInstance()->
        GetMinorGeometryShaderVersion(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMajorGeometryShaderVersion(
    const unsigned int uiVersion)
{
    return NiShaderFactory::GetInstance()->
        GetMajorGeometryShaderVersion(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::CreateGeometryShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    return NiShaderFactory::GetInstance()->
        CreateGeometryShaderVersion(uiMajorVersion, uiMinorVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMinorPixelShaderVersion(
    const unsigned int uiVersion)
{
    return NiShaderFactory::GetInstance()->
        GetMinorPixelShaderVersion(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::GetMajorPixelShaderVersion(
    const unsigned int uiVersion)
{
    return NiShaderFactory::GetInstance()->
        GetMajorPixelShaderVersion(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiShaderRequirementDesc::CreatePixelShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    return NiShaderFactory::GetInstance()->
        CreatePixelShaderVersion(uiMajorVersion, uiMinorVersion);
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::AddVertexShaderVersion(unsigned int uiVersion)
{
    if (m_uiMinVertexShaderVersion > uiVersion)
        m_uiMinVertexShaderVersion = uiVersion;
    if (m_uiMaxVertexShaderVersion < uiVersion)
        m_uiMaxVertexShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::AddGeometryShaderVersion(unsigned int uiVersion)
{
    if (m_uiMinGeometryShaderVersion > uiVersion)
        m_uiMinGeometryShaderVersion = uiVersion;
    if (m_uiMaxGeometryShaderVersion < uiVersion)
        m_uiMaxGeometryShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::AddPixelShaderVersion(unsigned int uiVersion)
{
    if (m_uiMinPixelShaderVersion > uiVersion)
        m_uiMinPixelShaderVersion = uiVersion;
    if (m_uiMaxPixelShaderVersion < uiVersion)
        m_uiMaxPixelShaderVersion = uiVersion;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::AddUserDefinedVersion(unsigned int uiVersion)
{
    if (m_uiMinUserVersion > uiVersion)
        m_uiMinUserVersion = uiVersion;
    if (m_uiMaxUserVersion < uiVersion)
        m_uiMaxUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::AddPlatformFlags(unsigned int uiPlatformFlags)
{
    m_uiPlatformFlags |= uiPlatformFlags;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::RemovePlatformFlags(
    unsigned int uiPlatformFlags)
{
    m_uiPlatformFlags &= ~uiPlatformFlags;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetPlatformDX9(bool bAllowed)
{
    if (bAllowed)
        m_uiPlatformFlags |= NiShader::NISHADER_DX9;
    else
        m_uiPlatformFlags &= ~NiShader::NISHADER_DX9;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetPlatformD3D10(bool bAllowed)
{
    if (bAllowed)
        m_uiPlatformFlags |= NiShader::NISHADER_D3D10;
    else
        m_uiPlatformFlags &= ~NiShader::NISHADER_D3D10;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetPlatformXenon(bool bAllowed)
{
    if (bAllowed)
        m_uiPlatformFlags |= NiShader::NISHADER_XENON;
    else
        m_uiPlatformFlags &= ~NiShader::NISHADER_XENON;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetConfigurableMaterial(bool 
    bConfigurableMaterial)
{
    m_bConfigurableMaterial = bConfigurableMaterial;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetBonesPerPartition(unsigned int uiBPP)
{
    m_uiBonesPerPartition = uiBPP;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetBinormalTangentMethod(
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    m_eBinormalTangentMethod = eNBTMethod;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetBinormalTangentUVSource(
    unsigned short usSource)
{
    m_usNBTSourceUV = usSource;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetSoftwareVPAcceptable(bool bSoftwareVP)
{
    m_bSoftwareVPAcceptable = bSoftwareVP;
}
//---------------------------------------------------------------------------
void NiShaderRequirementDesc::SetSoftwareVPRequired(bool bSoftwareVP)
{
    m_bSoftwareVPRequired = bSoftwareVP;
}
//---------------------------------------------------------------------------
