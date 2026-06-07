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
#include "NiParticlePCH.h"

#include "NiPSVolumeEmitter.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSVolumeEmitter, NiPSEmitter);

//---------------------------------------------------------------------------
NiPSVolumeEmitter::NiPSVolumeEmitter(
    const NiFixedString& kName,
    NiAVObject* pkEmitterObj,
    float fSpeed,
    float fSpeedVar,
    float fDeclination,
    float fDeclinationVar,
    float fPlanarAngle,
    float fPlanarAngleVar,
    const NiRGBA& kColor,
    float fRadius,
    float fRadiusVar,
    float fLifeSpan,
    float fLifeSpanVar,
    float fRotAngle,
    float fRotAngleVar,
    float fRotSpeed,
    float fRotSpeedVar,
    bool bRandomRotSpeedSign,
    const NiPoint3& kRotAxis,
    bool bRandomRotAxis) :
    NiPSEmitter(
        kName,
        fSpeed,
        fSpeedVar,
        fDeclination,
        fDeclinationVar,
        fPlanarAngle,
        fPlanarAngleVar,
        kColor,
        fRadius,
        fRadiusVar,
        fLifeSpan,
        fLifeSpanVar,
        fRotAngle,
        fRotAngleVar,
        fRotSpeed,
        fRotSpeedVar,
        bRandomRotSpeedSign,
        kRotAxis,
        bRandomRotAxis),
    m_pkEmitterObj(pkEmitterObj)
{
}
//---------------------------------------------------------------------------
NiPSVolumeEmitter::NiPSVolumeEmitter() : m_pkEmitterObj(NULL)
{
}
//---------------------------------------------------------------------------
bool NiPSVolumeEmitter::ComputeInitialPositionAndVelocity(
    NiPSParticleSystem* pkParticleSystem,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    NiTransform kEmitterToPSys;

    // Modify velocity and position based on emitter object transforms.
    if (m_pkEmitterObj)
    {
        NiTransform kEmitter = m_pkEmitterObj->GetWorldTransform();
        NiTransform kPSys = pkParticleSystem->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        kEmitterToPSys = kInvPSys * kEmitter;
    }
    else
    {
        kEmitterToPSys.MakeIdentity();
    }

    return ComputeVolumeInitialPositionAndVelocity(
        kEmitterToPSys,
        kPosition,
        kVelocity);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSVolumeEmitter::CopyMembers(NiPSVolumeEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitter::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiPSVolumeEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSEmitter::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSVolumeEmitter* pkDest = (NiPSVolumeEmitter*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkEmitterObj, pkObject))
    {
        pkDest->m_pkEmitterObj = (NiAVObject*) pkObject;
    }
    else
    {
        pkDest->m_pkEmitterObj = m_pkEmitterObj;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSVolumeEmitter::LoadBinary(NiStream& kStream)
{
    NiPSEmitter::LoadBinary(kStream);

    m_pkEmitterObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSVolumeEmitter::LinkObject(NiStream& kStream)
{
    NiPSEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSVolumeEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSVolumeEmitter::SaveBinary(NiStream& kStream)
{
    NiPSEmitter::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkEmitterObj);
}
//---------------------------------------------------------------------------
bool NiPSVolumeEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSVolumeEmitter* pkDest = (NiPSVolumeEmitter*) pkObject;

    if ((pkDest->m_pkEmitterObj && !m_pkEmitterObj) ||
        (!pkDest->m_pkEmitterObj && m_pkEmitterObj))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSVolumeEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSVolumeEmitter::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Emitter Object", m_pkEmitterObj));
}
//---------------------------------------------------------------------------
