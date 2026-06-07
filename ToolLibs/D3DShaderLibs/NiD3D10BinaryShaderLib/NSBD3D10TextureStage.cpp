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

#include "NSBD3D10TextureStage.h"
#include "NSBD3D10Utility.h"
#include "NSBD3D10Shader.h"

//---------------------------------------------------------------------------
NSBD3D10TextureStage::NSBD3D10TextureStage() :
    m_pcName(0), 
    m_uiStage(0), 
    m_uiTextureFlags(0), 
    m_pkTextureStageGroup(0),
    m_pkSamplerStageGroup(0), 
    m_usObjTextureFlags(0),
    m_bTextureTransform(false), 
    m_uiTextureTransformFlags(0), 
    m_pcGlobalEntry(0)
{
    m_kTextureTransformation._11 = 1.0f;
    m_kTextureTransformation._12 = 0.0f;
    m_kTextureTransformation._13 = 0.0f;
    m_kTextureTransformation._14 = 0.0f;
    m_kTextureTransformation._21 = 0.0f;
    m_kTextureTransformation._22 = 1.0f;
    m_kTextureTransformation._23 = 0.0f;
    m_kTextureTransformation._24 = 0.0f;
    m_kTextureTransformation._31 = 0.0f;
    m_kTextureTransformation._32 = 0.0f;
    m_kTextureTransformation._33 = 1.0f;
    m_kTextureTransformation._34 = 0.0f;
    m_kTextureTransformation._41 = 0.0f;
    m_kTextureTransformation._42 = 0.0f;
    m_kTextureTransformation._43 = 0.0f;
    m_kTextureTransformation._44 = 1.0f;
}
//---------------------------------------------------------------------------
NSBD3D10TextureStage::~NSBD3D10TextureStage()
{
    NiFree(m_pcGlobalEntry);
    NiFree(m_pcName);
    NiDelete m_pkTextureStageGroup;
    NiDelete m_pkSamplerStageGroup;
}
//---------------------------------------------------------------------------
void NSBD3D10TextureStage::SetName(const char* pcName)
{
    NSBD3D10Utility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup* NSBD3D10TextureStage::GetTextureStageGroup()
{
    if (m_pkTextureStageGroup == 0)
        m_pkTextureStageGroup = NiNew NSBD3D10StateGroup();

    return m_pkTextureStageGroup;
}
//---------------------------------------------------------------------------
NSBD3D10StateGroup* NSBD3D10TextureStage::GetSamplerStageGroup()
{
    if (m_pkSamplerStageGroup == 0)
        m_pkSamplerStageGroup = NiNew NSBD3D10StateGroup();

    return m_pkSamplerStageGroup;
}
//---------------------------------------------------------------------------
bool NSBD3D10TextureStage::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    NiStreamSaveBinary(kStream, m_uiStage);
    NiStreamSaveBinary(kStream, m_uiTextureFlags);
    if (!NSBD3D10Utility::SaveBinaryStateGroup(kStream, m_pkTextureStageGroup))
        return false;
    if (!NSBD3D10Utility::SaveBinaryStateGroup(kStream, m_pkSamplerStageGroup))
        return false;
    if (!SaveBinaryTextureTransform(kStream))
        return false;
    // use unsigned int for backwards compatibility
    unsigned int uiTemp = m_usObjTextureFlags;
    NiStreamSaveBinary(kStream, uiTemp);
    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10TextureStage::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();
    NiStreamLoadBinary(kStream, m_uiStage);
    NiStreamLoadBinary(kStream, m_uiTextureFlags);
    if (!NSBD3D10Utility::LoadBinaryStateGroup(kStream,
        m_pkTextureStageGroup))
    {
        return false;
    }
    if (!NSBD3D10Utility::LoadBinaryStateGroup(kStream,
        m_pkSamplerStageGroup))
    {
        return false;
    }
    if (!LoadBinaryTextureTransform(kStream))
        return false;
    if (NSBD3D10Shader::GetReadVersion() >= 0x00010009)
    {
        // Use unsigned int for backwards compatibility.
        unsigned int uiObjTextureFlags;
        NiStreamLoadBinary(kStream, uiObjTextureFlags);
        m_usObjTextureFlags = (unsigned short) uiObjTextureFlags;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10TextureStage::SaveBinaryTextureTransform(NiBinaryStream& kStream)
{
    unsigned int uiValue = m_bTextureTransform ? 1 : 0;
    NiStreamSaveBinary(kStream, uiValue);

    NiStreamSaveBinary(kStream, m_kTextureTransformation._11);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._12);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._13);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._14);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._21);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._22);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._23);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._24);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._31);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._32);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._33);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._34);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._41);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._42);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._43);
    NiStreamSaveBinary(kStream, m_kTextureTransformation._44);

    NiStreamSaveBinary(kStream, m_uiTextureTransformFlags);
    kStream.WriteCString(m_pcGlobalEntry);

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10TextureStage::LoadBinaryTextureTransform(NiBinaryStream& kStream)
{
    unsigned int uiValue;
    NiStreamLoadBinary(kStream, uiValue);
    m_bTextureTransform = (uiValue != 0);

    NiStreamLoadBinary(kStream, m_kTextureTransformation._11);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._12);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._13);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._14);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._21);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._22);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._23);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._24);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._31);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._32);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._33);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._34);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._41);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._42);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._43);
    NiStreamLoadBinary(kStream, m_kTextureTransformation._44);

    NiStreamLoadBinary(kStream, m_uiTextureTransformFlags);
    m_pcGlobalEntry = kStream.ReadCString();

    return true;
}
//---------------------------------------------------------------------------
bool NSBD3D10TextureStage::InterpretFlags(unsigned int uiNSBTextureFlags, 
    unsigned int& uiGBMap, unsigned int& uiGBMapInstance)
{
    // Note that the NSBTextureFlags are actually from NiD3DTextureStage, 
    // from the NiDX9Renderer. If the enumerations in NiD3DTextureStage 
    // ever change, then these mappings will need to be updated.

    //enum TSTextureFlags
    //{
    //    TSTF_IGNORE         = 0x00000000,
    //    // NiTexturingProperty Map to use
    //    TSTF_NONE           = 0x00100000,
    //    TSTF_NDL_BASE       = 0x00200000,
    //    TSTF_NDL_DARK       = 0x00300000,
    //    TSTF_NDL_DETAIL     = 0x00400000,
    //    TSTF_NDL_GLOSS      = 0x00500000,
    //    TSTF_NDL_GLOW       = 0x00600000,
    //    TSTF_NDL_BUMP       = 0x00700000,
    //    TSTF_NDL_NORMAL     = 0x00800000,
    //    TSTF_NDL_PARALLAX   = 0x00900000,
    //    TSTF_NDL_TYPEMASK   = 0x00F00000,
    //    // Flags for map to use
    //    TSTF_MAP_DECAL      = 0x01000000,
    //    TSTF_MAP_SHADER     = 0x02000000,
    //    TSTF_MAP_MASK       = 0x0F000000,
    //    // Flag to indicate usage
    //    TSTF_MAP_USE_INDEX  = 0x10000000,
    //    TSTF_MAP_USE_MASK   = 0xF0000000,
    //    // Index for decals and shader maps
    //    TSTF_INDEX_MASK     = 0x0000FFFF,
    //};

    uiGBMapInstance = 0;
    if (uiNSBTextureFlags == 0x00000000)
    {
        uiGBMap = NiD3D10Pass::GB_MAP_IGNORE;
        return true;
    }

    // Ignore the TSTF_MAP_USE_INDEX flag entirely in D3D10, because that
    // is the responsibility of the pixel shader.

    unsigned int uiType = (uiNSBTextureFlags & 0x00F00000);
    if (uiType == 0x00100000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_NONE;
        return true;
    }
    else if (uiType == 0x00200000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_BASE;
        return true;
    }
    else if (uiType == 0x00300000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_DARK;
        return true;
    }
    else if (uiType == 0x00400000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_DETAIL;
        return true;
    }
    else if (uiType == 0x00500000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_GLOSS;
        return true;
    }
    else if (uiType == 0x00600000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_GLOW;
        return true;
    }
    else if (uiType == 0x00700000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_BUMP;
        return true;
    }
    else if (uiType == 0x00800000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_NORMAL;
        return true;
    }
    else if (uiType == 0x00900000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_PARALLAX;
        return true;
    }
    
    uiType = (uiNSBTextureFlags & 0x0F000000);
    unsigned int uiInstance = (uiNSBTextureFlags & 0x0000FFFF);
    if (uiType == 0x01000000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_DECAL;
        uiGBMapInstance = uiInstance;
        return true;
    }
    else if (uiType == 0x02000000)
    {
        uiGBMap |= NiD3D10Pass::GB_MAP_SHADER;
        uiGBMapInstance = uiInstance;
        return true;
    }
    
    return false;

}
//---------------------------------------------------------------------------
bool NSBD3D10TextureStage::SetupTextureStage(NiD3D10Pass& kPass)
{
    unsigned int uiSamplerId = kPass.GetActiveSamplerCount();
    kPass.SetSamplerName(uiSamplerId, m_pcName);

    // Set up sampler information
    if (m_pkSamplerStageGroup)
    {
        if (m_pkSamplerStageGroup->GetStateCount())
        {
            NiD3D10RenderStateGroup* pkRSGroup = kPass.GetRenderStateGroup();
            if (pkRSGroup == NULL)
            {
                pkRSGroup = NiNew NiD3D10RenderStateGroup;
                kPass.SetRenderStateGroup(pkRSGroup);
            }

            if (!m_pkSamplerStageGroup->SetupTextureSamplerGroup(
                *pkRSGroup, uiSamplerId))
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBD3D10TextureStage::Dump(FILE* pf)
{
    NSBD3D10Utility::Dump(pf, true, "TextureStage %s\n", m_pcName);
    NSBD3D10Utility::IndentInsert();

    NSBD3D10Utility::Dump(pf, true, "Stage         = %d\n", m_uiStage);
    NSBD3D10Utility::Dump(pf, true, "Texture Flags = 0x%08x\n",
        m_uiTextureFlags);

    if (m_pkTextureStageGroup)
    {
        NSBD3D10Utility::Dump(pf, true, "TextureStageGroup\n");
        NSBD3D10Utility::IndentInsert();
        m_pkTextureStageGroup->Dump(pf, NSBD3D10StateGroup::DUMP_STAGESTATES);
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "***  NO STAGE GROUP ***\n");
    }

    if (m_pkSamplerStageGroup)
    {
        NSBD3D10Utility::Dump(pf, true, "SamplerStageGroup\n");
        NSBD3D10Utility::IndentInsert();
        m_pkSamplerStageGroup->Dump(pf,
            NSBD3D10StateGroup::DUMP_SAMPLERSTATES);
        NSBD3D10Utility::IndentRemove();
    }
    else
    {
        NSBD3D10Utility::Dump(pf, true, "**  NO SAMPLER GROUP **\n");
    }

    NSBD3D10Utility::Dump(pf, true, "TextureTransform ");
    if (m_bTextureTransform)
        NSBD3D10Utility::Dump(pf, false, "ENABLE\n");
    else
        NSBD3D10Utility::Dump(pf, false, "DISABLED\n");

    NSBD3D10Utility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
