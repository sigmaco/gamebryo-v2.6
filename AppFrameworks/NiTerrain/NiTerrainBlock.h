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

#ifndef NITERRAINBLOCK_H
#define NITERRAINBLOCK_H

#include <NiTArray.h>
#include <NiMesh.h>

#include "NiTerrainLibType.h"
#include "NiTerrainSectorData.h"
#include "NiTerrainDataLeaf.h"
#include "NiTerrainVertex.h"
#include "NiRay.h"
#include "NiUCharPool.h"

class NiTerrainDecal;

/**
    NiTerrainBlock objects represent the renderable geometry of the terrain.
    
    The NiNode class is extended to store an additional list of children, that
    holds ONLY block children and will only ever contain either 0 or 4
    children. The base-class list of children contains not only the block
    children, but also the mesh. This results in either having 1 or 5 children.
    
    The mesh is stored as a child so that NiDynamicEffect objects propagate
    correctly, and the mesh responds correctly to the UpdateEffects function.
    This also allows the client application to gain direct access to the
    meshes, in order to attach NiProperty objects.
 */
class NITERRAIN_ENTRY NiTerrainBlock : public NiNode
{    
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    /// @endcond

public:

    // Streams
    enum STREAMS
    {
        /// Vertex position stream.
        STREAM_POSITION,

        /// Normals and Tangents stream.
        STREAM_NORMALTANGENT,

        /// Indices stream.
        STREAM_INDEX,

        /// Texture coordinate stream.
        STREAM_UV,

        /// Defines the maximum number of stream types.
        STREAM_MAX
    };

    /**
        @name Constructors
    */
    //@{

    /**
        Default Constructor to be used to create the block at the root of the 
        quadtree.

        @param pkSectorData NiSectorData object that is used by the containing
        sector.
    */
    NiTerrainBlock(const NiTerrainSectorData* pkSectorData);
    //@}

    /// Destructor
    virtual ~NiTerrainBlock();

    /**
        @name Data streams
    */
    //@{

    /**
        Finds the region in the static position stream that is associated with
        this block.
        
        This region will exist as long as the static stream is defined, but
        is only valid for the mesh if no dynamic vertex stream is in use.

        @return Region in the static position stream that is associated to 
            this block's data leaf ID.
     */
    inline NiDataStream::Region& GetStaticPositionRegion() const;

    /**
        Return true if this block has been assigned a dynamic vertex
        stream, false otherwise.
    */
    inline bool HasDynamicVertexStreams();

    /**
        Return true if this block has been assigned a dynamic UV stream,
        false otherwise.
    */
    inline bool HasDynamicUVStream();

    /**
        Return true if this block has been assigned a dynamic index stream,
        false otherwise.
    */
    inline bool HasDynamicIndexStream();

    /**
        Return a read lock to the dynamic position stream if the stream
        exists, otherwise NULL.
    */
    inline const NiDataStreamLock* GetDynamicPositionLockRead() const;

    /**
        Return a read lock to the dynamic normal stream if the stream
        exists, otherwise NULL.
    */
    inline const NiDataStreamLock* GetDynamicNormalLockRead() const;

    /**
        Return a read lock to the dynamic tangent stream if the stream
        exists, otherwise NULL.
    */
    inline const NiDataStreamLock* GetDynamicTangentLockRead() const;

    /**
        Return a read lock to the dynamic index stream if the stream
        exists, otherwise NULL.
    */
    inline const NiDataStreamLock* GetDynamicIndexLockRead() const;

    /**
        Return a read lock to the dynamic texture coordinate stream if the
        stream exists, otherwise NULL.
    */
    inline const NiDataStreamLock* GetDynamicUVLockRead() const;

    /**
        Return a read lock to the dynamic position stream if the stream
        exists, otherwise NULL.
    */
    inline NiDataStreamLock* GetDynamicPositionLockRead();

    /**
        Return a read lock to the dynamic normal stream if the stream
        exists, otherwise NULL.
    */
    inline NiDataStreamLock* GetDynamicNormalLockRead();

    /**
        Return a read lock to the dynamic tangent stream if the stream
        exists, otherwise NULL.
    */
    inline NiDataStreamLock* GetDynamicTangentLockRead();

