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

#ifndef NITERRAIN_H
#define NITERRAIN_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiEntityComponentInterface.h>
#include <NiFlags.h>

#include "NiTerrainLibType.h"
#include "NiMetaDataStore.h"
#include "NiSurfacePalette.h"
#include "NiRay.h"
#include "NiTerrainSector.h"

class NiTerrainInteractor;
class NiTerrainConfiguration;

/**
    The terrain component is attached to an NiEntity. It manages a loaded 
    terrain, but does not actually have any geometry associated with it. 
    It holds links to other NiTerrainSectorComponent objects, that hold 
    actual sector geometry
 */
class NITERRAIN_ENTRY NiTerrain : public NiNode
{
    friend class NiTerrainInteractor;
    
    NiDeclareRTTI;

public:

    enum FLAGS
    {
        PROP_STORAGE_FILENAME_CHANGED   = 0x01,
        PROP_SECTORS_CHANGED            = 0x02,
        PROP_NUMLOD_CHANGED             = 0x04,
        PROP_BLOCKSIZE_CHANGED          = 0x08,
        PROP_ENVMAP_CHANGED             = 0x10,
        TRIGGER_UNLOAD                  = 0x20,
        TRIGGER_RELOCATE                = 0x40,
        SETTING_CHANGED                 = 0x80
    };
    
    /// @name Constructors
    //@{

    /** 
        Default constructor. If a null meta data store is given, then the 
        default static meta data store singleton is used.

        @param pkMetaDataStore Optional meta data store that will be used by
        surfaces in this component
    */
    NiTerrain(NiMetaDataStore* pkMetaDataStore = 0);

    /// Destructor
    virtual ~NiTerrain(void);
    //@}

    /**
        Sets/Gets the fogging parameters being applied to the terrain. This is 
        to allow an application to apply the same fogging parameters to other 
        objects in the scene.
        @return The fogging property used by the terrain.
    */
    static NiFogProperty* GetGlobalFogProperty();
    static void SetGlobalFogProperty(NiFogProperty* pkFogProp);

    /// @cond EMERGENT_INTERNAL
    /** @name Sector management */
    //{@

    /**
        Detach a sector component from this terrain component.
        @param pkSector sector component to detach from this terrain component
    */
    void RemoveSector(NiTerrainSector* pkSector);

    /**
        Attach a sector to this terrain component.
        @param pkSector sector component to attach to this terrain component
    */
    void AddSector(NiTerrainSector* pkSector);

    /**
        Get the sector at a specified index.
    */
    NiTerrainSector* GetSector(NiInt16 iSectorX, NiInt16 iSectorY);

    /** 
        Create a sector with the specified index
    */
    NiTerrainSector* CreateSector(NiInt16 iSectorX, NiInt16 iSectorY);

    /**
        Update the list of sectors with this list, any sectors not already
        a part of the terrain will be created and any removed will be unloaded
        from memory
    */
    void UseSectorList(const NiTPrimitiveSet<NiUInt32>& kSectorList);

    /**
        @return the number of attached sector components that currently have 
        data loaded in memory.
    */
    NiUInt32 GetNumLoadedSectors() const;

    //@}
    /// @endcond

    /** @name Meta Data */
    //@{

    /**
        Define the meta data store that the terrain should use when loading
        surface packages to disk. If the meta data store is set to NULL, then
        the default static store is used.
    */
    //@{
    inline NiMetaDataStore* GetMetaDataStore() const;
    inline void SetMetaDataStore(NiMetaDataStore* pkMetaDataStore = 0);
    //@}
    //@}

    /** @name Surfaces */
    //@{

    /**
        The active surface palette is defined to be the default static surface 
        palette, unless a specific palette has been assigned to this component 
        directly via SetSurfacePalette.

        @returns The active surface palette for this component.
    */
    inline NiSurfacePalette* GetActiveSurfacePalette() const;

    /**
        Return the custom surface palette that has been optionally assigned to 
        this component. If no palette has been assigned, and the component is 
        using the default static palette, this function returns NULL.

        @return The custom surface palette currently in use by this component
        (if any)
    */
    inline NiSurfacePalette* GetSurfacePalette() const;

