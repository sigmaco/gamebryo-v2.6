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

#ifndef NITERRAINSECTOR_H
#define NITERRAINSECTOR_H

#include "NiTerrainLibType.h"

#include "NiTerrainSectorData.h"
#include "NiTerrainBlock.h"

#include "NiRay.h"
#include "NiTerrainDataLeaf.h"
#include "NiDeformablePointSet.h"
#include "NiSurfaceMask.h"
#include "NiMetaDataStore.h"
#include "NiSurfacePalette.h"
#include "NiTerrainUtils.h"
#include "NiTerrainDecalManager.h"
#include "NiTerrainSectorFile.h"
#include "NiIndex.h"

#include <NiPoint4.h>
#include <NiExternalAssetManager.h>

class NiTerrain;
class NiSurfaceMaskPixel;
class NiTerrainDecal;

NiSmartPointer(NiTerrainSector);

/**
    The Terrain Sector is a manager class for a collection of terrain blocks,
    contained within a quadtree.
    
    A NiTerrainSector object keeps track of data streams, decals, surface
    masks and the quad-trees of blocks themselves.

    This class should not be interacted with directly by the end application
    for anything other than attaching dynamic effects, lights, shadowing etc.
 */
class NITERRAIN_ENTRY NiTerrainSector : public NiNode 
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;

    /// @endcond

