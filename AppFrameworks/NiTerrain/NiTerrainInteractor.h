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

#ifndef NITERRAININTERACTOR_H
#define NITERRAININTERACTOR_H

#include <NiMemoryDefines.h>

#include "NiTerrainLibType.h"
#include "NiDeformablePointSet.h"
#include "NiTerrainDecal.h"

#include "NiTQuickReleasePool.h"

/**
    The terrain interactor is acts as an interface for an end user to interact 
    with a terrain. It can be used to perform physics queries, load and unload 
    data, place decals and perform real time deformation.
 */
class NITERRAIN_ENTRY NiTerrainInteractor : public NiMemObject
{
public:

    /** 
        Default parameterized constructor.

        @param uiPoolStartSize How many elements will created within a  
            deformable point pool at first creation.
        @param uiNumCaches Initial number of caches to allocate in the free 
            list of point pools. The point pools are used when deforming the 
            terrain.
        @param uiMaxCacheEntries Maximum number of cache entries or free list
            entries stored in the interactor. Higher max numbers reduce 
            memory allocations, lower numbers use lower memory.
     */
    NiTerrainInteractor(NiUInt32 uiPoolStartSize = 1000, 
        NiUInt32 uiNumCaches = 0, NiUInt32 uiMaxCacheEntries = 10);
    
    /// Destructor
    ~NiTerrainInteractor();

    /** @name Collision */
    //@{

    /** 
        Returns the terrain's NiBound.
        
        @param[out] kBound NiBound that contains the entire loaded terrain
     */
    void GetBound(NiBound& kBound) const;

    /** 
        Returns the terrain's NiBoundingVolume
     */
    NiNode* GetRootNode() const;

    /** 
        Returns a sector's NiNode object for attaching effects
        
        @param x x id of the sector
        @param y y id of the sector
     */
    NiNode* GetSectorNode(NiUInt32 x, NiUInt32 y) const;

    /** 
        Find the closest point of collision on the terrain with the given ray.
        The collision point is placed within the ray itself
        
        @param kRay Ray with which to find the collision point. This is as well
            a return parameter and information about the collision point will 
            be added in the ray. The ray should hold information about its 
            origin and its direction.
        @param uiDetailLOD Maximum detail level in which to search for an 
            intersection.
        @return true if collided false otherwise
     */
    NiBool Collide(NiRay& kRay, 
        NiUInt32 uiDetailLOD = NiTerrainUtils::ms_uiMAX_LOD) const;
    
    /** 
        Find the closest point of collision on an individual terrain sector 
        with the given ray. The collision point is placed within the ray 
        itself.

        @param kRay Ray with which to find the collision point. This is as well
            a return parameter and information about the collision point will 
            be added in the ray. The ray should hold information about its 
            origin and its direction.
        @param pkSectorOut The sector in which a collision was detected.
            This parameter is output only and may be set to NULL when calling.
        @param uiDetailLOD Maximum detail level in which to search for an 
            intersection.
        @return true if collided false otherwise
    */
	NiBool Collide(NiRay& kRay, NiTerrainSector*& pkSectorOut, 
         NiUInt32 uiDetailLOD = NiTerrainUtils::ms_uiMAX_LOD) const;

    /** 
        Test for a collision. No actual collision points are found as this 
        function only tests to see if a collision exists.
        
        @param kRay Ray with which to find the collision point. The ray 
            should hold information about its origin and its direction.
        @param uiDetailLOD Maximum detail level in which to search for an 
            intersection.
        @return true if collision was detected
     */
    NiBool Test(NiRay& kRay, NiUInt32 uiDetailLOD) const;

    //@}
    
    /** @name Deformation */
    //@{

