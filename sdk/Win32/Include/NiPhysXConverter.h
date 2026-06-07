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
#ifndef NIPHYSXCONVERTER_H
#define NIPHYSXCONVERTER_H

#include "NiPhysXLibType.h"

#include <NiMemObject.h>
#include <NiStream.h>
#include <NiTArray.h>
#include <NiTMap.h>

class NiPhysXProp;

/**
    This class manages backwards compatibility by automatically converting
    NiGeometry-based assets to the NiMesh-based system.

    The conversion function provided by this class is automatically
    registered when the NiPhysX library is included by an application. No
    explicit action is required by an application. Any NiPhysX-related,
    pre-NiMesh objects are converted to their NiMesh equivalents.
*/
class NIPHYSX_ENTRY NiPhysXConverter : public NiMemObject
{
public:

    /// @cond EMERGENT_INTERNAL
    NiPhysXConverter();
    ~NiPhysXConverter();

    static void ConvertToNiMesh(NiStream& kStream,
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

    void RecursiveConvert(NiPhysXProp* pkProp,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    /// @endcond

private:
    void ConvertClothes(NiPhysXProp* pkProp,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
    void ConvertSources(NiPhysXProp* pkProp,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
    void ConvertDestinations(NiPhysXProp* pkProp,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
};

#endif
