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

// Precompiled Header
#include <NiParticlePCH.h>

#include "NiPSysMeshEmitter.h"

NiImplementRTTI(NiPSysMeshEmitter, NiPSysEmitter);

//---------------------------------------------------------------------------
NiPSysMeshEmitter::NiPSysMeshEmitter() :
    m_kGeomEmitterArray(1, 2),
    m_eInitVelocityType(NI_VELOCITY_USE_NORMALS),
    m_eEmissionType(NI_EMIT_FROM_VERTICES),
    m_kEmitAxis(NiPoint3::UNIT_X)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysMeshEmitter);
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysEmitter::LoadBinary(kStream);

    kStream.ReadMultipleLinkIDs();   // m_kGeomEmitterArray

    NiStreamLoadEnum(kStream, m_eInitVelocityType);
    NiStreamLoadEnum(kStream, m_eEmissionType);
    m_kEmitAxis.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::LinkObject(NiStream& kStream)
{
    NiPSysEmitter::LinkObject(kStream);

     // link children
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    if (uiSize)
    {
        m_kGeomEmitterArray.SetSize(uiSize);
        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiRenderObject* pkChild = 
                (NiRenderObject*) kStream.GetObjectFromLinkID();

            m_kGeomEmitterArray.AddFirstEmpty((NiGeometry*) pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysMeshEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysEmitter::SaveBinary(kStream);

    // save GeomEmitters
    unsigned int uiGeomEmittersSize = m_kGeomEmitterArray.GetSize();
    NiStreamSaveBinary(kStream, uiGeomEmittersSize);
    for (unsigned int i = 0; i < uiGeomEmittersSize; i++)
    {
        NiGeometry* pkGeom = m_kGeomEmitterArray.GetAt(i);
        kStream.SaveLinkID(pkGeom);
    }

    NiStreamSaveEnum(kStream, m_eInitVelocityType);
    NiStreamSaveEnum(kStream, m_eEmissionType);
    m_kEmitAxis.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysMeshEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysMeshEmitter* pkDest = (NiPSysMeshEmitter*) pkObject;

    // children
    unsigned int uiCount0 = m_kGeomEmitterArray.GetSize();
    unsigned int uiCount1 = pkDest->m_kGeomEmitterArray.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    for (unsigned int i = 0; i < uiCount0; i++)
    {
        NiAVObject* pkGeomEmitter0 = m_kGeomEmitterArray.GetAt(i);
        NiAVObject* pkGeomEmitter1 = pkDest->m_kGeomEmitterArray.GetAt(i);
        if ((pkGeomEmitter0 && !pkGeomEmitter1) || 
            (!pkGeomEmitter0 && pkGeomEmitter1))
        {
            return false;
        }

        if (pkGeomEmitter0 && !pkGeomEmitter0->IsEqual(pkGeomEmitter1))
            return false;
    }

    if (m_eInitVelocityType != pkDest->m_eInitVelocityType)
        return false;

    if (m_eEmissionType != pkDest->m_eEmissionType)
        return false;

    if (m_kEmitAxis != pkDest->m_kEmitAxis)
        return false;

    return true;
}
//---------------------------------------------------------------------------
