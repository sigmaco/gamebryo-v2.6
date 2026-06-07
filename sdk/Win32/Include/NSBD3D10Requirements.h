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

#ifndef NSBD3D10REQUIREMENTS_H
#define NSBD3D10REQUIREMENTS_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NiD3D10BinaryShader.h"
#include <NiMain.h>

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10Requirements : public NiMemObject
{
public:
    NSBD3D10Requirements();
    ~NSBD3D10Requirements();

    unsigned int GetVSVersion() const;
    void SetVSVersion(unsigned int uiVersion);
    unsigned int GetGSVersion() const;
    void SetGSVersion(unsigned int uiVersion);
    unsigned int GetPSVersion() const;
    void SetPSVersion(unsigned int uiVersion);
    unsigned int GetUserVersion() const;
    void SetUserVersion(unsigned int uiVersion);
    unsigned int GetPlatformFlags() const;
    void SetPlatformFlags(unsigned int uiFlags);

    bool UsesNiRenderState() const;
    void SetUsesNiRenderState(bool bUses);

    bool UsesNiLightState() const;
    void SetUsesNiLightState(bool bUses);

    NiShaderRequirementDesc::NBTFlags GetBinormalTangentMethod() const;
    void SetBinormalTangentMethod(
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

    unsigned int GetBinormalTangentUVSource() const;
    void SetBinormalTangentUVSource(unsigned int uiSource);

    bool GetSoftwareVPAcceptable() const;
    void SetSoftwareVPAcceptable(bool bSoftwareVP);

    bool GetSoftwareVPRequired() const;
    void SetSoftwareVPRequired(bool bSoftwareVP);

    // For skinning shaders
    unsigned int GetBonesPerPartition() const;
    void SetBonesPerPartition(unsigned int uiBones);
    unsigned int GetBoneMatrixRegisters() const;
    void SetBoneMatrixRegisters(unsigned int uiRegisters);
    NiD3D10BinaryShader::BoneMatrixCalcMethod GetBoneCalcMethod() const;
    void SetBoneCalcMethod(NiD3D10BinaryShader::BoneMatrixCalcMethod eMethod);

    // *** begin Emergent internal use only
    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

#if defined(NIDEBUG)
    void Dump(FILE* pf);
#endif  //#if defined(NIDEBUG)
    // *** end Emergent internal use only

protected:
    unsigned int m_uiVSVersion;
    unsigned int m_uiGSVersion;
    unsigned int m_uiPSVersion;
    unsigned int m_uiUserVersion;
    unsigned int m_uiPlatformFlags;
    bool m_bUsesNiRenderState;
    bool m_bUsesNiLightState;
    unsigned int m_uiBonesPerPartition;
    unsigned int m_uiBoneMatrixRegisters;
    NiD3D10BinaryShader::BoneMatrixCalcMethod m_eBoneCalcMethod;
    NiShaderRequirementDesc::NBTFlags m_eBinormalTangentMethod;
    unsigned int m_uiBinormalTangentUVSource;

    bool m_bSoftwareVPAcceptable;
    bool m_bSoftwareVPRequired;
};

#endif  //NSBD3D10REQUIREMENTS_H