public:

    enum ErrorCode
    {
        /// Defined path does not exist.
        EC_INVALID_ARCHIVE_PATH         = 0x00001,

        /// Failed to open the dof file.
        EC_DOF_INVALID_FILE             = 0x00002,

        /// File is not of the latest supported version.
        EC_DOF_OUTOFDATE                = 0x00004,

        /// Sector size and size defined in the file do not correspond.
        EC_DOF_INVALID_SECTOR_SIZE      = 0x00008,

        /// Failed to create the necessary data streams.
        EC_DOF_STREAM_CREATION_ERROR    = 0x00010,

        /// File has missing data.
        EC_DOF_MISSING_DATA             = 0x00020,

        /// The file was loaded successfully.
        EC_DOF_LOADED                   = 0x00040,

        /// The file was saved successfully.
        EC_DOF_SAVED                    = 0x00080,

        /// The bounds saved in the file are invalid and need to be recomputed.
        EC_DOF_INVALID_BOUNDS           = 0x00100,

        /// Failed to find material package.
        EC_SURFACE_INVALID_PACKAGE      = 0x00200,

        /// Package name is empty.
        EC_SURFACE_EMPTY_PACKAGENAME    = 0x00400,

        /// Failed to load a surface.
        EC_SURFACE_FAILED_LOADSURFACE   = 0x00800,

        /// Surface has an invalid index.
        EC_SURFACE_INVALID_INDEX        = 0x01000,

        /// Failed to read the surface mask.
        EC_SURFACE_FAILED_READ_MASK     = 0x02000,

        /// Save failed because there is no terrain data.
        EC_SURFACE_INVALID_TERRAIN_DATA = 0x04000,

        /// Failed to create the directory hierarchy.
        EC_SURFACE_FAILED_DIRECTORY     = 0x08000,

        /// Failed to save the material xml file.
        EC_SURFACE_FAILED_SAVE          = 0x10000,           
    };

    /**
        Default constructor.

        The bIsDeformable must be true of the terrain will be modified at
        runtime.
    */
    NiTerrainSector(bool bIsDeformable = false);

    /// Destructor
    virtual ~NiTerrainSector();
    
    /// The Terrain object that this sector belongs to. 
    //{@
    NiTerrain* GetTerrain();
    const NiTerrain* GetTerrain() const;
    void SetTerrain(NiTerrain* pkTerrain);
    //@}

    /**
        Adds all visible blocks to the visible set defined in the given 
        culling process. Frustum culling and quadtree based LOD culling are 
        performed in this function, according to the camera provided by the
        culling process.
        
        @param kCuller The culling process to use during culling. 
        @return True if this sector drew anything.
     */
    bool ProcessLOD(NiCullingProcess &kCuller);
    
    /**
        After calculating the LOD, this function should be called to allow
        appropriate stitching to occur between blocks of diferent LOD's
     */
    void ProcessBorders();

    /**
        All visible decals are culled against the culling process and 
        added to the visible set.
        
        @param kCuller The culling process to use during culling. 
     */
    void ProcessDecals(NiCullingProcess& kCuller);

    /// @cond EMERGENT_INTERNAL
    /** 
        Controls the way a sector's visible set is built when using the
        NiNode system.

        @see NiNode
    */
    virtual void OnVisible(NiCullingProcess& kCuller);
    /// @endcond

    /**
        Spacing, in world coordinates, between two points of the given type
        at the specified detail level.
        
        If multiple spacings exist (as is possible in the case of mask
        pixels), the smallest is used.
        
        @param eType The point type for which spacing is requested.
        @param usDetailLevel The requested level of detail.
        @param fSpacing The returned spacing value.
        @return False if the detail level of type is not defined for the
            sector, true otherwise.
        
    */
    bool GetPointSpacing(NiDeformablePoint::POINT_TYPE eType, 
		float& fSpacing, NiUInt32 usDetailLevel = NiTerrainUtils::ms_uiMAX_LOD) 
        const;
	        
    /**
        Build a vertex from data at the given index and detail level.
        
        This function is computationally expensive and should be avoided in
        critical paths.

        @note The given index must be in the sector scope, at detail level 0.
            If no vertex exists at the given index in the requested detail 
            level, an assertion is thrown.

        @param[out] kVertex Discovered vertex, or undefined if no vertex
            exists at the requested index for this detail level.
        @param kIndex Sector scope index of the vertex.
        @param uiLevel Detail level from which to retrieve the vertex.
    */
    void GetVertexAt(NiTerrainVertex& kVertex, const NiIndex& kIndex, 
        NiUInt32 uiLevel = NiTerrainUtils::ms_uiMAX_LOD) const;

    /**
        Find the world position from vertex data at the given index and detail 
        level.
        
        This function is computationally expensive and should be avoided
        in critical paths.

        @note The given index must be in the sector scope, at detail level 0.
        If no vertex exists at the given index in the requested detail 
        level, an assertion is thrown.

        @param[out] kVertex Discovered position, or undefined if no vertex 
        exists at the requested index for this detail level
        @param kIndex Sector scope index of the vertex from which to retrieve 
        the world position
        @param uiLevel Detail level from which to retrieve the vertex
    */
    void GetVertexAt(NiPoint3& kVertex, const NiIndex& kIndex, 
        NiUInt32 uiLevel = NiTerrainUtils::ms_uiMAX_LOD) const;

    /**
        Get the height value of the vertex specified by the given sector
        scope index, at the specified detail level.
        
        If the given index is invalid at the specified detail level, an
        assertion is thrown and the height is undefined.
    */
    float GetHeightAt(const NiIndex& kIndex, 
        NiUInt32 uiLevel = NiTerrainUtils::ms_uiMAX_LOD) const;

    /**
        Find the X and Y position of the sector in the terrain list.
    */
    void GetSectorIndex(NiInt16& sXIndex, NiInt16& sYIndex) const;
 
    /**
        Set the X and Y position of the sector in the terrain list.
    */
    void SetSectorIndex(NiInt16 sXIndex, NiInt16 sYIndex);

    /**
        Get the sector that borders this sector according to the 
        border description passed in. The border descriptions are 
        identical to those used for NiTerrainDataLeafs.
    */
    NiTerrainSector* GetAdjacentSector(NiUInt32 uiBorder) const;
    
    /**
        A function to return the path to the terrain archive that this
        sector belongs to. 
     */
    inline const NiFixedString& GetArchivePath() const;

    /**
        Set the terrain archive path for this sector to use whenever loading
     */
    void SetArchivePath(NiFixedString kArchivePath);

    /**
        A function to return the path to this sector.
        
        Leaf the pcArchive parameter as 0 to use the currently set archive
        path as the base for the new path.

        @param pcArchive The directory in which the archive is located.
        @return A path name, starting with pcArchive, to use for loading the
            sector.
    */
    inline NiFixedString GetSectorPath(const char* pcArchive = 0) const;

    /**
        A function to return the path to this sector.
        
        Leaf the pcArchive parameter as 0 to use the currently set archive
        path as the base for the new path.

        @param pcArchive The directory in which the archive is located.
        @return A path name, starting with pcArchive, to use to search for
            surface packages.
    */
    inline NiFixedString GetSurfacePath(const char* pcArchive = 0) const;

    /**
        Define the meta store that should be used by the terrain when loading
        surfaces.
        
        If a NULL meta data store is assigned, the default static
        singleton meta data store is used.
    */
    inline void SetMetaDataStore(NiMetaDataStore* pkMetaDataStore);

    /**
        Adds the specified decal to this sector.
        
        When the sector is next updated, the decals mesh will be created and
        attached to the quad mesh.
    */
    void AddDecal(NiTerrainDecal* pkDecal);

    /**
        Return true if this sector is currently using a 16 bit index buffer,
        false if using 32 bit.
    */
    NiBool GetUsingShortIndexBuffer();

    /**
        Return true if this sector is currently using a 16 bit index buffer,
        false if using 32 bit (const version).
    */
    NiBool GetUsingShortIndexBuffer() const;

    /**
        This function will return true if during the last update a deformation
        or translation/scaling/rotation of the terrain occurred. This
        function is useful in determining when to update any water objects
        associated with this terrain.
    */
    bool HasShapeChangedLastUpdate();
    NiBool HasChanged();

    /**
        Flag this sector as having changed in the last update.
    */
    void SetShapeChangedLastUpdate(bool bChanged);

    /**
        Recursive function that marks the given vertex as changed and 
        modifies the quad mesh accordingly.
        
        If bModifyParentLOD is set to true, all parent leaves that contain
        the given vertex will also be modified.
    */
    void ModifyVertexHeightFrom(const NiTerrainVertex& kVertex, 
        NiTerrainDataLeaf* pkCurrentLeaf, float fNewValue, 
        bool bModifyParentLOD = false);

    /**
        Recursive function that marks the given mask pixel as changed and 
        modifies the corresponding mask accordingly.
        
        If bModifyParentLOD is set to true, all parent leaves that contain
        the given mask pixel will also be modified.
    */
    void ModifySurfaceMaskFrom(const NiSurfaceMaskPixel* pkPixelData,
        NiTerrainDataLeaf* pkFirstLeaf, NiUInt8 ucNewValue, 
        const NiSurface* pkSurfaceToPaint = 0, bool bModifyParentLOD = false);
    
    /**
        Gather a set of deformable points that contain pixels within the
        specified world space bound. 

        Optionally gather deformable points from a specific detail level and 
        lower (i.e. a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
        vertices
        @param pkPointSet Deformable point set to add found pixels too
        @param pkOriginalPointPool Deformable point pool to allocate points 
        from
        @param pkSurface Gather pixels from the surface mask that represents
        this surface
        @param bCreateMasksIfNotExist If no surface mask exists for the given
        surface on any block within the given radius, create a mask for
        those blocks
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeLowerDetail Optionally toggle adding points from detail
        levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetPixelsInBound2D(const NiBound &kBound, 
        NiDeformablePointSet* pkPointSet, 
        NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool, 
        const NiSurface* pkSurface,
        bool bCreateMasksIfNotExist,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true);

    /// This function does nothing and always returns 0.
    NiUInt32 GetPixelsInBound(const NiBound &kBound, 
        NiDeformablePointSet* pkPointSet, 
        NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool, 
        const NiSurface* pkSurface,
        bool bCreateMasksIfNotExist,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true);

    /**
        Retrieve the opacity (from 0 to 255) of the given surface on this 
        sector at the local position derived from the specified world location.
        
        Only the X and Y components of the local position are taken into 
        account. If no valid surface mask is found at the given location for
        the specified detail level and bIncludeLowerDetail is set to true, the
        function will search up the tree for the first valid mask to use.

        @param pkSurface Surface to find surface mask for.
        @param kWorldLocation World location, which will be converted to 
            terrain space for checking X and Y against the terrain.
        @param ucValue Opacity (0 to 255) of the found mask pixel, or 0 if
            no valid mask was found.
        @param uiDetailLevel Detail level to check for a mask.
        @param bSearchLowerDetail If true, search up the quadtree for a valid
            mask if none is found at the specified level.

        @return true if a valid mask pixel was found.
    */
    bool GetSurfaceOpacity(const NiSurface* pkSurface, 
        const NiPoint3& kWorldLocation, 
        NiUInt8& ucValue,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bSearchLowerDetail = true) const;

    /**
        Retrieve the opacity (from 0 to 255) of the surface on this sector at 
        the pixel adjacent to the given pixel, in the given direction.

        If the adjacent pixel is beyond the scope of the given pixels mask, the
        function will attempt to find a neighboring mask first down, then up
        the tree.

        @param kPixel Pixel to find neighboring pixel from.
        @param pkSurface Surface to query.
        @param eDirection Adjacency direction.
        @param ucValue opacity (0 to 255) of the found mask pixel, or 0 if
            no valid mask was found.

        @return true if a valid mask pixel was found.
    */
    bool GetAdjacentSurfaceOpacity(
        const NiSurfaceMaskPixel& kPixel, const NiSurface* pkSurface, 
        NiTerrainDataLeaf::BORDERS eDirection, NiUInt8& ucValue) const;

    /**
        Make sure that a mask exists for each leaf within the given world
        space bound at the given detail level, for the specified surface.
        
        Any leaf which does not have a valid mask will have one allocated.
    */
    void EnsureMaskExists(const NiBound& kBound, const NiSurface* pkSurface, 
        NiUInt32 uiDetailLevel, NiTerrainDataLeaf* pkCurrentLeaf = 0);

    /**
        This will cause all blocks using the given surface to update any shader
        maps and constants derived from the surface.
    */
    void MarkSurfaceChanged(const NiSurface* pkSurface);

    /**
        Remove the given surface from this terrain, also removing associated
        masks.
        
        All surfaces below the given surface will be moved up to fill the gap.
    */
    void RemoveSurface(const NiSurface* pkSurface);

    /**
        Does this sector require a lighting rebuild?

        @return True if the sector requires a lighting rebuild. 
    */
    bool RequiresLightingRebuild() const;

    /**
        Mark this sector's lighting as requiring a rebuild.

        @param bOutdated True if the lighting is outdated. 
    */
    void SetLightingOutdated(bool bOutdated);

    /**
        Retrieves a list of leaves that require their lighting to be 
        rebuilt. The parameter should be set to true if all leaves should
        have their lighting rebuilt.

        @param pkChanged A pointer to a list to populate with the changed 
            leaves.
        @param bAllLeaves True if all leaves of the sector are to be added. 
    */
    void GetOutdatedLightingLeafList(
        NiTPrimitiveArray<NiTerrainDataLeaf*>* pkChanged, 
        bool bAllLeaves = false);

    /**
        Do a complete rebuild of lighting - normals, tangents.
        
        This function will delete the data streams and create new streams.
    */
    void RebuildAllLighting();

    /**
        Rebuild the lighting only on blocks that have outdated lighting (ie,
        they have been deformed since the last lighting build).
        
        Only blocks with dynamic streams can be affected by this function.
    */
    void RebuildChangedLighting();

    /// @cond EMERGENT_INTERNAL
    /**
        Tells the contained sector to save to the currently defined location.
        
        @param pcArchive The archive directory to save the sector to. Set to
            0 to use the currently stored archive directory.
        @param puiErrorCode A pointer to an integer to receive error flags.
    */
    bool Save(const char* pcArchive = 0, NiUInt32* puiErrorCode = 0);

    /**
        Save all surface masks from memory to disk in the current sector path.

        @note This functions assumes that all directories already exist.

        @param pcArchive Path that all surface package references will be 
        relative too.

        @return True if surface masks for this and all child levels were saved
        successfully, otherwise false.
    */
    bool SaveSurfaceMasks(const char* pcArchive, NiUInt32* puiErrorCode = 0);

    /**
        Save the quad data to the specified directory.

        @note This function does not save surface masks.

        @param pcArchive The archive directory to save to.
    */
    bool SaveToFile(const char* pcArchive, NiUInt32* puiErrorCode = 0);

    /**
        Tells the contained sector to load from the currently defined location.
        
        @param pcArchive The archive directory to load the sector from. Set to
            0 to use the currently stored archive directory.
        @param puiErrorCode A pointer to an integer to receive error flags.
    */
    bool Load(const char* pcArchive = 0, NiUInt32* puiErrorCode = 0);

    /**
        Tells the contained sector to unload all the data that has been 
        calculated and allocated to it. 
     */
    void Unload();

    /**
        Load quad data from the custom format file, building the necessary
        quad tree structure and meshes.

        @param kFileName Path to the file with which to load.
        @param pkMetaDataStore Meta data store which is populated with all 
            meta data found on the surfaces referenced by the sector.
    */
    bool LoadFromFile(const NiFixedString &kFileName,
		NiUInt32 uiMaxDetailLevel, NiUInt32* puiErrorCode = 0);
	bool LoadFromFile(NiTerrainSectorFile *pkSectorFile,
		NiUInt32 uiMaxDetailLevel, NiUInt32* puiErrorCode = 0);

	/** 
		Based upon the target LOD level, LOD levels will be either loaded
		or unloaded appropriately. Target LOD level is generally controlled
		by an external dynamic loader.

        @note Update must be called on this sector after LOD data has been
            updated to allow texturing on newly loaded blocks to be valid 
            before rendering.
	 */
	void UpdateLODData();

    /// @endcond

    /**
        Read the height values from the maximum detail level of the terrain
        into the specified buffer.

        The count, minimum and maximum of added
        values are passed to the corresponding out parameters.

        @param pfMap User allocated buffer to read height-map values into.
        @param stBufferSize Size in bytes of pfMap.
        @param uiNumVal Number of values added to the buffer.
        @param fMinVal The minimum value that was added to the buffer.
        @param fMaxVal The maximum value that was added to the buffer.

        @return True if all values were read into the buffer successfully,
            otherwise false.
    */
    bool GetHeightMapValues(float*& pfMap, size_t stBufferSize, 
        NiUInt32& uiNumVal, float& fMinVal, float& fMaxVal);

    /** 
        Sets the heights of the sector point to the value in the map parameter.

        Calling this function assumes a sector has already been created
        and the map is at least big enough to cover the whole sector. Will
        return false if numVal isn't equal to the number of vertices in the
        sector. This also assumes the values in map are already sensible.

        @param pfMap Contains the values that WILL be set as height for the 
            vertices in the sector.
        @param stBufferSize Size in bytes of pfMap.
        @param uiNumVal The number of values contained in the map.
        @param bSmoothLowerLevels If set to true, lower levels of detail will 
            be smoothed to decrease artifacts generated by the detail loss.
        @return True if succeeded, false otherwise.
    */
    bool SetHeightFromMap(float* pfMap, size_t stBufferSize, NiUInt32 uiNumVal,
        bool bSmoothLowerLevels = false);

    /**
        An entry point into the loading system which creates a terrain with a 
        continuous 0.0 height.
        
        The size of the created terrain is defined from the current block
        size and LOD level settings defined in the sector data.
    */
    bool CreateBlankGeometry();

    /// Get the current surface palette in use by this sector.
    inline NiSurfacePalette* GetSurfacePalette() const;

    /// Set the surface palette for use by this sector.
    inline void SetSurfacePalette(NiSurfacePalettePtr spPalette);

    /**
        Load surfaces for this sector from the masks and data files located
        at the given sector directory.

        @param spSurfacePalette Surface palette to load surface packages 
            through.
        @param pcArchive Sector directory to load masks from.
        @param uiDetailLevel The number of LOD levels.
        @param puiErrorCode Pointer to the OR'd mask of error codes from the
            operation.
    */
    void LoadSurfaces(const NiSurfacePalettePtr& spSurfacePalette,
        const char* pcArchive, NiUInt32 uiDetailLevel, 
        NiUInt32* puiErrorCode = 0);

    /**
        Set the default mask size.
        
        All new surface masks will be created with this as their side length.
 
        @note Side length of a mask must be a power of 2
    */
    void SetDefaultMaskSize(NiUInt32 uiSize);

    /**
        Get the default mask size.
    */
    NiUInt32 GetDefaultMaskSize() const;

    /// @cond EMERGENT_INTERNAL

    /// Should be called whenever a mask is removed from a certain detail level
    void RecalculateBestPixelSpacing(NiUInt32 uiDetailLevel);

    /// Should be called whenever a surface mask is added to the given leaf
    void UpdateBestPixelSpacing(const NiTerrainDataLeaf* pkLeaf);

    /// Smooth the lower LOD levels to remove artifacts caused by detail loss
    /// at these levels.
    void SmoothLowerLOD();

    /**
        Calculates the normals and tangents for all leaves in the given array.
        Where applicable, the resulting data is placed in a blocks dynamic
        stream.

        @note If not in tool mode, valid write locks must exist for this 
            sectors static data streams.
     */
    //{@
    void CalculateNormalsTangents(
        const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves);
    void CalculateNormalsTangentsPhase1(
        const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves);
    void CalculateNormalsTangentsPhase2(
        const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves);
    void CalculateNormalsTangentsPhase3(
        const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves);
    //@}

    /**
        Create GeoMorphing positional, normal and tangent data for all leaves
        in the given array from existing position, normal and tangent data.
        Where applicable, the resulting data is placed in a blocks dynamic 
        stream.

        This function assumes that ALL the data streams (including all leaves
        not in the given array) are in our compressed format, ready for the 
        morphing data to be written.
     */
    void CalculateGeoMorph(
        const NiTPrimitiveArray<NiTerrainDataLeaf*>& kLeaves); 

    /// Regenerate blend mask textures for all blocks
    void RegenerateBlendMaskTextures();
    
    /// Register for notification of a DX9 device reset
    void SubscribeToDXDeviceResetNotification();

    /// Unregister from DX9 device reset notifications
    void UnsubscribeToDXDeviceResetNotification();

    /// Handle a DX9 device reset by regenerating the blend mask textures
    static bool HandleDXDeviceReset(bool bBeforeReset, void* pkVoid);

    /**
        Use these values to adjust the level of detail viewable at different 
        distances.

        @param fScale The scale factor for the LOD. Higher is better detail. 
        Must be at least 2.0f * NiSqrt(2.0f)
    */
    //{@
    void SetLODScale(float fScale, float fShift);
    float GetLODShift() const;
    float GetLODScale() const;
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
    NiUInt32 GetLODMode() const;
    bool SetLODMode(NiUInt32 uiLODMode);
    //@}

    /// @endcond


    /**
        Define whether or not this sector can be deformed. Will initialize the
        dynamic stream cache if necessary.
    */
    void SetIsDeformable(bool bIsDeformable);

    /**
        Gather a set of deformable points that contain vertices within the
        specified world space bound

        Optionally gather deformable points from a specific detail level and 
        lower (i.e, a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.
        lower (ie, a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
        vertices
        @param pkPointSet Deformable point set to add found vertices too
        @param pkOriginalPointPool Deformable point pool to allocate points 
        from
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeParentLOD Optionally toggle adding points from detail
        levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetVertsInBound(const NiBound &kBound, 
        NiDeformablePointSet* pkPointSet,
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bIncludeParentLOD = false) const;

    /**
        Gather a set of deformable points that contain vertices within the
        specified world space bound, considering only the 2D projection of the
        bound in the X-Y plane.

        See GetVertsInBound for details.
    */
    NiUInt32 GetVertsInBound2D(const NiBound &kBound, 
        NiDeformablePointSet* pkPointSet,
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bIncludeParentLOD = false) const;

    /**
        Gather a set of deformable points that contain vertices within the
        specified abstract bound.

        See GetVertsInBound for details.
    */
    NiUInt32 GetVertsInBound(const NiBoundingVolume &kBound, 
        NiDeformablePointSet* pkPointSet,
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bIncludeParentLOD = false) const;

    /**
        Get the number of triangles that have been rendered since the last
        Update.

        @note This is an estimate and may not be exact, as it does not take 
            border stitching into account.
    */
    NiInt32 GetNumTriangles();

    /**
        Find the data leaf that contains the given index at the specified 
        level.
        
        In border cases where an index may be found in multiple leaves,
        the leaf with the smallest region ID takes precedence.
    */
    inline NiTerrainDataLeaf* GetLeafContaining(const NiIndex& kIndex,
        NiUInt32 uiDetailLevel) const;

    /**
        Find the data leaf that contains the index of the given vertex at the 
        specified level.
        
        In border cases where an index may be found in multiple leaves,
        the leaf with the smallest region ID takes precedence.
    */
    NiTerrainDataLeaf* GetLeafContaining(const NiTerrainVertex& kVertex, 
        NiUInt32 uiDetailLevel) const;

    /**
        Retrieve a non-const pointer to a data leaf corresponding to the given 
        block ID (LOD Space indexing - Siblings are not stored sequentially)
    */
    NiTerrainDataLeaf* GetLeaf(NiUInt32 uiLeafID) const;

    /**
        Retrieve a non-const pointer to a data leaf corresponding to the given 
        region ID. (QuadTree Space indexing ie. First child = uiLeafID * 4 + 1)
     */
    NiTerrainDataLeaf* GetLeafByRegion(NiUInt32 uiLeafID) const;

    /**
        Returns the offset in the internal leaf array for a leaf at the given
        detail level.
        
        This value is used to generate the leaf region ID.
    */
    NiUInt32 GetLeafOffset(NiUInt32 uiDetailLevel) const;

    /**
        Retrieve the sector settings object.
    */
    const NiTerrainSectorData* GetSectorData() const;

    /**
        Change the dimensions of each block, and dictate how many
        levels of LOD exist.
        
        An attempt is made to preserve data, but some detail may be lost.
    */
    void ReformatData(NiUInt32 uiBlockSize, NiUInt32 uiNumLOD);

    /**
        Unload all supporting data that is not tied to a specific level of 
        detail. This function must be called before changing any of the 
        terrain's basic settings (ie. blocksize).
    */
    void DestroySupportingData();
    
    /**
		The level of detail requested to be loaded. Usually one above the 
		LOD required to draw to the screen. MAX if terrain is marked as 
		deformable. If this is less than HighestLODLoaded then levels may
		be unloaded.
		
		@param sTargetLoadedLOD Level of detail to load from file or unload.
		    -1 is nothing loaded from file
		@param bAllowUnloading set to true if unloading of data is allowed. 
	 */
	void SetTargetLoadedLOD(NiInt32 sTargetLoadedLOD, bool bAllowUnloading);

    /**
        Attempt to collide a world space ray with the sector, at the given 
        detail level.
        
        Any collision data is stored in the ray itself.

        @return true if a collision occurred at the given detail level.
    */
    bool CollideWithRay(NiRay& kRay, NiUInt32 uiDetailLOD = 0) const;

    /// @cond EMERGENT_INTERNAL
    inline const NiTerrainConfiguration& GetConfiguration() const;
    inline NiDataStreamElement::Format GetPositionStreamFormat(
        const NiTerrainConfiguration* pkConfiguration = NULL) const;
    inline NiDataStreamElement::Format GetNormalStreamFormat(
        const NiTerrainConfiguration* pkConfiguration = NULL) const;
    inline NiDataStreamElement::Format GetTangentStreamFormat(
        const NiTerrainConfiguration* pkConfiguration = NULL) const;
    /// @endcond
    
