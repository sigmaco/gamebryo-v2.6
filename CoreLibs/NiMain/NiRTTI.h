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

#ifndef NIRTTI_H
#define NIRTTI_H

#include "NiMainLibType.h"
#include <NiMemObject.h>

// run-time type information

class NIMAIN_ENTRY NiRTTI : public NiMemObject
{
public:
    NiRTTI (const char* pcName, const NiRTTI* pkBaseRTTI);

    inline const char* GetName() const {return m_pcName;}
    inline const NiRTTI* GetBaseRTTI() const {return m_pkBaseRTTI;}

    bool CopyName(char* acNameBuffer, unsigned int uiMaxSize) const;

protected:
    const char* m_pcName;
    const NiRTTI* m_pkBaseRTTI;
};

// insert in root class declaration
#define NiDeclareRootRTTI(classname) \
    public: \
        static const NiRTTI ms_RTTI; \
        virtual const NiRTTI* GetRTTI() const {return &ms_RTTI;} \
        static bool IsExactKindOf(const NiRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return false; \
            } \
            return pkObject->IsExactKindOf(pkRTTI); \
        } \
        bool IsExactKindOf(const NiRTTI* pkRTTI) const \
        { \
            return (GetRTTI() == pkRTTI); \
        } \
        static bool IsKindOf(const NiRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return false; \
            } \
            return pkObject->IsKindOf(pkRTTI); \
        } \
        bool IsKindOf(const NiRTTI* pkRTTI) const \
        { \
            const NiRTTI* pkTmp = GetRTTI(); \
            while (pkTmp) \
            { \
                if (pkTmp == pkRTTI) \
                { \
                    return true; \
                } \
                pkTmp = pkTmp->GetBaseRTTI(); \
            } \
            return false; \
        } \
        static classname* VerifyStaticCastDebug(const NiRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return NULL; \
            } \
            classname* pkDynamicCast = DynamicCast(pkRTTI, pkObject); \
            NIASSERT("NiVerifyStaticCast() caught an invalid type cast." \
                "Check callstack for invalid object typecast assumption." \
                && pkDynamicCast); \
            return pkDynamicCast; \
        } \
        static classname* DynamicCast(const NiRTTI* pkRTTI, \
            const classname* pkObject) \
        { \
            if (!pkObject) \
            { \
                return NULL; \
            } \
            return pkObject->DynamicCast(pkRTTI); \
        } \
        classname* DynamicCast(const NiRTTI* pkRTTI) const \
        { \
            return (IsKindOf(pkRTTI) ? (classname*) this : 0 ); \
        }

// insert in class declaration
#define NiDeclareRTTI \
    public: \
        static const NiRTTI ms_RTTI; \
        virtual const NiRTTI* GetRTTI() const {return &ms_RTTI;}

// insert in root class source file
#define NiImplementRootRTTI(rootclassname) \
    const NiRTTI rootclassname::ms_RTTI(#rootclassname, 0)

// insert in class source file
#define NiImplementRTTI(classname, baseclassname) \
    const NiRTTI classname::ms_RTTI(#classname, &baseclassname::ms_RTTI)

// macros for run-time type testing
#define NiIsExactKindOf(classname, pkObject) \
    classname::IsExactKindOf(&classname::ms_RTTI, pkObject)

#define NiIsKindOf(classname, pkObject) \
    classname::IsKindOf(&classname::ms_RTTI, pkObject)

// macro for compile time type casting
#define NiStaticCast(classname, pkObject) \
    ((classname*) pkObject)

// macro for compile time type casting, with debug run-time assert
#ifdef NIDEBUG
    #define NiVerifyStaticCast(classname, pkObject) \
        ((classname*) classname::VerifyStaticCastDebug( \
            &classname::ms_RTTI, pkObject))
#else
    #define NiVerifyStaticCast(classname, pkObject) ((classname*) (pkObject))
#endif

// macro for run-time type casting, returns NULL if invalid cast
#define NiDynamicCast(classname, pkObject) \
    ((classname*) classname::DynamicCast(&classname::ms_RTTI, pkObject))

#endif
