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

#include "NiMPPMessages.h"

const char NiMPPMessages::ERROR_MISSING_BASIS[] =
    "  ERROR(001): Semantic marked for auto-generation, however, no " \
    "stream-basis could be found.\n";
const char NiMPPMessages::ERROR_NO_STREAM_FOUND[] =
    "  ERROR(002): No streams found on mesh.\n";
const char NiMPPMessages::ERROR_NON_TOOL_DATASTREAM_ENCOUNTERED[] =
    "  ERROR(003): Non-tool datastreams encountered. Check plug-ins " \
    "in script.\n";
const char NiMPPMessages::ERROR_INTERLEAVE_INIT[] =
    "  ERROR(004): Intialization step during Interleave operation failed.\n";
const char NiMPPMessages::ERROR_INTERLEAVE_FINALIZE[] =
    "  ERROR(005): Finalize step during Interleave operation failed.\n";
const char NiMPPMessages::ERROR_STRICTINTERLEAVE_WITH_2ARGS[] =
    "  ERROR(006): Unable to satisfy StrictInterleave.\n" \
    "    Check profile and mesh modifiers { %s, %d }.\n";
const char NiMPPMessages::ERROR_INTERLEAVE_MISSING_SEMANTIC_2ARGS[] =
    "  ERROR(007): Unable to find semantic for interleave.\n" \
    "    Check profile and mesh modifiers { %s, %d }.\n";
const char NiMPPMessages::ERROR_CASTCONVERT_FAILED_2ARGS[] =
    "  ERROR(008): CastConvert operation failed.\n" \
    "    Check Profile. Check Semantic  { %s, %d }.\n";
const char NiMPPMessages::ERROR_CLAMP_FAILED[]=
    "  ERROR(009): Clamp operation failed. Check Profile.\n";
const char NiMPPMessages::ERROR_REINTERPRETFORMAT_FAILED[]=
    "  ERROR(010): ReinterpretFormat operation failed. Check Profile.\n";
const char NiMPPMessages::ERROR_REMAP_FAILED[]=
    "  ERROR(011): Remap operation failed. Check Profile.\n";
const char NiMPPMessages::ERROR_RENAME_FAILED[]=
    "  ERROR(012): Rename operation failed. Check Profile.\n";
const char NiMPPMessages::ERROR_MERGEANDSWIZZLE_FAILED[] =
    "  ERROR(013): Unable to find required Semantic for Merge and Swizzle" \
    " operation.\n" \
    "    Check Semantics in Profile.\n";
const char NiMPPMessages::ERROR_MERGEANDSWIZZLE_FAILED_2ARGS[]=
    "  ERROR(014): Unable to find required Semantic for Merge and Swizzle" \
    "operation. Hint: { %s , %d }.\n"
    "    Check Semantic in Profile or consider auto generating.\n";
const char NiMPPMessages::ERROR_MERGEANDSWIZZLE_FAILED_TO_SET_3ARGS[] =
    "  ERROR(015): Found specified stream but was unable to set it as an" \
    " input for the Merge and Swizzle operation.\n" \
    "    Check referenced component { %d } for stream { %s, %d }.\n";
const char NiMPPMessages::ERROR_MERGEANDSWIZZLE_FAILED_UNKNOWN[]=
    "  ERROR(016): Merge and Swizzle operation failed. Check Profile.\n";
const char NiMPPMessages::ERROR_EXPECTED_REQUIRED_STREAM_FAILED_RESOLVE_2ARGS[]
    = "  ERROR(017): A required stream was expected to resolve but failed." \
      "Interleaving will be skipped.\n" \
    "    Check Profile regarding Semantic: {%s, %d}\n";
const char NiMPPMessages::ERROR_EXPECTED_STREAM_FAILED_RESOLVE_2ARGS[]=
    "  ERROR(018): A stream was expected to resolve but failed.\n" \
    "Interleaving will be skipped.\n" \
    "    Check Profile regarding Semantic: {%s, %d}\n";