protected:

    /// Prevent update interference
    bool m_bUpdating;

    /// Enables recycling of dynamic streams when too many blocks have been 
    /// deformed
    bool m_bRecycleDynamicStreams;

    /// Used when constructing indices
    bool m_bUsingShortIndexBuffer;

    /// Are any block's lighting out of data
    bool m_bLightingOutdated;

    /// Parent Terrain managing this sector
    NiTerrain* m_pkTerrain;
    
    /// Sector Data keeps track of this sectors settings in such a way that 
    /// quadtree nodes can also access it.
    NiTerrainSectorData* m_pkSectorData;

    /// The DX9 Renderer device reset callback index
    bool m_bRegisteredDXDeviceResetCallback;
    NiUInt32 m_uiDXDeviceResetCallbackIndex;

    /// Number of blocks added to the last processed visible set, multiplied by
    /// number of triangles per block
    NiUInt32 m_uiNumRenderedTriangles;

    /// Stores level offsets of blocks in the block array
    NiUInt32* m_puiLeafOffsets; 

    /// A copy of the world space camera, that has been translated to terrain
    /// local space.
    NiCamera* m_pkLocalCamera;

    /// Decal manager for this sector
    NiTerrainDecalManagerPtr m_spDecalManager;

    /// Default surface palette
    NiSurfacePalettePtr m_spSurfacePalette;

    /// Default meta data store used by surfaces
    NiMetaDataStore* m_pkMetaDataStore;

    /// CPU and GPU
    //@{
    NiDataStreamLock* m_pkIndexLock;
    NiDataStreamLock* m_pkUVLock;
    NiTPrimitiveArray<NiDataStreamLock*> m_kPositionLocks;
    NiTPrimitiveArray<NiDataStreamLock*> m_kNormalLocks;
    NiTPrimitiveArray<NiDataStreamLock*> m_kTangentLocks;
    //@}

    /// Current Archive path of the terrain.
    NiFixedString m_kArchivePath;

    /// Data quad tree
    NiTerrainDataLeaf* m_pkQuadData;
    
    /// Geometry quad tree
    NiTerrainBlock* m_pkQuadMesh;
    
    /**
        All allocated surface mask container objects. Does not contain the
        actual mask data itself. The purpose of the mask stack is to allow
        the sector to maintain an authoritative copy of the data at all times, 
        so that it can modify masks belonging to const NiSurfaceMask objects.
     */
    NiSurfaceMask* m_pkSurfaceMaskStack;
  
    /// Current position in the surface mask stack. This will point to the 
    /// first free entry
    NiUInt32 m_uiSurfaceMaskStackPos;

    /// Current size of the surface mask stack, in entries not bytes
    NiUInt32 m_uiSurfaceMaskStackSize;

    /**
        Stores the minimum spacing of mask pixels, at each detail level.
        The minimum spacing corresponds to the best resolution. 
        The values stored in this array are in local coordinates.
     */
    float* m_pfBestPixelSpacingCache;

    /// New masks created for an empty block have this as their X and Y 
    /// dimension
    NiUInt32 m_uiDefaultMaskSize;

    /// Which blocks have which streams in the dynamic cache - used when we 
    /// want to recycle blocks.
    NiTPointerList<NiTerrainBlock*> m_kDynamicBlockQueue;

    /// An array that contains pointers to all leaves loaded in the system.
    /// Level 0 blocks are first, followed by next LOD's
	/// (LOD Space Indexing -- Siblings are not stored sequentially)
    NiTPrimitiveArray<NiTerrainDataLeaf*> m_kLeafArray;
  
	/// An array that contains pointers to all leaves loaded in the system.
	/// Sorted by region ID (QuadTree Space Indexing)
	NiTPrimitiveArray<NiTerrainDataLeaf*> m_kLeafRegionArray;

    /// A boolean that will allow to inform dependant entities (water) that
    /// the terrain has changed and they should be updated
    bool m_bHasShapeChangedLastUpdate;

