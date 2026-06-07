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

#include "NiDataStream.h"
#include "NiDataStreamFactory.h"
#include "NiLog.h"
#include "NiRenderer.h"
#include "NiStream.h"
#include "NiToolDataStream.h"

NiImplementRTTI(NiDataStream, NiObject);

//---------------------------------------------------------------------------
static const NiUInt32 MAX_COMPS = 64;
NiDataStreamFactory* NiDataStream::ms_pkFactory = NULL;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiDataStream::~NiDataStream()
{
    // This assertion detects the deletion of a locked NiDataStream. All 
    // streams should be properly unlocked before deletion.
    NIASSERT(!GetLocked() && "Attempt to delete NiDataStream in a locked "
        "state. All streams should be properly unlocked before deletion.");
}
//---------------------------------------------------------------------------
bool NiDataStream::GetSizeInfo(NiUInt32* auiComponentSizes, 
    const NiUInt32 uiMaxComponentCount, NiUInt32& uiActualComponentCount)
{
    uiActualComponentCount = 0;

    NiUInt32 uiNumElements = m_kElements.GetSize();
    for (NiUInt32 i = 0; i < uiNumElements; i++)
    {
        // Account for each component in this element
        NiDataStreamElement& kElem = m_kElements.GetAt(i);
        NiUInt8 uiCount = 
            NiDataStreamElement::GetComponentCount(kElem.GetFormat());

        // Make sure that there's sufficient space in auiComponentSizes array
        if (uiActualComponentCount + uiCount > uiMaxComponentCount)
            return false;

        for (NiUInt32 j = 0; j < uiCount; j++)
        {
            auiComponentSizes[uiActualComponentCount++] = 
                NiDataStreamElement::GetComponentSize(kElem.GetFormat());
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
NiDataStream* NiDataStream::CreateDataStream(
    const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    Usage eUsage,
    bool bForceCreateToolDS)
{
    return NiDataStreamFactory::CreateDataStream(
        kElements, 
        uiCount, 
        uiAccessMask, 
        eUsage, 
        bForceCreateToolDS);
}    
//--------------------------------------------------------------------------
NiDataStream* NiDataStream::CreateDataStream( 
    NiUInt8 uiAccessMask, 
    Usage eUsage,
    bool bForceCreateToolDS,
    bool bCanOverrideAccessMask)
{
    return NiDataStreamFactory::CreateDataStream(
        uiAccessMask, 
        eUsage, 
        bForceCreateToolDS, 
        bCanOverrideAccessMask);
}
//---------------------------------------------------------------------------
NiDataStream* NiDataStream::CreateFullCopy()
{   
    // There is no good use case for copying a ACCESS_CPU_WRITE_STATIC 
    // renderer data stream, so it is disallowed. The 
    // ACCESS_CPU_WRITE_STATIC_INITIALIZED flag is checked instead of 
    // the ACCESS_CPU_WRITE_STATIC flag in order to allow NiToolDataStream
    // objects to ignore this restriction.
    NIASSERT(!(GetAccessMask() & ACCESS_CPU_WRITE_STATIC_INITIALIZED));

    return CreateFullCopyEx(GetAccessMask(), GetUsage(), true, true);
}
//---------------------------------------------------------------------------
NiDataStream* NiDataStream::CreateFullCopyEx(NiUInt8 uiAccessMask,
    Usage eUsage, bool bCopyRegions, bool bCopyBuffer)
{
    NiDataStream* pkObject = CreateDataStream(m_kElements, GetTotalCount(),
        uiAccessMask, eUsage, false);
    NIASSERT(pkObject);
    pkObject->m_eCloningBehavior = m_eCloningBehavior;
    pkObject->m_bStreamable = m_bStreamable;
    CopyData(pkObject, bCopyRegions, bCopyBuffer);
    return pkObject;
}
//---------------------------------------------------------------------------
void NiDataStream::CopyData(NiDataStream* pkDest, bool bCopyRegions, 
    bool bCopyBuffer)
{
    NIASSERT(pkDest);
    if (bCopyBuffer)
    {
        const void* pvData = Lock(LOCK_READ | LOCK_TOOL_READ);

        // You hit this assert if you try to clone a data stream that is not
        // readable. Readability is required to copy the data out of
        // the stream.
        NIASSERT(pvData &&
            "NiDataStream::CopyMembers: "
            "pkDataStream->Lock(LOCK_READ) failed!");

        NIASSERT(pkDest != NULL);
            
        void* pvNewData = pkDest->Lock(LOCK_WRITE | LOCK_TOOL_WRITE);

        // You hit this assert if the usage on the cloned object does not 
        // allow CPU writing.
        NIASSERT(pvNewData &&
            "NiDataStream::CopyMembers: pkDest->Lock(LOCK_WRITE) failed!");
        NiMemcpy(pvNewData, pvData, GetSize());

        // Unlock both streams.
        pkDest->Unlock(LOCK_WRITE | LOCK_TOOL_WRITE);
        Unlock(LOCK_READ | LOCK_TOOL_READ);
    }

    if (bCopyRegions)
    {
        pkDest->RemoveAllRegions();

        // Copy regions
        const NiUInt32 uiRegionCount = GetRegionCount();
        for (NiUInt32 ui = 0; ui < uiRegionCount; ++ui)
            pkDest->AddRegion(GetRegion(ui));
    }
}
//---------------------------------------------------------------------------
void* NiDataStream::Lock(NiUInt8 uiLockMask)
{
    // If the lock request is valid, perform the platform-specific lock.
    void* pvData = NULL;
    if (IsLockRequestValid(uiLockMask))
        pvData = LockImpl(uiLockMask);

    // Return the mapped address (NULL if Lock failed).
    return pvData;
}
//---------------------------------------------------------------------------
void* NiDataStream::LockRegion(NiUInt32 uiRegion, NiUInt8 uiLockMask) 
{ 
    // Same as Lock above, but returns address offset to the region start.
    char* pcData = (char*) Lock(uiLockMask);
    
    if (pcData != NULL)
    {
        const Region& kRegion = m_kRegions.GetAt(uiRegion);
        NiUInt32 uiOffset = kRegion.GetStartIndex() * m_kElements.m_uiStride;
        pcData += uiOffset;
    }
    return pcData;
}
//---------------------------------------------------------------------------
bool NiDataStream::IsLockRequestValid(NiUInt8 uiLockMask) const
{   
    // Check against read attempt on non-readable buffer
    if ((uiLockMask & LOCK_READ) && 
        !(m_uiAccessMask & ACCESS_CPU_READ) && 
        (uiLockMask & LOCK_TOOL_READ) == 0)
    {
        NILOG("LOCK_READ on a stream without ACCESS_CPU_READ\n");
        return false;
    }

    // Check against write attempt on non-writable buffer
    if ((uiLockMask & LOCK_WRITE) && 
        !(m_uiAccessMask & ACCESS_CPU_WRITE_ANY) && 
        (uiLockMask & LOCK_TOOL_WRITE) == 0)
    {
        NILOG("LOCK_WRITE on a stream without ACCESS_CPU_WRITE_*\n");
        return false;
    }

    // Check against write attempt on already-initialized static buffer
    if ((uiLockMask & LOCK_WRITE) && 
        (m_uiAccessMask & ACCESS_CPU_WRITE_STATIC_INITIALIZED) && 
        (uiLockMask & LOCK_TOOL_WRITE) == 0)
    {
        NILOG("LOCK_WRITE on an already-initialized "
            "ACCESS_CPU_WRITE_STATIC stream\n");
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiDataStream::IsAccessRequestValid(NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage)
{
    // No more than one CPU_WRITE_* flags
    NiUInt8 uiCPUWriteFlags = static_cast<NiUInt8>(
        uiAccessMask & ACCESS_CPU_WRITE_ANY);
    if (uiCPUWriteFlags != 0 &&
        uiCPUWriteFlags != ACCESS_CPU_WRITE_STATIC && 
        uiCPUWriteFlags != ACCESS_CPU_WRITE_MUTABLE && 
        uiCPUWriteFlags != ACCESS_CPU_WRITE_VOLATILE)
    {
        NILOG("ACCESS_CPU_WRITE_* flags are mutually exclusive "
            "with each other\n");
        return false;
    }

    // No GPU_WRITE and CPU_READ
    if (((uiAccessMask & ACCESS_CPU_READ) != 0) && 
        ((uiAccessMask & ACCESS_GPU_WRITE) != 0))
    {
        NILOG("ACCESS_GPU_WRITE and ACCESS_CPU_READ flags are mutually "
            "exclusive\n");
        return false;
    }

    // No GPU_WRITE and CPU_WRITE_VOLATILE
    if (((uiAccessMask & ACCESS_GPU_WRITE) != 0) && 
        ((uiAccessMask & ACCESS_CPU_WRITE_VOLATILE) != 0))
    {
        NILOG("ACCESS_GPU_WRITE and ACCESS_CPU_WRITE_VOLATILE flags are "
            "mutually exclusive\n");
        return false;
    }

    // No GPU_WRITE and CPU_WRITE_STATIC
    if (((uiAccessMask & ACCESS_GPU_WRITE) != 0) && 
        ((uiAccessMask & ACCESS_CPU_WRITE_STATIC) != 0))
    {
        NILOG("ACCESS_GPU_WRITE and ACCESS_CPU_WRITE_STATIC flags are "
            "mutually exclusive\n");
        return false;
    }

    // No CPU_READ and CPU_WRITE_VOLATILE
    if (((uiAccessMask & ACCESS_CPU_READ) != 0) && 
        ((uiAccessMask & ACCESS_CPU_WRITE_VOLATILE) != 0))
    {
        NILOG("ACCESS_CPU_WRITE_VOLATILE and ACCESS_CPU_READ flags are "
            "mutually exclusive\n");
        return false;
    }

    // CPU_WRITE or GPU_WRITE must be present.
    if (((uiAccessMask & ACCESS_CPU_READ) == 0) && 
        ((uiAccessMask & ACCESS_GPU_READ) == 0))
    {
        NILOG("ACCESS_CPU_READ or ACCESS_GPU_READ must be present\n");
        return false;
    }

    if (eUsage == USAGE_USER)
    {
        if ((uiAccessMask & ACCESS_GPU_READ) != 0)
        {
            NILOG("USAGE_USER buffers cannot have the ACCESS_GPU_READ flag "
                "set\n");
            return false;
        }
    }

    return true;
}
//--------------------------------------------------------------------------
NiDataStream* NiDataStream::CreateSingleElementDataStream(
    NiDataStreamElement::Format eFormat,
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask,
    Usage eUsage, 
    const void* pvSource, 
    bool bCreateRegion0, 
    bool bForceCreateToolDS)
{
    // Add the single element
    NiDataStreamElementSet kElements;
    kElements.AddElement(eFormat);

    // Allocate stream
    NiDataStream* pkStream = CreateDataStream(kElements, uiCount, 
        uiAccessMask, eUsage, bForceCreateToolDS);
    NIASSERT(pkStream && "Allocation of NiDataStream failed!");

    // Allocate Region 0
    if (bCreateRegion0)
    {
        Region kRegion(0, uiCount);
        pkStream->AddRegion(kRegion);
    }

    // If source data valid, copy into stream
    if (pvSource)
    {
        NiDataStream::LockType eLock = NiDataStream::LOCK_WRITE;
        void *pvData = pkStream->Lock((NiUInt8)eLock);
        NIASSERT(pvData && "pkStream->LockImpl failed!");
        NiMemcpy(pvData, pvSource, pkStream->GetSize());
        pkStream->Unlock((NiUInt8)eLock);
    }
    return pkStream;
}
//---------------------------------------------------------------------------
void NiDataStream::SetDefaultCloningBehavior()
{
    NiUInt8 uiCPUWriteFlags = static_cast<NiUInt8>(
        m_uiAccessMask & ACCESS_CPU_WRITE_ANY);
    switch (uiCPUWriteFlags)
    {
    case ACCESS_CPU_WRITE_MUTABLE:
        if (m_uiAccessMask & ACCESS_CPU_READ)
            SetCloningBehavior(CLONE_COPY);
        else
            SetCloningBehavior(CLONE_SHARE);
        break;
    case ACCESS_CPU_WRITE_VOLATILE:
        SetCloningBehavior(CLONE_BLANK_COPY);
        break;
    default:
        SetCloningBehavior(CLONE_SHARE);
        break;
    }
}
//---------------------------------------------------------------------------
// NiSPDataStream
//---------------------------------------------------------------------------
NiUInt32 NiDataStream::GetBlockCount(NiUInt32 uiRegionIdx) const
{
    return GetCount(uiRegionIdx);
}
//---------------------------------------------------------------------------
const void* NiDataStream::LockSource(NiUInt32 uiRegionIdx)
{
    return LockRegion(uiRegionIdx, (NiUInt8)(GetLockFlags()));
}
//---------------------------------------------------------------------------
void NiDataStream::UnlockSource()
{
    Unlock((NiUInt8)(GetLockFlags()));
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiObject* NiDataStream::CreateClone(NiCloningProcess& kCloning) 
{ 
    // Check if "this" was already added to the clone map
    NiObject* pkCloneObject = 0;
    NiDataStream* pkCloneDS;
    if (kCloning.m_pkCloneMap->GetAt(this, pkCloneObject))
    {
        // "this" was already cloned, return the previous clone.
        NIASSERT(pkCloneObject == this);
        pkCloneDS = (NiDataStream*) pkCloneObject;
    }
    else // "this" not found in clone map
    {
        // For CLONE_SHARE, clone == "this"
        if (m_eCloningBehavior == CLONE_SHARE)
        {
            pkCloneDS = this;
        }
        else
        {
            // Create the correct derived datastream class. 
            // Strip ACCESS_CPU_WRITE_STATIC_INITIALIZED from the access mask
            pkCloneDS = CreateDataStream(m_kElements, 
                GetTotalCount(),
                (NiUInt8)(GetAccessMask() & ~ACCESS_CPU_WRITE_STATIC_INITIALIZED), 
                GetUsage(), false);
            NIASSERT(pkCloneDS != NULL); 

            // Copy in the internal members. The CopyMembers function is 
            // "smart" in that it will take into account the cloning behavior
            // flags.
            CopyMembers(pkCloneDS, kCloning); 

        }
        // add clone to clone map
        kCloning.m_pkCloneMap->SetAt(this, pkCloneDS);
    }
    // return the clone
    return pkCloneDS; 
} 
//---------------------------------------------------------------------------
void NiDataStream::CopyMembers(NiDataStream* pkDest, 
    NiCloningProcess&)
{
    pkDest->m_eCloningBehavior = m_eCloningBehavior;
    pkDest->m_bStreamable = m_bStreamable;

    if (m_eCloningBehavior == CLONE_COPY)
    {
        // There is no good use case for copying a ACCESS_CPU_WRITE_STATIC 
        // renderer data stream, so it is disallowed. The 
        // ACCESS_CPU_WRITE_STATIC_INITIALIZED flag is checked instead of 
        // the ACCESS_CPU_WRITE_STATIC flag in order to allow NiToolDataStream
        // objects to ignore this restriction.
        NIASSERT(!(GetAccessMask() & ACCESS_CPU_WRITE_STATIC_INITIALIZED));

        // If CLONE_COPY, all data is transferred.
        CopyData(pkDest, true, true);
    }
    else if (m_eCloningBehavior == CLONE_BLANK_COPY)
    {
        // If CLONE_BLANK_COPY, only the regions are copied
        CopyData(pkDest, true, false);
    }
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiObject* NiDataStream::CreateObject(const char** pcArguments,
    unsigned int uiArgCount) 
{ 
    NIASSERT(uiArgCount == 2);
    if (uiArgCount != 2)
    {
        return NULL;
    }
    
    // parameter 0 - Usage
    // parameter 1 - Access flags
    Usage eUsage = (Usage)atoi(pcArguments[0]);
    NiUInt8 uiAccessMask = (NiUInt8)atoi(pcArguments[1]); 

    NiDataStream* pkObject = CreateDataStream(uiAccessMask, eUsage);
    NIASSERT(pkObject != NULL); 
    return pkObject; 
}
//---------------------------------------------------------------------------
bool NiDataStream::GetStreamableRTTIName(char* acName, 
    unsigned int uiMaxSize) const
{
    // serialize all NiDataStream descendant as NiDataStream, 
    // instead of subclass.
    if ( NiDataStream::ms_RTTI.CopyName(acName, uiMaxSize))
    {
        char acDelimeter [] = {NiStream::ms_cRTTIDelimiter, '\0'};
        NiStrcat(acName, uiMaxSize, acDelimeter);

        char acScratchBuffer[256];
        NiSprintf(acScratchBuffer, 256, "%d", GetUsage());

        NiStrcat(acName, uiMaxSize, acScratchBuffer);
        NiStrcat(acName, uiMaxSize, acDelimeter);

        NiUInt32 uiAccessMask = (NiUInt32)GetAccessMask();
        uiAccessMask = uiAccessMask & 
            (~ACCESS_CPU_WRITE_STATIC_INITIALIZED);

        NiSprintf(acScratchBuffer, 256, "%d", uiAccessMask);
        NiStrcat(acName, uiMaxSize, acScratchBuffer);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiDataStream::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiSize);
    NiStreamLoadBinary(kStream, m_eCloningBehavior);

    // Load Regions
    NiUInt32 uiNumRegions;
    NiStreamLoadBinary(kStream, uiNumRegions);

    for (NiUInt32 uiRegion = 0; uiRegion < uiNumRegions; uiRegion++)
    {
        NiDataStream::Region kThisRegion;
        NiUInt32 uiStartIndex;
        NiUInt32 uiRange;
        NiStreamLoadBinary(kStream, uiStartIndex);
        NiStreamLoadBinary(kStream, uiRange);
        kThisRegion.SetStartIndex(uiStartIndex);
        kThisRegion.SetRange(uiRange);
        m_kRegions.Add(kThisRegion);
    }

    // Load elements
    NiUInt32 uiElementCount;
    NiStreamLoadBinary(kStream, uiElementCount);
    for (NiUInt32 uiElt = 0; uiElt < uiElementCount; uiElt++)
    {
        NiDataStreamElement::Format eFormat;
        NiStreamLoadBinary(kStream, eFormat);
        m_kElements.AddElement(eFormat);
    }
    
    // Determine the sizes needed to automatically endian convert the data
    NiUInt32 auiComps[MAX_COMPS];
    NiUInt32 uiCompCount = 0;
    NIVERIFY(GetSizeInfo(auiComps, MAX_COMPS, uiCompCount));
   
    // Load the stream data using per component information.
    // In this manner, endian issues are resolved auto-magically inside
    // the streaming code. If NIF endian and platform endian match
    // (in this case big endian) then no conversion takes place.
    // However if little endian data is loaded this function will 
    // convert the data to big endian on the fly.
    Allocate();
    if (ValidateDataAlignment(auiComps, uiCompCount))
    {
        void* pvData = Lock(LOCK_WRITE);
        if (pvData)
        {
            NiStreamLoadBinary(kStream, pvData, GetTotalCount(), auiComps, 
                uiCompCount);
        }
        else
        {
            // Failed to lock buffer. Need to recover to continue streaming.
            NILOG("Error: Failed to lock DataStream in"
                " NiDataStream::LoadBinary()\n");

            kStream.Istr().Seek(m_uiSize);
        }
        Unlock(LOCK_WRITE);
    }
    else
    {
        // Data is not correctly aligned for the current platform. Attempting
        // to load/use this data will result in an alignment fault.
        NILOG("Error: Attempted to load a data stream with a data alignment"
            " that is not compatible with the current platform.\nThe contents"
            " of the data stream will be undefined.\n");

        kStream.Istr().Seek(m_uiSize);
    }

    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    m_bStreamable = NIBOOL_IS_TRUE(bValue);
}
//---------------------------------------------------------------------------
void NiDataStream::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiDataStream::RegisterStreamables(NiStream& kStream)
{
    if (m_bStreamable)
    {
        return NiObject::RegisterStreamables(kStream);
    }

    return false;
}
//---------------------------------------------------------------------------
void NiDataStream::SaveBinary(NiStream& kStream)
{
    // If this assertion is hit, an NiDataStream that is marked as
    // non-streamable is being streamed. Any code saving out NiDataStream
    // objects should use the NiDataStream::SaveLinkID function instead of
    // the NiStream::SaveLinkID function.
    NIASSERT(m_bStreamable);

    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiSize);
    NiStreamSaveBinary(kStream, m_eCloningBehavior);

    // Save Regions
    NiUInt32 uiNumRegions = m_kRegions.GetSize();
    NiStreamSaveBinary(kStream, uiNumRegions);
    for (NiUInt32 uiRegion = 0; uiRegion < uiNumRegions; uiRegion++)
    {
        const NiDataStream::Region& kRegion = m_kRegions.GetAt(uiRegion);
        NiStreamSaveBinary(kStream, kRegion.GetStartIndex());
        NiStreamSaveBinary(kStream, kRegion.GetRange());
    }

    // Save elements
    NiUInt32 uiElemCount = m_kElements.GetSize();
    NiStreamSaveBinary(kStream, uiElemCount);
    for (NiUInt32 uiElt = 0; uiElt < uiElemCount; uiElt++)
    {
        const NiDataStreamElement& kElem = m_kElements.GetAt(uiElt);
        NiStreamSaveBinary(kStream, kElem.GetFormat());
    }

    // Determine the sizes needed to automatically endian convert the data
    NiUInt32 auiComps[MAX_COMPS];
    NiUInt32 uiCompCount = 0;
    NIVERIFY(GetSizeInfo(auiComps, MAX_COMPS, uiCompCount));
    
    // Save the stream data using per component information.
    // In this manner, endian issues are resolved auto-magically inside
    // the streaming code. If NIF endian and platform endian match
    // (in this case big endian) then no conversion takes place.
    // However if little endian data is loaded this function will 
    // convert the data to big endian on the fly.
    NiUInt8* puiData = (NiUInt8*) Lock(LOCK_READ | LOCK_TOOL_READ);

    static bool sbWarn = true;
    // Save the data
    if (puiData)
    {
        NiStreamSaveBinary(kStream, puiData, GetTotalCount(), auiComps,
            uiCompCount);
        sbWarn = true;
    }
    else
    {
        // This conditional masks the assertion until a stream succeeds.
        // Ideally, we'd warn once per call to NiStream::Save. However, that
        // would require NiStream to be able to signal NiDataStream which 
        // introduces a circular dependency. The current setup at least 
        // prevents hundreds of warnings if all streams lack CPU_READ.
        if (sbWarn)
        {
            NIASSERT(!"NiDataStream::SaveBinary called on a stream without "
                "CPU_READ access. Data lock failed. All streamed data will be "
                "zero. Assertion will be masked until successful lock occurs");
            sbWarn = false;
        }

        // Calculate the total number of bytes that we'll need.
        NiUInt32 uiBytes = 0;
        for (NiUInt32 ui = 0; ui < uiCompCount; uiBytes += auiComps[ui++]) { }
        uiBytes *= GetTotalCount();

        // Allocate a buffer as a fake data stream and fill it with zeroes.
        NiUInt8* pcBuffer = NiAlloc(NiUInt8, uiBytes);
        memset(pcBuffer, 0, uiBytes);

        // Stream out the temporary data and delete.
        NiStreamSaveBinary(kStream, pcBuffer, uiBytes);
        NiFree(pcBuffer);
    }
    
    // Unlock the streams
    Unlock(LOCK_READ | LOCK_TOOL_READ);

    NiStreamSaveBinary(kStream, NiBool(m_bStreamable));
}
//---------------------------------------------------------------------------
bool NiDataStream::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiDataStream* pkDS = (NiDataStream*) pkObject;

    // Compare members
    if (m_uiSize != pkDS->m_uiSize ||        
        m_uiAccessMask != pkDS->m_uiAccessMask ||
        m_eUsage!= pkDS->m_eUsage)
    {
        return false;
    }

    // Compare Region set sizes
    if (m_kRegions.GetSize() != pkDS->m_kRegions.GetSize())
        return false;

    // Compare Region values
    NiUInt32 uiRegionCount = m_kRegions.GetSize();
    for (NiUInt32 uiRegion = 0; uiRegion < uiRegionCount; uiRegion++)
    {
        if (!m_kRegions.GetAt(uiRegion).IsEqual(pkDS->m_kRegions.GetAt(
            uiRegion)))
        {
            return false;
        }
    }

    // Compare element set sizes
    if (m_kElements.GetSize() != pkDS->m_kElements.GetSize())
        return false;

    // Compare elements
    NiUInt32 uiElementCount = m_kElements.GetSize();
    for (NiUInt32 uiElem = 0; uiElem < uiElementCount; uiElem++)
    {
        if (!m_kElements.GetAt(uiElem).IsEqual(pkDS->m_kElements.GetAt(
            uiElem))) 
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiDataStream::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiDataStream::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Size", m_uiSize));
    pkStrings->Add(NiGetViewerString("Stride", m_kElements.m_uiStride));
    pkStrings->Add(NiGetViewerString("Access Mask", m_uiAccessMask));
    pkStrings->Add(NiGetViewerString("Usage", m_eUsage));

    pkStrings->Add(NiGetViewerString("Num Regions", m_kRegions.GetSize()));
    pkStrings->Add(NiGetViewerString("Num Elements", m_kElements.GetSize()));
}
//---------------------------------------------------------------------------
NiString NiDataStream::AccessFlagsToString(NiUInt8 uiFlags)
{
    NiString kReturn(256);

    if (uiFlags & ACCESS_CPU_READ)
        kReturn += "ACCESS_CPU_READ";
    
    if (uiFlags & ACCESS_GPU_READ)
    {
        if (kReturn.Length() > 0)
             kReturn += " | ";
        kReturn += "ACCESS_GPU_READ";
    }

    if (uiFlags & ACCESS_CPU_WRITE_STATIC)
    {
        if (kReturn.Length() > 0)
             kReturn += " | ";
        kReturn += "ACCESS_CPU_WRITE_STATIC";
    }

    if (uiFlags & ACCESS_CPU_WRITE_MUTABLE)
    {
        if (kReturn.Length() > 0)
             kReturn += " | ";
        kReturn += "ACCESS_CPU_WRITE_MUTABLE";
    }

    if (uiFlags & ACCESS_CPU_WRITE_VOLATILE)
    {
        if (kReturn.Length() > 0)
             kReturn += " | ";
        kReturn += "ACCESS_CPU_WRITE_VOLATILE";
    }
    
    if (uiFlags & ACCESS_GPU_WRITE)
    {
        if (kReturn.Length() > 0)
             kReturn += " | ";
        kReturn += "ACCESS_GPU_WRITE";
    }

    if (uiFlags & ACCESS_CPU_WRITE_STATIC_INITIALIZED)
    {
        if (kReturn.Length() > 0)
             kReturn += " | ";
        kReturn += "ACCESS_CPU_WRITE_STATIC_INITIALIZED";
    }

    if (kReturn.Length() == 0)
        kReturn += "NO FLAGS";

    return kReturn;
}
//---------------------------------------------------------------------------
const char* NiDataStream::UsageToString(Usage eUsage)
{
    switch(eUsage)
    {
        case USAGE_VERTEX_INDEX:
            return "USAGE_VERTEX_INDEX";
        case USAGE_VERTEX:
            return "USAGE_VERTEX";
        case USAGE_SHADERCONSTANT:
            return "USAGE_SHADERCONSTANT";
        case USAGE_USER:
            return "USAGE_USER";
        case USAGE_DISPLAYLIST:
            return "USAGE_DISPLAYLIST";
        case USAGE_MAX_TYPES:
            return "USAGE_MAX_TYPES";
    }

    return "UNKNOWN";
}
//---------------------------------------------------------------------------
