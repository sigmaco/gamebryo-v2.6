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
#include "NiMainPCH.h"

#include <NiRTLib.h>
#include "NiRTTI.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiRTTI::NiRTTI(const char* pcName, const NiRTTI* pkBaseRTTI) :
    m_pcName(pcName), m_pkBaseRTTI(pkBaseRTTI)
{
}
//---------------------------------------------------------------------------
bool NiRTTI::CopyName(char* acNameBuffer, unsigned int uiMaxSize) const
{
    const char* pcName = GetName();
    if (!pcName || !acNameBuffer)
    {
        NiStrcpy(acNameBuffer, uiMaxSize, "\0");
        return false;
    }

    NiStrcpy(acNameBuffer, uiMaxSize, pcName);
    return true;
}
//---------------------------------------------------------------------------
