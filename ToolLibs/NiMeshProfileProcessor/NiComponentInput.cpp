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

#include <NiSystem.h>
#include "NiComponentInput.h"
#include "NiMeshProfileOperations.h"

//---------------------------------------------------------------------------
NiComponentInput::StreamBasis& NiComponentInput::StreamBasis::operator= 
    (const StreamBasis& b)
{
    m_bIsPerInstance = b.m_bIsPerInstance;
    m_uiNumSubmeshes = b.m_uiNumSubmeshes;
    m_eCloningBehavior = b.m_eCloningBehavior;
    m_uiAccessMask = b.m_uiAccessMask;
    m_uiCount = b.m_uiCount;
    m_kElement = b.m_kElement;
    m_eUsage = b.m_eUsage;

    m_kRegions.RemoveAll();
    m_kRegions.Realloc(b.m_kRegions.GetSize());
    for (NiUInt32 ui = 0; ui < b.m_kRegions.GetSize(); ui++)
    {
        m_kRegions.Add(b.m_kRegions.GetAt(ui));
    }

    m_kRegionRefs.RemoveAll();
    for (NiUInt32 ui = 0; ui < b.m_kRegionRefs.GetSize(); ui++)
    {
        m_kRegionRefs.Add(b.m_kRegionRefs.GetAt(ui));
    }

    m_bIsValid = b.m_bIsValid;

    return *this;
};
//---------------------------------------------------------------------------
NiComponentInput::StreamBasis& NiComponentInput::StreamBasis::operator= 
    (const NiDataStreamRef& kSR)
{
    const NiDataStream* pkDS = kSR.GetDataStream();
    NIASSERT(pkDS);

    m_uiAccessMask = pkDS->GetAccessMask();
    m_eUsage = pkDS->GetUsage();
    m_eCloningBehavior = pkDS->GetCloningBehavior();
    m_eUsage = pkDS->GetUsage(); 

    m_kElement = pkDS->GetElementDescAt(0);

    m_kRegions.RemoveAll();
    for(NiUInt32 uiRegion = 0; uiRegion < pkDS->GetRegionCount(); uiRegion++)
    {
        m_kRegions.Add(pkDS->GetRegion(uiRegion));
    }

    m_kRegionRefs.RemoveAll();
    for (NiUInt32 ui = 0; ui < kSR.GetSubmeshRemapCount(); ui++)
    {
        m_kRegionRefs.Add(kSR.GetRegionIndexForSubmesh(ui));
    }

    m_uiCount = pkDS->GetSize() / pkDS->GetStride();

    m_bIsPerInstance = kSR.IsPerInstance();

    m_uiNumSubmeshes = (NiUInt16)kSR.GetSubmeshRemapCount();

    m_bIsValid = kSR.IsValid();

    return *this;
}
//---------------------------------------------------------------------------