    /**
        Gather a set of deformable points that contain vertices within the
        specified world space bound
        
        Optionally gather deformable points from a specific detail level and 
        lower (i.e., a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
            vertices
        @param pkPointSet Deformable point set to add found vertices too
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeParentLOD Optionally toggle adding points from detail
            levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetVerticesInBound(const NiBound& kBound, 
        NiDeformablePointSet* pkPointSet,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bIncludeParentLOD = false);
    /**
        Gather a set of deformable points that contain vertices within the
        specified world space bound
        
        Optionally gather deformable points from a specific detail level and 
        lower (i.e., a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
            vertices
        @param pkPointSet Deformable point set to add found vertices too
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeParentLOD Optionally toggle adding points from detail
            levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetVerticesInBound(const NiBoundingVolume& kBound, 
        NiDeformablePointSet* pkPointSet,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bIncludeParentLOD = false);
        
    /**
        Gather a set of deformable points that contain vertices within the
        specified world space bound
        
        Optionally gather deformable points from a specific detail level and 
        lower (i.e., a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
            vertices
        @param pkPointSet Deformable point set to add found vertices too
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeParentLOD Optionally toggle adding points from detail
            levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetVerticesInBound2D(const NiBound& kBound,
        NiDeformablePointSet* pkPointSet,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bIncludeParentLOD = false);

    /**
        Gather a set of deformable points that contain pixels within the
        specified world space bound. 
        
        Optionally gather deformable points from a specific detail level and 
        lower (i.e., a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
            vertices
        @param pkPointSet Deformable point set to add found pixels too
        @param pkSurface Gather pixels from the surface mask that represents
            this surface
        @param bCreateMasksIfNotExist If no surface mask exists for the given
            surface on any block within the given radius, create a mask for
            those blocks
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeLowerDetail Optionally toggle adding points from detail
            levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetPixelsInBound(const NiBound& kBound, 
        NiDeformablePointSet* pkPointSet, const NiSurface* pkSurface,
        bool bCreateMasksIfNotExist,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true);
    /**
        Gather a set of deformable points that contain pixels within the
        specified world space bound. 
        
        Optionally gather deformable points from a specific detail level and 
        lower (i.e., a given detail level and all 'more coarse' detail levels
        above it)

        Optionally specify that points should only be gathered from the 
        specified detail level and none from corresponding 'lower' detail
        levels with the radius.

        @param kBound World space bound defining space from which to gather
            vertices
        @param pkPointSet Deformable point set to add found pixels too
        @param pkSurface Gather pixels from the surface mask that represents
            this surface
        @param bCreateMasksIfNotExist If no surface mask exists for the given
            surface on any block within the given radius, create a mask for
            those blocks
        @param uiDetailLevel Optional detail level from which to gather points
        @param bIncludeLowerDetail Optionally toggle adding points from detail
            levels lower than the specified uiDetailLevel
    */
    NiUInt32 GetPixelsInBound2D(const NiBound& kBound, 
        NiDeformablePointSet* pkPointSet, const NiSurface* pkSurface,
        bool bCreateMasksIfNotExist,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true);
    
    /** 
        Update the terrain to reflect any changes that were made to the 
        given deformable points. Optionally, any changes made can be propagated
        up the quadtree through the lower levels of detail. This option should
        not be enabled if the set contains points from multiple detail levels.
        
        @param pkPointSet Deformable point-set to apply
        @param bModifyParentLOD Specify that changes to high detail points 
            should be propagated up the tree to the lower levels.
        @return true if changes were made successfully, false otherwise
     */
    NiBool CommitChanges(NiDeformablePointSet* pkPointSet,
        bool bModifyParentLOD = true);

    /**
        Update all currently loaded sectors that are linked to this terrain
     */
    void UpdateSectors(float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);

    //@}
    
    /** @name Lighting */
    //@{

    /**
        Rebuild the terrain's lighting in all loaded sectors. If the parameter
        is true then all leaves will have their lighting updated. This can be
        an expensive operation. The data rebuilt includes the following; 
        normals, tangents and associated geo-morphing data.
     */
    void RebuildLighting(bool bAllLighting = false);

    /// Do a complete rebuild of lighting data for all loaded sectors in this 
    /// terrain - normals, tangents and associated geo-morphing data.
    void RebuildAllLighting();

    /**
        Rebuild the lighting only on blocks that have outdated lighting (i.e.,
        they have been deformed since the last lighting build)
     */
    void RebuildChangedLighting();
    //@}
    
    /** @name Surfaces */
    //@{

    /**
        This will cause all blocks using the given surface to update any shader
        maps and constants derived from the surface.
     */
    void MarkSurfaceChanged(const NiSurface* pkSurface);

    /**
        Remove the given surface from this terrain, also removing associated
        masks. All surfaces below the given surface will be moved up to fill
        the gap.
     */
    inline void RemoveSurface(const NiSurface* pkSurface);
    //@}
    
    /** @name Decals */
    //@{

