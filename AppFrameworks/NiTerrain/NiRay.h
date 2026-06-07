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

#ifndef NIRAY_H
#define NIRAY_H

#include <NiPoint3.h>

#include "NiTerrainLibType.h"
#include "NiTerrainDataLeaf.h"

/**
    The NiRay class is a special ray used in the terrain collision system.
    
    Each ray is given an origin in world space, a direction vector and an
    optional length. When used in a collision with the terrain, the details
    of any collision will be stored within and is accessible from the ray
    object itself.
 */
class NITERRAIN_ENTRY NiRay : public NiMemObject 
{
public:

    /// @name Constructors
    //@{

    /// Default constructor
    inline NiRay();

    /**
        Ray constructor, with parameters.
        
        @param kRayOrigin Origin of the ray, in world space
        @param kRayDir Direction vector, optionally normalized
     */
    inline NiRay(const NiPoint3& kRayOrigin, const NiPoint3& kRayDir);
    //@}

    /**
        Given a point, this function will calculate the shortest distance from
            that point to the ray.
        
        @param kPoint World space coordinate to test against the ray
        @return Distance between the given point, and the ray.

        @note This function introduces a slight floating point error margin,
            since it makes use of the faster NiPoint3::VectorLength function.
     */
    inline float DistanceTo(const NiPoint3& kPoint) const;

    /**
        Finds a vector that originates on this ray and passes through the
        given point.
        
        @param kPoint Point in world space to test against the ray
        @param kIntersection Point in world space that lies on this ray, which 
            the vector originates from.
        @param kDirection Direction of the resulting perpendicular vector.
        @return True if a valid point was found

        @note This function introduces a slight floating point error margin,
            since it makes use of the faster NiPoint3::VectorLength function.
     */
    inline bool GetPerpendicularPassingThrough(const NiPoint3& kPoint, 
        NiPoint3& kIntersection, NiPoint3& kDirection) const;

    /**
        Tests whether or not this ray passes through the given bounding sphere
        
        @param kBound Bounding sphere to test against this ray.
        @return True if the ray and the sphere intersect at any point
     */
    inline bool PassesThroughBound(const NiBound& kBound) const;

    /**
        Given a triangle, this function will pitch the ray against it to check 
        for a collision.
            
        Triangle facing is taken into account; the ray will only collide with
        the upward face of the triangle.
        
        @param k0 Corner 0 of triangle
        @param k1 Corner 1 of triangle
        @param k2 Corner 2 of triangle
        @param kCollisionPoint Point of collision between the triangle and ray
        @return True if this ray collides with the triangle
     */
    inline bool TryTriangle(const NiPoint3&k0, const NiPoint3&k1, const NiPoint3&k2, 
        NiPoint3& kCollisionPoint);
            
    /**
        Retrieve the set length of the ray.
        
        A length of 0 indicates an infinite length. WARNING: Involves a Sqrt
        operation.

        @return The length of the ray. 
     */
    inline float GetLength() const;

    /**
        Re-initializes the ray with the given parameters.

        @param kRayOrigin Origin of the ray, in world space.
        @param kRayDir Direction vector, optionally normalized.
        @param fRayLength Maximum length of the ray, 0 = infinite length.
     */
    inline void SetRay(const NiPoint3& kRayOrigin, const NiPoint3& kRayDir, 
        float fRayLength);

    /**
        Get the origin and length of the ray packed into a transform.
        
        The origin is placed in the translation component of the transform,
        and the length of the ray is in the transform's scale field.
    */
    NiTransform GetTransform() const;

    /**
        Set the origin of the ray and scale the length based on a transform.
        
        The transform's translation is used as the new ray origin, the
        ray direction is unchanged and the ray's length is scaled by the
        transform's scale.
     */
    inline void SetTransform(NiTransform kTransform);

    /// Get the direction vector (normalized) of the ray.
    inline const NiPoint3& GetDirection() const;

    /// Get the origin (world space) of the ray.
    inline const NiPoint3& GetOrigin() const;

    /// @cond EMERGENT_INTERNAL

    /**
        Get and set the quad leaf on which the last collision occured

        @note Intended for Emergent internal usage only.
        @internal
     */
    //@{
    inline const NiTerrainDataLeaf* GetCollidedLeaf();
    inline void SetCollidedLeaf(const NiTerrainDataLeaf* pLeaf);
    //@}

    /// @endcond   
     
    /**
        Distance from the most recent intersection that this ray encountered to
        the ray origin.
        
        @return Distance to the most recent intersection
    */
    float GetDistanceToIntersection() const;

    /**
        Resets any intersection and collision data this ray may have collected.
        
        Should be called before any new collision test is performed.
    */
    inline void ResetIntersection();

    /**
        Retrieves details about the last intersection that this ray encountered
        
        @param kIntersectionPoint location of intersection in world space
        @param kIntersectionNormal Normal of the triangle the ray intersected
            with
        @return True if this ray has intersected with anything
    */
    inline bool GetIntersection(NiPoint3& kIntersectionPoint, 
        NiPoint3& kIntersectionNormal) const;

    /// @cond EMERGENT_INTERNAL

    /**
        Sets the intersection data of a ray. Will only set the intersection
            points if the collision point is within the length of the ray.
        
        @param kIntersectionPoint Intersection location in world space
        @param kIntersectionNormal Normal of the triangle the ray intersected
            with
        @param fDistanceSqrToIntersection (optional) Squared distance from ray 
            origin to intersection point. If excluded, it is calculated within
            the function
        @return True if the collision point is within the length of the ray

        @note Intended for Emergent internal usage only.
        @internal
     */
    inline bool SetIntersection(const NiPoint3& kIntersectionPoint, 
        const NiPoint3& kIntersectionNormal, 
        float fDistanceSqrToIntersection = 0.0f);

    /**
        Overwrites the intersection data of a ray, if the given point is closer
            to the ray origin, that the currently stored point. Will only set 
            the intersection points if the collision point is within the length
            of the ray.
        
        @param kIntersectionPoint Intersection location in world space
        @param kIntersectionNormal Normal of the triangle the ray intersected
            with
        @return True if the stored collision point was overwritten by the point
            defined by the function parameters

        @note Intended for Emergent internal usage only.
        @internal
     */
    inline bool UseClosestIntersection(const NiPoint3& kIntersectionPoint, 
        const NiPoint3& kIntersectionNormal);

    /// @endcond

private:

    NiPoint3 m_kRayOrigin;
    NiPoint3 m_kRayDir;
    float m_fRayLengthSqr;
    float m_fDistanceSqrToIntersection;
    const NiTerrainDataLeaf* m_pkCollidedLeaf;
    NiPoint3 m_kIntersectPoint;
    NiPoint3 m_kIntersectNormal;
    bool m_bIntersected;
};

#include "NiRay.inl"

#endif // NIRAY_H
