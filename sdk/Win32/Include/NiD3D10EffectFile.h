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

#ifndef NID3D10EFFECTFILE_H
#define NID3D10EFFECTFILE_H

#include "NiD3D10EffectAttributeTable.h"
#include "NiD3D10EffectShaderLibType.h"
#include "NiD3D10EffectTechniqueSet.h"

#include <NiD3D10Headers.h>
#include <NiFixedString.h>
#include <NiMemObject.h>
#include <NiTFixedStringMap.h>
#include <NiUniversalTypes.h>

class NiShaderDesc;
class NiD3D10EffectShader;

NiSmartPointer(NiD3D10ShaderConstantMap);

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectFile : public NiMemObject
{

public:
    NiD3D10EffectFile(ID3D10Blob* pkCompiledEffect);
    ~NiD3D10EffectFile();

    // Name
    inline const NiFixedString& GetName() inline const;
    
    // *** begin Emergent internal use only ***
    inline ID3D10Effect* GetEffect() const;

    NiD3D10EffectTechnique* GetFirstTechnique();
    NiD3D10EffectTechnique* GetNextTechnique();

    NiD3D10EffectTechniqueSet* GetTechniqueSet(const NiFixedString& kName)
        const;

    NiShaderDesc* GetFirstShaderDesc() const;
    NiShaderDesc* GetNextShaderDesc() const;

    NiD3D10ShaderConstantMap* GetShaderConstantMap(const NiFixedString& kName)
        const;

    inline void SetName(const NiFixedString& kName);

    // Parse the effect - this should only ever be called once.
    bool ParseEffect(ID3D10Effect* pkEffect);

    inline void* GetCompiledEffectData() const;
    inline NiUInt32 GetCompiledEffectDataSize() const;

    // Set/release effect and other effect-derived handles
    void SetD3D10Effect(ID3D10Effect* pkEffect);
    void ReleaseD3D10Effect();

    // Set/release renderer-owned data, including effect-derived handles
    void LinkRendererData();
    void DestroyRendererData();

    // *** end Emergent internal use only ***

protected:

    void CreateShaderConstantMaps(ID3D10Effect* pkEffect);

    void AddTechnique(NiD3D10EffectTechnique* pkTechnique);

    NiFixedString m_kName;

    // Techniques and parameters
    NiTFixedStringMap<NiD3D10EffectTechniqueSetPtr> m_kTechniques;
    NiTPointerList<NiD3D10EffectParameter*> m_kParameters;
    mutable NiTMapIterator m_kTechniqueIter;
    NiUInt32 m_uiTechniqueIndex;
    NiD3D10EffectTechniqueSet* m_pkCurrTechniqueSet;

    NiD3D10EffectAttributeTable m_kAttribTable;

    // Shader constant maps
    NiTFixedStringMap<NiD3D10ShaderConstantMapPtr> m_kConstantMaps;
    
    ID3D10Blob* m_pkCompiledEffect;
    ID3D10Effect* m_pkD3D10Effect;

    bool m_bConstantBuffersLoaded;
};

#include "NiD3D10EffectFile.inl"

#endif  //NID3D10EFFECTFILE_H