    /** 
        Creates a decal on the terrain according to the information given.
        Note that if only a texture and a position are given, the decal
        will be created with default values.

        @param pDecalTexture NiTexture to apply on the decal
        @param kPosition Position in world coordinates around which to center
            the decal. The decal will be aligned and projected to the surface
            of the terrain.
        @param uiSize Side length of the decal. The decal will always be square
            so this represents both the X and Y components
        @param uiRatio The scale of the decal texture itself within the decal.
            A larger ratio value will result in a smaller texture within the
            decal.
        @param fTimeOfDeath accum application time at which the decal will be 
            destroyed
        @param fDecayLength Time, in seconds, before fTimeOfDeath that the 
            decal will start to become transparent
        @param fDepthBiasOffset Small offset to apply to the decal such that
            it does not cause depth buffer fighting artifacts.
        @return A pointer to the created decal object. Care should be taken,
            as this pointer will become invalid once the decal has expired.
    */
    NiTerrainDecal* CreateDecal(NiTexture* pDecalTexture, 
        NiPoint3 kPosition, NiUInt32 uiSize = 2,
        NiUInt32 uiRatio = 1, float fTimeOfDeath = 0.0f, 
        float fDecayLength = -1.0f, float fDepthBiasOffset = 0.005f);

    //@}
    
    /** @name General query functions */
    //@{

    /**
        Finds the average of this deformable point, as well as all adjacent 
        points.
     */
    NiBool GetSmoothedValue(
        NiDeformablePointSet* pkPointSet,const NiDeformablePoint*& pkPoint, 
        float& fValue, const NiSurface* pkSurface = 0) const;

    /** 
        Collide the given ray with the terrain, at the given detail level
        to find the alpha of the given surface at that point. Collision 
        world location is placed in the ray
        
        @param kRay ray to collide with the terrain
        @param pkSurface Surface we want the alpha value of
        @param ucOpacity Discovered alpha value.
        @param uiDetailLevel Detail level 
        @param bIncludeLowerDetail Checks lower detail levels for mask values
            as well.
        @return returns true if successful false otherwise
     */
    NiBool GetSurfaceOpacity(NiRay& kRay, const NiSurface* pkSurface, 
        NiUInt8& ucOpacity, 
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true) const;

    /**
        Gets all the meta-data from the terrain where the point at which the 
        meta-data is retrieved is computed by intersecting with the terrain 
        along the specified ray. Note, the meta-data associated with materials 
        that are not impacting the terrain at the computed intersection (the 
        layer mask has a zero weight) are not returned.
        @param kRay Ray with which to intersect the terrain.
        @param kMetaData Map of terrain materials to the meta-data associated 
            with that material.
        @return True if successful, false otherwise.
    */
    NiBool GetMetaData(NiRay& kRay, NiTFixedStringMap<NiMetaData>& kMetaData) 
        const;

    /**
        Gets all the meta-data from the terrain at the specified intersection
        point. Note, the meta-data associated with materials that are not 
        impacting the terrain at the computed intersection (the layer mask has 
        a zero weight) are not returned.
        @param kIntersectionPt Intersection point with which to retrieve 
            meta data.
        @param pkLeaf Terrain leaf at which the intersection occurred. This is 
            usually retrieved from the ray after a collision has occurred.
        @param kMetaData Map of terrain materials to the meta-data associated 
            with that material.
        @return True if successful, false otherwise.
    */
    void GetMetaData(const NiPoint3& kIntersectionPt, 
        const NiTerrainDataLeaf *pkLeaf, 
        NiTFixedStringMap<NiMetaData>& kMetaData) const;

    /**
        Gets all the meta-data from the terrain at the intersection point
        between the given ray and the terrain. Also returns a blended meta-data
        for all the meta data values at this point.
        @param kRay Ray with which to intersect the terrain.
        @param kMetaData Map of terrain materials to the meta-data associated 
            with that material.
        @param kBlendedMetaData the resulting blended meta data values
        @return True if successful, false otherwise.
    */
    NiBool GetBlendedMetaData(NiRay& kRay, 
        NiTFixedStringMap<NiMetaData>& kMetaData, 
        NiMetaData& kBlendedMetaData) const;
    
    /**
        Gets all the meta-data from the terrain at the given point.
        Also returns a blended meta-data for all the meta data values at 
        this point.
        @param kIntersectionPt Intersection point with which to retrieve 
            meta data.
        @param pkLeaf Terrain leaf at which the intersection occurred. This is 
            usually retrieved from the ray after a collision has occurred.
        @param kMetaData Map of terrain materials to the meta-data associated 
            with that material.
        @param kBlendedMetaData the resulting blended meta data values
        @return True if successful, false otherwise.
    */
    void GetBlendedMetaData(const NiPoint3& kIntersectionPt, 
        const NiTerrainDataLeaf *pkLeaf, 
        NiTFixedStringMap<NiMetaData>& kMetaData,
        NiMetaData& kBlendedMetaData) const; 

