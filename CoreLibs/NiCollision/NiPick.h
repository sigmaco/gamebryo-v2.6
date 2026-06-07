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

#ifndef NIPICK_H
#define NIPICK_H

#include "NiCollisionLibType.h"
#include <NiColor.h>
#include <NiNode.h>
#include <NiPoint3.h>
#include <NiPoint2.h>
#include <NiTArray.h>

class NiAVObject;
class NiRenderObject;

class NICOLLISION_ENTRY NiPick : public NiMemObject
{
public:
    // construction and destruction
    NiPick (unsigned int uiResultsArraySize = 0, 
        unsigned int uiResultsArrayGrowBy = 8);
    ~NiPick ();

    // sort type (default = SORT_ON)
    // used only if QueryType is QUERY_ALL
    typedef enum { SORT_ON, SORT_OFF } SortType;
    inline void SetSortType (SortType eType);
    SortType GetSortType () const;

    // intersect type (default = INTERSECT_COLLISION_DATA)
    typedef enum { INTERSECT_BOUND, INTERSECT_TRIANGLE,
        INTERSECT_COLLISION_DATA } IntersectType;
    inline void SetIntersectType (IntersectType eType);
    IntersectType GetIntersectType () const;

    // query type (default = QUERY_ALL)
    typedef enum { QUERY_FIRST, QUERY_CLOSEST, QUERY_ALL } QueryType;
    inline void SetQueryType (QueryType eType);
    QueryType GetQueryType () const;

    // fallback method (default = FALLBACK_TRIANGLE)
    // Only used if IntersectType is INTERSECT_COLLISION_DATA. 
    typedef enum { FALLBACK_BOUND, FALLBACK_TRIANGLE,
        FALLBACK_FALSE } FallbackType;
    inline void SetFallbackType (FallbackType eType);
    FallbackType GetFallbackType () const;

    // intersection point space (default = COORDINATES_WORLD)
    typedef enum { COORDINATES_MODEL, COORDINATES_WORLD } CoordinateType;
    inline void SetCoordinateType (CoordinateType eType);
    CoordinateType GetCoordinateType () const;

    // set/get doublesided mode (default = true)
    inline void SetFrontOnly (bool bFrontOnly);
    inline bool GetFrontOnly () const;

    // stop the pick if 'app culled' flag is ste
    inline void SetObserveAppCullFlag (bool bObserveAppCullFlag);
    inline bool GetObserveAppCullFlag () const;

    // for application to add/remove objects
    inline void SetTarget (NiAVObject* pObject);
    inline void RemoveTarget ();

    // attribute return types (default = false for all)
    inline void SetReturnTexture (bool bReturnTexture);
    inline bool GetReturnTexture () const;
    inline void SetReturnNormal (bool bReturnNormal);
    inline bool GetReturnNormal () const;
    inline void SetReturnSmoothNormal (bool bReturnSmoothNormal);
    inline bool GetReturnSmoothNormal () const;
    inline void SetReturnColor (bool bReturnColor);
    inline bool GetReturnColor () const;

    // pick operation
    bool PickObjects (const NiPoint3 &origin, const NiPoint3 &dir,
        bool bAppend = false);

    // Logically removes results, but does not deallocate records
    void ClearResultsArray ();

    // The policy class controls what happens when any NiRender object is 
    // tested in the picking process and has already passed any bounds tests.
    // It will only be invoked when the triangle testing path is executed.
    // Applications can customize this to pick against ABV's, special modifier
    // logic, or any approximation that fits their needs for performance and 
    // pick result quality.
    class NICOLLISION_ENTRY PickObjectPolicy : public NiRefObject
    {
    public:
        virtual bool FindIntersections(
            const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
            NiRenderObject* pkRenderObj);
    };

    typedef NiPointer<PickObjectPolicy> PickObjectPolicyPtr;

    // Get/Set the policy used when trying to pick on render objects directly.
    // Note that the NiPick constructor defaults to the static default pick  
    // object policy. A NULL policy is never valid, and will result in using  
    // the static default policy.
    PickObjectPolicy* GetPickObjectPolicy() const;
    inline void SetPickObjectPolicy(PickObjectPolicy* pkPolicy);

    // Get/Set the default policy used when instantiating NiPick objects.
    // Note that NULL is never a valid value for the static policy, and will
    // result in an instantiation of the default policy class.
    static PickObjectPolicy* GetDefaultPickObjectPolicy();
    static void SetDefaultPickObjectPolicy(PickObjectPolicy* pkPolicy);

    class NICOLLISION_ENTRY Record : public NiMemObject
    {
    public:
        Record (NiAVObject* pObject);

        inline NiAVObject* GetAVObject ();

        inline NiNode* GetParent ();

        inline void SetIntersection (const NiPoint3& intersect);
        inline const NiPoint3& GetIntersection () const;

        inline void SetDistance (float fDistance);
        inline float GetDistance () const;

        inline void SetTriangleIndex (unsigned int uiIndex);
        inline unsigned int GetTriangleIndex () const;

        inline void SetVertexIndices (unsigned int uiIndex0,
            unsigned int uiIndex1, unsigned int uiIndex2);
        inline void GetVertexIndices (unsigned int& uiIndex0,
            unsigned int& uiIndex1, unsigned int& uiIndex2) const;

        inline unsigned int GetSubmeshIndex() const;
        inline void SetSubmeshIndex(unsigned int uiSubmeshIdx);
        
        inline bool GetIndicesValid() const;
        inline void SetInvalidIndices();

