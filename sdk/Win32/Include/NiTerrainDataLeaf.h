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

#ifndef NITERRAINDATALEAF_H
#define NITERRAINDATALEAF_H

#include <NiMain.h>
#include <NiBoxBV.h>

#include "NiTerrainLibType.h"
#include "NiTerrainVertex.h"
#include "NiTerrainSectorData.h"
#include "NiMetaDataStore.h"
#include "NiTTerrainRandomAccessIterator.h"

#include "NiIndex.h"
#include "NiPoint4.h"

class NiMetaDataStore;
class NiTerrainSector;
class NiTerrainBlock;
class NiSurface;
class NiSurfaceMask;

/**
    This class acts as a data repository for the terrain. It holds references 
    to vertex data, texture alpha layers, block level meta data. It is 
    essentially a data index, which merely references the authoritative data 
    contained in the parent sector
 */
class NITERRAIN_ENTRY NiTerrainDataLeaf : public NiMemObject 
{
    // The terrain block requires access to the stream iterators
    friend class NiTerrainBlock;

public:

    /// Used to distinguish the different borders of this data leaf.
    enum BORDERS 
    {
        /// Unspecified border.
        BORDER_NONE = 0,

        /// Borders the bottom.
        BORDER_BOTTOM = 1,

        /// Borders the right.
        BORDER_RIGHT = 2,

        /// Borders the top.
        BORDER_TOP = 4,

        /// Borders the left.
        BORDER_LEFT = 8
    };

    /// Different types of extended data required.
    enum EXTENDED_DATA {
        /// No extended data required by the leaf.
        EXTENDED_DATA_NONE = 0,

        /// Indicates corner data is required by the leaf.
        EXTENDED_DATA_CORNER = 2,

        /// Indicates right data is required by the leaf.
        EXTENDED_DATA_RIGHT = 4,

        /// Indicates top data is required by the leaf.
        EXTENDED_DATA_TOP = 8,
    };

    /// Default constructor
    NiTerrainDataLeaf(NiUInt32 uiLevel, NiMetaDataStore* pkMetaDataStore);

    /// Destructor
    ~NiTerrainDataLeaf();
    
    /**
        Dynamically generate a (sector-space) vertex from the height-map.

        Since geometry indices are in local (block) scope, we need the
        location of the lower-left corner of the block to convert to
        sector scope for looking up in the height-map.
        Allows us to avoid reading vertices back from the quadtree, which is
        hopefully living in GPU memory.  Could be inaccurate if smoothing or
        other post-processes have been applied.
        
        @param pVertex The output vertex at the given local index.
        @param local The local index for which we want the sector-space vertex.
        @param uiLod The level of detail to use in the conversion.
    */
    void CreateVertexAt(NiPoint3& pVertex, const NiIndex& local,
        NiUInt32 uiLod = 0) const;

    /**
        These functions are used for moving around in the terrain quadtree.

        Valid child index values are:
        0: Bottom left
        1: Bottom right
        2: Top right
        3: Top left
    */
    /// @name Quadtree Navigation
    //@{

    /// Get the child leaf at the requested index (const version).
    inline const NiTerrainDataLeaf* GetChildAt(NiUInt32 uiIndex) const;

    /// Get the child leaf at the requested index.
    inline NiTerrainDataLeaf* GetChildAt(NiUInt32 uiIndex);
    
    /// Set the child leaf at the given index.
    inline void SetChildAt(NiUInt32 uiIndex, NiTerrainDataLeaf* pkChild);

    /// Retrieve a pointer to our parent leaf in the quadtree (const version).
    inline const NiTerrainDataLeaf* GetParent() const;

    /// Retrieve a pointer to our parent leaf in the quadtree.
    inline NiTerrainDataLeaf* GetParent();

    /// Set the pointer to our parent leaf in the quadtree.
    inline void SetParent(NiTerrainDataLeaf* pkParent);

    /**
        Attempts to find a leaf adjacent to this leaf, sharing the given 
        border.
        
        Diagonals can be represented by bitwise AND-ing together 
        two neighboring borders.

        @param uiBorder bitwise mask of NiTerrainDataLeaf::BORDERS enum
            dictating on which border to check for an adjacent leaf.
        @return Adjacent leaf if found, null if no adjacent leaf exists on
            given border.
    */
    inline NiTerrainDataLeaf* GetAdjacent(NiUInt32 uiBorder);

