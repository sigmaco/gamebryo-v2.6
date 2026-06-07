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
#ifndef NIDX9SOURCECUBEMAPDATA_H
#define NIDX9SOURCECUBEMAPDATA_H

#include "NiDX9SourceTextureData.h"
#include <NiSourceCubeMap.h>

//---------------------------------------------------------------------------
class NIDX9RENDERER_ENTRY NiDX9SourceCubeMapData : 
    public NiDX9SourceTextureData
{
public:
    static NiDX9SourceCubeMapData* Create(NiSourceCubeMap* pkTexture, 
        NiDX9Renderer* pkRenderer);
    virtual ~NiDX9SourceCubeMapData();

protected:
    NiDX9SourceCubeMapData(NiSourceCubeMap* pkTexture, 
        NiDX9Renderer* pkRenderer);

    bool CreateCubeMap();

    void CopyDataToCubeMapFace(const NiPixelData& kPixels,
        NiSourceCubeMap::FaceID eFace);
    void CopyRendererDataToCubeMapFace(const
        NiDX9PersistentSrcTextureRendererData*
        pkPersistentSrcTextureRendererData,
        NiSourceCubeMap::FaceID eFace);

    virtual void Update();

    unsigned int m_uiSourceRevID;
};
//---------------------------------------------------------------------------

typedef NiPointer<NiDX9SourceCubeMapData> NiDX9SourceCubeMapDataPtr;

#endif  //#ifndef NIDX9SOURCETEXTUREDATA_H
