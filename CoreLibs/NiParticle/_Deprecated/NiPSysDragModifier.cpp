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

#include "NiPSysDragModifier.h"
#include <NiAVObject.h>

NiImplementRTTI(NiPSysDragModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysDragModifier::NiPSysDragModifier() :
    m_pkDragObj(NULL),
    m_kDragAxis(NiPoint3::UNIT_X),
    m_fPercentage(0.05f),
    m_fRange(NI_INFINITY),
    m_fFalloff(NI_INFINITY)
{
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysDragModifier);
//---------------------------------------------------------------------------
void NiPSysDragModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkDragObj
    m_kDragAxis.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fPercentage);
    NiStreamLoadBinary(kStream, m_fRange);
    NiStreamLoadBinary(kStream, m_fFalloff);
}
//---------------------------------------------------------------------------
void NiPSysDragModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkDragObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysDragModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysDragModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkDragObj);
    m_kDragAxis.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fPercentage);
    NiStreamSaveBinary(kStream, m_fRange);
    NiStreamSaveBinary(kStream, m_fFalloff);
}
//---------------------------------------------------------------------------
bool NiPSysDragModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysDragModifier* pkDest = (NiPSysDragModifier*) pkObject;

    if ((m_pkDragObj && !pkDest->m_pkDragObj) ||
        (!m_pkDragObj && pkDest->m_pkDragObj) ||
        (m_pkDragObj && pkDest->m_pkDragObj &&
            !m_pkDragObj->IsEqual(pkDest->m_pkDragObj)))
    {
        return false;
    }

    if (pkDest->m_kDragAxis != m_kDragAxis ||
        pkDest->m_fPercentage != m_fPercentage ||
        pkDest->m_fRange != m_fRange ||
        pkDest->m_fFalloff != m_fFalloff)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