    /**
    @return the default static surface palette singleton
    */
    inline static NiSurfacePalette* GetStaticSurfacePalette();

    /**
        Optionally set the surface palette that any new surfaces created by 
        this component will be added to. If no custom surface palette is 
        defined, the component will add all surfaces to the static palette 
        singleton.

        @param spSurfacePalette Custom surface palette to add new surfaces too
    */
    inline void SetSurfacePalette(NiSurfacePalette* spSurfacePalette);
    //@}

    /**
        Causes all attached sectors to unload all mesh and surface data. The
        next time a sector is updated, it will then reload its data from disk.
    */
    void Unload();
    
    /**
        This function will return true if during the last update a deformation
        or translation/scaling/rotation of the terrain occured. This
        function is useful in determining when to update any water objects
        associated with this terrain.
     */
    //{@
    inline bool HasShapeChangedLastUpdate();
    inline void SetShapeChangedLastUpdate(bool bChanged);
    //@}

    /**
        Generate the absolute path to the surfaces used for this terrain
        based upon the terrain path given.
    */
    const NiFixedString CreateSurfacePath(
        const NiFixedString& kTerrainPath);

    /**
        Trigger a save of all attached sectors data to disk, in the directory 
        and format currently specified by this component.

        @param pcFileName The archive directory to save to. If this value is
            0 then the currently set archive directory will be used. 
        @param puiErrorCode Pointer to the OR'd mask of error codes from the
            operation. Error codes are enumerated in NiTerrainSector.
    */
    bool Save(const char* pcFileName = 0, NiUInt32* puiErrorCode = 0);

    /**
        Trigger a load of all attached sectors data from disk, according to 
        to the format currently specified.3

        @param pcFileName The archive directory to load from. If this value is
            0 then the currently set archive directory will be used. 
        @param puiErrorCode Pointer to the OR'd mask of error codes from the
            operation. Error codes are enumerated in NiTerrainSector.
    */
    bool Load(const char* pcFileName = 0, NiUInt32* puiErrorCode = 0);

    /**
        The path containing all the sector folders to be used by this terrain.
        An update must be executed on the terrain for changes to this value
        to be reflected in the sectors.

        @note during runtime, changing the storage filename could cause a 
        large amount of loading to occur on the next update. It is recommended
        set the storage filename once upon initialisation of the terrain.
    **/
    //{@
    inline const NiFixedString& GetArchivePath() const;
    inline void SetArchivePath(const NiFixedString& kArchive);
    //@}

    /// the maximum number of sectors in the X direction.
    //{@
    inline NiUInt32 GetSectorsX() const;
    inline void SetSectorsX(NiUInt32 uiSectorsX);
    //@}

    /// The maximum number of sectors in the Y direction.
    //{@
    inline NiUInt32 GetSectorsY() const;
    inline void SetSectorsY(NiUInt32 uiSectorsY);
    //@}

    /**
        The factor used to adjust the LOD viewable at different distances. 
        Values above 1 adjust the LOD ranges geometrically, increasing distant
        and foreground detail equally, whilst values below 1 linearly shift the
        LOD ranges closer to the camera, until only lower levels of detail 
        can be seen.
    */
    //{@
    inline void SetLODScale(float fScale);
    inline float GetLODScale() const;
    inline float GetLODShift() const;
    //@}

    /**
        Use these functions to control the method used to perform LOD 
        ransitions on the terrain. The following modes are supported:
         LOD_MODE_2D - Uses the camera's X and Y position to determine
          the level of detail to display. Z is ignored. 
         LOD_MODE_25D - Uses the camera's X and Y position as in 2D mode, 
          but a maximum level of detail is placed on the terrain
          according to the camera's Z height.           
         LOD_MODE_3D - Uses the camera's XYZ position to determine the 
          level of detail to display. This mode may cause cracks and popping
          to appear on terrains that have quite steep regions. 
    */
    //@{
    inline NiUInt32 GetLODMode() const;
    inline bool SetLODMode(NiUInt32 uiLODMode);
    //@}

    /**
        The size of each quadtree block in the terrain in quads. The width
        of the block in vertices will be (BlockSize + 1). 
        
        @note a each block shares vertices with neighbouring blocks. 
    **/
    //{@
    inline NiUInt32 GetBlockSize() const;
    inline bool SetBlockSize(NiUInt32 uiBlockSize);
    //@}

