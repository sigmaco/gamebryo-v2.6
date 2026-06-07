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

#ifndef NIMESHREQUIREMENTS_H
#define NIMESHREQUIREMENTS_H

#include <NiRefObject.h>
#include <NiFixedString.h>
#include <NiTPtrSet.h>
#include <NiTArray.h>

#include "NiMeshLibType.h"
#include "NiDataStream.h"
#include "NiDataStreamElement.h"
#include "NiMeshErrorInterface.h"

/**
    This class holds the set of requirements that a NiMesh object
    must meet in order to support an associated shader, modifier, etc.
    
    These requirements are on the semantics present on the stream and
    all aspects of their formatting and storage.

    Each object holds an array of requirement sets. The requirements will be
    considered satisfied if all of the contents of any one set are satisified.

    The NiMeshRequirements class is used by the tool pipeline profile process
    and that system attempts to satisfies requirements from various modifiers
    and will use the first satisfyable set. As such, requirement sets should
    be specified with the most restrictive sets first.
*/
class NIMESH_ENTRY NiMeshRequirements : public NiRefObject
{
public:
    /// Constructed object contains no requirements.
    NiMeshRequirements(const char* pkRequirementName,
        NiSystemDesc::RendererID eRenderer);

    /// Destructor
    ~NiMeshRequirements();

    /**
        The various ways in which a stream interleaving request can be
        satisfied.
        
        STRICT_INTERLEAVE indicates that any semantic with the same streamID
        must be kept together - those semantics can not exist in separate
        data stream.
        
        CAN_SPLIT allows additional semantics to be specified in a stream, but
        allows for those semantics to be split apart if necessary. If those
        semantics are split apart, it is important to note that only other
        semantics having the same StreamID can exists with them.

        FLOATER indicates that the stream that the semantic is placed in
        doesn't matter. The FLOATER enumeration is used specifically by the
        tool pipeline profile system to allow a semantic to be merged into any
        stream that the profile needs the semantic to be in. In general,
        mixing FLOATERS with CAN_SPLIT and STRICT_INTERLEAVE streams should be
        avoided to ensure maximum flexibility.

        As an example, imagine that a profile specifies semantics A,B,C as
        CAN_SPLIT and gives each semantic the same streamID. Also imagine that
        a modifer specifies that A and B are optionally together (CAN_SPLIT)
        in the same stream but that C is marked as a FLOATER. In this
        situation, the merge will fail. When semantics are defined with
        STRICT_INTERLEAVE or CAN_SPLIT, the stream is being defined such that
        no other semantics can exist in that stream other than those
        specified.

        In the example given, the profile specifies that ABC can exist
        together in the same stream but that no other semantic can exist with
        it. It also allows for the semantics to be split apart and in any
        combination just as long as no other semantic is placed with it.
        Likewise, the modifier, by specifying semantics A and B as CAN_SPLIT
        and giving them the same streamID is ruling out that C can ever exist
        in that stream. In order for a successful merge to occur, the modifier
        should specify C as CAN_SPLIT, or alternatively, it should define
        semantics A and B as FLOATERS so that the profile will merge it in
        with semantic C. 

        No concept exists at present for specifying that a subset of semantics
        should strictly be interleaved, but that any other semantic (a
        wildcard specification) may be added to the same stream.
    */
    enum StreamBehavior
    {
        /// See text above.
        STRICT_INTERLEAVE = 0,

        /// See text above.
        CAN_SPLIT,

        /// See text above.
        FLOATER
    };

    /**
        A SemanticRequirement object represents the needs for a single
        semantic.
    */
    class NIMESH_ENTRY SemanticRequirement : public NiRefObject
    {
    public:
        /// Default constructor sets the semantic name to NULL, hence leaving
        /// the requirement invalid.
        SemanticRequirement();

        /**
            Constructor sets all of the properties of the requirement.
            
            @param kSemantic The semantic name for which the requirement
                holds.
            @param uiSemanticIndex The semantic index for which the
                requirement holds.
            @param eStreamBehavior The stream behavior requirements. See the
                details documented under the
                NiMeshRequirements::StreamBehavior enumeration.
            @param uiStreamID The id stream for which the semantic should be
                associated with. Note that the stream behavior parameter
                effects how this id is interpreted. For FLOATERs, the stream
                id has no effect.
            @param eUsage Specifies the required usage. When the tool-pipeline
                profile system attempts to merge modifier requirements, usage
                must have exact agreement or else merging fails.
            @param uiAccess Specifies the required access flags. When the
                tool pipeline profile system attempts to merge access flags,
                it allows for increases in access.
            @param eFormat Specifies the required format. If more formats are
                allowed, they can be added after construction.
        */
        SemanticRequirement(const NiFixedString& kSemantic,
            NiUInt32 uiSemanticIndex, StreamBehavior eStreamBehavior,
            NiUInt32 uiStreamID, NiDataStream::Usage eUsage, NiUInt8 uiAccess,
            NiDataStreamElement::Format eFormat);

        /// Type to hold a set of formats.
        typedef NiTPrimitiveSet<NiDataStreamElement::Format> FormatSet;

        /// The required semantic name.
        NiFixedString m_kSemantic;

