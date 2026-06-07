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

#ifndef NISCREENFILLINGRENDERVIEW_H
#define NISCREENFILLINGRENDERVIEW_H

#include "NiRenderView.h"
#include "NiDynamicEffect.h"
#include "NiCriticalSection.h"

class NIMAIN_ENTRY NiScreenFillingRenderView : public NiRenderView
{
    NiDeclareRTTI;

public:
    // factory method. Implementation provided by derived class
    static NiScreenFillingRenderView* Create();

    NiScreenFillingRenderView();
    virtual ~NiScreenFillingRenderView();

    // Function that sets the screen-space camera data for the renderer.
    virtual void SetCameraData(const NiRect<float>& kViewport);

    // Functions for manipulating the properties on the screen-filling quad.
    virtual void AttachProperty(NiProperty* pkProperty) = 0;
    virtual void DetachProperty(NiProperty* pkProperty) = 0;
    virtual void DetachAllProperties() = 0;
    virtual NiProperty* GetProperty(int iType) const = 0;
    virtual NiPropertyPtr RemoveProperty(int iType) = 0;

    // Functions for manipulating dynamic effects for the screen-filling quad.
    inline void AttachEffect(NiDynamicEffect* pkEffect);
    inline void DetachEffect(NiDynamicEffect* pkEffect);
    inline void DetachAllEffects();

    // Access to the underlying screen-filling quad geometry object so that
    // materials and shaders can be attached.
    virtual NiRenderObject& GetScreenFillingQuad() const = 0;

protected:
    // Static factory method. To be assigned by derived class
    static NiScreenFillingRenderView* (*CreateFunc)();
    // Function that adds the screen-filling quad to the PV geometry array.
    virtual void CalculatePVGeometry() = 0;

    // List of dynamic effects and an effect state to use when rendering the
    // screen-filling quad.
    NiDynamicEffectList m_kEffectList;
    NiDynamicEffectStatePtr m_spEffectState;
    bool m_bEffectsChanged;
};

NiSmartPointer(NiScreenFillingRenderView);

#include "NiScreenFillingRenderView.inl"

#endif  // #ifndef NISCREENFILLINGRENDERVIEW_H
