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

#ifndef NICOMPONENTINPUT_H
#define NICOMPONENTINPUT_H

#include <NiObject.h>
#include <NiDataStreamRef.h>
#include <NiFloat16.h>
#include "NiMeshProfileProcessorLibType.h"

/// @cond EMERGENT_INTERNAL
/**
    An interface abstraction class used by the 
    NiMeshProfileOperations::MergeAndSwizzle operation. This class allows
    the operation to care not if the input data is coming from a stream
    or literal value.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiComponentInput : public NiMemObject
{
public:
    inline virtual ~NiComponentInput() {};

    virtual bool GetValue(NiUInt32 uiIndex, float& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiFloat16& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt32& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt16& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiUInt8& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiInt32& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiInt16& val) = 0;
    virtual bool GetValue(NiUInt32 uiIndex, NiInt8& val) = 0;

    /**
        Defines a basis for creating or comparing streams. In the context of
        the NiMeshProfileOperations::MergeAndSwizzle operation, the basis
        for all inputs must be compatible.
    */
    class StreamBasis : public NiMemObject
    {
    public:
        StreamBasis() : m_bIsValid(false) {}; 
        bool m_bIsValid;

        bool m_bIsPerInstance;
        NiUInt16 m_uiNumSubmeshes;
        NiObject::CloningBehavior m_eCloningBehavior;
        NiUInt8 m_uiAccessMask;
        NiDataStream::Usage m_eUsage;
        NiUInt32 m_uiCount;
        NiDataStreamElement m_kElement;
        NiTObjectSet<NiDataStream::Region> m_kRegions;
        NiTPrimitiveSet<NiUInt32> m_kRegionRefs;

        StreamBasis& operator= (const StreamBasis& b);
        StreamBasis& operator= (const NiDataStreamRef& kSR);
    };

    inline virtual bool AssignBasis(StreamBasis&) const { return false; }
    inline virtual bool IsCompatible(const StreamBasis&) const { return true; }

    inline virtual void LockInput() {};
    inline virtual void UnlockInput() {};
};

#include "NiComponentInput.inl"

#endif // NICOMPONENTINPUT