    /// @cond EMERGENT_INTERNAL

    /**
        This function gets meta data information quickly but is only used
        when most calculations have already been done by the calling method.
        It is mainly used by the NiTerrainDecorationFunctor. Note that it 
        returns a float that cointains the sum of weigths for the given meta 
        data at the given index and the one on top of it.
        
        @note This is inlined for more efficiency
    */
    inline void GetMetaDataFast(float fX, 
        float fMinY, float fMaxY,
        const NiTPrimitiveArray<const NiSurfaceMask*>& kMasks,
        const NiTPrimitiveArray<float>& kValues,
        float& fProbability);
    /// @endcond

    /**
        Spacing, in world coordinates, between two points of the given type
        at the specified detail level. If multiple spacings exist (as is 
        possible in the case of mask pixels), the smallest is used.
     */ 
    bool GetPointSpacing(NiDeformablePoint::POINT_TYPE eType, 
        NiUInt32 uiDetailLevel, float& fSpacing) const;

    /**
        Populate the given data array with terrain scope height values from the
        maximum detail level. The top left of the terrain (ie, [-x, +y]) is 
        placed at the start of the array, proceeding row by row with decreasing
        y.

        @param pfMap User allocated memory to populate with terrain height 
            values.
        @param stBufferSize Size of the user allocated buffer, in bytes
        @param[out] uiNumVal the number of values that were added to the buffer
        @param[out] fMinVal The minimum of the height values added to the 
            buffer
        @param[out] fMaxVal The maximum of the height values added to the 
            buffer
        @return true if the buffer was successfully populated, false if the
            buffer was the wrong size or the terrain was not loaded.
     */
    bool GetHeightMapValues(float*& pfMap, size_t stBufferSize, 
        NiUInt32& uiNumVal, float& fMinVal, float& fMaxVal);

    /**
        Set the heights of the terrain to those contained within the given
        buffer. The top left of the terrain (ie, [-x, +y]) is read from the 
        start of the buffer, proceeding row by row with decreasing y.

        The number of entries in the buffer must exactly match the number of
        vertices found at the maximum detail of a sector, otherwise the import
        will fail.

        @param pfMap Buffer from which to read terrain data.
        @param stBufferSize size in bytes of the buffer
        @param uiNumVal number of values contained within the buffer
        @return true if the buffer was the correct size and the data was
            successfully imported
     */
    bool SetHeightFromMap(float* pfMap, size_t stBufferSize, 
        NiUInt32 uiNumVal);

    /// Number of triangles drawn during the last render step
    int GetNumTriangles() const;

    //@}

    /// Gets the terrain object we will be interacting with
    NiTerrain* GetTerrain() const;
    /// Sets the terrain object we will be interacting with
    void SetTerrain(NiTerrain* pkTerrain);

    /// Returns whether or not this terrain is in tool mode
    static bool InToolMode();
    /// Setes whether or not this terrain is in tool mode
    static void SetInToolMode(bool bInToolMode);
    //@}

    /** @name Paging Management */
    //{@
    /// Set the target LOD to be loaded for a particular sector:
	void SetTargetLoadedLOD(NiInt16 iSectorX, NiInt16 iSectorY, 
		NiInt32 usTargetLOD = NiTerrainUtils::ms_uiMAX_LOD);
    //@}

    /** @name Memory management */
    //@{

    /** 
        Initializes the given point set, which retrieves a deformable point
        allocator of the correct type from the cache.
    */
    void InitPointPool(NiDeformablePointSet* pkPointSet);

    /** 
        Invalidates the given point set, which returns its deformable point
        allocator to the cache for re-use.
    */
    void InvalidatePointSet(NiDeformablePointSet* pkPointSet);

    //@}

private:

    /// Keeps track of whether or not tool mode is active
    static bool ms_bInTool;

    /// The initial size of any deformable point pools that are created
    NiUInt32 m_uiPoolStartSize;

    /// Point pools are cached in a free list in order to minimize the amount
    /// of memory allocations used when painting the terrain. This size caps
    /// the number of nodes available in the free list.
    NiUInt32 m_uiPoolFreeListSize;

    /// Terrain component that we interact with
    NiTerrain* m_pkTerrain;

    /// Contains a cache for minimizing memory allocation needed when modifying
    /// the terrain height-field or materials.
    NiTPointerList<NiMemObject*> m_kOriginalPointPools[
        NiDeformablePoint::MAX_TYPE];
};

#include "NiTerrainInteractor.inl"

#endif // NITERRAININTERACTOR_H
