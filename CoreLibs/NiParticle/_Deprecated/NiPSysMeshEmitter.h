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

#ifndef NIPSYSMESHEMITTER_H
#define NIPSYSMESHEMITTER_H

#include "NiPSysEmitter.h"
#include <NiGeometry.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSMeshEmitter.
*/
class NIPARTICLE_ENTRY NiPSysMeshEmitter : public NiPSysEmitter
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    enum MeshEmissionType
    {
        NI_EMIT_FROM_VERTICES = 0,
        NI_EMIT_FROM_FACE_CENTER,
        NI_EMIT_FROM_EDGE_CENTER,
        NI_EMIT_FROM_FACE_SURFACE,
        NI_EMIT_FROM_EDGE_SURFACE,
        NI_EMIT_MAX
    };
    enum InitialVelocityType
    {
        NI_VELOCITY_USE_NORMALS = 0,
        NI_VELOCITY_USE_RANDOM,
        NI_VELOCITY_USE_DIRECTION,
        NI_VELOCITY_MAX
    };
    inline NiGeometry* GetGeometryEmitter(unsigned int uiWhichEmitter);
    inline unsigned int GetGeometryEmitterCount();
    inline unsigned int GetGeometryEmitterArrayCount();
    inline MeshEmissionType GetMeshEmissionType();
    inline InitialVelocityType GetInitialVelocityType();
    inline const NiPoint3& GetEmitAxis();
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysMeshEmitter();

    // Emitter object and size.
    NiTObjectArray<NiGeometryPtr> m_kGeomEmitterArray;
    InitialVelocityType m_eInitVelocityType;
    MeshEmissionType m_eEmissionType;
    NiPoint3 m_kEmitAxis;
};

NiSmartPointer(NiPSysMeshEmitter);

/// @endcond

#include "NiPSysMeshEmitter.inl"

#endif  // #ifndef NIPSYSMESHEMITTER_H
