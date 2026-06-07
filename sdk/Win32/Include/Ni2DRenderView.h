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

#ifndef NI2DRENDERVIEW_H
#define NI2DRENDERVIEW_H

#include "NiRenderView.h"
#include "NiScreenElements.h"

class NIMAIN_ENTRY Ni2DRenderView : public NiRenderView
{
    NiDeclareRTTI;

public:
    // Function that sets the screen-space camera data for the renderer.
    virtual void SetCameraData(const NiRect<float>& kViewport);

    // Functions for accessing the list of screen elements.
    inline void AppendScreenElement(NiScreenElements* pkScreenElement);
    inline void PrependScreenElement(NiScreenElements* pkScreenElement);
    inline void RemoveScreenElement(NiScreenElements* pkScreenElement);
    inline void RemoveAllScreenElements();
    inline NiTPointerList<NiScreenElementsPtr>& GetScreenElements();
    inline const NiTPointerList<NiScreenElementsPtr>& GetScreenElements() const;

protected:
    // Function that adds all screen elements to the PV geometry array.
    virtual void CalculatePVGeometry();

    // The list of screen elements to use when rendering.
    NiTPointerList<NiScreenElementsPtr> m_kScreenElements;
};

NiSmartPointer(Ni2DRenderView);

#include "Ni2DRenderView.inl"

#endif  // #ifndef NI2DRENDERVIEW_H