    /**
        Return a read lock to the dynamic index stream if the stream
        exists, otherwise NULL.
    */
    inline NiDataStreamLock* GetDynamicIndexLockRead();

    /**
        Return a read lock to the dynamic texture coordinate stream if the
        stream exists, otherwise NULL.
    */
    inline NiDataStreamLock* GetDynamicUVLockRead();

    /**
        Return the dynamic stream corresponding to the given stream type,
        or NULL if no dynamic stream exists for the given type.
     */
    NiDataStream* GetDynamicStream(STREAMS eStream);

    /**
        Request that the dynamic stream belonging to this block be locked for
        the the given lock type.

        This function will fail if the stream is already locked, or no dynamic
        stream exists for the requested stream index.

        @return Locked stream buffer
     */
    void* LockDynamicStream(STREAMS eStream, NiUInt8 eLockType);

    /**
        Unlock the dynamic stream within this block with given stream type,
        for the given lock type.
    */
    void UnlockDynamicStream(STREAMS eStream, NiUInt8 eLockType);

    /**
        Assign a dynamic vertex stream to this block.
        
        This will take the place of the static stream for this block only.
    */
    bool SetDynamicVertexStreams(NiDataStream* pkPositionStream, 
        NiDataStream* pkNormalTangentStream);

    /**
        Assign a dynamic index stream to this block.
        
        This will take the place of the static stream for this block only.
    */
    bool SetDynamicIndexStream(NiDataStream* pkIndexStream);

    /**
        Assign a dynamic texture coordinate stream to this block.
        
        This will take the place of the static stream for this block only.
    */
    bool SetDynamicUVStream(NiDataStream* pkUVStream);

    /**
        Revert this block to use a static vertex data streams if a
        dynamic streams were previously assigned.
        
        This function reverts the position, normal and tangent streams.
    */
    void RemoveDynamicVertexStreams();

    /**
        Revert this block to use a static index data stream if a
        dynamic stream was previously assigned.
    */
    void RemoveDynamicIndexStream();

    /**
        Revert this block to use a static texture coordinate data stream if a
        dynamic stream was previously assigned.
    */
    void RemoveDynamicUVStream();

    //@}
    
    /** @name Mesh Management */
    //@{
    /**
        Creates the mesh for this block using currently available data streams.

        The mesh will use any custom data streams it finds, but will fall back
        on the static streams found in the sector data.
     */
    void CreateMesh();

    /**
        Destroy the mesh associated with this block, including surface blend
        textures and shader extra data.
        
        This function will not have any effect on associated dynamic streams,
        or delete any decals.
     */
    void DestroyMesh();

    /**
        Return the NiMesh object currently attached to this block.
        
        This function should only be called if a valid mesh exists in this
        block.
     */
    inline NiMesh& GetMesh() const;

    /**
        Creates the hierarchy of blocks, and attaches the associated data from
        the tree.
     */
    void CreateHierarchy(NiTerrainDataLeaf* pkData);

    /**
        Returns true if any of the vertices in this block have changed position
        since the lighting was last built, or if an adjacent block has deformed
        to affect our normals.
     */
    inline bool RequiresLightingRebuild() const;

    /**
        Returns true if any of the vertices in this block have changed position
        since the morphing was last built, or if an adjacent block has deformed
        to affect our data.
     */
    inline bool RequiresMorphRebuild() const;
    //@}
    
    /**
        Mark that this block needs to have its lighting rebuilt during the next
        selective lighting update.
     */
    inline void RequestLightingRebuild();

    /**
        Specify that the lighting has now been built for this block and is up
        to date.
     */
    void MarkLightingBuilt();

    /// @cond EMERGENT_INTERNAL
    /** 
        Has this block calculated the lighting on it's edges yet?

        @return True if 
    */
    bool IsLightingInProgress();

    /** 
        Indicate that this block has calculated the lighting on it's edges
    */
    void MarkLightingInProgress();
    /// @endcond

    /**
        Mark that this block needs to have its morph data rebuilt during the 
        next selective lighting update.
     */
    inline void RequestMorphRebuild();

    /**
        Specify that the morph data has now been built for this block and is up
        to date.
     */
    inline void MarkMorphBuilt();

    /**
        Return a pointer to the data leaf associated with this block
     */
    inline const NiTerrainDataLeaf* GetDataLeaf() const;
        
