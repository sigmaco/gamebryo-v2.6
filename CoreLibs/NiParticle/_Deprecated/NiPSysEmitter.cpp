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
#include <NiParticlePCH.h>

#include "NiPSysEmitter.h"

NiImplementRTTI(NiPSysEmitter, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysEmitter::NiPSysEmitter() :
    m_fSpeed(0.0f),
    m_fSpeedVar(0.0f),
    m_fDeclination(0.0f),
    m_fDeclinationVar(0.0f),
    m_fPlanarAngle(0.0f),
    m_fPlanarAngleVar(0.0f),
    m_kInitialColor(NiColorA::WHITE),
    m_fInitialRadius(1.0f), 
    m_fRadiusVar(0.0f),
    m_fLifeSpan(0.0f),
    m_fLifeSpanVar(0.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fSpeed);
    NiStreamLoadBinary(kStream, m_fSpeedVar);
    NiStreamLoadBinary(kStream, m_fDeclination);
    NiStreamLoadBinary(kStream, m_fDeclinationVar);
    NiStreamLoadBinary(kStream, m_fPlanarAngle);
    NiStreamLoadBinary(kStream, m_fPlanarAngleVar);
    m_kInitialColor.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fInitialRadius);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 3, 0, 2))
    {
        NiStreamLoadBinary(kStream, m_fRadiusVar);
    }
    NiStreamLoadBinary(kStream, m_fLifeSpan);
    NiStreamLoadBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
void NiPSysEmitter::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fSpeed);
    NiStreamSaveBinary(kStream, m_fSpeedVar);
    NiStreamSaveBinary(kStream, m_fDeclination);
    NiStreamSaveBinary(kStream, m_fDeclinationVar);
    NiStreamSaveBinary(kStream, m_fPlanarAngle);
    NiStreamSaveBinary(kStream, m_fPlanarAngleVar);
    m_kInitialColor.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fInitialRadius);
    NiStreamSaveBinary(kStream, m_fRadiusVar);
    NiStreamSaveBinary(kStream, m_fLifeSpan);
    NiStreamSaveBinary(kStream, m_fLifeSpanVar);
}
//---------------------------------------------------------------------------
bool NiPSysEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysEmitter* pkDest = (NiPSysEmitter*) pkObject;

    if (pkDest->m_fSpeed != m_fSpeed ||
        pkDest->m_fSpeedVar != m_fSpeedVar ||
        pkDest->m_fDeclination != m_fDeclination ||
        pkDest->m_fDeclinationVar != m_fDeclinationVar ||
        pkDest->m_fPlanarAngle != m_fPlanarAngle ||
        pkDest->m_fPlanarAngleVar != m_fPlanarAngleVar ||
        pkDest->m_kInitialColor != m_kInitialColor ||
        pkDest->m_fInitialRadius != m_fInitialRadius ||
        pkDest->m_fRadiusVar != m_fRadiusVar ||
        pkDest->m_fLifeSpan != m_fLifeSpan ||
        pkDest->m_fLifeSpanVar != m_fLifeSpanVar)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
