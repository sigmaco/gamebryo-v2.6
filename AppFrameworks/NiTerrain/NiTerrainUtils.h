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

#ifndef NITERRAINUTILS_H
#define NITERRAINUTILS_H

#include <NiCollision.h>

#include "NiDeformablePointSet.h"
#include "NiTQuickReleasePool.h"

class NiAVObject;
class NiTerrainDataLeaf;
class NiTerrainSector;
class NiRay;
class NiSurface;

/// @cond EMERGENT_INTERNAL

/** 
    Namespace containing collision utilities functions.
    @internal
    @note Intended for Emergent internal use only
*/
namespace NiTerrainUtils
{

    /// A const variable used to identify the MAXIMUM level of detail 
	/// in searching functions:
	static const NiUInt32 ms_uiMAX_LOD = (NiUInt32)(-1);

    /**
        Tests the given ray against an NiTerrainDataLeaf tree for 
        collision. The ray should be set with LOCAL space coordinate.
        
        @param kRay ray to test against
        @param pkDataLeaf Optional root of AeQuadDataTree to use as 
            beginning of search
        @return True if the Bound intersects the quad tree
    */
    bool TestRay2D(NiRay& kRay, const NiTerrainDataLeaf* pkDataRoot = 0, 
        NiUInt32 uiDetailLOD = NiTerrainUtils::ms_uiMAX_LOD);
   
    /**
        Tests the given ray against an NiTerrainDataLeaf tree for 
        collision. The ray should be set with LOCAL space coordinate
        
        @param kRay ray to test against
        @param pkDataLeaf Optional root of AeQuadDataTree to use as 
            beginning of search
        @return True if the Bound intersects the quad tree
    */
    bool TestRay(NiRay& kRay, const NiTerrainDataLeaf* pkDataRoot = 0, 
        NiUInt32 uiDetailLOD = NiTerrainUtils::ms_uiMAX_LOD);

    /**
        Tests the given bounding sphere against an NiTerrainDataLeaf tree
        for collision. This test is only used if the bound is static. The 
        bound should be in LOCAL space coordinates.

        @param kVolume Bounding sphere to test against
        @param pkDataLeaf Optional root of AeQuadDataTree to use as 
        beginning of search
        @return True if the Bound intersects the quad tree
    */
    bool TestBound(const NiBound& kVolume,
        const NiTerrainDataLeaf* pkDataLeaf = NULL);

    /**
        Tests the given bounding sphere against an NiTerrainDataLeaf tree
        for collision. This test is only used if the bound is static
        and only tests the bound against the horizontal plane of the 
        quadleaf. The bound should be in LOCAL space coordinates

        @param kVolume Bounding sphere to test against
        @param pkDataLeaf Optional root of AeQuadDataTree to use as
        beginning of search
        @return True if the Bound intersects the quad tree
    */
    bool TestBound2D(const NiBound& kVolume,
        const NiTerrainDataLeaf* pkDataLeaf = NULL);

    /** 
        @name Vertex retrieval functions 

        @internal
        @note Intended for Emergent internal use only
     */
    //@{

    /**
        Function that returns the vertices included inside a given
        NiBound. The bound should be in LOCAL space coordinates

        @param kBound the volume to test with
        @param spPointSet a set of points that will be modified by the
            function to return the points contained in the bound
        @param uiNumVerts The number of vertices contained in the bound
            (returned parameter)
        @param pkDataLeaf the quadtree leaf against which to test 
            the bound
    */
    void GetVertsInBound(const NiBound &kLocalBound, 
        NiDeformablePointSet* pkPointSet, NiUInt32& uiNumVerts,
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true,
        const NiTerrainDataLeaf* pkDataLeaf = 0) ;

    /**
        Function that returns the vertices included inside a given
        NiBound. This function only tests the bound against the horizontal 
        plane of the quadleaf. The bound should be in LOCAL space 
        coordinates

        @param kBound the volume to test with
        @param spPointSet a set of points that will be modified by the
            function to return the points contained in the bound
        @param uiNumVerts The number of vertices contained in the bound
            (returned parameter)
        @param pkDataLeaf the quadtree leaf against which to test 
            the bound
    */
    void GetVertsInBound2D(const NiBound &kLocalBound, 
        NiDeformablePointSet* pkPointSet, NiUInt32& uiNumVerts,
        NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true,
        const NiTerrainDataLeaf* pkDataLeaf = 0) ;

    /** 
        @name Surface mask pixel retrieval functions 

        @internal
        @note Intended for Emergent internal use only
     */
    //@{

    /**
        Assume the bound is in world space coordinates
    */
    void GetPixelsInBound(const NiBound &kLocalBound,
        NiDeformablePointSet* pkPointSet,
        NiUInt32& uiNumPixels,
        NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool,
        const NiSurface* pkSurface,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true,
        NiTerrainDataLeaf* pkDataLeaf = 0);

    /**
        Assume the bound is in world space coordinates
    */
    void GetPixelsInBound2D(const NiBound &kLocalBound,
        NiDeformablePointSet* pkPointSet,
        NiUInt32& uiNumPixels,
        NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool,
        const NiSurface* pkSurface,
        NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD,
        bool bIncludeLowerDetail = true,
        NiTerrainDataLeaf* pkDataLeaf = 0);

    /**
        This attempts to return the opacity of the given surface, at the 
        given WORLD location. Does not take into account surfaces on higher
        levels of detail. Does not take into account the Z component of the
        given world location
            
        @note Assumes the bound is in world space coordinates
    */
    bool GetSurfaceOpacity(
        const NiSurface* pkSurface, const NiPoint3& kLocation,
        NiUInt8& ucValue, NiUInt32 uiDetailLevel = NiTerrainUtils::ms_uiMAX_LOD, 
        bool bSearchLowerDetail = true,
        const NiTerrainDataLeaf* pkDataLeaf = 0) ;

    //@}

    #include "NiTerrainUtils.inl"
}

/// @endcond

#endif // NITERRAINUTILS_H