    /**
        Update a single vertex in this mesh.
        
        @param kLocalIndex Index of the terrain vertex to modify.
        @param fNewHeight New height of the vertex.
        @return True if this block, or any of its children contained the index.
        @note The vertex is assumed to exist in the mesh.
     */
    void MarkVertexChanged(const NiIndex& kLocalIndex, float fNewHeight);

    /**
        Update the surface masks for the terrain.

        @param bCalcUV Indicates whether the UV sets need to be recalculated.
     */
    void MarkSurfaceMasksChanged(bool bCalcUV = false);

    /** @name Updates */
    //@{
    /**
        Return true if RequestUpdate has been called on this block, or any
        of its children, since it was last updated.
     */
    bool RequiresUpdate() const;

    /**
        Request that next time an update is sent to the block tree, this block 
        and its parents up to the root of the tree be updated.
     */
    void RequestUpdate();

    /**
        Performs an update of this node, plus all of its children.
        
        Assumes that an update is required.

        @note To optimize, this function assumes that if any one child 
            exists, all other children will be present.
      
        @param fTime Accumulated app time, in seconds
     */
    virtual void Update(float fTime);
    //@}

    /**
        Set the 'Drawn' flags for all blocks in the quadtree to false.
        
        This function should only be called on the root node of the quadtree,
        as the drawn cache is shared by all blocks in a tree
    */
    void ResetLOD();

    /**
        Recursively build the visible set of blocks, adding it to the visible 
        set stored in the sector data's rendering context.
        
        This function also prepares the 'drawn' member variables to be used
        in ProcessBorders.

        @note To optimize, this function assumes that if any one child 
            exists, all other children will be present.
      
        @return True if this block or any of its children are visible.
    */
    bool ProcessLOD();

    /**
        Get the ID of the region in the index buffer to use for this block. 
        
        Generally universal across standard and custom index buffers.
    */
    NiUInt8 GetStitchingIndex() const;

    /**
        This override of the base class function SetWorldScale is used to
        update the morphing data when scaling is done on the terrain
    */
    void SetWorldScale(float fScale);

    /**
        This function updates the shader constant data for morphing. 
        This function is called whenever the scaling of the terrain is
        changed, and when the LOD scale is changed.
    */
    void UpdateMorphConstants();

    /**
        Update the index buffer region used by this block according to which
        borders need to be stitched.
        
        This is a recursive function, that will also process the borders of
        all block children.
    */
    void ProcessBorders();

    /// @cond EMERGENT_INTERNAL
    /**
        Manage the child block array, which only keeps a track of quadtree 
        related children.
    */
    //@{
    NiTerrainBlock* GetBlockChildAt(NiUInt32 uiPosition);
    const NiTerrainBlock* GetBlockChildAt(NiUInt32 uiPosition) const;
    void SetBlockChildAt(NiUInt32 uiPosition, NiTerrainBlock* pkBlock);
    //@}
    /// @endcond

    /**
        Return a pointer to the immediate parent of this block.
    */
    NiTerrainBlock* GetBlockParent();

    /**
        Return a pointer to the immediate parent of this block (const version).
    */
    const NiTerrainBlock* GetBlockParent() const;

    /** @name Decals */
    //@{
    /**
        Add the specified decal to this block if it hasn't already been added.

        @param pkDecal Decal to add to this block
    */
    void AddDecal(NiTerrainDecal* pkDecal);
    //@}
    

    /// @cond EMERGENT_INTERNAL
    static void _SDMInit();
    static void _SDMShutdown();
    /// @endcond

protected:

    // LOD
    enum DRAWN 
    {
        SELF = 1,
        CHILDREN = 2
    };

    // Helper enumeration.
    enum
    {
        NUM_CHILDREN = 4,
    };

    // Lighting Status enumerations.
    enum
    {
        LIGHTING_BUILT = 0,
        LIGHTING_OUTDATED = 1,
        LIGHTING_IN_PROGRESS = 2
    };
    
    /**
        Constructor to be used to create the any child node within the 
        quadtree.

        @param pkParent Parent block in the quadtree
        @param pkSectorData NiSectorData object that is used by the 
        containing sector.
    */
    NiTerrainBlock(NiTerrainBlock* pkParent, 
        const NiTerrainSectorData* pkSectorData);