    /**
        Attempts to find a leaf adjacent to this leaf, sharing the given 
        border (const version).
    */
    inline const NiTerrainDataLeaf* GetAdjacent(NiUInt32 uiBorder) const;

    /**
        Attempts to find a leaf adjacent to this leaf, sharing the given 
        border.
        
        Diagonals can be represented by bitwise AND-ing together 
        two neighboring borders. This is a fast method which always assumes
        that the adjacent exists.

        @param uiBorder bitwise mask of NiTerrainDataLeaf::BORDERS enum
            dictating on which border to check for an adjacent leaf.
        @return Adjacent leaf if found, bad pointer if no adjacent leaf exists 
            on given border
    */
    inline NiTerrainDataLeaf* GetAdjacentFast(NiUInt32 uiBorder);

    /**
        Attempts to find a leaf adjacent to this leaf, sharing the given 
        border (fast, const version).
    */
    inline const NiTerrainDataLeaf* GetAdjacentFast(NiUInt32 uiBorder) const;
    //@}

    /// @name Parent Sector
    //@{
    /// Retrieve a pointer to the sector that owns this quadtree leaf (const
    /// version).
    inline const NiTerrainSector* GetContainingSector() const;

    /// Retrieve a pointer to the sector that owns this quadtree leaf.
    inline NiTerrainSector* GetContainingSector();
    
    /// Set the pointer to the sector that owns this quadtree leaf.
    void SetContainingSector(NiTerrainSector* pkContainingSector);
    //@}

    /**
        Get the detail level of this tree node.
        
        From 0 at lowest detail, to n at the highest detail.
    */
    inline NiUInt32 GetLevel() const;

    /**
        Set the detail level of this tree node.
        
        From 0 at lowest detail, to n at the highest detail.
    */
    inline void SetLevel(NiUInt32 uiLevel);
    
    /**
        Get the number of LOD subdivisions below this block's level. This is 
		equivalent to Sector->GetNumLOD() - GetLevel() - 1.
     */
	inline NiUInt32 GetNumSubDivisions() const;

    /// Retrieve the SIZE of the data leaf. Size is always a power of 2.
    inline NiUInt32 GetBlockSize() const;

    /// Retrieve the WIDTH of the data leaf, in vertices.
    ///
    /// This value will be equal to (2^n) + 1 for some n.
    inline NiUInt32 GetWidthInVerts() const;

    /// World Location bounding sphere.
    inline const NiBound& GetLocalBound() const;

    /// World Location bounding box.
    inline const NiBoxBV& GetLocalBoxBound() const;

    /**
        Get the bottom left index of this leaf.
        
        The bottom left index corresponds to the sector scope index of the 
        vertex found at the bottom left of this leaf.

        @param kIndex Sector scope index of the bottom left vertex found in 
            this leaf.
    */
    inline void GetBottomLeftIndex(NiIndex& kIndex) const;

    /**
        Set the bottom left index of this leaf.
        
        @param kIndex Sector scope index of the bottom left vertex found in 
            this leaf.
    */
    inline void SetBottomLeftIndex(const NiIndex& kIndex);

    /**
        The Block ID of a leaf is a unique key that is used to identify any 
        given leaf, at any level. 

        A Block ID takes into account the level of the leaf and the bottom 
            left index.
     */
    inline NiUInt32 GetBlockID() const;
    
    
    /// Set the block ID for this leaf.
    inline void SetBlockID(NiUInt32 usRegionID);

    /**                   
        Get the region ID for this leaf.
        
        The region ID also corresponds to the index of the 
        NiDataStream::Region within the sectors static position stream
        that belongs to this leaf.
    */
    inline NiUInt32 GetRegionID() const;
    
    /// Set the region ID for this leaf.
    inline void SetRegionID(NiUInt32 uiRegionID);

    /**
        Return the current sum of all the vertex world locations within this
        leaf.
    */
    inline const NiPoint3& GetSumOfVertexPositions() const;

