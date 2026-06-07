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
#ifndef NIPHYSXFLUIDCONVERTER_H
#define NIPHYSXFLUIDCONVERTER_H

#include "NiPhysXFluidLibType.h"

#include <NiTPointerMap.h>
#include <NiTLargeArray.h>
#include <NiSmartPointer.h>
#include <NiObject.h>

class NiStream;
class NiAVObject;
class NiPhysXProp;
class NiPhysXPropDesc;
class NiParticleSystem;
class NiPSParticleSystem;
class NiPSMeshFluidSystem;
class NiPhysXPSFluidSystem;
class NiPhysXFluidPropDesc;
class NiPhysXFluidSystem;
class NiPhysXPSMeshFluidSystem;
class NiPhysXMeshFluidSystem;
class NiMesh;
class NiGeometry;
class NiPSSpawner;
class NiPSysSpawnModifier;
class NiSequenceData;

/**
    Manages backwards compatibility by automatically converting
    NiGeometry-based fluid system assets to the NiMesh-based system.

    Conversion functions from this class are automatically registered for
    use by the streaming system in converting assets. Applications should not
    need to create one explicitly.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidConverter : public NiMemObject
{
public:

    /// Constructor.
    NiPhysXFluidConverter();
    
    /// Destructor.
    ~NiPhysXFluidConverter();

    /**
        Conversion function.

        Given a stream, this function iterates through all of the objects
        looking for PhysX fluids-related particle systems and converting them
        appropriately.
    */
    static void Convert(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

private:
    static void ConvertSceneGraph(NiStream& kStream, NiAVObject* pkObject,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap);

    static NiPhysXPSFluidSystem* ConvertOldFluid(
        NiPhysXFluidSystem* pkOldFluid,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        bool bAttachModifiers);

    static NiPhysXPSMeshFluidSystem* ConvertOldMeshFluid(
        NiPhysXMeshFluidSystem* pkOldFluid, 
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        bool bAttachModifiers);

    static void ConvertPSysModifiers(NiParticleSystem* pkOldFluid,
        NiPSParticleSystem* pkNewFluid, 
        NiTPointerMap<NiPSysSpawnModifier*, NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    static void ConvertPSysControllers(NiParticleSystem* pkOldFluid,
        NiPSParticleSystem* pkNewFluid);

    static void ReportSequenceConversionError(const char* pcSequenceName,
        const char* pcCtlrType);

    static NiPSSpawner* ConvertOldSpawnModifier(
        NiPSysSpawnModifier* pkOldSpawnModifier,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap);

    static NiAVObject* LookupObject(NiAVObject* pkOriginalObject,
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    static void ConvertNiPhysXProp(NiPhysXProp* pkPhysXProp, 
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
    
    static void ConvertNiPhysXFluidDesc(NiPhysXFluidPropDesc* pkFluidPropDesc,
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
};

#include "NiPhysXFluidConverter.inl"
#endif // #define NIPHYSXFLUIDCONVERTER_H
