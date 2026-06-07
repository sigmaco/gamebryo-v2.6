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

#ifndef NIPSCONVERTER_H
#define NIPSCONVERTER_H

#include "NiParticleLibType.h"

#include <NiTPointerMap.h>
#include <NiTLargeArray.h>
#include <NiSmartPointer.h>

class NiStream;
class NiAVObject;
class NiPSParticleSystem;
class NiPSMeshParticleSystem;
class NiParticleSystem;
class NiMesh;
class NiMeshParticleSystem;
class NiGeometry;
class NiPSSpawner;
class NiPSysSpawnModifier;
class NiSequenceData;

NiSmartPointer(NiObject);

/**
    Converts pre-NiMesh particle systems into NiMesh-based particle systems.

    This class contains static functions that are called as part of the
    NiStream post-processing step. After the file has been loaded, all
    top-level objects are passed to the Convert function. All NiParticleSystem
    objects found in a scene graph are replaced with NiPSParticleSystem or
    NiPSMeshParticleSystem objects.
    
    With the introduction of NiInterpolatorConverter in the Gamebryo 2.6
    animation system, NiPSConverter is no longer responsible for converting
    deprecated time controllers referenced by animation sequences to their 
    current equivalent. NiInterpolatorConverter performs this task as 
    sequences are read during streaming.
*/
class NIPARTICLE_ENTRY NiPSConverter
{
public:
    /**
        Main conversion function.

        This function is registered as a post-processing function with
        NiStream.

        @param kStream The stream calling this function.
        @param kTopObjects The array of top-level objects to convert.
    */
    static void Convert(NiStream& kStream, 
        NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

private:
    /// @name Specialized Conversion Functions
    //@{
    /**
        Recursively converts all deprecated particle systems in the specified
        scene graph to current particle systems.

        The old particle systems are removed from the scene graph and replaced
        by the new ones.

        @param kStream The stream containing the scene graph.
        @param pkObject The root of the scene graph to convert.
        @param kOldToNewSpawnerMap A map from old spawn modifiers to new ones,
            shared across all particle systems.
    */
    static void ConvertSceneGraph(
        NiStream& kStream,
        NiAVObject* pkObject,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap);

    /**
        Converts an NiParticleSystem object into an NiPSParticleSystem object.

        Called by ConvertSceneGraph.

        @param pkOldPSys The NiParticleSystem object to convert.
        @param kOldToNewSpawnerMap A map from old spawn modifiers to new ones,
            shared across all particle systems.
        @param kConversionMap The conversion map stored in the NiStream object,
            used to resolve removed NiAVObjects in the scene graph.
        @param bAttachModifiers If true, the simulator mesh modifier should
            be attached to the final mesh object.
        @return The new NiPSParticleSystem object.
    */
    static NiPSParticleSystem* ConvertOldPSys(
        NiParticleSystem* pkOldPSys,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        bool bAttachModifiers);

    /**
        Converts an NiMeshParticleSystem object into an NiPSMeshParticleSystem
        object.

        Called by ConvertSceneGraph.

        @param pkOldPSys The NiMeshParticleSystem object to convert.
        @param kOldToNewSpawnerMap A map from old spawn modifiers to new ones,
            shared across all particle systems.
        @param kConversionMap The conversion map stored in the NiStream object,
            used to resolve removed NiAVObjects in the scene graph.
        @param bAttachModifiers If true, the simulator mesh modifier should
            be attached to the final mesh object.
        @return The new NiPSMeshParticleSystem object.
    */
    static NiPSMeshParticleSystem* ConvertOldMeshPSys(
        NiMeshParticleSystem* pkOldPSys,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap,
        bool bAttachModifiers);
    //@}

    /// @name Component Conversion Functions
    //@{
    /**
        Converts all modifiers on an NiParticleSystem to equivalent modifiers
        on an NiPSParticleSystem.

        @param pkOldPSys The NiParticleSystem object containing the old
            modifiers.
        @param pkNewPSys The NiPSParticleSystem to which new modifiers should
            get attached.
        @param kOldToNewSpawnerMap A map from old spawn modifiers to new ones,
            shared across all particle systems.
        @param kConversionMap The conversion map stored in the NiStream object,
            used to resolve removed NiAVObjects in the scene graph.
    */
    static void ConvertPSysModifiers(
        NiParticleSystem* pkOldPSys,
        NiPSParticleSystem* pkNewPSys,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap,
        NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);

    /**
        Converts all time controllers on an NiParticleSystem object to
        equivalent controllers on an NiPSParticleSystem.

        @param pkOldPSys The NiParticleSystem object containing the old
            controllers.
        @param pkNewPSys The NiPSParticleSystem object to which new controllers
            should get attached.
    */
    static void ConvertPSysControllers(
        NiParticleSystem* pkOldPSys,
        NiPSParticleSystem* pkNewPSys);
    //@}

    /// @name Helper Functions
    //@{
    /**
        Generates a sequence conversion error message and reports it with
        NILOG.

        @param pcSequenceName The name of the sequence being converted.
        @param pcCtlrType The controller RTTI name that could not be converted.
    */
    static void ReportSequenceConversionError(
        const char* pcSequenceName,
        const char* pcCtlrType);

    /**
        Converts an NiPSysSpawnModifier to an NiPSSpawner.

        Uses the kOldToNewSpawnerMap parameter to ensure that only one
        NiPSSpawner is created for each NiPSysSpawnModifier.

        @param pkOldSpawnModifier The old spawn modifier to convert.
        @param kOldToNewSpawnerMap A map used to ensure a one-to-one
            correspondence between NiPSysSpawnModifier objects and NiPSSpawner
            objects.
        @return An NiPSSpawner object corresponding to the NiPSysSpawnModifier
            object.
    */
    static NiPSSpawner* ConvertOldSpawnModifier(
        NiPSysSpawnModifier* pkOldSpawnModifier,
        NiTPointerMap<NiPSysSpawnModifier*,NiPSSpawner*>& kOldToNewSpawnerMap);

    /**
        Uses a conversion map to resolve a valid pointer to a scene graph
        object.

        The conversion map identifies objects that have been removed from the
        scene graph in a previous conversion operation and links them to thier
        replacement objects. This function will return the original object if
        it is not found in the map.

        @param pkOriginalObject The object to look up in the map.
        @param kConversionMap The conversion map used to resolve the returned
            object.
        @return A valid NiAVObject corresponding to pkOriginalObject (may be
            the same object).
    */
    static NiAVObject* LookupObject(
        NiAVObject* pkOriginalObject,
        const NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap);
    //@}
};

#include "NiPSConverter.inl"

#endif  // #ifndef NIPSCONVERTER_H
