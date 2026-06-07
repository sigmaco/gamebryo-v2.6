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

#ifndef NID3D10DIRECT3DTEXTUREDATA_H
#define NID3D10DIRECT3DTEXTUREDATA_H

#include "NiD3D10TextureData.h"
class NiD3D10Direct3DTexture;

class NID3D10RENDERER_ENTRY NiD3D10Direct3DTextureData : 
    public NiD3D10TextureData
{
public:
    static NiD3D10Direct3DTextureData* Create(
        NiD3D10Direct3DTexture* pkTexture, ID3D10Resource* pkD3D10Texture, 
        ID3D10ShaderResourceView* pkResourceView);
    virtual ~NiD3D10Direct3DTextureData();

protected:
    NiD3D10Direct3DTextureData(NiD3D10Direct3DTexture* pkTexture);
};

#endif  //#ifndef NID3D10DIRECT3DTEXTUREDATA_H
