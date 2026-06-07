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

#include "NiPhysXClothDest.h"

NiImplementRTTI(NiPhysXClothDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXClothDest::NiPhysXClothDest()
{
    m_spTarget = 0;
    m_fMeshGrowFactor = 1.5;
    m_bDoSleepXforms = true;
}
//---------------------------------------------------------------------------
NiPhysXClothDest::~NiPhysXClothDest()
{
    m_spTarget = 0;
}
//---------------------------------------------------------------------------
NiTriShape* NiPhysXClothDest::GetTarget()
{
    return m_spTarget;
}
//---------------------------------------------------------------------------
float NiPhysXClothDest::GetMeshGrowFactor() const
{
    return m_fMeshGrowFactor;
}
//---------------------------------------------------------------------------
bool NiPhysXClothDest::GetDoSleepXforms() const
{
    return m_bDoSleepXforms;
}
//---------------------------------------------------------------------------
unsigned short NiPhysXClothDest::GetNBTSet() const
{
    return m_usNBTSet;
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::UpdateFromActors(const float,
    const NiTransform&, const float,
    const bool)
{
    // Deliberately empty
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::UpdateSceneGraph(const float,
    const NiTransform&, const float,
    const bool)
{
    // Deliberately empty
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
inline bool NiPhysXClothDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXClothDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    if (m_spTarget)
        m_spTarget->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
    
    kStream.SaveLinkID(m_spTarget);

    NiStreamSaveBinary(kStream, m_fMeshGrowFactor);

    NiBool bBool(m_bDoSleepXforms);
    NiStreamSaveBinary(kStream, bBool);
    
    NiStreamSaveBinary(kStream, m_usNBTSet);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXClothDest);
//---------------------------------------------------------------------------
void NiPhysXClothDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_spTarget = (NiTriShape*)kStream.ResolveLinkID();
    
    NiStreamLoadBinary(kStream, m_fMeshGrowFactor);
    
    NiBool bBool;
    NiStreamLoadBinary(kStream, bBool);
    m_bDoSleepXforms = bBool ? true : false;
    
    NiStreamLoadBinary(kStream, m_usNBTSet);
}
//---------------------------------------------------------------------------
void NiPhysXClothDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
    
    m_spTarget->SetConsistency(NiGeometryData::MUTABLE);
}
//---------------------------------------------------------------------------
bool NiPhysXClothDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXClothDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;
        
    NiPhysXClothDest* pkThatObject = (NiPhysXClothDest*)pkObject;
    
    if (m_spTarget)
    {
        if (pkThatObject->m_spTarget)
        {
            if (!m_spTarget->IsEqual(pkThatObject->m_spTarget))
                return false;
        }
        else
        {
            return false;
        }
    }
    else if (pkThatObject->m_spTarget)
    {
        return false;
    }
    
    if (m_fMeshGrowFactor != pkThatObject->m_fMeshGrowFactor ||
        m_bDoSleepXforms != pkThatObject->m_bDoSleepXforms ||
        m_usNBTSet != pkThatObject->m_usNBTSet)
        return false;

    return true;
}
//---------------------------------------------------------------------------
