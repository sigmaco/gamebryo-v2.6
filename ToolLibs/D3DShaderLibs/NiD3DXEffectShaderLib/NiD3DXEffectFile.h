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

#ifndef NID3DXEFFECTFILE_H
#define NID3DXEFFECTFILE_H

#include "NiD3DXEffectAttributeTable.h"
#include "NiD3DXEffectShaderLibType.h"
#include "NiD3DXEffectTechniqueSet.h"

class NiShaderDesc;
class NiD3DXEffectShader;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectFile : public NiMemObject
{

public:
    NiD3DXEffectFile();
    ~NiD3DXEffectFile();

    bool Initialize(LPD3DXBASEEFFECT pkD3DXEffect, D3DDevicePtr pkD3DDevice);

    // Name
    inline const char* GetName() const;
    
    // Get the D3D shader
/*    NiD3DXEffectShader* GetD3DShader(NiD3DRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    bool ReleaseShader(const char* pcName, unsigned int uiImplementation);
*/
    // *** begin Emergent internal use only ***
    inline LPD3DXBASEEFFECT GetEffect() const;
    inline LPD3DXEFFECT GetRenderableEffect() const;

    NiD3DXEffectTechnique* GetFirstTechnique();
    NiD3DXEffectTechnique* GetNextTechnique();

//    NiD3DXEffectTechnique* GetTechnique(const char* pcName, 
//       unsigned int uiImplementation) const;
    NiD3DXEffectTechniqueSet* GetTechniqueSet(const char* pcName) const;

    NiShaderDesc* GetFirstShaderDesc();
    NiShaderDesc* GetNextShaderDesc();

    // These functions are used during the creation
    void SetName(const char* pszName);
    inline void SetEffect(LPD3DXBASEEFFECT pkEffect);
    inline void SetDXVersion(unsigned int uiDXVersion);

    void LoadEffect(NiD3DRenderer* pkRenderer);

    void HandleLostDevice();
    void HandleResetDevice();

    void DestroyRendererData();

    // *** end Emergent internal use only ***

protected:
    bool ParseEffect(D3DDevicePtr pkD3DDevice);
    void AddTechnique(NiD3DXEffectTechnique* pkTechnique,
        D3DDevicePtr pkD3DDevice);
    bool LinkHandles();

    bool ParseSemantics(D3DXParameterPtr pkParam, 
        D3DXPARAMETER_DESC* pkParamDesc, char*& pcDesc, bool& bHidden, 
        float*& pfInit, float*& pfMin, float*& pfMax);

    char* m_pcName;
    unsigned int m_uiDXVersion;

    NiTStringMap<NiD3DXEffectTechniqueSetPtr> m_kTechniques;
    NiTPointerList<NiD3DXEffectParameter*> m_kParameters;
    NiTMapIterator m_kTechniqueIter;
    unsigned int m_uiIndex;
    NiD3DXEffectTechniqueSet* m_pkCurrTechniqueSet;

    NiD3DXEffectAttributeTable m_kAttribTable;

    LPD3DXBASEEFFECT m_pkD3DXEffect;
    bool m_bRenderableEffect;
    bool m_bParsed;
};

#include "NiD3DXEffectFile.inl"

#endif  //NID3DXEFFECTFILE_H
