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

#ifndef NIFLATWIREFRAMEMATERIAL_H
#define NIFLATWIREFRAMEMATERIAL_H

#include "NiMaterial.h"
#include "NiShader.h"

#include "NiCommonMaterialLibType.h"

class NICOMMONMATERIAL_ENTRY NiFlatWireframeMaterial : public NiMaterial
{
    NiDeclareRTTI;
public:
    NiFlatWireframeMaterial();
    virtual ~NiFlatWireframeMaterial();

    virtual bool IsShaderCurrent(NiShader* pkShader, 
        const NiRenderObject* pkGeometry,
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);
    virtual NiShader* GetCurrentShader(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);
    virtual void UnloadShaders();

protected:
    NiShaderPtr m_spSkinnedShader;
    NiShaderPtr m_spUnskinnedShader;
};

#endif // NISINGLESHADERMATERIAL_H
