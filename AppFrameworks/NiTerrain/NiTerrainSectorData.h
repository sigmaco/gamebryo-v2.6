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

#ifndef NITERRAINSECTORDATA_H
#define NITERRAINSECTORDATA_H

#include <NiEntityRenderingContext.h>
#include <NiMemoryDefines.h>
#include <NiMeshLib.h>

#include "NiTerrainLibType.h"
#include "NiTQuickReleasePool.h"
#include "NiDynamicStreamCache.h"
#include "NiTerrainMaterial.h"

#include "NiDynamicStreamCache.h"
#include "NiTerrainConfiguration.h"

class NiTerrainBlock;

/**
    This class is responsible for keeping a record of data and variables that 
    are consistent across all leaves in the quad hierarchy; such as block size,
    renderer data, shared data streams etc.

    It should be regarded as internal only and not be used by an 
    application directly.
*/

/// @cond EMERGENT_INTERNAL
class NITERRAIN_ENTRY NiTerrainSectorData : public NiMemObject
{
public:

    /// The number of regions that exist in the index streams, one region per
    /// stitching state
    enum
    {
        /// Specifies the number of index regions in the index stream to 9.
        NUM_INDEX_REGIONS = 9,
    };


    /// The types of LOD that the terrain system can provide. As described in
    /// the function "SetLODMode"
    enum
    {
        LOD_MODE_2D = 0,
        LOD_MODE_25D,
        LOD_MODE_3D,
        NUM_LOD_MODES,

        LOD_MORPH_ENABLE = 0x08
    };

public:

    /// Default constructor.
    NiTerrainSectorData();

    /// Destructor
    ~NiTerrainSectorData();

    /// Block size in both theoretical and vertex space
    /// Side length of a block in this sector, in voxels
    NiUInt32 GetBlockSize() const;

    /// Side length of a block in this sector, in vertices
    NiUInt32 GetBlockWidthInVerts() const;

    /// Side length of this sector, in voxels (at maximum detail - level 0)
    NiUInt32 GetSectorSize() const;

    /**
        Set the side length of each block in this sector, in voxels. Calling
        this function will also implicitly update the side length in vertices,
        and the sector side length members.
    */
    void SetBlockSize(NiUInt32 uiSize);

    /// Side length of this sector, in vertices (at maximum detail - level 0)
    NiUInt32 GetSectorWidthInVerts() const;

    /// Number of lower levels of detail, below the maximum (level 0)
    NiUInt32 GetNumLOD() const;

    /**
        Number of lower levels of detail, below the maximum (level 0). Calling
        this function will also update the side length of the sector and the
        total block count.
    */
    void SetNumLOD(NiUInt32 uisNumLOD);

    /// The number of blocks that exist in total, throughout the entire 
    /// quadtree in this sector.
    NiUInt32 GetNumBlocks() const;

    /// The level of the highest LOD level that has been completely loaded
    //@{
    NiInt32 GetHighestLoadedLOD() const;
    void SetHighestLoadedLOD(NiInt32 iHighestLoadedLOD);
    //@}

    /**
        The level of detail requested to be loaded. Usually one above the 
        LOD required to draw to the screen. MAX if terrain is marked as 
        deformable. If this is less than HighestLODLoaded then levels may
        be unloaded.
     */
    //@{
    NiInt32 GetTargetLoadedLOD() const;
    void SetTargetLoadedLOD(NiInt32 iTargetLoadedLOD, bool bAllowUnloading);
    //@}

    /**
        The LOD Scale is different to the camera LOD, in that camera LOD is a
        general detail level that is applied to the whole world (generally 
        in a games settings), but the LOD Scale is a separate adjustment 
        specific to a terrain. Both the camera LOD and the LOD scale are 
        taken into account.

        @param fScale The scale factor for the LOD. Higher is better detail. 
        Must be at least 2.0f * NiSqrt(2.0f)
    */
    //@{
    float GetLODScale() const;
    float GetLODShift() const;
    void SetLODScale(float fScale, float fShift);
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

    /// World scale of the sector
    //@{
    float GetWorldScale() const;
    void SetWorldScale(float fScale);
    //@}

