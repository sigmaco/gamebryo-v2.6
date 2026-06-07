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


#ifndef NID3D10PERSISTENTSRCTEXTURERENDERERDATA_H
#define NID3D10PERSISTENTSRCTEXTURERENDERERDATA_H

#include <NiSystem.h>
#include <NiD3D10RendererLibType.h>
#include <NiPersistentSrcTextureRendererData.h>
#include <NiPixelFormat.h>
#include <NiSourceTexture.h>

#include "NiD3D10Headers.h"

NiSmartPointer(NiD3D10PersistentSrcTextureRendererData);


class NID3D10RENDERER_ENTRY NiD3D10PersistentSrcTextureRendererData :
    public NiPersistentSrcTextureRendererData
{
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareViewerStrings;

public:
    ~NiD3D10PersistentSrcTextureRendererData();

    // *** begin Emergent internal use only ***

    // streaming support
    virtual bool GetStreamableRTTIName(char* acName, 
        unsigned int uiMaxSize) const;

    // *** end Emergent internal use only ***

protected:
    NiD3D10PersistentSrcTextureRendererData();
};

#endif
