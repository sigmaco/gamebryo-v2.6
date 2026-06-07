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
#ifndef NIPHYSXPARTICLECONVERTER_H
#define NIPHYSXPARTICLECONVERTER_H

#include "NiPhysXParticleLibType.h"

#include <NiTPointerMap.h>
#include <NiTLargeArray.h>
#include <NiSmartPointer.h>
#include <NiObject.h>

class NiStream;
class NiAVObject;
class NiParticleSystem;
class NiMeshParticleSystem;
class NiPhysXParticleSystem;
class NiPhysXMeshParticleSystem;
class NiPhysXMeshPSysProp;
class NiPhysXPSysProp;
class NiPSParticleSystem;
class NiPSMeshParticleSystem;
class NiPhysXPSMeshParticleSystem;
class NiPhysXPSMeshParticleSystemProp;
class NiPhysXPSParticleSystem;
class NiPhysXPSParticleSystemProp;
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
class NIPHYSXPARTICLE_ENTRY NiPhysXParticleConverter : public NiMemObject
{
public:

    /// Constructor.
    NiPhysXParticleConverter();

    /// Destructor.
    ~NiPhysXParticleConverter();

    /**
        Conversion function.

        Given a stream, this function iterates through all of the objects
        looking for PhysX-based particle systems and converting them
        appropriately.
    */
    static void Convert(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

private:
    static void ConvertSceneGraph(
        NiStream& kStream,
        NiAVObject* pkObject,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap);

    static NiPhysXPSParticleSystem* ConvertOldPSys(
        NiPhysXParticleSystem* pkOldPSys,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        bool bAttachModifiers);

    static NiPhysXPSMeshParticleSystem* ConvertOldMeshPSys(
        NiPhysXMeshParticleSystem* pkOldPSys,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        bool bAttachModifiers);

    static void ConvertPSysModifiers(
        NiParticleSystem* pkOldPSys,
        NiPSParticleSystem* pkNewPSys,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    static void ConvertPSysControllers(NiParticleSystem* pkOldPSys, 
        NiPSParticleSystem* pkNewPSys);

    static void ReportSequenceConversionError(const char* pcSequenceName,
        const char* pcCtlrType);

    static NiPSSpawner* ConvertOldSpawnModifier(
        NiPSysSpawnModifier* pkOldSpawnModifier, 
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap);

    static NiAVObject* LookupObject(NiAVObject* pkOriginalObject,
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    static NiPhysXPSParticleSystemProp* ConvertPhysXParticlesProp(
        NiPhysXPSysProp* pkPhysXPSysProp, 
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    static NiPhysXPSMeshParticleSystemProp* ConvertPhysXMeshParticlesProp(
        NiPhysXMeshPSysProp* pkPhysXMeshPSysProp, 
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
};

#include "NiPhysXParticleConverter.inl"
#endif // #define NIPHYSXPARTICLECONVERTER_H
