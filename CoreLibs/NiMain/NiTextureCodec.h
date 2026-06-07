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
#ifndef NITEXTURECODEC_H
#define NITEXTURECODEC_H

#include "NiPixelFormat.h"

class NIMAIN_ENTRY NiTextureCodec : public NiMemObject
{
public:
    NiTextureCodec();
    ~NiTextureCodec();

    NiPixelData* ExpandCompressedData(const NiPixelData& kSrc);

protected:
    enum 
    {
        DEST_PIXEL_STRIDE = 4
    };

    void DecodeT3AlphaBlock(unsigned char* pucAlphas);
    void DecodeT5AlphaBlock(unsigned char* pucAlphas);
    void DecodeColorBlock(unsigned char* pucColors);
    void DecodeColorAlphaBlock(unsigned char* pucDest);

    void DecodeBlock(unsigned char* pucDest, unsigned int uiCols, 
        unsigned int uiRows);

    void (NiTextureCodec::*m_pfnAlphaBlockDecoder)(unsigned char* pucDest);
    void (NiTextureCodec::*m_pfnColorBlockDecoder)(unsigned char* pucDest);

    const unsigned char* m_pucSrcPtr;

    unsigned int m_uiDestRowStride;

    unsigned char m_aaucExplicitAlphaLookup[256][2];
};

#endif
