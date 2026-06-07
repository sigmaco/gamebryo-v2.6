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

#ifndef NID3D10EFFECTPASS_H
#define NID3D10EFFECTPASS_H

#include "NiD3D10EffectShaderLibType.h"
#include <NiD3D10Headers.h>
#include <NiD3D10Pass.h>

class NiD3D10EffectTechnique;

NiSmartPointer(NiD3D10EffectPass);

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectPass : public NiD3D10Pass
{
public:
    static NiD3D10PassPtr Create();

    virtual unsigned int ApplyShaderPrograms(const NiRenderCallContext& kRCC);
    virtual unsigned int ApplyShaderConstants(const NiRenderCallContext& kRCC);

    // *** begin Emergent internal use only

    void SetStateBlockMask(const D3D10_STATE_BLOCK_MASK* pkMask);
    const D3D10_STATE_BLOCK_MASK* GetStateBlockMask() const;

    void SetD3D10Pass(ID3D10EffectPass* pkD3D10Pass);
    ID3D10EffectPass* GetD3D10Pass() const;

    void AddConstantMap(NiD3D10ShaderConstantMap* pkMap);

    // *** end Emergent internal use only

protected:
    NiD3D10EffectPass();
    virtual ~NiD3D10EffectPass();

    // Only store pointer to the mask; the struct itself will reside in
    // the NiD3D10EffectTechnique::EffectPass object.
    const D3D10_STATE_BLOCK_MASK* m_pkStateBlockMask;
    ID3D10EffectPass* m_pkPass;
};

#endif  //#ifndef NID3D10EFFECTPASS_H