    /**
        Set the current sum of all the vertex world locations within this
        leaf.
        
        Used internally when loading vertex data from disk
     */
    inline void SetSumOfVertexPositions(const NiPoint3& kSum);

    /**
        Reset the NiBound object according to the given center and radius, and
        store the given sum of positions.

        @param kCenter Center of bounding sphere
        @param fRadius Radius of bounding sphere
        @param kPositionSum Sum of all vertex positions that were used to
            calculate the center and radius
    */
    void SetBoundData(NiPoint3 kCenter, float fRadius, NiPoint3 kPositionSum);
    
    /// Reset the bouding box according to the given box.
    inline void SetBoundVolumeBox(const NiBoxBV& kBox);
    
    /**
        Create stream iterators to be stored in this leaf, that point to the
        corresponding regions in the correct data streams.
        
        All stream locks given as parameters to this function must be locks
        to the sectors static data streams. If this function detects that
        our block has a dynamic data stream, it will point the iterator to
        the dynamic stream instead.

        @param pkPositionLock A lock to the static position stream
        @param pkNormalLock A lock to the static normal stream
        @param pkTangentLock A lock to the static tangent stream
        @param pkUVLock A lock to the static uv stream
        @param pkIndexLock A lock to the static index stream
     */
    void CreateIterators(NiDataStreamLock* pkPositionLock,
        NiDataStreamLock* pkNormalLock, 
        NiDataStreamLock* pkTangentLock,
        NiDataStreamLock* pkUVLock,
        NiDataStreamLock* pkIndexLock);

    /**
        Set the given iterator to the last position iterator generated by the
        CreateIterators function.
     */
    inline void GetPositionIterator(
        NiTerrainPositionRandomAccessIterator& kIterator) const;

    /**
        Set the given iterator to the last normal iterator generated by the
        CreateIterators function.
     */
    inline void GetNormalIterator(
        NiTerrainNormalRandomAccessIterator& kIterator) const;

    /**
        Set the given iterator to the last tangent iterator generated by the
        CreateIterators function.
     */
    inline void GetTangentIterator(
        NiTerrainTangentRandomAccessIterator& kIterator) const;

    /**
        Set the given iterator to the last index iterator generated by the
        CreateIterators function (16 bit version).
     */
    inline void GetIndexIterator(
        NiTStridedRandomAccessIterator<NiUInt16>& kIterator) const;

    /**
        Set the given iterator to the last index iterator generated by the
        CreateIterators function (32 bit version).
     */
    inline void GetIndexIterator(
        NiTStridedRandomAccessIterator<NiUInt32>& kIterator) const;

    /**
        Set the given iterator to the last UV iterator generated by the
        CreateIterators function.
     */
    inline void GetUVIterator(
        NiTStridedRandomAccessIterator<NiPoint2>& kIterator) const;

    /** 
        Returns a bitwise OR'd enum defining what type of extended data this
        leaf requires. Uses the NiTerrainDataLeaf::EXTENDED_DATA enum.
     */
    inline NiUInt8 GetHasExtendedData() const;

    /** 
        Sets a bitwise OR'd enum defining what type of extended data this
        leaf requires. Uses the NiTerrainDataLeaf::EXTENDED_DATA enum.
     */
    inline void SetHasExtendedData(NiUInt8 ucMask);

    /**
        Retrieve an NiTerrainVertex object that is found within this block. 

        The given indices are in leaf scope, meaning a value from 0 to 
        GetSize() (inclusive). 
     
        @param kVertex Resulting vertex.
        @param kLocal LEAF SCOPE index of the vertex within this leaf to 
            retrieve.
        @note This function is not very fast, use the NiPoint3 variant where
            possible.
        @note Border vertices are conceptually shared between adjacent blocks, 
            but the data itself is duplicated.
     */
    void GetVertexAt(NiTerrainVertex& kVertex, const NiIndex& kLocal) 
        const;

