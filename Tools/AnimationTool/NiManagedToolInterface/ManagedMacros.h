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

#pragma once

// This macro converts a managed String* object to a const char* pointer.
#define MStringToCharPointer(stringname) \
    (const char*) System::Runtime::InteropServices::Marshal \
        ::StringToHGlobalAnsi(stringname).ToPointer()

// This macro frees a pointer that was allocated with MStringToCharPointer.
#define MFreeCharPointer(stringname) \
    System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*) \
        stringname))

// This macro defines a managed assertion function.
#define MAssert System::Diagnostics::Debug::Assert
