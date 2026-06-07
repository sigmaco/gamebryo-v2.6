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

#include "NiPSysTurbulenceFieldModifier.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysTurbulenceFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysTurbulenceFieldModifier::NiPSysTurbulenceFieldModifier() :
    NiPSysFieldModifier(),
    m_fFrequency(0.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysTurbulenceFieldModifier);
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fFrequency);
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysTurbulenceFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysTurbulenceFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fFrequency);
}
//---------------------------------------------------------------------------
bool NiPSysTurbulenceFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysTurbulenceFieldModifier* pkDest = (NiPSysTurbulenceFieldModifier*)
        pkObject;

    if (pkDest->m_fFrequency != m_fFrequency)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
