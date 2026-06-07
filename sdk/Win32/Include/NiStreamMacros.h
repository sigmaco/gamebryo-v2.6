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

#ifndef NISTREAMMACROS_H
#define NISTREAMMACROS_H

class NiStream;
class NiObject;

//---------------------------------------------------------------------------
// Macros for declaring and registering streams.
//---------------------------------------------------------------------------
// Insert in class declaration (note the start of public scope).  The
// routine IsEqual is for debugging purposes only.
#define NiDeclareAbstractStream \
    public: \
        virtual void LoadBinary(NiStream& kStream); \
        virtual void LinkObject(NiStream& kStream); \
        virtual bool RegisterStreamables(NiStream& kStream); \
        virtual void SaveBinary(NiStream& kStream); \
        virtual bool IsEqual(NiObject* pkObject)

#define NiDeclareStream \
    NiDeclareAbstractStream; \
        static NiObject* CreateObject(const char** pcArguments = 0, \
            unsigned int uiArgCount = 0)

// Call for each streamable class in libname##SDM::Init function.
#define NiRegisterStream(classname) \
    NiStream::RegisterLoader(#classname, classname::CreateObject)

// Call for each streamable class in libname##SDM::Shutdown function.
#define NiUnregisterStream(classname) \
    NiStream::UnregisterLoader(#classname)

// macros for creating a object
#define NiImplementCreateObject(classname) \
    NiObject* classname::CreateObject(const char**, \
            unsigned int) \
    { \
        classname* pkObject = NiNew classname; \
        NIASSERT(pkObject != NULL); \
        return pkObject; \
    }

#endif // NISTREAMMACROS_H
