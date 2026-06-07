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

#ifndef NIMPPMESSAGES_H
#define NIMPPMESSAGES_H

#include <NiSystem.h>
#include <NiMain.h>
#include "NiMeshProfileProcessorLibType.h"

/// @cond EMERGENT_INTERNAL
/**
    Specifies all the error, warning, and annotation messages used by the
    NiMeshProfileProcessor system.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiMPPMessages
{
public:
    // Errors
    static const char ERROR_MISSING_BASIS[];
    static const char ERROR_NO_STREAM_FOUND[];
    static const char ERROR_NON_TOOL_DATASTREAM_ENCOUNTERED[];
    static const char ERROR_INTERLEAVE_INIT[];
    static const char ERROR_INTERLEAVE_FINALIZE[];
    static const char ERROR_STRICTINTERLEAVE_WITH_2ARGS[];
    static const char ERROR_INTERLEAVE_MISSING_SEMANTIC_2ARGS[];
    static const char ERROR_CASTCONVERT_FAILED_2ARGS[];
    static const char ERROR_CLAMP_FAILED[];

    static const char ERROR_REINTERPRETFORMAT_FAILED[];
    static const char ERROR_REMAP_FAILED[];
    static const char ERROR_RENAME_FAILED[];
    static const char ERROR_MERGEANDSWIZZLE_FAILED[];
    static const char ERROR_MERGEANDSWIZZLE_FAILED_2ARGS[];
    static const char ERROR_MERGEANDSWIZZLE_FAILED_TO_SET_3ARGS[];
    static const char ERROR_MERGEANDSWIZZLE_FAILED_UNKNOWN[];
    static const char ERROR_EXPECTED_REQUIRED_STREAM_FAILED_RESOLVE_2ARGS[];
    static const char ERROR_EXPECTED_STREAM_FAILED_RESOLVE_2ARGS[];
    static const char ERROR_REQUIRED_STREAM_FAILED_TO_FIND_INPUT_2ARGS[];

    static const char ERROR_MERGE_SEMANTIC_CONFLICT_1ARG[];
    static const char ERROR_MERGE_USAGE_CONFLICT_2ARGS[];
    static const char ERROR_MERGE_ACCESS_CONFLICT_2ARGS[];
    static const char ERROR_MERGE_FORMAT_CONFLICT_2ARGS[];
    static const char ERROR_MERGE_INTERLEAVE_CONFLICT_2ARGS[];

    static const char ERROR_COMPONENTPACK_FAILED[];

    // Warnings
    static const char WARNING_PROFILE_NOT_FOUND[];
    static const char WARNING_PROFILE_NOT_FOUND_PROF_ONLY[];
    static const char WARNING_PROFILE_NOT_FOUND_MESH_ONLY[];
    static const char WARNING_PROFILE_NOT_FOUND_PROF_MESH[];
    static const char WARNING_FAILED_TO_MERGE_1ARG[];
    static const char WARNING_MESH_MODIFIER_REMOVED_1ARG[];
    static const char WARNING_DEINTERLEAVING_STREAMS_1ARG[];
};


#endif // NIMPPMESSAGES_H