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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"

NiImplementRTTI(NiPhysXSrc, NiObject);

//---------------------------------------------------------------------------
NiPhysXSrc::NiPhysXSrc()
{
    m_bActive = true;
    m_bInterp = false;
}
//---------------------------------------------------------------------------
NiPhysXSrc::~NiPhysXSrc()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
void NiPhysXSrc::CopyMembers(NiPhysXSrc* pkDest, NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_bActive = m_bActive;
    pkDest->m_bInterp = m_bInterp;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXSrc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXSrc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, NiBool(m_bActive));
    NiStreamSaveBinary(kStream, NiBool(m_bInterp));
}
//---------------------------------------------------------------------------
NiObject* NiPhysXSrc::CreateObject(const char**,
    NiUInt32)
{
    NIASSERT(false && "CreateClass should never be called on NiPhysXSrc.");
    return 0;
}
//---------------------------------------------------------------------------
void NiPhysXSrc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bActive = (bVal == 0) ? false : true;
    NiStreamLoadBinary(kStream, bVal);
    m_bInterp = (bVal == 0) ? false : true;
}
//---------------------------------------------------------------------------
void NiPhysXSrc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}

//---------------------------------------------------------------------------
bool NiPhysXSrc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXSrc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXSrc* pkNewObject = (NiPhysXSrc*)pkObject;

    if (pkNewObject->m_bActive != m_bActive ||
        pkNewObject->m_bInterp != m_bInterp)
        return false;

    return true;
}
//---------------------------------------------------------------------------