    /**
        Retrieves an NiTerrainVertex object that is found within this block. 

        The given indices are in leaf scope, meaning a value 
        from 0 to GetSize() (inclusive).

        @param kVertex Resulting vertex.
        @param kLocal LEAF SCOPE index of the vertex within this leaf to 
            retrieve.
        @note You can derive the local X, Y position of a vertex from the index
            itself. It is recommended to do so as opposed to calling this 
            function where possible.
        @note Border vertices are conceptually shared between adjacent blocks, 
            but the data itself is duplicated.
    */
    inline void GetVertexAt(NiPoint3& kVertex, const NiIndex& kLocal) const;

    /**
        Retrieves an NiTerrainVertex object that is found within the parent 
        sector.
        
        The given indices are in sector scope, meaning a value from 0 to
        SectorSize (inclusive).

        @param kVertex Resulting vertex.
        @param kLocal SECTOR SCOPE index of the vertex to retrieve
        @note This function is not very fast, use the NiPoint3 variant where
            possible.
     */
    inline void GetVertexAtSector(
        NiTerrainVertex& kVertex, const NiIndex& kLocal) const;

    /**
        Retrieves an NiTerrainVertex object that is found within the parent 
        sector.
        
        The given indices are in leaf scope, meaning a value from 0 to
        SectorSize (inclusive).

        @param kVertex Resulting vertex.
        @param kLocal SECTOR SCOPE index of the vertex to retrieve
        @note You can derive the local X, Y position of a vertex from the index
            itself. It is recommended to do so as opposed to calling this 
            function where possible.
    */
    inline void GetVertexAtSector(
        NiPoint3& kVertex, const NiIndex& kLocal) const;

    /**
        Return the height of the vertex at the given leaf scope index.
    */
    inline float GetHeightAt(const NiIndex& kLocal) const;

    /**
        Return the normal of the vertex at the given leaf scope index.
    */
    void GetNormalAt(NiPoint3& kNormal, const NiIndex& kLocal) const;

    /**
        Return the height of the vertex at the given sector scope index.
    */
    inline float GetHeightAtSector(const NiIndex& kSector) const;

    /**
        Return the sector scope index, generated from a given leaf scope index.
    */
    inline const NiIndex SectorIndexFromLocal(const NiIndex& kLocal) const;
    
    /// Match a node in the NiTerrainBlock quadtree to this leaf.
    void SetQuadMesh(NiTerrainBlock* pkQuadMesh);

    /**
        Return a reference to the equivalent node in the parallel 
        NiTerrainBlock tree.
    */
    NiTerrainBlock* GetQuadMesh() const;
    
    /**
        Calculates if the given vertex is on any of this leaves edges. The
        given vertex must be present within the leaf, otherwise this 
        function will return false. This function is not recursive.
         
        @param kVertex Vertex to perform border checking upon
        @return At least one NiTerrainDataLeaf::BORDERS member, OR'd together
     */
    NiUInt32 GetVertexBorders(const NiTerrainVertex& kVertex) const;

    /**
        Convert a given sector scope index to a leaf scope index within 
        this leaf. 
        
        @param kSector Sector scope index to convert.
        @param kLocal Resulting leaf scope index.
        @return True if given sector scope index corresponds to a local index
    */
    bool IndexToLocal(const NiIndex& kSector, NiIndex& kLocal) const;
    
    /**
        Convert a given sector scope index to a leaf scope index within 
        this leaf.
        
        This is an UNSAFE FUNCTION, it performs no bounds checking.
        
        @param kSector Sector scope index to convert.
        @param kLocal Resulting leaf scope index.
    */
    inline void IndexToLocalFast(const NiIndex& kSector, 
        NiIndex& kLocal) const;
    
    /// Return true if this leaf or any child leaves contain the given vertex.
    inline bool TreeContains(const NiTerrainVertex& kVertex) const;

    /**
        Add a surface and associated mask to this leaf.
        
        If the surface already exists, it will simply overwrite the existing
        mask reference.
        
        @param pkSurface The surface to associate with the leaf node.
        @param pkSurfaceMask Surface mask with which to associate with the 
            leaf node.
        @param uiNewPriority The position in the surface priority to insert.
        @return True if the addition was successful, false if the leaf is full.
     */
    bool AddSurfaceMask(const NiSurface* pkSurface, 
        const NiSurfaceMask* pkSurfaceMask, NiUInt32 uiNewPriority = 0);

