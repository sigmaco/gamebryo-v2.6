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

#include "NiD3D10EffectPass.h"
#include "NiD3D10EffectFile.h"
#include "NiD3D10EffectTechnique.h"
#include <NiD3D10Renderer.h>
#include <NiD3D10DataStream.h>

//---------------------------------------------------------------------------
NiD3D10EffectPass::NiD3D10EffectPass() :
    m_pkStateBlockMask(NULL),
    m_pkPass(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10EffectPass::~NiD3D10EffectPass()
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10PassPtr NiD3D10EffectPass::Create()
{
    NiD3D10EffectPassPtr spPass = NiNew NiD3D10EffectPass;

    return (NiSmartPointerCast(NiD3D10Pass, spPass));
}
//---------------------------------------------------------------------------
unsigned int NiD3D10EffectPass::ApplyShaderPrograms(
    const NiRenderCallContext&)
{
    // Do nothing here - must set all variables _before_ applying the pass
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10EffectPass::ApplyShaderConstants(
    const NiRenderCallContext& kRCC)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());

    if (m_bConstantMapsLinked == false && LinkUpConstantMaps() == false)
    {
    }

    // Vertex Shader
    NiShaderError eErr = NISHADERERR_OK;
    const unsigned int uiVSMapCount = m_kVertexShaderConstantMaps.GetSize();
    for (unsigned int i = 0; i < uiVSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            m_kVertexShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            eErr = pkMap->UpdateShaderConstants(kRCC, true);
            // Update the buffer.

            NiD3D10DataStream* pkDataStream = 
                pkMap->GetShaderConstantDataStream();
            NIASSERT(pkDataStream);
            pkDataStream->UpdateD3D10Buffers();

            // Don't set constant buffer on the device - the D3D10
            // effect system does that.
        }
    }

    // All shader constant maps should be VS maps - the assignment of the
    // constant maps is irrelevant, as D3D10 applies them. The only purpose
    // of the maps is to update the constant buffers.
    NIASSERT(m_kGeometryShaderConstantMaps.GetSize() == 0);
    NIASSERT(m_kPixelShaderConstantMaps.GetSize() == 0);

    // Apply pass here
    HRESULT hr = E_FAIL;
    if (m_pkPass)
        hr = m_pkPass->Apply(0);
    return (SUCCEEDED(hr) ? 0 : UINT_MAX);
}
//---------------------------------------------------------------------------
void NiD3D10EffectPass::SetStateBlockMask(
    const D3D10_STATE_BLOCK_MASK* pkMask)
{
    m_pkStateBlockMask = pkMask;
}
//---------------------------------------------------------------------------
const D3D10_STATE_BLOCK_MASK* NiD3D10EffectPass::GetStateBlockMask() const
{
    return m_pkStateBlockMask;
}
//---------------------------------------------------------------------------
void NiD3D10EffectPass::SetD3D10Pass(ID3D10EffectPass* pkD3D10Pass)
{
    NIASSERT(pkD3D10Pass == NULL || pkD3D10Pass->IsValid());
    m_pkPass = pkD3D10Pass;
}
//---------------------------------------------------------------------------
ID3D10EffectPass* NiD3D10EffectPass::GetD3D10Pass() const
{
    return m_pkPass;
}
//---------------------------------------------------------------------------
void NiD3D10EffectPass::AddConstantMap(NiD3D10ShaderConstantMap* pkMap)
{
    // Yes, all constant maps are getting added as vertex constant 
    // maps. Using the constant maps is a convenience to update the 
    // constant buffers, so their eventual destination does not matter.

    // Be sure the constant buffer is not already in the array
    NiUInt32 uiIndex = m_kVertexShaderConstantMaps.Find(
        pkMap);
    // Find returns UINT_MAX if not found.
    if (uiIndex > m_kVertexShaderConstantMaps.GetSize())
    {
        m_kVertexShaderConstantMaps.Add(pkMap);
    }
}
//---------------------------------------------------------------------------
