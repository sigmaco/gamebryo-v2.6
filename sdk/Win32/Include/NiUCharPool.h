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

#ifndef NIUCHARPOOL
#define NIUCHARPOOL

#include <NiMemManager.h>

class NiUCharPool : public NiMemObject
{
public:

    /**
        Default parameterized constructor

        @param uiSize The initial number of entries to allocate in the pool
        @param uiGrowby Number of entries to increase pool size by when it 
            becomes full
     */
    NiUCharPool(NiUInt32 uiSize = 1, NiUInt32 uiGrowby = 32);

    /// Destructor
    ~NiUCharPool();

    /// Set the size of the pool. It is valid to either increase or decrease
    /// the size in this way.
    inline void SetSize(NiUInt32 uiSize);

    /// The current size of the pool
    inline NiUInt32 GetSize() const;

    /// Number of entries to increase pool size by when it becomes full
    //@{
    inline void SetGrowBy(NiUInt32 uiGrowBy);
    inline NiUInt32 GetGrowBy() const;
    //@}

    /// Sets all the pool entries values to 0
    inline void Reset();

    /// Returns the index of the first available pool position. If the pool is
    /// full, it will grow by the current growBy amount
    inline NiUInt32 GetNew();

    /// Release the last allocated value.
    void ReleaseValue();

    /// Set the value of a pool entry at the given position
    inline void SetValue(NiUInt32 uiIndex, NiUInt8 ucValue);

    /// Set the value of a pool entry at the given position by the result of a
    /// Bitwise OR of the given value and the current entry value
    inline void CombineValue(NiUInt32 uiIndex, NiUInt8 ucValue);

    /// Get the value of the entry at the given position
    inline NiUInt8 GetValue(NiUInt32 uiIndex) const;

private:

    NiUInt32 m_uiMaxSize;
    NiUInt32 m_uiCurrentSize;
    NiUInt32 m_uiGrowBy;
    NiUInt8* m_pucPool;
};

#include "NiUCharPool.inl"

#endif // NIUCHARPOOL