    /**
        Set the alpha mask reference, for a surface contained within this 
        leaf.
        
        @param pkSurface The surface associated with the leaf node.
        @param pkSurfaceMask Surface mask associated with the leaf node.
        @return True if the surface was found, false if surface is not in leaf.
    */
    bool SetSurfaceMask(const NiSurface* pkSurface, 
        const NiSurfaceMask* pkSurfaceMask);

    /**
        Gets the alpha mask associated with the given surface.
      
        @return Mask associated with the given surface, or 0 if no mask was 
        found
     */
    const NiSurfaceMask* GetSurfaceMask(const NiSurface* pkSurface) const;

    void UpdateSurfaceMaskAddresses(NiUInt8* pusOldAddress,
        NiUInt8* pcNewAddress);

    /**
        Finds what the alpha of the given surface is, at a given vertex.
        
        Since the alpha may differ according to which block is being checked, 
        an average of all different versions of the point is taken - over
        all detail levels below this.
      
        @param kCurVertex The desired vertex location.
        @param pkSurface The surface associated with the leaf node.
        @return Alpha associated with the given surface at the given vertex
     */
    NiUInt8 GetAverageAlpha(const NiTerrainVertex& kCurVertex, 
        const NiSurface* pkSurface) const;

    /** 
        Gets the surface mask associated with the given mask priority.
        
        Does not take into account inherited masks from parents, only masks
        unique to this level.
        
        @param uiPriority Which mask to retrieve
        @return A pointer to the mask in use at the given priority, or 0 if
            no valid surface was found.
     */
    const NiSurfaceMask* GetSurfaceMask(NiUInt32 uiPriority) const;

    /** 
        @return Width in pixels of the surface mask for this data leaf.
     */
    const NiUInt32 GetSurfaceMaskWidth() const;

    /**
        Remove a surface from this leaf.
        
        Optionally specify not to recalculate the best pixel spacing cache
        after the surface is removed, in which case it must be performed
        manually before any surface deformation occurs.
        
        @param pkSurface A surface associated with the leaf node.
        @param bRecalculateBestPixelSpacing Whether or not to recompute the
            best pixel spacing mask.
        @return True if the surface was found.
     */
    bool RemoveSurfaceMask(const NiSurface* pkSurface, 
        bool bRecalculateBestPixelSpacing = true);

    /**
        Set the use of custom layer ordering.
        
        This flag is not used internally within this class, but by the
        calling class.
    */
    void SetUseCustomSurfacePriority(bool bUseCustomSurfacePriority);

    /**
        Get the use of custom layer ordering.
        
        This flag is not used internally within this class, but by the
        calling class.
    */
    bool GetUseCustomSurfacePriority() const;

    /**
        Change the priority of the given surface within this leaf.
        
        Will shift the priority of other surfaces within the list.
        
        @return True if the surface exists within this leaf.
    */
    bool SetSurfacePriority(const NiSurface* pkSurface, 
        NiUInt32 uiNewPriority);
    
    /**
        Retrieves the current drawing priority of the given NiSurface.
        
        @return True if the surface exists within this leaf.
    */
    bool GetSurfacePriority(const NiSurface* pkSurface, 
        NiUInt32& uiPriority) const;

    /** 
        The number of surfaces that this leaf uses, at this level. Does not 
        include inherited surfaces, only surfaces unique to this level.
        
        @return The number of surfaces used
    */
    NiUInt32 GetSurfaceCount() const;

    /** 
        Gets the surface associated with the given mask priority.
        
        Does not take into account inherited surfaces from parents, only
        surfaces unique to this level.
        
        @param uiPriority Which surface to retrieve
        @return A pointer to the surface in use at the given priority, or 0 if
            no valid surface was found.
    */
    const NiSurface* GetSurface(NiUInt32 uiPriority) const;
    
    /**
        Request an update of the tree.
        
        If any vertices within this leaf have changed their locations, 
        an update must be requested. This is a recursive function, and 
        will trigger an update request up to the root of the tree. 
        This function does not perform the actual update, just flags that 
        one is required. This function makes an assumption: if a parent 
        already has new data, then all of its parents also have new data.
     */
    inline void RequestUpdate();