        /// The required semantic index.
        NiUInt32 m_uiIndex;

        /// The required usage.
        NiDataStream::Usage m_kUsage;

        /// The set of required access flags. Any access flags that provide
        /// at least the required level of access will be considered
        /// acceptable.
        NiUInt8 m_uiAccessFlags;

        /// The set of allowable formats.
        FormatSet m_kFormats;

        /// Specifies how the stream id is to be used.
        StreamBehavior m_eStreamBehavior;

        /// An ID indicating which stream this element should go into.
        NiUInt32 m_uiStreamID;

        /// Assignment operator
        SemanticRequirement& operator= (const SemanticRequirement& r);
    };
    typedef NiPointer<SemanticRequirement> SemanticRequirementPtr;

    /// Type for holding a set of requirements
    typedef NiTObjectPtrSet<SemanticRequirementPtr> RequirementSet;

    /// Access to the target platform, required because modifiers, and
    /// hence requirements, might be platform specific.
    inline NiSystemDesc::RendererID GetRenderer() const;
    
    /// Set the renderer we are seeking requiremnts for (default is
    /// any renderer).
    inline void SetRenderer(const NiSystemDesc::RendererID eRenderer);

    /**
        Creates a new empty set of requirements.

        @return The index of the new set.
    */
    inline NiUInt32 CreateNewRequirementSet();

    /**
        Obtain the number of requirement sets.

        @return The number of indexes that must be iterated over to
        examine every set of requirements.
    */
    inline NiUInt32 GetSetCount() const;

    /**
        Add a requirement to the specified set.

        @param uiSet The set for which the requirement should be added to.

        @param pkRequirement The requirement to add. This pointer will
        subsequently be owned by the NiMeshRequirements class via a
        smart pointer, and applications must not delete it.

        @return The index at which the requirement was added.
    */
    inline NiUInt32 AddRequirement(NiUInt32 uiSet, SemanticRequirement*
        pkRequirement);

    /**
        Add a requirement to the specified set.

        @param uiSet The set for which the requirement should be added to.

        @param kSemantic The semantic name for which the requirement holds.

        @param uiSemanticIndex The semantic index for which the requirement
            holds.

        @param eStreamBehavior The stream behavior requirements. See the
            details documented under the StreamBehavior enumeration.

        @param uiStreamID The id stream for which the semantic should be
            associated with. Note that the stream behavior parameter effects
            how this id is interpreted. For FLOATERs, the stream id has no
            effect.

        @param eUsage Specifies the required usage. When the tool-pipeline
            profile system attempts to merge modifier requirements, usage
            must have exact agreement or else merging fails.

        @param uiAccess Specifies the required access flags. When the tool-
            pipeline profile system attempts to merge access flags, it allows
            for increases in access.

        @param eFormat Specifies the required format. If more formats are 
            allowed, they can be added after construction.

        @return The index at which the requirement was added.
    */
    inline NiUInt32 AddRequirement(NiUInt32 uiSet,
        const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIndex, NiMeshRequirements::StreamBehavior
        eStreamBehavior, NiUInt32 uiStreamID, NiDataStream::Usage eUsage,
        NiUInt8 uiAccess = 0, NiDataStreamElement::Format eFormat = 
        NiDataStreamElement::F_UNKNOWN);

    /**
        Get the number of requirements in the specified set.

        @param uiSet Specifies the requirement set to query.

        @return The number of requirements for the particular set.
    */
    inline NiUInt32 GetRequirementCount(NiUInt32 uiSet) const;

    /**
        Retrieve the requirement set at the specified index

        @param uiIndex Specifies the requirement set to retrieve.

        @return A pointer to the requested requirement-set.
    */
    inline RequirementSet* GetSetAt(NiUInt32 uiIndex) const;

    /**
        Obtain a requirement from the specified set.

        @param uiSet The set for which to query.

        @param uiIndex The index of the desired requirement.
        
        @return A pointer to the requirement, or NULL if the requested
        index does not hold a requirement. Applications should assign
        the returned pointer to a smart pointer if they wish the pointer to
        remain valid after the NiMeshRequirements object is deleted.
    */
    SemanticRequirement* GetRequirement(NiUInt32 uiSet, NiUInt32 uiIndex)
        const;

    /** 
        Deletes all requirement sets and releases all semantic requirements.
    */
    void ReleaseAllSemanticRequirements();

    /// Add a requirement set.
    inline void AddRequirementSet(RequirementSet* pkReqSet);

    /// Assignment operator.
    NiMeshRequirements& operator= (const NiMeshRequirements& r);

    /// A name given to the requirement.
    ///
    /// Not necessary but may be useful for debugging.
    NiString m_kReqName;

protected:
    /**
        Array of sets of requirements. The entire group of requirements is
        considered valid if any one of the sets can be satisfied.
    */
    NiTPrimitiveArray<RequirementSet*> m_kRequirementSets;

    /// The renderer for which these requirements are targeted.
    NiSystemDesc::RendererID m_eRenderer;
};

NiSmartPointer(NiMeshRequirements);

#include "NiMeshRequirements.inl"

#endif // #ifndef NIMESHREQUIREMENTS_H
