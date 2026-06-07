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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Error.h"
#include "NiD3D10MeshMaterialBinding.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10Renderer.h"
#include <NiMesh.h>
#include <NiMeshMaterialBinding.h>

//---------------------------------------------------------------------------
NiD3D10MeshMaterialBinding::InputLayoutEntry::InputLayoutEntry() :
    m_pkInputLayout(NULL),
    m_pvInputSignature(NULL),
    m_pkNext(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10MeshMaterialBinding::InputLayoutEntry::~InputLayoutEntry()
{
    if (m_pkInputLayout)
        m_pkInputLayout->Release();
    NiDelete m_pkNext;
}
//---------------------------------------------------------------------------
NiD3D10MeshMaterialBinding::NiD3D10MeshMaterialBinding() :
    m_pkElementDescArray(NULL),
    m_uiElementCount(0),
    m_pkInputLayouts(NULL),
    m_pkCurrentInputLayout(NULL),
    m_pkIndexStreamRef(NULL)
{
    memset(m_auiStreamsToSet, 0, sizeof(m_auiStreamsToSet));
}
//---------------------------------------------------------------------------
NiD3D10MeshMaterialBinding::~NiD3D10MeshMaterialBinding()
{
    ReleaseCachedInputLayouts();
    ReleaseElementArray();
}
//---------------------------------------------------------------------------
NiD3D10MeshMaterialBindingPtr NiD3D10MeshMaterialBinding::Create(
    NiRenderObject* pkMesh, const NiSemanticAdapterTable& kAdapterTable)
{
    NiD3D10MeshMaterialBindingPtr spMMB = NiNew NiD3D10MeshMaterialBinding;

    if (spMMB->FillElementDescArray(pkMesh, kAdapterTable))
    {
        return spMMB;
    }
    else
    {
        spMMB = NULL;
        return NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10MeshMaterialBinding::UpdateInputLayout(void* pvInputSignature,
    NiUInt32 uiInputSignatureSize)
{
    InputLayoutEntry* pkEntry = m_pkInputLayouts;
    while (pkEntry)
    {
        if (pkEntry->m_pvInputSignature == pvInputSignature)
        {
            m_pkCurrentInputLayout = pkEntry->m_pkInputLayout;
            return;
        }
        pkEntry = pkEntry->m_pkNext;
    }

    NIASSERT(NiD3D10Renderer::GetRenderer() != NULL);
    ID3D10Device* pkDevice = 
        NiD3D10Renderer::GetRenderer()->GetD3D10Device();
    NIASSERT(pkDevice != NULL);

    ID3D10InputLayout* pkInputLayout = NULL;
    HRESULT hr = pkDevice->CreateInputLayout(m_pkElementDescArray, 
        m_uiElementCount, pvInputSignature, uiInputSignatureSize, 
        &pkInputLayout);

    if (FAILED(hr) || pkInputLayout == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_INPUT_LAYOUT_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (NiUInt32)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_INPUT_LAYOUT_CREATION_FAILED,
                "No error message from D3D10, but input layout is NULL.");
        }

        if (pkInputLayout)
        {
            pkInputLayout->Release();
            pkInputLayout = NULL;
        }
        return;
    }

    pkEntry = NiNew InputLayoutEntry;
    pkEntry->m_pkInputLayout = pkInputLayout;
    pkEntry->m_pvInputSignature = pvInputSignature;
    pkEntry->m_pkNext = m_pkInputLayouts;
    m_pkInputLayouts = pkEntry;

    m_pkCurrentInputLayout = pkEntry->m_pkInputLayout;
}
//---------------------------------------------------------------------------
struct D3D10CreateBindingContext : 
    public NiMeshMaterialBinding::CreateBindingBaseContext
{
    D3D10_INPUT_ELEMENT_DESC m_akVertexElements[
        D3D10_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT + 1]; // +1 for end

    NiUInt16* m_puiStreamsToSet;
    NiUInt16* m_puiLastValidStream;

    virtual bool CallBack_EndOfElementLoop(
        NiUInt32 uiStream,
        const NiDataStreamRef* pkStreamRef,
        const NiDataStreamElement kElement,
        NiDataStreamElement::Format ePackedDataFormat, 
        const NiFixedString& kRendererSemantic, 
        const NiUInt8 uiRendererSemanticIndex, 
        NiUInt32 uiPackedDataFormatComponentCount, 
        NiUInt32 uiPackedOffset);
};
//---------------------------------------------------------------------------
bool D3D10CreateBindingContext::CallBack_EndOfElementLoop(NiUInt32 uiStream,
    const NiDataStreamRef* pkStreamRef, const NiDataStreamElement,
    NiDataStreamElement::Format ePackedDataFormat,
    const NiFixedString& kRendererSemantic,
    const NiUInt8 uiRendererSemanticIndex, NiUInt32, NiUInt32 uiPackedOffset)
{
    NIASSERT(m_uiCurrentStream < D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);

    // Add new entry
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    D3D10_INPUT_CLASSIFICATION eInstanceType;
    NiUInt32 uiInstanceStep;
    if (!pkStreamRef->IsPerInstance())
    {
        eInstanceType = D3D10_INPUT_PER_VERTEX_DATA;
        uiInstanceStep = 0;
    }
    else
    {
        eInstanceType = D3D10_INPUT_PER_INSTANCE_DATA;
        uiInstanceStep = 1;
    }

    DXGI_FORMAT eDXGIFormat = 
        NiD3D10PixelFormat::DetermineDXGIFormat(ePackedDataFormat);

    if (eDXGIFormat != DXGI_FORMAT_UNKNOWN &&
        pkRenderer->DoesFormatSupportFlag(eDXGIFormat, 
        D3D10_FORMAT_SUPPORT_IA_VERTEX_BUFFER))
    {
        if (m_uiCurrentElement >=
            D3D10_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)
        {
            NiRenderer::Warning(
                __FUNCTION__ "> "
                "Malformed semantic adapter table for mesh %s: "
                "too many vertex elements in stream %d.\n"
                "    Vertex declaration cannot be created.",
                m_pkMesh->GetName(), 
                m_uiCurrentStream);
            return false;
        }

        D3D10_INPUT_ELEMENT_DESC& kCurrentElement = 
            m_akVertexElements[m_uiCurrentElement];
        kCurrentElement.SemanticName = kRendererSemantic;
        kCurrentElement.SemanticIndex = uiRendererSemanticIndex;
        kCurrentElement.Format = eDXGIFormat;
        kCurrentElement.InputSlot = m_uiCurrentStream;
        kCurrentElement.AlignedByteOffset = uiPackedOffset;
        kCurrentElement.InputSlotClass = eInstanceType;
        kCurrentElement.InstanceDataStepRate = uiInstanceStep;

        // Store the streams that will need to be set to device.
        // For each device stream that will be set, 
        // associate our stream index from the NiMesh
        m_puiStreamsToSet[m_uiCurrentStream] = (NiUInt16)uiStream;
        *m_puiLastValidStream = (NiUInt16)m_uiCurrentStream;
    }
    else
    {
        NiRenderer::Warning(
            __FUNCTION__ "> "
            "Malformed semantic adapter table for mesh %s: Data type "
            "and/or renderer semantic name %s aren't supported by "
            "D3D10.\n"
            "    Vertex declaration cannot be created.",
            m_pkMesh->GetName(), 
            kRendererSemantic);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10MeshMaterialBinding::FillElementDescArray(
    NiRenderObject* pkMesh_beforecast, 
    const NiSemanticAdapterTable& kAdapterTable)
{
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, pkMesh_beforecast);

    // This should never be called if an element desc array already exists
    NIASSERT(m_pkElementDescArray == NULL && m_uiElementCount == 0);

    NiDataStreamRef* pkIndexRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    NiDataStreamRef* pkDLRef = pkMesh->FindStreamRef(NiCommonSemantics::DISPLAYLIST());
    if (pkDLRef && !pkIndexRef)
    {
        NiRenderer::Warning(
            "%s> Failed to create a vertex declaration for "
            "mesh (pointer: 0x%X, name: %s) because the INDEX stream "
            "was removed during a platform-specific export.",
            __FUNCTION__,
            pkMesh, 
            (const char*) pkMesh->GetName());
        return false;
    }

    D3D10CreateBindingContext kContext;
    kContext.m_pkMesh = pkMesh;
    kContext.m_puiStreamsToSet = m_auiStreamsToSet;
    kContext.m_puiLastValidStream = &m_uiLastValidStream;

    m_uiLastValidStream = 0;

    if (!NiMeshMaterialBinding::CreateBinding<D3D10CreateBindingContext>
        (kContext, kAdapterTable))
    {
        return false;
    }

    // Copy to NiD3D10MeshMaterialBinding object
    m_uiElementCount = (NiUInt16)kContext.m_uiCurrentElement;
    m_pkElementDescArray = NiAlloc(D3D10_INPUT_ELEMENT_DESC, m_uiElementCount);
    memcpy(m_pkElementDescArray, kContext.m_akVertexElements, 
        m_uiElementCount * sizeof(*m_pkElementDescArray));

    // Set the index stream
    m_pkIndexStreamRef = pkIndexRef;

    return true;
}
//---------------------------------------------------------------------------
