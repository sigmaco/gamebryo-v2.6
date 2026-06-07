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


#ifndef NIDX9PERSISTENTSRCTEXTURERENDERERDATA_H
#define NIDX9PERSISTENTSRCTEXTURERENDERERDATA_H

#include <NiSystem.h>
#include <NiPersistentSrcTextureRendererData.h>
#include <NiPixelFormat.h>
#include <NiSourceTexture.h>

#include "NiDX9RendererLibType.h"
#include "NiDX9Headers.h"

NiSmartPointer(NiDX9PersistentSrcTextureRendererData);


class NIDX9RENDERER_ENTRY NiDX9PersistentSrcTextureRendererData :
    public NiPersistentSrcTextureRendererData
{
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareViewerStrings;

public:
    ~NiDX9PersistentSrcTextureRendererData();

    // *** begin Emergent internal use only ***

    // streaming support
    virtual bool GetStreamableRTTIName(char* acName, 
        unsigned int uiMaxSize) const;

    // *** end Emergent internal use only ***

protected:
    NiDX9PersistentSrcTextureRendererData();
};

#endif