private:

    /*
        calling order:
        1. CreateStreams()
        2. BuildQuadTree()
        3. BuildMesh()
        4. (populate the position stream with data)
        4a. (optional) SmoothLowerLOD()
        5. BuildData()     
        6. (run the game)
        7. DestroyMesh()
        8. DestroySupportingData()
     */

    /**
        Allocate memory for the data streams to be used to the quad mesh. If 
        tool mode is enabled, no static streams are generated and the dynamic
        stream cache is created with an initial to allow for every block to be 
        dynamic.

        If terrain deformation is not enabled, the dynamic stream cache is not
        created.

		@param uiMaxDetailLevel The number of levels of detail to account for
     */
	//@{
    bool CreateStreams(NiUInt32 uiMaxDetailLevel = NiTerrainUtils::ms_uiMAX_LOD);
	bool CreatePNTStream(NiUInt32 uiMaxDetailLevel = NiTerrainUtils::ms_uiMAX_LOD);
    //@}

    /**
        Allocate memory for the data streams to be used to the quad mesh. If 
        tool mode is enabled, no static streams are generated and the dynamic
        stream cache is created with an initial to allow for every block to be 
        dynamic.

        If terrain deformation is not enabled, the dynamic stream cache is not
        created.
     */
	//@{
	bool CreateUVStream();
	bool CreateIndexStream();
	//@}

    /**
        Allocates a dynamic stream cache to be used by a deformable terrain.
     */
    bool CreateDeformationCache(NiUInt32 uiCacheSize,
        const NiDataStreamElementSet& kElementSetV, 
        const NiDataStreamElementSet& kElementSetNT);

    /**
        Create the NiTerrainBlock hierarchy and assign regions from the 
        appropriate data streams to each leaf.

        If the sector is in tool mode, each block is assigned a dynamic data
        stream, otherwise a region from the static stream is used.
     */
    void CreateBlockHierarchy();

    /**
        Generates normal, tangent, index and UV data from existing positional
        data for all leaves in the tree. This function will recognize that a
        block is currently using a dynamic stream and write to that where
        possible.

        @param uiMaxDetailLevel the LOD level we require to be completed up
			to. Will build the data for every block between the currently 
			loaded LOD and this new supplied level of detail.

        @note This function assumes write locks exist for all streams that will
            be used.
     */
	void BuildData(NiUInt32 uiMaxDetailLevel = NiTerrainUtils::ms_uiMAX_LOD);

    /**
        This is a fast function to create bounding spheres for all the leaves.
     */
    void BuildBounds();

    /// @cond EMERGENT_INTERNAL
    /**
        Override the normal bounds calculation to use the pre-cached bound info
        in the quadtree.
     */
    //{@
    virtual void UpdateNodeBound();
    virtual void UpdateWorldBound();
    //@}
    
    /** 
        Modify the default behaviour of NiNode so that all bounding information
        is retrieved from the base level of detail. 

        @see NiNode
    */
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    /// @endcond

    /**
        Generates default index data (for all 9 stitching states) into the
        stream associated with the given lock.

        @note The given lock MUST be a write lock.
     */
    void CalculateIndices(NiDataStreamLock* pkIndexLock);

    /**
        Template helper function used by CalculateIndices to add a single 
        triangle to the given stream.

        @param x0 
        @param y0 
        @param x1 
        @param y1 
        @param x2 
        @param y2 
        @param kIterator Adds the triangle to the beginning of this
            iterator, which is then incremented by the size of the inserted
            triangle.

        @note The given iterator will be incremented by 3 within this function.
     */
    template <typename T>
    void AddTri(NiUInt32 x0, NiUInt32 y0, NiUInt32 x1, NiUInt32 y1, 
        NiUInt32 x2, NiUInt32 y2, 
        NiTStridedRandomAccessIterator<T>& kIterator);

    /**
        Generates default UV data into the stream associated with the given 
        lock.

        @note The given lock MUST be a write lock.
     */
    void CalculateUV(NiDataStreamLock* pkUVStream); 

    /**
        Rebuild the morphing only on blocks that have outdated data (ie,
        they have been deformed since the last morphing build). Only blocks
        with dynamic streams can be affected by this function.
     */
    void RebuildChangedMorph();

    /**
        Creates the internal quad data and mesh tree, pointing to relevant 
        dynamic or static stream data. This new tree will replace any tree that
        already existed.

		@param uiMaxDetailLevel the LOD level we require to be completed up
			to. Will build the tree for every block between the currently 
			loaded LOD and this new supplied level of detail.
     */
    void BuildQuadTree(NiUInt32 uiMaxDetailLevel = NiTerrainUtils::ms_uiMAX_LOD);

    /**
        Traverses the quad tree building meshes for every block using its
        corresponding data streams.

		@param uiMaxDetailLevel the LOD level we require to be completed up
			to. Will build meshes for every block between the currently 
			loaded LOD and this new supplied level of detail.
     */
	void BuildMesh(NiUInt32 uiMaxDetailLevel = NiTerrainUtils::ms_uiMAX_LOD);

    /**
        Destroy the quad mesh tree, leaving the quad data tree intact.
    */
    void DestroyMesh();
   
    /**
		Unload the data contained in the specified LOD level, higher
		levels of detail will also be unloaded. All data is unloaded
		when asked to unload level 0.
	 */
	void UnloadLODData(NiUInt32 uiUnloadLODLevel = 0); 

	/** 
		Load the data for the specified LOD level, lower levels of detail
		will also be loaded. By default the maximum levels of detail
		will be loaded
	 */
	bool LoadLODData(NiUInt32 uiLoadLODLevel = NiTerrainUtils::ms_uiMAX_LOD,
        NiUInt32* puiErrorCode = 0, const char* pcArchive = 0);
   
    /** 
        Recursively load surfaces and masks for the given data leaf. All paths
            should have a trailing slash, and are assumed to be valid.
        
        @param spSurfacePalette The active surface palette which will be used 
            to load surface packages
        @param pcCurrentDir Current directory.
        @param pcSurfaceDir The directory from which the surface package
            references within the terrain file are relative too.
        @param pkCurrentLeaf The leaf for which to load the surface data
        @param kDom DOM tool that will be used within the function. It is
            re-initialized within the function.
        @param uiDetailLevel The number of LOD levels.
        @param puiErrorCode Pointer to the OR'd mask of error codes from the
            operation.
     */
    void LoadSurfacesRecursive(
        const NiSurfacePalettePtr& spSurfacePalette,
        const char* pcCurrentDir,
        const char* pcSurfaceDir,
        NiTerrainDataLeaf* pkCurrentLeaf,
        NiDOMTool& kDom,
        NiUInt32 uiDetailLevel,
        NiUInt32* puiErrorCode = 0);

    /**
        Create a new mask from the given pixel data. Copies all pixels from the
            given pixel data to the mask
     */
    NiSurfaceMask* CreateMask(NiPixelData* pkPixelData);

    /**
        Create a blank mask, with equal width and height. Initializes all 
            pixels to 0
     */
    NiSurfaceMask* CreateMask(NiUInt32 uiWidth);

    /// Retrieve the surface mask at the given mask index
    NiSurfaceMask* GetMask(NiUInt32 uiIndex);

    /** 
        Initializes the surface mask NiTexture pool. If the stack size param is
            set to zero, the stack is deleted. This function must not be called
            if there are already items on the stack, with the exception of
            increasing the size of the stack.
        
        @param uiStackSize Number of masks that can be stored in the stack
     */
    void InitMaskStack(NiUInt32 uiStackSize);

    /**
        Remove and deallocate all surface masks within this sector, and reset
        the stack position to 0. The stack itself will not be deallocated.

        This function will not remove any surfaces from the quadtree, it should
        be used for memory de-allocation only.
     */
    void RemoveAllMasks();

    /**
        Trigger an update of internal data, if required. All data loading,
        preparation and cache updates are performed within this function.
        If any deformation has occurred on the terrain, DoUpdate MUST be called
        before any attempt to render.

        @param kUpdate The NiUpdateProcess used to update the sector.                 
    */
    void DoUpdate(NiUpdateProcess& kUpdate);

    bool FindCollision(float fDeltaTime, NiAVObject* pkCollider, 
        NiTerrainDataLeaf* pkQuadLeaf) const;
    bool TestCollision(float fDeltaTime, NiAVObject* pkCollider, 
        NiTerrainDataLeaf* pkQuadLeaf) const;
};

#include "NiTerrainSector.inl"
#include "NiTerrainSectorCollision.inl"

#endif // NITERRAINSECTOR_H
