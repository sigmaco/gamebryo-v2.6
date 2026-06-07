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

#include "NiMainPCH.h"
#include "NiMainMetrics.h"

const char NiMainMetrics::ms_acNames
    [NiMainMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_MAIN_PREFIX "NumUpdates"
};

NiUInt32 NiMainMetrics::ms_uiNumUpdates = 0;

//---------------------------------------------------------------------------
void NiMainMetrics::RecordMetrics()
{
    NIMETRICS_MAIN_ADDVALUE(NUM_UPDATES, ms_uiNumUpdates);
    ms_uiNumUpdates = 0;
}
//---------------------------------------------------------------------------

