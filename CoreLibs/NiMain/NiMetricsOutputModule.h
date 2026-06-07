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

#ifndef NIMETRICSOUTPUTMODULE_H
#define NIMETRICSOUTPUTMODULE_H

#include "NiRefObject.h"
#include "NiSmartPointer.h"

// Forward declared to avoid circular reference.
// Derived classes will need to include "NiMetricsTimer.h".
class NiMetricsTimer;

// Abstract virtual interface that output modules that the NiMetricsLayer
// class uses must derive from.
class NIMAIN_ENTRY NiMetricsOutputModule : public NiRefObject
{
public:
    virtual void AddValue(const char* pcName, float fValue) = 0;
    virtual void StartTimer(NiMetricsTimer& kTimer) = 0;
    virtual void EndTimer(NiMetricsTimer& kTimer) = 0;
    virtual void AddImmediateEvent(const char* pcName, float fValue) = 0;
    virtual void Update() = 0;
};

NiSmartPointer(NiMetricsOutputModule);

#endif //#ifndef NIMETRICSOUTPUTMODULE_H
