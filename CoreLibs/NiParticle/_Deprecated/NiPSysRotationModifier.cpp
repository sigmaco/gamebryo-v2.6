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

#include "NiPSysRotationModifier.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysRotationModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysRotationModifier::NiPSysRotationModifier() :
    m_fInitialRotSpeed(0.0f),
    m_fInitialRotSpeedVar(0.0f),
    m_fInitialRotAngle(0.0f),
    m_fInitialRotAngleVar(0.0f), 
    m_kInitialAxis(NiPoint3::UNIT_X),
    m_bRandomInitialAxis(true),
    m_bRandomRotSpeedSign(false)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysRotationModifier);
//---------------------------------------------------------------------------
void NiPSysRotationModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fInitialRotSpeed);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 0, 0, 2))
    {
        NiStreamLoadBinary(kStream, m_fInitialRotSpeedVar);
        NiStreamLoadBinary(kStream, m_fInitialRotAngle);
        NiStreamLoadBinary(kStream, m_fInitialRotAngleVar);

        NiBool bRandomRotSpeedSign;
        NiStreamLoadBinary(kStream, bRandomRotSpeedSign);
        m_bRandomRotSpeedSign = (bRandomRotSpeedSign != 0);
    }

    NiBool bRandomInitialAxis;
    NiStreamLoadBinary(kStream, bRandomInitialAxis);
    m_bRandomInitialAxis = (bRandomInitialAxis != 0);
    m_kInitialAxis.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::PostLinkObject(NiStream& kStream)
{
    NiPSysModifier::PostLinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 0, 0, 2))
    {
        // Set the rotation speed for all active particles.
        NiPSysData* pkData = (NiPSysData*) m_pkTarget->GetModelData();
        NIASSERT(pkData);
        float* pfRotationSpeeds = pkData->GetRotationSpeeds();
        if (pfRotationSpeeds)
        {
            for (unsigned short us = 0; us < pkData->GetNumParticles();
                us++)
            {
                pfRotationSpeeds[us] = m_fInitialRotSpeed;
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysRotationModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysRotationModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fInitialRotSpeed);
    NiStreamSaveBinary(kStream, m_fInitialRotSpeedVar);
    NiStreamSaveBinary(kStream, m_fInitialRotAngle);
    NiStreamSaveBinary(kStream, m_fInitialRotAngleVar);
    NiStreamSaveBinary(kStream, NiBool(m_bRandomRotSpeedSign));
    NiStreamSaveBinary(kStream, NiBool(m_bRandomInitialAxis));
    m_kInitialAxis.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysRotationModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysRotationModifier* pkDest = (NiPSysRotationModifier*) pkObject;

    if (pkDest->m_fInitialRotSpeed != m_fInitialRotSpeed ||
        pkDest->m_fInitialRotSpeedVar != m_fInitialRotSpeedVar ||
        pkDest->m_bRandomRotSpeedSign != m_bRandomRotSpeedSign ||
        pkDest->m_fInitialRotAngle != m_fInitialRotAngle ||
        pkDest->m_fInitialRotAngleVar != m_fInitialRotAngleVar ||
        pkDest->m_bRandomInitialAxis != m_bRandomInitialAxis ||
        pkDest->m_kInitialAxis != m_kInitialAxis)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
