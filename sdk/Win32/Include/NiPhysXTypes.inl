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

// NiPhysXTypes inline functions

//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxVec3ToNiPoint3(const NxVec3& kNxVSrc,
    NiPoint3& kNiPDest)
{
    kNiPDest.x = kNxVSrc.x;
    kNiPDest.y = kNxVSrc.y;
    kNiPDest.z = kNxVSrc.z;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NiPoint3ToNxVec3(const NiPoint3& kNiPSrc,
    NxVec3& kNxVDest)
{
    kNxVDest.x = kNiPSrc.x;
    kNxVDest.y = kNiPSrc.y;
    kNxVDest.z = kNiPSrc.z;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxQuatToNiQuaternion(const NxQuat& kNxQSrc,
    NiQuaternion& kNiQDest)
{
    kNiQDest.SetValues(kNxQSrc.w, kNxQSrc.x, kNxQSrc.y, kNxQSrc.z);
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NiQuaternionToNxQuat(const NiQuaternion& kNiQSrc,
    NxQuat& kNxQDest)
{
    kNiQSrc.GetValues(kNxQDest.w, kNxQDest.x, kNxQDest.y, kNxQDest.z);
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxMat33ToNiMatrix3(const NxMat33& kNxMSrc,
    NiMatrix3& kNiMDest)
{
    kNiMDest.SetEntry(0, 0, kNxMSrc(0, 0));
    kNiMDest.SetEntry(0, 1, kNxMSrc(0, 1));
    kNiMDest.SetEntry(0, 2, kNxMSrc(0, 2));
    kNiMDest.SetEntry(1, 0, kNxMSrc(1, 0));
    kNiMDest.SetEntry(1, 1, kNxMSrc(1, 1));
    kNiMDest.SetEntry(1, 2, kNxMSrc(1, 2));
    kNiMDest.SetEntry(2, 0, kNxMSrc(2, 0));
    kNiMDest.SetEntry(2, 1, kNxMSrc(2, 1));
    kNiMDest.SetEntry(2, 2, kNxMSrc(2, 2));
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NiMatrix3ToNxMat33(const NiMatrix3& kNiMSrc,
    NxMat33& kNxMDest)
{
    kNxMDest(0, 0) = kNiMSrc.GetEntry(0, 0);
    kNxMDest(0, 1) = kNiMSrc.GetEntry(0, 1);
    kNxMDest(0, 2) = kNiMSrc.GetEntry(0, 2);
    kNxMDest(1, 0) = kNiMSrc.GetEntry(1, 0);
    kNxMDest(1, 1) = kNiMSrc.GetEntry(1, 1);
    kNxMDest(1, 2) = kNiMSrc.GetEntry(1, 2);
    kNxMDest(2, 0) = kNiMSrc.GetEntry(2, 0);
    kNxMDest(2, 1) = kNiMSrc.GetEntry(2, 1);
    kNxMDest(2, 2) = kNiMSrc.GetEntry(2, 2);
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxMat33ToNiQuaternion(const NxMat33& kNxMSrc,
    NiQuaternion& kNiQDest)
{
    NxQuat kNxQ;

#ifdef _PS3
    kNxQ.setWXYZ(0,0,0,0);
#endif

    kNxMSrc.toQuat(kNxQ);
    NxQuatToNiQuaternion(kNxQ, kNiQDest);
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NiQuaternionToNxMat33(const NiQuaternion& kNiQSrc,
    NxMat33& kNxMDest)
{
    NxQuat kNxQ;
    NiQuaternionToNxQuat(kNiQSrc, kNxQ);
    kNxMDest.fromQuat(kNxQ);
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxMat34ToNiTransform(const NxMat34& kNxMSrc,
    NiMatrix3& kNiMDest, NiPoint3& kNiPDest)
{
    kNiMDest.SetEntry(0, 0, kNxMSrc.M(0, 0));
    kNiMDest.SetEntry(0, 1, kNxMSrc.M(0, 1));
    kNiMDest.SetEntry(0, 2, kNxMSrc.M(0, 2));
    kNiMDest.SetEntry(1, 0, kNxMSrc.M(1, 0));
    kNiMDest.SetEntry(1, 1, kNxMSrc.M(1, 1));
    kNiMDest.SetEntry(1, 2, kNxMSrc.M(1, 2));
    kNiMDest.SetEntry(2, 0, kNxMSrc.M(2, 0));
    kNiMDest.SetEntry(2, 1, kNxMSrc.M(2, 1));
    kNiMDest.SetEntry(2, 2, kNxMSrc.M(2, 2));

    kNiPDest.x = kNxMSrc.t.x;
    kNiPDest.y = kNxMSrc.t.y;
    kNiPDest.z = kNxMSrc.t.z;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NiTransformToNxMat34(const NiMatrix3& kNiMSrc,
    const NiPoint3& kNiPSrc, NxMat34& kNxMDest)
{
    kNxMDest.M(0, 0) = kNiMSrc.GetEntry(0, 0);
    kNxMDest.M(0, 1) = kNiMSrc.GetEntry(0, 1);
    kNxMDest.M(0, 2) = kNiMSrc.GetEntry(0, 2);
    kNxMDest.M(1, 0) = kNiMSrc.GetEntry(1, 0);
    kNxMDest.M(1, 1) = kNiMSrc.GetEntry(1, 1);
    kNxMDest.M(1, 2) = kNiMSrc.GetEntry(1, 2);
    kNxMDest.M(2, 0) = kNiMSrc.GetEntry(2, 0);
    kNxMDest.M(2, 1) = kNiMSrc.GetEntry(2, 1);
    kNxMDest.M(2, 2) = kNiMSrc.GetEntry(2, 2);

    kNxMDest.t.x = kNiPSrc.x;
    kNxMDest.t.y = kNiPSrc.y;
    kNxMDest.t.z = kNiPSrc.z;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxMat34ToNiQuatTransform(const NxMat34& kNxMSrc,
    NiQuaternion& kNiQDest, NiPoint3& kNiPDest)
{
#ifdef _PS3
    NxQuat kNxQ;
    kNxQ.setWXYZ(0,0,0,0);
    kNxQ = NxQuat(kNxMSrc.M);
#else
    NxQuat kNxQ(kNxMSrc.M);
#endif
    NxQuatToNiQuaternion(kNxQ, kNiQDest);

    kNiPDest.x = kNxMSrc.t.x;
    kNiPDest.y = kNxMSrc.t.y;
    kNiPDest.z = kNxMSrc.t.z;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NiQuatTransformToNxMat34(const NiQuaternion& kNiQSrc,
    const NiPoint3& kNiPSrc, NxMat34& kNxMDest)
{
    NxQuat kNxQ;
    NiQuaternionToNxQuat(kNiQSrc, kNxQ);
    kNxMDest.M.fromQuat(kNxQ);

    kNxMDest.t.x = kNiPSrc.x;
    kNxMDest.t.y = kNiPSrc.y;
    kNxMDest.t.z = kNiPSrc.z;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxColorToNiColorA(const NxU32 kNxSrc,
    NiColorA& kNiDest)
{
    kNiDest.r = NxF32((kNxSrc>>16)&0xff)/255.0f;
    kNiDest.g = NxF32((kNxSrc>>8)&0xff)/255.0f;
    kNiDest.b = NxF32((kNxSrc)&0xff)/255.0f;
    kNiDest.a = 1.0f;
}
//---------------------------------------------------------------------------
inline void NiPhysXTypes::NxColorToNiRGBA(const NxU32 kNxSrc, 
    NiRGBA& kNiDest)
{
    kNiDest.r() = (NiUInt8)((kNxSrc>>16)&0xff);
    kNiDest.g() = (NiUInt8)((kNxSrc>>8)&0xff);
    kNiDest.b() = (NiUInt8)((kNxSrc)&0xff);
    kNiDest.a() = 0xff;
}
//---------------------------------------------------------------------------