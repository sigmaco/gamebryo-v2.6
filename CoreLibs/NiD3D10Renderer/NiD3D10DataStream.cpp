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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10DataStream.h"

#include "NiD3D10DataStreamFactory.h"
#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ResourceManager.h"

//---------------------------------------------------------------------------
NiD3D10DataStream::NiD3D10DataStream(const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, NiUInt8 uiAccessMask, Usage eUsage, 
    D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
    NiUInt32 uiCPUAccessFlags) :
    NiDataStream(kElements, uiCount, uiAccessMask, eUsage), 
    m_pkD3D10Buffer(NULL),
    m_pvBackingBuffer(NULL),
    m_eD3D10Usage(eD3D10Usage),
    m_uiD3D10BindFlags(uiBindFlags),
    m_uiD3D10CPUAccessFlags(uiCPUAccessFlags),
    m_bDirty(true),
    m_bHidingBufferInBackingBuffer(false)
{
    Allocate();
}
//---------------------------------------------------------------------------
NiD3D10DataStream::NiD3D10DataStream(NiUInt8 uiAccessMask, Usage eUsage, 
    D3D10_USAGE eD3D10Usage, NiUInt32 uiBindFlags, 
    NiUInt32 uiCPUAccessFlags) :
    NiDataStream(uiAccessMask, eUsage), 
    m_pkD3D10Buffer(NULL),
    m_pvBackingBuffer(NULL),
    m_eD3D10Usage(eD3D10Usage),
    m_uiD3D10BindFlags(uiBindFlags),
    m_uiD3D10CPUAccessFlags(uiCPUAccessFlags),
    m_bDirty(true),
    m_bHidingBufferInBackingBuffer(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10DataStream::~NiD3D10DataStream()
{
    Deallocate();
}
//---------------------------------------------------------------------------
NiD3D10DataStream* NiD3D10DataStream::Create(ID3D10Buffer* pkBuffer)
{
    NIASSERT(pkBuffer);
    pkBuffer->AddRef();

    D3D10_BUFFER_DESC kDesc;
    pkBuffer->GetDesc(&kDesc);

    Usage eUsage;
    NiUInt8 uiAccessMask;
    InterpretD3D10Parameters(kDesc.Usage, kDesc.BindFlags, 
        kDesc.CPUAccessFlags, eUsage, uiAccessMask);

    NiD3D10DataStream* pkDataStream = (NiD3D10DataStream*)
        CreateDataStream(uiAccessMask, eUsage, false, false);

    NIASSERT(pkDataStream->m_uiAccessMask == uiAccessMask);    
    NIASSERT(pkDataStream->m_eD3D10Usage == kDesc.Usage);
    NIASSERT(pkDataStream->m_uiD3D10BindFlags == kDesc.BindFlags);
    NIASSERT(pkDataStream->m_uiD3D10CPUAccessFlags == kDesc.CPUAccessFlags);

    pkDataStream->m_pkD3D10Buffer = pkBuffer;
    pkDataStream->m_uiSize = kDesc.ByteWidth;

    // Local buffers will be created for all Volatile and Mutable buffers, or 
    // for buffers that don't get D3D10 buffers.
    if (NeedsLocalBuffer(pkDataStream->m_uiAccessMask))
    {
        NIASSERT(pkDataStream->m_pvBackingBuffer == NULL && 
            !pkDataStream->m_bHidingBufferInBackingBuffer);
        pkDataStream->m_pvBackingBuffer = 
            NiAlloc(NiUInt8, pkDataStream->m_uiSize);
    }

    return pkDataStream;
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::Resize(NiUInt32 uiSize)
{
    Deallocate();

    m_uiSize = uiSize;

    Allocate();
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::Allocate()
{
    // Check for existence of buffer already
    if (m_pkD3D10Buffer != NULL || m_pvBackingBuffer != NULL)
        return;

    NIASSERT(NiD3D10Renderer::GetRenderer());

    // Assertions:
    // - USER buffers and DISPLAYLISTS have bind flags of 0.
    NIASSERT((GetUsage() != USAGE_USER && 
        GetUsage() != USAGE_DISPLAYLIST) || (m_uiD3D10BindFlags == 0));
    // - Anything with GPU_READ has a non-zero bind flag
    NIASSERT(((m_uiAccessMask & ACCESS_GPU_READ) == 0) || 
        (m_uiD3D10BindFlags != 0));
    // - USER buffers or DISPLAYLISTS
    //   are (CPU_READ | CPU_WRITE_STATIC) or 
    //   (CPU_READ | CPU_WRITE_MUTABLE).
    NIASSERT((GetUsage() != USAGE_USER && GetUsage() != USAGE_DISPLAYLIST) || 
        (m_uiAccessMask == (ACCESS_CPU_READ | ACCESS_CPU_WRITE_STATIC) ||
        m_uiAccessMask == (ACCESS_CPU_READ | ACCESS_CPU_WRITE_MUTABLE)) || 
        (GetUsage() == USAGE_DISPLAYLIST));
    // - STAGING buffers have bind flags of 0
    //   and others have non-0 bind flags
    NIASSERT((m_eD3D10Usage == D3D10_USAGE_STAGING) == 
        (m_uiD3D10BindFlags == 0));

    // D3D10 buffers will not be created here - Only the CPU buffers.
    NIASSERT(m_pvBackingBuffer == NULL && !m_bHidingBufferInBackingBuffer);
    m_pvBackingBuffer = NiAlloc(NiUInt8, m_uiSize);
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::Deallocate()
{
    NIASSERT(!m_bHidingBufferInBackingBuffer);
    if (m_pvBackingBuffer)
    {
        NiFree(m_pvBackingBuffer);
        m_pvBackingBuffer = NULL;
    }

    if (m_pkD3D10Buffer)
    {
        m_pkD3D10Buffer->Release();
        m_pkD3D10Buffer = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::D3D10Allocate(bool bInitialize)
{
    NiD3D10ResourceManager* pkResourceManager = 
        NiD3D10Renderer::GetRenderer()->GetResourceManager();
    NIASSERT(pkResourceManager);

    // D3D10 buffers will be created for all buffers except CPU-only buffers, 
    // which have 0 bind flags.
    if (m_uiD3D10BindFlags == 0)
        return;

    // Check for already-allocated buffers
    if (m_pkD3D10Buffer)
        return;
 
    D3D10_SUBRESOURCE_DATA kData;
    if (bInitialize)
    {
        NIASSERT(!m_bHidingBufferInBackingBuffer);
        kData.pSysMem = m_pvBackingBuffer;

        // D3D10 docs recommendation:
        // If the resource being created does not require SysMemPitch 
        // or SysMemSlicePitch, then it may be useful to use those 
        // members when debugging by specifying the total size of the 
        // resource in bytes.
        kData.SysMemPitch = m_uiSize;
        kData.SysMemSlicePitch = m_uiSize;
    }

    m_pkD3D10Buffer = pkResourceManager->CreateBuffer(m_uiSize, 
        m_uiD3D10BindFlags, m_eD3D10Usage, m_uiD3D10CPUAccessFlags, 
        0, (bInitialize ? &kData : NULL));

    if (m_pkD3D10Buffer == NULL)
    {
        // Resource creation would have thrown error message
        NiD3D10Error::ReportWarning(
            "NiD3D10DataStream::D3D10Allocate failed because buffer "
            "could not be created.");
    }
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::InterpretDataStreamFlags(NiUInt8& uiAccessMask, 
    NiDataStream::Usage eUsage, D3D10_USAGE& eD3D10Usage, 
    NiUInt32& uiBindFlags, NiUInt32& uiCPUAccessFlags)
{
    eD3D10Usage = D3D10_USAGE_STAGING;
    uiCPUAccessFlags = 0;

    // Vertex, index, and constant buffers must have the GPU_ACCESS_READ flag 
    // set, while other buffers must not have that flag set.
    if (eUsage == USAGE_VERTEX ||
        eUsage == USAGE_VERTEX_INDEX ||
        eUsage == USAGE_SHADERCONSTANT)
    {
        uiAccessMask |= ACCESS_GPU_READ;
    }
    else
    {
        uiAccessMask &= ~ACCESS_GPU_READ;
    }

    // Interpret Usage parameter
    uiBindFlags = InterpretUsage(eUsage, uiAccessMask);

    // Check validity of access mask
    if (!InterpretAccessMask(uiAccessMask, eD3D10Usage))
        return false;

    // USER buffers are CPU-only, and cannot have GPU_READ set on them. 
    NIASSERT((eUsage != USAGE_USER) || 
        ((uiAccessMask & ACCESS_GPU_READ) == 0));

    // Other buffers may have GPU_READ set on them, but it is not required.
    // If they do have GPU_READ, then they must have non-0 bind flags.
    NIASSERT(((uiAccessMask & ACCESS_GPU_READ) != 0) == (uiBindFlags != 0));

    // STAGING will be returned for actual STAGING buffers or for local
    // CPU-only non-D3D10 buffers. The way to differentiate is whether the
    // access flags have any CPU_WRITE flags set - those are non-D3D10 buffers.

    if ((eD3D10Usage != D3D10_USAGE_STAGING) ||
        ((uiAccessMask & ACCESS_CPU_WRITE_ANY) == 0))
    {
        // GPU buffer
        NIASSERT(uiBindFlags != 0);

        // Get valid CPUAccess flags
        if (eD3D10Usage == D3D10_USAGE_DYNAMIC)
        {
            uiCPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
        }
        else if (eD3D10Usage == D3D10_USAGE_STAGING)
        {
            uiCPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
        }
    }
    else
    {
        // CPU buffer
        NIASSERT(uiBindFlags == 0);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10DataStream::InterpretAccessMask(NiUInt8 uiAccessMask, 
    D3D10_USAGE& eUsage)
{
    if ((uiAccessMask & ACCESS_GPU_READ) == 0)
    {
        // No GPU read - treat as STAGING or local buffer
        
        // Only certain combinations should have gotten through 
        // IsAccessRequestValid
        NIASSERT(uiAccessMask == ACCESS_CPU_READ ||
            uiAccessMask == (ACCESS_CPU_READ | ACCESS_CPU_WRITE_STATIC) ||
            uiAccessMask == (ACCESS_CPU_READ | ACCESS_CPU_WRITE_MUTABLE));

        eUsage = D3D10_USAGE_STAGING;
        return true;
    }

    if ((uiAccessMask & ACCESS_CPU_WRITE_VOLATILE) != 0)
    {
        // Volatile - treat as DYNAMIC buffer.

        // Only certain combinations should have gotten through 
        // IsAccessRequestValid
        NIASSERT(uiAccessMask == 
            (ACCESS_CPU_WRITE_VOLATILE | ACCESS_GPU_READ));

        eUsage = D3D10_USAGE_DYNAMIC;
        return true;
    }

    if ((uiAccessMask & ACCESS_CPU_WRITE_STATIC) != 0)
    {
        // Static - treat as IMMUTABLE buffer

        // Only certain combinations should have gotten through 
        // IsAccessRequestValid
        NIASSERT(uiAccessMask == (ACCESS_CPU_WRITE_STATIC | ACCESS_GPU_READ) ||
            uiAccessMask == 
            (ACCESS_CPU_READ | ACCESS_CPU_WRITE_STATIC | ACCESS_GPU_READ));

        eUsage = D3D10_USAGE_IMMUTABLE;
        return true;
    }

    // Mutable - treat as DEFAULT buffer.

    // Special case: certain combinations are not useful as there is no way to
    // fill them with data
    if (uiAccessMask == ACCESS_GPU_READ ||
        uiAccessMask == (ACCESS_CPU_READ | ACCESS_GPU_READ))
    {
        return false;
    }

    // Only certain combinations should have gotten through to this point
    NIASSERT(uiAccessMask == (ACCESS_GPU_READ | ACCESS_GPU_WRITE) ||
        uiAccessMask == 
        (ACCESS_CPU_WRITE_MUTABLE | ACCESS_GPU_READ | ACCESS_GPU_WRITE) ||
        uiAccessMask == (ACCESS_CPU_WRITE_MUTABLE | ACCESS_GPU_READ) ||
        uiAccessMask == 
        (ACCESS_CPU_READ | ACCESS_CPU_WRITE_MUTABLE | ACCESS_GPU_READ));

    eUsage = D3D10_USAGE_DEFAULT;
    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10DataStream::InterpretUsage(NiDataStream::Usage eUsage, 
    NiUInt32 uiAccessMask)
{
    NiUInt32 uiBindFlags = 0;
    if ((uiAccessMask & ACCESS_GPU_READ) != 0)
    {
        switch (eUsage)
        {
        case USAGE_VERTEX_INDEX:
            uiBindFlags = D3D10_BIND_INDEX_BUFFER;
            break;
        case USAGE_VERTEX:
            uiBindFlags = D3D10_BIND_VERTEX_BUFFER;
            break;
        case USAGE_SHADERCONSTANT:
            uiBindFlags = D3D10_BIND_CONSTANT_BUFFER;
            break;
        case USAGE_DISPLAYLIST: 
            // USAGE_DISPLAYLISTS can't be bound to pipeline.
        default:
            // USAGE_USER can't be bound to pipeline.
            break;
        }
    }

    if ((uiAccessMask & ACCESS_GPU_WRITE) != 0)
    {
        // Support for Stream Output:
        uiBindFlags |= D3D10_BIND_STREAM_OUTPUT;
        if ((uiAccessMask & ACCESS_GPU_READ) != 0)
            uiBindFlags |= D3D10_BIND_SHADER_RESOURCE;
    }

    return uiBindFlags;
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::InterpretD3D10Parameters(D3D10_USAGE eD3D10Usage, 
    NiUInt32 uiD3D10BindFlags, NiUInt32 uiD3D10CPUAccessFlags, 
    NiDataStream::Usage& eUsage, NiUInt8& uiAccessMask)
{
    if ((uiD3D10BindFlags & D3D10_BIND_INDEX_BUFFER) != 0)
    {
        eUsage = USAGE_VERTEX_INDEX;
    }
    else if ((uiD3D10BindFlags & D3D10_BIND_VERTEX_BUFFER) != 0)
    {
        eUsage = USAGE_VERTEX;
    }
    else if ((uiD3D10BindFlags & D3D10_BIND_CONSTANT_BUFFER) != 0)
    {
        eUsage = USAGE_SHADERCONSTANT;
    }
    else
    {
        eUsage = USAGE_USER;
    }

    if (eD3D10Usage == D3D10_USAGE_DYNAMIC)
    {
        uiAccessMask = ACCESS_GPU_READ | 
            ACCESS_CPU_WRITE_VOLATILE;
        // Dynamic buffers must not be readable.
        NIASSERT((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_READ) == 0);
        // Though it's not clear what it means for them to not be writable...
        NIASSERT((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_WRITE) != 0);
    }
    else if (eD3D10Usage == D3D10_USAGE_IMMUTABLE)
    {
        uiAccessMask = ACCESS_GPU_READ |
            ACCESS_CPU_WRITE_STATIC | 
            ACCESS_CPU_WRITE_STATIC_INITIALIZED;
        // IMMUTABLE buffers can't be readable or writable, but the 
        // ACCESS_CPU_WRITE_STATIC and ACCESS_CPU_WRITE_STATIC_INITIALIZED 
        // access flags are still set to indicate that the IMMUTABLE buffers 
        // have already been updated.
        NIASSERT((uiD3D10CPUAccessFlags & 
            (D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ)) == 0);
    }
    else if (eD3D10Usage == D3D10_USAGE_DEFAULT)
    {
        uiAccessMask = ACCESS_GPU_READ | 
            ACCESS_CPU_WRITE_MUTABLE;
        // Default buffers can't be readable or writable, but the 
        // ACCESS_CPU_WRITE_MUTABLE access flag is still set to indicate that 
        // Gamebryo will allow writes (through the subresource update).
        NIASSERT((uiD3D10CPUAccessFlags & 
            (D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ)) == 0);
    }
    else
    {
        NIASSERT(eD3D10Usage == D3D10_USAGE_STAGING);
        uiAccessMask = 0;
        // Staging buffers can be readable and/or writable, though I don't
        // know how useful it would be for one to be set but not the other.
        if ((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_WRITE) != 0)
            uiAccessMask |= ACCESS_CPU_WRITE_MUTABLE;
        if ((uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_READ) != 0)
            uiAccessMask |= ACCESS_CPU_READ;
    }

    // Support for Stream Output:
    if ((uiD3D10BindFlags & D3D10_BIND_SHADER_RESOURCE) != 0)
        uiAccessMask |= NiDataStream::ACCESS_GPU_READ;
    if ((uiD3D10BindFlags & D3D10_BIND_STREAM_OUTPUT) != 0)
    {
        uiAccessMask |= NiDataStream::ACCESS_GPU_READ;
        uiAccessMask |= NiDataStream::ACCESS_GPU_WRITE;
    }

    NIASSERT(uiD3D10BindFlags == InterpretUsage(eUsage, uiAccessMask));
}
//---------------------------------------------------------------------------
void* NiD3D10DataStream::MapBuffer(NiUInt8 uiLockMask, NiUInt32, NiUInt32)
{ 
    if (m_pvBackingBuffer != NULL)
    {
        return m_pvBackingBuffer;
    }
    else if ((uiLockMask & (LOCK_TOOL_READ | LOCK_TOOL_WRITE)) != 0)
    {
        // A lock type of TOOL_READ or TOOL_WRITE can force this path.
        // A D3D buffer must exist at this point; lock it as necessary.
        // Don't worry about concurrent locks
        NIASSERT(m_pkD3D10Buffer);

        // If there's no backing store, then either the buffer is DYNAMIC,
        // in which case it can be written to (but not read), or the buffer
        // has to be copied back to system memory.

        bool bReadDesired = (uiLockMask & (LOCK_READ | LOCK_TOOL_READ)) != 0;
        bool bWriteDesired = 
            (uiLockMask & (LOCK_WRITE | LOCK_TOOL_WRITE)) != 0;
        bool bBufferCanRead = 
            (m_uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_READ) != 0;
        bool bBufferCanWrite = 
            (m_uiD3D10CPUAccessFlags & D3D10_CPU_ACCESS_WRITE) != 0;

        D3D10_MAP eMap;
        if (bReadDesired)
        {
            if (bWriteDesired)
                eMap = D3D10_MAP_READ_WRITE;
            else
                eMap = D3D10_MAP_READ;
        }
        else
        {
            NIASSERT(bWriteDesired);
            eMap = D3D10_MAP_WRITE;
        }

        void* pvData = NULL;
        // Check to see if the existing buffer can be locked directly
        if ((!bReadDesired || bBufferCanRead) &&
            (!bWriteDesired || bBufferCanWrite))
        {
            HRESULT hr = m_pkD3D10Buffer->Map(eMap, 0, &pvData);
            NIASSERT(SUCCEEDED(hr));
            return pvData;
        }

        // Otherwise, must create a temporary resource and copy it over.
        // Must hide this resource in m_pvBackingBuffer.

        NiD3D10ResourceManager* pkResourceManager = 
            NiD3D10Renderer::GetRenderer()->GetResourceManager();
        NIASSERT(pkResourceManager);
        ID3D10Buffer* pkTempBuffer = pkResourceManager->CreateBuffer(
            m_uiSize, 0, D3D10_USAGE_STAGING, 
            D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE, 
            0, NULL);
        NIASSERT(pkTempBuffer);

        ID3D10Device* pkDevice = 
            NiD3D10Renderer::GetRenderer()->GetD3D10Device();
        NIASSERT(pkDevice);
        pkDevice->CopyResource(pkTempBuffer, m_pkD3D10Buffer);

        m_pvBackingBuffer = (void*)pkTempBuffer;
        m_bHidingBufferInBackingBuffer = true;

        pkTempBuffer->Map(eMap, 0, &pvData);

        return pvData;
    }
    else
    {
        NIASSERT(false && 
            "Invalid lock mask specified for DataStream::LockImpl.");
        return NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::UnmapBuffer(NiUInt8 uiLockType, 
    NiUInt32, NiUInt32)
{
    NIASSERT(m_pvBackingBuffer != NULL);

    bool bWrite = (uiLockType & (LOCK_WRITE | LOCK_TOOL_WRITE)) != 0;

    // Check for a forced lock
    if (m_bHidingBufferInBackingBuffer)
    {
        ID3D10Buffer* pkTempBuffer = (ID3D10Buffer*)m_pvBackingBuffer;
        m_pvBackingBuffer = NULL;
        m_bHidingBufferInBackingBuffer = false;

        pkTempBuffer->Unmap();

        // If writing data, upload it back to the buffer
        if (bWrite)
        {
            if (m_eD3D10Usage == D3D10_USAGE_IMMUTABLE)
            {
                // Must replace old buffer with new.
                NIASSERT(m_pkD3D10Buffer);
                m_pkD3D10Buffer->Release();
                m_pkD3D10Buffer = NULL;

                pkTempBuffer->Map(D3D10_MAP_READ, 0, &m_pvBackingBuffer);

                D3D10Allocate(true);

                pkTempBuffer->Unmap();
            }
            else
            {
                ID3D10Device* pkDevice = 
                    NiD3D10Renderer::GetRenderer()->GetD3D10Device();
                NIASSERT(pkDevice);

                pkDevice->CopyResource(m_pkD3D10Buffer, pkTempBuffer);
            }

        }

        pkTempBuffer->Release();
        return;
    }
    else if (bWrite)
    {
        // Check for a forced lock on an immutable buffer that has CPU Read
        // (and therefore has a valid m_pvBackingBuffer)
        if (m_eD3D10Usage == D3D10_USAGE_IMMUTABLE &&
            m_pkD3D10Buffer != NULL)
        {
            // Must release old buffer so it can be replaced.
            m_pkD3D10Buffer->Release();
            m_pkD3D10Buffer = NULL;
        }

        m_bDirty = true;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::UpdateD3D10Buffers(bool bReleaseSystemMemory)
{
    // Only want to update GPU-readable buffers
    NIASSERT((GetAccessMask() & NiDataStream::ACCESS_GPU_READ) != 0);

    if (m_bDirty)
    {
        NIASSERT(m_pvBackingBuffer && !m_bHidingBufferInBackingBuffer);
        // Allocate buffer, if necessary
        if (m_pkD3D10Buffer == NULL)
        {
            // This will also initialize the buffer with the data in 
            // m_pvBackingBuffer
            D3D10Allocate(true);
        }
        else
        {
            PerformD3D10BufferUpdate();
        }

        // Can only release system memory if CPU_READ mask is not set and
        // data stream is static
        if (bReleaseSystemMemory &&
            (GetAccessMask() & NiDataStream::ACCESS_CPU_READ) == 0 && 
            (GetAccessMask() & NiDataStream::ACCESS_CPU_WRITE_STATIC) != 0)
        {
            if (m_pvBackingBuffer)
                NiFree(m_pvBackingBuffer);
            m_pvBackingBuffer = NULL;
        }

        m_bDirty = false;
    }

    NIASSERT(m_pkD3D10Buffer);
}
//---------------------------------------------------------------------------
void NiD3D10DataStream::PerformD3D10BufferUpdate()
{
    NIASSERT(m_pkD3D10Buffer);

    // IMMUTABLE buffers cannot be updated.
    NIASSERT(m_eD3D10Usage != D3D10_USAGE_IMMUTABLE);

    NIASSERT(!m_bHidingBufferInBackingBuffer);
    if (m_eD3D10Usage == D3D10_USAGE_DEFAULT)
    {
        // DEFAULT buffers: Update using UpdateSubresource

        ID3D10Device* pkDevice = 
            NiD3D10Renderer::GetRenderer()->GetD3D10Device();
        NIASSERT(pkDevice);

        pkDevice->UpdateSubresource(m_pkD3D10Buffer, 0, NULL, 
            m_pvBackingBuffer, m_uiSize, m_uiSize);
    }
    else
    {
        // DYNAMIC and STAGING buffers: Update using Lock/update/unlock buffer

        D3D10_MAP eMapType;
        if (m_eD3D10Usage == D3D10_USAGE_DYNAMIC)
        {
            eMapType = D3D10_MAP_WRITE_DISCARD;
        }
        else
        {
            NIASSERT(m_eD3D10Usage == D3D10_USAGE_STAGING);
                eMapType = D3D10_MAP_WRITE;
        }

        void* pvLockedBuffer = NULL;
        HRESULT hr = m_pkD3D10Buffer->Map(eMapType, 0, &pvLockedBuffer);

        if (FAILED(hr) || pvLockedBuffer == NULL)
        {
            /* */
        }

        NiMemcpy(pvLockedBuffer, m_pvBackingBuffer, m_uiSize);

        m_pkD3D10Buffer->Unmap();
    }
}
//---------------------------------------------------------------------------
