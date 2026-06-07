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

#include "NiRenderTargetGroup.h"
#include "NiRenderedTexture.h"
#include "NiRenderer.h"

NiImplementRTTI(NiRenderTargetGroup, NiObject);

//---------------------------------------------------------------------------
NiRenderTargetGroup* NiRenderTargetGroup::Create(unsigned int uiNumBuffers, 
    NiRenderer* pkRenderer)
{
    if (!pkRenderer)
        return NULL;

    if (pkRenderer->GetMaxBuffersPerRenderTargetGroup() < uiNumBuffers)
        return NULL;

    if (MAX_RENDER_BUFFERS < uiNumBuffers)
        return NULL;

    NiRenderTargetGroup* pkRenderTargetGroup = NiNew NiRenderTargetGroup();
    pkRenderTargetGroup->m_uiNumBuffers = uiNumBuffers;
    return pkRenderTargetGroup;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiRenderTargetGroup::Create(Ni2DBuffer* pkBuffer, 
    NiRenderer* pkRenderer, bool bUseDefaultDepthStencil,
    bool bCreateCompatibleDepthStencil)
{
    NiDepthStencilBuffer* pkDSBuffer = NULL;
    if (bUseDefaultDepthStencil)
    {
        pkDSBuffer = pkRenderer->GetDefaultDepthStencilBuffer();
        if (pkDSBuffer != NULL &&
            !pkRenderer->IsDepthBufferCompatible(pkBuffer, pkDSBuffer))
        {
            pkDSBuffer = NULL;
        }
    }

    if (!pkDSBuffer && bCreateCompatibleDepthStencil)
    {
        pkDSBuffer = NiDepthStencilBuffer::CreateCompatible(pkBuffer,
            pkRenderer);
    }

    return Create(pkBuffer, pkRenderer, pkDSBuffer);
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiRenderTargetGroup::Create(Ni2DBuffer* pkBuffer, 
    NiRenderer* pkRenderer, NiDepthStencilBuffer* pkDSBuffer)
{
    if (!pkRenderer)
        return NULL;

    if (!pkBuffer)
        return NULL;

    if (pkRenderer->GetMaxBuffersPerRenderTargetGroup() < 1)
        return NULL;

    NiUInt8 uiMaxRenderBuffers = MAX_RENDER_BUFFERS;
    if (uiMaxRenderBuffers < 1)
        return NULL;

    if (pkDSBuffer && 
        !pkRenderer->IsDepthBufferCompatible(pkBuffer, pkDSBuffer))
    {
        NiOutputDebugString("NiRenderTargetGroup::Create failed due to "
            "incompatibility between color buffer and depth buffer.\n");
        return NULL;
    }

    NiRenderTargetGroup* pkRenderTargetGroup = NiNew NiRenderTargetGroup();
    pkRenderTargetGroup->m_uiNumBuffers = 1;
    NIVERIFY(pkRenderTargetGroup->AttachBuffer(pkBuffer, 0));

    if (pkDSBuffer)
    {
        NIASSERT(pkDSBuffer->GetWidth() >= pkBuffer->GetWidth());
        NIASSERT(pkDSBuffer->GetHeight() >= pkBuffer->GetHeight());
        NIVERIFY(pkRenderTargetGroup->AttachDepthStencilBuffer(pkDSBuffer));
    }
    return pkRenderTargetGroup;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup::NiRenderTargetGroup() :
    m_uiNumBuffers(0),
    m_pkRendererData(NULL)
{
}
//---------------------------------------------------------------------------
NiRenderTargetGroup::~NiRenderTargetGroup()
{
    NiDelete m_pkRendererData;
}
//---------------------------------------------------------------------------
unsigned int NiRenderTargetGroup::GetWidth(unsigned int uiIndex) const
{
    if (uiIndex >= m_uiNumBuffers)
        return 0;

    if (m_aspBuffers[uiIndex])
        return m_aspBuffers[uiIndex]->GetWidth();
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiRenderTargetGroup::GetHeight(unsigned int uiIndex) const
{
    if (uiIndex >= m_uiNumBuffers)
        return 0;

    if (m_aspBuffers[uiIndex])
        return m_aspBuffers[uiIndex]->GetHeight();
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiRenderTargetGroup::GetDepthStencilWidth() const
{
    if (m_spDepthStencilBuffer)
        return m_spDepthStencilBuffer->GetWidth();
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiRenderTargetGroup::GetDepthStencilHeight() const
{
    if (m_spDepthStencilBuffer)
        return m_spDepthStencilBuffer->GetHeight();
    else
        return 0;
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::HasDepthStencil() const
{
    return m_spDepthStencilBuffer != NULL;
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::IsValid() const
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!pkRenderer)
        return false;

    return pkRenderer->ValidateRenderTargetGroup((NiRenderTargetGroup*)this);
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::CheckMSAAPrefConsistency() const
{
    if (!m_uiNumBuffers || !m_aspBuffers[0])
        return false;

    Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        m_aspBuffers[0]->GetMSAAPref();

    for (unsigned int ui = 1; ui < m_uiNumBuffers; ui++)
    {
        if (!m_aspBuffers[ui] || eMSAAPref != m_aspBuffers[ui]->GetMSAAPref())
        {
            NiRenderer::Warning("Render target MSAA preferences do not match");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiRenderTargetGroup::GetPixelFormat(
    unsigned int uiIndex) const
{
    if (uiIndex >= m_uiNumBuffers)
        return NULL;

    if (m_aspBuffers[uiIndex])
        return m_aspBuffers[uiIndex]->GetPixelFormat();
    else
        return 0;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiRenderTargetGroup::GetDepthStencilPixelFormat() const
{
    if (m_spDepthStencilBuffer)
        return m_spDepthStencilBuffer->GetPixelFormat();
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiRenderTargetGroup::GetBufferCount() const
{
    return m_uiNumBuffers;
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::AttachBuffer(Ni2DBuffer* pkBuffer, 
    unsigned int uiIndex)
{
    SetRendererData(NULL); // changed the state, must clear render data.
    NIASSERT(m_uiNumBuffers <= MAX_RENDER_BUFFERS);
    if (uiIndex < m_uiNumBuffers)
    {
        m_aspBuffers[uiIndex] = pkBuffer;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::AttachDepthStencilBuffer(NiDepthStencilBuffer* 
    pkDepthBuffer)
{
    SetRendererData(NULL); // changed the state, must clear render data.
    m_spDepthStencilBuffer = pkDepthBuffer;
    return true;
}
//---------------------------------------------------------------------------
Ni2DBuffer* NiRenderTargetGroup::GetBuffer(unsigned int uiIndex) const
{
    if (uiIndex >= m_uiNumBuffers)
        return NULL;

    return m_aspBuffers[uiIndex];
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiRenderTargetGroup::GetDepthStencilBuffer() const
{
    return m_spDepthStencilBuffer;
}
//---------------------------------------------------------------------------
const NiRenderTargetGroup::RendererData* NiRenderTargetGroup::GetRendererData()
    const
{
    return m_pkRendererData;
}
//---------------------------------------------------------------------------
void NiRenderTargetGroup::SetRendererData(
    NiRenderTargetGroup::RendererData* pkRendererData)
{
    NiDelete m_pkRendererData;
    m_pkRendererData = pkRendererData;
}
//---------------------------------------------------------------------------
Ni2DBuffer::RendererData* NiRenderTargetGroup::GetBufferRendererData(
    unsigned int uiIndex) const
{
    if (uiIndex >= m_uiNumBuffers)
        return NULL;

    if (m_aspBuffers[uiIndex])
        return m_aspBuffers[uiIndex]->GetRendererData();
    else
        return NULL;
}
//---------------------------------------------------------------------------
Ni2DBuffer::RendererData* NiRenderTargetGroup::
    GetDepthStencilBufferRendererData() const
{
    if (m_spDepthStencilBuffer)
        return m_spDepthStencilBuffer->GetRendererData();
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup::CubeMapSetupHelper::CubeMapSetupHelper(
    NiRenderedCubeMap* pkCubeMap)
{
    InitializeFromCubeMap(pkCubeMap);
}
//---------------------------------------------------------------------------
void NiRenderTargetGroup::CubeMapSetupHelper::InitializeFromCubeMap(
    NiRenderedCubeMap* pkCubeMap)
{
    if (pkCubeMap == NULL)
        return;

    m_aspRenderTargetGroups[0] = NiRenderTargetGroup::Create(
        pkCubeMap->GetFaceBuffer((NiRenderedCubeMap::FaceID)0), 
        NiRenderer::GetRenderer(), true, true);
    if (m_aspRenderTargetGroups[0])
    {
        // Share DS buffer among all faces
        NiDepthStencilBuffer* pkDSBuffer = 
            m_aspRenderTargetGroups[0]->GetDepthStencilBuffer();
        for (unsigned int i = 1; i < NiRenderedCubeMap::FACE_NUM; i++)
        {
            m_aspRenderTargetGroups[i] = NiRenderTargetGroup::Create(
                pkCubeMap->GetFaceBuffer((NiRenderedCubeMap::FaceID)i), 
                NiRenderer::GetRenderer(), pkDSBuffer);
        }
    }
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRenderTargetGroup);
//---------------------------------------------------------------------------
void NiRenderTargetGroup::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiRenderTargetGroup::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiRenderTargetGroup::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiRenderTargetGroup::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiRenderTargetGroup::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiRenderTargetGroup::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
