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

#include "NiPSysDragFieldModifier.h"
#include <NiBool.h>

NiImplementRTTI(NiPSysDragFieldModifier, NiPSysFieldModifier);

//---------------------------------------------------------------------------
NiPSysDragFieldModifier::NiPSysDragFieldModifier() :
    m_bUseDirection(false),
    m_kDirection(NiPoint3::ZERO)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysDragFieldModifier);
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysFieldModifier::LoadBinary(kStream);

    // Load Use Direction
    NiBool bBool;
    NiStreamLoadBinary(kStream, bBool);
    m_bUseDirection = (bBool != 0);
    
    // Load Direction
    m_kDirection.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysFieldModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysDragFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysFieldModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysDragFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysFieldModifier::SaveBinary(kStream);

    // Save Use Direction
    NiBool bBool = m_bUseDirection;
    NiStreamSaveBinary(kStream, bBool);

    // Save Direction
    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysDragFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysFieldModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysDragFieldModifier* pkDest = (NiPSysDragFieldModifier*) pkObject;

    if (pkDest->m_bUseDirection != m_bUseDirection ||
        pkDest->m_kDirection != m_kDirection)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
