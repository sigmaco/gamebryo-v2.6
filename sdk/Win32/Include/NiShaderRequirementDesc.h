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

#ifndef NISHADERREQUIREMENTDESC_H
#define NISHADERREQUIREMENTDESC_H

// These classes and functions are used for DLL-based shader libraries
#include "NiShaderDescBase.h"

// Description which contains requirements
class NIMAIN_ENTRY NiShaderRequirementDesc : public NiShaderDescBase
{
public:
    NiShaderRequirementDesc();
    virtual ~NiShaderRequirementDesc();

    enum 
    {
        BINORMALTANGENTUVSOURCEDEFAULT = USHRT_MAX
    };

    enum NBTFlags
    {
        NBT_METHOD_NONE         = 0x0000,
        NBT_METHOD_NDL          = 0x1000,
        NBT_METHOD_DCC          = 0x2000,
        NBT_METHOD_ATI          = 0x3000,
        NBT_METHOD_MASK         = 0xF000,
        TEXTURE_SET_MASK        = 0x003F
    };

    virtual const unsigned int GetMinVertexShaderVersion() const;
    virtual const unsigned int GetMaxVertexShaderVersion() const;
    virtual const unsigned int GetMinGeometryShaderVersion() const;
    virtual const unsigned int GetMaxGeometryShaderVersion() const;
    virtual const unsigned int GetMinPixelShaderVersion() const;
    virtual const unsigned int GetMaxPixelShaderVersion() const;
    virtual const unsigned int GetMinUserDefinedVersion() const;
    virtual const unsigned int GetMaxUserDefinedVersion() const;
    virtual const unsigned int GetPlatformFlags() const;

    // Add to these platforms as NiShader support expands
    virtual const bool IsPlatformDX9() const;
    virtual const bool IsPlatformD3D10() const;
    virtual const bool IsPlatformXenon() const;

    unsigned int GetBonesPerPartition() const;
    NBTFlags GetBinormalTangentMethod() const;
    unsigned short GetBinormalTangentUVSource() const;

    bool GetSoftwareVPAcceptable() const;
    bool GetSoftwareVPRequired() const;
    
    bool GetConfigurableMaterial() const;

    // Translate shader version values
    static const unsigned int GetMajorVertexShaderVersion(
        const unsigned int uiVersion);
    static const unsigned int GetMinorVertexShaderVersion(
        const unsigned int uiVersion);
    static const unsigned int CreateVertexShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion);
    static const unsigned int GetMajorGeometryShaderVersion(
        const unsigned int uiVersion);
    static const unsigned int GetMinorGeometryShaderVersion(
        const unsigned int uiVersion);
    static const unsigned int CreateGeometryShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion);
    static const unsigned int GetMajorPixelShaderVersion(
        const unsigned int uiVersion);
    static const unsigned int GetMinorPixelShaderVersion(
        const unsigned int uiVersion);
    static const unsigned int CreatePixelShaderVersion(
        const unsigned int uiMajorVersion, const unsigned int uiMinorVersion);

    // *** begin Emergent internal use only ***
    virtual void AddVertexShaderVersion(unsigned int uiVersion);
    virtual void AddGeometryShaderVersion(unsigned int uiVersion);
    virtual void AddPixelShaderVersion(unsigned int uiVersion);
    virtual void AddUserDefinedVersion(unsigned int uiVersion);
    virtual void AddPlatformFlags(unsigned int uiPlatformFlags);
    virtual void RemovePlatformFlags(unsigned int uiPlatformFlags);
    virtual void SetPlatformDX9(bool bAllowed);
    virtual void SetPlatformD3D10(bool bAllowed);
    virtual void SetPlatformXenon(bool bAllowed);

    virtual void SetConfigurableMaterial(bool bConfigurableMaterial);

    virtual void SetBonesPerPartition(unsigned int uiBPP);
    virtual void SetBinormalTangentMethod(NBTFlags eNBTMethod);
    virtual void SetBinormalTangentUVSource(unsigned short usSource);

    virtual void SetSoftwareVPAcceptable(bool bSoftwareVP);
    virtual void SetSoftwareVPRequired(bool bSoftwareVP);
    // *** end Emergent internal use only ***

protected:
    unsigned int m_uiMinVertexShaderVersion;
    unsigned int m_uiMaxVertexShaderVersion;
    unsigned int m_uiMinGeometryShaderVersion;
    unsigned int m_uiMaxGeometryShaderVersion;
    unsigned int m_uiMinPixelShaderVersion;
    unsigned int m_uiMaxPixelShaderVersion;
    unsigned int m_uiMinUserVersion;
    unsigned int m_uiMaxUserVersion;
    unsigned int m_uiPlatformFlags;
    unsigned int m_uiBonesPerPartition;
    NBTFlags m_eBinormalTangentMethod;
    unsigned short m_usNBTSourceUV;
    bool m_bSoftwareVPAcceptable;
    bool m_bSoftwareVPRequired;
    bool m_bConfigurableMaterial;
};

typedef NiPointer<NiShaderRequirementDesc> NiShaderRequirementDescPtr;

#endif  //#ifndef NISHADERREQUIREMENTDESC_H
