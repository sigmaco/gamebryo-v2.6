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

#ifndef NIDEBUG_H
#define NIDEBUG_H

#include "NiSystemLibType.h"

//
// NiDebugBreakProc
//

// for NiDebugBreakProc, define platform specific implementations
#if defined(_PS3)
    #if defined(__SPU__)
        #define NIDEBUGBREAKPROCIMPL() __asm__ volatile("stopd 0,1,1")
    #else
        #define NIDEBUGBREAKPROCIMPL() __asm__ volatile("tw 31,1,1")
    #endif
#elif defined(WIN32)
    #define NIDEBUGBREAKPROCIMPL() __debugbreak()
#elif defined(_WII)
    #define NIDEBUGBREAKPROCIMPL() { asm volatile {trap;}}
#elif defined(_XENON)
    #define NIDEBUGBREAKPROCIMPL() __debugbreak()
#else
    #error Platform undefined.
#endif // #ifdef _PS3

// Causes a break in debugger, function always returns false
inline bool NiDebugBreakProc() { NIDEBUGBREAKPROCIMPL(); return false; }


//
// NIASSERT
//
typedef bool (*NiAssertFailProcFuncPtr)(
    const char* pcExpression,
    const char* pcFile,
    const char* pcFunction,
    const int iLine);

class NISYSTEM_ENTRY NiAssertFail
{
public:
    // This assertion is the generic platform-specific assert handler
    static bool DefaultAssertFail(
        const char* pcExpression,
        const char* pcFile,
        const char* pcFunction,
        const int iLine);

    // This assertion routine will simply write out a debug string. 
    // It will not trigger a breakpoint. This is useful for running 
    // automated tests.
    static bool SimpleAssertFail(
        const char* pcExpression,
        const char* pcFile,
        const char* pcFunction,
        const int iLine);

    static NiAssertFailProcFuncPtr ms_pfnNiAssertFailProc;
};

// NIASSERTINLINE results in a boolean type, 
// to enable nesting into statement calls, such as NiVerifyStaticCast
#ifdef NIDEBUG
    #if defined(_PS3) && defined(__SPU__)
        #define NIASSERTINLINE(boolean_expresion) \
            ((boolean_expresion) ? true : NIDEBUGBREAK())
    #else
        #define NIASSERTINLINE(boolean_expresion) \
            ((boolean_expresion) ? true : \
            NiAssertFail::ms_pfnNiAssertFailProc( \
                #boolean_expresion, __FILE__, __FUNCTION__, __LINE__))
    #endif
#else
    #define NIASSERTINLINE(value) false
    // Defaults to false to avoid misuse of form: ClientFunc(NIASSERT(...))
    // Use NIVERIFY instead in those situations.
#endif

#define NIASSERT(boolean_expresion) ((void)NIASSERTINLINE(boolean_expresion));

//
// NIVERIFY
//
#ifdef NIDEBUG
    #define NIVERIFY(condition) NIASSERTINLINE(condition)
#else
    #define NIVERIFY(condition) (condition)
#endif

//
// NIVERIFYEQUALS
//
#ifdef NIDEBUG
    #define NIVERIFYEQUALS(always_computed_value, debug_verified_value) \
        NIASSERTINLINE((always_computed_value) == (debug_verified_value))
#else
    #define NIVERIFYEQUALS(always_computed_value, debug_verified_value) \
        (always_computed_value)
#endif

//
// NIVERIFYNOTEQUALS
//
#ifdef NIDEBUG
    #define NIVERIFYNOTEQUALS(always_computed_value, debug_verified_value) \
        NIASSERTINLINE((always_computed_value) != (debug_verified_value))
#else
    #define NIVERIFYNOTEQUALS(always_computed_value, debug_verified_value) \
        (always_computed_value)
#endif

//
// NIDEBUGBREAK
//
#ifdef NIDEBUG
    #define NIDEBUGBREAK() NiDebugBreakProc()
#else
    #define NIDEBUGBREAK() false
#endif


#endif // NIDEBUG_H
