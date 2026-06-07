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

#ifndef NIDEFORMABLEPOINTSET_H
#define NIDEFORMABLEPOINTSET_H

#include <NiRefObject.h>

#include "NiTerrainLibType.h"
#include "NiDeformablePoint.h"
#include "NiTQuickReleasePool.h"

/** 
    This is a special data structure used to store deformable points for the
    deformation aspect of the terrain engine. 
    
    It has been designed so that the contents of the structure can be removed
    very quickly, as well as storing extra internal information about how the 
    contents of the deformable points were allocated.
 */
class NITERRAIN_ENTRY NiDeformablePointSet : public NiRefObject
{
public:
    
    /// Default constructor.
    inline NiDeformablePointSet(NiDeformablePoint::POINT_TYPE eType);
   
    /// Destructor
    inline virtual ~NiDeformablePointSet();

    inline NiDeformablePointSet* Clone();

    /**
        Methods that allow addition and retrieval of deformable points from
        this set.
    */
    /// @name Deformable Point Access
    //@{
    /// Retrieve the deformable point object at the given index.
    ///
    /// Returns 0 on failure.
    inline NiDeformablePoint* GetAt(NiUInt32 uiIndex) const;

    /// Get the number of deformable points that have been added to the set.
    inline NiUInt32 GetNumPoints() const;

    /// Set the type of point this set is intended to contain.
    inline void SetPointType(NiDeformablePoint::POINT_TYPE eType);

    /// Get the type of point this set is intended to contain.
    inline NiDeformablePoint::POINT_TYPE GetPointType() const;

    /// Retrieve the status of the set.
    ///
    /// A set is valid if it contains at least one deformable point.
    inline bool IsDataValid() const;

    /** 
        Create and add a deformable point to the set.

        @param uiIndex Terrain data object index.
        @param fValue Initial value of the deformable point.
        @param kWorldLocation The world location of the deformable point. For 
            reference only, this value cannot be modified by the deformation 
            operation.
        @param fRadius The radius of this point.
        @param fSlope The slope angle for this point
        @param eType The type of point being added.
        @param bActive Whether or not the point is initially active.
     */
    inline void AddPoint(NiUInt32 uiIndex, float fValue, 
        const NiPoint3& kWorldLocation, float fRadius, float fSlope, 
        NiDeformablePoint::POINT_TYPE eType, bool bActive = true);

    /** 
        How quickly the internal array of deformable points will grow as new 
        points are added.
        
        @param fGrowBy A percentage value representing how much of the original
            size will be added upon expansion. Valid values lie between 0.0f 
            and 1.0f.
     */
    inline void SetGrowbyPercent(float fGrowBy);

    /**
        Grows the internal array of deformable point objects to the specified
        size.
        
        If a value lower than the current size is given, no action is
        performed.
     */
    inline void GrowTo(NiUInt32 uiSize);
    //@}
    
    /// @cond EMERGENT_INTERNAL

    /** 
        An ID given to this set that is used by the terrain engine for 
        management of the 'original data' within the Deformable Points.
     */
    //@{
    inline NiMemObject* SetOriginalPointPool(NiMemObject* pkOriginalPointPool);
    inline NiMemObject* GetOriginalPointPool() const;
    //@}
    
    /// Sets the effective size of the deformable set and marks it as having no
    /// valid data.
    inline void InvalidateData();

    /// Attempt to mark the set as having valid data. Will only succeed if the
    /// set contains at least one element.
    inline void ValidateData();

    /// @endcond

private:

    /// Internal array of deformable points
    NiDeformablePoint* m_pkPoints;

    /**
        A pointer to an object that holds references to all the original data
        objects pointed to by the contained deformable points. This is used
        externally to this class, for memory management and cleanup.
    */
    NiMemObject* m_pkOriginalPointPool;

    /// The allocated size of the internal array
    NiUInt32 m_uiMaxSize;

    /// The current location in the array of deformable points, gets set to 0
    /// when the set is invalidated
    NiUInt32 m_uiCurrentSize;

    /// How quickly the internal array of deformable points will grow when 
    /// it gets full. Value between 0.0f and 1.0f
    float m_fGrowByPercent;

    /// What type of points this set contains
    NiDeformablePoint::POINT_TYPE m_eType;

    /// State of the internal array; true if it contains current and valid
    /// data.
    bool m_bDataIsValid;
};

NiSmartPointer(NiDeformablePointSet);
#include "NiDeformablePointSet.inl"

#endif // NIDEFORMABLEPOINTSET_H
