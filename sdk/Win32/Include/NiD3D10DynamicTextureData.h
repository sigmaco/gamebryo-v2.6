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
#ifndef NID3D10DYNAMICTEXTUREDATA_H
#define NID3D10DYNAMICTEXTUREDATA_H

#include "NiD3D10TextureData.h"

#include <NiDynamicTexture.h>

// This define can be used to switch the method used to implement 
// dynamic textures. 
//      The staging method uses a default texture
// and a stagging texture. The lock and unlock of a dynamic texture
// result in a map and unmap of the staging texture. After the unmap
// the staging texture is copied into the default texture.
//      The nonstaging method uses a "D3D10 dynamic" texture which does
// a map and unmap when the texture is locked and unlocked. This 
// method relies on the driver to correctly use buffer renaming based
// on the usage flags and the CPU access flags.
//      Measurements on a 8600 gt show the nonstaging method to be 3.84
// times as fast as DX9 and the staging method as 5.43 times as fast
// as DX9.


class NiD3D10Renderer;

class NID3D10RENDERER_ENTRY NiD3D10DynamicTextureData : 
    public NiD3D10TextureData
{

public:
    static NiD3D10DynamicTextureData* Create(NiDynamicTexture* pkTexture);

    virtual ~NiD3D10DynamicTextureData();

    virtual void* Lock(int& iPitch);

    virtual bool UnLock() const;

    virtual bool IsLocked();
    // *** begin Emergent internal use only ***
    virtual void SetLocked(bool bLocked);
    // *** end Emergent internal use only ***

protected:
    NiD3D10DynamicTextureData(NiDynamicTexture* pkTexture);

    void* Texture1DLock();
    void* Texture2DLock(int& iPitch);
    void* Texture3DLock(int& iRowPitch,int& iDepthPitch);

    const NiPixelFormat* CreateTexture(const NiDynamicTexture* pkTexture);

    bool m_bTextureLocked;

    D3D10_MAP m_kMapFlags;
    ID3D10Resource* m_pkStagingTexture;

};

typedef NiPointer<NiD3D10DynamicTextureData> NiD3D10DynamicTextureDataPtr;

#include "NiD3D10DynamicTextureData.inl"

#endif  //#ifndef NID3D10DYNAMICTEXTUREDATA_H
