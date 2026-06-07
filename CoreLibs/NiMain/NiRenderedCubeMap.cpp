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

#include "NiRenderedCubeMap.h"
#include "NiRenderer.h"

NiImplementRTTI(NiRenderedCubeMap, NiRenderedTexture);

//---------------------------------------------------------------------------
NiRenderedCubeMap::NiRenderedCubeMap()
{
    m_eFace = FACE_POS_X;
}
//---------------------------------------------------------------------------
NiRenderedCubeMap* NiRenderedCubeMap::Create(unsigned int uiSize,
    NiRenderer* pkRenderer, FormatPrefs& kPrefs)
{
    if (!(pkRenderer && NiIsPowerOf2(uiSize)))
        return NULL;

    NiRenderedCubeMap* pkThis = NiNew NiRenderedCubeMap;
    pkThis->m_kFormatPrefs = kPrefs;
    for (unsigned int ui = 0; ui < FACE_NUM; ui++)
    {
        pkThis->m_aspFaceBuffers[ui] = Ni2DBuffer::Create(uiSize, uiSize);
    }

    // The creation of the renderer data should also populate
    // the Ni2DBuffer::RendererData
    if (!pkRenderer->CreateRenderedCubeMapRendererData(pkThis))
    {
        NiDelete pkThis;
        return NULL;
    }
    return pkThis;
}
//---------------------------------------------------------------------------
unsigned int NiRenderedCubeMap::GetWidth() const
{
    NIASSERT(m_aspFaceBuffers[0]);
    return m_aspFaceBuffers[0]->GetWidth();
}
//---------------------------------------------------------------------------
unsigned int NiRenderedCubeMap::GetHeight() const
{
    NIASSERT(m_aspFaceBuffers[0]);
    return m_aspFaceBuffers[0]->GetHeight();
}
//---------------------------------------------------------------------------
