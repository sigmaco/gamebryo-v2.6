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
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectTechniqueSet.h"
#include "NiD3DXEffectAttributeTable.h"
#include "NiD3DXEffectTechnique.h"
#include "NiD3DXEffectShader.h"

#include <NiShaderDesc.h>
#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
NiD3DXEffectTechniqueSet::NiD3DXEffectTechniqueSet() :
    m_pcName(NULL),
    m_pkBestImplementation(NULL)
{ /* */ }
//---------------------------------------------------------------------------
NiD3DXEffectTechniqueSet::~NiD3DXEffectTechniqueSet()
{
    NiD3DUtility::Log("Killing NiD3DXEffectTechniqueSet %s\n", m_pcName);

    m_kImplementations.RemoveAll();

    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
NiShaderDesc* NiD3DXEffectTechniqueSet::GetShaderDesc(
    NiD3DXEffectAttributeTable* pkAttribTable)
{
    if (m_spShaderDesc == NULL)
    {
        m_spShaderDesc = NiNew NiShaderDesc();
        NIASSERT(m_spShaderDesc);

        m_spShaderDesc->SetName(m_pcName);
        NiD3DXEffectTechnique* pkTech = m_kImplementations.GetAt(0);
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

        for (unsigned int i = 0; i < m_kImplementations.GetSize(); i++)
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
NiD3DXEffectShader* NiD3DXEffectTechniqueSet::GetD3DShader(
    NiD3DRenderer* pkRenderer, unsigned int uiImplementation)
{
    NIASSERT(pkRenderer);
    NiD3DXEffectTechnique* pkTechnique = NULL;
    if (uiImplementation < m_kImplementations.GetSize())
        pkTechnique = m_kImplementations.GetAt(uiImplementation);

    NiD3DXEffectShader* pkShader = NULL;
    if (pkTechnique)
        pkShader = pkTechnique->GetD3DShader(pkRenderer);
    else
        pkShader = GetBestD3DShader(pkRenderer);

    return pkShader;
}
//---------------------------------------------------------------------------
NiD3DXEffectShader* NiD3DXEffectTechniqueSet::GetBestD3DShader(
    NiD3DRenderer* pkRenderer)
{
    if (m_pkBestImplementation)
        return m_pkBestImplementation->GetD3DShader(pkRenderer);

    for (unsigned int i = 0; i < m_kImplementations.GetSize(); i++)
    {
        NiD3DXEffectTechnique* pkTechnique = m_kImplementations.GetAt(i);
        if (pkTechnique)
        {
            NiD3DXEffectShader* pkShader = 
                pkTechnique->GetD3DShader(pkRenderer);
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
void NiD3DXEffectTechniqueSet::DestroyRendererData()
{
    for (unsigned int i = 0; i < m_kImplementations.GetSize(); i++)
    {
        NiD3DXEffectTechnique* pkTech = m_kImplementations.GetAt(i);
        if (pkTech)
            pkTech->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechniqueSet::AddImplementation(
    NiD3DXEffectTechnique* pkTechnique)
{
    NIASSERT(pkTechnique);
    m_kImplementations.SetAtGrow(pkTechnique->GetImplementation(), 
        pkTechnique);
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechniqueSet::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
