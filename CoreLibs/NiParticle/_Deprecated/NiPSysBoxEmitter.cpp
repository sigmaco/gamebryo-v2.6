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

#include "NiPSysBoxEmitter.h"

NiImplementRTTI(NiPSysBoxEmitter, NiPSysVolumeEmitter);

//---------------------------------------------------------------------------
NiPSysBoxEmitter::NiPSysBoxEmitter() :
    m_fEmitterWidth(0.0f),
    m_fEmitterHeight(0.0f),
    m_fEmitterDepth(0.0f)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysBoxEmitter);
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fEmitterWidth);
    NiStreamLoadBinary(kStream, m_fEmitterHeight);
    NiStreamLoadBinary(kStream, m_fEmitterDepth);
}
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::LinkObject(NiStream& kStream)
{
    NiPSysVolumeEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysBoxEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysVolumeEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysBoxEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysVolumeEmitter::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fEmitterWidth);
    NiStreamSaveBinary(kStream, m_fEmitterHeight);
    NiStreamSaveBinary(kStream, m_fEmitterDepth);
}
//---------------------------------------------------------------------------
bool NiPSysBoxEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysVolumeEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysBoxEmitter* pkDest = (NiPSysBoxEmitter*) pkObject;

    if (m_fEmitterWidth != pkDest->m_fEmitterWidth ||
        m_fEmitterHeight != pkDest->m_fEmitterHeight ||
        m_fEmitterDepth != pkDest->m_fEmitterDepth)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
