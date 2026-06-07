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

#include "NiPSBoxEmitter.h"
#include <NiTransform.h>

NiImplementRTTI(NiPSBoxEmitter, NiPSVolumeEmitter);

//---------------------------------------------------------------------------
NiPSBoxEmitter::NiPSBoxEmitter(
    const NiFixedString& kName,
    float fEmitterWidth,
    float fEmitterHeight,
    float fEmitterDepth,
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
    m_fEmitterWidth(fEmitterWidth),
    m_fEmitterHeight(fEmitterHeight),
    m_fEmitterDepth(fEmitterDepth)
{
}
//---------------------------------------------------------------------------
NiPSBoxEmitter::NiPSBoxEmitter() : 
    m_fEmitterWidth(0.0f),
    m_fEmitterHeight(0.0f),
    m_fEmitterDepth(0.0f)
{
}
//---------------------------------------------------------------------------
bool NiPSBoxEmitter::ComputeVolumeInitialPositionAndVelocity(
    NiTransform& kEmitterToPSys,
    NiPoint3& kPosition,
    NiPoint3& kVelocity)
{
    // Compute random initial position in Box.
    float fWidth = (m_fEmitterWidth == 0.0f) ? m_fEmitterWidth :
        (m_fEmitterWidth * (NiUnitRandom() - 0.5f));
    float fHeight = (m_fEmitterHeight == 0.0f) ? m_fEmitterHeight :
        (m_fEmitterHeight * (NiUnitRandom() - 0.5f));
    float fDepth = (m_fEmitterDepth == 0.0f) ? m_fEmitterDepth :
        (m_fEmitterDepth * (NiUnitRandom() - 0.5f));
    kPosition = NiPoint3(fWidth, fHeight, fDepth);
   
    // Update position.
    kPosition = kEmitterToPSys * kPosition;

    // Update velocity.
    kVelocity = kEmitterToPSys.m_Rotate * kVelocity;

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSBoxEmitter);
//---------------------------------------------------------------------------
void NiPSBoxEmitter::CopyMembers(NiPSBoxEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSVolumeEmitter::CopyMembers(pkDest, kCloning);

    pkDest->m_fEmitterWidth = m_fEmitterWidth;
    pkDest->m_fEmitterHeight = m_fEmitterHeight;
    pkDest->m_fEmitterDepth = m_fEmitterDepth;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSBoxEmitter);
//---------------------------------------------------------------------------
void NiPSBoxEmitter::LoadBinary(NiStream& kStream)
{
    NiPSVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterWidth);
    NiStreamLoadBinary(kStream, m_fEmitterHeight);
    NiStreamLoadBinary(kStream, m_fEmitterDepth);
}
//---------------------------------------------------------------------------
void NiPSBoxEmitter::LinkObject(NiStream& kStream)
{
    NiPSVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSBoxEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSBoxEmitter::SaveBinary(NiStream& kStream)
{
    NiPSVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterWidth);
    NiStreamSaveBinary(kStream, m_fEmitterHeight);
    NiStreamSaveBinary(kStream, m_fEmitterDepth);
}
//---------------------------------------------------------------------------
bool NiPSBoxEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSBoxEmitter* pkDest = (NiPSBoxEmitter*) pkObject;

    if (pkDest->m_fEmitterWidth != m_fEmitterWidth ||
        pkDest->m_fEmitterHeight != m_fEmitterHeight ||
        pkDest->m_fEmitterDepth != m_fEmitterDepth)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSBoxEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSVolumeEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSBoxEmitter::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Emitter Width", m_fEmitterWidth));
    pkStrings->Add(NiGetViewerString("Emitter Height", m_fEmitterHeight));
    pkStrings->Add(NiGetViewerString("Emitter Depth", m_fEmitterDepth));
}
//---------------------------------------------------------------------------
