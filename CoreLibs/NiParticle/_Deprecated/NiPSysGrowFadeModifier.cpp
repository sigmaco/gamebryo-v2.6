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

#include "NiPSysGrowFadeModifier.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysGrowFadeModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysGrowFadeModifier::NiPSysGrowFadeModifier() :
    m_fGrowTime(0.0f),
    m_usGrowGeneration(0),
    m_fFadeTime(0.0f),
    m_usFadeGeneration(0)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysGrowFadeModifier);
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fGrowTime);
    NiStreamLoadBinary(kStream, m_usGrowGeneration);

    NiStreamLoadBinary(kStream, m_fFadeTime);
    NiStreamLoadBinary(kStream, m_usFadeGeneration);
}
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGrowFadeModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGrowFadeModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fGrowTime);
    NiStreamSaveBinary(kStream, m_usGrowGeneration);

    NiStreamSaveBinary(kStream, m_fFadeTime);
    NiStreamSaveBinary(kStream, m_usFadeGeneration);
}
//---------------------------------------------------------------------------
bool NiPSysGrowFadeModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysGrowFadeModifier* pkDest = (NiPSysGrowFadeModifier*) pkObject;

    if (m_fGrowTime != pkDest->m_fGrowTime ||
        m_usGrowGeneration != pkDest->m_usGrowGeneration ||
        m_fFadeTime != pkDest->m_fFadeTime ||
        m_usFadeGeneration != pkDest->m_usFadeGeneration)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
