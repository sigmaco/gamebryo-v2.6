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
// Emergent Game Technologies; Chapel Hill; North Carolina 27517
// http://www.emergent.net

#ifndef NIRENDERCALLCONTEXT_H
#define NIRENDERCALLCONTEXT_H

class NiBound;
class NiDynamicEffectState;
class NiRenderObject;
class NiPropertyState;
class NiSCMExtraData;
class NiTransform;

typedef void* NiVertexDeclarationCache; // also defined in NiShader.h

class NiRenderCallContext
{
public:
    NiRenderObject* m_pkMesh;
    const NiBound* m_pkWorldBound;
    const NiDynamicEffectState* m_pkEffects;
    const NiPropertyState* m_pkState;
    const NiTransform* m_pkWorld;
    const NiVertexDeclarationCache* m_pkMeshMaterialBinding;
    unsigned int m_uiPass;
    unsigned int m_uiSubmesh;

    inline NiRenderCallContext() :
        m_pkMesh(NULL),
        m_pkWorldBound(NULL),
        m_pkEffects(NULL),
        m_pkState(NULL),
        m_pkWorld(NULL),
        m_pkMeshMaterialBinding(NULL),
        m_uiPass(0),
        m_uiSubmesh(0)
    {
    }
};

#endif
