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

//---------------------------------------------------------------------------
inline void NiPhysXAccumRigidBodyDest::SetAccumTranslation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ)
{
    SetBit(bAccumX, ACCUM_X_TRANS);
    SetBit(bAccumY, ACCUM_Y_TRANS);
    SetBit(bAccumZ, ACCUM_Z_TRANS);
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumRigidBodyDest::GetAccumTranslation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const
{
    bAccumX = GetBit(ACCUM_X_TRANS);
    bAccumY = GetBit(ACCUM_Y_TRANS);
    bAccumZ = GetBit(ACCUM_Z_TRANS);
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumRigidBodyDest::SetAccumRotation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ)
{
    SetBit(bAccumX, ACCUM_X_ROT);
    SetBit(bAccumY, ACCUM_Y_ROT);
    SetBit(bAccumZ, ACCUM_Z_ROT);
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumRigidBodyDest::GetAccumRotation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const
{
    bAccumX = GetBit(ACCUM_X_ROT);
    bAccumY = GetBit(ACCUM_Y_ROT);
    bAccumZ = GetBit(ACCUM_Z_ROT);
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumRigidBodyDest::SetAccumFrontFacing(
    const unsigned char ucAxis, const bool bIsNegative)
{
    switch (ucAxis)
    {
        case 0:
            SetBit(true, ACCUM_X_FRONT);
            SetBit(false, ACCUM_Y_FRONT);
            SetBit(false, ACCUM_Z_FRONT);
            break;
        case 1:
            SetBit(false, ACCUM_X_FRONT);
            SetBit(true, ACCUM_Y_FRONT);
            SetBit(false, ACCUM_Z_FRONT);
            break;
        case 2:
            SetBit(false, ACCUM_X_FRONT);
            SetBit(false, ACCUM_Y_FRONT);
            SetBit(true, ACCUM_Z_FRONT);
            break;
    }
    SetBit(bIsNegative, ACCUM_NEG_FRONT);
}
//---------------------------------------------------------------------------
inline void NiPhysXAccumRigidBodyDest::GetAccumFrontFacing(
    unsigned char& ucAxis, bool& bIsNegative) const
{
    if (GetBit(ACCUM_X_FRONT))
    {
        ucAxis = 0;
    }
    else if (GetBit(ACCUM_Y_FRONT))
    {
        ucAxis = 1;
    }
    else
    {
        ucAxis = 2;
    }
    bIsNegative = GetBit(ACCUM_NEG_FRONT);
}
//---------------------------------------------------------------------------