    /**
    @name Surfaces
    */
    //@{

    NiTexture* GetSurfaceBlendMask(NiUInt32 uiIndex) const;
    inline NiSourceTexture* GetCombinedDistributionMask();
    inline void SetCombinedDistributionMask(NiSourceTexture* pkTexture);
    NiDynamicTexture* GetBlendTexture(NiUInt32 uiIndex);
    NiDynamicTexture* CreateBlendTexture(NiUInt32 uiIndex);
    NiDynamicTexture* ResizeBlendTexture(NiUInt32 uiIndex);
    inline bool CreateShaderData(bool bInheritingMask);
    void UpdateMasks();

    NiTexturingProperty* GetTexturingProperty();

    //@}

    /** @name Level Of Detail */
    void UpdatePositionMorphData();
    //@{

    /**
        Calculate whether this block should be drawn, according to the 
        camera position in the current rendering context. This function 
        takes both raw 3D distance from camera and morph radius weightings 
        into account. This function does not perform frustum culling.

        @return true if this block is a candidate for rendering
    */
    bool IsInRange();        

    void SetStitchingIndex(NiUInt8 ucStitchingIndex);

    /**
        Add this block to the visible set contained in the sector data's
        rendering context
    */
    inline void AddToVisible();

    /**
        An efficient method to detect whether or not this block was culled by
        the most recent call to ProcessLOD

        @return true if this block was not added to the most recently 
        generated visible set.
    */
    bool CulledByLOD() const;

    /// Get the active configuration in use by the terrain containing this 
    /// block
    const NiTerrainConfiguration& GetConfiguration() const;

    /**
        Request that all decals attached to this block be updated during the 
        next update cycle
    */

    /**
        Request that all decals attached to this block be updated during the 
        next update cycle
    */
    void RequestDecalsUpdate();
    //@}

    float m_fMaxDistanceSqr;
    NiUInt32 m_uiDrawnPoolIndex;
    NiUCharPool* m_pkDrawnPool;

    bool m_bMaskChanged;        // Has the contents of a mask changed?
    bool m_bMaskLayersChanged;  // Has a mask been added or removed?

    // Mesh
    bool m_bRequiresUpdate;
    NiUInt8 m_ucLightingStatus;
    bool m_bMorphOutdated;

    // Is this block locked for write on at least one stream?
    NiUInt8 m_ucStreamsLockedForWrite;
    NiUInt8 m_ucStitchingIndex;

    //@{
    /**
        Not-null if we are using a custom dynamic stream.

        Stream-ref positions:
        0: Position
        1: Normal/Tangent
        2: Index
        3: UV
    */
    NiDataStream* m_pkCustomPositionStream;    
    NiDataStream* m_pkCustomNormalTangentStream;
    NiDataStream* m_pkCustomIndexStream;
    NiDataStream* m_pkCustomUVStream;
    //@}

    // Stream locks - Read or write depending on block state
    NiDataStreamLock* m_pkCustomPositionLock;
    NiDataStreamLock* m_pkCustomNormalLock;
    NiDataStreamLock* m_pkCustomTangentLock;
    NiDataStreamLock* m_pkCustomIndexLock;
    NiDataStreamLock* m_pkCustomUVLock;

    /// Keeps track of the lock status of all streams
    NiUInt8 m_aucLockStatus[STREAM_MAX];

    // Quadtree
    NiTerrainBlock* m_pkParent;
    NiMeshPtr m_spMesh;

    // A pointer to current sector settings
    const NiTerrainSectorData* m_pkSectorData;

    /// Associated node in the parallel data quadtree
    const NiTerrainDataLeaf* m_pkDataLeaf;

    /// Combined surface distribution mask.
    NiSourceTexture* m_pkDistributionMask;

    NiTerrainBlock* m_apkChildren[NUM_CHILDREN];
    NiTQuickReleasePrimitivePool<NiUInt32> m_kRecentlyModifiedVerts;

    NiTObjectArray<NiDynamicTexturePtr> m_kSurfaceBlendMasks;

    // Decals on this block
    NiTPointerList<NiTerrainDecal*> m_kDecals;

};

#include "NiTerrainBlock.inl"

#endif // NITERRAINBLOCK_H
