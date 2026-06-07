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
#include "NiSystemPCH.h"

#include "NiStaticDataManager.h"
#include "NiMemManager.h"

NiMemManager* NiMemManager::ms_pkMemManager = 0;

//---------------------------------------------------------------------------
bool NiMemManager::IsInitialized()
{
    return ms_pkMemManager != 0;
}
//---------------------------------------------------------------------------
void NiMemManager::_SDMInit()
{
    ms_pkMemManager = NiExternalNew NiMemManager();
    ms_pkMemManager->m_pkAllocator = 
        NiStaticDataManager::GetInitOptions()->GetAllocator();
    NIASSERT(ms_pkMemManager->m_pkAllocator);
    ms_pkMemManager->m_pkAllocator->Initialize();
}
//---------------------------------------------------------------------------
void NiMemManager::_SDMShutdown()
{
    NIASSERT(ms_pkMemManager->m_pkAllocator);
    ms_pkMemManager->m_pkAllocator->Shutdown(); 
    NiExternalDelete ms_pkMemManager;
    ms_pkMemManager = NULL;
}
//---------------------------------------------------------------------------
bool NiMemManager::VerifyAddress(const void* pvMemory)
{
    NIASSERT(ms_pkMemManager->m_pkAllocator);
    return ms_pkMemManager->m_pkAllocator->VerifyAddress(pvMemory);
}
//---------------------------------------------------------------------------
