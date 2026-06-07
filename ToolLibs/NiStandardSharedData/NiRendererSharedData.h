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

#ifndef NIRENDERERSHAREDDATA_H
#define NIRENDERERSHAREDDATA_H

#include "NiStandardSharedDataLibType.h"
#include "NiSharedData.h"
#include <NiRenderer.h>
#include <NiPoint2.h>

/// A shared data object to contain a Renderer pointer.
class NISTANDARDSHAREDDATA_ENTRY NiRendererSharedData : public NiSharedData
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiRendererSharedData();

    /// Virtual destructor.
    virtual ~NiRendererSharedData();

    /// Gets a pointer to the renderer.
    /// @return A pointer to the current renderer.
    NiRendererPtr GetRenderer();
    void SetRenderer(NiRenderer* pkRenderer);

    NiColor GetBackgroundColor();
    void SetBackgroundColor(NiColor kColor);
    
    bool GetFullscreen();
    void SetFullscreen(bool bFullscreen);

    NiPoint2 GetWindowSize();
    void SetWindowSize(NiPoint2 kSize);

    void PurgeRendererData(NiAVObject* pkObject);

protected:
    /// A smart pointer to the renderer.
    NiRendererPtr m_spRenderer;

    /// The background color for the renderer
    NiColor m_kBackgroundColor;

    /// The size of the window this renderer uses
    NiPoint2 m_kWindowSize;

    /// Whether or not this renderer is fullscreen
    bool m_bFullscreen;
};

NiSmartPointer(NiRendererSharedData);

#endif  // #ifndef NIRENDERERSHAREDDATA_H
