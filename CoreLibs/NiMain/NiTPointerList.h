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

#ifndef NITPOINTERLIST_H
#define NITPOINTERLIST_H

#include "NiMainLibType.h"
#include <NiRTLib.h>
#include <NiUniversalTypes.h>

#include "NiTPointerListBase.h"
#include "NiTPointerAllocator.h"

template <class T> class NiTPointerList : public
    NiTPointerListBase<NiTPointerAllocator<size_t>, T>{};

#endif // NITPOINTERLIST_H
