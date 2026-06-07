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

#ifndef NIMESH2DRENDERVIEW_H
#define NIMESH2DRENDERVIEW_H

#include <NiRenderer.h>
#include <NiRenderView.h>

#include "NiMeshScreenElements.h"

/**
    A render view class that is used to render NiMeshScreenElements objects.
    
    Sets screen-space camera data and adds all screen elements in 
    its list to the potentially visible geometry array.
*/
class NIMESH_ENTRY NiMesh2DRenderView : public NiRenderView
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    
    /// @endcond

public:
    /// Sets the screen-space camera data for the renderer.
    virtual void SetCameraData(const NiRect<float>& kViewport);

    /// Add screen element to tail of list.
    inline void AppendScreenElement(NiMeshScreenElements* pkScreenElement);

    /// Add screen element to head of list.
    inline void PrependScreenElement(NiMeshScreenElements* pkScreenElement);

    /// Remove screen element from list.
    inline void RemoveScreenElement(NiMeshScreenElements* pkScreenElement);

    /// Remove all screen elements from list.
    inline void RemoveAllScreenElements();

    /// Returns a reference to the list of screen elements.
    inline NiTPointerList<NiMeshScreenElementsPtr>& GetScreenElements();

    /// Returns a const reference to the list of screen elements.
    inline const NiTPointerList<NiMeshScreenElementsPtr>&
        GetScreenElements() const;

protected:
    /// Adds all screen elements to the PV geometry array.
    virtual void CalculatePVGeometry();

    /// The list of screen elements to use when rendering.
    NiTPointerList<NiMeshScreenElementsPtr> m_kScreenElements;
};

NiSmartPointer(NiMesh2DRenderView);

#include "NiMesh2DRenderView.inl"

#endif  // #ifndef NiMesh2DRenderView_H
