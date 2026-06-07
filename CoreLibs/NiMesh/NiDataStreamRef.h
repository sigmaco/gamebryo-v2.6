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

#ifndef NIDATASTREAMREF_H
#define NIDATASTREAMREF_H

#include "NiDataStream.h"
#include <NiFixedString.h>
#include <NiStream.h>

/**
    Helper class to encapsulate all common interactions with datastreams that
    are bound to an NiMesh.
*/
class NIMESH_ENTRY NiDataStreamRef : public NiMemObject
{
public:
    /// Constructor
    NiDataStreamRef();

    /// Copy Constructor
    NiDataStreamRef(const NiDataStreamRef& kOther);

    /// Destructor
    ~NiDataStreamRef();
    
    /// Assignment operator
    NiDataStreamRef& operator=(const NiDataStreamRef& kSrc);

    /// @name NiDataStream Access
    //@{
    /// Get the datastream for this stream ref 
    inline NiDataStream* GetDataStream();

    /// Get the datastream for this stream ref (const-safe)
    inline const NiDataStream* GetDataStream() const;

    /** 
        Set the datastream for this stream ref.  
        
        @note Note that this will potentially trigger a reallocation of the 
            internal semantic mapping array if the size is not the same. 
            The contents will be preserved.
    */
    void SetDataStream(NiDataStream* pkDataStream);
    //@}

    /// @name Iteration Helper Methods
    //@{
    /// Get the stride of a single instance in the datastream.
    inline NiUInt32 GetStride() const;
    
    /// Get the size of the buffer in bytes
    inline NiUInt32 GetSize() const;

    /// Gets the number of all items (verts, indices, etc) in the stream.
    ///
    /// @note This ignores the region information.
    inline NiUInt32 GetTotalCount() const;

    /**
        Search through all bound submesh regions, summing up the ranges for
        all uniquely defined regions. 
        
        GetUniqueCount() will differ from GetTotalCount() if regions overlap
        or if there are portions of the data stream with no defined region
        bound to a submesh.
    */
    NiUInt32 GetUniqueCount() const;

    /// Gets the number of items (verts, indices, etc) bound to a given 
    /// submesh.
    inline NiUInt32 GetCount(NiUInt32 uiSubmeshIdx) const;

    /// Get the access rights on the stream
    inline NiUInt8 GetAccessMask() const;

    /// Gets the usage of the stream (index, vertex, constant)
    inline NiDataStream::Usage GetUsage() const;   
    //@}

    /// @name NiDataStream::Region Access
    //@{
    /// Get the region of the datastream associated with this submesh
    inline NiDataStream::Region& GetRegionForSubmesh(NiUInt32 uiSubmeshIdx); 

    /// Get the region of the datastream associated with this submesh 
    /// (const-safe)
    inline const NiDataStream::Region& GetRegionForSubmesh(
        NiUInt32 uiSubmeshIdx) const; 

    /**
        When using the datastream for a given submesh, it should only use the
        specified sub-region of the datastream.
        
        This method will add the region to the datastream if it doesn't
        already exist.
    */
    void BindRegionToSubmesh(NiUInt32 uiSubmeshIdx, 
        const NiDataStream::Region& kRegion);

    /**
        When using the datastream for a given submesh, it should only use the
        specified sub-region of the datastream.
        
        This method must reference a pre-existing region on the datastream.
    */
    inline void BindRegionToSubmesh(NiUInt32 uiSubmeshIdx, 
        NiUInt32 uiDataStreamRegionIdx);
    //@}

    /// @name NiDataStreamElement Description Access
    //@{
    /// Get the element descriptions contained within this StreamRef
    inline NiUInt32 GetElementDescCount() const;
    
    /// Get the element description at the specified index
    inline const NiDataStreamElement& GetElementDescAt(
        NiUInt32 uiElementDescIdx) const;
    
    /// Get the semantic name bound to the specified element description index
    inline const NiFixedString& GetSemanticNameAt(
        NiUInt32 uiElementDescIdx) const;

    /// Get the semantic index bound to the specified element description
    /// index
    inline NiUInt32 GetSemanticIndexAt(NiUInt32 uiElementDescIdx) const;

    /// Get the element description bound to the specified semantic 
    inline const NiDataStreamElement& FindElementDesc(
        const NiFixedString& kSemantic, NiUInt32 uiSemanticIdx = 0) const;

