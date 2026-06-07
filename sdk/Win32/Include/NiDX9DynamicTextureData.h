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
#ifndef NIDX9DYNAMICTEXTUREDATA_H
#define NIDX9DYNAMICTEXTUREDATA_H

#include "NiDX9TextureData.h"
#include <NiDynamicTexture.h>

class NIDX9RENDERER_ENTRY NiDX9DynamicTextureData : public NiDX9TextureData
{
public:
    static NiDX9DynamicTextureData* Create(NiDynamicTexture* pkTexture);

    virtual ~NiDX9DynamicTextureData();

    virtual D3DBaseTexturePtr GetSysMemTexture() const;

    virtual bool IsLocked();
    // *** begin Emergent internal use only ***
    virtual void SetLocked(bool bLocked);
    // *** end Emergent internal use only ***

protected:
    NiDX9DynamicTextureData(NiDynamicTexture* pkTexture, 
        NiDX9Renderer* pkRenderer);

    const NiPixelFormat* CreateSurf(const NiDynamicTexture* pkTexture,
        bool bDynTexturesCap);

    D3DBaseTexturePtr m_pkD3DLockableSysMemTexture;

    bool m_bTextureLocked;
};

typedef NiPointer<NiDX9DynamicTextureData> NiDX9DynamicTextureDataPtr;

#include "NiDX9DynamicTextureData.inl"

#endif  //#ifndef NIDX9DYNAMICTEXTUREDATA_H
