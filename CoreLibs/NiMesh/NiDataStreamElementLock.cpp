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

#include "NiMeshPCH.h"

#include "NiMesh.h"
#include "NiDataStreamElementLock.h"

//---------------------------------------------------------------------------
NiDataStreamElementLock::NiDataStreamElementLock() :
    m_pkRef(NULL), m_uiSubmeshCount(0), m_pcBuffer(NULL), m_uiLockType(0)
{
}
//---------------------------------------------------------------------------
NiDataStreamElementLock::NiDataStreamElementLock(
    const NiDataStreamElementLock& kOther) :
    m_pkRef(kOther.m_pkRef), m_kElement(kOther.m_kElement), 
    m_uiSubmeshCount(kOther.m_uiSubmeshCount), m_pcBuffer(NULL), 
    m_uiLockType(kOther.m_uiLockType)
{
    if (m_pkRef)
    {
        NiDataStream* pkDS = m_pkRef->GetDataStream();
        if (pkDS)
            m_pcBuffer = (char*)pkDS->Lock(m_uiLockType);
    }
}
//---------------------------------------------------------------------------
NiDataStreamElementLock::NiDataStreamElementLock(NiMesh* pkMesh,
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex, 
    NiDataStreamElement::Format eFormat, NiUInt8 uiLockMask) :
    m_pkRef(NULL), m_uiSubmeshCount(0), m_pcBuffer(NULL), m_uiLockType(0)
{
    if (pkMesh && pkMesh->FindStreamRefAndElementBySemantic(
        kSemantic, uiSemanticIndex, eFormat, m_pkRef, m_kElement))
    {
        m_uiSubmeshCount = pkMesh->GetSubmeshCount();
        m_uiLockType = uiLockMask;
        if (m_pkRef)
        {
            NiDataStream* pkDS = m_pkRef->GetDataStream();
            if (pkDS)
                m_pcBuffer = (char*)pkDS->Lock(m_uiLockType);
        }
    }
}
//---------------------------------------------------------------------------
NiDataStreamElementLock& NiDataStreamElementLock::operator=(
    const NiDataStreamElementLock& kRHS)
{
    if (this != &kRHS)
    {
        Unlock();
        
        m_pkRef = kRHS.m_pkRef;
        m_kElement = kRHS.m_kElement;
        m_uiSubmeshCount = kRHS.m_uiSubmeshCount;
        m_pcBuffer = NULL;
        m_uiLockType = kRHS.m_uiLockType;

        if (m_pkRef)
        {
            NiDataStream* pkDS = m_pkRef->GetDataStream();
            if (pkDS)
                m_pcBuffer = (char*)pkDS->Lock(m_uiLockType);
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