    /**
        Returns true if this leaf, or any leafs below us require an update,
        otherwise false.
    */
    inline bool RequiresUpdate() const;
    
    /**
        Apply any incremental changes that were made to this leaves bounds via
        the RecenterCountainingBounds function.
    */
    void Update();
        
    /**
        Recalculate the average vertex location, assuming pkVertex moved from
        its old kOldLocation to the new (current) location within pkVertex.
        
        Will only update the bound for this leaf, not any child leaves.
    */
    inline void RecenterCountainingBound(const NiIndex& kLocalIndex, 
        float fOldHeight, float fNewHeight);

protected:

    /// Determines the quad tree level.
    NiUInt32 m_uiLevel;

    /// The ID of our position region (in the appropriate LOD's stream)
	/// This ID is in QuadTree space, ie Siblings are stored sequentially
    NiUInt32 m_uiRegionID;

	/// The ID of the block and the position in the sectors leaf 
    /// array
    NiUInt32 m_uiBlockID;

    /// Associated node in the parallel NiTerrainBlock tree
    NiTerrainBlock* m_pkQuadMesh;

    /// A pointer to the sector that contains this quad tree
    NiTerrainSector* m_pkContainingSector;

    /// A pointer to the sectors settings for ease of access
    const NiTerrainSectorData* m_pkSectorData;

    NiTerrainDataLeaf* m_pkParent;
    NiTerrainDataLeaf* m_apkChildren[4];

    NiUInt32 m_uiVertexStride;
    NiIndex m_kDataBottomLeftIndex;

    // Some iterators to allow fast access to the relevant data
    NiTerrainPositionRandomAccessIterator m_kPositionIterator;
    NiTerrainNormalRandomAccessIterator m_kNormalIterator;
    NiTerrainTangentRandomAccessIterator m_kTangentIterator;
    NiTStridedRandomAccessIterator<NiPoint2> m_kUVIterator;
    NiTStridedRandomAccessIterator<NiUInt16> m_kIndexIteratorNiUInt16;
    NiTStridedRandomAccessIterator<NiUInt32> m_kIndexIteratorNiUInt32;

    /// Extended data mask. Defines which extended data quadrants this leaf
    /// requires.
    NiUInt8 m_ucExtendedDataMask;

    /// Contains all meta data associated with this leaf
    NiMetaData m_kMetaData;

    /// The maximum number of surfaces a single leaf can reference
    static const NiUInt32 ms_uiMaxNumSurfaces = 8;

    /// The number of surfaces this leaf currently references
    NiUInt32 m_uiNumUsedSurfaces;

    /// An array of references to surface alphas. The ordering of members of 
    /// this array is NOT associated with the priority of the surfaces.
    const NiSurface* m_pkSurfaces[ms_uiMaxNumSurfaces];

    /// An array of references to surface alphas, corresponding to the surfaces
    /// within the m_pkSurfaces array
    const NiSurfaceMask* m_pkSurfaceMasks[ms_uiMaxNumSurfaces];

    /// Layer Ordering. Index 0 is the highest priority. Each value represents
    /// an index in m_pkSurfaces and m_appcSurfaceAlphas. 
    NiUInt32 m_auiSurfacePriority[ms_uiMaxNumSurfaces];

    /// This flag has no effect internally within this class, however is used
    /// by the calling classes.
    bool m_bUseCustomSurfacePriority;

    /// A sum of all the vertex locations within this leaf.
    NiPoint3 m_kSumOfVertexLocations;

    /// World Location bounding sphere
    NiBound m_kBound;            

    /// World Location bounding box
    NiBoxBV m_kBoxBound;

    /// A flag used to request an update in the next update cycle
    bool m_bRequiresUpdate;

private:        
    NiTerrainDataLeaf* DoGetAdjacent(NiUInt32 uiBorder) const;
    NiTerrainDataLeaf* DoGetAdjacentFast(NiUInt32 uiBorder) const;
};

#include "NiTerrainDataLeaf.inl"

#endif // NITERRAINDATALEAF_H
