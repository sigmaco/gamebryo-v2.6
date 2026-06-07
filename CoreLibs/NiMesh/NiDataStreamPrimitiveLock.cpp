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
#include "NiDataStreamLock.h"
#include "NiDataStreamPrimitiveLock.h"

//---------------------------------------------------------------------------
NiDataStreamPrimitiveLock::NiDataStreamPrimitiveLock(NiMesh* pkMesh,
    NiUInt8 uiLockMask) : m_pkRef(NULL), m_uiSubmeshCount(0),
    m_eType(NiPrimitiveType::PRIMITIVE_MAX)
{    
    NIASSERT(pkMesh != NULL);
    m_eType = pkMesh->GetPrimitiveType();
    m_uiSubmeshCount = pkMesh->GetSubmeshCount();

    if (pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::INDEX(), 
        0, NiDataStreamElement::F_UNKNOWN, m_pkRef, m_kElement))
    {

        // Determine the type of the elements in the stream
        NIASSERT(m_pkRef->GetElementDescCount() == 1);

        m_bIndexed = true;
        m_uiLockType = uiLockMask;

        if (m_pkRef)
        {
            NiDataStream* pkDS = m_pkRef->GetDataStream();
            if (pkDS)
                m_pcBuffer = (char*)pkDS->Lock(m_uiLockType);
        }

    }
    else
    {        
        m_pkRef = pkMesh->GetFirstUsageVertexPerVertexStreamRef();

        NIASSERT(m_pkRef && m_pkRef->GetDataStream());
        m_bIndexed = false;
        m_uiLockType = uiLockMask;
        m_pcBuffer = NULL;
    }   
    
    NIASSERT(m_pkRef);
}
//---------------------------------------------------------------------------
NiDataStreamPrimitiveLock::NiDataStreamPrimitiveLock(
    const NiDataStreamPrimitiveLock& kOther) : m_pkRef(kOther.m_pkRef), 
    m_kElement(kOther.m_kElement), m_uiSubmeshCount(kOther.m_uiSubmeshCount),
    m_eType(kOther.m_eType), m_bIndexed(kOther.m_bIndexed), m_pcBuffer(NULL),
    m_uiLockType(kOther.m_uiLockType)
{    
    if (m_pkRef && m_bIndexed)
    {
        NiDataStream* pkDS = m_pkRef->GetDataStream();
        if (pkDS)
            m_pcBuffer = (char*)pkDS->Lock(m_uiLockType);
    }
}
//---------------------------------------------------------------------------
NiDataStreamPrimitiveLock& NiDataStreamPrimitiveLock::operator=(
    const NiDataStreamPrimitiveLock& kRHS)
{
    if (this != &kRHS)
    {
        Unlock();

        m_pkRef = kRHS.m_pkRef;
        m_kElement = kRHS.m_kElement;
        m_uiSubmeshCount = kRHS.m_uiSubmeshCount;
        m_eType = kRHS.m_eType;
        m_bIndexed = kRHS.m_bIndexed;
        m_pcBuffer = NULL;
        m_uiLockType = kRHS.m_uiLockType;

        if (m_pkRef && m_bIndexed)
        {
            NiDataStream* pkDS = m_pkRef->GetDataStream();
            if (pkDS)
                m_pcBuffer = (char*)pkDS->Lock(m_uiLockType);
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
