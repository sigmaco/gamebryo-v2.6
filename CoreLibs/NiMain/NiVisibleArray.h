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

#ifndef NIVISIBLEARRAY_H
#define NIVISIBLEARRAY_H

#include "NiTArray.h"

class NiRenderObject;

class NIMAIN_ENTRY NiVisibleArray : public NiMemObject
{
public:
    NiVisibleArray();
    NiVisibleArray(unsigned int uiMaxSize, unsigned int uiGrowBy);
    ~NiVisibleArray();

    // adds to the end of the array, incrementing count by one
    inline void Add(NiRenderObject& kObj);

    // Fast - does not clear each element
    inline void RemoveAll();

    // uiIndex < GetCount()
    inline void SetAt(unsigned int uiIndex, NiRenderObject& kObj);

    // Ensures that the array stays packed, but does change the order
    // (fills empty slot with last item in array - DOES NOT slide all
    // other items over)
    inline NiRenderObject& RemoveAtAndFill(unsigned int uiIndex);

    // uiIndex < GetCount()
    inline NiRenderObject& GetAt(unsigned int uiIndex);
    inline NiRenderObject& GetAt(unsigned int uiIndex) const;

    inline unsigned int GetCount() const;

    inline unsigned int GetAllocatedSize() const;
    void SetAllocatedSize(unsigned int uiSize);

    inline unsigned int GetGrowBy() const;

    // Grow by _must_ be positive.  Passing uiGrowBy of 0 will cause the 
    // grow by member to be set to 1
    inline void SetGrowBy(unsigned int uiGrowBy);

protected:

    enum { DEF_MAX_SIZE = 1024 };
    enum { DEF_GROW_BY = 1024 };

    NiRenderObject** m_ppkArray;
    unsigned int m_uiCurrentSize;
    unsigned int m_uiAllocatedSize;

    unsigned int m_uiGrowBy;
};

#include "NiVisibleArray.inl"

#endif