    // World translation of the sector
    //@{
    const NiPoint3& GetWorldTranslate() const;
    void SetWorldTranslate(const NiPoint3& kWorldTranslate);
    //@}

    // World transformation of the sector
    //@{
    const NiTransform& GetWorldTransform() const;
    void SetWorldTransform(const NiTransform& kWorldTransform);
    //@}

    /** 
        @name Static Data Streams
        Static data streams used by blocks in an unmodified state.
    */
    //@{
    NiDataStream* GetStaticIndexStream() const;
    void SetStaticIndexStream(NiDataStream* pkStream);
    NiDataStream* GetStaticPositionStream(NiUInt32 uiDetailLevel) const;
    void SetStaticVertexStream(NiUInt32 uiDetailLevel, NiDataStream* pkStream);
    NiDataStream* GetStaticNormalTangentStream(NiUInt32 uiDetailLevel) const;
    void SetStaticNormalTangentStream(NiUInt32 uiDetailLevel, NiDataStream* pkStream);
    NiDataStream* GetStaticUVStream() const;
    void SetStaticUVStream(NiDataStream* pkStream);
    //@}

    /// Dynamic stream cache that is used to allocate dynamic streams to any
    /// deformed blocks. This will only exist if the sector is deformable, or
    /// in tool mode.
    //@{
    NiDynamicStreamCache* GetDynamicStreamCache() const;
    void SetDynamicStreamCache(NiDynamicStreamCache* pkCache);
    //@}

    /**
        Find the region in the index stream that corresponds to the given
        4-bit block stitching mask. 

        @note Index stream regions are shared between both static and dynamic
        index streams.

        @param ucStitchingIndex Stitching index of the vertex index stream 
        region to retrieve. Value must be less than or equal to 0xF.

        @return Index stream region corresponding to the given stitching index,
        or NULL if region not found or ucStitchingIndex is out of bounds.
    */
    NiDataStream::Region* GetIndexRegionByStitchIndex(
        NiUInt8 ucStitchingIndex) const;

    /**
        Retrieve the index stream region at the given position.

        @note Index stream regions are shared between both static and dynamic
        index streams.

        @return Region in static index stream at given position, or NULL if
        no region exists at that position.
    */
    NiDataStream::Region* GetIndexRegion(NiUInt8 ucPos)
        const;

    /**
        Set the index stream region for the given index. Ownership is not taken
        of the region, it must be deleted by the sector during shutdown.

        @param ucPos Region index to set. Valid values must be 
            < ms_ucNumIndexRegions.
        @param pkIndexRegion Region with which to set at the specified
            position.
        @note Index stream regions are shared between both static and dynamic
            index streams.
    */
    void SetIndexRegion(NiUInt8 ucPos, NiDataStream::Region* pkIndexRegion);

    /**
    @name Rendering
    */
    //@{
    /// Culling Process to be used by this sector
    NiCullingProcess* GetCullingProcess() const;

    /// Camera to be used during LOD and rendering calculations:
    NiCamera* GetLODCamera() const;

    /// The LOD factor, squared, taken from the NiCamera contained in the last
    /// rendering context to be assigned to this sector
    float GetCameraLODSqr() const;

    /// The NiCamera frustum planes cached from the NiCamera contained in the
    /// last rendering context to be assigned to this sector
    const NiFrustumPlanes& GetFrustumPlanes() const;

    /// Set the culling process to be used by the blocks in this sector 
    /// during rendering.
    void SetCullingProcess(NiCullingProcess* pkCuller);

    /// Set the Camera to use during LOD calculations:
    void SetLODCamera(NiCamera *pkCamera);

    /// Add the given block to the visible set
    void AddToVisible(const NiTerrainBlock* pkBlock) const;

    /// Retrieve a pointer to the current visible set
    void GetVisibleBlocks(
        const NiTQuickReleasePrimitivePool<const NiTerrainBlock*>*& 
        pkVisibleBlocks) const;

    /**
        Empty the visible set of blocks.
        @return the number of blocks that were visible in total, before the 
        reset
    */
    NiUInt32 ResetVisible();

    /**
        Returns the number of blocks that are currently visible. Only valid
        after BuildVisibleSet has been called at least once on a sector.
    */
    NiUInt32 GetNumVisible();
    //@}

