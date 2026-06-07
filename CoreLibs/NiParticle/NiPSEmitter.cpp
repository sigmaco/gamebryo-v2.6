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

#include "NiPSEmitter.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulator.h"

NiImplementRTTI(NiPSEmitter, NiObject);

//---------------------------------------------------------------------------
NiPSEmitter::NiPSEmitter(
    const NiFixedString& kName,
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
    m_kName(kName),
    m_fSpeed(fSpeed),
    m_fSpeedVar(fSpeedVar),
    m_fDeclination(fDeclination),
    m_fDeclinationVar(fDeclinationVar),
    m_fPlanarAngle(fPlanarAngle),
    m_fPlanarAngleVar(fPlanarAngleVar),
    m_kColor(kColor),
    m_fRadius(fRadius),
    m_fRadiusVar(fRadiusVar),
    m_fLifeSpan(fLifeSpan),
    m_fLifeSpanVar(fLifeSpanVar),
    m_fRotAngle(fRotAngle),
    m_fRotAngleVar(fRotAngleVar),
    m_fRotSpeed(fRotSpeed),
    m_fRotSpeedVar(fRotSpeedVar),
    m_kRotAxis(kRotAxis),
    m_bRandomRotSpeedSign(bRandomRotSpeedSign),
    m_bRandomRotAxis(bRandomRotAxis)
{
}
//---------------------------------------------------------------------------
NiPSEmitter::NiPSEmitter() :
    m_fSpeed(1.0f),
    m_fSpeedVar(0.0f),
    m_fDeclination(0.0f),
    m_fDeclinationVar(0.0f),
    m_fPlanarAngle(0.0f),
    m_fPlanarAngleVar(0.0f),
    m_kColor(NiRGBA::WHITE),
    m_fRadius(1.0f),
    m_fRadiusVar(0.0f),
    m_fLifeSpan(1.0f),
    m_fLifeSpanVar(0.0f),
    m_fRotAngle(0.0f),
    m_fRotAngleVar(0.0f),
    m_fRotSpeed(0.0f),
    m_fRotSpeedVar(0.0f),
    m_kRotAxis(NiPoint3::UNIT_X),
    m_bRandomRotSpeedSign(false),
    m_bRandomRotAxis(true)
{
}
//---------------------------------------------------------------------------
void NiPSEmitter::EmitParticles(
    NiPSParticleSystem* pkParticleSystem,
    float fTime,
    NiUInt32 uiNumParticles,
    const float* pfAges)
{
    for (NiUInt32 ui = 0; ui < uiNumParticles; ++ui)
    {
        // Check to make sure the particle is not already dead. If so,
        // don't create the particle in the first place.
        float fAge = pfAges ? pfAges[ui] : 0.0f;
        float fLifeSpan = m_fLifeSpan;
        if (m_fLifeSpanVar != 0.0f)
        {
            fLifeSpan += m_fLifeSpanVar * (NiUnitRandom() - 0.5f);
        }
        if (fAge > fLifeSpan)
        {
            continue;
        }

        // Create new particle from scratch.

        // Calculate new speed
        float fSpeed = m_fSpeed;
        if (m_fSpeedVar != 0.0f)
        {
            fSpeed += m_fSpeedVar * (NiUnitRandom() - 0.5f);
        }

        // Calculate new direction
        float fDec = m_fDeclination;
        if (m_fDeclinationVar != 0.0f)
        {
            fDec += m_fDeclinationVar * NiSymmetricRandom();
        }
        NiPoint3 kDir(0.0f, 0.0f, 1.0f);
        if (fDec != 0.0f)
        {
            float fSinDec = 0.0f;
            float fCosDec = 1.0f;
            NiSinCos(fDec, fSinDec, fCosDec);

            float fPlan = m_fPlanarAngle;
            if (m_fPlanarAngleVar != 0.0f)
            {
                fPlan += m_fPlanarAngleVar * NiSymmetricRandom();
            }
            float fSinPlan = 0.0f;
            float fCosPlan = 1.0f;
            if (fPlan != 0.0f)
            {
                NiSinCos(fPlan, fSinPlan, fCosPlan);
            }

            kDir = NiPoint3(fSinDec * fCosPlan, fSinDec * fSinPlan, fCosDec);
        }

        // Compute initial position and velocity.
        NiPoint3 kPosition;
        NiPoint3 kVelocity = fSpeed * kDir;
        if (!ComputeInitialPositionAndVelocity(
            pkParticleSystem,
            kPosition,
            kVelocity))
        {
            continue;
        }

        // Get new particle index.
        NiUInt32 uiNewIndex = pkParticleSystem->AddParticle();
        if (uiNewIndex == NiPSParticleSystem::INVALID_PARTICLE)
        {
            return;
        }

        // Various particle initializations
        pkParticleSystem->GetAges()[uiNewIndex] = fAge;
        pkParticleSystem->GetLifeSpans()[uiNewIndex] = fLifeSpan;
        pkParticleSystem->GetFlags()[uiNewIndex] = 0;
        pkParticleSystem->GetPositions()[uiNewIndex] = kPosition;
        pkParticleSystem->GetVelocities()[uiNewIndex] = kVelocity;

        // Initialize other particle data.
        if (pkParticleSystem->HasColors())
        {
            pkParticleSystem->GetColors()[uiNewIndex] = m_kColor;
        }
        float fRadius = m_fRadius;
        if (m_fRadiusVar != 0.0f)
        {
            fRadius += m_fRadiusVar * NiSymmetricRandom();
        }
        pkParticleSystem->GetRadii()[uiNewIndex] = fRadius;
        pkParticleSystem->GetSizes()[uiNewIndex] = 1.0f;
        pkParticleSystem->GetLastUpdateTimes()[uiNewIndex] = fTime - fAge;

        if (pkParticleSystem->HasRotations())
        {
            float fRotAngle = m_fRotAngle;
            if (m_fRotAngleVar != 0.0f)
            {
                fRotAngle += m_fRotAngleVar * NiSymmetricRandom();
            }
            pkParticleSystem->GetRotationAngles()[uiNewIndex] = fRotAngle;
            
            float fRotSpeed = m_fRotSpeed;
            if (m_fRotSpeedVar != 0.0f)
            {
                fRotSpeed += m_fRotSpeedVar * NiSymmetricRandom();
            }
            if (m_bRandomRotSpeedSign)
            {
                // Compute random rot speed sign.
                fRotSpeed = (NiUnitRandom() > 0.5f) ? fRotSpeed : -fRotSpeed;
            }
            pkParticleSystem->GetRotationSpeeds()[uiNewIndex] = fRotSpeed;
        }

        if (pkParticleSystem->HasRotationAxes())
        {
            NiPoint3 kRotAxis = m_kRotAxis;
            if (m_bRandomRotAxis)
            {
                // Compute random rotation axis.
                float fPhi = NiUnitRandom() * NI_PI;
                float fZ = NiCos(fPhi);
                float fHypot = NiSqrt(1.0f - fZ * fZ);
                float fTheta = NiUnitRandom() * NI_TWO_PI;
                kRotAxis.x = fHypot * NiCos(fTheta);
                kRotAxis.y = fHypot * NiSin(fTheta);
                kRotAxis.z = fZ;
            }
            pkParticleSystem->GetRotationAxes()[uiNewIndex] = kRotAxis;
        }

        pkParticleSystem->InitializeParticle(uiNewIndex);
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSEmitter::CopyMembers(
    NiPSEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kName = m_kName;
    pkDest->m_fSpeed = m_fSpeed;
    pkDest->m_fSpeedVar = m_fSpeedVar;
    pkDest->m_fDeclination = m_fDeclination;
    pkDest->m_fDeclinationVar = m_fDeclinationVar;
    pkDest->m_fPlanarAngle = m_fPlanarAngle;
    pkDest->m_fPlanarAngleVar = m_fPlanarAngleVar;
    pkDest->m_kColor = m_kColor;
    pkDest->m_fRadius = m_fRadius;
    pkDest->m_fRadiusVar = m_fRadiusVar;
    pkDest->m_fLifeSpan = m_fLifeSpan;
    pkDest->m_fLifeSpanVar = m_fLifeSpanVar;
    pkDest->m_fRotAngle = m_fRotAngle;
    pkDest->m_fRotAngleVar = m_fRotAngleVar;
    pkDest->m_fRotSpeed = m_fRotSpeed;
    pkDest->m_fRotSpeedVar = m_fRotSpeedVar;
    pkDest->m_kRotAxis = m_kRotAxis;
    pkDest->m_bRandomRotSpeedSign = m_bRandomRotSpeedSign;
    pkDest->m_bRandomRotAxis = m_bRandomRotAxis;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSEmitter::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kName);
    NiStreamLoadBinary(kStream, m_fSpeed);
    NiStreamLoadBinary(kStream, m_fSpeedVar);
    NiStreamLoadBinary(kStream, m_fDeclination);
    NiStreamLoadBinary(kStream, m_fDeclinationVar);
    NiStreamLoadBinary(kStream, m_fPlanarAngle);
    NiStreamLoadBinary(kStream, m_fPlanarAngleVar);
    m_kColor.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fRadius);
    NiStreamLoadBinary(kStream, m_fRadiusVar);
    NiStreamLoadBinary(kStream, m_fLifeSpan);
    NiStreamLoadBinary(kStream, m_fLifeSpanVar);
    NiStreamLoadBinary(kStream, m_fRotAngle);
    NiStreamLoadBinary(kStream, m_fRotAngleVar);
    NiStreamLoadBinary(kStream, m_fRotSpeed);
    NiStreamLoadBinary(kStream, m_fRotSpeedVar);
    m_kRotAxis.LoadBinary(kStream);
    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    m_bRandomRotSpeedSign = NIBOOL_IS_TRUE(bValue);
    NiStreamLoadBinary(kStream, bValue);
    m_bRandomRotAxis = NIBOOL_IS_TRUE(bValue);
}
//---------------------------------------------------------------------------
void NiPSEmitter::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitter::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    kStream.RegisterFixedString(m_kName);

    return true;
}
//---------------------------------------------------------------------------
void NiPSEmitter::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveFixedString(m_kName);
    NiStreamSaveBinary(kStream, m_fSpeed);
    NiStreamSaveBinary(kStream, m_fSpeedVar);
    NiStreamSaveBinary(kStream, m_fDeclination);
    NiStreamSaveBinary(kStream, m_fDeclinationVar);
    NiStreamSaveBinary(kStream, m_fPlanarAngle);
    NiStreamSaveBinary(kStream, m_fPlanarAngleVar);
    m_kColor.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fRadius);
    NiStreamSaveBinary(kStream, m_fRadiusVar);
    NiStreamSaveBinary(kStream, m_fLifeSpan);
    NiStreamSaveBinary(kStream, m_fLifeSpanVar);
    NiStreamSaveBinary(kStream, m_fRotAngle);
    NiStreamSaveBinary(kStream, m_fRotAngleVar);
    NiStreamSaveBinary(kStream, m_fRotSpeed);
    NiStreamSaveBinary(kStream, m_fRotSpeedVar);
    m_kRotAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, NiBool(m_bRandomRotSpeedSign));
    NiStreamSaveBinary(kStream, NiBool(m_bRandomRotAxis));
}
//---------------------------------------------------------------------------
bool NiPSEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSEmitter* pkDest = (NiPSEmitter*) pkObject;

    if (pkDest->m_kName != m_kName ||
        pkDest->m_fSpeed != m_fSpeed ||
        pkDest->m_fSpeedVar != m_fSpeedVar ||
        pkDest->m_fDeclination != m_fDeclination ||
        pkDest->m_fDeclinationVar != m_fDeclinationVar ||
        pkDest->m_fPlanarAngle != m_fPlanarAngle ||
        pkDest->m_fPlanarAngleVar != m_fPlanarAngleVar ||
        pkDest->m_kColor != m_kColor ||
        pkDest->m_fRadius != m_fRadius ||
        pkDest->m_fRadiusVar != m_fRadiusVar ||
        pkDest->m_fLifeSpan != m_fLifeSpan ||
        pkDest->m_fLifeSpanVar != m_fLifeSpanVar ||
        pkDest->m_fRotAngle != m_fRotAngle ||
        pkDest->m_fRotAngleVar != m_fRotAngleVar ||
        pkDest->m_fRotSpeed != m_fRotSpeed ||
        pkDest->m_fRotSpeedVar != m_fRotSpeedVar ||
        pkDest->m_kRotAxis != m_kRotAxis ||
        pkDest->m_bRandomRotSpeedSign != m_bRandomRotSpeedSign ||
        pkDest->m_bRandomRotAxis != m_bRandomRotAxis)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitter::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Name", m_kName));
    pkStrings->Add(NiGetViewerString("Speed", m_fSpeed));
    pkStrings->Add(NiGetViewerString("SpeedVar", m_fSpeedVar));
    pkStrings->Add(NiGetViewerString("Declination", m_fDeclination));
    pkStrings->Add(NiGetViewerString("DeclinationVar", m_fDeclinationVar));
    pkStrings->Add(NiGetViewerString("PlanarAngle", m_fPlanarAngle));
    pkStrings->Add(NiGetViewerString("PlanarAngleVar", m_fPlanarAngleVar));
    pkStrings->Add(m_kColor.GetViewerString("Color"));
    pkStrings->Add(NiGetViewerString("Radius", m_fRadius));
    pkStrings->Add(NiGetViewerString("RasiusVar", m_fRadiusVar));
    pkStrings->Add(NiGetViewerString("LifeSpan", m_fLifeSpan));
    pkStrings->Add(NiGetViewerString("LifeSpanVar", m_fLifeSpanVar));
    pkStrings->Add(NiGetViewerString("RotAngle", m_fRotAngle));
    pkStrings->Add(NiGetViewerString("RotAngleVar", m_fRotAngleVar));
    pkStrings->Add(NiGetViewerString("RotSpeed", m_fRotSpeed));
    pkStrings->Add(NiGetViewerString("RotSpeedVar", m_fRotSpeedVar));
    pkStrings->Add(NiGetViewerString("RandomRotSpeedSign",
        m_bRandomRotSpeedSign));
    pkStrings->Add(m_kRotAxis.GetViewerString("RotAxis"));
    pkStrings->Add(NiGetViewerString("RandomRotAxis", m_bRandomRotAxis));
}
//---------------------------------------------------------------------------