        inline void SetTexture (const NiPoint2& texture);
        inline const NiPoint2& GetTexture () const;

        inline void SetNormal (const NiPoint3& normal);
        inline const NiPoint3& GetNormal () const;

        inline void SetColor (const NiColorA& color);
        inline const NiColorA& GetColor () const;

        inline void SetRayInsideABV (const bool bRayInsideABV);
        inline const bool GetRayInsideABV() const;

        // *** begin Emergent internal use only ***

        // NiAlphaSorter uses this function
        inline void SetProxyParent (NiNode* pProxyParent);
        // NiPick::Results uses this function in lieu of the
        // constructor.
        inline void Initialize (NiAVObject* pObject);

        // *** end Emergent internal use only ***

    protected:
        // The picked object.
        NiAVObjectPtr m_spObject;

        // Support for picking in NiAlphaSorter.  The picked NiMesh have
        // been removed from the scene graph, so their parent information is
        // no longer available.  This member stores the original parent of
        // the NiMesh before it was replaced by an NiAlphaProxy.
        NiNodePtr m_spProxyParent;

        // Coordinate space in which the intersection point lives is
        // determined by m_eCoordinateType (COORDINATES_MODEL or
        // COORDINATES_WORLD), default is COORDINATES_WORLD.
        NiPoint3 m_intersect;

        // A true distance only when the initial ray has unit-length
        // direction vector.  Provides ordering along the ray, anyway.
        float m_fDistance;

        // signifies that the pick ray begins inside a solid bounding volume.
        // The intersection point will be the pick origin, and
        // The returned normal will be the pick ray direction.
        bool m_bRayInsideABV;

        // surface attributes at the intersection point
        unsigned int m_uiTriangleIndex;
        unsigned int m_uiVertexIndex[3];
        unsigned int m_uiSubmeshIndex;
        NiPoint2 m_texture;
        NiPoint3 m_normal;
        NiColorA m_color;
    };

    // Simplified array interface.  Used to minimize reallocate/deallocates
    // of NiPick::Records.  Valid results are located at index 0 to
    // GetSize() - 1 (and are packed tightly).
    //
    // The results array serves as both a pool of previously used pick
    // records and as an array of currently valid pick records.  Record
    // objects left in the results array will be reused when the array
    // is cleared, unless they are removed individually with a 
    // RemoveAtAndFill call.
    class NICOLLISION_ENTRY Results : protected NiTPrimitiveArray<Record*>
    {
    public:
        Results (unsigned int uiInitialSize, unsigned int uiGrowBy);
        ~Results ();

        // Returns number of valid results
        inline unsigned int GetSize () const;
        // Returns number of valid results
        inline unsigned int GetEffectiveSize() const;
        // NiTArray wrapper
        Record* GetAt (unsigned int uiIndex) const;

        // Removal functions for particular records.  This will remove
        // a record from the array so that it will not be reused after
        // a RemoveAll call.  The caller is responsible for deleting this
        // memory (and should store it in a smart pointer).
        //
        // Removes from location uiIndex and swaps the last valid
        // record into its place, decrementing the size of the array.
        Record* RemoveAtAndFill (unsigned int uiIndex);

        // Invalidates all records in the array and puts them back in the
        // pool for reuse.  If it is desired to keep a record around and
        // not have it be reused (and modified) during a subsequent
        // pick, use the above removal function to remove a
        // specific record.
        inline void ClearArray ();

        // Sort array by intersection distance from viewer, closest to
        // farthest.
        void SortResults ();

        // Sort array and remove all but the closest elements
        void FindClosest ();

        // Returns a new record.  Tries to use an existing (invalidated) 
        // record if possible.
        inline Record* AddNew (NiAVObject* pObject);
    protected:
        unsigned int m_uiValidResults;
    };

    // pick results array
    Results& GetResults ();
    inline const Results& GetResults() const;

    // *** begin Emergent internal use only ***

    Record* Add (NiAVObject* pkObject);
    inline unsigned int GetSize ();
    
    // NiAlphaSorter uses this function
    Record* GetLastAddedRecord ();

    static void _SDMInit();
    static void _SDMShutdown();

    // *** end Emergent internal use only ***

protected:
    // picking parameters
    QueryType m_eQueryType;
    SortType m_eSortType;
    IntersectType m_eIntersectType;
    FallbackType m_eFallbackType;
    CoordinateType m_eCoordinateType;
    bool m_bFrontOnly;
    bool m_bObserveAppCullFlag;
    NiAVObjectPtr m_spRoot;

    // pick results array
    Results m_pickResults;
    Record* m_pLastAddedRecord;
    PickObjectPolicyPtr m_spPickPolicy;

    // attribute return information
    bool m_bReturnTexture;
    bool m_bReturnNormal;  // true = return normal
    bool m_bReturnSmoothNormal;  // true = return interpolated vertex normals
                                 // false = return facet plane normal
    bool m_bReturnColor;
    bool m_bFindResult;

    static PickObjectPolicyPtr ms_spDefaultPickPolicy;

public:
#if NIMETRICS
    // keep track of how many node and triangle comparisons this pick does
    unsigned int m_uiNodeComparisons;
    unsigned int m_uiTriComparisons;
#endif
};

class NICOLLISION_ENTRY SimpleSkinPickObjectPolicy : 
    public NiPick::PickObjectPolicy
{
public:
    virtual bool FindIntersections(
        const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
        NiRenderObject* pkRenderObj);
};

#include "NiPick.inl"

#endif


