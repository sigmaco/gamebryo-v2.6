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

#include "StdAfx.h"
#include "NiTDataStreamFactoryProxy.h"

//---------------------------------------------------------------------------
template <class T> inline
NiDataStream* NiTDataStreamFactoryProxy<T>::CreateDataStreamImpl(
    const NiDataStreamElementSet& kElements, 
    NiUInt32 uiCount, 
    NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage)
{
    NiUInt8 uiOrigAccessMask = uiAccessMask;
    NiDataStreamFactory::ForceCPUReadAccessCallback(uiAccessMask, eUsage);

    NiDataStream* pkStream = T::CreateDataStreamImpl(
        kElements, 
        uiCount,
        uiAccessMask, 
        eUsage);
    
    CNifDoc::GetDocument()->SetDataStreamInfo(pkStream, uiOrigAccessMask);

    return pkStream;
}
//---------------------------------------------------------------------------
template <class T> inline
NiDataStream* NiTDataStreamFactoryProxy<T>::CreateDataStreamImpl(
    NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage,
    bool bCanOverrideAccessMask)
{
    NiUInt8 uiOrigAccessMask = uiAccessMask;
    if (bCanOverrideAccessMask)
        NiDataStreamFactory::ForceCPUReadAccessCallback(uiAccessMask, eUsage);

    NiDataStream* pkStream = T::CreateDataStreamImpl(
        uiAccessMask, 
        eUsage, 
        bCanOverrideAccessMask);

    CNifDoc::GetDocument()->SetDataStreamInfo(pkStream, uiOrigAccessMask);

    return pkStream;
}
//---------------------------------------------------------------------------
