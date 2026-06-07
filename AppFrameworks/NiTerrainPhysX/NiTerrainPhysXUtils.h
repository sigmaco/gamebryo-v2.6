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

#ifndef NITERRAINPHYSXUTILS_H
#define NITERRAINPHYSXUTILS_H

#include <NiTerrainSector.h>
#include <NiPhysXScene.h>
#include "NiTerrainPhysXLibType.h"

/**
    PhysX Utility class that manages the creation, loading and saving of
    terrain PhysX data. 
 */
class NITERRAINPHYSX_ENTRY NiTerrainPhysXUtils : public NiRefObject
{
public:

    /**
        ReturnCode is used in the CreatePhysXActor and the LoadPhysXActors 
        functions. Depending a given scenario, the functions will set their 
        ReturnCode parameter to describe what (if anything) went wrong.
        
        For example, if the return code contains RC_LOADED and RC_OUTOFDATE,
            the file has been loaded even though it was out of date. 
        
        When RC_BUILT_DATA is present, it will as well return a reason why data 
            wasn't loaded from a file (RC_FILENOTFOUND or RC_FILEOUTOFDATE). 
    */
    enum ReturnCode
    {
        /// An error occurred when initializing the creation function.
        RC_INVALID_SECTOR   = 0x01,

        /// File format is invalid.
        RC_INVALID_FORMAT   = 0x02,

        /// The file is out of date.
        RC_OUTOFDATE        = 0x04,

        /// The file was not found.
        RC_FILENOTFOUND     = 0x08,

        /// An error occurred when reading the file.
        RC_FILE_ERROR       = 0x10,

        /// The file was not loaded, data was created on the fly.
        RC_BUILT_DATA       = 0x20,

        /// The file was loaded successfully.
        RC_LOADED           = 0x40,
    };
        
    /// Default constructor
    NiTerrainPhysXUtils();
    ~NiTerrainPhysXUtils();

    /// @cond EMERGENT_INTERNAL
    static void SDM_Init();
    static void SDM_Shutdown();
    /// @endcond
	    
    /**
        Entry point for initialization of PhysX data for a given terrain 
        sector. The function will attempt to load data from the PhysX data
        file on disk (unless bCreateAlways is false).

        If data could not be loaded from disk, it will be created on the fly.

        @param pkSector Terrain sector for which to create PhysX data
        @param pkScene PhysX scene to which created PhysX actors will be 
            added
        @param kActorDesc PhysX Actor Descriptor used to create the PhysX 
            actors
        @param kActorArray Populated with a pointer to each PhysX actor that
            was added to the PhysX scene
        @param bCreateAlways If true, force generation of new PhysX data, 
            ignoring any PhysX data on disk
        @param puiErrorCode returns a bitwise combination of 
            NiTerrainPhysXUtils::ReturnCode. 
        @param bLoadOutOfDate If true, force loading of PhysX data from disk
            even if it is out of date. False by default, which will cause data
            to be created on the fly if the file is out of date.
    */
    bool CreatePhysXActor(NiTerrainSector* pkSector,
        NiPhysXScene* pkScene, NxActorDesc kActorDesc, 
        NiTPrimitiveArray<NxActor*>& kActorArray, bool bCreateAlways = false,
        NiUInt8* puiErrorCode = 0, bool bLoadOutOfDate = false);

    /**
        Attempt to load PhysX data from disk. If data could not be loaded for
        any reason, the function will return false without creating any PhysX 
        actors.

        @param pkSector Terrain sector for which to create PhysX data
        @param pkScene PhysX scene to which created PhysX actors will be 
            added
        @param kActorArray Populated with a pointer to each PhysX actor that
            was added to the PhysX scene
        @param kActorDesc PhysX Actor Descriptor used to create the PhysX 
            actors
        @param puiErrorCode returns a bitwise combination of 
            NiTerrainPhysXUtils::ReturnCode. 
        @param bLoadOutOfDate If true, force loading of PhysX data from disk
            even if it is out of date. False by default, which will cause the
            function to abort loading if the file is out of date.
    */
    bool LoadPhysXActors(NiTerrainSector* pkSector, NiPhysXScene* pkScene, 
        NiTPrimitiveArray<NxActor*>& kActorArray, NxActorDesc kActorDesc,
         NiUInt8* puiErrorCode = 0, bool bLoadOutOfDate = false);