const char NiMPPMessages::ERROR_REQUIRED_STREAM_FAILED_TO_FIND_INPUT_2ARGS[]=
    "  ERROR(019): A stream definition could not find its required input.\n" \
    "    Check Profile regarding Semantic: {%s, %d}\n";

const char NiMPPMessages::ERROR_MERGE_SEMANTIC_CONFLICT_1ARG[] =
    "  ERROR(020): Semantic Merge Conflict : Semantic {%s } is a modifier " \
    " requirement but is either not defined by the profile or is missing.\n";
const char NiMPPMessages::ERROR_MERGE_USAGE_CONFLICT_2ARGS[] =
    "  ERROR(021): Usage Merge Conflict: Semantic { %s:%d } conflicted in " \
    " USAGE.\n" \
    "    Check the profile and modifier's requirements.\n";
const char NiMPPMessages::ERROR_MERGE_ACCESS_CONFLICT_2ARGS[] =
    "  ERROR(022): Access Merge Conflict: Semantic { %s:%d } had " \
    "non-mergeable access flags.\n" \
    "    Check the profile and modifier's requirements.\n";
const char NiMPPMessages::ERROR_MERGE_FORMAT_CONFLICT_2ARGS[] =
    "  ERROR(023): Format Merge Conflict: Semantic { %s:%d } " \
    "had non-mergeable formats.\n" \
    "    Check the profile and modifier's requirements.\n";
const char NiMPPMessages::ERROR_MERGE_INTERLEAVE_CONFLICT_2ARGS[] =
    "  ERROR(024): Interleave Merge Conflict: The profile stream containing " \
    "the semantic { %s:%d } could not be merged with a stream specified by " \
    "a modifier due to StrictInterleave requirements.\n" \
    "    Check the profile and modifier's requirements.\n";
const char NiMPPMessages::ERROR_COMPONENTPACK_FAILED[]=
    "  ERROR(025): ComponentPack operation failed. Check Profile.\n";

const char NiMPPMessages::WARNING_PROFILE_NOT_FOUND[]=
    "  WARNING(001): Profile could not be found.\n" \
    "    Check the assigned profile. Trying the default profile.\n";
const char NiMPPMessages::WARNING_PROFILE_NOT_FOUND_PROF_ONLY[] =
    "  WARNING(001): Profile { %s } could not be found.\n" \
    "    Check the assigned profile. Trying the default profile.\n";
const char NiMPPMessages::WARNING_PROFILE_NOT_FOUND_MESH_ONLY[] =
    "  WARNING(001): Profile could not be found for mesh { %s }.\n" \
    "    Check the assigned profile. Trying the default profile.\n";
const char NiMPPMessages::WARNING_PROFILE_NOT_FOUND_PROF_MESH[] =
    "  WARNING(001): Profile { %s } could not be found for mesh { %s }.\n" \
    "    Check the assigned profile. Trying the default profile.\n";
const char NiMPPMessages::WARNING_FAILED_TO_MERGE_1ARG[]=
    "  WARNING(002): Failed to merge modifiers requirements {%s}.\n" \
    "    For details, see the messages that follow which report all merge\n" \
    "    failures from all requirement sets. Keep in mind that only\n" \
    "    one set needs to be satisfied and that all semantic conflicts do\n" \
    "    not necessarly need to be resolved for a successful merge.\n\n";
const char NiMPPMessages::WARNING_MESH_MODIFIER_REMOVED_1ARG[]=
    "  WARNING(003): Mesh Modifier { %s } Removed.\n";

const char NiMPPMessages::WARNING_DEINTERLEAVING_STREAMS_1ARG[]=
    "  WARNING(004): Found streams that contain interleaved elements.\n" \
    "    The following streams will be de-interleaved in order to operate\n" \
    "    with the mesh profile system:\n%s\n";


