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

#include "NiPSCylinderEmitter.h"
#include <NiTransform.h>

NiImplementRTTI(NiPSCylinderEmitter, NiPSVolumeEmitter);

//---------------------------------------------------------------------------
NiPSCylinderEmitter::NiPSCylinderEmitter(
    const NiFixedString& kName,
    float fEmitterRadius,
    float fEmitterHeight,
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
    m_fEmitterRadius(fEmitterRadius),
    m_fEmitterHeight(fEmitterHeight)
{
}
//---------------------------------------------------------------------------
NiPSCylinderEmitter::NiPSCylinderEmitter() : 
    m_fEmitterRadius(0.0f),
    m_fEmitterHeight(0.0f)
{
}
//---------------------------------------------------------------------------
bool NiPSCylinderEmitter::ComputeVolumeInitialPositionAndVelocity(
    NiTransform& kEmitterToPSys,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Compute random initial position in cylinder.
    if (m_fEmitterRadius == 0.0f)
    {
        kPosition.x = kPosition.y = 0.0f;
    }
    else
    {
        float fRadius = m_fEmitterRadius * NiUnitRandom();
        float fPhi = NiUnitRandom() * NI_TWO_PI;

        float fSinPhi, fCosPhi;
        NiSinCos(fPhi, fSinPhi, fCosPhi);

        kPosition.x = fRadius * fCosPhi;
        kPosition.y = fRadius * fSinPhi;
    }
    kPosition.z = m_fEmitterHeight;
    if (kPosition.z != 0.0f)
    {
        kPosition.z *= NiUnitRandom() - 0.5f;
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
NiImplementCreateClone(NiPSCylinderEmitter);
//---------------------------------------------------------------------------
void NiPSCylinderEmitter::CopyMembers(NiPSCylinderEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSVolumeEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_fEmitterRadius = m_fEmitterRadius;
    pkDest->m_fEmitterHeight = m_fEmitterHeight;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSCylinderEmitter);
//---------------------------------------------------------------------------
void NiPSCylinderEmitter::LoadBinary(NiStream& kStream)
{
    NiPSVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterRadius);
    NiStreamLoadBinary(kStream, m_fEmitterHeight);
}
//---------------------------------------------------------------------------
void NiPSCylinderEmitter::LinkObject(NiStream& kStream)
{
    NiPSVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSCylinderEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSCylinderEmitter::SaveBinary(NiStream& kStream)
{
    NiPSVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterRadius);
    NiStreamSaveBinary(kStream, m_fEmitterHeight);
}
//---------------------------------------------------------------------------
bool NiPSCylinderEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSCylinderEmitter* pkDest = (NiPSCylinderEmitter*) pkObject;

    if (pkDest->m_fEmitterRadius != m_fEmitterRadius ||
        pkDest->m_fEmitterHeight != m_fEmitterHeight)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSCylinderEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSVolumeEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSCylinderEmitter::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Emitter Radius", m_fEmitterRadius));
    pkStrings->Add(NiGetViewerString("Emitter Height", m_fEmitterHeight));
}
//---------------------------------------------------------------------------
