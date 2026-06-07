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
#ifndef NIINTERLEAVEPROFILE_H
#define NIINTERLEAVEPROFILE_H

#include "NiMeshProfileProcessorLibType.h"

#include <NiTSet.h>
#include <NiFixedString.h>
#include <NiString.h>
#include <NiDataStreamRef.h>
#include <NiMemObject.h>

class NiDataStream;

/// @cond EMERGENT_INTERNAL
/**
    Specifies how existing data streams should be interleaved together.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiInterleaveProfile :
    public NiMemObject
{
public:
    /**
        Defines a semantic element entry within a stream layout.
    */
    class Element: public NiMemObject
    {
    public:

        // Constructors
        Element();
        Element(NiFixedString& kSemantic, NiUInt32 kSemanticIndex,
            bool bStrictInterleave = true, bool bCoalesceOnSplit = false);
        Element(const char* pcSemantic, NiUInt32 kSemanticIndex,
            bool bStrictInterleave = true, bool bCoalesceOnSplit = false);
        Element(const Element&);

        // Assignment operator for set support
        inline Element& operator=(const Element& rhs);

        // Search parameters used to locate stream
        NiFixedString m_kSemantic;
        NiUInt32 m_uiSemanticIndex;

        // If true, specifies that element *must* be interleaved as defined
        // otherwise, an error must be reported.
        bool m_bStrictInterleave;

        // Only valid if strict interleaving is false. If strict interleaving
        // is false and m_bCoalesceOnSplit is also false, the element will
        // receive its own datastream. If true, it will be coalesced with
        // other like elements.
        bool m_bCoalesceOnSplit;

        // Result parameters determined at runtime
        NiDataStreamRef m_kStreamRef;
        NiUInt32 m_uiElementIdx;

        NiDataStreamElement m_kElement;

    };

    /**
        Defines a layout of semantic elements specifying how the data streams
        should be interleaved into a.
    */
    class StreamLayout : public NiTObjectSet<Element>
    {
    public:
        
        // Constructors
        StreamLayout();
        StreamLayout(const StreamLayout&);
        
        // Assignment operator for set support
        StreamLayout& operator=(const StreamLayout& rhs);
    };

    /**
        Defines the overall interleave scheme with a sett of stream layouts.
    */
    class StreamLayoutSet: public NiTObjectSet<StreamLayout>
    {
    public:
        // Constructors
        StreamLayoutSet();
        StreamLayoutSet(const StreamLayoutSet&);
        
        // Assignment operator for set support
        StreamLayoutSet& operator=(const StreamLayoutSet& rhs);
    };
    
    // Constructor
    NiInterleaveProfile();

    // Gets the set of streams, each of which describes its element layout
    inline StreamLayoutSet& GetStreamLayoutSet() { return m_kStreamLayoutSet;}

protected:
    StreamLayoutSet m_kStreamLayoutSet; // Streams
};

#include "NiInterleaveProfile.inl"

#endif // NIINTERLEAVEPROFILE_H

