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

#ifndef NITSTRINGMAP_H
#define NITSTRINGMAP_H

#include "NiTMap.h"
#include "NiTPointerMap.h"

// The hash function class
class NiStringHashFunctor
{
public:
    static unsigned int KeyToHashIndex(const char* pcKey,
        unsigned int uiTableSize);
};

// The equality function class
class NiStringEqualsFunctor
{
public:
    static bool IsKeysEqual(const char* pcKey1, const char* pcKey2);
};

template <class TPARENT, class TVAL,
    class THASH = NiStringHashFunctor,
    class TEQUALS = NiStringEqualsFunctor > class NiTStringTemplateMap :
    public TPARENT
{
public:
    // construction and destruction
    NiTStringTemplateMap(unsigned int uiHashSize = 37, bool bCopy = true);
    virtual ~NiTStringTemplateMap();

protected:
    virtual void SetValue(NiTMapItem<const char*, TVAL>* pkItem,
        const char* pcKey, TVAL val);
    virtual void ClearValue(NiTMapItem<const char*, TVAL>* pkItem);

    bool m_bCopy;
};

template <class TVAL,
    class THASH = NiStringHashFunctor,
    class TEQUALS = NiStringEqualsFunctor > class NiTStringMap :
    public NiTStringTemplateMap<NiTMap<const char*, TVAL, THASH, TEQUALS>,
    TVAL>
{
public:
    NiTStringMap(unsigned int uiHashSize = 37, bool bCopy = true);
};

template <class TVAL,
    class THASH = NiStringHashFunctor,
    class TEQUALS = NiStringEqualsFunctor > class NiTStringPointerMap :
    public NiTStringTemplateMap
    <NiTPointerMap<const char*, TVAL, THASH, TEQUALS>, TVAL>
{
public:
    NiTStringPointerMap(unsigned int uiHashSize = 37, bool bCopy = true);
};

#include "NiTStringMap.inl"


#endif // NITSTRINGMAP_H
