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

#ifndef NIBITFIELD_H
#define NIBITFIELD_H

#include "NiString.h"

//---------------------------------------------------------------------------
#define NIBITFIELD_TYPE static const unsigned int NI_UNUSED
//---------------------------------------------------------------------------

template<class TYPE> 
inline void NiTSetField(TYPE& uFlags, TYPE uValue, TYPE uMask, TYPE uPos)
{
    uFlags = (uFlags & ~uMask) | (uValue << uPos);
}

template<class TYPE> 
inline TYPE NiTGetField(TYPE uFlags, TYPE uMask, TYPE uPos)
{
    return (uFlags & uMask) >> uPos; 
}

template<class TYPE> 
inline void NiTSetBit(TYPE& uFlags, bool bValue, TYPE uMask)
{
    if (bValue)
    {
        uFlags |= uMask;
    }
    else
    {
        uFlags &= ~uMask;
    }
}

template<class TYPE> 
inline bool NiTGetBit(TYPE uFlags, TYPE uMask)
{
    return (uFlags & uMask) != 0;
}

template<class TYPE> 
inline TYPE NiTGetBitMask(TYPE uPos, TYPE uSize)
{
    TYPE uAllOnes = (TYPE) 0xffffffff;
    return (uAllOnes << uPos) ^ (uAllOnes << (uPos + uSize));
}

#define NiBeginBitfieldEnum() \
    NIBITFIELD_TYPE BITFIELDALLONES = 0xffffffff; \
    NIBITFIELD_TYPE BITFIELDSTART_POS = 0; \
    NIBITFIELD_TYPE BITFIELDSTART_SIZE = 0; \

#define NiBeginDefaultBitfieldEnum() \
    NiBeginBitfieldEnum() \
    inline void ToStringBITFIELDSTART(NiString&, \
        bool) const  {} \

#define NiEndBitfieldEnum() 

#define NiEndDefaultBitfieldEnum() 

#define NiDeclareIndexedBitfieldEntryFunctions(name, size, previous, \
    data_member, type, printfSymbol) \
    inline type Get##name() const \
    { \
        return NiTGetField< type >(data_member[name##_INDEX], \
            name##_MASK, name##_POS); \
    } \
    inline void Set##name(type value)  \
    { \
         NiTSetField< type >(data_member[name##_INDEX], value, name##_MASK, \
            name##_POS); \
    } \
    inline void ToString##name(NiString& kString, \
        bool bRecurse) const  \
    { \
        if (bRecurse) \
        { \
            ToString##previous(kString, bRecurse); \
        } \
        type tValue = Get##name(); \
        const char* pcName = #name; \
        bool bAppend = kString.Length() != 0; \
        if (bAppend) \
            kString += "\n"; \
        \
        kString += pcName; \
        kString += " = "; \
        char acBuffer[256]; \
        NiSprintf(acBuffer, 256, #printfSymbol, tValue); \
        kString += acBuffer; \
    } 

#define NiDeclareBitfieldEntryFunctions(name, size, previous, \
    data_member, type, printfSymbol) \
    inline type Get##name() const \
    { \
        return NiTGetField< type >(data_member, \
            name##_MASK, name##_POS); \
    } \
    inline void Set##name(type value)  \
    { \
         NiTSetField< type >(data_member, value, name##_MASK, \
            name##_POS); \
    } \
    inline void ToString##name(NiString& kString, \
        bool bRecurse) const  \
    { \
        if (bRecurse) \
        { \
            ToString##previous(kString, bRecurse); \
        } \
        type tValue = Get##name(); \
        const char* pcName = #name; \
        bool bAppend = kString.Length() != 0; \
        if (bAppend) \
            kString += "\n"; \
        \
        kString += pcName; \
        kString += " = "; \
        char acBuffer[256]; \
        NiSprintf(acBuffer, 256, #printfSymbol, tValue); \
        kString += acBuffer; \
    } 

#define NiDeclareDefaultIndexedBitfieldEntryFunctions(name, size, previous) \
    NiDeclareIndexedBitfieldEntryFunctions(name, size, previous,  \
        m_auiBitArray, unsigned int, %d)

#define NiDeclareBitfieldEntry(name, size, previous) \
    NIBITFIELD_TYPE name##_POS = \
        previous##_POS + previous##_SIZE; \
    NIBITFIELD_TYPE name##_SIZE = size; \
    NIBITFIELD_TYPE name##_MASK = \
        (BITFIELDALLONES << name##_POS) ^ \
        (BITFIELDALLONES << (name##_POS + name##_SIZE));

#define NiDeclareLastBitfieldEntry(name, size, previous) \
    NIBITFIELD_TYPE name##_POS = \
        previous##_POS + previous##_SIZE; \
    NIBITFIELD_TYPE name##_SIZE = size; \
    NIBITFIELD_TYPE name##_MASK = \
        (BITFIELDALLONES << name##_POS);

#define NiDeclareIndexedBitfieldEntry(name, size, previous, index) \
    NiDeclareBitfieldEntry(name, size, previous) \
    NIBITFIELD_TYPE name##_INDEX = index;

#define NiDeclareLastIndexedBitfieldEntry(name, size, previous, index) \
    NiDeclareLastBitfieldEntry(name, size, previous) \
    NIBITFIELD_TYPE name##_INDEX = index;

#define NiDeclareDefaultIndexedBitfieldEntry(name, size, previous, index) \
    NiDeclareBitfieldEntry(name, size, previous) \
    NIBITFIELD_TYPE name##_INDEX = index; \
    NiDeclareDefaultIndexedBitfieldEntryFunctions(name, size, previous)

#define NiDeclareDefaultLastIndexedBitfieldEntry(name, size, previous, index) \
    NiDeclareLastBitfieldEntry(name, size, previous) \
    NIBITFIELD_TYPE name##_INDEX = index; \
    NiDeclareDefaultIndexedBitfieldEntryFunctions(name, size, previous)

#define NiBitfieldGetField(name, type, data_member) \
    NiTGetField< type >(data_member, name##_MASK, name##_POS)

#define NiBitfieldSetField(name, value, type, data_member) \
    NiTSetField< type >(data_member, value, name##_MASK, name##_POS)

#define NiBitfieldGetIndexedField(name, type, data_member) \
    NiTGetField< type >(data_member[name##_INDEX], name##_MASK, \
    name##_POS)

#define NiBitfieldSetIndexedField(name, value, type, data_member) \
    NiTSetField< type >(data_member[name##_INDEX], value, name##_MASK, \
    name##_POS)

#endif // NIBITFIELD_H
