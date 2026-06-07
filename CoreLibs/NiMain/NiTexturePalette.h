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

#ifndef NITEXTUREPALETTE_H
#define NITEXTUREPALETTE_H

#include "NiRefObject.h"
#include "NiTStringMap.h"

#include "NiSourceTexture.h"

class NIMAIN_ENTRY NiTexturePalette : public NiRefObject
{
public:
    inline NiTexturePalette() { /* */ }
    inline ~NiTexturePalette() { /* */ }

    virtual NiTexture* GetTexture(const char* pcPath, NiTexture* pkSelf) const
        = 0;
    virtual void SetTexture(const char* pcPath, NiTexture* pkTex) = 0;
};

class NIMAIN_ENTRY NiDefaultTexturePalette : public NiTexturePalette
{
public:
    NiDefaultTexturePalette(bool bSearchGlobalTextureList = false);
    inline ~NiDefaultTexturePalette() { /* */ }

    inline virtual NiTexture* GetTexture(const char* pcPath,
        NiTexture* pkSelf) const;
    virtual void SetTexture(const char* pcPath, NiTexture* pkTex);

    virtual void SetSearchGlobalTextureList(bool bSearch);
    virtual bool GetSearchGlobalTextureList() const;

protected:
    NiTStringPointerMap<NiTexturePtr> m_kHash;
    bool m_bSearchGlobalTextureList;
};

#include "NiTexturePalette.inl"

#endif
