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

#include "NiToolDataStream.h"
#include "NiStream.h"

NiImplementRTTI(NiToolDataStream, NiDataStream);

//---------------------------------------------------------------------------
NiToolDataStream::NiToolDataStream(const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    Usage eUsage)
    : NiDataStream(kElements, uiCount, uiAccessMask, eUsage)
{
    Allocate();
}
//---------------------------------------------------------------------------
NiToolDataStream::NiToolDataStream(NiUInt8 uiAccessMask, Usage eUsage)
    : NiDataStream(uiAccessMask, eUsage), m_pucMemory(0)
{
}
//---------------------------------------------------------------------------
NiToolDataStream::~NiToolDataStream()
{
    NiAlignedFree(m_pucMemory);
}
//---------------------------------------------------------------------------
void NiToolDataStream::Allocate()
{
    m_pucMemory = (unsigned char*) NiAlignedMalloc(m_uiSize, 128);
    NIASSERT(m_pucMemory);
}
//---------------------------------------------------------------------------
void NiToolDataStream::Resize(unsigned int uiSize)
{
    m_pucMemory = (unsigned char*)NiAlignedRealloc(m_pucMemory, uiSize, 128);
    NIASSERT(m_pucMemory);
    m_uiSize = uiSize;
}
//---------------------------------------------------------------------------
bool NiToolDataStream::IsLockRequestValid(NiUInt8 uiLockMask) const
{
    // Allow tool streams to always be locked, regardless of flag
    // Applications querying the validity of the lock flags on a tool 
    // data stream for the purposes of validating a platform-specific data 
    // stream can call the NiDataStream::IsLockRequestValid function 
    // explicitly.
    // e.g., pkDataStream->NiDataStream::IsLockRequestValid(uiLockMask);
    return true;
}
//---------------------------------------------------------------------------
void* NiToolDataStream::LockImpl(NiUInt8)
{
    return m_pucMemory;
}
//---------------------------------------------------------------------------
void NiToolDataStream::UnlockImpl(NiUInt8)
{
    // Do nothing
}
//---------------------------------------------------------------------------
void NiToolDataStream::SetAccessMask(NiUInt8 uiAccessMask)
{
    if (IsAccessRequestValid(uiAccessMask, m_eUsage))
        m_uiAccessMask = uiAccessMask;
}
//---------------------------------------------------------------------------
void NiToolDataStream::SetUsage(NiDataStream::Usage eUsage)
{
    m_eUsage = eUsage;
}
//---------------------------------------------------------------------------
void NiToolDataStream::MakeWritable()
{
    // If CPU readable, change to mutable
    if (m_uiAccessMask & ACCESS_CPU_READ)
    {
        // Clear current CPU_WRITE access flags
        m_uiAccessMask &= ~ACCESS_CPU_WRITE_ANY;
        
        // Set to ACCESS_CPU_WRITE_MUTABLE
        m_uiAccessMask |= ACCESS_CPU_WRITE_MUTABLE;
    }
    // If static, change to volatile
    else if (m_uiAccessMask & ACCESS_CPU_WRITE_STATIC)
    {
        // Clear current CPU_WRITE access flags
        m_uiAccessMask &= ~ACCESS_CPU_WRITE_ANY;

        // Set to ACCESS_CPU_WRITE_VOLATILE
        m_uiAccessMask |= ACCESS_CPU_WRITE_VOLATILE;
    }

    // otherwise leave as is

    // confirm expected result
    NIASSERT(m_uiAccessMask & 
        (ACCESS_CPU_WRITE_MUTABLE | ACCESS_CPU_WRITE_VOLATILE));
    NIASSERT(IsAccessRequestValid(m_uiAccessMask, m_eUsage));
}
//---------------------------------------------------------------------------
void NiToolDataStream::MakeReadable()
{
    // If volatile, change to mutable
    if (m_uiAccessMask & ACCESS_CPU_WRITE_VOLATILE)
    {
        // Clear current CPU_WRITE access flags
        m_uiAccessMask &= ~ACCESS_CPU_WRITE_ANY;

        // Set to ACCESS_CPU_WRITE_MUTABLE
        m_uiAccessMask |= ACCESS_CPU_WRITE_MUTABLE;
    }

    // Or-in CPU read flag.
    m_uiAccessMask |= ACCESS_CPU_READ;

    // confirm expected result
    NIASSERT(IsAccessRequestValid(m_uiAccessMask, m_eUsage));
}
//---------------------------------------------------------------------------
