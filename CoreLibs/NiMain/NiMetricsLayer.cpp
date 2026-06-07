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
#include "NiMetricsLayer.h"

// Start array at zero so that it won't be initialized until after NiInit().
NiTObjectArray<NiMetricsOutputModulePtr> NiMetricsLayer::ms_kOutputs(0, 5);

NiFastCriticalSection NiMetricsLayer::ms_kMutex;

//---------------------------------------------------------------------------
void NiMetricsLayer::_SDMInit()
{
    
}
//---------------------------------------------------------------------------
void NiMetricsLayer::_SDMShutdown()
{
    // This shouldn't be necessary, but we'll do it just to be safe.
    ms_kMutex.Lock();

    for (unsigned int i = 0; i < ms_kOutputs.GetSize(); i++)
    {
        ms_kOutputs.SetAt(i, NULL);
    }
    
    // Clean up statically declared NiTObjectArray so that allocated
    // memory can be destroyed before NiShutdown()
    ms_kOutputs.SetSize(0);

    ms_kMutex.Unlock();
}
//---------------------------------------------------------------------------