    /**
        Get the index of the element description bound to the specified
        semantic.
        
        The return is UINT_MAX if not found.
    */
    NiUInt32 FindElementDescIndex(const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIdx = 0) const;

    /** 
        Set the semantic for the specified element description index.
        
        The element description index must already exist on the NiDataStream
        for this method to succeed. 
    */
    inline void BindSemanticToElementDescAt(NiUInt32 uiElementIdx, 
        const NiFixedString& kSemantic, NiUInt32 uiSemanticIdx);

    /// Get the number of element descs that match the input semantic name.
    NiUInt32 GetSemanticCount(const NiFixedString& kSemantic) const;
    
    /// Gets the largest semantic index matching the input semantic name.
    ///
    /// @return -1 if not found, otherwise the index.
    NiInt32 GetMaxIndexBySemantic(const NiFixedString& kSemantic) const;

    //@}

    /// @name Instancing Methods
    /// @note All instanced datastreams on a mesh must have the same count of
    ///     struct instances per submesh. 
    //@{
    /// Determine whether or not the datastream contains instanced data.
    inline bool IsPerInstance() const;
    /// Set whether or not the datastream contains instanced data.
    inline void SetPerInstance(bool bIsPerInstance);
    //@}

    /// Sets the active number of items for a given submesh.
    inline void SetActiveCount(NiUInt32 uiSubmeshIdx, NiUInt32 uiCount);

    /// Clear out all of the internal storage associated with this streamref.
    inline void Reset();

    /// Compare this stream ref with another stream ref.
    bool IsEqual(const NiDataStreamRef* pkOther) const;

    /**
       Returns true if the data stream reference is valid for consumption
       by the renderers and NiMeshAlgorithms.
       
       Checks if the data stream reference is non-null, if there is at 
       least one entry in the submesh-to-region map, and if each entry in the 
       submesh-to-region map references a valid region.

       @param pkInvalidDescription If provided, set to cause of failure.
    */
    bool IsValid(NiString* pkInvalidDescription = NULL) const;

    /// @cond EMERGENT_INTERNAL

    /**
        Set the submesh count to the specified size. 
        
        This will resize the internal array, preserving existing values, 
        and filling in the remainder with uiRemainderFill.
    */
    void SetSubmeshCount(NiUInt32 uiSubmeshCount, 
        NiUInt16 uiRemainderFill = 0xFFFF);

    /// Helper method for getting the internal remapping index.
    NiUInt32 GetRegionIndexForSubmesh(NiUInt32 uiSubmeshIdx) const; 

    /// Get number of submesh to region mappings.
    NiUInt32 GetSubmeshRemapCount() const;

    /// Creates a deep copy of this ref, including its datastream.
    NiDataStreamRef CreateFullCopy() const;

    bool GetStreamable() const;
    void SetStreamable(bool bStreamable);
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);
    bool RegisterStreamables(NiStream& kStream);

    /// @endcond

private:
    /**
        Wrapper class for a semantic index and string
    */
    class Semantic : public NiMemObject
    {
    public:
        Semantic();
        NiFixedString m_kName;
        NiUInt32 m_uiIndex;
    };

    /// Compute the region on the datastream associated with the given
    /// submesh.
    inline NiUInt32 RemapSubmeshToRegion(NiUInt32 uiSubmeshIdx) const;

    /// The data stream
    NiDataStreamPtr m_spDataStream;

    /// Array mapping from submesh index to the datastream's region index
    NiUInt16* m_puiSubmeshToRegionIndexMap;

    /// Array of semantics bound to this datastream
    Semantic* m_pkSemantics;

    /// Determines whether or not the stream is instanced
    bool m_bIsPerInstance;

    /// The count of regions to submesh mappings
    NiUInt16 m_uiSubmeshToRegionIndexMapCount;

};


// Do these two stream refs exactly match?
bool  NIMESH_ENTRY operator==(const NiDataStreamRef& r1, 
    const NiDataStreamRef& r2);
bool  NIMESH_ENTRY operator!=(const NiDataStreamRef& r1, 
    const NiDataStreamRef& r2);

#include "NiDataStreamRef.inl"

#endif // #ifndef NIDATASTREAMREF_H
