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

#ifndef NISINGLESHADERMATERIAL_H
#define NISINGLESHADERMATERIAL_H

#include "NiMaterial.h"
#include "NiShader.h"

class NIMAIN_ENTRY NiSingleShaderMaterial : public NiMaterial
{
    NiDeclareRTTI;
public:
    static NiSingleShaderMaterial* Create(NiShader* pkShader,
        bool bManualShaderManagement = false);
    static NiSingleShaderMaterial* Create(const char* pcName,
        bool bPreloadShader = true);
    virtual ~NiSingleShaderMaterial();

    virtual bool IsShaderCurrent(NiShader* pkShader, 
        const NiRenderObject* pkGeometry,
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);
    virtual NiShader* GetCurrentShader(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData);
    virtual void UnloadShaders();

    // Begin Emergent Internal Use Only
    void SetCachedShader(NiShader* pkShader);
    bool GetManualShaderManagement();
    // End Emergent Internal Use Only

protected:
    NiSingleShaderMaterial(const NiFixedString& kName);
    NiSingleShaderMaterial(const NiFixedString& kName, NiShader* pkShader,
        bool bManualShaderManagement);
    NiSingleShaderMaterial(const NiFixedString& kName, const char* pcName, 
        bool bPreloadShader);

    NiFixedString m_kShaderName;
    NiShaderPtr m_spCachedShader;
    bool m_bManualShaderManagement;
};

#endif // NISINGLESHADERMATERIAL_H
