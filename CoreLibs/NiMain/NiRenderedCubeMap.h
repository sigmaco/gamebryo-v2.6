
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

#ifndef NIRENDEREDCUBEMAP_H
#define NIRENDEREDCUBEMAP_H

#include "NiRenderedTexture.h"

class NIMAIN_ENTRY NiRenderedCubeMap : public NiRenderedTexture
{
    NiDeclareRTTI;

public:
    enum FaceID
    {
        FACE_POS_X,
        FACE_NEG_X,
        FACE_POS_Y,
        FACE_NEG_Y,
        FACE_POS_Z,
        FACE_NEG_Z,
        FACE_NUM
    };

    static NiRenderedCubeMap* Create(unsigned int uiSize, 
        NiRenderer* pkRenderer, FormatPrefs& kPrefs = ms_kDefaultPrefs);

    FaceID GetCurrentCubeFace() const;
    inline void SetCurrentCubeFace(FaceID eFace);

    inline Ni2DBuffer* GetFaceBuffer(FaceID eFace);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;

protected:
    NiRenderedCubeMap();

    FaceID m_eFace;

    Ni2DBufferPtr m_aspFaceBuffers[FACE_NUM];
};

typedef NiPointer<NiRenderedCubeMap> NiRenderedCubeMapPtr;


//---------------------------------------------------------------------------
//  Inline include

#include "NiRenderedCubeMap.inl"
//---------------------------------------------------------------------------

#endif
