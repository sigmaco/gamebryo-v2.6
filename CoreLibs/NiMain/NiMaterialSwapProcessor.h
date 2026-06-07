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

#ifndef NIMATERIALSWAPPROCESSOR_H
#define NIMATERIALSWAPPROCESSOR_H

#include "NiRenderListProcessor.h"
#include "NiMaterialInstance.h"

class NIMAIN_ENTRY NiMaterialSwapProcessor : public NiRenderListProcessor
{
    NiDeclareRTTI;

public:
    NiMaterialSwapProcessor(NiMaterial* pkMaterial = NULL,
        unsigned int uiMaterialExtraData =
        NiMaterialInstance::DEFAULT_EXTRA_DATA);

    // Pre-processing function called before any objects are rendered.
    virtual void PreRenderProcessList(const NiVisibleArray* pkInput,
        NiVisibleArray& kOutput, void* pvExtraData);

    // Functions for accessing the NiMaterial to apply.
    inline void SetMaterial(NiMaterial* pkMaterial, unsigned int uiMaterialExtraData);
    inline NiMaterial* GetMaterial() const;
    inline unsigned int GetMaterialExtraData() const;

protected:
    // The NiMaterial to apply to objects when rendering.
    NiMaterialPtr m_spMaterial;

    // Extra data for the material.
    unsigned int m_uiMaterialExtraData;
};

NiSmartPointer(NiMaterialSwapProcessor);

#include "NiMaterialSwapProcessor.inl"

#endif  // #ifndef NIMATERIALSWAPPROCESSOR_H
