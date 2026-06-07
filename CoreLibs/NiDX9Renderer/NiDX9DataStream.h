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

#ifndef NIDX9DATASTREAM_H
#define NIDX9DATASTREAM_H

#include <NiDataStream.h>
#include <NiDataStreamLockPolicy.h>

#include "NiDX9RendererLibType.h"
#include "NiDX9Headers.h"

#include <NiSmartPointer.h>

NiSmartPointer(NiDX9DataStream);

class NIDX9RENDERER_ENTRY NiDX9DataStream : public NiDataStream
{
    NiDeclareRTTI;
public:

    class NIDX9RENDERER_ENTRY NiDX9BufferInfo : public NiMemObject
    {
    public:
        NiDX9BufferInfo();
        NiUInt32 m_uiCurrentIndex;
        NiUInt32 m_uiSize;
        NiUInt32 m_uiUnusedSize;
        NiUInt32 m_uiLockFrameID;

        inline void IncRefCount();
        inline void DecRefCount();
        inline unsigned int GetRefCount() const;
        
        inline void IncLockCount();
        inline void DecLockCount();
        inline unsigned int GetLockCount() const;

        // Used to keep track of locking on shared DX9 buffers.
        void* m_pvLockedData;

        union 
        {
            LPDIRECT3DVERTEXBUFFER9 m_pkVBBuffer;
            LPDIRECT3DINDEXBUFFER9 m_pkIBBuffer;
        };

        bool m_bShared;
    protected:
        NiUInt32 m_uiRefCount;      
        NiUInt32 m_uiDX9LockCount;
    };

    //
    // NiDataStream Overrides
    //

    // Constructor
    NiDX9DataStream(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        Usage eUsage);
    NiDX9DataStream(
        NiUInt8 uiAccessMask, 
        Usage eUsage);
    
    // Destructor
    virtual ~NiDX9DataStream();

    // Resizes the data stream
    virtual void Resize(NiUInt32 uiSize);

    void SetAccessMask(NiUInt8 uiAccessMask);

    void* MapBuffer(NiUInt8 uiLockType, NiUInt32 uiReadLockCount,
        NiUInt32 uiWriteLockCount);
    void UnmapBuffer(NiUInt8 uiLockType, NiUInt32 uiReadLockCount,
        NiUInt32 uiWriteLockCount);

    //
    // DX9 Specific
    //
    NiUInt32 GetDX9UsageFlags() const;
    LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer() const;
    LPDIRECT3DINDEXBUFFER9 GetIndexBuffer() const;

    // *** begin Emergent internal use only ***
    void LostDevice();
    void Recreate();

    void UpdateD3DBuffers(bool bReleaseSystemMemory = false);

    /// Offset into the DirectX buffer where this logical NiDataStream starts.
    NiUInt32 GetD3DOffset();
    // *** end Emergent internal use only ***

protected:
    // Allocates the data for a data stream.
    virtual void Allocate();

    // Deallocates data
    void Deallocate();

    // Ensures the access mask is consistent with the usage flags
    void ValidateAccessMask();

    //
    // DX9 Specific
    //
    void DX9Allocate();
    void DX9Deallocate();
    void AcquireDX9BufferInfo();

    void* DX9Lock(NiUInt8 uiLockType);
    void DX9Unlock();

    D3DPOOL DetermineD3DPool();

    NiDX9BufferInfo* m_pkDX9BufferInfo;
    NiUInt8* m_pucLocalBufferCopy;

    NiUInt32 m_uiDX9UsageFlags;
    NiUInt32 m_uiD3DBufferOffset;
    bool m_bDirty;

    static NiUInt32 ms_uiSharedBufferDefaultSize;
};


template <class LockPolicy>
class NIDX9RENDERER_ENTRY NiDX9LockableDataStream : 
    public NiDX9DataStream
{
public:
    // Constructor
    NiDX9LockableDataStream(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        Usage eUsage);

    // Constructor
    NiDX9LockableDataStream (
        NiUInt8 uiAccessMask, 
        Usage eUsage);

    // Destructor
    virtual ~NiDX9LockableDataStream ();

protected:
    // Locks the underlying buffer with the specified constraints
    virtual void* LockImpl(NiUInt8 uiLockMask);

    // Platform specific unlocking mechanism
    virtual void UnlockImpl(NiUInt8 uiLockMask);

    // The LockPolicy implements Locking and Unlocking
    LockPolicy m_kLockPolicy;
};

#endif
