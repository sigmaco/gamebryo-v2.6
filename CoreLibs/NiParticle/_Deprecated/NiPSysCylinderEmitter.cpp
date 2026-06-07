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

#include "NiPSysCylinderEmitter.h"

NiImplementRTTI(NiPSysCylinderEmitter, NiPSysVolumeEmitter);

//---------------------------------------------------------------------------
NiPSysCylinderEmitter::NiPSysCylinderEmitter() :
    m_fEmitterRadius(0.0f),
    m_fEmitterHeight(0.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysCylinderEmitter);
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterRadius);
    NiStreamLoadBinary(kStream, m_fEmitterHeight);
}
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::LinkObject(NiStream& kStream)
{
    NiPSysVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysCylinderEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysCylinderEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterRadius);
    NiStreamSaveBinary(kStream, m_fEmitterHeight);
}
//---------------------------------------------------------------------------
bool NiPSysCylinderEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysCylinderEmitter* pkDest = (NiPSysCylinderEmitter*) pkObject;

    if (m_fEmitterRadius != pkDest->m_fEmitterRadius ||
        m_fEmitterHeight != pkDest->m_fEmitterHeight)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
