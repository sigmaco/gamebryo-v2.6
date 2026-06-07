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

NiImplementRTTI(NiPhysXShapeDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXShapeDesc::NiPhysXShapeDesc() : m_kShapeName(NULL)
{
    // Where possible these are the PhysX defults from SDK V2.3.1
    m_eType = NX_SHAPE_COUNT;
    m_kLocalPose.id();
    m_uiShapeFlags =
        NX_SF_VISUALIZATION | NX_SF_CLOTH_TWOWAY | NX_SF_SOFTBODY_TWOWAY;
    m_uiShapeGroup = 0;
    m_uiMaterialIndex = 0;
    m_fDensity = 1.0f;
    m_fMass = -1.0f;
    m_fSkinWidth= -1.0f;
    m_uiNonInteractingCompartmentTypes = 0;
    m_auiCollisionBits[0] = 0;
    m_auiCollisionBits[1] = 0;
    m_auiCollisionBits[2] = 0;
    m_auiCollisionBits[3] = 0;
    
    m_fVal1 = 0.0f;
    m_fVal2 = 0.0f;
    m_kPoint1.zero();
    m_spMeshDesc = 0;
    
    m_pkShape = 0;
}
//---------------------------------------------------------------------------
NiPhysXShapeDesc::~NiPhysXShapeDesc()
{
    m_spMeshDesc = 0;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToShapeDesc(NxShapeDesc& kShapeDesc) const
{
    kShapeDesc.name = m_kShapeName;
    kShapeDesc.localPose = m_kLocalPose;
    kShapeDesc.shapeFlags = m_uiShapeFlags;
    kShapeDesc.group = m_uiShapeGroup;
    kShapeDesc.materialIndex = m_uiMaterialIndex;
    kShapeDesc.ccdSkeleton = 0;
    kShapeDesc.density = m_fDensity;
    kShapeDesc.mass = m_fMass;
    kShapeDesc.skinWidth = m_fSkinWidth;
    kShapeDesc.nonInteractingCompartmentTypes = 
        m_uiNonInteractingCompartmentTypes;
    kShapeDesc.groupsMask.bits0 = m_auiCollisionBits[0];
    kShapeDesc.groupsMask.bits1 = m_auiCollisionBits[1];
    kShapeDesc.groupsMask.bits2 = m_auiCollisionBits[2];
    kShapeDesc.groupsMask.bits3 = m_auiCollisionBits[3];
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::FromShapeDesc(const NxShapeDesc& kShapeDesc)
{
    if (m_eType == NX_SHAPE_COUNT)
        m_eType = kShapeDesc.getType();
    
    m_kShapeName = kShapeDesc.name;    
    m_kLocalPose = kShapeDesc.localPose;
    m_uiShapeFlags = kShapeDesc.shapeFlags;
    m_uiShapeGroup = kShapeDesc.group;
    m_uiMaterialIndex = kShapeDesc.materialIndex;
    m_fDensity = kShapeDesc.density;
    m_fMass = kShapeDesc.mass;
    m_fSkinWidth = kShapeDesc.skinWidth;
    m_uiNonInteractingCompartmentTypes = 
        kShapeDesc.nonInteractingCompartmentTypes;
    m_auiCollisionBits[0] = kShapeDesc.groupsMask.bits0;
    m_auiCollisionBits[1] = kShapeDesc.groupsMask.bits1;
    m_auiCollisionBits[2] = kShapeDesc.groupsMask.bits2;
    m_auiCollisionBits[3] = kShapeDesc.groupsMask.bits3;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToPlaneDesc(NxPlaneShapeDesc& kPlaneDesc) const
{
    NIASSERT(m_eType == NX_SHAPE_PLANE);

    ToShapeDesc(kPlaneDesc);
    
    kPlaneDesc.d = m_fVal1;
    kPlaneDesc.normal = m_kPoint1;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::FromPlaneDesc(const NxPlaneShapeDesc& kPlaneDesc)
{
    FromShapeDesc(kPlaneDesc);

    m_fVal1 = kPlaneDesc.d;
    m_kPoint1 = kPlaneDesc.normal;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToSphereDesc(NxSphereShapeDesc& kSphereDesc) const
{
    NIASSERT(m_eType == NX_SHAPE_SPHERE);

    ToShapeDesc(kSphereDesc);
    
    kSphereDesc.radius = m_fVal1;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::FromSphereDesc(const NxSphereShapeDesc& kSphereDesc)
{
    FromShapeDesc(kSphereDesc);

    m_fVal1 = kSphereDesc.radius;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToCapsuleDesc(NxCapsuleShapeDesc& kCapsuleDesc) const
{
    NIASSERT(m_eType == NX_SHAPE_CAPSULE);

    ToShapeDesc(kCapsuleDesc);
    
    kCapsuleDesc.radius = m_fVal1;
    kCapsuleDesc.height = m_fVal2;
    kCapsuleDesc.flags = m_uiCapsuleFlags;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::FromCapsuleDesc(const NxCapsuleShapeDesc& kCapsuleDesc)
{
    FromShapeDesc(kCapsuleDesc);

    m_fVal1 = kCapsuleDesc.radius;
    m_fVal2 = kCapsuleDesc.height;
    m_uiCapsuleFlags = kCapsuleDesc.flags;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToBoxDesc(NxBoxShapeDesc& kBoxDesc) const
{
    NIASSERT(m_eType == NX_SHAPE_BOX);

    ToShapeDesc(kBoxDesc);
    
    kBoxDesc.dimensions = m_kPoint1;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::FromBoxDesc(const NxBoxShapeDesc& kBoxDesc)
{
    FromShapeDesc(kBoxDesc);

    m_kPoint1 = kBoxDesc.dimensions;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToConvexDesc(NxConvexShapeDesc& kConvexDesc,
    const bool bKeepData, const bool bCheckPlatform)
{
    NIASSERT(m_eType == NX_SHAPE_CONVEX);

    ToShapeDesc(kConvexDesc);
    
    m_spMeshDesc->ToConvexDesc(kConvexDesc, bKeepData, bCheckPlatform);
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::ToTriMeshDesc(
    NxTriangleMeshShapeDesc& kTriMeshDesc, const bool bKeepData,
    const bool bCheckPlatform)
{
    NIASSERT(m_eType == NX_SHAPE_MESH);
    
    ToShapeDesc(kTriMeshDesc);
    
    m_spMeshDesc->ToTriMeshDesc(kTriMeshDesc, bKeepData, bCheckPlatform);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXShapeDesc);
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::CopyMembers(NiPhysXShapeDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
    
    pkDest->m_eType = m_eType;
    pkDest->m_kLocalPose = m_kLocalPose;
    pkDest->m_uiShapeFlags = m_uiShapeFlags;
    pkDest->m_uiShapeGroup = m_uiShapeGroup;
    pkDest->m_uiMaterialIndex = m_uiMaterialIndex;
    pkDest->m_fDensity = m_fDensity;
    pkDest->m_fMass = m_fMass;
    pkDest->m_fSkinWidth = m_fSkinWidth;
    pkDest->m_uiNonInteractingCompartmentTypes = 
        m_uiNonInteractingCompartmentTypes;

    pkDest->m_auiCollisionBits[0] = m_auiCollisionBits[0];
    pkDest->m_auiCollisionBits[1] = m_auiCollisionBits[1];
    pkDest->m_auiCollisionBits[2] = m_auiCollisionBits[2];
    pkDest->m_auiCollisionBits[3] = m_auiCollisionBits[3];

    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kShapeName = m_kShapeName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kShapeName)
        {
            size_t stQuantity = strlen(m_kShapeName) + 2;
            char* pcNamePlus = NiAlloc(char, stQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, stQuantity, m_kShapeName);
            pcNamePlus[stQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[stQuantity - 1] = 0;
            pkDest->m_kShapeName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }
        

    // Used to encode the individual shapes.
    pkDest->m_fVal1 = m_fVal1;
    pkDest->m_fVal2 = m_fVal2;
    pkDest->m_kPoint1 = m_kPoint1;
    
    pkDest->m_spMeshDesc = m_spMeshDesc;
    
    pkDest->m_pkShape = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXShapeDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;
    
    kStream.RegisterFixedString(m_kShapeName);
        
    if (m_eType == NX_SHAPE_CONVEX || m_eType == NX_SHAPE_MESH)
        m_spMeshDesc->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    NiStreamSaveEnum(kStream, m_eType);
    
    NiPhysXTypes::NxMat34SaveBinary(kStream, m_kLocalPose);

    NiStreamSaveBinary(kStream, m_uiShapeFlags);
    NiStreamSaveBinary(kStream, m_uiShapeGroup);
    NiStreamSaveBinary(kStream, m_uiMaterialIndex);

    // CCD skeletons are not streamable because the class implementation
    // is not known - i.e. once created you can't get at it. Gamebryo does
    // not support saving out CCD skeletons or streaming them in. In the
    // future, as CCD skeletons become better supported in PhysX, Gamebryo
    // will include support.

    NiStreamSaveBinary(kStream, m_fDensity);
    NiStreamSaveBinary(kStream, m_fMass);
    NiStreamSaveBinary(kStream, m_fSkinWidth);
    
    // User data is not streamed - applications should subclass this class
    // to stream such data.
    
    kStream.SaveFixedString(m_kShapeName);
    NiStreamSaveBinary(kStream, m_uiNonInteractingCompartmentTypes);
    
    NiStreamSaveBinary(kStream, m_auiCollisionBits[0]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[1]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[2]);
    NiStreamSaveBinary(kStream, m_auiCollisionBits[3]);

    switch (m_eType)
    {
        case NX_SHAPE_PLANE:
            NiStreamSaveBinary(kStream, m_fVal1);
            NiPhysXTypes::NxVec3SaveBinary(kStream, m_kPoint1);
            break;

        case NX_SHAPE_SPHERE:
            NiStreamSaveBinary(kStream, m_fVal1);
            break;

        case NX_SHAPE_CAPSULE:
            NiStreamSaveBinary(kStream, m_fVal1);
            NiStreamSaveBinary(kStream, m_fVal2);
            NiStreamSaveBinary(kStream, m_uiCapsuleFlags);
            break;

        case NX_SHAPE_BOX:
            NiPhysXTypes::NxVec3SaveBinary(kStream, m_kPoint1);
            break;
        
        case NX_SHAPE_CONVEX:
        case NX_SHAPE_MESH: {
            kStream.SaveLinkID(m_spMeshDesc);
            } break;
            
        default:;
    }  
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXShapeDesc);
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadEnum(kStream, m_eType);

    NiPhysXTypes::NxMat34LoadBinary(kStream, m_kLocalPose);

    NiStreamLoadBinary(kStream, m_uiShapeFlags);
    NiStreamLoadBinary(kStream, m_uiShapeGroup);
    NiStreamLoadBinary(kStream, m_uiMaterialIndex);

    NiStreamLoadBinary(kStream, m_fDensity);
    NiStreamLoadBinary(kStream, m_fMass);
    NiStreamLoadBinary(kStream, m_fSkinWidth);
    
    kStream.LoadFixedString(m_kShapeName);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, m_uiNonInteractingCompartmentTypes);
    }
    
    NiStreamLoadBinary(kStream, m_auiCollisionBits[0]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[1]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[2]);
    NiStreamLoadBinary(kStream, m_auiCollisionBits[3]);

    switch (m_eType)
    {
        case NX_SHAPE_PLANE:
            NiStreamLoadBinary(kStream, m_fVal1);
            NiPhysXTypes::NxVec3LoadBinary(kStream, m_kPoint1);
            break;

        case NX_SHAPE_SPHERE:
            NiStreamLoadBinary(kStream, m_fVal1);
            break;

        case NX_SHAPE_CAPSULE:
            NiStreamLoadBinary(kStream, m_fVal1);
            NiStreamLoadBinary(kStream, m_fVal2);
            NiStreamLoadBinary(kStream, m_uiCapsuleFlags);
            break;

        case NX_SHAPE_BOX:
            NiPhysXTypes::NxVec3LoadBinary(kStream, m_kPoint1);
            break;
            
        case NX_SHAPE_CONVEX:
        case NX_SHAPE_MESH: {
            m_spMeshDesc = (NiPhysXMeshDesc*)kStream.ResolveLinkID();
            } break;
        
        default:;
    }  
}
//---------------------------------------------------------------------------
void NiPhysXShapeDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXShapeDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXShapeDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXShapeDesc* pkNewObject = (NiPhysXShapeDesc*)pkObject;

    NiMatrix3 kNewM;
    NiPoint3 kNewP;
    NiMatrix3 kThisM;
    NiPoint3 kThisP;
    
    NiPhysXTypes::NxMat34ToNiTransform(pkNewObject->m_kLocalPose,
        kNewM, kNewP);
    NiPhysXTypes::NxMat34ToNiTransform(m_kLocalPose,
        kThisM, kThisP);

    if (pkNewObject->m_eType != m_eType ||
        pkNewObject->m_kShapeName != m_kShapeName ||
        kNewM != kThisM || kNewP != kThisP ||
        pkNewObject->m_uiShapeFlags != m_uiShapeFlags  ||
        pkNewObject->m_uiShapeGroup != m_uiShapeGroup ||
        pkNewObject->m_uiMaterialIndex != m_uiMaterialIndex ||
        pkNewObject->m_fDensity != m_fDensity ||
        pkNewObject->m_fMass != m_fMass ||
        pkNewObject->m_fSkinWidth != m_fSkinWidth ||
        pkNewObject->m_uiNonInteractingCompartmentTypes 
        != m_uiNonInteractingCompartmentTypes)
    {
        return false;
    }

    switch (m_eType)
    {
        case NX_SHAPE_PLANE:
            if (m_fVal1 != pkNewObject->m_fVal1 ||
                m_kPoint1 != pkNewObject->m_kPoint1)
                return false;
            break;

        case NX_SHAPE_SPHERE:
            if (m_fVal1 != pkNewObject->m_fVal1)
                return false;
            break;

        case NX_SHAPE_CAPSULE:
            if (m_fVal1 != pkNewObject->m_fVal1 ||
                m_fVal2 != pkNewObject->m_fVal2 ||
                m_uiCapsuleFlags != pkNewObject->m_uiCapsuleFlags)
                return false;
            break;

        case NX_SHAPE_BOX:
            if (m_kPoint1 != pkNewObject->m_kPoint1)
                return false;
            break;
        
        case NX_SHAPE_CONVEX:
        case NX_SHAPE_MESH:
            if (!m_spMeshDesc->IsEqual(pkNewObject->m_spMeshDesc))
                return false;
            break;
        
        default:;
    }
 
    return true;
}
//---------------------------------------------------------------------------

