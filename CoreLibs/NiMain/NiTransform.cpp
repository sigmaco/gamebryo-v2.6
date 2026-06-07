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
#include "NiMainPCH.h"

#include "NiTransform.h"
#include "NiQuaternion.h"

//---------------------------------------------------------------------------
void NiTransform::MakeIdentity()
{
    m_Rotate.MakeIdentity();
    m_Translate = NiPoint3::ZERO;
    m_fScale = 1.0f;
}

//---------------------------------------------------------------------------
bool NiTransform::operator!=(const NiTransform &xform) const
{
    return 
        m_Rotate != xform.m_Rotate ||
        m_Translate != xform.m_Translate ||
        m_fScale != xform.m_fScale;
}

//---------------------------------------------------------------------------
void NiTransform::Invert(NiTransform& kDest) const
{
    kDest.m_Rotate = m_Rotate.Transpose();
    kDest.m_fScale = 1.0f / m_fScale;
    kDest.m_Translate = kDest.m_fScale * (kDest.m_Rotate * -m_Translate);
}
//---------------------------------------------------------------------------
void NiTransform::Interpolate(
    float fAmount, 
    const NiTransform& kStart,
    const NiTransform& kFinish,
    NiTransform& kOutput)
{
    if (fAmount <= 0.f)
    {
        kOutput = kStart;
    }
    else if (fAmount >= 1.f)
    {
        kOutput = kFinish;
    }
    else
    {
        kOutput.m_fScale = 
            NiLerp(fAmount, 
            kStart.m_fScale, 
            kFinish.m_fScale);

        kOutput.m_Translate = 
            NiLerp(fAmount, 
            kStart.m_Translate, 
            kFinish.m_Translate);

        NiQuaternion kStartQuat;
        kStartQuat.FromRotation(kStart.m_Rotate);
        NiQuaternion kFinishQuat;
        kFinishQuat.FromRotation(kFinish.m_Rotate);
        NiQuaternion kResultQuat(NiQuaternion::Slerp(
            fAmount, 
            kStartQuat,
            kFinishQuat));
        kResultQuat.ToRotation(kOutput.m_Rotate);
    }
}

#ifndef __SPU__
//---------------------------------------------------------------------------
void NiTransform::LoadBinary(NiStream& stream)
{
    m_Rotate.LoadBinary(stream);
    m_Translate.LoadBinary(stream);
    NiStreamLoadBinary(stream, m_fScale);
}

//---------------------------------------------------------------------------
void NiTransform::SaveBinary(NiStream& stream)
{
    m_Rotate.SaveBinary(stream);
    m_Translate.SaveBinary(stream);
    NiStreamSaveBinary(stream, m_fScale);
}
//---------------------------------------------------------------------------
#endif
