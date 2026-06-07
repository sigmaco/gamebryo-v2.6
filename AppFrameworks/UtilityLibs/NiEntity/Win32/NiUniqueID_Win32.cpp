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
#include "NiEntityPCH.h"

#include "NiUniqueID.h"

// This library is needed for the UuidCreate function call.
#pragma comment(lib, "Rpcrt4.lib")

//---------------------------------------------------------------------------
bool NiUniqueID::Generate(NiUniqueID& kUniqueID)
{
    GUID kGuid = GUID_NULL;
    HRESULT hResult = ::UuidCreate(&kGuid);
    if (HRESULT_CODE(hResult) != RPC_S_OK)
    {
        return false;
    }

    NIASSERT(sizeof(kGuid) == NUM_BYTES_IN_UNIQUE_ID);
    kUniqueID = NiUniqueID((unsigned char*) &kGuid);
    return true;
}
//---------------------------------------------------------------------------
