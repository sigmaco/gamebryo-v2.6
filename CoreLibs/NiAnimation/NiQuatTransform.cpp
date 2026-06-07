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
#include "NiAnimationPCH.h"

#include "NiQuatTransform.h"
#include <NiStream.h>
#include <NiBool.h>

const NiPoint3 NiQuatTransform::INVALID_TRANSLATE = NiPoint3(-FLT_MAX,
    -FLT_MAX, -FLT_MAX);
const NiQuaternion NiQuatTransform::INVALID_ROTATE = NiQuaternion(
    -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
const float NiQuatTransform::INVALID_SCALE = -FLT_MAX;

//---------------------------------------------------------------------------
void NiQuatTransform::Invert(NiQuatTransform& kDest) const
{
    if (!IsScaleValid())
    {
        kDest.SetScaleValid(false);
    }
    else
    {
        kDest.SetScale(1.0f / m_fScale);
    }

    if (!IsRotateValid())
    {
        kDest.SetRotateValid(false);
    }
    else
    {
        kDest.SetRotate(NiQuaternion::UnitInverse(m_kRotate));
    }

    if (!IsTranslateValid())
    {
        kDest.SetTranslateValid(false);
    }
    else
    {
        kDest.SetTranslate(-m_kTranslate);
    }
}
//---------------------------------------------------------------------------
void NiQuatTransform::HierInvert(NiQuatTransform& kDest) const
{
    float fInvScale = 1.0f;
    if (!IsScaleValid())
    {
        kDest.SetScaleValid(false);
    }
    else
    {
        kDest.SetScale(1.0f / m_fScale);
        fInvScale = kDest.GetScale();
    }

    NiMatrix3 kInvRotate(NiMatrix3::IDENTITY);
    if (!IsRotateValid())
    {
        kDest.SetRotateValid(false);
    }
    else
    {
        kDest.SetRotate(NiQuaternion::UnitInverse(m_kRotate));
        kDest.GetRotate().ToRotation(kInvRotate);
    }

    if (!IsTranslateValid())
    {
        kDest.SetTranslateValid(false);
    }
    else
    {
        kDest.SetTranslate(fInvScale * (kInvRotate * -m_kTranslate));
    }
}
//---------------------------------------------------------------------------
NiQuatTransform NiQuatTransform::HierApply(
    const NiQuatTransform& kTransform) const
{
    NiQuatTransform kDest;

    bool bIsScaleValid = IsScaleValid();
    if (!bIsScaleValid || !kTransform.IsScaleValid())
    {
        kDest.SetScaleValid(false);
    }
    else
    {
        kDest.SetScale(m_fScale * kTransform.GetScale());
    }

    bool bIsRotateValid = IsRotateValid();
    if (!bIsRotateValid || !kTransform.IsRotateValid())
    {
        kDest.SetRotateValid(false);
    }
    else
    {
        NiQuaternion kTempRot = m_kRotate * kTransform.GetRotate();
        kTempRot.Normalize();
        kDest.SetRotate(kTempRot);
    }

    if (!IsTranslateValid() || !kTransform.IsTranslateValid())
    {
        kDest.SetTranslateValid(false);
    }
    else
    {
        float fScale = 1.0f;
        if (bIsScaleValid)
        {
            fScale = m_fScale;
        }

        NiMatrix3 kRotate(NiMatrix3::IDENTITY);
        if (bIsRotateValid)
        {
            m_kRotate.ToRotation(kRotate);
        }

        kDest.SetTranslate(m_kTranslate + fScale * (kRotate *
            kTransform.GetTranslate()));
    }

    return kDest;
}
//---------------------------------------------------------------------------
NiQuatTransform NiQuatTransform::operator*(const NiQuatTransform& kTransform)
    const
{
    NiQuatTransform kDest;

    float fScale = 1.0f;
    if (!IsScaleValid() || !kTransform.IsScaleValid())
    {
        kDest.SetScaleValid(false);
    }
    else
    {
        kDest.SetScale(m_fScale * kTransform.GetScale());
        fScale = kDest.GetScale();
    }

    if (!IsRotateValid() || !kTransform.IsRotateValid())
    {
        kDest.SetRotateValid(false);
    }
    else
    {
        NiQuaternion kTempRot = m_kRotate * kTransform.GetRotate();
        kTempRot.Normalize();
        kDest.SetRotate(kTempRot);
    }

    if (!IsTranslateValid() || !kTransform.IsTranslateValid())
    {
        kDest.SetTranslateValid(false);
    }
    else
    {
        kDest.SetTranslate(m_kTranslate + kTransform.GetTranslate());
    }

    return kDest;
}
//---------------------------------------------------------------------------
void NiQuatTransform::LoadBinary(NiStream& kStream)
{
    m_kTranslate.LoadBinary(kStream);
    m_kRotate.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fScale);
}
//---------------------------------------------------------------------------
void NiQuatTransform::SaveBinary(NiStream& kStream)
{
    m_kTranslate.SaveBinary(kStream);
    m_kRotate.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fScale);
}
//---------------------------------------------------------------------------
void NiQuatTransform::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString("NiQuatTransform"));

    pkStrings->Add(m_kTranslate.GetViewerString("m_kTranslate"));
    pkStrings->Add(m_kRotate.GetViewerString("m_kRotate"));
    pkStrings->Add(NiGetViewerString("m_fScale", m_fScale));
}
//---------------------------------------------------------------------------
