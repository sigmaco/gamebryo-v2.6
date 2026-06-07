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

#include "NiPixelData.h"

//---------------------------------------------------------------------------
void NiPixelData::AllocateData(unsigned int uiLevels, unsigned int uiFaces,
    unsigned int uiBytes)
{
    NIASSERT(uiFaces > 0);
    NIASSERT(uiLevels > 0);

    // Rounding up for multiple of 4 for pointer allignment.
    uiBytes = (uiBytes + 3) & ~3;

    // uiLevels * 3     (Levels for each: Width,Height,OffsetInBytes)
    // << 2             ( * 4 to make room for unsigned int )
    // + uiBytes        (plus the space allocated for pixels)
    // * uiFaces        (* the number of faces for this pixel data)
    m_pucPixels = NiAlloc2(unsigned char,
        ((uiLevels * 3 + 1) << 2) + uiBytes*uiFaces, NiMemHint::TEXTURE);
    m_puiWidth = (unsigned int*)(m_pucPixels + uiBytes*uiFaces);
    m_puiHeight = m_puiWidth + uiLevels;
    m_puiOffsetInBytes = m_puiHeight + uiLevels;
    NIASSERT(m_pucPixels != NULL);
}
//---------------------------------------------------------------------------
void NiPixelData::FreeData()
{
    NiFree(m_pucPixels);
    m_puiWidth = m_puiHeight = m_puiOffsetInBytes = 0;
    m_pucPixels = 0;
}
//---------------------------------------------------------------------------
