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

#include "NiMeshPCH.h"
#include "NiMesh.h"
#include "NiMeshScreenElements.h"
#include "NiDataStreamElementLock.h"
#include <NiVertexColorProperty.h>

NiImplementRTTI(NiMeshScreenElements, NiMesh);

const NiUInt16 NiMeshScreenElements::ms_usInvalid = (NiUInt16)~0;

//---------------------------------------------------------------------------
NiColorA NiMeshScreenElements::BGRAToColorA(const NiRGBA& kRGBA)
{
    NiColorA kColor;
    kColor.r = (float)kRGBA.b() / 255.0f;
    kColor.g = (float)kRGBA.g() / 255.0f;
    kColor.b = (float)kRGBA.r() / 255.0f;
    kColor.a = (float)kRGBA.a() / 255.0f;

    return kColor;
}
//---------------------------------------------------------------------------
NiRGBA NiMeshScreenElements::ColorAToBGRA(const NiColorA& kColor)
{
    NiRGBA kRGBA;
    kRGBA.r() = (NiUInt8)(kColor.b * 255.0f);
    kRGBA.g() = (NiUInt8)(kColor.g * 255.0f);
    kRGBA.b() = (NiUInt8)(kColor.r * 255.0f);
    kRGBA.a() = (NiUInt8)(kColor.a * 255.0f);

    return kRGBA;
}
//---------------------------------------------------------------------------
NiMeshScreenElements* NiMeshScreenElements::Create(
    NiRenderer* pkRenderer, 
    const float fXOffset, 
    const float fYOffset,
    const unsigned int uiWidth, 
    const unsigned int uiHeight,
    const NiRenderer::DisplayCorner eCorner, 
    const bool bForceSafeZone)
{
    unsigned int uiX, uiY;

    pkRenderer->GetOnScreenCoord(fXOffset, fYOffset, uiWidth, uiHeight, 
        uiX, uiY, eCorner, bForceSafeZone);

    NiMeshScreenElements* pkScreenElements = NiNew NiMeshScreenElements(
        false, true, 1);

    float fX, fY;
    float fWidth, fHeight;
    pkRenderer->ConvertFromPixelsToNDC(uiX, uiY, fX, fY);
    pkRenderer->ConvertFromPixelsToNDC(uiWidth, uiHeight, fWidth, fHeight);

    // We know that the polygon handle is zero and will use it directly in
    // the vertex and texture coordinate assignments.
    pkScreenElements->Insert(4);
    pkScreenElements->SetRectangle(0, fX, fY, fWidth, fHeight);
    pkScreenElements->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
    pkScreenElements->UpdateBound();

    return pkScreenElements;
}
//---------------------------------------------------------------------------
NiMeshScreenElements* NiMeshScreenElements::Create(NiTexture* pkTexture,
    NiTexturingProperty::ApplyMode eMode)
{
    NiMeshScreenElements* pkScreenElements = NiNew NiMeshScreenElements(
        false, true, 1);

    NiTexturingProperty *pkTexProp = NiNew NiTexturingProperty();
    pkTexProp->SetBaseTexture(pkTexture);
    pkTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    pkTexProp->SetApplyMode(eMode);
    pkTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    NiAlphaProperty *pkAlphaProp = NiNew NiAlphaProperty();
    pkAlphaProp->SetAlphaBlending(true);

    NiVertexColorProperty *pkVCProp = NiNew NiVertexColorProperty(); 
    pkVCProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVCProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);

    NiZBufferProperty *pkZBufProp = NiNew NiZBufferProperty();
    pkZBufProp->SetZBufferTest(false);
    pkZBufProp->SetZBufferWrite(true);

    pkScreenElements->AttachProperty(pkTexProp);
    pkScreenElements->AttachProperty(pkAlphaProp);
    pkScreenElements->AttachProperty(pkZBufProp);
    pkScreenElements->AttachProperty(pkVCProp);
    pkScreenElements->UpdateProperties();

    return pkScreenElements;
}
//---------------------------------------------------------------------------
NiMeshScreenElements::NiMeshScreenElements(
    bool bWantNormals,
    bool bWantColors, 
    NiUInt16 usNumTextureSets, 
    NiInt32 iMaxPQuantity,
    NiInt32 iPGrowBy, 
    NiInt32 iMaxVQuantity, 
    NiInt32 iVGrowBy,
    NiInt32 iMaxTQuantity, 
    NiInt32 iTGrowBy, 
    NiUInt8 uiAccessMask,
    bool bForceFixedFunctionColors) : 
    NiMesh(),
    m_pPolygon(0),
    m_puiPIndexer(0),
    m_usMaxPQuantity(0),
    m_usPGrowBy(0),
    m_usPQuantity(0),
    m_usMaxVQuantity(0),
    m_usVGrowBy(0),
    m_usMaxIQuantity(0),
    m_usIGrowBy(0),
    m_eColorFormat(NiDataStreamElement::F_FLOAT32_4),
    m_bBoundNeedsUpdate(false),
    m_bHasNormals(false),
    m_bHasColors(false),
    m_uiNumTextureSets(0),
    m_pLockedVertexData(NULL),
    m_pLockedColorData(NULL),
    m_pLockedNormalData(NULL),
    m_iTextureDataCount(0),
    m_ppLockedTextureData(NULL),
    m_pLockedIndexData(NULL),
    m_uiLockMask(0),
    m_bBatchLock(false)
{
    NIASSERT(iMaxPQuantity > 0 && iPGrowBy > 0);
    NIASSERT(iMaxVQuantity > 0 && iVGrowBy > 0);
    NIASSERT(iMaxTQuantity > 0 && iTGrowBy > 0);

    SetInitialColorFormat(bForceFixedFunctionColors);

    // polygons
    m_usMaxPQuantity =
        (NiUInt16)(iMaxPQuantity > 0 ? iMaxPQuantity : 1);
    SetPGrowBy(iPGrowBy);
    m_pPolygon = NiAlloc(Polygon, m_usMaxPQuantity);
    m_puiPIndexer = NiAlloc(NiUInt16,m_usMaxPQuantity);
    memset(m_puiPIndexer, 0xFF, m_usMaxPQuantity * sizeof(NiUInt16));

    // vertices
    m_usMaxVQuantity =
        (NiUInt16)(iMaxVQuantity > 0 ? iMaxVQuantity : 1);
    SetVGrowBy(iVGrowBy);

    AddStream(NiCommonSemantics::POSITION(), 0, 
        NiDataStreamElement::F_FLOAT32_3, 
        iMaxVQuantity, uiAccessMask, 
        NiDataStream::USAGE_VERTEX);

    // normals
    if (bWantNormals)
    {
        m_bHasNormals = true;

        AddStream( NiCommonSemantics::NORMAL(), 0,
            NiDataStreamElement::F_FLOAT32_3,
            iMaxVQuantity, uiAccessMask, 
            NiDataStream::USAGE_VERTEX);
    }

    // colors (initial maximum storage assumes all rectangles)
    if (bWantColors)
    {
        m_bHasColors = true;
        AddStream( NiCommonSemantics::COLOR(), 0,
            m_eColorFormat,
            iMaxVQuantity, uiAccessMask, 
            NiDataStream::USAGE_VERTEX);
    }

    // texture coordinates (initial maximum storage assumes all rectangles)
    if (usNumTextureSets > 0)
    {
        m_uiNumTextureSets = usNumTextureSets;

        for (NiUInt32 ui = 0; ui < usNumTextureSets; ui++)
        {
            AddStream(NiCommonSemantics::TEXCOORD(),
                ui, 
                NiDataStreamElement::F_FLOAT32_2,
                iMaxVQuantity, uiAccessMask, NiDataStream::USAGE_VERTEX);
        }
    }

    // triangles (initial maximum storage assumes all rectangles)
    m_usMaxIQuantity =
        (NiUInt16)(iMaxTQuantity > 0 ? 3 * iMaxTQuantity : 3);
    SetTGrowBy(iTGrowBy);

    // Set storage for indices.
    AddStream( NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UINT16_1,
        m_usMaxIQuantity, uiAccessMask, 
        NiDataStream::USAGE_VERTEX_INDEX);

    SetSubmeshCount(1);

    SetAttributesRegionRange(0);
    SetIndicesRange(0);

    if (m_bHasNormals)
    {
        NiDataStream* pkNormalDS = GetNormalDataStream();
        NiPoint3* pkNormal = (NiPoint3*)pkNormalDS->Lock(
            NiDataStream::LOCK_WRITE);
        for (NiUInt16 i = 0; i < m_usMaxVQuantity; i++)
            pkNormal[i] = -NiPoint3::UNIT_Z;
        pkNormalDS->Unlock(NiDataStream::LOCK_WRITE);
    }
}
//---------------------------------------------------------------------------
NiMeshScreenElements::NiMeshScreenElements() : NiMesh(),
    m_pPolygon(0),
    m_puiPIndexer(0),
    m_usMaxPQuantity(0),
    m_usPGrowBy(0),
    m_usPQuantity(0),
    m_usMaxVQuantity(0),
    m_usVGrowBy(0),
    m_usMaxIQuantity(0),
    m_usIGrowBy(0),
    m_eColorFormat(NiDataStreamElement::F_FLOAT32_4),
    m_bBoundNeedsUpdate(false),
    m_bHasNormals(false),
    m_bHasColors(false),
    m_uiNumTextureSets(0),
    m_pLockedVertexData(NULL),
    m_pLockedColorData(NULL),
    m_pLockedNormalData(NULL),
    m_iTextureDataCount(0),
    m_ppLockedTextureData(NULL),
    m_pLockedIndexData(NULL),
    m_uiLockMask(0),
    m_bBatchLock(false)
{
    SetInitialColorFormat(false);
}
//---------------------------------------------------------------------------
NiMeshScreenElements::~NiMeshScreenElements()
{
    NiFree(m_ppLockedTextureData);
    NiFree( m_pPolygon);
    NiFree( m_puiPIndexer);
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetInitialColorFormat(
    bool bForceFixedFunctionColors)
{
#if defined(WIN32)
    if (bForceFixedFunctionColors ||
        NiRenderer::GetRenderer() != NULL &&
        NiRenderer::GetRenderer()->GetRendererID() == 
        NiSystemDesc::RENDERER_DX9)
    {
        m_eColorFormat = NiDataStreamElement::F_NORMUINT8_4_BGRA;
    }
#elif defined(_WII)
    m_eColorFormat = NiDataStreamElement::F_NORMUINT8_4;
#else
    NI_UNUSED_ARG(bForceFixedFunctionColors);
    m_eColorFormat = NiDataStreamElement::F_FLOAT32_4;
#endif
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::BeginBatchUpdate(NiUInt8 uiLockMask)
{
    NIASSERT(!m_bBatchLock)

    NiDataStream* pkStream;

    pkStream = GetVertexDataStream();
    m_pLockedVertexData = 
        (NiPoint3*) pkStream->Lock(uiLockMask);

    pkStream = GetIndexDataStream();
    m_pLockedIndexData = 
        (NiUInt16*) pkStream->Lock(uiLockMask);

    if (m_bHasNormals)
    {
        pkStream = GetNormalDataStream();
        m_pLockedNormalData = 
            (NiPoint3*) pkStream->Lock(uiLockMask);
    }

    if (m_bHasColors)
    {
        pkStream = GetColorDataStream();
        m_pLockedColorData = pkStream->Lock(uiLockMask);
    }

    if (m_ppLockedTextureData == NULL)
        m_ppLockedTextureData = NiAlloc(NiPoint2*, m_uiNumTextureSets);
    
    for (NiUInt32 uiSet=0; uiSet < m_uiNumTextureSets; uiSet++)
    {
        pkStream = GetTexCoordDataStream(uiSet);
        m_ppLockedTextureData[uiSet] = 
            (NiPoint2*) pkStream->Lock(uiLockMask);
    }

    m_uiLockMask = uiLockMask;
    m_bBatchLock = true;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::EndBatchUpdate()
{
    NIASSERT(m_bBatchLock)

    NiDataStream* pkStream;

    pkStream = GetVertexDataStream();
    pkStream->Unlock(m_uiLockMask);
    m_pLockedVertexData = NULL;

    pkStream = GetIndexDataStream();
    pkStream->Unlock(m_uiLockMask);
    m_pLockedIndexData = NULL;

    if (m_bHasNormals)
    {
        pkStream = GetNormalDataStream();
        pkStream->Unlock(m_uiLockMask);
        m_pLockedNormalData = NULL;
    }

    if (m_bHasColors)
    {
        pkStream = GetColorDataStream();
        pkStream->Unlock(m_uiLockMask);
        m_pLockedColorData = NULL;
    }

    for (NiUInt32 uiSet=0; uiSet < m_uiNumTextureSets; uiSet++)
    {
        pkStream = GetTexCoordDataStream(uiSet);
        pkStream->Unlock(m_uiLockMask);
        m_ppLockedTextureData[uiSet] = NULL;
    }

    m_uiLockMask = 0;
    m_bBatchLock = false;
}
//---------------------------------------------------------------------------
NiPoint3* NiMeshScreenElements::LockVertexStream(NiUInt8 uiLockMask)
{
    if (m_pLockedVertexData != NULL)
    {
        return m_pLockedVertexData;
    }
    else
    {
        return (NiPoint3*) (GetVertexDataStream())->Lock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
const NiPoint3* NiMeshScreenElements::LockVertexStreamRead() const
{
    if (m_pLockedVertexData != NULL)
    {
        return m_pLockedVertexData;
    }
    else
    {
        return (const NiPoint3*) ((NiDataStream*)GetVertexDataStream())->Lock(
            NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
NiUInt16* NiMeshScreenElements::LockIndexStream(NiUInt8 uiLockMask)
{
    if (m_pLockedIndexData != NULL)
    {
        return m_pLockedIndexData;
    }
    else
    {
        return (NiUInt16*) (GetIndexDataStream())->Lock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
const NiUInt16* NiMeshScreenElements::LockIndexStreamRead() const
{
    if (m_pLockedIndexData != NULL)
    {
        return m_pLockedIndexData;
    }
    else
    {
        return (const NiUInt16*) ((NiDataStream*)GetIndexDataStream())->Lock(
            NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
NiPoint3* NiMeshScreenElements::LockNormalStream(NiUInt8 uiLockMask)
{
    if (m_pLockedNormalData != NULL)
    {
        return m_pLockedNormalData;
    }
    else
    {
        if (m_bHasNormals)
        {
            return (NiPoint3*) (GetNormalDataStream())->Lock(uiLockMask);
        }
        else
        {
            return NULL;
        }
    }
}
//---------------------------------------------------------------------------
const NiPoint3* NiMeshScreenElements::LockNormalStreamRead() const
{
    if (m_pLockedNormalData != NULL)
    {
        return m_pLockedNormalData;
    }
    else
    {
        if (m_bHasNormals)
        {
            return (const NiPoint3*) 
                ((NiDataStream*)GetNormalDataStream())->Lock(
                NiDataStream::LOCK_READ);
        }
        else
        {
            return NULL;
        }
    }
}
//---------------------------------------------------------------------------
void* NiMeshScreenElements::LockColorStream(NiUInt8 uiLockMask)
{
    if (m_pLockedColorData != NULL)
    {
        return m_pLockedColorData;
    }
    else
    {
        if (m_bHasColors)
        {
            return GetColorDataStream()->Lock(uiLockMask);
        }
        else
        {
            return NULL;
        }
    }
}
//---------------------------------------------------------------------------
const void* NiMeshScreenElements::LockColorStreamRead() const
{
    if (m_pLockedColorData != NULL)
    {
        return m_pLockedColorData;
    }
    else
    {
        if (m_bHasColors)
        {
            return ((NiDataStream*)GetColorDataStream())->Lock(
                NiDataStream::LOCK_READ);
        }
        else
        {
            return NULL;
        }
    }
}
//---------------------------------------------------------------------------
NiPoint2* NiMeshScreenElements::LockTexCoordStream(
    unsigned int uiSet,
    NiUInt8 uiLockMask)
{
    if (m_ppLockedTextureData != NULL)
    {
        NIASSERT(uiSet < m_uiNumTextureSets)

        return m_ppLockedTextureData[uiSet];
    }
    else
    {
        if (uiSet < m_uiNumTextureSets)
        {
            return (NiPoint2*) (GetTexCoordDataStream(uiSet))->
                Lock(uiLockMask);
        }
        else
        {
            return NULL;
        }
    }
}
//---------------------------------------------------------------------------
const NiPoint2* NiMeshScreenElements::LockTexCoordStreamRead(
    unsigned int uiSet) const
{
    if (m_ppLockedTextureData != NULL)
    {
        NIASSERT(uiSet < m_uiNumTextureSets)

        return m_ppLockedTextureData[uiSet];
    }
    else
    {
        if (uiSet < m_uiNumTextureSets)
        {
            return (const NiPoint2*) 
                ((NiDataStream*)GetTexCoordDataStream(uiSet))->Lock(
                NiDataStream::LOCK_READ);
        }
        else
        {
            return NULL;
        }
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockVertexStream(NiUInt8 uiLockMask)
{
    if (!m_bBatchLock)
    {
        GetVertexDataStream()->Unlock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockIndexStream(NiUInt8 uiLockMask)
{
    if (!m_bBatchLock)
    {
        GetIndexDataStream()->Unlock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockColorStream(NiUInt8 uiLockMask)
{
    if (!m_bBatchLock && m_bHasColors)
    {
        GetColorDataStream()->Unlock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockNormalStream(NiUInt8 uiLockMask)
{
    if (!m_bBatchLock && m_bHasColors)
    {
        GetNormalDataStream()->Unlock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockTexCoordStream(
    unsigned int uiSet,
    NiUInt8 uiLockMask)
{
    if (!m_bBatchLock && uiSet < m_uiNumTextureSets)
    {
        GetTexCoordDataStream(uiSet)->Unlock(uiLockMask);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockVertexStreamRead() const
{
    if (!m_bBatchLock)
    {
        ((NiDataStream*)GetVertexDataStream())->Unlock(
            NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockIndexStreamRead() const
{
    if (!m_bBatchLock)
    {
        ((NiDataStream*)GetIndexDataStream())->Unlock(NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockColorStreamRead() const
{
    if (!m_bBatchLock && m_bHasColors)
    {
        ((NiDataStream*)GetColorDataStream())->Unlock(NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockNormalStreamRead() const
{
    if (!m_bBatchLock && m_bHasColors)
    {
        ((NiDataStream*)GetNormalDataStream())->Unlock(
            NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UnlockTexCoordStreamRead(unsigned int uiSet) const
{
    if (!m_bBatchLock && uiSet < m_uiNumTextureSets)
    {
        ((NiDataStream*)GetTexCoordDataStream(uiSet))->Unlock(
            NiDataStream::LOCK_READ);
    }
}
//---------------------------------------------------------------------------
NiUInt16 NiMeshScreenElements::GetTextureSets() const
{
    return (NiUInt16)m_uiNumTextureSets;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetAttributesRegionRange(NiUInt32 uiRange)
{
    // SetAttributesRegionRange sets the region range on the verts, normals,
    // colors, and texcoords. This simulates the "active" region

    // Change range of verts region
    GetVertexDataStream()->GetRegion(0).SetRange(uiRange);

    // Change range of normals region
    if (m_bHasNormals)
        GetNormalDataStream()->GetRegion(0).SetRange(uiRange);
    
    // Change range of colors region
    if (m_bHasColors)
        GetColorDataStream()->GetRegion(0).SetRange(uiRange);

    // Change range of tex coords region for all sets
    for (NiUInt32 ui = 0; ui < m_uiNumTextureSets; ui++)
        GetTexCoordDataStream(ui)->GetRegion(0).SetRange(uiRange);
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::ResizeStream(NiDataStream* pkStream, 
    size_t stElementSize, NiUInt16 usNewMaxQuantity)
{
    NIASSERT(!m_bBatchLock);

    NiUInt32 uiElementCount = pkStream->GetTotalCount();
    size_t iNumBytes = stElementSize * uiElementCount;
    NiUInt8* pkTempData = NULL;

    bool bCopyData = 
        (pkStream->GetAccessMask() & NiDataStream::ACCESS_CPU_READ) != 0;

    // copy to temp buffer, if possible
    if (bCopyData)
    {
        pkTempData = NiAlloc(NiUInt8, iNumBytes);

        NiUInt8* pkData = (NiUInt8*)pkStream->Lock(NiDataStream::LOCK_READ);
        NIASSERT(pkData != NULL);
        NiMemcpy(pkTempData, pkData, iNumBytes);
        pkStream->Unlock(NiDataStream::LOCK_READ);
    }

    // resize original buffer
    pkStream->Resize((NiUInt32)(usNewMaxQuantity * stElementSize));

    if (bCopyData)
    {
        // copy back to original stream
        NiUInt8* pkNewData = (NiUInt8*)pkStream->Lock(
            NiDataStream::LOCK_WRITE);
        NIASSERT(pkNewData);
        NiMemcpy(pkNewData, pkTempData, iNumBytes);
        pkStream->Unlock(NiDataStream::LOCK_WRITE);
    }

    NiFree(pkTempData);
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::ResizeAttributeStreams(
    NiUInt16 usNewMaxQuantity)
{
    NiInt32 iDelta = (NiInt32)usNewMaxQuantity - (NiInt32)m_usMaxVQuantity;

    if (iDelta <= 0)
        return;

    NIASSERT(!m_bBatchLock);

    NiUInt16 usChunks;
    float fRatio = (float)iDelta / (float)m_usVGrowBy;
    usChunks = 1 + (NiUInt16)(fRatio + 0.5f);
    usNewMaxQuantity = m_usMaxVQuantity + usChunks * m_usVGrowBy;

    NiDataStream* pkStream;

    pkStream = GetVertexDataStream();
    ResizeStream(pkStream, sizeof(NiPoint3), usNewMaxQuantity);

    // resize normals
    if (m_bHasNormals)
    {
        pkStream = GetNormalDataStream();
        ResizeStream(pkStream, sizeof(NiPoint3), usNewMaxQuantity);
    }

    // resize colors
    if (m_bHasColors)
    {
        pkStream = GetColorDataStream();

#if defined(_WII)
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4);
        ResizeStream(pkStream, sizeof(NiRGBA), usNewMaxQuantity);
#else
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4 ||
            m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA);
        ResizeStream(
            pkStream, 
            m_eColorFormat == NiDataStreamElement::F_FLOAT32_4 ? 
            sizeof(NiColorA) : sizeof(NiRGBA), 
            usNewMaxQuantity);
#endif
    }

    // Resize texture coordinates.
    for (NiUInt32 uiSet=0; uiSet < m_uiNumTextureSets; uiSet++)
    {
        pkStream = GetTexCoordDataStream(uiSet);
        ResizeStream(pkStream, sizeof(NiPoint2), usNewMaxQuantity);
    }

    m_usMaxVQuantity = usNewMaxQuantity;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::ResizeIndexStream(NiUInt16
    usNewMaxIQuantity)
{
    NiInt32 iDelta = (NiInt32)usNewMaxIQuantity - (NiInt32)m_usMaxIQuantity;
    if (iDelta <= 0)
        return;

    NIASSERT(!m_bBatchLock);

    float fRatio = (float)iDelta / (float)m_usIGrowBy;
    NiUInt16 usChunks = 1 + (NiUInt16)(fRatio + 0.5f);

    usNewMaxIQuantity = m_usMaxIQuantity + usChunks * m_usIGrowBy;

    NiDataStream* pkStream = GetIndexDataStream();
    ResizeStream(pkStream, sizeof(NiUInt16), usNewMaxIQuantity);

    m_usMaxIQuantity = usNewMaxIQuantity;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetIndicesRange(NiUInt32 uiRange)
{
    // Change range of indices region
    GetIndexDataStream()->GetRegion(0).SetRange(uiRange);
    UpdateCachedPrimitiveCount();
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::IsValid(NiInt32 iPolygon) const
{
    return 0 <= iPolygon && iPolygon < (NiInt32)m_usMaxPQuantity &&
        m_puiPIndexer[iPolygon] != ms_usInvalid;
}
//---------------------------------------------------------------------------
NiInt32 NiMeshScreenElements::Insert(NiUInt16 usNumVertices,
    NiUInt16 usNumTriangles, const NiUInt32* auiTriList)
{
    NIASSERT(usNumVertices >= 3);
    if (usNumVertices < 3)
        return -1;

    NIASSERT(usNumTriangles == 0 || auiTriList);
    if (usNumTriangles > 0 && !auiTriList)
        return -1;

    // If the input number of triangles is zero, the polygon is assumed to be
    // convex.  A triangle fan is used to represent it.
    if (usNumTriangles == 0)
        usNumTriangles = usNumVertices - 2;

    // resize polygon array (if necessary)
    NiUInt16 usPolygon = m_usPQuantity++;
    NiUInt16 usNewMaxQuantity;
    NiInt32 iNumBytes;
    NiUInt32 uiDestBytes;
    if (m_usPQuantity > m_usMaxPQuantity)
    {
        usNewMaxQuantity = m_usMaxPQuantity + m_usPGrowBy;

        Polygon* akNewPolygon = NiAlloc(Polygon,usNewMaxQuantity);
        iNumBytes = m_usMaxPQuantity * sizeof(Polygon);
        NiMemcpy(akNewPolygon, m_pPolygon, iNumBytes);
        NiFree( m_pPolygon);
        m_pPolygon = akNewPolygon;

        NiUInt16* ausNewPIndexer = NiAlloc(NiUInt16,
            usNewMaxQuantity);
        uiDestBytes = usNewMaxQuantity * sizeof(NiUInt16);
        iNumBytes = m_usMaxPQuantity * sizeof(NiUInt16);
        NiMemcpy(ausNewPIndexer, uiDestBytes, m_puiPIndexer, iNumBytes);
        iNumBytes =
            (usNewMaxQuantity - m_usMaxPQuantity) * sizeof(NiUInt16);
        NIASSERT(iNumBytes > 0);
        memset(&ausNewPIndexer[m_usMaxPQuantity], 0xFF, iNumBytes);
        NiFree( m_puiPIndexer);
        m_puiPIndexer = ausNewPIndexer;

        m_usMaxPQuantity = usNewMaxQuantity;
    }

    // create the polygon
    Polygon& kPoly = m_pPolygon[usPolygon];
    kPoly.m_usNumVertices = usNumVertices;

    NiUInt16 usVertices = (NiUInt16) GetActiveVertexCount();
    NiUInt16 usIndices = (NiUInt16) GetActiveIndexCount();

    kPoly.m_usVOffset = usVertices;
    kPoly.m_usNumTriangles = usNumTriangles;
    kPoly.m_usIOffset = usIndices; /* 3 * usTriangles; */

    // Find the first available polygon handle for the new index.
    NiUInt16 i;
    for (i = 0; i < m_usPQuantity; i++)
    {
        if (m_puiPIndexer[i] == ms_usInvalid)
        {
            m_puiPIndexer[i] = usPolygon;
            usPolygon = i;
            break;
        }
    }
    NIASSERT(i <= m_usMaxPQuantity);


    // Invalidate the batch locks if a resize is necessary 
    const NiUInt32 uiNewVertCount = usVertices + kPoly.m_usNumVertices;
    const NiUInt32 uiNewIndicesCount = usIndices + (usNumTriangles * 3);

    const bool bNeedResize = (uiNewVertCount > m_usMaxVQuantity) ||
        (uiNewIndicesCount > m_usMaxIQuantity);
    const bool bSaveRestoreBatched = m_bBatchLock && bNeedResize;
    if (bSaveRestoreBatched)
        EndBatchUpdate();

    // resize vertex arrays (if necessary)
    ResizeAttributeStreams((NiUInt16)uiNewVertCount);
    SetAttributesRegionRange(uiNewVertCount);

    // resize attribute and index arrays (if necessary)
    ResizeIndexStream((NiUInt16)uiNewIndicesCount);
    SetIndicesRange(uiNewIndicesCount);

    // restore the batch locks
    if (bSaveRestoreBatched)
        BeginBatchUpdate();

    NiUInt16* pIndices = LockIndexStream(NiDataStream::LOCK_WRITE);

    // Insert the polygon index array.  It is important to use the values
    // kPoly.m_usVOffset and kPoly.m_usIOffset here instead of the active
    // vertex and triangle counts, since the latter quantities were
    // incremented when resizing the vertex and index arrays.
    pIndices += kPoly.m_usIOffset;

    if (auiTriList)
    {
        // Copy over each index adding in the polygon offset
        NiUInt16 uiLoop;
        for (uiLoop = 0; uiLoop < usNumTriangles * 3; uiLoop++)
            pIndices[uiLoop] = (NiUInt16)(
            auiTriList[uiLoop] + kPoly.m_usVOffset);
    }
    else
    {
        // convex polygon, use a triangle fan
        for (i = 0; i < usNumTriangles; i++)
        {
            (*pIndices++) =  kPoly.m_usVOffset;
            (*pIndices++) =  kPoly.m_usVOffset + i + 1;
            (*pIndices++) =  kPoly.m_usVOffset + i + 2;
        }
    }

    UnlockIndexStream(NiDataStream::LOCK_WRITE);
    return (NiInt32)usPolygon;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::AddNewScreenRect(short sTop, short sLeft, 
    unsigned short usWidth, unsigned short usHeight, 
    unsigned short usTexTop, unsigned short usTexLeft,
    NiColorA kColor, unsigned int uiScreenWidth, unsigned int uiScreenHeight)
{
    NiUInt16 uiPoly = (NiUInt16) Insert(4);

    Ni2DBuffer* pkBackBuffer = 
        NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    if (!uiScreenWidth)
        uiScreenWidth = pkBackBuffer->GetWidth();
    if (!uiScreenHeight)
        uiScreenHeight = pkBackBuffer->GetHeight();

    float fX, fY, fW, fH;
    fX = (float)sLeft / (float)uiScreenWidth;
    fY = (float)sTop / (float)uiScreenHeight;
    fW = (float)usWidth / (float)uiScreenWidth;
    fH = (float)usHeight / (float)uiScreenHeight;

    NiTexturingProperty* pkTexProp = 
        (NiTexturingProperty*)GetProperty(NiProperty::TEXTURING);
    if (pkTexProp)
    {
        NiTexture* pkTexture = pkTexProp->GetBaseTexture();
        unsigned int uiTexWidth = pkTexture->GetWidth();
        unsigned int uiTexHeight = pkTexture->GetHeight();

        float fTexX0 = (float) usTexLeft / (float) uiTexWidth;
        float fTexY0 = (float) usTexTop / (float) uiTexHeight;
        float fTexX1 = (float) (usTexLeft + usWidth)  / (float) uiTexWidth;
        float fTexY1 = (float) (usTexTop + usHeight) / (float) uiTexHeight;

        SetTextures(uiPoly, 0, fTexX0, fTexY0, fTexX1, fTexY1);
    }

    SetRectangle(uiPoly, fX, fY, fW, fH);
    SetColors(uiPoly, kColor);
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::Remove(NiInt32 iPolygon)
{
    if (!IsValid(iPolygon))
        return false;

    // Update the indexer array that maps handles to array indices.  First,
    // invalidate the caller's handle.
    NiUInt16 usTarget = m_puiPIndexer[iPolygon];
    m_puiPIndexer[iPolygon] = ms_usInvalid;

    // Second, decrement all indices that are larger than the target index.
    // This reflects the fact that the polygons will be shifted left by one
    // to maintain a compact polygon array.
    NiUInt16 i, usVisited = 1;
    for (i = 0; usVisited < m_usPQuantity && i < m_usMaxPQuantity; i++)
    {
        if (m_puiPIndexer[i] != ms_usInvalid)
        {
            usVisited++;
            if (m_puiPIndexer[i] > usTarget)
                m_puiPIndexer[i]--;
        }
    }

    if (m_usPQuantity > 1)
    {
        Polygon& kDstPoly = m_pPolygon[usTarget];
        if (usTarget + 1 == m_usPQuantity)
        {
            // Removing last polygon, so no need to shift arrays.  Just
            // decrement the vertex and triangle counts.
            NiUInt32 uiVertices = GetActiveVertexCount();
            SetAttributesRegionRange(uiVertices - kDstPoly.m_usNumVertices);
            
            NiUInt32 uiIndices = GetActiveIndexCount();
            SetIndicesRange(uiIndices - (kDstPoly.m_usNumTriangles * 3));
        }
        else
        {
            Polygon& kSrcPoly = m_pPolygon[usTarget + 1];

            // Shift the vertices to the left.
            NiInt32 iVDst = kDstPoly.m_usVOffset;
            NiInt32 iVSrc = kSrcPoly.m_usVOffset;
            NIASSERT(iVSrc - iVDst == kDstPoly.m_usNumVertices);
            
            NiUInt32 uiVertices = GetActiveVertexCount();
            NiInt32 iVRem = (NiInt32)uiVertices - iVSrc;
            NiInt32 iNumBytes = iVRem * sizeof(NiPoint3);

            NiPoint3* pkVertex = LockVertexStream(NiDataStream::LOCK_WRITE);
            memmove(&pkVertex[iVDst], &pkVertex[iVSrc], iNumBytes);
            UnlockVertexStream(NiDataStream::LOCK_WRITE);

            SetAttributesRegionRange(uiVertices - kDstPoly.m_usNumVertices);

            // Shift the normals to the left.
            if (m_bHasNormals)
            {
                NiPoint3* pkNormal = LockNormalStream(
                    NiDataStream::LOCK_WRITE);
                memmove(&pkNormal[iVDst], &pkNormal[iVSrc], iNumBytes);
                UnlockNormalStream(NiDataStream::LOCK_WRITE);
            }

            // Shift the colors to the left.
            if (m_bHasColors)
            {
#if defined(_WII)
                NiRGBA* pkColor = (NiRGBA*)LockColorStream(
                    NiDataStream::LOCK_WRITE);
                iNumBytes = iVRem * sizeof(NiRGBA);
                memmove(&pkColor[iVDst], &pkColor[iVSrc], iNumBytes);
                UnlockColorStream(NiDataStream::LOCK_WRITE);
#else
#if defined(WIN32)
                if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
                {
                    NiRGBA* pkColor = (NiRGBA*)LockColorStream(
                        NiDataStream::LOCK_WRITE);
                    iNumBytes = iVRem * sizeof(NiRGBA);
                    memmove(&pkColor[iVDst], &pkColor[iVSrc], iNumBytes);
                    UnlockColorStream(NiDataStream::LOCK_WRITE);
                }
                else
#endif //#if defined(WIN32)
                {
                    NIASSERT(m_eColorFormat == 
                        NiDataStreamElement::F_FLOAT32_4);
                    NiColorA* pkColor = (NiColorA*)LockColorStream(
                        NiDataStream::LOCK_WRITE);
                    iNumBytes = iVRem * sizeof(NiColorA);
                    memmove(&pkColor[iVDst], &pkColor[iVSrc], iNumBytes);
                    UnlockColorStream(NiDataStream::LOCK_WRITE);
                }
#endif //#if defined(_WII)
            }

            if (m_uiNumTextureSets > 0)
            {
                iNumBytes = iVRem * sizeof(NiPoint2);
                for (NiUInt32 uiSet = 0; uiSet < GetTextureSets(); uiSet++)
                {
                    // Shift the texture coordinates to the left.
                    NiPoint2* akTexture = LockTexCoordStream(
                        uiSet, NiDataStream::LOCK_WRITE);
                    memmove(&akTexture[iVDst], &akTexture[iVSrc], iNumBytes);
                    UnlockTexCoordStream(uiSet, NiDataStream::LOCK_WRITE);
                }
            }

            // Shift the triangle index array to the left.
            NiInt32 iIDst = kDstPoly.m_usIOffset;
            NiInt32 iISrc = kSrcPoly.m_usIOffset;
            NIASSERT(iISrc - iIDst == 3 * kDstPoly.m_usNumTriangles);

            NiUInt32 uiActiveIndices = GetActiveIndexCount();
            NiInt32 iIRem = uiActiveIndices  - (NiInt32)iISrc;
            iNumBytes = iIRem * sizeof(NiUInt32);

            NiUInt16* puiIndices = LockIndexStream(NiDataStream::LOCK_WRITE);
            memmove(&puiIndices[iIDst], &puiIndices[iISrc], iNumBytes);
            
            SetIndicesRange(
                uiActiveIndices - (kDstPoly.m_usNumTriangles * 3));

            // Adjust the index values to account for the shift in the vertex
            // arrays.
            puiIndices += iIDst;
            for (NiInt32 j = 0; j < iIRem; j++)
            {
                *puiIndices = (NiUInt16)
                    (*puiIndices - kDstPoly.m_usNumVertices);
                puiIndices++;
            }
            UnlockIndexStream(NiDataStream::LOCK_WRITE);

            // Shift the polygon array left by one to maintain a compact
            // array.
            for (i = usTarget; i + 1 < m_usPQuantity; i++)
            {
                Polygon& kDst = m_pPolygon[i];
                Polygon& kSrc = m_pPolygon[i + 1];

                kDst.m_usVOffset = kSrc.m_usVOffset - kDst.m_usNumVertices;
                kDst.m_usIOffset = kSrc.m_usIOffset -
                    3 * kDst.m_usNumTriangles;
                kDst.m_usNumVertices = kSrc.m_usNumVertices;
                kDst.m_usNumTriangles = kSrc.m_usNumTriangles;
            }
        }
    }
    else  // m_usPQuantity == 1
    {
        SetAttributesRegionRange(0);
        SetIndicesRange(0);
    }

    m_usPQuantity--;
    m_bBoundNeedsUpdate = true;
    return true;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::RemoveAll()
{
    if (m_usPQuantity == 0)
        return;

    memset(m_pPolygon, 0, m_usPQuantity * sizeof(Polygon));
    m_usPQuantity = 0;
    SetAttributesRegionRange(0);
    SetIndicesRange(0);
    m_bBoundNeedsUpdate = true;
}
//---------------------------------------------------------------------------
NiInt32 NiMeshScreenElements::GetNumVertices(NiInt32 iPolygon) const
{
    if (IsValid(iPolygon))
        return (NiInt32)GetPolygon(iPolygon).m_usNumVertices;

    return 0;
}
//---------------------------------------------------------------------------
NiInt32 NiMeshScreenElements::GetNumTriangles(NiInt32 iPolygon) const
{
    if (IsValid(iPolygon))
        return GetPolygon(iPolygon).m_usNumTriangles;

    return 0;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetVertex(NiInt32 iPolygon, NiInt32 iVertex,
    const NiPoint2& kValue)
{
    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;

    NiPoint3* pkVertex = LockVertexStream(NiDataStream::LOCK_WRITE);

    pkVertex[i].x = kValue.x;
    pkVertex[i].y = kValue.y;
    pkVertex[i].z = 0.0f;

    UnlockVertexStream(NiDataStream::LOCK_WRITE);
    m_bBoundNeedsUpdate = true;
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetVertex(NiInt32 iPolygon, NiInt32 iVertex,
    NiPoint2& kValue) const
{
    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;
    const NiPoint3* pkVertex = LockVertexStreamRead();

    kValue.x = pkVertex[i].x;
    kValue.y = pkVertex[i].y;

    UnlockVertexStreamRead();
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetVertices(NiInt32 iPolygon, 
    const NiPoint2* akValue)
{
    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);

    NiPoint3* pkVertex = LockVertexStream(NiDataStream::LOCK_WRITE);
    NiPoint3* akVertex = &pkVertex[kPoly.m_usVOffset];
    for (NiInt32 i = 0; i < (NiInt32)kPoly.m_usNumVertices; i++)
    {
        akVertex[i].x = akValue[i].x;
        akVertex[i].y = akValue[i].y;
        akVertex[i].z = 0.0f;
    }

    UnlockVertexStream(NiDataStream::LOCK_WRITE);
    m_bBoundNeedsUpdate = true;
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetVertices(NiInt32 iPolygon, NiPoint2* akValue)
    const
{
    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);

    const NiPoint3* pkVertex = LockVertexStreamRead();
    const NiPoint3* akVertex = &pkVertex[kPoly.m_usVOffset];
    for (NiInt32 i = 0; i < (NiInt32)kPoly.m_usNumVertices; i++)
    {
        akValue[i].x = akVertex[i].x;
        akValue[i].y = akVertex[i].y;
    }
    
    UnlockVertexStreamRead();
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetRectangle(NiInt32 iPolygon, float fLeft,
    float fTop, float fWidth, float fHeight)
{
    if (!IsValid(iPolygon))
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    float fRight = (fLeft + fWidth);
    float fBottom = (fTop + fHeight);

    NiPoint3* pkVertex = LockVertexStream(NiDataStream::LOCK_WRITE);

    NiInt32 i = (NiInt32)kPoly.m_usVOffset;
    pkVertex[i].x = fLeft;
    pkVertex[i].y = fTop;
    pkVertex[i].z = 0.0f;
    i++;

    pkVertex[i].x = fLeft;
    pkVertex[i].y = fBottom;
    pkVertex[i].z = 0.0f;
    i++;

    pkVertex[i].x = fRight;
    pkVertex[i].y = fBottom;
    pkVertex[i].z = 0.0f;
    i++;

    pkVertex[i].x = fRight;
    pkVertex[i].y = fTop;
    pkVertex[i].z = 0.0f;
    i++;

    UnlockVertexStream(NiDataStream::LOCK_WRITE);
    m_bBoundNeedsUpdate = true;
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetRectangle(NiInt32 iPolygon, float& fLeft,
    float& fTop, float& fWidth, float& fHeight) const
{
    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    const NiPoint3* pkVertex = LockVertexStreamRead();

    NiInt32 i = (NiInt32)kPoly.m_usVOffset;
    fLeft = pkVertex[i].x;
    fTop = pkVertex[i].y;
    i += 2;
    fWidth = pkVertex[i].x - fLeft;
    fHeight = pkVertex[i].y - fTop;
    UnlockVertexStreamRead();
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetNormal(NiInt32 iPolygon, NiInt32 iVertex,
    const NiPoint3& kValue)
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;
    Polygon& kPoly = GetPolygon(iPolygon);
    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;

    NiPoint3* pkNormal = LockNormalStream(NiDataStream::LOCK_WRITE);
    pkNormal[i] = kValue;
    UnlockNormalStream(NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetNormal(NiInt32 iPolygon, NiInt32 iVertex,
    NiPoint3& kValue) const
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;
    const NiPoint3* pkNormal = LockNormalStreamRead();

    kValue = pkNormal[i];
    UnlockNormalStreamRead();
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetNormals(NiInt32 iPolygon,
    const NiPoint3* akValue)
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

    NiPoint3* pkNormal = LockNormalStream(NiDataStream::LOCK_WRITE);
    NiPoint3* akNormal = &pkNormal[kPoly.m_usVOffset];
    NiInt32 iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint3);
    NiMemcpy(akNormal, akValue, iNumBytes);
    UnlockNormalStream(NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetNormals(NiInt32 iPolygon,
    const NiPoint3& kCommonValue)
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

    NiPoint3* pkNormal = LockNormalStream(NiDataStream::LOCK_WRITE);
    NiPoint3* akNormal = &pkNormal[kPoly.m_usVOffset];
    for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
        akNormal[i] = kCommonValue;
    UnlockNormalStream(NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetNormals(NiInt32 iPolygon, NiPoint3* akValue)
    const
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices <= 0)
        return false;

    const NiPoint3* pkNormal = LockNormalStreamRead();

    const NiPoint3* akNormal = &pkNormal[kPoly.m_usVOffset];
    NiInt32 iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint3);
    NiMemcpy(akValue, akNormal, iNumBytes);
    UnlockNormalStreamRead();
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetNormals(NiInt32 iPolygon,
    const NiPoint3& kUpperLeft, const NiPoint3& kLowerLeft,
    const NiPoint3& kLowerRight, const NiPoint3& kUpperRight)
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    NiPoint3* pkNormal = LockNormalStream(NiDataStream::LOCK_WRITE);

    NiPoint3* akNormal = &pkNormal[kPoly.m_usVOffset];
    akNormal[0] = kUpperLeft;
    akNormal[1] = kLowerLeft;
    akNormal[2] = kLowerRight;
    akNormal[3] = kUpperRight;
    UnlockNormalStream(NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetNormals(NiInt32 iPolygon, NiPoint3& kUpperLeft,
    NiPoint3& kLowerLeft, NiPoint3& kLowerRight, NiPoint3& kUpperRight) const
{
    if (!m_bHasNormals)
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    const NiPoint3* pkNormal = LockNormalStreamRead();

    const NiPoint3* akNormal = &pkNormal[kPoly.m_usVOffset];
    kUpperLeft = akNormal[0];
    kLowerLeft = akNormal[1];
    kLowerRight = akNormal[2];
    kUpperRight = akNormal[3];
    UnlockNormalStreamRead();
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetColor(NiInt32 iPolygon, NiInt32 iVertex,
    const NiColorA& kValue)
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;

#if defined(_WII)
    NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
    kValue.GetAs(pkColor[i]);
    UnlockColorStream(NiDataStream::LOCK_WRITE);
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
        pkColor[i] = ColorAToBGRA(kValue);
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        NiColorA* pkColor = 
            (NiColorA*)LockColorStream(NiDataStream::LOCK_WRITE);
        pkColor[i] = kValue;
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
#endif //#if defined(_WII)

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetColor(NiInt32 iPolygon, NiInt32 iVertex,
    NiColorA& kValue) const
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);

    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;

#if defined(_WII)
    const NiRGBA* pkColor = (NiRGBA*)LockColorStreamRead();
    pkColor[i].GetAs(kValue);
    UnlockColorStreamRead();
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        const NiRGBA* pkColor = (NiRGBA*)LockColorStreamRead();
        kValue = BGRAToColorA(pkColor[i]);
        UnlockColorStreamRead();
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        const NiColorA* pkColor = (NiColorA*)LockColorStreamRead();
        kValue = pkColor[i];
        UnlockColorStreamRead();
    }
#endif //#if defined(_WII)

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetColors(NiInt32 iPolygon,
    const NiColorA* akValue)
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

#if defined(_WII)
        NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
        NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
        for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
        {
            akValue[i].GetAs(akColor[i]);
        }
        UnlockColorStream(NiDataStream::LOCK_WRITE);
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
        NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
        for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
        {
            akColor[i] = ColorAToBGRA(akValue[i]);
        }
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        NiColorA* pkColor = 
            (NiColorA*)LockColorStream(NiDataStream::LOCK_WRITE);
        NiColorA* akColor = &pkColor[kPoly.m_usVOffset];
        NiInt32 iNumBytes = kPoly.m_usNumVertices * sizeof(NiColorA);
        NiMemcpy(akColor, akValue, iNumBytes);
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
#endif //#if defined(_WII)
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetColors(NiInt32 iPolygon,
    const NiColorA& kCommonValue)
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

#if defined(_WII)
    NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
    NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
    NiRGBA kValue;
    kCommonValue.GetAs(kValue);
    for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
        akColor[i] = kValue;
    UnlockColorStream(NiDataStream::LOCK_WRITE);
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
        NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
        NiRGBA kValue = ColorAToBGRA(kCommonValue);
        for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
            akColor[i] = kValue;
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        NiColorA* pkColor = (NiColorA*)LockColorStream(
            NiDataStream::LOCK_WRITE);
        NiColorA* akColor = &pkColor[kPoly.m_usVOffset];
        for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
            akColor[i] = kCommonValue;
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
#endif //#if defined(_WII)
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetColors(NiInt32 iPolygon, NiColorA* akValue)
    const
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

#if defined(_WII)
    const NiRGBA* pkColor = (NiRGBA*)LockColorStreamRead();
    const NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
    for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
    {
        akColor[i].GetAs(akValue[i]);
    }
    UnlockColorStreamRead();
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        const NiRGBA* pkColor = (NiRGBA*)LockColorStreamRead();
        const NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
        for (NiUInt16 i = 0; i < kPoly.m_usNumVertices; i++)
        {
            akValue[i] = BGRAToColorA(akColor[i]);
        }
        UnlockColorStreamRead();
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        const NiColorA* pkColor = (NiColorA*)LockColorStreamRead();
        const NiColorA* akColor = &pkColor[kPoly.m_usVOffset];
        NiInt32 iNumBytes = kPoly.m_usNumVertices * sizeof(NiColorA);
        NiMemcpy(akValue, akColor, iNumBytes);
        UnlockColorStreamRead();
    }
#endif //#if defined(_WII)
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetColors(NiInt32 iPolygon,
    const NiColorA& kUpperLeft, const NiColorA& kLowerLeft,
    const NiColorA& kLowerRight, const NiColorA& kUpperRight)
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

#if defined(_WII)
    NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
    NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
    kUpperLeft.GetAs(akColor[0]);
    kLowerLeft.GetAs(akColor[1]);
    kLowerRight.GetAs(akColor[2]);
    kUpperRight.GetAs(akColor[3]);
    UnlockColorStream(NiDataStream::LOCK_WRITE);
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        NiRGBA* pkColor = (NiRGBA*)LockColorStream(NiDataStream::LOCK_WRITE);
        NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
        akColor[0] = ColorAToBGRA(kUpperLeft);
        akColor[1] = ColorAToBGRA(kLowerLeft);
        akColor[2] = ColorAToBGRA(kLowerRight);
        akColor[3] = ColorAToBGRA(kUpperRight);
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        NiColorA* pkColor = (NiColorA*)LockColorStream(
            NiDataStream::LOCK_WRITE);
        NiColorA* akColor = &pkColor[kPoly.m_usVOffset];
        akColor[0] = kUpperLeft;
        akColor[1] = kLowerLeft;
        akColor[2] = kLowerRight;
        akColor[3] = kUpperRight;
        UnlockColorStream(NiDataStream::LOCK_WRITE);
    }
#endif //#if defined(_WII)
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetColors(NiInt32 iPolygon, NiColorA& kUpperLeft,
    NiColorA& kLowerLeft, NiColorA& kLowerRight, NiColorA& kUpperRight) const
{
    if (!m_bHasColors)
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

#if defined(_WII)
    const NiRGBA* pkColor = (NiRGBA*)LockColorStreamRead();
    const NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
    akColor[0].GetAs(kUpperLeft);
    akColor[1].GetAs(kLowerLeft);
    akColor[2].GetAs(kLowerRight);
    akColor[3].GetAs(kUpperRight);
    UnlockColorStreamRead();
#else
#if defined(WIN32)
    if (m_eColorFormat == NiDataStreamElement::F_NORMUINT8_4_BGRA)
    {
        const NiRGBA* pkColor = (NiRGBA*)LockColorStreamRead();
        const NiRGBA* akColor = &pkColor[kPoly.m_usVOffset];
        kUpperLeft = BGRAToColorA(akColor[0]);
        kLowerLeft = BGRAToColorA(akColor[1]);
        kLowerRight = BGRAToColorA(akColor[2]);
        kUpperRight = BGRAToColorA(akColor[3]);
        UnlockColorStreamRead();
    }
    else
#endif //#if defined(WIN32)
    {
        NIASSERT(m_eColorFormat == NiDataStreamElement::F_FLOAT32_4);
        const NiColorA* pkColor = (NiColorA*)LockColorStreamRead();
        const NiColorA* akColor = &pkColor[kPoly.m_usVOffset];
        kUpperLeft = akColor[0];
        kLowerLeft = akColor[1];
        kLowerRight = akColor[2];
        kUpperRight = akColor[3];
        UnlockColorStreamRead();
    }
#endif //#if defined(_WII)
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetTexture(NiInt32 iPolygon, NiInt32 iVertex,
    NiUInt16 usSet, const NiPoint2& kValue)
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    //NiPoint2* akTexture = GetTextureSet(usSet);

    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    NiPoint2* pkTexCoord =
        LockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;
    pkTexCoord[i] = kValue;
    UnlockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetTexture(NiInt32 iPolygon, NiInt32 iVertex,
    NiUInt16 usSet, NiPoint2& kValue) const
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (iVertex < 0)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    //const NiPoint2* akTexture = GetTextureSet(usSet);

    if (iVertex >= (NiInt32)kPoly.m_usNumVertices)
        return false;

    const NiPoint2* pkTexCoord = LockTexCoordStreamRead(usSet);

    NiInt32 i = iVertex + (NiInt32)kPoly.m_usVOffset;
    kValue = pkTexCoord[i];
    UnlockTexCoordStreamRead(usSet);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetTextures(NiInt32 iPolygon, NiUInt16 usSet,
    const NiPoint2* akValue)
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

    //NiPoint2* akTSet = GetTextureSet(usSet);
    NiPoint2* pkTexCoord =
        LockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);
    NiPoint2* akTexture = &pkTexCoord[kPoly.m_usVOffset];
    NiInt32 iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint2);
    NiMemcpy(akTexture, akValue, iNumBytes);
    UnlockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetTextures(NiInt32 iPolygon, NiUInt16 usSet,
    NiPoint2* akValue) const
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    if (!akValue)
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices == 0)
        return false;

    const NiPoint2* pkTexCoord = LockTexCoordStreamRead(usSet);

    const NiPoint2* akTexture = &pkTexCoord[kPoly.m_usVOffset];
    NiInt32 iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint2);
    NiMemcpy(akValue, akTexture, iNumBytes);
    UnlockTexCoordStreamRead(usSet);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetTextures(NiInt32 iPolygon, NiUInt16 usSet,
    float fLeft, float fTop, float fRight, float fBottom)
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    NiPoint2* pkTexCoord =
        LockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);

    NiPoint2* akTexture = &pkTexCoord[kPoly.m_usVOffset];
    akTexture[0].x = fLeft;
    akTexture[0].y = fTop;
    akTexture[1].x = fLeft;
    akTexture[1].y = fBottom;
    akTexture[2].x = fRight;
    akTexture[2].y = fBottom;
    akTexture[3].x = fRight;
    akTexture[3].y = fTop;
    UnlockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::SetTextures(NiInt32 iPolygon, NiUInt16 usSet,
    const NiPoint2& kUpperLeft, const NiPoint2& kLowerLeft,
    const NiPoint2& kLowerRight, const NiPoint2& kUpperRight)
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    NiPoint2* pkTexCoord =
        LockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);

    NiPoint2* akTexture = &pkTexCoord[kPoly.m_usVOffset];
    akTexture[0] = kUpperLeft;
    akTexture[1] = kLowerLeft;
    akTexture[2] = kLowerRight;
    akTexture[3] = kUpperRight;
    UnlockTexCoordStream(usSet, NiDataStream::LOCK_WRITE);
    return true;
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::GetTextures(NiInt32 iPolygon, NiUInt16 usSet,
    NiPoint2& kUpperLeft, NiPoint2& kLowerLeft, NiPoint2& kLowerRight,
    NiPoint2& kUpperRight) const
{
    if (usSet >= GetTextureSets())
        return false;

    if (!IsValid(iPolygon))
        return false;

    const Polygon& kPoly = GetPolygon(iPolygon);
    if (kPoly.m_usNumVertices != 4)
        return false;

    const NiPoint2* pkTexCoord = LockTexCoordStreamRead(usSet);

    const NiPoint2* akTexture = &pkTexCoord[kPoly.m_usVOffset];
    kUpperLeft = akTexture[0];
    kLowerLeft = akTexture[1];
    kLowerRight = akTexture[2];
    kUpperRight = akTexture[3];
    UnlockTexCoordStreamRead(usSet);
    return true;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::UpdateBound()
{
    m_bBoundNeedsUpdate = false;

    NiUInt32 uiVertices = GetActiveVertexCount();

    if (uiVertices == 0)
        return;

    const NiDataStream* pkVertexDS = GetVertexDataStream();
    const NiPoint3* pkVertex = (const NiPoint3*)
        ((NiDataStream*)pkVertexDS)->Lock(NiDataStream::LOCK_READ);
    m_kBound.ComputeFromData(uiVertices, pkVertex);
    ((NiDataStream*)pkVertexDS)->Unlock(NiDataStream::LOCK_READ);
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::Resize(float fXScale, float fYScale)
{
    if (fXScale <= 0.0f || fYScale <= 0.0f)
        return false;

    NiDataStream* pkVertexDS = GetVertexDataStream();
    NiPoint3* pkVertex = (NiPoint3*)pkVertexDS->Lock(
        NiDataStream::LOCK_WRITE);

    NiUInt32 uiActiveVertices = GetActiveVertexCount();

    NiPoint3* pkV = pkVertex;
    for (NiUInt32 ui = 0; ui < uiActiveVertices; ui++)
    {
        pkV->x = fXScale * pkV->x;
        pkV->y = fYScale * pkV->y;
        pkV++;
    }
    pkVertexDS->Unlock(NiDataStream::LOCK_WRITE);
    m_bBoundNeedsUpdate = true;
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMeshScreenElements);
//---------------------------------------------------------------------------
void NiMeshScreenElements::CopyMembers(NiMeshScreenElements* pkDest,
    NiCloningProcess& kCloning)
{
    NiMesh::CopyMembers(pkDest, kCloning);

    pkDest->m_usMaxPQuantity = m_usMaxPQuantity;

    NiInt32 iNumBytes;

    pkDest->m_pPolygon = NiAlloc(Polygon, m_usMaxPQuantity);
    iNumBytes = m_usMaxPQuantity * sizeof(Polygon);
    NiMemcpy(pkDest->m_pPolygon, m_pPolygon, iNumBytes);

    pkDest->m_puiPIndexer = NiAlloc(NiUInt16, m_usMaxPQuantity);
    iNumBytes = m_usMaxPQuantity * sizeof(NiUInt16);
    NiMemcpy(pkDest->m_puiPIndexer, m_puiPIndexer, iNumBytes);

    pkDest->m_usPGrowBy = m_usPGrowBy;
    pkDest->m_usPQuantity = m_usPQuantity;
    pkDest->m_usMaxVQuantity = m_usMaxVQuantity;
    pkDest->m_usVGrowBy = m_usVGrowBy;
    pkDest->m_usMaxIQuantity = m_usMaxIQuantity;
    pkDest->m_usIGrowBy = m_usIGrowBy;
    pkDest->m_bHasNormals = m_bHasNormals;
    pkDest->m_bHasColors = m_bHasColors;
    pkDest->m_uiNumTextureSets = m_uiNumTextureSets;
    pkDest->m_eColorFormat = pkDest->m_eColorFormat;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMeshScreenElements);
//---------------------------------------------------------------------------
void NiMeshScreenElements::LoadBinary(NiStream& kStream)
{
    NiMesh::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usMaxPQuantity);
    m_pPolygon = NiAlloc(Polygon, m_usMaxPQuantity);
    m_puiPIndexer = NiAlloc(NiUInt16,m_usMaxPQuantity);

    NiUInt32 uiMaxPQuantity = (NiUInt32)m_usMaxPQuantity;
    NiStreamLoadBinary(kStream, m_pPolygon, uiMaxPQuantity);
    NiStreamLoadBinary(kStream, m_puiPIndexer, uiMaxPQuantity);
    NiStreamLoadBinary(kStream, m_usPGrowBy);
    NiStreamLoadBinary(kStream, m_usPQuantity);
    NiStreamLoadBinary(kStream, m_usMaxVQuantity);
    NiStreamLoadBinary(kStream, m_usVGrowBy);
    NiStreamLoadBinary(kStream, m_usMaxIQuantity);
    NiStreamLoadBinary(kStream, m_usIGrowBy);

    if(kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 12))
    {
        NiStreamLoadEnum(kStream, m_eColorFormat);
    }
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::LinkObject(NiStream& kStream)
{
    NiMesh::LinkObject(kStream);

    ResetCacheData();
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::RegisterStreamables(NiStream& kStream)
{
    return NiMesh::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SaveBinary(NiStream& kStream)
{
    NiMesh::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usMaxPQuantity);

    NiUInt32 uiMaxPQuantity = (NiUInt32)m_usMaxPQuantity;
    NiStreamSaveBinary(kStream, m_pPolygon, uiMaxPQuantity);
    NiStreamSaveBinary(kStream, m_puiPIndexer, uiMaxPQuantity);
    NiStreamSaveBinary(kStream, m_usPGrowBy);
    NiStreamSaveBinary(kStream, m_usPQuantity);
    NiStreamSaveBinary(kStream, m_usMaxVQuantity);
    NiStreamSaveBinary(kStream, m_usVGrowBy);
    NiStreamSaveBinary(kStream, m_usMaxIQuantity);
    NiStreamSaveBinary(kStream, m_usIGrowBy);

    NiStreamSaveEnum(kStream, m_eColorFormat);
}
//---------------------------------------------------------------------------
bool NiMeshScreenElements::IsEqual(NiObject* pkObject)
{
    if (!NiMesh::IsEqual(pkObject))
        return false;

    NiMeshScreenElements* pkSED = (NiMeshScreenElements*)pkObject;

    // compare array and growth sizes
    if (m_usMaxPQuantity != pkSED->m_usMaxPQuantity ||
        m_usPGrowBy != pkSED->m_usPGrowBy ||
        m_usPQuantity != pkSED->m_usPQuantity ||
        m_usMaxVQuantity != pkSED->m_usMaxVQuantity ||
        m_usVGrowBy != pkSED->m_usVGrowBy ||
        m_usMaxIQuantity != pkSED->m_usMaxIQuantity ||
        m_usIGrowBy != pkSED->m_usIGrowBy ||
        m_eColorFormat != pkSED->m_eColorFormat)
    {
        return false;
    }

    // compare polygons
    NiInt32 iBytes = m_usMaxPQuantity * sizeof(Polygon);
    if (memcmp(m_pPolygon, pkSED->m_pPolygon, iBytes) != 0)
        return false;

    // compare polygon indexing
    iBytes = m_usMaxPQuantity * sizeof(NiUInt16);
    if (memcmp(m_puiPIndexer, pkSED->m_puiPIndexer, iBytes) != 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMesh::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiMeshScreenElements::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_akPolygon", m_pPolygon));
    pkStrings->Add(NiGetViewerString("m_ausPIndexer", m_puiPIndexer));
    pkStrings->Add(NiGetViewerString("m_usMaxPQuantity", m_usMaxPQuantity));
    pkStrings->Add(NiGetViewerString("m_usPGrowBy", m_usPGrowBy));
    pkStrings->Add(NiGetViewerString("m_usPQuantity", m_usPQuantity));
    pkStrings->Add(NiGetViewerString("m_usMaxVQuantity", m_usMaxVQuantity));
    pkStrings->Add(NiGetViewerString("m_usVGrowBy", m_usVGrowBy));
    pkStrings->Add(NiGetViewerString("m_usMaxIQuantity", m_usMaxIQuantity));
    pkStrings->Add(NiGetViewerString("m_usIGrowBy", m_usIGrowBy));
    pkStrings->Add(NiGetViewerString("m_eColorFormat", m_eColorFormat));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// conversion
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetMaxPQuantity(NiUInt16 usQuantity)
{
    m_usMaxPQuantity = usQuantity; 
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetPQuantity(NiUInt16 usQuantity)
{
    m_usPQuantity = usQuantity; 
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetMaxVQuantity(NiUInt16 usQuantity)
{
    m_usMaxVQuantity = usQuantity;
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetMaxTQuantity(NiUInt16 usQuantity)
{
    m_usMaxIQuantity = usQuantity; 
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::ResetCacheData()
{
    // From streams, set convenience variables
    NIASSERT(GetSubmeshCount() == 1);
    
    NiDataStreamRef* pkStreamRef = NULL;
    NiDataStreamElement kElement;

    if (FindStreamRefAndElementBySemantic(
        NiCommonSemantics::NORMAL(), 0,
        NiDataStreamElement::F_FLOAT32_3, pkStreamRef,
        kElement))
    {
        NIASSERT(pkStreamRef->GetDataStream() &&
            pkStreamRef->GetDataStream()->GetCount(0) > 0);
        m_bHasNormals = true;
    }
    else
    {
        m_bHasNormals = false;
    }

    if (FindStreamRefAndElementBySemantic(
        NiCommonSemantics::COLOR(), 0,
        m_eColorFormat, pkStreamRef,
        kElement))
    {
        NIASSERT(pkStreamRef->GetDataStream() &&
            pkStreamRef->GetDataStream()->GetCount(0) > 0);
        m_bHasColors = true;
    }
    else
    {
        m_bHasColors = false;
    }
    
    m_uiNumTextureSets = GetSemanticCount(NiCommonSemantics::TEXCOORD());
}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetPolygonArray(void* pPolyArray,
    NiUInt16 usMaxPolys)
{
    class OldPolygonFormat : public NiMemObject
    {
    public:
        unsigned short m_usNumVertices;
        unsigned short m_usVOffset;  // offset into the vertex arrays
        unsigned short m_usNumTriangles;
        unsigned short m_usIOffset;  // offset into the index array
    };

    NIASSERT(m_pPolygon == NULL);

    if (usMaxPolys == 0)
        return;

    OldPolygonFormat* pkOldArray = (OldPolygonFormat*)pPolyArray;

    m_pPolygon = NiAlloc(Polygon, usMaxPolys);

    // In case current format changes from being 4 unsigned shorts,
    // we set the new data element by element.
    for(NiUInt16 i=0; i<usMaxPolys; i++)
    {
        m_pPolygon[i].m_usNumVertices = pkOldArray[i].m_usNumVertices;
        m_pPolygon[i].m_usVOffset = pkOldArray[i].m_usVOffset;
        m_pPolygon[i].m_usNumTriangles = pkOldArray[i].m_usNumTriangles;
        m_pPolygon[i].m_usIOffset = pkOldArray[i].m_usIOffset;
    }

}
//---------------------------------------------------------------------------
void NiMeshScreenElements::SetIndexerArray(NiUInt16* pkOldIndexer,
    NiUInt16 usMaxPolys)
{
    NIASSERT(m_puiPIndexer == NULL);

    if (usMaxPolys == 0)
        return;

    m_puiPIndexer = NiAlloc(NiUInt16, usMaxPolys);

    for(NiUInt16 i=0; i<usMaxPolys; i++)
    {
        m_puiPIndexer[i] = pkOldIndexer[i];
    }
}
//---------------------------------------------------------------------------
