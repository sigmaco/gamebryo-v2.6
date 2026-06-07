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

#include "NiLookAtInterpolator.h"
#include "NiQuatTransform.h"
#include <NiNode.h>
#include <NiCloningProcess.h>

NiImplementRTTI(NiLookAtInterpolator, NiInterpolator);

//---------------------------------------------------------------------------
NiLookAtInterpolator::NiLookAtInterpolator() : m_uFlags(0), m_pkLookAt(NULL)
{
    SetFlip(false);
    SetAxis(X);
    m_aspInterpolators[0] = NULL;
    m_aspInterpolators[1] = NULL;
    m_aspInterpolators[2] = NULL;
}
//---------------------------------------------------------------------------
NiLookAtInterpolator::NiLookAtInterpolator(NiAVObject* pkLookAt,
    Axis eAxis, bool bFlip) : m_uFlags(0), m_pkLookAt(pkLookAt)
{
    SetFlip(bFlip);
    SetAxis(eAxis);
    m_aspInterpolators[0] = NULL;
    m_aspInterpolators[1] = NULL;
    m_aspInterpolators[2] = NULL;
}
//---------------------------------------------------------------------------
NiLookAtInterpolator::~NiLookAtInterpolator()
{
    SetLookAtName(NULL);
    m_pkLookAt = NULL;
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::IsTransformValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
void NiLookAtInterpolator::Collapse()
{
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui] != NULL)
        {
            m_aspInterpolators[ui]->Collapse();
        }
    }
}
//---------------------------------------------------------------------------
void NiLookAtInterpolator::GetActiveTimeRange(float& 
    fBeginKeyTime, float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;
    
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui] != NULL)
        {
            float fBegin, fEnd;
            m_aspInterpolators[ui]->GetActiveTimeRange(fBegin, fEnd);
            if (fBegin != fEnd)
            {
                if (fBegin < fBeginKeyTime)
                {
                    fBeginKeyTime = fBegin;
                }
                if (fEnd > fEndKeyTime)
                {
                    fEndKeyTime = fEnd;
                }
            }
        }
    }

    if (fBeginKeyTime == NI_INFINITY && fEndKeyTime == -NI_INFINITY)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiLookAtInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui] != NULL)
        {
            m_aspInterpolators[ui]->GuaranteeTimeRange(fStartTime, 
                fEndTime);
        }
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiLookAtInterpolator::GetSequenceInterpolator(
    float fStartTime, float fEndTime)
{
    NiLookAtInterpolator* pkSeqInterp = (NiLookAtInterpolator*)
        NiInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    NIASSERT(pkSeqInterp);
    NIVERIFY(pkSeqInterp->SetUpDependencies());

    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui] != NULL)
        {
            pkSeqInterp->m_aspInterpolators[ui] = 
                m_aspInterpolators[ui]->GetSequenceInterpolator(fStartTime,
                fEndTime);
        }
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::Update(float fTime, 
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
{
    if (!m_pkLookAt)
    {
        kValue.MakeInvalid();
        m_kTransformValue.MakeInvalid();
        return false;
    }

    NIASSERT(pkInterpTarget && NiIsKindOf(NiAVObject, pkInterpTarget));

    // m_pkLookAt is assumed to have been updated before the
    // LookAtInterpolator is evaluated. If this assumption is violated then
    // the LookAtInterpolator will appear to be a frame behind in the
    // animation.
    NiPoint3 kTargetWorldPos = m_pkLookAt->GetWorldTranslate();

    // pkTarget has not yet had its world data updated. We compute what the
    // world translation will be in here so that we can set kUp the rotation
    // correctly.
    NiMatrix3 kParentWorldRot;
    NiPoint3 kMyWorldPos;
    NiAVObject* pkTarget = (NiAVObject*)pkInterpTarget;
    NiNode* pkParent = pkTarget->GetParent();

    NiInterpolator* pkTranslateInterp = GetTranslateInterpolator();
    NiPoint3 kLocalTranslate;

    // If necessary, compute the current local space translation
    if (pkTranslateInterp)
    {
        if (!pkTranslateInterp->Update(fTime, pkInterpTarget, kLocalTranslate))
        {
            m_kTransformValue.MakeInvalid();
            return false;
        }
        m_kTransformValue.SetTranslate(kLocalTranslate);
    }
    else
    {
        kLocalTranslate = pkTarget->GetTranslate();
    }

    // Now compute the world position
    if (pkParent)
    {
        kParentWorldRot = pkParent->GetWorldRotate();
        kMyWorldPos = pkParent->GetWorldTranslate() +
            pkParent->GetWorldScale() * (kParentWorldRot *
            kLocalTranslate);
    }
    else
    {
        kParentWorldRot = NiMatrix3::IDENTITY;
        kMyWorldPos = kLocalTranslate;
    }
    
    NiMatrix3 kRot;
    NiPoint3 kAt = kTargetWorldPos - kMyWorldPos;
    if (kAt.SqrLength() < 0.001f)
    {
        // We have zero vector so don't update the transform
        kRot.MakeIdentity();
    }
    else    
    {
        NiPoint3 kUp, kRight;
        float fDot;

        kAt.Unitize();

        if (((kAt.z < 1.001f) && (kAt.z > 0.999f)) ||
            ((kAt.z > -1.001f) && (kAt.z < -0.999f)))
        {
            // kUp & kAt are too close - use the Y axis as an alternate kUp
            kUp = NiPoint3::UNIT_Y;
            fDot = kAt.y;
        }
        else
        {
            kUp = NiPoint3::UNIT_Z;
            fDot = kAt.z;
        }


        // subtract off the component of 'kUp' in the 'kAt' direction
        kUp -= fDot*kAt;
        kUp.Unitize();

        if (!GetFlip())
        {
            kAt = -kAt;
        }

        kRight = kUp.Cross(kAt);

        switch (GetAxis())
        {
            case X:
                kRot.SetCol(0, kAt);
                kRot.SetCol(1, kUp);
                kRot.SetCol(2, -kRight);
                break;
            case Y:
                kRot.SetCol(0, kRight);
                kRot.SetCol(1, kAt);
                kRot.SetCol(2, -kUp);
                break;
            case Z:
                kRot.SetCol(0, kRight);
                kRot.SetCol(1, kUp);
                kRot.SetCol(2, kAt);
                break;
        }

        // kAt this point "kRot" is the desired world rotation. The next
        // line converts it into the model space rotation necessary to
        // result in the desired world space rotation.
        kRot = kParentWorldRot.TransposeTimes(kRot);

        // Re-orient the local space rotation by a roll interpolator
        // if necessary
        NiInterpolator* pkRollInterp = GetRollInterpolator();
        if (pkRollInterp)
        {
            float fRoll;
            if (!pkRollInterp->Update(fTime, pkInterpTarget, fRoll))
            {
                m_kTransformValue.MakeInvalid();
                return false;
            }
            
            NiMatrix3 roll;
            roll.MakeZRotation(-fRoll);
            kRot = kRot*roll;
        }
    }

    // Convert from an NiMatrix to an NiQuaternion
    NiQuaternion kQuatValue;
    kQuatValue.FromRotation(kRot);
    m_kTransformValue.SetRotate(kQuatValue);

    // Compute the local scale, if necessary
    NiInterpolator* pkScaleInterp = GetScaleInterpolator();
    if (pkScaleInterp)
    {
        float fScale;
        if (!pkScaleInterp->Update(fTime, pkInterpTarget, fScale))
        {
            m_kTransformValue.MakeInvalid();
            return false;
        }
        
        m_kTransformValue.SetScale(fScale);
    }
   
    if (m_kTransformValue.IsTransformInvalid())
    {
        kValue.MakeInvalid();
        return false;
    }

    kValue = m_kTransformValue;
    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::ResolveDependencies(NiAVObjectPalette* pkPalette)
{
    if (m_pkLookAt != NULL)
        return true;
    
    NiAVObject* pkTarget = pkPalette->GetAVObject(m_kLookAtName);
    if (pkTarget != NULL)
    {
        m_pkLookAt = pkTarget;
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::SetUpDependencies()
{
    NIASSERT(m_pkLookAt);
    const char* pcName = m_pkLookAt->GetName();
    if (pcName)
    {
        SetLookAtName(pcName);
        m_pkLookAt = NULL;
        return true;
    }
    else
    {
        SetLookAtName(NULL);
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::AlwaysUpdate() const
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLookAtInterpolator);
//---------------------------------------------------------------------------
void NiLookAtInterpolator::CopyMembers(NiLookAtInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
    pkDest->SetFlip(GetFlip());
    pkDest->SetAxis(GetAxis());
    pkDest->SetLookAt(GetLookAt());
    
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui] != NULL)
        {
            pkDest->m_aspInterpolators[ui] = (NiInterpolator*)
                m_aspInterpolators[ui]->Clone();
        }
    }
}
//---------------------------------------------------------------------------
void NiLookAtInterpolator::ProcessClone(NiCloningProcess& kCloning)
{
    NiInterpolator::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiLookAtInterpolator* pkDest = (NiLookAtInterpolator*) pkClone;

    bCloned = kCloning.m_pkCloneMap->GetAt(m_pkLookAt, pkClone);
    if (bCloned)
    {
        pkDest->m_pkLookAt = (NiAVObject*) pkClone;
    }
    else
    {
        pkDest->m_pkLookAt = m_pkLookAt;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLookAtInterpolator);
//---------------------------------------------------------------------------
void NiLookAtInterpolator::LoadBinary(NiStream& kStream)
{
    NiInterpolator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    m_pkLookAt = (NiAVObject*) kStream.ResolveLinkID();
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kLookAtName);
    }
    else
    {
        kStream.LoadFixedString(m_kLookAtName);
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 4, 0, 13))
    {
        NiQuatTransform kDummyValue;
        kDummyValue.LoadBinary(kStream);
    }

    for (unsigned int ui = 0; ui < 3; ui++)
    {
        m_aspInterpolators[ui] = (NiInterpolator*) kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiLookAtInterpolator::LinkObject(NiStream& kStream)
{
    NiInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiInterpolator::RegisterStreamables(kStream))
        return false;

    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui] && !m_aspInterpolators[ui]->
            RegisterStreamables(kStream))
        {
            return false;
        }
    }

    kStream.RegisterFixedString(m_kLookAtName);

    return true;
}
//---------------------------------------------------------------------------
void NiLookAtInterpolator::SaveBinary(NiStream& kStream)
{
    NiInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
    kStream.SaveLinkID(m_pkLookAt);
    kStream.SaveFixedString(m_kLookAtName);

    for (unsigned int ui = 0; ui < 3; ui++)
    {
        kStream.SaveLinkID(m_aspInterpolators[ui]);
    }
}
//---------------------------------------------------------------------------
bool NiLookAtInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiLookAtInterpolator* pkDest = (NiLookAtInterpolator*) pkObject;

    if (m_uFlags != pkDest->m_uFlags)
    {
        return false;
    }

    if ((m_pkLookAt && !pkDest->m_pkLookAt) ||
        (!m_pkLookAt && pkDest->m_pkLookAt) ||
        (m_pkLookAt && pkDest->m_pkLookAt &&
            !m_pkLookAt->IsEqual(pkDest->m_pkLookAt)))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < 3; ui++)
    {
        NiInterpolator* pkInterp = m_aspInterpolators[ui];
        NiInterpolator* pkDestInterp = pkDest->m_aspInterpolators[ui];
        if ((pkInterp && !pkDestInterp) ||
            (!pkInterp && pkDestInterp) ||
            (pkInterp && !pkInterp->IsEqual(pkDestInterp)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiLookAtInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiLookAtInterpolator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_pkLookAt", m_pkLookAt));

    m_kTransformValue.GetViewerStrings(pkStrings);
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        if (m_aspInterpolators[ui])
            m_aspInterpolators[ui]->GetViewerStrings(pkStrings);
    }
}
//---------------------------------------------------------------------------
