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

#include "NiLog.h"

NiLogBehavior NiLogBehavior::ms_kDefault;
NiLogBehavior* NiLogBehavior::ms_pkSingleton = &NiLogBehavior::ms_kDefault;

//---------------------------------------------------------------------------
void NiLogBehavior::Set(NiLogBehavior* pkBehavior)
{
    if (pkBehavior)
        ms_pkSingleton = pkBehavior;
    else
        ms_pkSingleton = &ms_kDefault;
        
}
//---------------------------------------------------------------------------
NiLogBehavior* NiLogBehavior::Get()
{
    return ms_pkSingleton;
}
//---------------------------------------------------------------------------
void NiLogBehavior::Initialize()
{
#if !defined(NI_LOGGER_DISABLE)
    NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_0, true);
    NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_1, true);
    NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_2, true);
#endif
    // all other message types are ignored

}
//---------------------------------------------------------------------------
NiLogBehavior::~NiLogBehavior()
{
    // virtual destructor does nothing in base class but is 
    // defined for potential future use by derived classes.
}
//---------------------------------------------------------------------------
