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
#include "NiMainPCH.h"

#include "NiAccumulator.h"

NiImplementRTTI(NiAccumulator,NiObject);

//---------------------------------------------------------------------------
NiAccumulator::NiAccumulator() 
{ 
    m_pkCamera = NULL; 
}
//---------------------------------------------------------------------------
void NiAccumulator::StartAccumulating(const NiCamera* pkCamera)
{
    NIASSERT( pkCamera );

    m_pkCamera = pkCamera;
}
//---------------------------------------------------------------------------
void NiAccumulator::FinishAccumulating()
{ 
    m_pkCamera = NULL;
}
//---------------------------------------------------------------------------
bool NiAccumulator::IsAccumulating() const
{
    return m_pkCamera ? true : false;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiAccumulator::CopyMembers(NiAccumulator* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // NiAccumulator has no members that should be cloned...
    // m_pCamera should only be non-null in the middle of accumulating, which
    // will only happen in the middle of a frame.  So, there is no need to
    // clone it - we leave it at its constructed value (NULL)

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiAccumulator::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

}
//---------------------------------------------------------------------------
void NiAccumulator::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiAccumulator::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiAccumulator::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

}
//---------------------------------------------------------------------------
bool NiAccumulator::IsEqual(NiObject* pkObject)
{
    return NiObject::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
