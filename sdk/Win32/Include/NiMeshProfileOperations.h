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

#ifndef NIMESHPROFILEOPERATIONS_H
#define NIMESHPROFILEOPERATIONS_H

#include <NiSystem.h>
#include "NiMeshProfileProcessorLibType.h"
#include "NiInterleaveProfile.h"
#include "NiComponentInput.h"

class NiDataStreamRef;

/// @cond EMERGENT_INTERNAL
/**
    Provides a number of static operations that allows for transforming
    and processing datastreams in a variety of ways. Operations typically
    take a set of inputs and returns an NiDataStreamRef object that reflects
    the operation. Operations typically will result in producing new data
    streams and leave the original data intact.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiMeshProfileOperations
{
public:
    /**
        Enumerates how the CastConvert operation should perform.
    */
    enum CastConvertType
    {
        /**
        This enumeration specifies that the data is to be converted by
        traversing all the data and verifying that the data can be converted
        without any data loss. If it detects a single occurance it will warn
        and return without converting the stream.
        */
        CT_SMARTCLAMP = 0,

        /**
            This enumeration specifies that the data is to be converted using a
            built-in range remapping from the source type range to the
            destination type range. In the case of float values, the type
            range is considered to be 0.0f to 1.0f. If possible, an
            appropriate predefined normalized format will be used for the
            final format.
        */
        CT_NORMALIZE,

        /** 
            This enumeration specifies that the data is to be clamped by the
            upper and lower bounds of the data type that is being converted to.
        */
        CT_TYPECLAMP,

        /// This enumeration specifies that the data is simply casted to the
        /// new data type.
        CT_BLINDCAST,
        CT_COUNT
    };

    /**
        @name Static Methods
        Methods to perform operations over data streams
    */
    //@{

    /**
        MergeAndSwizzle takes a semantic name, an index, 1 to 4 stream or 
        literal value components, and results in a swizzle and merged 
        datastream that is wrapped by the returning NiDataStreamRef object.

        @param kNewSemantic Specifies the new semantic name for the newly
            merged and/or swizzled data stream 
        @param uiNewIndex Specifies the new index for the newly merged and/or
            siwzzled data stream.
        @param pkIn0 Specifies the first component input.
        @param pkIn1 Specifies the second component input.
        @param pkIn2 Specifies the third component input.
        @param pkIn3 Specifies the fourth component input.

        @note At least one input must come from a stream component in order
        for the operation to succeed in a valid NiDataStreamRef. The reason
        being is that an existing datastream is required in order to
        determine the number of elements and various datastream properties.
    */
    static NiDataStreamRef MergeAndSwizzle(
        NiFixedString& kNewSemantic, NiUInt8 uiNewIndex,
        NiComponentInput* pkIn0, NiComponentInput* pkIn1 = NULL,
        NiComponentInput* pkIn2 = NULL, NiComponentInput* pkIn3 = NULL);

    /**
        Clamp takes a NiDataStreamRef that contains a single element
        datastream, a selected component, and min/max values to use for
        clamping. If iComponent is -1, then all components are clamped.
        The returning NiDataStreamRef represents the results of the Clamp
        operation.

        @param kSrcRef The single element NiDataStreamRef that will be clamped.
        @param iComponent The component index that should be clamped. A value
            of -1 indicates that all components should be clamped.
        @param dMin The min range.
        @param dMax The max range.
    */
    static NiDataStreamRef Clamp(const NiDataStreamRef& kSrcRef,
        NiInt32 iComponent, double dMin, double dMax);

    /**
        Remap takes a NiDataStreamRef that contains a single element
        datastream, a selected component, and source/destination ranges for
        linear remapping. If iComponent is -1, then all components are
        linearly remapped. The returning NiDataStreamRef represents the
        results of the remap operation.

        @param kSrcRef The single element NiDataStreamRef that will be
            remapped.
        @param dSrcBegin The source start range.
        @param dSrcEnd The source end range.
        @param dDstBegin The destination start range.
        @param dDstEnd The destination end range.
        @param iComponent The component index that should be remapped. A value
            of -1 indicates that all components should be remapped.
    */
    static NiDataStreamRef Remap(const NiDataStreamRef& kSrcRef,
        double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
        NiInt32 iComponent);

    /**
        Rename takes a NiDataStreamRef that contains a single element
        datastream, a semantic, and an index. The returning NiDataStreamRef 
        will have the new semantic name and index.

        @param kSrcRef The single element NiDataStreamRef that will have its
            semantic renamed.
        @param kSemantic The new semantic.
        @param uiIndex The new index.

        @note The Rename operation, which only needs to change the semantic
            and index, will continue to share the underlying datastream that
            what passed in (since the datastream data did not change). This
            is the only operation that does not make a unique copy of the
            datastream.
    */
    static NiDataStreamRef Rename(const NiDataStreamRef& kSrcRef,
        NiFixedString& kSemantic, NiUInt8 uiIndex);

    /**
        CastConvert takes a NiDataStreamRef that contains a single element
        datastream, a destination type, and a conversion type. The conversion
        type may specify if the cast is a blind one, if it is type-clamped,
        or if it should be normalized. If normalized is specified, the
        final format will be one of the known normazlied formats (if
        possible). The returning NiDataStreamRef represents the results of
        the CastConvert operation.

        @param kSrcRef The single element NiDataStreamRef that will have its
            element cast-converted.
        @param eDstType The enumerated data type that each component will be
            converted to. The final format will retain the source component
            count.
        @param eConvertType The cast-method that will be used.
    */
    static NiDataStreamRef CastConvert(const NiDataStreamRef& kSrcRef,
        NiDataStreamElement::Type eDstType, CastConvertType eConvertType);

    /**
        ComponentPack takes a NiDataStreamRef that contains a single
        non-packed element datastream and packs it to a predefined Gamebryo
        pack format using the passed in parameters. The same layout as the
        incoming NiDataStreamRef will be used. If no predefined format exists
        for the parameter specification, or if the operation fails, then the
        returning NiDataStreamRef will not be valid. The operation will also
        fail if the number of components of the incoming element does not match
        the number of fields specified for component packing. If the operation,
        succeeds, the returning NiDataStreamRef will represent the results of
        the ComponentPack operation.

        @param kSrcRef The single element NiDataStreamRef that will have its
            element cast-converted.
        @param bSigned Indicates that the fields are to be signed.
        @param bNormalized Indicates that the fields are to be normalized.
        @param bLastFieldIsLiteral indicates the last field gets literal
        @param uiF0 Indicates the number of bits reserved for the first field.
        @param uiF1 Indicates the number of bits reserved for the second field.
        @param uiF2 Is optional, but if specified, indicates the number of bits
            reserved for the third field.
        @param uiF3 Is optional, but if specified, indicates the number of bits
            reserved for the fourth field.
    */
    static NiDataStreamRef ComponentPack(NiDataStreamRef& kSrcRef,
        bool bSigned, bool bNormalized, bool bLastFieldIsLiteral, NiUInt8 uiF0,
        NiUInt8 uiF1, NiUInt8 uiF2 = 0, NiUInt8 uiF3 = 0);

    /**
        ReinterpretFormat takes a NiDataStreamRef that contains a single
        element datastream and simply changes the format type w/o changing
        the data. In order for the function to succeed, the overall size of
        the data stream must be evenly divisible by the size of the new
        format type.

        @param kSrcRef The single element NiDataStreamRef that will have its
            data stream reinterpreted.
        @param eDstFormat The desired format.

        @note Reinterperting the format may result in a change of the element
            count. For example, going from a FLOAT32_4 to a FLOAT32_2 will
            double the overall count while going the reverse direction would
            half it. Also note that some reinterpretations may fail.
    */
    static NiDataStreamRef ReinterpretFormat(const NiDataStreamRef& kSrcRef, 
        NiDataStreamElement::Format eDstFormat);
    //@}

    // The following functions perform operations in place (without copying, 
    // cloning, or creating a new NiDataStreamRef).
    static void ApplyClampInPlace(NiDataStreamRef& kStreamRef, 
        double dMin, double dMax, NiInt32 iComponent);
    static void ApplyRemapInPlace(NiDataStreamRef& kStreamRef, 
        double dSrcBegin, double dSrcEnd, double dDstBegin, double dDstEnd,
        NiInt32 iComponent);
    static void ApplyRenameInPlace(NiDataStreamRef& kStreamRef, 
        NiFixedString& kGenericSemantic, NiUInt8 uiGenericIndex);

    /**
        All Mesh Operations have the same basic requirements for 
        NiDataStreamRef's. This method is used to determine if the
        NiDataStreamRef object referred to by pkStreamRef meets those 
        requirements.
    */
    static bool IsValidStreamRef(const NiDataStreamRef& pkStreamRef);
};

#endif // NIMESHPROFILEOPERATIONS_H