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

#include "NiPhysX.h"

NiImplementRTTI(NiPhysXAccumRigidBodyDest, NiPhysXRigidBodyDest);

//---------------------------------------------------------------------------
NiPhysXAccumRigidBodyDest::NiPhysXAccumRigidBodyDest()
{
    m_uFlags = ACCUM_X_FRONT;
}
//---------------------------------------------------------------------------
NiPhysXAccumRigidBodyDest::NiPhysXAccumRigidBodyDest(
    NxActor* pkActor, NxActor* pkActorParent) :
    NiPhysXRigidBodyDest(pkActor, pkActorParent)
{
    m_uFlags = ACCUM_X_FRONT;
}
//---------------------------------------------------------------------------
NiPhysXAccumRigidBodyDest::~NiPhysXAccumRigidBodyDest()
{
}
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::GetAccumTransforms(float fTime,
    NiQuaternion& kAccumRotation, NiPoint3& kAccumTranslation,
    NiQuaternion& kNonAccumRotation, NiPoint3& kNonAccumTranslation,
    const NiTransform& kRootTransform)
{
    // Use NiPhysXRigidBodyDest::GetTransforms to determine the overall
    // transformation that we need to effect.
    NiTransform kLocalXform;
    GetTransforms(
        fTime, kLocalXform.m_Rotate, kLocalXform.m_Translate, kRootTransform);
    kLocalXform.m_fScale = 1.0f;

    // Now split that transformation into accumulated and non-accumulated
    // pieces.
    // Short cut for one case
    if (GetBit(ACCUM_X_TRANS) && GetBit(ACCUM_Y_TRANS) &&
        GetBit(ACCUM_Z_TRANS) && GetBit(ACCUM_X_ROT) &&
        GetBit(ACCUM_Y_ROT) && GetBit(ACCUM_Z_ROT) &&
        GetBit(ACCUM_X_FRONT) && !GetBit(ACCUM_NEG_FRONT))
    {
        // Just set on the accum node
        kAccumRotation.FromRotation(kLocalXform.m_Rotate);
        kAccumTranslation = kLocalXform.m_Translate;
        
        kNonAccumRotation = NiQuaternion::IDENTITY;
        kNonAccumTranslation = NiPoint3::ZERO;
    }
    else
    {
        // Compute accum axis quaternions.
        NiQuaternion kQuatAccumAxis, kQuatAccumAxisInv;
        if (GetBit(ACCUM_X_FRONT) && GetBit(ACCUM_NEG_FRONT))
        {
            // rot about z by 180 deg (NiMatrix3 sense of rotation)
            kQuatAccumAxis.SetValues(0.0f, 0.0f, 0.0f, -1.0f);
            kQuatAccumAxisInv.SetValues(0.0f, 0.0f, 0.0f, 1.0f);
        }
        else if (GetBit(ACCUM_Y_FRONT))
        {
            float f1_sqrt2 = 1.0f / NiSqrt(2.0f);
            if (GetBit(ACCUM_NEG_FRONT))
            {
                // rot about z by -90 deg (NiMatrix3 sense of rotation)
                kQuatAccumAxis.SetValues(f1_sqrt2, 0.0f, 0.0f, f1_sqrt2);
                kQuatAccumAxisInv.SetValues(f1_sqrt2, 0.0f, 0.0f, -f1_sqrt2);
            }
            else
            {
                // rot about z by 90 deg (NiMatrix3 sense of rotation)
                kQuatAccumAxis.SetValues(f1_sqrt2, 0.0f, 0.0f, -f1_sqrt2);
                kQuatAccumAxisInv.SetValues(f1_sqrt2, 0.0f, 0.0f, f1_sqrt2);
            }
        }
        else
        {
            kQuatAccumAxis = NiQuaternion::IDENTITY;
            kQuatAccumAxisInv = NiQuaternion::IDENTITY;
        }
        
        // Factor rotation
        NiMatrix3 kAccumRot, kNonAccumRot;
        FactorRot(kLocalXform.m_Rotate, kAccumRot, kNonAccumRot);
        NiQuaternion kNonAccumQuat;
        kNonAccumQuat.FromRotation(kNonAccumRot);

        kNonAccumRotation = kQuatAccumAxisInv * kNonAccumQuat;

        NiQuaternion kAccumQuat;
        kAccumQuat.FromRotation(kAccumRot);
        kAccumRotation = kAccumQuat * kQuatAccumAxis;
        
        // Factor translation. We do not support scaling animations!
        NiPoint3 kAccumTrans, kNonAccumTrans;
        FactorTrans(kLocalXform.m_Translate, kAccumTrans, kNonAccumTrans);

        kAccumRotation.ToRotation(kAccumRot);
        NiMatrix3 kAccumRootInv = kAccumRot.Transpose();
        kNonAccumTranslation = kAccumRootInv * kNonAccumTrans;
        kAccumTranslation = kAccumTrans;
    }
}
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::FactorRot(const NiMatrix3& kFullRot,
    NiMatrix3& kAccumRot, NiMatrix3& kNonAccumRot)
{
    float fXAngle, fYAngle, fZAngle;
    if (GetBit(ACCUM_X_ROT) && GetBit(ACCUM_Y_ROT) && GetBit(ACCUM_Z_ROT))
    {
        kAccumRot = kFullRot;
        kNonAccumRot = NiMatrix3::IDENTITY;
    }
    else if (GetBit(ACCUM_X_ROT))
    {
        kFullRot.ToEulerAnglesXYZ(fXAngle, fYAngle, fZAngle);
        kAccumRot.FromEulerAnglesXYZ(fXAngle, 0.0f, 0.0f);
        kNonAccumRot.FromEulerAnglesXYZ(0.0f, fYAngle, fZAngle);
    }
    else if (GetBit(ACCUM_Y_ROT))
    {
        kFullRot.ToEulerAnglesYXZ(fYAngle, fXAngle, fZAngle);
        kAccumRot.FromEulerAnglesYXZ(fYAngle, 0.0f, 0.0f);
        kNonAccumRot.FromEulerAnglesYXZ(0.0f, fXAngle, fZAngle);
    }
    else if (GetBit(ACCUM_Z_ROT))
    {
        kFullRot.ToEulerAnglesZXY(fZAngle, fXAngle, fYAngle);
        kAccumRot.FromEulerAnglesZXY(fZAngle, 0.0f, 0.0f);
        kNonAccumRot.FromEulerAnglesZXY(0.0f, fXAngle, fYAngle);
    }
    else
    {
        kAccumRot = NiMatrix3::IDENTITY;
        kNonAccumRot = kFullRot;
    }
}
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::FactorTrans(const NiPoint3& kFullTrans,
    NiPoint3& kAccumTrans, NiPoint3& kNonAccumTrans)
{
    kAccumTrans = NiPoint3::ZERO;
    kNonAccumTrans = kFullTrans;
    if (GetBit(ACCUM_X_TRANS))
    {
        kAccumTrans.x = kFullTrans.x;
        kNonAccumTrans.x = 0.0f;
    }
    if (GetBit(ACCUM_Y_TRANS))
    {
        kAccumTrans.y = kFullTrans.y;
        kNonAccumTrans.y = 0.0f;
    }
    if (GetBit(ACCUM_Z_TRANS))
    {
        kAccumTrans.z = kFullTrans.z;
        kNonAccumTrans.z = 0.0f;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::CopyMembers(NiPhysXAccumRigidBodyDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXRigidBodyDest::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXAccumRigidBodyDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXRigidBodyDest::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::SaveBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::SaveBinary(kStream);
    
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
NiObject* NiPhysXAccumRigidBodyDest::CreateObject(const char**,
    NiUInt32)
{
    NIASSERT(false &&
        "CreateClass should never be called on NiPhysXAccumRigidBodyDest.");
    return 0;
}
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::LoadBinary(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiPhysXAccumRigidBodyDest::LinkObject(NiStream& kStream)
{
    NiPhysXRigidBodyDest::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXAccumRigidBodyDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXAccumRigidBodyDest, pkObject));
    if(!NiPhysXRigidBodyDest::IsEqual(pkObject))
        return false;

    if (m_uFlags != ((NiPhysXAccumRigidBodyDest*)pkObject)->m_uFlags)
        return false;

    return true;
}
//---------------------------------------------------------------------------
