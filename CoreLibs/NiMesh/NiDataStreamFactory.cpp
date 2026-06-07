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
#include "NiMeshPCH.h"

#include "NiDataStreamFactory.h"
#include "NiToolDataStream.h"

//---------------------------------------------------------------------------
void NiDataStreamFactory::ForceCPUReadAccessCallback(
    NiUInt8& uiAccessMask,
    NiDataStream::Usage eUsage)
{
    NI_UNUSED_ARG(eUsage);
    // If volatile, change to mutable
    if (uiAccessMask & NiDataStream::ACCESS_CPU_WRITE_VOLATILE)
    {
        // Clear current CPU_WRITE access flags
        uiAccessMask &= ~NiDataStream::ACCESS_CPU_WRITE_ANY;

        // Set to ACCESS_CPU_WRITE_MUTABLE
        uiAccessMask |= NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
    }

    // Or-in CPU read flag.
    uiAccessMask |= NiDataStream::ACCESS_CPU_READ;

    // confirm expected result
    NIASSERT(NiDataStream::IsAccessRequestValid(uiAccessMask, eUsage));    
}
//---------------------------------------------------------------------------
NiDataStream* NiDataStreamFactory::CreateDataStream(
    const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage,
    bool bForceCreateToolDS)
{
    NiDataStreamFactory* pkFactory = NiDataStream::GetFactory();

    // If a callback has been set on the factory, call it.
    if (pkFactory && pkFactory->GetCallback())
        (*pkFactory->GetCallback())(uiAccessMask, eUsage);

    if (NiDataStream::IsAccessRequestValid(uiAccessMask, eUsage))
    {
        if (pkFactory && !bForceCreateToolDS)
        {
            return pkFactory->CreateDataStreamImpl(
                kElements, 
                uiCount, 
                uiAccessMask, 
                eUsage);
        }

        return NiNew NiToolDataStream(
            kElements, 
            uiCount, 
            uiAccessMask,
            eUsage);
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiDataStream* NiDataStreamFactory::CreateDataStream( 
    NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage,
    bool bForceCreateToolDS,
    bool bCanOverrideAccessMask)
{
    NiDataStreamFactory* pkFactory = NiDataStream::GetFactory();

    if (bCanOverrideAccessMask)
    {
        // If a callback has been set on the factory, call it.
        if (pkFactory && pkFactory->GetCallback())
            (*pkFactory->GetCallback())(uiAccessMask, eUsage);
    }

    if (NiDataStream::IsAccessRequestValid(uiAccessMask, eUsage))
    {
        if (pkFactory && !bForceCreateToolDS)
        {
            return pkFactory->CreateDataStreamImpl(
                uiAccessMask, 
                eUsage, 
                bCanOverrideAccessMask);
        }

        return NiNew NiToolDataStream(uiAccessMask, eUsage);
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
