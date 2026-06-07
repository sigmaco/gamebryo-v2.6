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

#ifndef NITQUICKRELEASEPOOL_H
#define NITQUICKRELEASEPOOL_H

/**
    This is an abstract class that serves as object pool that can be quickly
    reset. This class allows a group of objects to be gathered, used, release,
    and reused without having to continually perform memory allocations and
    de-allocations.
 */
template <class T, class TAlloc> class NiTQuickReleasePool : 
    public NiMemObject
{
public:
    
    /**
        Constructor
        @param uiSize The initial size of the pool. Size is measured in
        number of objects.
        @param fGrowByPercent Percentage of the current size to grow the pool 
        by when there are no free objects left in the pool.
    */
    NiTQuickReleasePool(NiUInt32 uiSize = 1, float fGrowByPercent = 0.1f);
    
    /// Destructor
    ~NiTQuickReleasePool();
    
    /// Sets the active size of the object pool. Size is measured in number of 
    /// objects.
    void SetSize(NiUInt32 uiSize);

    /// Returns the active size of the object pool. Size is measured in number 
    /// of objects.
    void GetSize(NiUInt32& uiSize) const;

    /// Returns the maximum size of the object pool.
    inline void GetMaxSize(NiUInt32& uiSize) const;

    /// Sets the percentage to grow the pool by when there are no free objects
    /// left in the pool.
    inline void SetGrowByPercent(float fGrowByPercent);

    /// Returns the percentage to grow the pool by when there are no free 
    /// objects left in the pool.
    inline void GetGrowByPercent(float& fGrowByPercent) const;

    /** @name Pool data access functions */
    //@{

    /// Resets the active pool size to zero.
    inline void Reset();

    /// Reserves a location in the pool. The index to the reserved location 
    /// is returned and can be used with SetValue & GetValue
    inline NiUInt32 GetNew();

    /// Sets an object to the specified index in the pool. Only valid indicies 
    /// provided by GetNew should be used.
    inline void SetValue(NiUInt32 uiIndex, T& value);
    
    /// Returns an object from the specified index in the pool. Only valid 
    /// indicies provided by GetNew should be used.
    inline void GetValue(NiUInt32 uiIndex, T& value) const;

    /// Returns a pointer to an object at the specified index. Only valid 
    /// indicies provided by GetNew should be used.
    inline void GetPointerTo(NiUInt32 uiIndex, T*& pValue) const;

    //@}

    //@{
    /// Reference management functions
    inline void IncRefCount();
    inline void DecRefCount();
    inline NiUInt32 GetRefCount() const;
    //@}

protected:

    void Copy(const NiTQuickReleasePool<T, TAlloc>* pkOther);

    NiUInt32 m_uiMaxSize;
    NiUInt32 m_uiCurrentSize;

    /// Determines how fast the pool grows.
    float m_fGrowByPercent;

    /// Data pool
    T* m_pPool;

    /// Reference counter
    NiUInt32 m_uiRefCount;
};


/** 
    Instansiable version of the NiTQuickReleasePool that implements objects via
    NiTNewInterface.
 */
template <class T> class NiTQuickReleaseObjectPool : public 
    NiTQuickReleasePool<T, NiTNewInterface<T> >
{
    public:
    
        NiTQuickReleaseObjectPool(NiUInt32 uiMaxSize = 1, 
            float fGrowByPercent = 0.1f);

        /// @cond EMERGENT_INTERNAL
        //@{
        void Copy(const NiTQuickReleaseObjectPool<T>* pkOther);
        //@}
        /// @endcond

};

/** 
    Instansiable version of the NiTQuickReleasePool that implements primitives
    via NiTMallocInterface.
 */
template <class T> class NiTQuickReleasePrimitivePool : public
    NiTQuickReleasePool<T, NiTMallocInterface<T> >
{
    public:
    
        NiTQuickReleasePrimitivePool(NiUInt32 uiMaxSize = 1, 
            float fGrowbyPercent = 0.1f);

        /// @cond EMERGENT_INTERNAL
        //@{
        void Copy(const NiTQuickReleasePrimitivePool<T>* pkOther);
        //@}
        /// @endcond
    
};

#include "NiTQuickReleasePool.inl"

#endif // NITQUICKRELEASEPOOL_H