    /**
        The number of levels of detail to allow per sector NOT including 
        the highest level of detail. i.e. a setting of 0 effectivly makes
        the terrain a simple heightfield. 
    **/
    //{@
    inline NiUInt32 GetNumLOD() const;
    inline bool SetNumLOD(NiUInt32 uiNumLOD);
    //@}

    /**
        Get and Set function for the sector mask size.
    */
    //{@
    NiUInt32 GetDefaultMaskSize() const;
    bool SetDefaultMaskSize(NiUInt32 uiMaskSize);
    //@}

    /**
        The size of a sector in verts based upon the Blocksize and 
        NumLOD settings. The calculated sector size is calculated using
        the following formula: BlockSize * (2 ^ NumLOD) + 1.
    **/
    NiUInt32 GetCalcSectorSize() const;    

    /**
        The NiTerrainInteractor object that should be used to interact with 
        this terrain. 
    **/
    NiTerrainInteractor* GetInteractor();

    // Don't show documentation for these functions
    /// @cond EMERGENT_INTERNAL
    static void _SDMInit();
    static void _SDMShutdown();

    /** 
        Modify the default behaviour of NiNode so that all bounding information
        is retrieved from the base level of detail. 

        @see NiNode
    */
    //{@
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    //@}

    /**
        Trigger an update of internal data, if required. All data loading,
        preparation and cache updates are performed within this function.
        If any deformation has occurred on the terrain, Update MUST be called
        before any attempt to render. Note this function does not propagate
        the update to children and is only public to allow NiTerrainComponent
        access to it.
    */
    void DoUpdate(bool bUpdateWorldData = false);

    /** 
        Controls the way a sector's visible set is built when using the
        NiNode system.

        @see NiNode
    */
    virtual void OnVisible(NiCullingProcess& kCuller);

    /**
        Modify a vertex on a specific sector. This method also checks if this
        vert is shared by other sectors and propagates the changes to those 
        sectors if necessary.
    */
    void ModifyVertexHeightFrom(NiTerrainSector* pkSector, 
        const NiTerrainVertex& kVertex, float fNewHeight, 
        bool bModifyParentLOD = false);

    static const NiTerrainConfiguration GetDefaultConfiguration();
    static void SetDefaultConfiguration(NiTerrainConfiguration kConfig);

    const NiTerrainConfiguration GetConfiguration() const;
    /// @endcond

protected:

    // Bit flags to store property uniqueness
    NiDeclareFlags(NiUInt8);

    // Component Management
    NiMetaDataStore* m_pkMetaDataStore;

    // Surfaces
    NiSurfacePalettePtr m_spCustomSurfacePalette;

    // Global fog property.
    static NiFogPropertyPtr ms_spFogProperty;

    // Has the shape changed during the last update?
    bool m_bHasShapeChangedLastUpdate;

    // Local 'working copy' of terrain details. Changes to these need to be
    // 'committed' to the terrain when changed.
    NiFixedString m_kArchivePath;
    NiUInt32 m_auiSectors[2];
    NiUInt32 m_uiBlockSize;
    NiUInt32 m_uiNumLOD; 
    NiUInt32 m_uiMaskSize;
    float m_fLODScale;
    float m_fLODShift;
    NiUInt32 m_uiLODMode;
    static NiTerrainConfiguration ms_kDefaultConfiguration;

    // Terrain Interactor
    NiTerrainInteractor* m_pkTerrainInteractor;

    // Member sector information
    NiTMap<NiUInt32, NiTerrainSector*> m_kSectors;

    /**
        Returns whether or not the settings have changed since the last update
    */
    bool HasSettingChanged() const;

    /**
        Update a single sector with the settings for this terrain
    */
    inline void UpdateSector(NiTerrainSector* pkSector);

    /**
        Initialize the terrain's basic values. Called by the constuctor
        of the NiTerrain.
    **/
    void Initialize();
};

NiSmartPointer(NiTerrain);

#include "NiTerrain.inl"

#endif // NITERRAINCOMPONENT_H
