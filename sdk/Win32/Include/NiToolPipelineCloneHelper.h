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

#ifndef NITOOLPIPELINECLONEHELPER_H
#define NITOOLPIPELINECLONEHELPER_H

#include <NiMeshToolsLibType.h>

#include <NiMemObject.h>
#include <NiAVObject.h>
#include <NiTMap.h>
#include <NiTArray.h>
#include "NiMesh.h"
#include "NiDataStream.h"

/**
    Provides a set of helper functions to aid adding, removing, and replacing
    tool data-streams shared between mesh within the context of the tool
    pipeline.  The class assumes that all data-streams are single element
    data-streams and that they are not shared between multiple NiDataStreamRef
    objects on the same mesh.
*/
class NIMESHTOOLS_ENTRY NiToolPipelineCloneHelper : public NiMemObject
{
public:
    /// The default constructor creates the helper without any initialization.
    /// The helper may later be initialized by using the InitializeCloneSetMaps
    /// method.
    NiToolPipelineCloneHelper();
    
    /// Constructor initializes the helper with all NiMesh objects that can be
    /// found by recursively iterating over pkObject.
    NiToolPipelineCloneHelper(NiAVObject* pkObject);
    
    /// Constructor initializes the helper with all NiMesh objects that can be
    /// found by recursively iterating over each array element in kTopObjects.
    NiToolPipelineCloneHelper(NiTLargeObjectArray<NiObjectPtr>& kTopObjects);

    /**
        Set of NiMesh objects containing streams to clone
    */
    class NIMESHTOOLS_ENTRY CloneSet : public NiRefObject
    {
        NiDeclareRootRTTI(CloneSet);

    public:
        /// The default constructor creates an empty clone-set.
        CloneSet() {};

        /// Returns the number of mesh contained by the set.
        NiUInt32 GetSize();

        /// Adds a mesh object to the clone-set
        NiUInt32 Add(NiMesh* pkMesh);

        /// Returns a pointer to the mesh at location specified.
        NiMesh* GetAt(NiUInt32 uiItem);

        /// Returns true if the clone-set contains the specified mesh and false
        /// otherwise.
        bool Contains(NiMesh* pkMesh);

        /// Returns a reference to an array of all mesh contained by the set.
        inline NiTPrimitiveArray<NiMesh*>& GetMeshSet() { return m_akMeshSet; };

        // Internal use only
        void Sort(NiTMap<NiMesh*, NiUInt32>& kMeshToIDMap);

    protected:
        NiTPrimitiveArray<NiMesh*> m_akMeshSet;

    private:
        /**
            To prevent an application from 
            inadvertently causing the compiler to
            generate the default copy constructor
            or default assignment operator,
            these methods are declared as private. 
            They are not defined anywhere,
            so code that attempts to use them will not link.
        */
        CloneSet(const CloneSet& kObject);
        CloneSet& operator=(const CloneSet& kObject);
    };
    typedef NiPointer<CloneSet> CloneSetPtr;

    /// Removes all data-streams from the set of mesh in the clone-set.
    bool ReleaseAllStreamRefs(CloneSetPtr& spCloneSet);

    /**
        Replaces the stream-ref for every mesh in the clone-set, provided that
        the semantic, index, and format can be matched. If the semantic, index,
        and format can not be matched, then the method returns false and no
        replacement occurs.
    */
    bool ReplaceStreamRef(CloneSetPtr& spCloneSet,
        const NiFixedString& kSemantic, NiUInt8 uiIndex,
        NiDataStreamElement::Format eFormat,
        NiDataStreamRef* pkNewStreamRef);

    /**
        The method looks to the old stream-ref in order to determine the data
        stream that is used and finds the set of mesh (clone-set) that share
        this data stream. Afterwards, the method replaces the stream-ref for
        all mesh in the clone-set. If the semantic, index, and format could not
        be matched, then the method returns false and no replacement occurs.
    */
    bool ReplaceStreamRef(NiDataStreamRef* pkOldStreamRef,
        NiDataStreamRef* pkNewStreamRef);

