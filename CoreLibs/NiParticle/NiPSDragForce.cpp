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

#include "NiPSDragForce.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSDragForce, NiPSForce);

//---------------------------------------------------------------------------
NiPSDragForce::NiPSDragForce(
    const NiFixedString& kName,
    NiAVObject* pkDragObj,
    const NiPoint3& kDragAxis,
    float fPercentage,
    float fRange,
    float fRangeFalloff) :
    NiPSForce(kName, NiPSForceDefinitions::FORCE_DRAG),
    m_pkDragObj(pkDragObj)
{
    SetDragAxis(kDragAxis);
    SetPercentage(fPercentage);
    SetRange(fRange);
    SetRangeFalloff(fRangeFalloff);
}
//---------------------------------------------------------------------------
NiPSDragForce::NiPSDragForce() : m_pkDragObj(NULL)
{
}
//---------------------------------------------------------------------------
size_t NiPSDragForce::GetDataSize() const
{
    return sizeof(m_kInputData);
}
//---------------------------------------------------------------------------
void* NiPSDragForce::GetData() const
{
    return (void*) &m_kInputData;
}
//---------------------------------------------------------------------------
bool NiPSDragForce::Update(
    NiPSParticleSystem* pkParticleSystem,
    float)
{
    if (!m_pkDragObj || m_kInputData.m_fPercentage <= 0.0f)
    {
        return false;
    }

    NiTransform kDrag = m_pkDragObj->GetWorldTransform();
    NiTransform kPSys = pkParticleSystem->GetWorldTransform();
    NiTransform kInvPSys;
    kPSys.Invert(kInvPSys);
    NiTransform kDragToPSys = kInvPSys * kDrag;

    m_kInputData.m_kPosition = kDragToPSys.m_Translate;
    m_kInputData.m_kDirection = kDragToPSys.m_Rotate *
        m_kInputData.m_kDragAxis;
    m_kInputData.m_kDirection.Unitize();

    return true;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSDragForce);
//---------------------------------------------------------------------------
void NiPSDragForce::CopyMembers(
    NiPSDragForce* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForce::CopyMembers(pkDest, kCloning);

    pkDest->SetDragAxis(GetDragAxis());
    pkDest->SetPercentage(GetPercentage());
    pkDest->SetRange(GetRange());
    pkDest->SetRangeFalloff(GetRangeFalloff());
}
//---------------------------------------------------------------------------
void NiPSDragForce::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSForce::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSDragForce* pkDest = (NiPSDragForce*) pkObject;

    if (kCloning.m_pkCloneMap->GetAt(m_pkDragObj, pkObject))
    {
        pkDest->m_pkDragObj = (NiAVObject*) pkObject;
    }
    else
    {
        pkDest->m_pkDragObj = m_pkDragObj;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSDragForce);
//---------------------------------------------------------------------------
void NiPSDragForce::LoadBinary(NiStream& kStream)
{
    NiPSForce::LoadBinary(kStream);

    NiPoint3 kValue;
    float fValue;

    kValue.LoadBinary(kStream);
    SetDragAxis(kValue);

    NiStreamLoadBinary(kStream, fValue);
    SetPercentage(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetRange(fValue);

    NiStreamLoadBinary(kStream, fValue);
    SetRangeFalloff(fValue);

    m_pkDragObj = (NiAVObject*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPSDragForce::LinkObject(NiStream& kStream)
{
    NiPSForce::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSDragForce::RegisterStreamables(NiStream& kStream)
{
    return NiPSForce::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSDragForce::SaveBinary(NiStream& kStream)
{
    NiPSForce::SaveBinary(kStream);

    GetDragAxis().SaveBinary(kStream);
    NiStreamSaveBinary(kStream, GetPercentage());
    NiStreamSaveBinary(kStream, GetRange());
    NiStreamSaveBinary(kStream, GetRangeFalloff());
    
    kStream.SaveLinkID(m_pkDragObj);
}
//---------------------------------------------------------------------------
bool NiPSDragForce::IsEqual(NiObject* pkObject)
{
    if (!NiPSForce::IsEqual(pkObject))
    {
        return false;
    }

    NiPSDragForce* pkDest = (NiPSDragForce*) pkObject;

    if ((pkDest->m_pkDragObj && !m_pkDragObj) ||
        (!pkDest->m_pkDragObj && m_pkDragObj))
    {
        return false;
    }

    if (pkDest->GetDragAxis() != GetDragAxis() ||
        pkDest->GetPercentage() != GetPercentage() ||
        pkDest->GetRange() != GetRange() ||
        pkDest->GetRangeFalloff() != GetRangeFalloff())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSDragForce::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSForce::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSDragForce::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Drag Object", m_pkDragObj));
    pkStrings->Add(GetDragAxis().GetViewerString("DragAxis"));
    pkStrings->Add(NiGetViewerString("Percentage", GetPercentage()));
    pkStrings->Add(NiGetViewerString("Range", GetRange()));
    pkStrings->Add(NiGetViewerString("RangeFalloff", GetRangeFalloff()));
}
//---------------------------------------------------------------------------
