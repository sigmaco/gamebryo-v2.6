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

#include "NiPSSphereEmitter.h"
#include <NiTransform.h>

NiImplementRTTI(NiPSSphereEmitter, NiPSVolumeEmitter);

//---------------------------------------------------------------------------
NiPSSphereEmitter::NiPSSphereEmitter(
    const NiFixedString& kName,
    float fEmitterRadius,
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
    NiPSVolumeEmitter(
        kName,
        pkEmitterObj,
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
    m_fEmitterRadius(fEmitterRadius)
{
}
//---------------------------------------------------------------------------
NiPSSphereEmitter::NiPSSphereEmitter() : m_fEmitterRadius(0.0f)
{
}
//---------------------------------------------------------------------------
bool NiPSSphereEmitter::ComputeVolumeInitialPositionAndVelocity(
    NiTransform& kEmitterToPSys,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Compute random initial position in sphere.
    if (m_fEmitterRadius == 0.0f)
    {
        kPosition = NiPoint3::ZERO;
    }
    else
    {
        float fRadius = m_fEmitterRadius * NiUnitRandom();
        float fPhi = NiUnitRandom() * NI_TWO_PI;
        float fTheta = NiUnitRandom() * NI_TWO_PI;

        float fSinTheta, fCosTheta, fSinPhi, fCosPhi;
        NiSinCos(fPhi, fSinPhi, fCosPhi);
        NiSinCos(fTheta, fSinTheta, fCosTheta);

        kPosition.x = fRadius * fSinTheta * fCosPhi;
        kPosition.y = fRadius * fSinTheta * fSinPhi;
        kPosition.z = fRadius * fCosTheta;
    }
  
    // Update position.
    kPosition = kEmitterToPSys * kPosition;

    // Update velocity.
    kVelocity = kEmitterToPSys.m_Rotate * kVelocity;

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSphereEmitter);
//---------------------------------------------------------------------------
void NiPSSphereEmitter::CopyMembers(NiPSSphereEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSVolumeEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_fEmitterRadius = m_fEmitterRadius;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSphereEmitter);
//---------------------------------------------------------------------------
void NiPSSphereEmitter::LoadBinary(NiStream& kStream)
{
    NiPSVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterRadius);
}
//---------------------------------------------------------------------------
void NiPSSphereEmitter::LinkObject(NiStream& kStream)
{
    NiPSVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSphereEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSSphereEmitter::SaveBinary(NiStream& kStream)
{
    NiPSVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterRadius);
}
//---------------------------------------------------------------------------
bool NiPSSphereEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSphereEmitter* pkDest = (NiPSSphereEmitter*) pkObject;

    if (pkDest->m_fEmitterRadius != m_fEmitterRadius)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSphereEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSVolumeEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSphereEmitter::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("EmitterRadius", m_fEmitterRadius));
}
//---------------------------------------------------------------------------
