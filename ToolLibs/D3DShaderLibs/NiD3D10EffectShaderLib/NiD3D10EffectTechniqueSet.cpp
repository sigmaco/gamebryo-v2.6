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

#include "NiD3D10EffectTechniqueSet.h"
#include "NiD3D10EffectAttributeTable.h"
#include "NiD3D10EffectTechnique.h"
#include "NiD3D10EffectShader.h"

#include <NiRenderer.h>
#include <NiShaderDesc.h>

//---------------------------------------------------------------------------
NiD3D10EffectTechniqueSet::NiD3D10EffectTechniqueSet() :
    m_pkBestImplementation(NULL)
{ /* */ }
//---------------------------------------------------------------------------
NiD3D10EffectTechniqueSet::~NiD3D10EffectTechniqueSet()
{
    NILOG(
        "%s> Killing NiD3D10EffectTechniqueSet %s\n", 
        __FUNCTION__,
        m_kName);
    
    m_kImplementations.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10EffectShader* NiD3D10EffectTechniqueSet::GetD3D10Shader(
    NiUInt32 uiImplementation)
{
    NiD3D10EffectTechnique* pkTechnique = NULL;
    if (uiImplementation < m_kImplementations.GetSize())
        pkTechnique = m_kImplementations.GetAt(uiImplementation);

    NiD3D10EffectShader* pkShader = NULL;
    if (pkTechnique)
        pkShader = pkTechnique->GetD3D10Shader();
    else
        pkShader = GetBestD3D10Shader();

    return pkShader;
}
//---------------------------------------------------------------------------
NiD3D10EffectShader* NiD3D10EffectTechniqueSet::GetBestD3D10Shader()
{
    if (m_pkBestImplementation)
        return m_pkBestImplementation->GetD3D10Shader();

    for (NiUInt32 i = 0; i < m_kImplementations.GetSize(); i++)
    {
        NiD3D10EffectTechnique* pkTechnique = m_kImplementations.GetAt(i);
        if (pkTechnique)
        {
            NiD3D10EffectShader* pkShader = 
                pkTechnique->GetD3D10Shader();
            if (pkShader)
            {
                m_pkBestImplementation = pkTechnique;
                pkShader->SetIsBestImplementation(true);
                return pkShader;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiD3D10EffectTechniqueSet::GetShaderDesc(
    const NiD3D10EffectAttributeTable* pkAttribTable)
{
    // pkAttribTable has already been filled in with all the attributes from
    // this FX file.
    if (m_spShaderDesc == NULL)
    {
        m_spShaderDesc = NiNew NiShaderDesc();
        NIASSERT(m_spShaderDesc);

        m_spShaderDesc->SetName(m_kName);
        NiD3D10EffectTechnique* pkTech = m_kImplementations.GetAt(0);
        if (pkTech)
            m_spShaderDesc->SetDescription(pkTech->GetDescription());

        m_spShaderDesc->SetNumberOfImplementations(
            m_kImplementations.GetEffectiveSize());

        // Add attributes
        NiShaderAttributeDesc* pkDesc = pkAttribTable->GetFirstAttribute();
        while (pkDesc)
        {
            NiShaderAttributeDesc* pkNewDesc = 
                NiNew NiShaderAttributeDesc;
            NIASSERT(pkNewDesc);
            
            *pkNewDesc = *pkDesc;

            m_spShaderDesc->AddAttribute(pkNewDesc);

            pkDesc = pkAttribTable->GetNextAttribute();
        }

        for (NiUInt32 i = 0; i < m_kImplementations.GetSize(); i++)
        {
            pkTech = m_kImplementations.GetAt(i);
            if (pkTech)
            {
                // Add requirements
                NiShaderRequirementDesc* pkReq = pkTech->GetRequirements();
                NIASSERT(pkReq);

                m_spShaderDesc->AddPixelShaderVersion(
                    pkReq->GetMinPixelShaderVersion());
                m_spShaderDesc->AddPixelShaderVersion(
                    pkReq->GetMaxPixelShaderVersion());
                m_spShaderDesc->AddVertexShaderVersion(
                    pkReq->GetMinVertexShaderVersion());
                m_spShaderDesc->AddVertexShaderVersion(
                    pkReq->GetMaxVertexShaderVersion());
                m_spShaderDesc->AddUserDefinedVersion(
                    pkReq->GetMinUserDefinedVersion());
                m_spShaderDesc->AddUserDefinedVersion(
                    pkReq->GetMaxUserDefinedVersion());
                m_spShaderDesc->AddPlatformFlags(pkReq->GetPlatformFlags());

                m_spShaderDesc->SetBonesPerPartition(
                    pkReq->GetBonesPerPartition());
                m_spShaderDesc->SetBinormalTangentMethod(
                    pkReq->GetBinormalTangentMethod());

                m_spShaderDesc->SetSoftwareVPAcceptable(
                    pkReq->GetSoftwareVPAcceptable());
                m_spShaderDesc->SetSoftwareVPRequired(
                    pkReq->GetSoftwareVPRequired());

                m_spShaderDesc->SetImplementationDescription(i, pkReq);
            }
        }
    }
    return m_spShaderDesc;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechniqueSet::DestroyRendererData()
{
    for (NiUInt32 i = 0; i < m_kImplementations.GetSize(); i++)
    {
        NiD3D10EffectTechnique* pkTech = m_kImplementations.GetAt(i);
        if (pkTech)
            pkTech->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechniqueSet::AddImplementation(
    NiD3D10EffectTechnique* pkTechnique)
{
    NIASSERT(pkTechnique);
    m_kImplementations.SetAtGrow(pkTechnique->GetImplementation(), 
        pkTechnique);
}
//---------------------------------------------------------------------------
