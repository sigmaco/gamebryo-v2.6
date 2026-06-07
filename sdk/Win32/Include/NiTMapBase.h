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

#ifndef NITMAPBASE_H
#define NITMAPBASE_H

#include <NiRTLib.h>
#include <NiMemObject.h>
#include <NiUniversalTypes.h>

// The map class implements a hash table of TKEY to store values of TVAL.
// It uses modular arithmetic for building the hash keys with a default
// table size of 37.  If you want a larger table size, the best bet is to
// us a large prime number.  Consult a standard text on hashing for the
// basic theory.
//
// TKEY MUST BE THE SAME SIZE AS A POINTER!
//
// The template class assumes that type TKEY has the following:
//   1.  Default constructor, TKEY::TKEY();
//   2.  Copy constructor, TKEY::TKEY(const TKEY&);
//   3.  Assignment, TKEY& operator=(const TKEY&);
//   4.  Comparison, bool TKEY::operator==(const TKEY&), or supply a
//       specialized equality testing class in your template.
//   5.  Implicit conversion, TKEY::operator long(), for building hash key,
//       or you must pass in your own hash function class in your template.
//
// The template class assumes that type TVAL has the following:
//   1.  Default constructor, TVAL::TVAL();
//   2.  Copy constructor, TVAL::TVAL(const TVAL&);
//   3.  Assignment, TVAL& operator=(const TVAL&);
//
// In both cases, the compiler-generated default constructor, copy
// constructor, and assignment operator are acceptable.
//
// Example of iteration over map
//
//     NiTMap<TKEY,TVAL> kMap;
//     NiTMapIterator pos = kMap.GetFirstPos();
//     while (pos)
//     {
//         TKEY key;
//         TVAL val;
//         kMap.GetNext(pos,key,val);
//         <process key and val here>;
//     }

typedef void* NiTMapIterator;

// The NiTMapItem class used to be nested in NiTMap. It was made a separate
// class because having it nested caused g++ to hit an internal error.

template <class TKEY, class TVAL> class NiTMapItem : public NiMemObject
{
public:
    NiTMapItem* m_pkNext;
    TKEY m_key;
    TVAL m_val;
};

// The hash function class
template <class TKEY> class NiTMapHashFunctor : public NiMemObject
{
public:
    static unsigned int KeyToHashIndex(TKEY key, NiUInt32 uiTableSize);
};

// The equality function class
template <class TKEY> class NiTMapEqualsFunctor : public NiMemObject
{
public:
    static bool IsKeysEqual(TKEY key1, TKEY key2);
};

template <class TheAllocator, class TKEY, class TVAL,
    class THASH = NiTMapHashFunctor<TKEY>,
    class TEQUALS = NiTMapEqualsFunctor<TKEY> > class NiTMapBase :
    public NiMemObject
{
public:
    // construction and destruction
    NiTMapBase(NiUInt32 uiHashSize = 37);
    virtual ~NiTMapBase();

    // counting elements in map
    inline NiUInt32 GetCount() const;
    inline bool IsEmpty() const;

    // add or remove elements
    inline void SetAt(TKEY key, TVAL val);
    inline bool RemoveAt(TKEY key);
    inline void RemoveAll();

    // element access
    inline bool GetAt(TKEY key, TVAL& val) const;

    // map traversal
    inline NiTMapIterator GetFirstPos() const;
    inline void GetNext(NiTMapIterator& pos, TKEY& key, TVAL& val) const;
    
    /**
        Resize the map.
        
        The hash table of the map is resized to the given size (which must be
        non-zero), and all of the entries in the map are re-mapped to new
        locations. Any interators will be invalid after this operation. This
        operation is expensive for large maps - the cost is at least linear
        in the number of entries.
    */
    inline void Resize(NiUInt32 uiNewHashSize);

protected:
    // hash table stored as array of doubly-linked lists
    NiTMapItem<TKEY,TVAL>* GetPtrToAssoc(TKEY key) const;
    virtual void SetValue(NiTMapItem<TKEY,TVAL>* pkItem, TKEY key, TVAL val);
    virtual void ClearValue(NiTMapItem<TKEY,TVAL>* pkItem);
    
    virtual NiTMapItem<TKEY, TVAL>* NewItem() = 0;
    virtual void DeleteItem(NiTMapItem<TKEY, TVAL>* pkItem) = 0;
    
    NiUInt32 m_uiHashSize;             // maximum slots in hash table
    NiTMapItem<TKEY,TVAL>** m_ppkHashTable;// hash table storage

    struct AntiBloatAllocator : public TheAllocator
    {
        // We reduce TheAllocator by 4 bytes by deriving
        // See http://www.cantrip.org/emptyopt.html
        NiUInt32 m_uiCount;      // number of elements in list
    };

    AntiBloatAllocator m_kAllocator;  

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTMapBase(const NiTMapBase&);
    NiTMapBase& operator=(const NiTMapBase&);
};

//---------------------------------------------------------------------------
// Inline include
#include "NiTMapBase.inl"
//---------------------------------------------------------------------------

#endif // NITMAPBASE_H