    /**
        Saves the PhysX data of a sector to disk in the sectors data directory.

        @param pkSector The sector who's PhysX data will be saved.
    */
    bool SavePhysXData(NiTerrainSector* pkSector);

    //{@
    /// Static variables holding the names of the meta data to be used for
    /// creation of NxMaterial
    static NiFixedString ms_kStaticFriction;
    static NiFixedString ms_kDynamicFriction;
    static NiFixedString ms_kRestitution;
    //@}
    
private: 

    /// A map that associate a surface with a NxMaterialIndex
    NiTMap<const NiSurface*, NiUInt16> m_kMaterialIndexMap;

    /// Holds the file format version.
    static const NiUInt32 ms_uiCurrentVersion;

    /**
        Function to generate the PhysX filename from a sector path.
    */
    NiString GetPhysXFilename(const NiFixedString &kSectorPath);

    /**
        Function that actually creates the NxHeightfield for a given Data leaf
        It will not only work out the heights from the terrain's position 
        stream but sets the material index to use for each height field tile.
        Returns the created NxActor
    */
    NxActor* BuildPhysXData(NiPhysXScene* pkScene, NxActorDesc kActorDesc,
        NiTerrainDataLeaf* pkQuadLeaf);

    /**
        This function handles the creation of PhysX data for a sector.
        It will recursively step through all the blocks finding the high LOD
        block from which to create a PhysX height field. 
        
        For each max detail block, NiTerrainBlock::BuildPhysXData is called
        which creates the PhysX data.

        @param spScene PhysX scene to which created PhysX actors will be 
            added
        @param kActorDesc PhysX Actor Descriptor used to create the PhysX 
            actors
        @param kActorArray Populated with a pointer to each PhysX actor that
            was added to the PhysX scene
        @param pkQuadLeaf Current position in a sectors quad-tree
    */
    bool CreatePhysXActor(NiPhysXScene* spScene, NxActorDesc kActorDesc, 
        NiTPrimitiveArray<NxActor*>& kActorArray, 
        NiTerrainDataLeaf* pkQuadLeaf);

    /**
        Calculates which is the dominant surface at a given point on a block
        and returns the NxMaterial index associated with that surface. Since 
        surface layers are subject to an order of priority, each layer's mask 
        value is weighted according to the surface's priority.
    */
    //@{
    void GetDominantPhysXMaterialIndex(NiPhysXScene* pkScene,
        NiTerrainDataLeaf* pkQuadLeaf, NiUInt32 x, NiUInt32 y, bool bTessFlag, 
        NiUInt32& uiMatIndex0, NiUInt32& usMatIndex1);

    void GetDominantSurface(NiTerrainDataLeaf* pkQuadLeaf,
        const NiTMap<const NiSurface*, NiUInt16>& kSurfaceMapIndex, 
        NiUInt32 x, NiUInt32 y, bool bTessFlag, 
        NiUInt16& usMatIndex0, NiUInt16& usMatIndex1);
    //@}

    /**
        Return the NxMaterial index that corresponds with the given terrain
        material.

        If no NxMaterial exists, one will be created and bound to the given
        terrain material for future use.

        @note This function allows quick resolution of meta data values that 
            will be used for the collision response during the PhysX simulation 
            step. Only NX_RESTITUTION, NX_STATICFRICTION and NX_DYNAMICFRICTION
            terrain meta data types are supported.
    */
    NiUInt16 GetPhyXMaterialIndex(NiPhysXScene* pkScene,
        const NiSurface* pkSurface);	
    
    /**
        This function reads a section of the file corresponding to one block.
        For each block it will create a PhysX actor and add it to the PhysX 
        scene.

        @return the created actor.
    */
    NxActor* LoadBlockData(NiTerrainSector* pkSector, NiPhysXScene* pkScene, 
        NiFile* pkFile, const NiTMap<NiUInt16, NiFixedString>& kPackageMap,
        const NiTMap<NiUInt16, NiFixedString>& kSurfaceMap, 
        NxActorDesc kActorDesc);  

    /**
        Saves created PhysX data for the given terrain block to the given file
        at the current file cursor.
    */
    bool SaveBlockData(NiFile* pkFile, 
        const NiTMap<const NiSurface*, NiUInt16>& kSurfaceMapIndex,
        NiTerrainDataLeaf* pkLeaf);

};

NiSmartPointer(NiTerrainPhysXUtils);

#endif