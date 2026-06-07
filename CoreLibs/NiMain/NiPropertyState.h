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

#ifndef NIPROPERTYSTATE_H
#define NIPROPERTYSTATE_H

#include "NiProperty.h"

class NiAlphaProperty;
class NiDitherProperty;
class NiFogProperty;
class NiMaterialProperty;
class NiRendererSpecificProperty;
class NiShadeProperty;
class NiSpecularProperty;
class NiStencilProperty;
class NiTexturingProperty;
class NiVertexColorProperty;
class NiWireframeProperty;
class NiZBufferProperty;

class NIMAIN_ENTRY NiPropertyState : public NiRefObject
{
public:
    NiPropertyState();
    NiPropertyState(const NiPropertyState& kState);
    ~NiPropertyState();

    NiPropertyState& operator=(const NiPropertyState& kState);
    inline NiPropertyPtr& operator[](int iType);

    void Reset();
    inline void SetProperty(NiProperty* pkProp);
    void GetProperties(NiProperty* apkProps[NiProperty::MAX_TYPES]) const;
    inline void SwapProperties(NiPropertyList& kProplist);

    inline NiAlphaProperty* GetAlpha() const;
    inline NiDitherProperty* GetDither() const;
    inline NiFogProperty* GetFog() const;
    inline NiMaterialProperty* GetMaterial() const;
    inline NiRendererSpecificProperty* GetRendererSpecific() const;
    inline NiShadeProperty* GetShade() const;
    inline NiSpecularProperty* GetSpecular() const;
    inline NiStencilProperty* GetStencil() const;
    inline NiTexturingProperty* GetTexturing() const;
    inline NiVertexColorProperty* GetVertexColor() const;
    inline NiWireframeProperty* GetWireframe() const;
    inline NiZBufferProperty* GetZBuffer() const;

protected:
    NiPropertyPtr m_aspProps[NiProperty::MAX_TYPES];
};

typedef NiPointer<NiPropertyState> NiPropertyStatePtr;

//---------------------------------------------------------------------------
//  Inline include
#include "NiPropertyState.inl"

//---------------------------------------------------------------------------

#endif