    /**
        Adds the stream-ref to the set of mesh specified by the clone-set. The
        underlying data-stream used by the stream-ref should not have
        previously been used by any mesh. Once the stream-ref has been
        successfully added to the clone-set, the helper will create a mapping
        from the data-stream to the clone-set for any future operations.
    */
    bool AddStreamRef(CloneSetPtr& spCloneSet, NiDataStreamRef* pkStreamRef);

    /**
        Finds the set of mesh sharing the same data-stream as the stream-ref
        and then removes from those mesh all stream-refs that use that
        data-stream.
    */
    bool ReleaseStreamRef(NiDataStreamRef* pkStreamRef);
    
    /**
        Removes the stream-refs from the set of mesh specified by the clone-set
        that match the semantic, index, and format. Once the stream-ref has
        been successfully removed from the clone-set, the helper will remove
        its internal mapping from the underlying datastream to the clone-set.
    */
    bool ReleaseStreamRef(
        CloneSetPtr& spCloneSet, 
        const NiFixedString& kSemantic,
        NiUInt8 uiIndex, 
        NiDataStreamElement::Format eFormat);

    /// Replaces an existing data-stream with another. This method will use the
    /// helpers internal data-stream to clone-set mapping to find the set of
    /// mesh that are affected.
    bool ReplaceDataStream(NiDataStream* pkDS, NiDataStream* pkReplacement);

    /// Finds all mesh associated with the data-stream. Then, for each of those
    /// mesh, finds the data-stream refs that use that data-stream. Then, for
    /// those data-streams, it calls BindRegionToSubmesh with uiSubmeshIdx and
    /// uiDataStreamRegionIdx as parameters.
    bool BindRegionToSubmesh(NiDataStream* pkDS, NiUInt32 uiSubmeshIdx, 
    NiUInt32 uiDataStreamRegionIdx);

    /// Retrieves a clone-set by data-stream. If the data-stream to clone-set
    /// mapping doesn't exist, false is returned.
    bool GetCloneSet(NiDataStream* pkDataStream, CloneSetPtr& spCloneMap);

    /// Retrieves all registered data-streams from the helper if the clone-set
    /// parameter is NULL. Otherwise, the clone-set parameter acts as a filter
    /// and only data streams of the specified clone-set will be retrieved.
    void GetDataStreams(NiTPrimitiveSet<NiDataStream*>& kDataStreamSet,
        CloneSet* pkCloneSet = NULL);

    /// Initializes the helper with all NiMesh objects that can be found by
    /// recursively iterating over pkObject.
    void InitializeCloneSetMaps(NiAVObject* pkObject);

    /// Initializes the helper with all NiMesh objects that can be found by
    /// recursively iterating over each array element in kTopObjects.
    void InitializeCloneSetMaps(NiTLargeObjectArray<NiObjectPtr>&
        kTopObjects);

    /// Retrieves the set of all registered NiMesh objects.
    NiTPrimitiveSet<NiMesh*>& GetCompleteMeshSet();

    /// Returns all clone-sets
    void RetrieveAllCloneSets(NiTPrimitiveSet<CloneSet*>& kAllCloneSets);

    /// Retrieve all the clone-sets
    void GetAllCloneSets(NiTObjectSet<CloneSetPtr>& kAllCloneSets);

    /// Retrieve all registered clone-sets that contain the specified mesh.
    void GetCloneSetsThatIncludeMesh(NiTObjectSet<CloneSetPtr>& kCloneSets,
        NiMesh* pkMesh);

    /// Clears the clone maps. This effectively re-initializes the helper.
    void ClearMaps();



protected:
    /**
        Traverses the scene graph and makes a map of all shared
        data streams. The mapping indicates which mesh share the
        same data streams. Also, a secondary map is made that allows
        a mesh to find the data stream set it is in.
    */
    void InitializeCloneSetMaps();

    /// Recursively builds the set of NiMesh objects from the root specified
    void RecursiveConstructMeshSet(NiAVObject* pkObject);

    /// Mapping to clone-sets. Current implementation allows clone-set to be
    /// retrieved by data stream pointer.
    NiTPointerMap<NiDataStream*, CloneSetPtr> m_kDSToCloneSet;

    /// The complete known set of mesh
    NiTPrimitiveSet<NiMesh*> m_kCompleteMeshSet;
};


#endif // #ifndef NITOOLPIPELINECLONEHELPER_H
