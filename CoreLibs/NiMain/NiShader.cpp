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

#include "NiMaterial.h"
#include "NiShader.h"

NiImplementRootRTTI(NiShader);
NiImplementRootRTTI(NiShader::NiShaderInstanceDescriptor);

//---------------------------------------------------------------------------
NiShader::NiShader() :
    m_uiImplementation(0),
    m_bInitialized(false),
    m_bBestImplementation(false)
{
}
//---------------------------------------------------------------------------
NiShader::~NiShader()
{
}
//---------------------------------------------------------------------------
const NiFixedString& NiShader::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiShader::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
unsigned int NiShader::GetImplementation() const
{
    return m_uiImplementation;
}
//---------------------------------------------------------------------------
void NiShader::SetImplementation(unsigned int uiImplementation)
{
    m_uiImplementation = uiImplementation;
}
//---------------------------------------------------------------------------
bool NiShader::GetIsBestImplementation() const
{
    return m_bBestImplementation;
}
//---------------------------------------------------------------------------
void NiShader::SetIsBestImplementation(bool bIsBest)
{
    m_bBestImplementation = bIsBest;
}
//---------------------------------------------------------------------------
inline bool NiShader::IsInitialized()
{
    return m_bInitialized;
}
//---------------------------------------------------------------------------
bool NiShader::Initialize()
{
    m_bInitialized = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiShader::SetupGeometry(NiRenderObject*, NiMaterialInstance*)
{
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PreProcessPipeline(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::UpdatePipeline(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::FirstPass()
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::SetupRenderingPass(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::SetupTransformations(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::SetupShaderPrograms(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PreRenderSubmesh(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PostRenderSubmesh(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PostRender(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::NextPass()
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PostProcessPipeline(const NiRenderCallContext&)
{
    return 0;
}
//---------------------------------------------------------------------------
bool NiShader::GetVertexInputSemantics(
    NiShaderDeclaration::ShaderRegisterEntry* pkSemantics, 
    unsigned int uiSemanticEntryCount,
    NiShaderDeclaration* pkDecl)
{
    if (pkSemantics == NULL)
        return false;

    if (uiSemanticEntryCount < NiMaterial::VS_INPUTS_MAX_NUM)
    {
        pkSemantics[0].m_uiUsageIndex =
            (unsigned int) NiMaterial::VS_INPUTS_USE_GEOMETRY;
        return false;
    }

    if (!pkDecl)
    {
        // Default implementation requests position.
        pkSemantics[0].m_uiPreferredStream = 0;
        pkSemantics[0].m_eInput =
            NiShaderDeclaration::SHADERPARAM_NI_POSITION0;
        pkSemantics[0].m_eType = NiShaderDeclaration::SPTYPE_FLOAT3;
        pkSemantics[0].m_kUsage = NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_POSITION);
        pkSemantics[0].m_uiUsageIndex = 0;
        pkSemantics[0].m_uiExtraData = 0;

        pkSemantics[1].m_uiUsageIndex =
            (unsigned int) NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
        return true;
    }

    unsigned int uiStreamCount = pkDecl->GetStreamCount();
    unsigned int uiEntryCount = 0;
    unsigned int uiSemanticCount = 0;
    const NiShaderDeclaration::ShaderRegisterEntry* pkEntry = NULL;

    for (unsigned int ui = 0; ui < uiStreamCount; ++ui)
    {
        uiEntryCount = pkDecl->GetEntryCount(ui);
        for (unsigned int uj = 0; uj < uiEntryCount; ++uj)
        {
            // uj counts entries, and GetEntry takes the entry arg first.
            pkEntry = pkDecl->GetEntry(uj, ui);
            if (!pkEntry)
                continue;

            // Packing offset is coerced to be the stream, so set it to ui.
            pkSemantics[uiSemanticCount].m_uiPreferredStream = ui;
            pkSemantics[uiSemanticCount].m_eInput = pkEntry->m_eInput;
            pkSemantics[uiSemanticCount].m_eType = pkEntry->m_eType;
            pkSemantics[uiSemanticCount].m_kUsage = pkEntry->m_kUsage;
            pkSemantics[uiSemanticCount].m_uiUsageIndex = 
                pkEntry->m_uiUsageIndex; 
            pkSemantics[uiSemanticCount].m_uiExtraData = 
                pkEntry->m_uiExtraData;
            ++uiSemanticCount;

            if (uiSemanticCount == NiMaterial::VS_INPUTS_MAX_NUM - 1)
                break;
        }
        if (uiSemanticCount == NiMaterial::VS_INPUTS_MAX_NUM - 1)
            break;
    }

    // Terminate the array.
    pkSemantics[uiSemanticCount].m_uiUsageIndex = 
        (unsigned int) NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiShader::IsGenericallyConfigurable()
{
    return false;
}
//---------------------------------------------------------------------------
bool NiShader::AppendRenderPass(unsigned int&)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiShader::SetAlphaOverride(
    unsigned int,
    bool,
    bool,
    NiAlphaProperty::AlphaFunction,
    bool,
    NiAlphaProperty::AlphaFunction)
{
    return false;
}

//---------------------------------------------------------------------------
bool NiShader::SetUsesNiRenderState(bool)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiShader::SetGPUProgram(
    unsigned int,
    NiGPUProgram*,
    NiGPUProgram::ProgramType&)
{
    return false;
}
//---------------------------------------------------------------------------
NiShaderConstantMap* NiShader::CreateShaderConstantMap(
    unsigned int,
    NiGPUProgram::ProgramType,
    unsigned int)
{
    return NULL;
}
//---------------------------------------------------------------------------
bool NiShader::AppendTextureSampler(
    unsigned int,
    unsigned int&,
    const NiFixedString&,
    const NiFixedString&,
    unsigned int)
{
    return false;
}
//---------------------------------------------------------------------------
const NiShader::NiShaderInstanceDescriptor*
    NiShader::GetShaderInstanceDesc() const
{
    return NULL;
}
//---------------------------------------------------------------------------
const NiSemanticAdapterTable& NiShader::GetSemanticAdapterTable() const
{
    return m_kAdapterTable;
}
//---------------------------------------------------------------------------
NiSemanticAdapterTable& NiShader::GetSemanticAdapterTable()
{
    return m_kAdapterTable;
}
//---------------------------------------------------------------------------
void NiShader::SetSemanticAdapterTableFromShaderDeclaration(
    NiShaderDeclaration* pkShaderDecl)
{
    NiShaderDeclaration::ShaderRegisterEntry 
        akShaderInputSemantics[NiMaterial::VS_INPUTS_MAX_NUM];
    if (!GetVertexInputSemantics(akShaderInputSemantics, 
        NiMaterial::VS_INPUTS_MAX_NUM, pkShaderDecl))
    {
        return;
    }

    if (!m_kAdapterTable.CreateDefaultTable(akShaderInputSemantics, 
        NiMaterial::VS_INPUTS_MAX_NUM))
    {
        return;
    }
}
//---------------------------------------------------------------------------
void NiShader::ReferenceVertexDeclarationCache(
    NiVertexDeclarationCache) const
{
    /* */
}
//---------------------------------------------------------------------------
void NiShader::ReleaseVertexDeclarationCache(
    NiVertexDeclarationCache) const
{
    /* */
}
//---------------------------------------------------------------------------
