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

#ifndef NIMATERIAL_H
#define NIMATERIAL_H

#include "NiFixedString.h"
#include "NiRefObject.h"
#include "NiRTTI.h"
#include "NiSmartPointer.h"
#include "NiTFixedStringMap.h"
#include "NiSmartPointer.h"

class NiDynamicEffectState;
class NiRenderObject;
class NiMaterialInstance;
class NiPropertyState;
class NiShader;

NiSmartPointer(NiMaterial);
typedef NiTMapIterator NiMaterialIterator;

class NIMAIN_ENTRY NiMaterial : public NiRefObject
{
    NiDeclareRootRTTI(NiMaterial);
public:
    virtual ~NiMaterial();

    inline const NiFixedString& GetName() const;
    virtual bool IsShaderCurrent(NiShader* pkShader, 
        const NiRenderObject* pkGeometry,
        const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData) = 0;
    virtual NiShader* GetCurrentShader(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects,
        unsigned int uiMaterialExtraData) = 0;

    enum
    {
        VS_INPUTS_MAX_NUM           = 16,
        VS_INPUTS_USE_GEOMETRY      = 0xFFFFFFFE,
        VS_INPUTS_TERMINATE_ARRAY   = 0xFFFFFFFF
    };

    virtual void UnloadShaders() = 0;

    virtual void SetWorkingDirectory(const char* pcWorkingDir);

    static NiMaterial* GetMaterial(const NiFixedString& kName);
    static unsigned int GetMaterialCount();

    /// Convenience method to UnloadShaders on all materials.
    static void UnloadShadersForAllMaterials();

    /// Convenience method to SetWorkingDirectory on all materials.
    static void SetWorkingDirectoryForAllMaterials(const char* pcWorkingDir);

    /// Set the default working directory that is set when a NiMaterial
    /// is created.
    static void SetDefaultWorkingDirectory(const char* pcWorkingDir);

    /// Get the default working directory that is set when a NiMaterial
    /// is created.
    static const char* GetDefaultWorkingDirectory();

    /// Get an iterator to the first NiMaterial instance. 
    static NiMaterialIterator GetFirstMaterialIter();

    /// Get a pointer to the NiMaterialInstance held in
    /// the iterator and advance to the next one.
    static NiMaterial* GetNextMaterial(NiMaterialIterator& kIter);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiMaterial(const NiFixedString& kName);

    NiFixedString m_kMaterialName;

    static NiTFixedStringMap<NiMaterial*>* ms_pkMaterials;
    static char ms_acDefaultWorkingDirectory[NI_MAX_PATH];
};

#include "NiMaterial.inl"

#endif // NIMATERIAL_H
