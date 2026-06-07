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

#ifndef NIDEFORMABLEPOINT_H
#define NIDEFORMABLEPOINT_H

#include <NiMemObject.h>

#include "NiTerrainLibType.h"
#include "NiTerrainVertex.h"
#include "NiSurfaceMaskPixel.h"

/** 
    An NiDeformablePoint object represents an arbitrary modifiable value
    of the terrain such as a height value, a texture mask pixel etc. 
    
    This class does not care what the value actually represents; it acts 
    as a common data type between the terrain engine and deformation 
    operation.
 */
class NITERRAIN_ENTRY NiDeformablePoint : public NiMemObject
{
public:

    /**
        Enumerates the different logical types of data represented by the 
        deformable point class.
    */
    enum POINT_TYPE 
    {
        /// Vertices are being deformed.
        VERTEX = 0,

        /// Terrain material layer mask pixels are being deformed.
        MASK_PIXEL = 1,

        /// Placeholder for maximum type.
        MAX_TYPE = 2,
    };

    /// Default constructor.
    inline NiDeformablePoint();
    
    /// @cond EMERGENT_INTERNAL
    
    inline void SetOriginalIndex(NiUInt32 uiIndex, POINT_TYPE eType);
    inline NiUInt32 GetOriginalIndex() const;
    
    /// @endcond

    /// Gets the value of the deformable point.
    inline float GetValue() const;

    /// Sets the value of the deformable point.
    inline void SetValue(float fValue);
    
    /**
        Gets the original location of this point in world space.
    
        @note Depending on the type of deformable point, this operation may 
            not be applicable.
    */
    inline const NiPoint3& GetWorldLocation() const;

    /**
        Sets the original location of this point in world space.
    
        @note Depending on the type of deformable point, this operation may 
            not be applicable.
    */
    inline void SetWorldLocation(const NiPoint3& kWorldLoc);

    /**
        Get and set the slope angle value
    */
    //@{
    inline float GetSlopeAngle() const;
    inline void SetSlopeAngle(float fSlope);
    //@}

    /**
        Gets radius of this vertex.
    */
    inline float GetRadius() const;

    /**
        Sets the radius of this vertex.
    */
    inline void SetRadius(const float fRadius);

    /// Gets the type of the deformable point.
    inline POINT_TYPE GetType() const;

    /// Query as to whether or not the point has changed. 
    inline bool GetHasChanged() const;

    /**
        Sets whether or not the point has changed.
    
        The changed state of the point is application specific and not
        managed automatically.
    */
    inline void SetHasChanged(bool bVal); 

    /// Query as to whether or not this point is active.
    inline bool IsActive() const;

    /**
        Sets whether or not the point is active.
        
        The active state of the point is application specific and not
        managed automatically.
    */
    inline void SetActive(bool bVal);

private:

    NiPoint3 m_kWorldLocation;
    float m_fRadius;
    float m_fValue;
    float m_fSlopeAngle;
    NiUInt32 m_uiOriginalIndex;
    POINT_TYPE m_eType;
    bool m_bHasChanged;
    bool m_bActive;
};

#include "NiDeformablePoint.inl"

#endif // NIDEFORMABLEPOINT_H
