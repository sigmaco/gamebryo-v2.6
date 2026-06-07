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

#ifndef NID3DXEFFECTTECHNIQUESET_H
#define NID3DXEFFECTTECHNIQUESET_H

#include "NiD3DXEffectShaderLibType.h"
#include "NiD3DXEffectTechnique.h"
#include <NiD3DRendererHeaders.h>

NiSmartPointer(NiShaderDesc);

class NiD3DXEffectAttributeTable;
class NiD3DXEffectShader;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectTechniqueSet : public NiRefObject
{
public:
    NiD3DXEffectTechniqueSet();
    ~NiD3DXEffectTechniqueSet();

    inline const char* GetName() const;
    inline unsigned int GetImplementationCount() const;

    // Get the D3D shader
    NiD3DXEffectShader* GetD3DShader(NiD3DRenderer* pkRenderer,
        unsigned int uiImplementation);
    NiD3DXEffectShader* GetBestD3DShader(NiD3DRenderer* pkRenderer);

    // *** begin Emergent internal use only ***
    NiShaderDesc* GetShaderDesc(NiD3DXEffectAttributeTable* pkAttribTable);
    inline NiD3DXEffectTechnique* GetTechnique(unsigned int uiImplementation);
    inline unsigned int GetTechniqueArrayCount() const;

    void AddImplementation(NiD3DXEffectTechnique* pkTechnique);

    void DestroyRendererData();

    // These functions are used during the creation
    void SetName(const char* pcName);

    // *** end Emergent internal use only ***

protected:
    char* m_pcName;

    unsigned int m_uiMinPixelShaderVersion;
    unsigned int m_uiMaxPixelShaderVersion;
    unsigned int m_uiMinVertexShaderVersion;
    unsigned int m_uiMaxVertexShaderVersion;
    unsigned int m_uiMinUserVersion;
    unsigned int m_uiMaxUserVersion;

    NiTObjectArray<NiD3DXEffectTechniquePtr> m_kImplementations;
    NiD3DXEffectTechnique* m_pkBestImplementation;

    NiShaderDescPtr m_spShaderDesc;
};

NiSmartPointer(NiD3DXEffectTechniqueSet);

#include "NiD3DXEffectTechniqueSet.inl"

#endif  //NID3DXEFFECTTECHNIQUESET_H
