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

#ifndef NID3DDEFAULTSHADER_H
#define NID3DDEFAULTSHADER_H

#include "NiD3DShader.h"

class NiD3DTextureStage;
class NiDynamicEffectState;

// The Gamebryo fixed function pipeline
class NID3D_ENTRY NiD3DDefaultShader : public NiD3DShader
{
    NiDeclareRTTI;
public:
    NiD3DDefaultShader();
    virtual ~NiD3DDefaultShader();

    // Initialize function
    virtual bool Initialize();

    // Override these functions to implement a custom pipeline...
    virtual unsigned int PreProcessPipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int UpdatePipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int PostProcessPipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int PreRenderSubmesh(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupTransformations(const NiRenderCallContext& kRCC);

    // This function is called by NiMaterialInstance::GetCurrentShader, and
    // is used to verify the geometry is usable for the shader.
    virtual bool SetupGeometry(NiRenderObject* pkGeometry, 
        NiMaterialInstance* pkMaterialInstance);

    void UseQuickDecals(bool bQuickDecals);

    // *** begin Emergent internal use only ***

    void NextFrame();

    // *** end Emergent internal use only ***

protected:
    void InitializeDeviceCaps();

    bool CreateSemanticAdapterTable(NiRenderObject* pkGeometry);

    unsigned int ConstructPipeline(const NiRenderCallContext& kRCC);

    void ClearProjectedTextures();
    void UpdateProjectedTextures(const NiDynamicEffectState* pkEffects);

    void PostProcessFogProperties(const NiFogProperty* pkFog, 
        const NiAlphaProperty* pkAlpha, 
        const NiTexturingProperty::Map* pkBaseMap, 
        NiTexturingProperty::ApplyMode eApply);

    bool SaveProjectedTexture(const NiTextureEffect* pkEffect, 
        bool bClipped = false);

    const NiTextureEffect* GetClippedProjectedLight(unsigned int uiIndex)
        const;
    const NiTextureEffect* GetProjectedLight(unsigned int uiIndex) const;
    const NiTextureEffect* GetClippedProjectedShadow(unsigned int uiIndex)
        const;
    const NiTextureEffect* GetProjectedShadow(unsigned int uiIndex) const;
    const NiTextureEffect* GetEnvironmentMap(unsigned int uiIndex) const;
    const NiTextureEffect* GetFogMap(unsigned int uiIndex) const;
    unsigned int GetClippedProjectedLightCount() const;
    unsigned int GetProjectedLightCount() const;
    unsigned int GetClippedProjectedShadowCount() const;
    unsigned int GetProjectedShadowCount() const;
    unsigned int GetEnvironmentMapCount() const;
    unsigned int GetFogMapCount() const;

    void OpenNewPass(D3DBLEND eSrcBlend = D3DBLEND_ONE, 
        D3DBLEND eDestBlend = D3DBLEND_ZERO, 
        bool bReturnBaseTextureAlpha = false, 
        bool bReturnVertexColorAlpha = false);
    void ClosePass(const NiTexturingProperty::Map* pkBaseMap = NULL);
    NiD3DTextureStage* AddTextureStage(const NiTexturingProperty::Map* pkMap, 
        unsigned int uiIndex);
    NiD3DTextureStage* AddTextureStage(
        const NiTexturingProperty::BumpMap* pkMap, unsigned int uiIndex);
    NiD3DTextureStage* AddTextureStage(const NiTextureEffect* pkEffect);
    NiD3DTextureStage* AddClipperStage(const NiTextureEffect* pkEffect);
    bool IsCurrentCombineMode(D3DBLEND eSrcBlend, D3DBLEND eDestBlend);

    // These arrays will only be added to or emptied completely, so
    // they should never not be compact.
    typedef NiTPrimitiveArray<const NiTextureEffect*>  NiTextureEffectArray;

    NiTextureEffectArray m_kClippedProjectedLights;
    NiTextureEffectArray m_kProjectedLights;
    NiTextureEffectArray m_kClippedProjectedShadows;
    NiTextureEffectArray m_kProjectedShadows;
    NiTextureEffectArray m_kEnvironmentMaps;
    NiTextureEffectArray m_kFogMaps;

    // General purpose texture transform packer
    void PackTextureTransform(const NiMatrix3& kTexMatrix, 
        NiD3DTextureStage* pkStage);

    // Pipeline construction values
    bool m_bBaseTextureAlphaNeeded;
    bool m_bVertexColorAlphaNeeded;
    unsigned int m_uiMaxTextureIndex;
    unsigned int m_uiRemainingStages;
    unsigned int m_uiRemainingTextures;
    unsigned int m_uiOldLightingValue;

    enum PassFogStatus
    {
        NO_FOG,
        BLACK_FOG,
        STANDARD_FOG
    };

    PassFogStatus* m_peFogPassArray;
    unsigned int m_uiFogPassArraySize;

    // Caps dependent values
    bool m_bBlendTextureAlpha;
    D3DTEXTUREOP m_eEnvBumpOp;

    bool m_bQuickDecals;

    const NiDynamicEffectState* m_pkLastState;
};

typedef NiPointer<NiD3DDefaultShader> NiD3DDefaultShaderPtr;

#endif  //#ifndef NID3DDEFAULTSHADER_H