    /// Defines whether or not this sector is deformable
    //@{
    bool GetDeformable() const;
    void SetDeformable(bool bDeformable);
    //@}

    /**
        Define whether or not this sector is running in a tool, such as the 
        scene designer
    */
    static bool InToolMode();


    const NiUInt16* GetHeightMap() const;
    NiUInt16* GetHeightMap();
    int GetMinHeight() const;
    void SetMinHeight (int minHeight);
    int GetMaxHeight() const;
    void SetMaxHeight (int maxHeight);
    float GetHeightScale() const;
    void SetHeightScale (float heightScale);
    float GetHeightShift() const;
    void SetHeightShift (float heightShift);
    NiInt16 GetSectorIndexX();
    void SetSectorIndexX(NiInt16 sXIndex);
    NiInt16 GetSectorIndexY();
    void SetSectorIndexY(NiInt16 sYIndex);

    /// @cond EMERGENT_INTERNAL
    void AllocateHeightMap();
    /// @endcond

    inline const NiTerrainConfiguration& GetConfiguration() const;
    inline void SetConfiguration(NiTerrainConfiguration kConfiguration);


private:

    /// Always a power of 2 - length of a block in voxels
    NiUInt32 m_uiBlockSize;

    /// Always equal to blockSize + 1. Length of a block in vertices
    NiUInt32 m_uiBlockWidthInVerts;

    /// Always a power of 2 - size of the sector side, in voxels
    NiUInt32 m_uiSectorSize;

    /// Always equal to m_uiSectorSize + 1. Length of the sector in vertices
    NiUInt32 m_uiSectorWidthInVerts;

    /// Deformation
    bool m_bIsDeformable;

    /// Number of lower levels of detail that exist, in addition to the maximum
    NiUInt32 m_uiNumLOD;

    /// Sector Index
    //{@
    NiInt16 m_sSectorIndexX;
    NiInt16 m_sSectorIndexY;
    //@}

    /// The level of the highest LOD level that has been completely loaded
    NiInt32 m_iHighestLoadedLOD;

    /// The level of detail requested to be loaded:
    NiInt32 m_iTargetLoadedLOD;

    /// Static data streams that are used by non deformed blocks
    NiDataStreamPtr m_spStaticIndexStream;
    NiDataStreamPtr m_spStaticUVStream;
    NiTObjectArray<NiDataStreamPtr> m_kStaticVertexStreams;
    NiTObjectArray<NiDataStreamPtr> m_kStaticNormalTangentStreams;

    // Responsible for managing dynamic block data
    NiDynamicStreamCachePtr m_spDynamicStreamCache;

    /// Rendering info
    NiCullingProcess* m_pkCullingProcess;
    NiCamera* m_pkLODCamera;
    float m_fCameraLODSqr;
    float m_fTerrainLODscale;
    float m_fTerrainLODshift;
    NiUInt32 m_uiTerrainLODmode;
    NiTexturePtr m_spEnvMap;
    NiTexturePtr m_spDefaultEnvMap;

    /// The default material that all blocks in this sector will use
    NiMaterialPtr m_spTerrainMaterial;

    /// Visible Set
    mutable NiTQuickReleasePrimitivePool<const NiTerrainBlock*> 
        m_kVisibleBlocks;

    // Index regions
    NiUInt8 m_akIndexRegionsMap[16];
    NiTPrimitiveArray<NiDataStream::Region*> m_kIndexRegions;

    /// The transformation of the terrain in the world. A scale of 1.0 will 
    /// lead to a vertex spacing of 100.0 units at max detail level
    NiTransform m_kWorldTransform;

    /// Frustum of the last camera to be set via SetRenderingContext()
    NiFrustumPlanes m_kFrustumPlanes;

    /// Array of heights read from RAW file
    NiUInt16* m_pusHeightMap;

    int m_iMinHeight;
    int m_iMaxHeight;
    float m_fHeightScaleFactor;
    float m_fHeightShift;

    NiTerrainConfiguration m_kTerrainConfiguration;

};
/// @endcond

#include "NiTerrainSectorData.inl"

#endif // NITERRAINSECTORDATA_H
