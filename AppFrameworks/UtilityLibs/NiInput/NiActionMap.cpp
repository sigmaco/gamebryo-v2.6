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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiInputPCH.h"

#include "NiActionMap.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiTPool<NiAction, NiTNewInterface<NiAction> >* 
    NiActionMap::ms_pakActionPool = 0;
//---------------------------------------------------------------------------
NiActionMap::NiActionMap() :
    m_pcName(0), 
    m_pvPlatformSpecificData(0),
    m_eLastError(NIIERR_OK) 
{
    m_akActions.RemoveAll();
    m_akActions.SetSize(16);
    m_akActions.SetGrowBy(8);
}
//---------------------------------------------------------------------------
NiActionMap::NiActionMap(const char* pcName, void* pvPlatformSpecificData) :
    m_pcName(0), 
    m_pvPlatformSpecificData(0),
    m_eLastError(NIIERR_OK)
{
    SetName(pcName);
    m_pvPlatformSpecificData = pvPlatformSpecificData;
    m_akActions.RemoveAll();
    m_akActions.SetSize(16);
    m_akActions.SetGrowBy(8);
}
//---------------------------------------------------------------------------
NiActionMap::~NiActionMap()
{
    NiAction* pkAction;
    unsigned int uiSize = m_akActions.GetSize();

    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        pkAction = m_akActions.GetAt(ui);
        if (pkAction)
        {
            ReturnActionToPool(pkAction);
            m_akActions.SetAt(ui, 0);
        }
    }

    m_akActions.RemoveAll();

    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
void NiActionMap::SetName(const char* pcName)
{
    if (m_pcName && pcName && (strcmp(m_pcName, pcName) == 0))
        return;
    
    if (m_pcName)
    {
        NiFree(m_pcName);
        m_pcName = 0;
    }

    if (!pcName)
        return;

    size_t stLen = strlen(pcName) + 1;
    m_pcName = NiAlloc(char, stLen);
    NIASSERT(m_pcName);
    NiStrcpy(m_pcName, stLen, pcName);
}
//---------------------------------------------------------------------------
void NiActionMap::SetPlatformSpecificData(void* pvData)
{
    m_pvPlatformSpecificData = pvData;
}
//---------------------------------------------------------------------------
NiAction* NiActionMap::GetAction(unsigned int uiIndex)
{
    if (uiIndex > m_akActions.GetSize())
        return 0;
    return m_akActions.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiAction* NiActionMap::GetAction(unsigned int uiIndex) const
{
    if (uiIndex > m_akActions.GetSize())
        return 0;

    return m_akActions.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiAction* NiActionMap::AddAction(const char* pcName, unsigned int uiAppData, 
    unsigned int uiSemantic, unsigned int uiFlags, unsigned int uiDeviceID, 
    int iRangeLow, int iRangeHigh, void* pvContext, 
    unsigned int uiKeyModifiers, unsigned int uiMouseModifiers, 
    unsigned int uiPadModifiers)
{
    // Grab a 'free' action
    NiAction* pkAction = GetActionFromPool();
    if (!pkAction)
    {
        m_eLastError = NIIERR_ALLOCFAILED;
        return 0;
    }

    pkAction->SetName(pcName);
    pkAction->SetAppData(uiAppData);
    pkAction->SetSemantic(uiSemantic);
    pkAction->SetFlags(uiFlags);
    pkAction->SetDeviceID(uiDeviceID);
    pkAction->SetRange(iRangeLow, iRangeHigh);
    pkAction->SetContext(pvContext);
    pkAction->SetKeyModifiers(uiKeyModifiers);
    pkAction->SetMouseModifiers(uiMouseModifiers);
    pkAction->SetPadModifiers(uiPadModifiers);

    // Add it to the array
    if (m_akActions.GetEffectiveSize() == m_akActions.GetSize())
    {
        // If this block is hit, the AddFirstEmpty will grow the array!
        // 
    }

    m_akActions.AddFirstEmpty(pkAction);

    return pkAction;
}
//---------------------------------------------------------------------------
void NiActionMap::CreateActionPool()
{
    if (!ms_pakActionPool)
    {
        ms_pakActionPool = NiNew NiTPool<NiAction, 
            NiTNewInterface<NiAction> >;
        NIASSERT(ms_pakActionPool);
    }
}
//---------------------------------------------------------------------------
void NiActionMap::ReleaseActionPool()
{
    NiDelete ms_pakActionPool;
    ms_pakActionPool = 0;
}
//---------------------------------------------------------------------------
