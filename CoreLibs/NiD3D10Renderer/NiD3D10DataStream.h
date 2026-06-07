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

#ifndef NID3D10DATASTREAM_H
#define NID3D10DATASTREAM_H

#include "NiD3D10RendererLibType.h"

#include "NiD3D10Headers.h"

#include <NiDataStream.h>
#include <NiDataStreamLockPolicy.h>
#include <NiSmartPointer.h>

NiSmartPointer(NiD3D10DataStream);

class NID3D10RENDERER_ENTRY NiD3D10DataStream : public NiDataStream
{
public:
    // Create from an ID3D10Buffer object
    static NiD3D10DataStream* Create(ID3D10Buffer* pkBuffer);

    // Resizes the data stream
    virtual void Resize(NiUInt32 uiSize);

    // D3D10-specific functions
    inline D3D10_USAGE GetD3D10Usage() const;
    inline NiUInt32 GetD3D10BindFlags() const;
    inline NiUInt32 GetD3D10CPUAccessFlags() const;
    inline ID3D10Buffer* GetBuffer() const;

    // D3D10-specific interpretation of flags
    static bool InterpretDataStreamFlags(NiUInt8& uiAccessMask,
        Usage eUsage, D3D10_USAGE& eD3D10Usage, NiUInt32& uiBindFlags, 
        NiUInt32& uiCPUAccessFlags);
    static bool InterpretAccessMask(NiUInt8 uiAccessMask, 
        D3D10_USAGE& eUsage);
    static NiUInt32 InterpretUsage(Usage eUsage, NiUInt32 uiAccessMask);
    static void InterpretD3D10Parameters(D3D10_USAGE eD3D10Usage, 
        NiUInt32 uiD3D10BindFlags, NiUInt32 uiD3D10CPUAccessFlags, 
        Usage& eUsage, NiUInt8& uiAccessFlags);

    // Maps/unmaps buffer for locking
    void* MapBuffer(NiUInt8 uiLockMask, NiUInt32 m_uiReadLockCount,
        NiUInt32 m_uiWriteLockCount);
    void UnmapBuffer(NiUInt8 uiLockMask, NiUInt32 m_uiReadLockCount,
        NiUInt32 m_uiWriteLockCount);

    inline void SetAccessMask(NiUInt8 uiAccessMask);

    // *** begin Emergent internal use only ***

    void UpdateD3D10Buffers(bool bReleaseSystemMemory = false);

    // *** end Emergent internal use only ***

protected:
    NiD3D10DataStream(const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, NiUInt8 uiAccessMask, Usage eUsage, 
        D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
        NiUInt32 uiCPUAccessFlags);

    NiD3D10DataStream(NiUInt8 uiAccessMask, Usage eUsage, 
        D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
        NiUInt32 uiCPUAccessFlags);

    // Destructor
    virtual ~NiD3D10DataStream();

    // Allocates the data for a data stream.
    virtual void Allocate();

    // Deallocates data
    void Deallocate();

    void D3D10Allocate(bool bInitialize);
    void PerformD3D10BufferUpdate();

    inline static bool NeedsLocalBuffer(NiUInt8 uiAccessMask);

    ID3D10Buffer* m_pkD3D10Buffer;
    void* m_pvBackingBuffer;

    NiUInt32 m_uiD3D10BindFlags;
    NiUInt32 m_uiD3D10CPUAccessFlags;
    D3D10_USAGE m_eD3D10Usage;

    bool m_bDirty;
    bool m_bHidingBufferInBackingBuffer;

    friend class NiD3D10DataStreamFactory;
};

template <class LockPolicy>
class NiD3D10LockableDataStream : public NiD3D10DataStream
{
public:
    NiD3D10LockableDataStream(const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, NiUInt8 uiAccessMask, Usage eUsage, 
        D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
        NiUInt32 uiCPUAccessFlags);

    NiD3D10LockableDataStream(NiUInt8 uiAccessMask, Usage eUsage, 
        D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
        NiUInt32 uiCPUAccessFlags);
protected:
    // Destructor
    virtual ~NiD3D10LockableDataStream();

    // Locks the underlying buffer with the specified constraints
    virtual void* LockImpl(NiUInt8 uiLockMask);

    // Unlocks the vertex buffer
    virtual void UnlockImpl(NiUInt8 uiLockMask);

    LockPolicy m_kLockPolicy;

    friend class NiD3D10DataStreamFactory;
};

// By design, Lock/Unlock always fail for this implementation.
// (Primarily used for 'Stream Output' streams.)
class NiD3D10NonLockableDataStream : public NiD3D10DataStream
{
public:
    NiD3D10NonLockableDataStream(const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, NiUInt8 uiAccessMask, Usage eUsage, 
        D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
        NiUInt32 uiCPUAccessFlags);

    NiD3D10NonLockableDataStream(NiUInt8 uiAccessMask, Usage eUsage, 
        D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
        NiUInt32 uiCPUAccessFlags);
protected:
    // Destructor
    virtual ~NiD3D10NonLockableDataStream();

    // FAILS - returns NULL (unless TOOL masks used).
    virtual void* LockImpl(NiUInt8 uiLockMask);

    // Does nothing (unless TOOL masks used).
    virtual void UnlockImpl(NiUInt8 uiLockMask);

    friend class NiD3D10DataStreamFactory;
};

#include "NiD3D10DataStream.inl"

#endif // NID3D10DATASTREAM_H
