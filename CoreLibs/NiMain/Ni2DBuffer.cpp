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

#include "Ni2DBuffer.h"
#include "NiRenderer.h"
#include "NiDepthStencilBuffer.h"

NiImplementRTTI(Ni2DBuffer, NiObject);

//---------------------------------------------------------------------------
Ni2DBuffer* Ni2DBuffer::Create(unsigned int uiWidth, unsigned int uiHeight)
{
    Ni2DBuffer* pkBuffer = NiNew Ni2DBuffer();
    pkBuffer->m_uiWidth = uiWidth;
    pkBuffer->m_uiHeight = uiHeight;
    return pkBuffer;
}
//---------------------------------------------------------------------------
Ni2DBuffer* Ni2DBuffer::Create(unsigned int uiWidth, unsigned int uiHeight,
    Ni2DBuffer::RendererData* pkData)
{
    if (pkData == NULL)
        return NULL;

    const NiPixelFormat* pkFormat = pkData->GetPixelFormat();
    if (pkFormat == NULL)
        return NULL;

    Ni2DBuffer* pkBuffer = NiNew Ni2DBuffer();
    pkBuffer->m_uiWidth = uiWidth;
    pkBuffer->m_uiHeight = uiHeight;
    pkBuffer->m_spRendererData = pkData;
    pkBuffer->SetRendererData(pkData);
    return pkBuffer;
}
//---------------------------------------------------------------------------
Ni2DBuffer::Ni2DBuffer() : m_uiWidth(0), m_uiHeight(0)
{
}
//---------------------------------------------------------------------------
Ni2DBuffer::~Ni2DBuffer()
{
}
//---------------------------------------------------------------------------
bool Ni2DBuffer::CreateRendererData(const NiPixelFormat&, 
    Ni2DBuffer::MultiSamplePreference) 
{
    // This function is only useful when creating renderer data for an 
    // NiDepthStencilBuffer object.

    return false;
}
//---------------------------------------------------------------------------
void Ni2DBuffer::ResetDimensions(unsigned int uiWidth, unsigned int uiHeight)
{
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
}
//---------------------------------------------------------------------------
bool Ni2DBuffer::FastCopy(Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    unsigned int uiDestX, unsigned int uiDestY) const
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!m_spRendererData || !pkRenderer || !pkDest->m_spRendererData)
    {
        return false;
    }
    else
    {
        return pkRenderer->FastCopy(this, pkDest, pkSrcRect, uiDestX, 
            uiDestY);
    }
}
//---------------------------------------------------------------------------
bool Ni2DBuffer::Copy(Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    const NiRect<unsigned int>* pkDestRect,
    CopyFilterPreference ePref) const
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!m_spRendererData || !pkRenderer || !pkDest->m_spRendererData)
    {
        return false;
    }
    else
    {
        return pkRenderer->Copy(this, pkDest, pkSrcRect, pkDestRect, 
            ePref);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(Ni2DBuffer);
//---------------------------------------------------------------------------
void Ni2DBuffer::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void Ni2DBuffer::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool Ni2DBuffer::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void Ni2DBuffer::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool Ni2DBuffer::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void Ni2DBuffer::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(Ni2DBuffer::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

