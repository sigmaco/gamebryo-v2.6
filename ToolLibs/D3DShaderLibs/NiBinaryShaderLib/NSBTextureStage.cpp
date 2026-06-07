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

#include "NSBShader.h"
#include "NSBTextureStage.h"
#include "NSBUtility.h"

//---------------------------------------------------------------------------
NSBTextureStage::NSBTextureStage() :
    m_pcName(0), 
    m_uiStage(0), 
    m_uiTextureFlags(0), 
    m_pkTextureStageGroup(0),
    m_pkSamplerStageGroup(0), 
    m_usObjTextureFlags(0),
    m_bTextureTransform(false), 
    m_uiTextureTransformFlags(NiTextureStage::TSTTF_IGNORE), 
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
NSBTextureStage::~NSBTextureStage()
{
    NiFree(m_pcGlobalEntry);
    NiFree(m_pcName);
    NiDelete m_pkTextureStageGroup;
    NiDelete m_pkSamplerStageGroup;
}
//---------------------------------------------------------------------------
void NSBTextureStage::SetName(const char* pcName)
{
    NiRendererUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
NSBStateGroup* NSBTextureStage::GetTextureStageGroup()
{
    if (m_pkTextureStageGroup == 0)
        m_pkTextureStageGroup = NiNew NSBStateGroup();

    return m_pkTextureStageGroup;
}
//---------------------------------------------------------------------------
NSBStateGroup* NSBTextureStage::GetSamplerStageGroup()
{
    if (m_pkSamplerStageGroup == 0)
        m_pkSamplerStageGroup = NiNew NSBStateGroup();

    return m_pkSamplerStageGroup;
}
//---------------------------------------------------------------------------
bool NSBTextureStage::SaveBinary(NiBinaryStream& kStream)
{
    kStream.WriteCString(m_pcName);
    NiStreamSaveBinary(kStream, m_uiStage);
    NiStreamSaveBinary(kStream, m_uiTextureFlags);
    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkTextureStageGroup))
        return false;
    if (!NSBUtility::SaveBinaryStateGroup(kStream, m_pkSamplerStageGroup))
        return false;
    if (!SaveBinaryTextureTransform(kStream))
        return false;
    // use unsigned int for backwards compatibility
    unsigned int uiTemp = m_usObjTextureFlags;
    NiStreamSaveBinary(kStream, uiTemp);
    return true;
}
//---------------------------------------------------------------------------
bool NSBTextureStage::LoadBinary(NiBinaryStream& kStream)
{
    m_pcName = kStream.ReadCString();

    NiStreamLoadBinary(kStream, m_uiStage);
    NiStreamLoadBinary(kStream, m_uiTextureFlags);
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkTextureStageGroup))
        return false;
    if (!NSBUtility::LoadBinaryStateGroup(kStream, m_pkSamplerStageGroup))
        return false;
    if (!LoadBinaryTextureTransform(kStream))
        return false;
    if (NSBShader::GetReadVersion() >= 0x00010009)
    {
        // Use unsigned int for backwards compatibility.
        unsigned int uiObjTextureFlags;
        NiStreamLoadBinary(kStream, uiObjTextureFlags);
        m_usObjTextureFlags = (unsigned short) uiObjTextureFlags;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NSBTextureStage::SaveBinaryTextureTransform(NiBinaryStream& kStream)
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
bool NSBTextureStage::LoadBinaryTextureTransform(NiBinaryStream& kStream)
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
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
void NSBTextureStage::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "TextureStage %s\n", m_pcName);
    NSBUtility::IndentInsert();

    NSBUtility::Dump(pf, true, "Stage         = %d\n", m_uiStage);
    NSBUtility::Dump(pf, true, "Texture Flags = 0x%08x\n", m_uiTextureFlags);

    if (m_pkTextureStageGroup)
    {
        NSBUtility::Dump(pf, true, "TextureStageGroup\n");
        NSBUtility::IndentInsert();
        m_pkTextureStageGroup->Dump(pf, NSBStateGroup::DUMP_STAGESTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "***  NO STAGE GROUP ***\n");
    }

    if (m_pkSamplerStageGroup)
    {
        NSBUtility::Dump(pf, true, "SamplerStageGroup\n");
        NSBUtility::IndentInsert();
        m_pkSamplerStageGroup->Dump(pf, NSBStateGroup::DUMP_SAMPLERSTATES);
        NSBUtility::IndentRemove();
    }
    else
    {
        NSBUtility::Dump(pf, true, "**  NO SAMPLER GROUP **\n");
    }

    NSBUtility::Dump(pf, true, "TextureTransform ");
    if (m_bTextureTransform)
        NSBUtility::Dump(pf, false, "ENABLE\n");
    else
        NSBUtility::Dump(pf, false, "DISABLED\n");

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
