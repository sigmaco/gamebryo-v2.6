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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiImplementRTTI(NiPhysXMaterialDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXMaterialDesc::NiPhysXMaterialDesc(const NiUInt16 usIndex)
{
    m_usIndex = usIndex;
    m_uiNumStates = 0;
    m_pkMaterialDescs = 0;
}
//---------------------------------------------------------------------------
NiPhysXMaterialDesc::~NiPhysXMaterialDesc()
{
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        if (m_pkMaterialDescs[ui].spring)
            NiFree(m_pkMaterialDescs[ui].spring);
    }
    NiFree(m_pkMaterialDescs);
}
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::FromMaterialDesc(
    const NxMaterialDesc& kMaterialDesc, const NiUInt32 uiIndex)
{
    if (m_uiNumStates <= uiIndex)
    {
        m_uiNumStates = uiIndex + 1;
        m_pkMaterialDescs = (NxMaterialDesc*)NiRealloc(
            (void*)m_pkMaterialDescs, sizeof(NxMaterialDesc) * m_uiNumStates);
        NIASSERT(m_pkMaterialDescs);
    }

    m_pkMaterialDescs[uiIndex] = kMaterialDesc;
    m_pkMaterialDescs[uiIndex].spring = 0;
}
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::RemoveStatesBeyond(const NiUInt32 uiState)
{
    if (m_uiNumStates > uiState)
    {
        m_uiNumStates = uiState;
        m_pkMaterialDescs = (NxMaterialDesc*)NiRealloc(
            (void*)m_pkMaterialDescs, m_uiNumStates * sizeof(NxMaterialDesc));
        NIASSERT(m_pkMaterialDescs);
    }
}
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::RemoveState(const NiUInt32 uiState)
{
    if (m_uiNumStates > uiState)
    {
        NxMaterialDesc* pkNewArray =
            NiAlloc(NxMaterialDesc, m_uiNumStates - 1);
        NiUInt32 uiIndex = 0;
        for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
        {
            if (ui != uiState)
                pkNewArray[uiIndex++] = m_pkMaterialDescs[ui];
        }
        NiFree(m_pkMaterialDescs);
        m_pkMaterialDescs = pkNewArray;
        m_uiNumStates--;
    }
}
//---------------------------------------------------------------------------
bool NiPhysXMaterialDesc::IsDataEqual(const NxMaterialDesc& kMaterialDesc,
    const NiUInt32 uiIndex)
{
    if (m_pkMaterialDescs[uiIndex].dynamicFriction !=
        kMaterialDesc.dynamicFriction ||
        m_pkMaterialDescs[uiIndex].staticFriction !=
        kMaterialDesc.staticFriction ||
        m_pkMaterialDescs[uiIndex].restitution !=
        kMaterialDesc.restitution ||
        m_pkMaterialDescs[uiIndex].dynamicFrictionV !=
        kMaterialDesc.dynamicFrictionV ||
        m_pkMaterialDescs[uiIndex].staticFrictionV !=
        kMaterialDesc.staticFrictionV ||
        m_pkMaterialDescs[uiIndex].dirOfAnisotropy !=
        kMaterialDesc.dirOfAnisotropy ||
        m_pkMaterialDescs[uiIndex].flags !=
        kMaterialDesc.flags ||
        m_pkMaterialDescs[uiIndex].frictionCombineMode !=
        kMaterialDesc.frictionCombineMode ||
        m_pkMaterialDescs[uiIndex].restitutionCombineMode !=
        kMaterialDesc.restitutionCombineMode)
        return false;
   
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXMaterialDesc);
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::CopyMembers(NiPhysXMaterialDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_usIndex = m_usIndex;
    pkDest->m_uiNumStates = m_uiNumStates;
    pkDest->m_pkMaterialDescs = NiAlloc(NxMaterialDesc, m_uiNumStates);
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        pkDest->m_pkMaterialDescs[ui] = m_pkMaterialDescs[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXMaterialDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usIndex);
    
    NiStreamSaveBinary(kStream, m_uiNumStates);
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        NiStreamSaveBinary(kStream, m_pkMaterialDescs[ui].dynamicFriction);
        NiStreamSaveBinary(kStream, m_pkMaterialDescs[ui].staticFriction);
        NiStreamSaveBinary(kStream, m_pkMaterialDescs[ui].restitution);
        NiStreamSaveBinary(kStream, m_pkMaterialDescs[ui].dynamicFrictionV);
        NiStreamSaveBinary(kStream, m_pkMaterialDescs[ui].staticFrictionV);
        NiPhysXTypes::NxVec3SaveBinary(kStream,
            m_pkMaterialDescs[ui].dirOfAnisotropy);
        NiStreamSaveBinary(kStream, m_pkMaterialDescs[ui].flags);
        NiStreamSaveEnum(kStream, m_pkMaterialDescs[ui].frictionCombineMode);
        NiStreamSaveEnum(kStream,
            m_pkMaterialDescs[ui].restitutionCombineMode);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMaterialDesc);
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_usIndex);
    
    NiStreamLoadBinary(kStream, m_uiNumStates);
    m_pkMaterialDescs = NiAlloc(NxMaterialDesc, m_uiNumStates);

    NiPoint3 kP;
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        NiStreamLoadBinary(kStream, m_pkMaterialDescs[ui].dynamicFriction);
        NiStreamLoadBinary(kStream, m_pkMaterialDescs[ui].staticFriction);
        NiStreamLoadBinary(kStream, m_pkMaterialDescs[ui].restitution);
        NiStreamLoadBinary(kStream, m_pkMaterialDescs[ui].dynamicFrictionV);
        NiStreamLoadBinary(kStream, m_pkMaterialDescs[ui].staticFrictionV);
        NiPhysXTypes::NxVec3LoadBinary(kStream,
            m_pkMaterialDescs[ui].dirOfAnisotropy);
        NiStreamLoadBinary(kStream, m_pkMaterialDescs[ui].flags);
        NiStreamLoadEnum(kStream, m_pkMaterialDescs[ui].frictionCombineMode);
        NiStreamLoadEnum(kStream,
            m_pkMaterialDescs[ui].restitutionCombineMode);
        
        if (kStream.GetFileVersion() <= kStream.GetVersion(20, 2, 3, 0))
        {
            // Load deprecated spring information.
            NiBool bVal;
            NiStreamLoadBinary(kStream, bVal);
            if (bVal)
            {
                NxSpringDesc kSpring;
                NiStreamLoadBinary(kStream, kSpring.damper);
                NiStreamLoadBinary(kStream, kSpring.spring);
                NiStreamLoadBinary(kStream, kSpring.targetValue);            
            }
        }
        m_pkMaterialDescs[ui].spring = 0;
    }
}
//---------------------------------------------------------------------------
void NiPhysXMaterialDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMaterialDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXMaterialDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXMaterialDesc* pkNewObject = (NiPhysXMaterialDesc*)pkObject;
   
    if (m_uiNumStates != pkNewObject->m_uiNumStates)
        return false;
        
    for (NiUInt32 ui = 0; ui < m_uiNumStates; ui++)
    {
        if (!IsDataEqual(pkNewObject->m_pkMaterialDescs[ui], ui))
            return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
