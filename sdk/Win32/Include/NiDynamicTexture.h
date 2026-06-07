
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

#ifndef NIDYNAMICTEXTURE_H
#define NIDYNAMICTEXTURE_H

#include "NiTexture.h"

class NIMAIN_ENTRY NiDynamicTexture : public NiTexture
{
    NiDeclareRTTI;

protected:
    static FormatPrefs ms_kDefaultPrefs;
public:
    static NiDynamicTexture* Create(unsigned int uiWidth,
        unsigned int uiHeight, FormatPrefs& kPrefs = ms_kDefaultPrefs,
        bool bTiledTexture = false);

    virtual unsigned int GetWidth() const;
    virtual unsigned int GetHeight() const;
    inline bool GetTiled() const;

    // Lock and unlock, required for editing the texture pixels.
    virtual void* Lock(int& iPitch);
    virtual bool UnLock() const;

protected:
    NiDynamicTexture(unsigned int uiWidth, unsigned int uiHeight);

    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    bool m_bTiled;
};

typedef NiPointer<NiDynamicTexture> NiDynamicTexturePtr;

//---------------------------------------------------------------------------
#include "NiDynamicTexture.inl"
//---------------------------------------------------------------------------

#endif
