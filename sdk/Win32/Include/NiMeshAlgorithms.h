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

#ifndef NIMESHALGORITHMS_H
#define NIMESHALGORITHMS_H

#include "NiMesh.h"
#include "NiDataStream.h"


/**
    A class containing static templated functions for performing various
    algorithms on NiMesh objects.
*/
class NIMESH_ENTRY NiMeshAlgorithms
{
public:

    /** 
        For each sub-mesh within the NiMesh, call the supplied functor for
        every region in the sub-mesh, unless that region has already been
        called for a different sub-mesh in which case it is not called again.

        The TRegionFunctor could be either a function or a class.
        The signature for a function would be:
    
        void CustomRegionFunction(
            void* pRegionDataBegin, 
            void* pRegionDataEnd, 
            NiUInt32 uiStride, 
            NiDataStreamElement::Format eFormat)
    
        The signature for a class with an overloaded () function:
    
        class ClassRegionFunctor
        {
        public:
            void operator () (
                void* pRegionDataBegin, 
                void* pRegionDataEnd, 
                NiUInt32 uiStride, 
                NiDataStreamElement::Format eFormat)
            {
            }
        };
    */
    template <typename TRegionFunctor>
    static bool ForEachUniqueRegion(
        NiMesh* pkMesh, 
        const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex, 
        TRegionFunctor& kFunc, 
        NiUInt8 ucLockMask);

    /**
        @name Primitive-Based Foreach Functions.

        The TPrimitiveFunctor could be either a function or a class.
        The signature for a function would be:

        bool CustomPrimitiveFunction(const NiUInt32* pIndices, 
            NiUInt32 uiIndexCount, NiUInt32 uiPrimitiveIdx, NiUInt16 uiSubMesh)

        The signature for a class with an overloaded () function:

        class PrimitiveFunctor
        {
          public:
              bool operator () (const NiUInt32* pIndices, 
                  NiUInt32 uiIndexCount, NiUInt32 uiPrimitiveIdx,
                  NiUInt16 uiSubMesh)
              {
              }
        };
        
        The pIndices array contains the vertex indices for the primitive,
        uiIndexCount is the number of indices, uiPrimitiveIndex is as index
        for the primitive in it's sub-mesh (given in uiSubMesh). The first
        primitive (triangle, for instance) in the sub-mesh is 0, the next
        triangle is 1, and so on. uiPrimitiveIndex is NOT an index into the
        INDEX data stream.
        
        @param pkMesh The mesh to iterate over.
        @param uiSubMesh The sub-mesh to work on (ForEachPrimitiveOneSubmesh
            only).
        @param kFunctor The functor to call on each primitive.
        @param ucLockMask The mask to use to lock the INDEX stream.
        @param bAllowEarlyOut If true, the iteration over primitive will
            terminate immediately if the functor returns false for any
            primitive element. If false, all primitives are processed even if
            one returns an error.
    */
    //@{
    /// Primitive foreach that visits the primitives for each submesh
    /// (static function).
    template <typename TPrimitiveFunctor>
    static bool ForEachPrimitiveAllSubmeshes(
        NiMesh* pkMesh, 
        TPrimitiveFunctor& kFunctor, 
        NiUInt8 ucLockMask = NiDataStream::LOCK_READ,
        bool bAllowEarlyOut = false);

    /// Primitive foreach that visits the primitives for a specific sub-mesh
    /// (static function).
    template <typename TPrimitiveFunctor>
    static bool ForEachPrimitiveOneSubmesh(
        NiMesh* pkMesh, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor, 
        NiUInt8 ucLockMask = NiDataStream::LOCK_READ,
        bool bAllowEarlyOut = false);
    //@}

protected:
    /**
        @name Helper Functions For Foreach Primitive Algorithms. 
        
        @return If bAllowEarlyOut is specified, the return value of each 
        functor is checked. If the value if false, primitive processing 
        finishes and the return value of the foreach will be false, indicating
        all primitives were not processed. If all primitives are processed, 
        the return value for the foreach will be true.
    */
    //@{
    /// Primitive foreach that visits explicit regions of the datastream.
    /// The submesh that is given should match the region that we are visiting.
    template <typename TPrimitiveFunctor>
    static bool ForEachPrimitiveByRegion(
        NiPrimitiveType::Type eType, 
        NiDataStream* pkDataStream, 
        NiUInt32 uiRegion, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor, 
        NiUInt8 ucLockMask = NiDataStream::LOCK_READ,
        bool bAllowEarlyOut = false);

    /// Primitive foreach that works on an index buffer. 
    /// Used by ForEachPrimitiveAllSubmeshes, ForEachPrimitiveOneSubmesh and
    /// ForEachPrimitiveByRegion.
    template <typename TPrimitiveFunctor>
    static bool ForEachPrimitiveCore(
        NiPrimitiveType::Type eType, 
        NiDataStreamElement::Format eFormat,
        void* pData, 
        NiUInt32 uiIndexCount, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false); 

    /// Helper function to deal with 32-bit indexed triangles.
    template <typename TPrimitiveFunctor>
    static bool ForEachIndexedTriangle(
        NiUInt32* pIndices,  
        NiUInt32 uiIndexCount, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false);

    /// Helper function to deal with 16-bit indexed triangles.
    template <typename TPrimitiveFunctor>
    static bool ForEachIndexedTriangle(
        NiUInt16* pIndices,  
        NiUInt32 uiIndexCount, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false);

    /// Helper function to deal with unindexed triangles.
    template <typename TPrimitiveFunctor>
    static bool ForEachUnindexedTriangle(
        NiUInt32 uiIndexCount,  NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false);

    /// Helper function to deal with 32-bit indexed tristrips.
    template <typename TPrimitiveFunctor>
    static bool ForEachIndexedTriStrip(
        NiUInt32* pIndices,  
        NiUInt32 uiIndexCount, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false);

    /// Helper function to deal with 16-bit indexed tristrips.
    template <typename TPrimitiveFunctor>
    static bool ForEachIndexedTriStrip(
        NiUInt16* pIndices,
        NiUInt32 uiIndexCount,  NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false);

    /// Helper function to deal with unindexed tristrips
    template <typename TPrimitiveFunctor>
    static bool ForEachUnindexedTriStrip(
        NiUInt32 uiIndexCount, NiUInt16 uiSubMesh,
        TPrimitiveFunctor& kFunctor,
        bool bAllowEarlyOut = false);
    //@}
};

#include "NiMeshAlgorithms.inl"

#endif // NIMESHALGORITHMS_H
