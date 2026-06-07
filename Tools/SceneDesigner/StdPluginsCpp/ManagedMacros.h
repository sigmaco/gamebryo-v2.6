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

// This macro increments the ref count for an NiRefObject.
#define MInitRefObject(refobject) \
    MAssert(refobject != NULL, "Error: Attempting to initialize null " \
    "NiRefObject!"); \
    refobject->IncRefCount();

// This macro decrements the ref count for an NiRefObject.
#define MDisposeRefObject(refobject) \
    if (refobject) \
    { \
    refobject->DecRefCount(); \
    refobject = 0; \
    }

// This macro increments the reference count for an interface.
#define MInitInterfaceReference(interfaceref) \
    MAssert(interfaceref != NULL, "Error: Attempting to initialize null " \
    "interface reference!"); \
    interfaceref->AddReference();

// This macro decrements the reference count for an interface.
#define MDisposeInterfaceReference(interfaceref) \
    if (interfaceref) \
    { \
    interfaceref->RemoveReference(); \
    interfaceref = 0; \
    }

// This macro verifies that an MDisposable instance is valid. It should be
// used at the start of every function of an MDisposable-derived class.
#define MVerifyValidInstance \
    MAssert(!HasBeenDisposed(), "Cannot access a previously disposed object");

// This macro returns a pointer to the registered service of the specified
// type. It returns a null pointer if the service cannot be found.
#define MGetService(servicetype) \
    dynamic_cast<servicetype*>(Emergent::Gamebryo::SceneDesigner::PluginAPI \
    ::ServiceProvider::Instance->GetService(__typeof(servicetype)));
