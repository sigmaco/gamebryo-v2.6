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

#ifndef NIBUILDCONFIGURATION_H
#define NIBUILDCONFIGURATION_H

#if (!defined(NIDEBUG) && !defined(NIRELEASE) && !defined(NISHIPPING))
#error Exactly one of NIDEBUG, NIRELEASE, or NISHIPPING must be defined.
#endif

// The standard build configuration options are only set if 
// the options have not been defined.  If it is desired to
// override these options then that should be done within the
// below section (or within a project configuration file).
//
// // e.g. This line will turn off metrics for all configurations
// #define NIMETRICS 0

// --- BEGIN USER CUSTOM OVERRIDES ---

// --- END USER CUSTOM OVERRIDES ---


// Standard build configuration options

#ifndef NIMETRICS
#    if defined(NIDEBUG)
#        define NIMETRICS 1
#    elif defined(NIRELEASE)
#        define NIMETRICS 1
#    elif defined(NISHIPPING)
#        define NIMETRICS 0
#    endif
#endif

#ifndef NI_USE_MEMORY_MANAGEMENT
#    if defined(__SPU__)
#        define NI_USE_MEMORY_MANAGEMENT 0
#    elif defined(NIDEBUG)
#        define NI_USE_MEMORY_MANAGEMENT 1
#    elif defined(NIRELEASE)
#        define NI_USE_MEMORY_MANAGEMENT 1
#    elif defined(NISHIPPING)
#        define NI_USE_MEMORY_MANAGEMENT 0
#    endif
#endif

#endif //#ifndef NIBUILDCONFIGURATION_H
