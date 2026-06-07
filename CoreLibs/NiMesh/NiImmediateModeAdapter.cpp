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
#include "NiMeshPCH.h"

#include "NiImmediateModeAdapter.h"
#include "NiTriShapeDynamicData.h"
#include "NiLines.h"
#include "NiVertexColorProperty.h"
#include "NiAlphaProperty.h"
#include "NiZBufferProperty.h"

//---------------------------------------------------------------------------
NiImmediateModeAdapter::NiImmediateModeAdapter(
    unsigned int uiInitialNumVerts, unsigned uiInitialNumIndices) :
    m_kDefaultColor(0.0f, 0.0f, 0.0f, 1.0f),
    m_uiInitialVertexBufferSize(uiInitialNumVerts),
    m_uiInitialIndexBufferSize(uiInitialNumIndices),
    m_uiVerts(0),
    m_uiIndices(0),
    m_uiTotalVerts(0),
    m_uiTotalIndices(0),
    m_bIsIndexed(false),
    m_spIndexStream(NULL),
    m_spPositionStream(NULL),
    m_spColorStream(NULL),
    m_pkPositionArray(NULL),
    m_pkColorArray(NULL),
    m_puiIndexArray(NULL),
    m_bIsOpaque(true),
    m_eLastError(NIIMERROR_OK),
    m_bInDrawState(false),
    m_bUseProjectedCamera(true),
    m_bUseNullWindowCoords(true)
{
    // Creation of streams deferred until renderer has been created.

    m_spMesh = NiNew NiMesh();

    // Set up the correct properties
    NiZBufferProperty* pkZ = NiNew NiZBufferProperty();
    pkZ->SetZBufferTest(true);
    pkZ->SetZBufferWrite(true);
    m_spMesh->AttachProperty(pkZ);

    NiAlphaProperty* pkAlpha = NiNew NiAlphaProperty();
    m_spMesh->AttachProperty(pkAlpha);

    NiVertexColorProperty* pkVC = NiNew NiVertexColorProperty();
    pkVC->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVC->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_spMesh->AttachProperty(pkVC);

    m_spMesh->Update(0.0f);
    m_spMesh->UpdateProperties();
    m_spMesh->UpdateEffects();
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Initialize(unsigned int uiInitialNumVerts, 
    unsigned uiInitialNumIndices)
{
    // Create position stream
    m_spPositionStream = NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_FLOAT32_3,
        uiInitialNumVerts,
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE | 
        NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX, NULL);

    m_spMesh->AddStreamRef(m_spPositionStream, NiCommonSemantics::POSITION());

    // Create color stream
    m_spColorStream = NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_FLOAT32_4,
        uiInitialNumVerts,
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE | 
        NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX, NULL);

    m_spMesh->AddStreamRef(m_spColorStream, NiCommonSemantics::COLOR());

    // Create index stream
    m_spIndexStream = NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1,
        uiInitialNumIndices,
        NiDataStream::ACCESS_CPU_WRITE_VOLATILE | 
        NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX_INDEX, NULL);

    // Set submesh count to 1
    m_spMesh->SetSubmeshCount(1, 0);
}
//---------------------------------------------------------------------------
NiImmediateModeAdapter::~NiImmediateModeAdapter()
{

}
//---------------------------------------------------------------------------
bool NiImmediateModeAdapter::IsValidRendererState(NiRenderer* pkRenderer)
{
    if (!pkRenderer)
    {
        m_eLastError = NIIMERROR_NO_RENDERER;
        return false;
    }

    if (!pkRenderer->IsRenderTargetGroupActive())
    {
        m_eLastError = NIIMERROR_NO_BOUND_RENDER_TARGET;
        return false;
    }

    if (m_bUseProjectedCamera && !m_spCamera)
    {
        m_eLastError = NIIMERROR_NO_CAMERA;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiImmediateModeAdapter::BeginDrawing(
    NiPrimitiveType::Type ePrimitiveType, bool bIsIndexedPrimitive)
{
    // Deferred mesh creation
    if (!m_spPositionStream)
    {
        if (!NiRenderer::GetRenderer())
        {
            m_eLastError = NIIMERROR_NO_RENDERER;
            return false;
        }

        Initialize(m_uiInitialVertexBufferSize, m_uiInitialIndexBufferSize);
    }

    m_spMesh->SetPrimitiveType(ePrimitiveType);
    m_bIsIndexed = bIsIndexedPrimitive;
        
    // Attach or detach index stream as necessary
    const NiDataStreamRef* pkFoundIndexRef = 
        m_spMesh->GetFirstStreamRef(NiDataStream::USAGE_VERTEX_INDEX);

    if (m_bIsIndexed && !pkFoundIndexRef)
    {
        m_spMesh->AddStreamRef(m_spIndexStream,
            NiCommonSemantics::INDEX());

    }
    else if (!m_bIsIndexed && pkFoundIndexRef)
    {
        m_spMesh->RemoveStreamRef(pkFoundIndexRef);
    }

    if (!BeginDrawingInternal())
    {
        return false;
    }

    m_uiTotalVerts = 0;
    m_uiTotalIndices = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiImmediateModeAdapter::BeginDrawingInternal()
{
    // For now, ignore indexed primitive argument.  It will be used in
    // the future potentially when implementing a "real" immediate mode or
    // on NiRenderObject objects.

    if (m_bInDrawState)
    {
        m_eLastError = NIIMERROR_REDUNDANT_BEGIN;
        return false;
    }
    if (!IsValidRendererState(NiRenderer::GetRenderer()))
    {
        return false;
    }

    // Lock all the streams
    if (m_bIsIndexed)
    {
        m_puiIndexArray = (unsigned int*)m_spIndexStream->Lock(
            NiDataStream::LOCK_WRITE);
        NIASSERT(m_puiIndexArray);
    }
    else
    {
        m_puiIndexArray = NULL;
    }

    m_pkPositionArray = (NiPoint3*)m_spPositionStream->Lock(
        NiDataStream::LOCK_WRITE);
    NIASSERT(m_pkPositionArray);
    m_pkColorArray = (NiColorA*)m_spColorStream->Lock(
        NiDataStream::LOCK_WRITE);

    // Reset primitive info
    m_bIsOpaque = true;
    m_uiVerts = 0;
    m_uiIndices = 0;

    m_bInDrawState = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiImmediateModeAdapter::EndDrawing()
{
    if (!m_bInDrawState)
    {
        m_eLastError = NIIMERROR_ALREADY_ENDED;

        // Even if the end fails, the draw state is done.
        m_bInDrawState = false;

        return false;
    }

    m_bInDrawState = false;

    // Unlock streams
    if (m_bIsIndexed)
    {
        m_spIndexStream->Unlock(NiDataStream::LOCK_WRITE);
        m_puiIndexArray = NULL;

        NiDataStream::Region& kIndexRegion = m_spIndexStream->GetRegion(0);
        kIndexRegion.SetRange(m_uiIndices);
    }

    m_spPositionStream->Unlock(NiDataStream::LOCK_WRITE);
    m_pkPositionArray = NULL;    
    NiDataStream::Region& kPositionRegion = m_spPositionStream->GetRegion(0);
    kPositionRegion.SetRange(m_uiVerts);

    m_spColorStream->Unlock(NiDataStream::LOCK_WRITE);
    m_pkColorArray = NULL;
    NiDataStream::Region& kColorRegion = m_spColorStream->GetRegion(0);
    kColorRegion.SetRange(m_uiVerts);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!IsValidRendererState(pkRenderer))
    {
        return false;
    }

    // Early out for no geometry.
    if (m_uiVerts == 0 || m_uiIndices == 0 && m_bIsIndexed)
        return true;

    // Set alpha state appropriately
    NiAlphaProperty* pkAlpha = (NiAlphaProperty*)m_spMesh->
        GetProperty(NiProperty::ALPHA);
    NIASSERT(pkAlpha);
    pkAlpha->SetAlphaBlending(!m_bIsOpaque);

    // Set camera
    if (m_bUseProjectedCamera)
    {
        NIASSERT(m_spCamera);
        pkRenderer->SetCameraData(m_spCamera);
    }
    else if (m_bUseNullWindowCoords)
    {
        pkRenderer->SetScreenSpaceCameraData();
    }
    else
    {
        pkRenderer->SetScreenSpaceCameraData(&m_kWindowCoords);
    }

    m_spMesh->RenderImmediate(pkRenderer);

    m_uiTotalVerts += m_uiVerts;
    m_uiTotalIndices += m_uiTotalIndices;

    return true;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(const NiPoint3* pkVert0)
{
    NIASSERT(pkVert0);
    if (!IsValidAppend(1, 1))
    {
        return;
    }

    SubmitIfCantFit(1, 1);
    CheckAlphaBlending(m_kDefaultColor);

    m_pkPositionArray[m_uiVerts] = *pkVert0;
    m_pkColorArray[m_uiVerts] = m_kDefaultColor;

    if (m_bIsIndexed)
    {
        m_puiIndexArray[m_uiIndices++] = m_uiVerts;
    }

    m_uiVerts++;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(const NiPoint3* pkVert0,
    const NiPoint3* pkVert1)
{
    NIASSERT(pkVert0);
    NIASSERT(pkVert1);
    if (!IsValidAppend(2, 2))
    {
        return;
    }

    SubmitIfCantFit(2, 2);
    CheckAlphaBlending(m_kDefaultColor);

    m_pkPositionArray[m_uiVerts] = *pkVert0;
    m_pkPositionArray[m_uiVerts + 1] = *pkVert1;

    m_pkColorArray[m_uiVerts] = m_kDefaultColor;
    m_pkColorArray[m_uiVerts + 1] = m_kDefaultColor;

    if (m_bIsIndexed)
    {
        m_puiIndexArray[m_uiIndices++] = m_uiVerts;
        m_puiIndexArray[m_uiIndices++] = m_uiVerts + 1;
    }

    m_uiVerts += 2;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(const NiPoint3* pkVert0,
    const NiPoint3* pkVert1, const NiPoint3* pkVert2)
{
    NIASSERT(pkVert0);
    NIASSERT(pkVert1);
    NIASSERT(pkVert2);
    if (!IsValidAppend(3, 3))
    {
        return;
    }

    SubmitIfCantFit(3, 3);
    CheckAlphaBlending(m_kDefaultColor);

    m_pkPositionArray[m_uiVerts] = *pkVert0;
    m_pkPositionArray[m_uiVerts + 1] = *pkVert1;
    m_pkPositionArray[m_uiVerts + 2] = *pkVert2;

    m_pkColorArray[m_uiVerts] = m_kDefaultColor;
    m_pkColorArray[m_uiVerts + 1] = m_kDefaultColor;
    m_pkColorArray[m_uiVerts + 2] = m_kDefaultColor;

    if (m_bIsIndexed)
    {
        m_puiIndexArray[m_uiIndices++] = m_uiVerts;
        m_puiIndexArray[m_uiIndices++] = m_uiVerts + 1;
        m_puiIndexArray[m_uiIndices++] = m_uiVerts + 2;
    }

    m_uiVerts += 3;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(const NiPoint3* pkVert0,
    const NiPoint3* pkVert1, const NiPoint3* pkVert2,
    const NiPoint3* pkVert3)
{
    NIASSERT(pkVert0);
    NIASSERT(pkVert1);
    NIASSERT(pkVert2);
    NIASSERT(pkVert3);
    if (!IsValidAppend(4, 4))
    {
        return;
    }

    SubmitIfCantFit(4, 4);
    CheckAlphaBlending(m_kDefaultColor);

    m_pkPositionArray[m_uiVerts] = *pkVert0;
    m_pkPositionArray[m_uiVerts + 1] = *pkVert1;
    m_pkPositionArray[m_uiVerts + 2] = *pkVert2;
    m_pkPositionArray[m_uiVerts + 3] = *pkVert3;

    m_pkColorArray[m_uiVerts] = m_kDefaultColor;
    m_pkColorArray[m_uiVerts + 1] = m_kDefaultColor;
    m_pkColorArray[m_uiVerts + 2] = m_kDefaultColor;
    m_pkColorArray[m_uiVerts + 3] = m_kDefaultColor;

    if (m_bIsIndexed)
    {
        m_puiIndexArray[m_uiIndices++] = m_uiVerts;
        m_puiIndexArray[m_uiIndices++] = m_uiVerts + 1;
        m_puiIndexArray[m_uiIndices++] = m_uiVerts + 2;
        m_puiIndexArray[m_uiIndices++] = m_uiVerts + 3;
    }

    m_uiVerts += 4;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(unsigned int uiNumVerts,
    const NiPoint3* pkVerts)
{
    NIASSERT(pkVerts);

    if (!IsValidAppend(uiNumVerts, uiNumVerts))
    {
        return;
    }
    
    SubmitIfCantFit(uiNumVerts, uiNumVerts);
    CheckAlphaBlending(m_kDefaultColor);

    if (m_bIsIndexed)
    {
        for (unsigned int i = 0; i < uiNumVerts; i++)
        {
            m_pkPositionArray[m_uiVerts + i] = pkVerts[i];
            m_pkColorArray[m_uiVerts + i] = m_kDefaultColor;
            m_puiIndexArray[m_uiIndices + i] = m_uiVerts + i;
        }

        m_uiIndices += uiNumVerts;
    }
    else
    {
        for (unsigned int i = 0; i < uiNumVerts; i++)
        {
            m_pkPositionArray[m_uiVerts + i] = pkVerts[i];
            m_pkColorArray[m_uiVerts + i] = m_kDefaultColor;
        }
    }

    m_uiVerts += uiNumVerts;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(unsigned int uiNumVerts,
    const NiPoint3* pkVerts, const NiColorA* pkColors)
{
    NIASSERT(pkVerts);
    NIASSERT(pkColors);

    if (!IsValidAppend(uiNumVerts, uiNumVerts))
    {
        return;
    }

    SubmitIfCantFit(uiNumVerts, uiNumVerts);

    if (m_bIsIndexed)
    {
        for (unsigned int i = 0; i < uiNumVerts; i++)
        {
            m_pkPositionArray[m_uiVerts + i] = pkVerts[i];
            m_pkColorArray[m_uiVerts + i] = pkColors[i];
            m_puiIndexArray[m_uiIndices + i] = m_uiVerts + i;
            CheckAlphaBlending(pkColors[i]);
        }

        m_uiIndices += uiNumVerts;
    }
    else
    {
        for (unsigned int i = 0; i < uiNumVerts; i++)
        {
            m_pkPositionArray[m_uiVerts + i] = pkVerts[i];
            m_pkColorArray[m_uiVerts + i] = pkColors[i];
            CheckAlphaBlending(pkColors[i]);
        }
    }

    m_uiVerts += uiNumVerts;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(unsigned int uiNumVerts, 
    const NiPoint3* pkVerts, unsigned int uiNumIndices,
    const unsigned int* puiIndices)
{
    NIASSERT(pkVerts);
    NIASSERT(puiIndices);

    if (!IsValidAppend(uiNumVerts, uiNumIndices))
    {
        return;
    }

    if (m_bIsIndexed)
    {
        SubmitIfCantFit(uiNumVerts, uiNumIndices);
        CheckAlphaBlending(m_kDefaultColor);

        unsigned int i;
        for (i = 0; i < uiNumVerts; i++)
        {
            m_pkPositionArray[m_uiVerts + i] = pkVerts[i];
            m_pkColorArray[m_uiVerts + i] = m_kDefaultColor;
        }

        for (i = 0; i < uiNumIndices; i++)
        {
            m_puiIndexArray[m_uiIndices + i] = puiIndices[i] + m_uiVerts;
        }

        m_uiIndices += uiNumIndices;
        m_uiVerts += uiNumVerts;
    }
    else
    {
        SubmitIfCantFit(uiNumIndices, uiNumIndices);
        CheckAlphaBlending(m_kDefaultColor);

        for (unsigned int i = 0; i < uiNumIndices; i++)
        {
            m_pkPositionArray[m_uiVerts] = pkVerts[puiIndices[i]];
            m_pkColorArray[m_uiVerts] = m_kDefaultColor;

            m_uiVerts++;
        }
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::Append(unsigned int uiNumVerts,
    const NiPoint3* pkVerts, unsigned int uiNumIndices, 
    const unsigned int* puiIndices, const NiColorA* pkColors)
{
    NIASSERT(pkVerts);
    NIASSERT(puiIndices);
    NIASSERT(pkColors);

    if (!IsValidAppend(uiNumVerts, uiNumIndices))
    {
        return;
    }

    if (m_bIsIndexed)
    {
        SubmitIfCantFit(uiNumVerts, uiNumIndices);

        unsigned int i;
        for (i = 0; i < uiNumVerts; i++)
        {
            m_pkPositionArray[m_uiVerts + i] = pkVerts[i];
            m_pkColorArray[m_uiVerts + i] = pkColors[i];
            CheckAlphaBlending(pkColors[i]);
        }

        for (i = 0; i < uiNumIndices; i++)
        {
            m_puiIndexArray[m_uiIndices + i] = puiIndices[i] + m_uiVerts;
        }

        m_uiIndices += uiNumIndices;
        m_uiVerts += uiNumVerts;
    }
    else
    {
        SubmitIfCantFit(uiNumIndices, uiNumIndices);

        for (unsigned int i = 0; i < uiNumIndices; i++)
        {
            unsigned int uiIndex = puiIndices[i];
            m_pkPositionArray[m_uiVerts] = pkVerts[uiIndex];
            m_pkColorArray[m_uiVerts] = pkColors[uiIndex];
            CheckAlphaBlending(pkColors[uiIndex]);

            m_uiVerts++;
        }
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::SetNumMaxVertices(unsigned int uiMaxVerts)
{
    if (!IsValidStateChange())
    {
        return;
    }

    // Vertex streams may not have been created yet.
    if (m_spPositionStream)
    {
        m_spPositionStream->Resize(
            uiMaxVerts * m_spPositionStream->GetStride());
        m_spColorStream->Resize(uiMaxVerts * m_spColorStream->GetStride());
    }
    else
    {
        m_uiInitialVertexBufferSize = uiMaxVerts;
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::SetNumMaxIndices(unsigned int uiMaxIndices)
{
    if (!IsValidStateChange())
    {
        return;
    }

    // Index stream may not have been created yet)
    if (m_spIndexStream)
    {
        m_spIndexStream->Resize(uiMaxIndices * m_spIndexStream->GetStride());
    }
    else
    {
        m_uiInitialIndexBufferSize = uiMaxIndices;
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::SetCurrentCamera(NiCamera* pkCamera)
{
    if (!IsValidStateChange())
    {
        return;
    }

    m_spCamera = pkCamera;
    m_bUseProjectedCamera = true;
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::SetScreenSpaceCameraData(
    const NiRect<float>* pkViewPort)
{
    if (!IsValidStateChange())
    {
        return;
    }

    m_bUseProjectedCamera = false;
    if (pkViewPort)
    {
        m_kWindowCoords = *pkViewPort;
        m_bUseNullWindowCoords = false;
    }
    else
    {
        m_bUseNullWindowCoords = true;
    }
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::SetWorldTransform(const NiTransform& kTrans)
{
    if (!IsValidStateChange())
    {
        return;
    }

    m_spMesh->SetLocalTransform(kTrans);
    m_spMesh->Update(0.0f);
}
//---------------------------------------------------------------------------
void NiImmediateModeAdapter::SetZBufferProperty(bool bCompareEnable,
    bool bWriteEnable)
{
    if (!IsValidStateChange())
    {
        return;
    }

    NiZBufferProperty* pkZ = (NiZBufferProperty*)
        m_spMesh->GetProperty(NiProperty::ZBUFFER);
    NIASSERT(pkZ);

    pkZ->SetZBufferTest(bCompareEnable);
    pkZ->SetZBufferWrite(bWriteEnable);
}
//---------------------------------------------------------------------------
