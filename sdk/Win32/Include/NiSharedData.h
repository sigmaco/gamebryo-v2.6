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

#ifndef NISHAREDDATA_H
#define NISHAREDDATA_H

#include "NiPluginToolkitLibType.h"
#include <NiObject.h>

typedef const NiRTTI* NiSharedDataType;

/// The NiSharedData object defines the base class for all structures
/// being shared between plug-ins. This defines the base interface used by
/// all NiSharedData objects.
class NIPLUGINTOOLKIT_ENTRY NiSharedData : public NiObject
{
public:
    NiDeclareRTTI;

    /// Virtual destructor.
    inline virtual ~NiSharedData(){}

    /// Returns the shared data type for the object.
    NiSharedDataType GetType();
};

NiSmartPointer(NiSharedData);

#define NiGetSharedDataType(classname)\
    &classname::ms_RTTI

#endif  // #ifndef NISHAREDDATA_H
