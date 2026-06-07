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
#include "NiD3DRendererPCH.h"

#include "NiDX9DataStream.h"

#include "NiDX9Error.h"
#include "NiDX9Renderer.h"

NiImplementRTTI(NiDX9DataStream, NiDataStream);

// Clients can configure this if needed with 
NiUInt32 NiDX9DataStream::ms_uiSharedBufferDefaultSize = 1024*1024; 

//---------------------------------------------------------------------------
// Local Functions
//---------------------------------------------------------------------------
NiDataStream::Usage GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(
    const NiDataStream* pkStream)
{
    if (pkStream->GetAccessMask() & NiDataStream::ACCESS_GPU_READ)
        return pkStream->GetUsage();
    else
        return NiDataStream::USAGE_USER;
}

//---------------------------------------------------------------------------
// Class Functions
//---------------------------------------------------------------------------
NiDX9DataStream::NiDX9BufferInfo::NiDX9BufferInfo() : 
    m_uiCurrentIndex(0), 
    m_uiSize(0), 
    m_uiUnusedSize(0), 
    m_uiLockFrameID(INT_MAX), 
    m_uiRefCount(0), 
    m_uiDX9LockCount(0), 
    m_pvLockedData(NULL), 
    m_pkVBBuffer(NULL), 
    m_bShared(false)
{
}
//---------------------------------------------------------------------------
inline void NiDX9DataStream::NiDX9BufferInfo::IncRefCount()
{
    NiAtomicIncrement(m_uiRefCount);
}
//---------------------------------------------------------------------------
inline void NiDX9DataStream::NiDX9BufferInfo::DecRefCount()
{
    NIASSERT(m_uiRefCount > 0 && "DecRefCount called when refcount == 0");
    NiAtomicDecrement(m_uiRefCount);
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9DataStream::NiDX9BufferInfo::GetRefCount() const
{
    return m_uiRefCount;
}
//---------------------------------------------------------------------------
inline void NiDX9DataStream::NiDX9BufferInfo::IncLockCount()
{
    NiAtomicIncrement(m_uiDX9LockCount);
}
//---------------------------------------------------------------------------
inline void NiDX9DataStream::NiDX9BufferInfo::DecLockCount()
{
    NIASSERT(m_uiDX9LockCount > 0 && "DecLockCount called when lockcount ==0");
    NiAtomicDecrement(m_uiDX9LockCount);
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9DataStream::NiDX9BufferInfo::GetLockCount() const
{
    return m_uiDX9LockCount;
}
//---------------------------------------------------------------------------
// NiDataStream overrides
//---------------------------------------------------------------------------
NiDX9DataStream::NiDX9DataStream(
    const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    Usage eUsage) : 
    NiDataStream(kElements, uiCount, uiAccessMask, eUsage), 
    m_pucLocalBufferCopy(NULL), 
    m_pkDX9BufferInfo(NULL), 
    m_uiDX9UsageFlags(0), 
    m_uiD3DBufferOffset(0), 
    m_bDirty(true)
{
    ValidateAccessMask();
    Allocate();
}
//---------------------------------------------------------------------------
NiDX9DataStream::NiDX9DataStream(
    NiUInt8 uiAccessMask, 
    Usage eUsage) : 
    NiDataStream(uiAccessMask, eUsage), 
    m_pucLocalBufferCopy(NULL), 
    m_pkDX9BufferInfo(NULL), 
    m_uiDX9UsageFlags(0), 
    m_uiD3DBufferOffset(0), 
    m_bDirty(true)
{
    ValidateAccessMask();
}
//---------------------------------------------------------------------------
NiDX9DataStream::~NiDX9DataStream()
{
    Deallocate();
}
//---------------------------------------------------------------------------
void NiDX9DataStream::Resize(NiUInt32 uiSize)
{
    Deallocate();

    m_uiSize = uiSize;

    Allocate();
}
//---------------------------------------------------------------------------
D3DPOOL NiDX9DataStream::DetermineD3DPool()
{
    if (m_uiAccessMask & ACCESS_CPU_WRITE_VOLATILE)
        return D3DPOOL_DEFAULT;

    return D3DPOOL_MANAGED;
}
//---------------------------------------------------------------------------
NiUInt32 NiDX9DataStream::GetD3DOffset()
{
    return m_uiD3DBufferOffset;
}
//---------------------------------------------------------------------------
NiUInt32 NiDX9DataStream::GetDX9UsageFlags() const
{
    return m_uiDX9UsageFlags;
}
//---------------------------------------------------------------------------
void NiDX9DataStream::Allocate()
{
    bool bAlreadyAllocated = m_pkDX9BufferInfo != NULL;
    NIASSERT(!bAlreadyAllocated);
    if (bAlreadyAllocated)
        return;

    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);
    // DISPLAYLIST and SHADERCONSTANT usages should be reported as USER here
    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX ||
        eUsage == NiDataStream::USAGE_USER);

    // Don't create D3D buffers during allocation - only just before
    // they're used (or when precached).  (all usages have a backing store
    // that behaves as if it were usage user)
    switch(eUsage)
    {
    case NiDataStream::USAGE_VERTEX:
        m_pucLocalBufferCopy = (NiUInt8*)NiMalloc2(m_uiSize * sizeof(NiUInt8),
            NiMemHint::USAGE_VERTEX | NiMemHint::IS_BULK);
        break;
    case NiDataStream::USAGE_VERTEX_INDEX:
        m_pucLocalBufferCopy = (NiUInt8*)NiMalloc2(m_uiSize * sizeof(NiUInt8),
            NiMemHint::USAGE_INDEX | NiMemHint::IS_BULK);
        break;
    case NiDataStream::USAGE_USER:
        // User buffers are just system-memory buffers.
        m_pucLocalBufferCopy = 
            (NiUInt8*)NiMalloc2(m_uiSize * sizeof(NiUInt8), NiMemHint::IS_BULK);
        break;
    default:
        NIASSERT(0 && "invalid eUsage");
    }

    if (!m_pucLocalBufferCopy)
    {
        NiDX9Renderer::Error(__FUNCTION__ 
            " failed to create a local buffer copy\n");
    }

    // If the associated DX9 buffer uses the Default pool inform the renderer
    // of the buffer. The renderer needs to be aware of all D3DPOOL_DEFAULT
    // buffers so they can be correctly managed in the event of a lost device.
    if (DetermineD3DPool() == D3DPOOL_DEFAULT &&
        (eUsage == NiDataStream::USAGE_VERTEX || 
        eUsage == NiDataStream::USAGE_VERTEX_INDEX))
    {
        NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
        NIASSERT(pkRenderer);
        pkRenderer->AddD3DDefaultBuffer(this);
    }
    
}
//---------------------------------------------------------------------------
void NiDX9DataStream::Deallocate()
{
    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);
    // DISPLAYLIST and SHADERCONSTANT usages should be reported as USER here
    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX ||
        eUsage == NiDataStream::USAGE_USER);

    // If the associated DX9 buffer uses the Default pool remove it from the 
    // renderer's list of active default buffers.
    if (DetermineD3DPool() == D3DPOOL_DEFAULT &&
        (eUsage == NiDataStream::USAGE_VERTEX || 
        eUsage == NiDataStream::USAGE_VERTEX_INDEX))
    {
        NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
        NIASSERT(pkRenderer);
        if (pkRenderer)
            pkRenderer->RemoveD3DDefaultBuffer(this);
    }

    switch(eUsage)
    {
    case NiDataStream::USAGE_VERTEX:
    case NiDataStream::USAGE_VERTEX_INDEX:
        // Deallocate DX9 buffers.
        DX9Deallocate();
        // Fall through...
    case NiDataStream::USAGE_USER:
        if (m_pucLocalBufferCopy)
            NiFree(m_pucLocalBufferCopy);
        m_pucLocalBufferCopy = NULL;
        break;
    default:
        NIASSERT(0 && "invalid usage");
    }
}
//---------------------------------------------------------------------------
void NiDX9DataStream::ValidateAccessMask()
{
    // Vertex and index buffers must have the GPU_ACCESS_READ flag set,
    // while other buffers must not have that flag set.
    if (m_eUsage == USAGE_VERTEX ||
        m_eUsage == USAGE_VERTEX_INDEX)
    {
        m_uiAccessMask |= ACCESS_GPU_READ;
    }
    else
    {
        m_uiAccessMask &= ~ACCESS_GPU_READ;
    }
}
//---------------------------------------------------------------------------
void NiDX9DataStream::AcquireDX9BufferInfo()
{
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);
    // DISPLAYLIST and SHADERCONSTANT usages should be reported as USER here
    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX ||
        eUsage == NiDataStream::USAGE_USER);

    if (m_uiDX9UsageFlags & D3DUSAGE_DYNAMIC)
    {
        NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
            eUsage == NiDataStream::USAGE_VERTEX_INDEX);

        if (eUsage == NiDataStream::USAGE_VERTEX)
            m_pkDX9BufferInfo = pkRenderer->GetSharedVertexBuffer(GetStride());
        else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
            m_pkDX9BufferInfo = pkRenderer->GetSharedIndexBuffer(GetStride());

        bool bCanShareABuffer = m_uiSize < ms_uiSharedBufferDefaultSize;

        if (// Current shared buffer not large enough to fit previous buffer
            m_pkDX9BufferInfo && 
            m_uiSize > m_pkDX9BufferInfo->m_uiUnusedSize 
            &&
            // and the new buffer we make will be shareable
            bCanShareABuffer)
        {
            // Current Vertex buffer not large enough. 
            // No longer consider it for sharing.
            if (eUsage == NiDataStream::USAGE_VERTEX)
                pkRenderer->RemoveSharedVertexBuffer(GetStride());
            else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
                pkRenderer->RemoveSharedIndexBuffer(GetStride());

            // Fall through, we will create a new one.
            m_pkDX9BufferInfo = NULL;
        }
        else if (// We can fit into the current shared buffer
            m_pkDX9BufferInfo && 
            m_uiSize <= m_pkDX9BufferInfo->m_uiUnusedSize)
        {
            // A buffer exists with enough unused space to store the data.
            m_pkDX9BufferInfo->m_bShared = true;
            m_pkDX9BufferInfo->IncRefCount();
            m_pkDX9BufferInfo->m_uiUnusedSize -= m_uiSize;
        }

        if (!m_pkDX9BufferInfo)
        {
            m_pkDX9BufferInfo = NiNew NiDX9DataStream::NiDX9BufferInfo();
            m_pkDX9BufferInfo->IncRefCount();
            m_pkDX9BufferInfo->m_uiCurrentIndex = 0;

            if (bCanShareABuffer)
                m_pkDX9BufferInfo->m_uiSize = ms_uiSharedBufferDefaultSize;
            else
                m_pkDX9BufferInfo->m_uiSize = m_uiSize;

            m_pkDX9BufferInfo->m_uiUnusedSize = 
                m_pkDX9BufferInfo->m_uiSize - m_uiSize;

            m_pkDX9BufferInfo->m_pkVBBuffer = NULL;

            if (bCanShareABuffer)
            {
                if (eUsage == NiDataStream::USAGE_VERTEX)
                {
                    pkRenderer->AddSharedVertexBuffer(GetStride(),
                        m_pkDX9BufferInfo);
                }
                else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
                {
                    pkRenderer->AddSharedIndexBuffer(GetStride(),
                        m_pkDX9BufferInfo);
                }
            }
        }

    }
    else
    {
        m_pkDX9BufferInfo = NiNew NiDX9DataStream::NiDX9BufferInfo();
        m_pkDX9BufferInfo->IncRefCount();
        m_pkDX9BufferInfo->m_uiCurrentIndex = 0;
        m_pkDX9BufferInfo->m_uiSize = m_uiSize;
        m_pkDX9BufferInfo->m_pkVBBuffer = NULL;
        m_uiD3DBufferOffset = 0;
    }
}
//---------------------------------------------------------------------------
void NiDX9DataStream::DX9Allocate()
{
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NIASSERT(pkRenderer);

    // This can only called from the rendering thread
    NIASSERT(NiGetCurrentThreadId() == pkRenderer->GetDeviceThreadID());

    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);
    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX);

    bool bAlreadyAllocated = (m_pkDX9BufferInfo != NULL);
    NIASSERT(!bAlreadyAllocated);
    if (bAlreadyAllocated)
        return;

    D3DPOOL ePool = DetermineD3DPool();

    // DX9 Buffers should always have a WRITEONLY usage. This is valid since 
    // all reading of the buffers is performed with the local CPU copy.
    m_uiDX9UsageFlags = D3DUSAGE_WRITEONLY;

    if (m_uiAccessMask & ACCESS_CPU_WRITE_VOLATILE)
        m_uiDX9UsageFlags |= D3DUSAGE_DYNAMIC;
    
    LPDIRECT3DDEVICE9 pkDevice = pkRenderer->GetD3DDevice();
    NIASSERT(pkDevice);

    if (eUsage == NiDataStream::USAGE_VERTEX)
    {
        NIASSERT(m_pkDX9BufferInfo == NULL);

        pkRenderer->AquireSharedBufferLock();
        AcquireDX9BufferInfo();

        if (!m_pkDX9BufferInfo->m_pkVBBuffer)
        {
            HRESULT hr = pkDevice->CreateVertexBuffer(
                m_pkDX9BufferInfo->m_uiSize, 
                m_uiDX9UsageFlags, 
                0, //uiFVF
                ePool, 
                &m_pkDX9BufferInfo->m_pkVBBuffer,
                NULL);

            if (SUCCEEDED(hr))
            {
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(
                    VERTEX_BUFFER_SIZE, 
                    m_uiSize);
            }
            else
            {
                NiDX9Renderer::Error(
                    "%s failed to create vertex buffer\n    %s", 
                    __FUNCTION__, 
                    NiDX9ErrorString((unsigned int)hr));
                NiDelete m_pkDX9BufferInfo;
                m_pkDX9BufferInfo = NULL;
            }

        }
        pkRenderer->ReleaseSharedBufferLock();
    }
    else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
    {
        NIASSERT(m_pkDX9BufferInfo == NULL);

        // Index buffers must contain only index data
        NIASSERT(m_kElements.GetSize() == 1); 
        NiDataStreamElement::Format kFormat = 
            m_kElements.GetAt(0).GetFormat();

        D3DFORMAT kD3DFormat = D3DFMT_INDEX32;
        switch(NiDataStreamElement::SizeOfFormat(kFormat))
        {
        case 2: kD3DFormat = D3DFMT_INDEX16; break;
        case 4: kD3DFormat = D3DFMT_INDEX32; break;
        default:
            NiRenderer::Warning(
                "%s can not create index buffer, unknown format for "
                "index buffer",
                __FUNCTION__ );
            return;
        }

        pkRenderer->AquireSharedBufferLock();
        AcquireDX9BufferInfo();

        if (!m_pkDX9BufferInfo->m_pkIBBuffer)
        {
            HRESULT hr = pkDevice->CreateIndexBuffer(
                m_pkDX9BufferInfo->m_uiSize, 
                m_uiDX9UsageFlags, 
                kD3DFormat, 
                ePool, 
                &m_pkDX9BufferInfo->m_pkIBBuffer,
                NULL);

            if (SUCCEEDED(hr))
            {
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(
                    INDEX_BUFFER_SIZE, 
                    m_uiSize);
            }
            else
            {
                NiDX9Renderer::Error( 
                    "%s failed to create a DX9 index buffer\n",
                    __FUNCTION__);

                NiDelete m_pkDX9BufferInfo;
                m_pkDX9BufferInfo = NULL;
            }
        }
        pkRenderer->ReleaseSharedBufferLock();
    }
}
//---------------------------------------------------------------------------
void NiDX9DataStream::DX9Deallocate()
{
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    if (pkRenderer == NULL)
    {
         NILOG("Destroying a DataStream without a renderer.\n");
    }

    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);

    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX);

    if (eUsage == NiDataStream::USAGE_VERTEX)
    {
        if (m_pkDX9BufferInfo)
        {
            NIASSERT(m_pkDX9BufferInfo->m_pkVBBuffer);
            m_pkDX9BufferInfo->DecRefCount();

            if (m_pkDX9BufferInfo->GetRefCount() == 0)
            {              
                NiDX9Renderer::ReleaseVBResource(
                    m_pkDX9BufferInfo->m_pkVBBuffer);
                m_pkDX9BufferInfo->m_pkVBBuffer = NULL;

                // Need to check the shared VB list even if this buffer
                // isn't explicitly shared, since it might be on the list
                // because it's available to be shared.
                NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
                if (pkRenderer)
                {
                    pkRenderer->AquireSharedBufferLock();
                    if (m_pkDX9BufferInfo == 
                        pkRenderer->GetSharedVertexBuffer(GetStride()))
                    {
                        pkRenderer->RemoveSharedVertexBuffer(GetStride());
                    }

                    pkRenderer->ReleaseSharedBufferLock();
                }
                else
                {
                    NILOG("Destroying a vertex buffer without a "
                        "renderer.\n");
                }

                NiDelete m_pkDX9BufferInfo;
            }
            else if (m_pkDX9BufferInfo->m_bShared)
            {
                m_pkDX9BufferInfo->m_uiUnusedSize += m_uiSize;
            }
            m_pkDX9BufferInfo = NULL;
        }
    }
    else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
    {
        if (m_pkDX9BufferInfo)
        {
            NIASSERT(m_pkDX9BufferInfo->m_pkIBBuffer);
            m_pkDX9BufferInfo->DecRefCount();

            if (m_pkDX9BufferInfo->GetRefCount() == 0)
            {              
                NiDX9Renderer::ReleaseIBResource(
                    m_pkDX9BufferInfo->m_pkIBBuffer);
                m_pkDX9BufferInfo->m_pkIBBuffer = NULL;

                NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
                if (pkRenderer)
                {
                    pkRenderer->AquireSharedBufferLock();
                    if (m_pkDX9BufferInfo == 
                        pkRenderer->GetSharedIndexBuffer(GetStride()))
                    {
                        pkRenderer->RemoveSharedIndexBuffer(GetStride());
                    }

                    pkRenderer->ReleaseSharedBufferLock();
                }
                else
                {
                    NILOG("Destroying an index buffer without a "
                        "renderer.\n");
                }

                NiDelete m_pkDX9BufferInfo;
            }
            else if (m_pkDX9BufferInfo->m_bShared)
            {
                m_pkDX9BufferInfo->m_uiUnusedSize += m_uiSize;
            }
            m_pkDX9BufferInfo = NULL;
        }
    }
}
//---------------------------------------------------------------------------
void NiDX9DataStream::LostDevice()
{
    NIASSERT(DetermineD3DPool() != D3DPOOL_MANAGED);
    DX9Deallocate();
}
//---------------------------------------------------------------------------
void NiDX9DataStream::Recreate()
{
    NIASSERT(DetermineD3DPool() != D3DPOOL_MANAGED);

    // This can only called from the rendering thread
    NIASSERT(NiDX9Renderer::GetRenderer() != NULL &&
        NiGetCurrentThreadId() == 
        NiDX9Renderer::GetRenderer()->GetDeviceThreadID());

    // Destroy previously owned DX9 buffer and create a new one.
    DX9Deallocate();
    DX9Allocate();

    if ((m_uiAccessMask & ACCESS_CPU_WRITE_VOLATILE) == 0)
    {
        if (!m_pucLocalBufferCopy)
        {
            NiOutputDebugString("Warning: Recreating a non-volatile DX9 "
                "buffer without a CPU copy. Contents of the buffer will be "
                "undefined.\n");
            return;
        }

        // Copy data from local CPU copy to newly created DX9 buffer.
        void* pvData = DX9Lock(LOCK_WRITE);
        if (pvData)
            NiMemcpy(pvData, m_pucLocalBufferCopy, m_uiSize);
        DX9Unlock();
    }
}
//---------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER9 NiDX9DataStream::GetVertexBuffer() const
{
    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);

    switch(eUsage)
    {
    case NiDataStream::USAGE_VERTEX:
        {
            if (m_pkDX9BufferInfo)
                return m_pkDX9BufferInfo->m_pkVBBuffer;
        }
        break;
    default:
        {
            NIASSERT(!"Incorrect Buffer Type");
            return NULL;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
LPDIRECT3DINDEXBUFFER9 NiDX9DataStream::GetIndexBuffer() const
{
    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);

    switch(eUsage)
    {
    case NiDataStream::USAGE_VERTEX_INDEX:
        {
            if (m_pkDX9BufferInfo)
                return m_pkDX9BufferInfo->m_pkIBBuffer;
        }
        break;
    default:
        {
            NIASSERT(!"Incorrect Buffer Type");
            return NULL;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiDX9DataStream::UpdateD3DBuffers(bool bReleaseSystemMemory)
{
    // Only want to update GPU-readable buffers
    NIASSERT((GetAccessMask() & NiDataStream::ACCESS_GPU_READ) != 0);

    // Volatile buffers are always dirty.
    if (m_bDirty || 
        (GetAccessMask() & NiDataStream::ACCESS_CPU_WRITE_VOLATILE) != 0)
    {
        // Allocate buffer, if necessary
        if (m_pkDX9BufferInfo == NULL)
            DX9Allocate();

        // Update VB from system buffer
        NIASSERT(m_pucLocalBufferCopy);
        void* pvData = DX9Lock(LOCK_WRITE);
        if (pvData)
            NiMemcpy(pvData, m_pucLocalBufferCopy, m_uiSize);
        DX9Unlock();

        m_bDirty = false;
    }
    NIASSERT(m_pkDX9BufferInfo);

    // Can only release system memory if CPU_READ mask is not set and
    // data stream is static
    if (bReleaseSystemMemory &&
        (GetAccessMask() & NiDataStream::ACCESS_CPU_READ) == 0 && 
        (GetAccessMask() & NiDataStream::ACCESS_CPU_WRITE_STATIC) != 0)
    {
        if (m_pucLocalBufferCopy)
            NiFree(m_pucLocalBufferCopy);
        m_pucLocalBufferCopy = NULL;
    }
}
//---------------------------------------------------------------------------
void* NiDX9DataStream::DX9Lock(NiUInt8 uiLockType)
{
    NI_UNUSED_ARG(uiLockType);
    if (!m_pkDX9BufferInfo)
        return NULL;

    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);

    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX); 

    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    NIASSERT(pkRenderer);

    void* pvData = NULL;
    DWORD uiFlags = 0;

    // We should only ever need to lock the DX9 buffer for write. 
    // All LOCK_READ only locks should be using the local CPU buffer 
    // and not need to actually touch the DX9 buffer.
    //
    // Note: If this assert is removed and it becomes valid to need a LOCK_READ
    // be sure to check for this case and use the D3DLOCK_READONLY flag.
    NIASSERT(uiLockType & LOCK_WRITE);

    bool bForceDiscard = 
        m_pkDX9BufferInfo->m_uiLockFrameID != pkRenderer->GetFrameID();
    m_pkDX9BufferInfo->m_uiLockFrameID = pkRenderer->GetFrameID();

    // If the DX9 buffer(must be shared) is already locked do not re-lock it 
    // as this will result in a busy-wait. Instead we used the already acquired
    // pointer to the locked buffer and add the buffer offset to it.
    if (m_pkDX9BufferInfo->GetLockCount() > 0)
    {
        m_pkDX9BufferInfo->IncLockCount();

        if (m_pkDX9BufferInfo->m_bShared)
        {
            if (m_pkDX9BufferInfo->m_uiCurrentIndex + m_uiSize > 
                m_pkDX9BufferInfo->m_uiSize)
            {
                m_pkDX9BufferInfo->m_uiCurrentIndex = m_uiSize;
                m_uiD3DBufferOffset = 0;
            }
            else
            {
                m_uiD3DBufferOffset = m_pkDX9BufferInfo->m_uiCurrentIndex;
                m_pkDX9BufferInfo->m_uiCurrentIndex += m_uiSize;
            }
        }
        NIASSERT((NiUInt8*)m_pkDX9BufferInfo->m_pvLockedData + 
            m_uiD3DBufferOffset);
        return (NiUInt8*)m_pkDX9BufferInfo->m_pvLockedData + 
            m_uiD3DBufferOffset;
    }
    
    if (eUsage == NiDataStream::USAGE_VERTEX)
    {
        NIASSERT(m_pkDX9BufferInfo && m_pkDX9BufferInfo->m_pkVBBuffer);

        if (m_uiDX9UsageFlags & D3DUSAGE_DYNAMIC)
        {              

            if (!m_pkDX9BufferInfo->m_bShared || bForceDiscard ||
                m_pkDX9BufferInfo->m_uiCurrentIndex == 0 ||
                m_pkDX9BufferInfo->m_uiCurrentIndex + m_uiSize >
                m_pkDX9BufferInfo->m_uiSize)
            {
                uiFlags |= D3DLOCK_DISCARD;
                m_pkDX9BufferInfo->m_uiCurrentIndex = m_uiSize;
                m_uiD3DBufferOffset = 0;
            }
            else
            {
                m_uiD3DBufferOffset = m_pkDX9BufferInfo->m_uiCurrentIndex;
                m_pkDX9BufferInfo->m_uiCurrentIndex += m_uiSize;
                uiFlags |= D3DLOCK_NOOVERWRITE;
            }
        }
        NIASSERT(m_pkDX9BufferInfo->GetLockCount() == 0);

        // Buffer is not in use, so we can lock without blocking.
        HRESULT eD3dRet = m_pkDX9BufferInfo->m_pkVBBuffer->Lock(0, 0, &pvData, 
            uiFlags);

        if (FAILED(eD3dRet))
        {
            NiDX9Renderer::Error("NiDX9Renderer::LockImpl: Failed to lock "
                "vertex buffer.\n");
            return NULL;
        }

        m_pkDX9BufferInfo->IncLockCount();
        m_pkDX9BufferInfo->m_pvLockedData = pvData;
    }
    else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
    {
        NIASSERT(m_pkDX9BufferInfo && m_pkDX9BufferInfo->m_pkIBBuffer);

        if (m_uiDX9UsageFlags & D3DUSAGE_DYNAMIC)
            uiFlags |= D3DLOCK_DISCARD;

        NIASSERT(m_pkDX9BufferInfo->GetLockCount() == 0);

        HRESULT eD3dRet = m_pkDX9BufferInfo->m_pkIBBuffer->Lock(0, 0, &pvData, 
            uiFlags);

        if (FAILED(eD3dRet))
        {
            NiDX9Renderer::Error("NiDX9Renderer::LockImpl: Failed to lock "
                "index buffer.\n");
            return NULL;
        }

        m_pkDX9BufferInfo->IncLockCount();
        m_pkDX9BufferInfo->m_pvLockedData = pvData;
    }

    NIASSERT((NiUInt8*)pvData + m_uiD3DBufferOffset);
    return (NiUInt8*)pvData + m_uiD3DBufferOffset;
}
//---------------------------------------------------------------------------
void NiDX9DataStream::DX9Unlock()
{
    if (!m_pkDX9BufferInfo)
        return;

    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);

    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX);

    if (m_pkDX9BufferInfo->GetLockCount() > 1)
    {
        m_pkDX9BufferInfo->DecLockCount();
        return;
    }

    if (eUsage == NiDataStream::USAGE_VERTEX)
    {
        NIASSERT(m_pkDX9BufferInfo && m_pkDX9BufferInfo->m_pkVBBuffer);
        NIASSERT(m_pkDX9BufferInfo->GetLockCount() == 1);
        m_pkDX9BufferInfo->m_pkVBBuffer->Unlock();
        m_pkDX9BufferInfo->m_pvLockedData = NULL;
        m_pkDX9BufferInfo->DecLockCount();
    }
    else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
    {
        NIASSERT(m_pkDX9BufferInfo && m_pkDX9BufferInfo->m_pkIBBuffer);
        NIASSERT(m_pkDX9BufferInfo->GetLockCount() == 1);
        m_pkDX9BufferInfo->m_pkIBBuffer->Unlock();
        m_pkDX9BufferInfo->m_pvLockedData = NULL;
        m_pkDX9BufferInfo->DecLockCount();
    }

}
//---------------------------------------------------------------------------
void* NiDX9DataStream::MapBuffer(NiUInt8 uiLockType, 
    NiUInt32, NiUInt32)
{ 
    // If allocate failed, the code below will fall through safely
    // but return NULL pointer values

    void* pvData = NULL;

    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);
    // DISPLAYLIST and SHADERCONSTANT usages should be reported as USER here
    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX ||
        eUsage == NiDataStream::USAGE_USER);

    if (eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX || 
        eUsage == NiDataStream::USAGE_USER)
    {
        if (m_pucLocalBufferCopy)
        {
            pvData = m_pucLocalBufferCopy;
        }
        else if ((uiLockType & (LOCK_TOOL_READ | LOCK_TOOL_WRITE)) != 0)
        {
            // A lock type of TOOL_READ or TOOL_WRITE can force this path.
            // A D3D buffer must exist at this point; lock it as necessary.
            // Don't worry about concurrent locks
            NIASSERT(m_pkDX9BufferInfo && 
                m_pkDX9BufferInfo->m_pvLockedData == NULL);
            NIASSERT(eUsage != NiDataStream::USAGE_USER);
            if (eUsage == NiDataStream::USAGE_VERTEX)
            {
                NIASSERT(m_pkDX9BufferInfo->m_pkVBBuffer);
                NiUInt32 uiFlags = 0;
                if ((uiLockType & (LOCK_WRITE | LOCK_TOOL_WRITE)) == 0)
                    uiFlags |= D3DLOCK_READONLY;
                HRESULT hr = 
                    m_pkDX9BufferInfo->m_pkVBBuffer->Lock(
                    0, 
                    0, 
                    &pvData, 
                    uiFlags);
                NIASSERT(SUCCEEDED(hr));
            }
            else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
            {
                NIASSERT(m_pkDX9BufferInfo->m_pkIBBuffer);
                NiUInt32 uiFlags = 0;
                if ((uiLockType & (LOCK_WRITE | LOCK_TOOL_WRITE)) == 0)
                    uiFlags |= D3DLOCK_READONLY;
                HRESULT hr = 
                    m_pkDX9BufferInfo->m_pkIBBuffer->Lock(
                    0, 
                    0, 
                    &pvData, 
                    uiFlags);
                NIASSERT(SUCCEEDED(hr));
            }
        }
        else
        {
            NIASSERT(false && 
                "Invalid lock mask specified for DataStream::LockImpl.");
        }
    }
    else
    {
        NIASSERT(false && "Invalid usage specified for DataStream::LockImpl.");
    }

    return pvData;
}
//---------------------------------------------------------------------------
void NiDX9DataStream::SetAccessMask(NiUInt8 uiAccessMask)
{
    m_uiAccessMask = uiAccessMask;
}
//---------------------------------------------------------------------------
void NiDX9DataStream::UnmapBuffer(NiUInt8 uiLockType, 
    NiUInt32, NiUInt32)
{
    Usage eUsage = GetUsage_Treat_Non_ACCESS_GPU_READ_As_USER(this);
    // DISPLAYLIST and SHADERCONSTANT usages should be reported as USER here
    NIASSERT(eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX ||
        eUsage == NiDataStream::USAGE_USER);

    if (eUsage == NiDataStream::USAGE_VERTEX ||
        eUsage == NiDataStream::USAGE_VERTEX_INDEX)
    {
        if (m_pucLocalBufferCopy)
        {
            // Check for Read Only
            if ((uiLockType & (LOCK_WRITE | LOCK_TOOL_WRITE)) == 0)
                return;

            m_bDirty = true;
        }
        else if ((uiLockType & (LOCK_TOOL_READ | LOCK_TOOL_WRITE)) != 0)
        {
            // A lock type of TOOL_READ or TOOL_WRITE can force this path.
            // A D3D buffer must exist at this point; lock it as necessary.
            // Don't worry about concurrent locks
            NIASSERT(m_pkDX9BufferInfo && 
                m_pkDX9BufferInfo->m_pvLockedData == NULL);
            NIASSERT(eUsage != NiDataStream::USAGE_USER);
            if (eUsage == NiDataStream::USAGE_VERTEX)
            {
                NIASSERT(m_pkDX9BufferInfo->m_pkVBBuffer);
                HRESULT hr = 
                    m_pkDX9BufferInfo->m_pkVBBuffer->Unlock();
                NIASSERT(SUCCEEDED(hr));
            }
            else if (eUsage == NiDataStream::USAGE_VERTEX_INDEX)
            {
                NIASSERT(m_pkDX9BufferInfo->m_pkIBBuffer);
                HRESULT hr = 
                    m_pkDX9BufferInfo->m_pkIBBuffer->Unlock();
                NIASSERT(SUCCEEDED(hr));
            }
        }
        else
        {
            NIASSERT(false && 
                "Invalid lock mask specified for DataStream::LockImpl.");
        }

    }
    else if ((eUsage != NiDataStream::USAGE_USER))
    {
        NIASSERT(false && "Invalid usage specified for "
            "DataStream::UnlockImpl.");
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
template <class LockPolicy>
NiDX9LockableDataStream<LockPolicy>::NiDX9LockableDataStream(
    const NiDataStreamElementSet& kElements, NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, Usage eUsage) :
    NiDX9DataStream(kElements, uiCount, uiAccessMask, eUsage)
{
}
//---------------------------------------------------------------------------
template <class LockPolicy>
NiDX9LockableDataStream<LockPolicy>::NiDX9LockableDataStream(
    NiUInt8 uiAccessMask, Usage eUsage)
    : NiDX9DataStream(uiAccessMask, eUsage)
{
}
//---------------------------------------------------------------------------
template <class LockPolicy>
NiDX9LockableDataStream<LockPolicy>::~NiDX9LockableDataStream()
{
    if (GetLocked())
    {
        NILOG("Warning: Destroying locked NiDX9DataStream.\n");
    }

    m_kLockPolicy.Delete(this);
}
//---------------------------------------------------------------------------
template <class LockPolicy>
void* NiDX9LockableDataStream<LockPolicy>::LockImpl(NiUInt8 uiLockMask)
{
    bool bForceRead = (uiLockMask & LOCK_TOOL_READ) != 0;
    bool bForceWrite = (uiLockMask & LOCK_TOOL_WRITE) != 0;
    bool bRead = (uiLockMask & LOCK_READ) != 0;
    bool bWrite = (uiLockMask & LOCK_WRITE) != 0;

    if (bRead || bForceRead)
    {
        if (bWrite || bForceWrite)
            return m_kLockPolicy.LockReadWrite(this, bForceRead, bForceWrite);
        else
            return m_kLockPolicy.LockRead(this, bForceRead);
    }
    else// if (bWrite || bForceWrite)
    {
        NIASSERT(bWrite || bForceWrite);
        return m_kLockPolicy.LockWrite(this, bForceWrite);
    }
}
//---------------------------------------------------------------------------
template <class LockPolicy>
void NiDX9LockableDataStream<LockPolicy>::UnlockImpl(NiUInt8 uiLockType)
{
    bool bForceRead = (uiLockType & LOCK_TOOL_READ) != 0;
    bool bForceWrite = (uiLockType & LOCK_TOOL_WRITE) != 0;
    bool bRead = (uiLockType & LOCK_READ) != 0;
    bool bWrite = (uiLockType & LOCK_WRITE) != 0;

    if (bRead || bForceRead)
    {
        if (bWrite || bForceWrite)
            return m_kLockPolicy.UnlockReadWrite(this, bForceRead, bForceWrite);
        else
            return m_kLockPolicy.UnlockRead(this, bForceRead);
    }
    else// if (bWrite || bForceWrite)
    {
        NIASSERT(bWrite || bForceWrite);
        return m_kLockPolicy.UnlockWrite(this, bForceWrite);
    }
}
//---------------------------------------------------------------------------
template class 
    NiDX9LockableDataStream<StaticLockPolicy<NiDX9DataStream> >;
template class 
    NiDX9LockableDataStream<MutableLockPolicy<NiDX9DataStream> >;
template class 
    NiDX9LockableDataStream<VolatileLockPolicy<NiDX9DataStream> >;
