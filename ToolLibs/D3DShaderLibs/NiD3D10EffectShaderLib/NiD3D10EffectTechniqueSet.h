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

#ifndef NID3D10EFFECTTECHNIQUESET_H
#define NID3D10EFFECTTECHNIQUESET_H

#include "NiD3D10EffectShaderLibType.h"
#include "NiD3D10EffectTechnique.h"

NiSmartPointer(NiShaderDesc);

class NiD3D10EffectAttributeTable;
class NiD3D10EffectShader;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectTechniqueSet : public NiRefObject
{
public:
    NiD3D10EffectTechniqueSet();
    ~NiD3D10EffectTechniqueSet();

    inline const NiFixedString& GetName() const;
    inline NiUInt32 GetImplementationCount() const;

    // Get the D3D10 shader
    NiD3D10EffectShader* GetD3D10Shader(NiUInt32 uiImplementation);
    NiD3D10EffectShader* GetBestD3D10Shader();

    // *** begin Emergent internal use only ***
    NiShaderDesc* GetShaderDesc(
        const NiD3D10EffectAttributeTable* pkAttribTable);
    inline NiD3D10EffectTechnique* GetTechnique(NiUInt32 uiImplementation);
    inline NiUInt32 GetTechniqueArrayCount() const;

    void AddImplementation(NiD3D10EffectTechnique* pkTechnique);

    void DestroyRendererData();

    // These functions are used during the creation
    inline void SetName(const NiFixedString& kName);

    // *** end Emergent internal use only ***

protected:
    NiFixedString m_kName;

    NiTObjectArray<NiD3D10EffectTechniquePtr> m_kImplementations;
    NiD3D10EffectTechnique* m_pkBestImplementation;

    NiShaderDescPtr m_spShaderDesc;
};

NiSmartPointer(NiD3D10EffectTechniqueSet);

#include "NiD3D10EffectTechniqueSet.inl"

#endif  //NID3D10EFFECTTECHNIQUESET_H
