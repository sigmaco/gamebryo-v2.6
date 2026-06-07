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

#include "NiCommonMaterialLibPCH.h"

#include "NiFlatWireframeMaterial.h"
#include <NiMesh.h>
#include <NiShaderFactory.h>
#include <NiSkinningMeshModifier.h>

NiImplementRTTI(NiFlatWireframeMaterial, NiMaterial);

//---------------------------------------------------------------------------
NiFlatWireframeMaterial::NiFlatWireframeMaterial() : 
    NiMaterial("NiFlatWireframeMaterial")
{
}
//---------------------------------------------------------------------------
NiFlatWireframeMaterial::~NiFlatWireframeMaterial()
{
}
//---------------------------------------------------------------------------
bool NiFlatWireframeMaterial::IsShaderCurrent(NiShader* pkShader, 
    const NiRenderObject*,
    const NiPropertyState*, const NiDynamicEffectState*,
    unsigned int)
{
    if (pkShader == m_spSkinnedShader || pkShader == m_spUnskinnedShader)
        return true;

    return false;
}
//---------------------------------------------------------------------------
NiShader* NiFlatWireframeMaterial::GetCurrentShader(
    const NiRenderObject* pkGeometry, 
    const NiPropertyState*, 
    const NiDynamicEffectState*,
    unsigned int)
{
    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, pkGeometry);
    if (pkMesh && NiGetModifier(NiSkinningMeshModifier, pkMesh))
    {
        if (m_spSkinnedShader == NULL)
        {
            NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
            if (pkFactory)
            {
                m_spSkinnedShader = pkFactory->RetrieveShader(
                    "FlatWireframeSkinned", 
                    (unsigned int)NiShader::DEFAULT_IMPLEMENTATION);
            }
        }
        return m_spSkinnedShader;
    }
    else
    {
        if (m_spUnskinnedShader == NULL)
        {
            NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
            if (pkFactory)
            {
                m_spUnskinnedShader = pkFactory->RetrieveShader(
                    "FlatWireframe", 
                    (unsigned int)NiShader::DEFAULT_IMPLEMENTATION);
            }
        }
        return m_spUnskinnedShader;
    }
}
//---------------------------------------------------------------------------
void NiFlatWireframeMaterial::UnloadShaders()
{
    m_spSkinnedShader = NULL;
    m_spUnskinnedShader = NULL;
}
//---------------------------------------------------------------------------
