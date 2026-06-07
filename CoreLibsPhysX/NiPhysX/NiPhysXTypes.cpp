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
#include "NiPhysXPCH.h"

#include "NiPhysXTypes.h"

const NxVec3 NiPhysXTypes::NXVEC3_ZERO(0.0f, 0.0f, 0.0f);
const NxMat33 NiPhysXTypes::NXMAT33_ID(
    NxVec3(1.f, 0.f, 0.f), NxVec3(0.f, 1.f, 0.f), NxVec3(0.f, 0.f, 1.f));
const NxMat34 NiPhysXTypes::NXMAT34_ID(
    NiPhysXTypes::NXMAT33_ID, NiPhysXTypes::NXVEC3_ZERO);

const NxPhysicsSDKDesc NiPhysXTypes::NXPHYSICSSDKDESC_DEFAULT;

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
void NiPhysXTypes::NxVec3SaveBinary(NiStream& kStream, const NxVec3& kNxVec)
{
    NiStreamSaveBinary(kStream, kNxVec.x);
    NiStreamSaveBinary(kStream, kNxVec.y);
    NiStreamSaveBinary(kStream, kNxVec.z);
}
//---------------------------------------------------------------------------
void NiPhysXTypes::NxVec3LoadBinary(NiStream& kStream, NxVec3& kNxVec)
{
    NiStreamLoadBinary(kStream, kNxVec.x);
    NiStreamLoadBinary(kStream, kNxVec.y);
    NiStreamLoadBinary(kStream, kNxVec.z);
}
//---------------------------------------------------------------------------
void NiPhysXTypes::NxMat34SaveBinary(NiStream& kStream, const NxMat34& kNxMat)
{
    NiStreamSaveBinary(kStream, kNxMat.M(0, 0));
    NiStreamSaveBinary(kStream, kNxMat.M(0, 1));
    NiStreamSaveBinary(kStream, kNxMat.M(0, 2));
    NiStreamSaveBinary(kStream, kNxMat.M(1, 0));
    NiStreamSaveBinary(kStream, kNxMat.M(1, 1));
    NiStreamSaveBinary(kStream, kNxMat.M(1, 2));
    NiStreamSaveBinary(kStream, kNxMat.M(2, 0));
    NiStreamSaveBinary(kStream, kNxMat.M(2, 1));
    NiStreamSaveBinary(kStream, kNxMat.M(2, 2));
    
    NxVec3SaveBinary(kStream, kNxMat.t);
}
//---------------------------------------------------------------------------
void NiPhysXTypes::NxMat34LoadBinary(NiStream& kStream, NxMat34& kNxMat)
{
    NiStreamLoadBinary(kStream, kNxMat.M(0, 0));
    NiStreamLoadBinary(kStream, kNxMat.M(0, 1));
    NiStreamLoadBinary(kStream, kNxMat.M(0, 2));
    NiStreamLoadBinary(kStream, kNxMat.M(1, 0));
    NiStreamLoadBinary(kStream, kNxMat.M(1, 1));
    NiStreamLoadBinary(kStream, kNxMat.M(1, 2));
    NiStreamLoadBinary(kStream, kNxMat.M(2, 0));
    NiStreamLoadBinary(kStream, kNxMat.M(2, 1));
    NiStreamLoadBinary(kStream, kNxMat.M(2, 2));
    
    NxVec3LoadBinary(kStream, kNxMat.t);
}
//---------------------------------------------------------------------------
void NiPhysXTypes::NxQuatSaveBinary(NiStream& kStream, const NxQuat& kNxQuat)
{
    NiStreamSaveBinary(kStream, kNxQuat.w);
    NiStreamSaveBinary(kStream, kNxQuat.x);
    NiStreamSaveBinary(kStream, kNxQuat.y);
    NiStreamSaveBinary(kStream, kNxQuat.z);
}
//---------------------------------------------------------------------------
void NiPhysXTypes::NxQuatLoadBinary(NiStream& kStream, NxQuat& kNxQuat)
{
    NiStreamLoadBinary(kStream, kNxQuat.w);
    NiStreamLoadBinary(kStream, kNxQuat.x);
    NiStreamLoadBinary(kStream, kNxQuat.y);
    NiStreamLoadBinary(kStream, kNxQuat.z);
}
//---------------------------------------------------------------------------
