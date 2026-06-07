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

#ifndef NIBOUNDINGVOLUME_H
#define NIBOUNDINGVOLUME_H

#include "NiCollisionLibType.h"
#include <NiTArray.h>
#include <NiViewerStrings.h>

class NiBound;
class NiBoundingVolume;
class NiTransform;
class NiPoint3;
class NiStream;
class NiMesh;

// Streaming support for bounding volumes.  Insert in class declaration for
// NiBoundingVolume-derived classes.  Note the start of public scope.
#define NiDeclareBVStream \
    public: \
        static NiBoundingVolume* CreateFromStream(NiStream& kStream); \
        virtual void LoadBinary(NiStream& kStream); \
        virtual void SaveBinary(NiStream& kStream)

// Insert in NiCollisionSDM::Init for all streamable bounding volumes.
#define NiRegisterBVStream(classname, type) \
    NiBoundingVolume::ms_apfnLoaders[NiBoundingVolume::type] = \
     classname::CreateFromStream

#define NiRegisterBVLoader(loadername, type) \
    NiBoundingVolume::ms_apfnLoaders[NiBoundingVolume::type] = \
     loadername

// Insert in NiCollisionSDM::Shutdown for all bounding volume types.
#define NiUnregisterBVStream(type) \
    NiBoundingVolume::ms_apfnLoaders[NiBoundingVolume::type] = 0

// Intersection function type definitions
typedef bool(*NiBVTestIntersectFunction)
   (float, const NiBoundingVolume&, const NiPoint3&, const NiBoundingVolume&, 
     const NiPoint3&); 

typedef bool(*NiBVFindIntersectFunction)
   (float, const NiBoundingVolume&, const NiPoint3&, const NiBoundingVolume&, 
     const NiPoint3&, float&, NiPoint3&, bool, NiPoint3&, NiPoint3&); 

typedef bool(*NiBVTriTestIntersectFunction)
   (float, const NiBoundingVolume&, const NiPoint3&, 
     const NiPoint3&, const NiPoint3&, const NiPoint3&, const NiPoint3&); 

typedef bool(*NiBVTriFindIntersectFunction)
   (float, const NiBoundingVolume&, const NiPoint3&, 
     const NiPoint3&, const NiPoint3&, const NiPoint3&, const NiPoint3&, 
     float&, NiPoint3&, bool, NiPoint3&, NiPoint3&); 


class NICOLLISION_ENTRY NiBoundingVolume : public NiMemObject
{
    NiDeclareBVStream;
public:
    inline virtual ~NiBoundingVolume() {/* */};

    // Run-time type information.  NiMain supports only sphere bounding
    // volumes.  To use the other bounding volumes, an application needs
    // to link in the NiCollision library.
    enum BoundType
    {
        BASE_BV      = -1,
        SPHERE_BV    =  0,
        BOX_BV       =  1 ,
        CAPSULE_BV   =  2,
        LOZENGE_BV   =  3,
        UNION_BV     =  4,
        HALFSPACE_BV =  5,
        MAXTYPE_BV   =  6
    };

    virtual int Type() const;

    // Index of primitive bounding volume that generated a collision in the
    // last call to NiCollisionGroup::FindCollisions(...).  Only ever nonzero
    // for union bounding volumes.
    virtual unsigned int WhichObjectIntersect() const;

    // *** begin Emergent internal use only ***
    
    // Called by the collision system to update the world space bounding
    // volume whenever the model space bounding volume has changed.
    virtual void UpdateWorldData(const NiBoundingVolume& kModelBV,
        const NiTransform &kWorld) = 0;
    
    // Used whenever an intersection test is done and the world space
    // bounding volume does not yet exist.
    virtual NiBoundingVolume* Create() const = 0;
    
    // cloning
    virtual NiBoundingVolume* Clone() const = 0;

    // deep-copy assignment
    virtual void Copy(const NiBoundingVolume& kBV) = 0;

    // comparision
    virtual bool operator==(const NiBoundingVolume& kBV) const = 0;
    virtual bool operator!=(const NiBoundingVolume& kBV) const = 0;
    
    // stream management
    typedef NiBoundingVolume*(*CreateFunction)(NiStream&);
    static CreateFunction GetCreateFunction(unsigned int i);
    
    // Intersection test/find routines that do function lookups based on the
    // types of input parameter bounding volumes.
    static bool TestIntersect(float fTime,
        const NiBoundingVolume& kBV0, const NiPoint3& kVel0, 
        const NiBoundingVolume& kBV1, const NiPoint3& kVel1);
    
    static bool FindIntersect(float fTime,
        const NiBoundingVolume& kBV0, const NiPoint3& kVel0, 
        const NiBoundingVolume& kBV1, const NiPoint3& kVel1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
    
    static bool TestTriIntersect(float fTime,
        const NiBoundingVolume& kBV, const NiPoint3& kVel0,
        const NiPoint3& kVert0, const NiPoint3& kVert1,
        const NiPoint3& kVert2, const NiPoint3& kVel1);
    
    static bool FindTriIntersect(float fTime,
        const NiBoundingVolume& kBV, const NiPoint3& kVel0,
        const NiPoint3& kVert0, const NiPoint3& kVert1,
        const NiPoint3& kVert2, const NiPoint3& kVel1, float& fIntrTime,
        NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0,
        NiPoint3& kNormal1);

    // For automatic generation of alternate bounding volumes.
    // "Fast" version returns a reference to static member data for speed.
    // "Safe" version returns a pointer to an allocated NiSphereBV.
    static NiBoundingVolume* ConvertToSphereBV_Fast(const NiBound& kBound);
    static NiBoundingVolume* ConvertToSphereBV_Safe(const NiBound& kBound);
    static NiBoundingVolume* ConvertToBoxBV(unsigned short usQuantity,
        const NiPoint3* pkVertex);
    static NiBoundingVolume* ConvertToBoxBV(NiMesh* pkMesh);

    // For printing of ABV types.
    virtual void AddViewerStrings(const char* pcPrefix, 
        NiViewerStringsArray* pkStrings) const = 0;

    // *** end Emergent internal use only ***

protected:
    // The NiCollision library hooks into NiMain via these members.
    friend class NiCollisionSDM;

    static NiBVTestIntersectFunction* ms_pfnTestFuncTable;
    static NiBVFindIntersectFunction* ms_pfnFindFuncTable;
    static NiBVTriTestIntersectFunction* ms_pfnTriTestFuncTable;
    static NiBVTriFindIntersectFunction* ms_pfnTriFindFuncTable;
    
    static NiBoundingVolume*(*ms_pfnConvertToSphereBV_Fast)(float fRad, 
        const NiPoint3& kCenter);
    static NiBoundingVolume*(*ms_pfnConvertToSphereBV_Safe)(float fRad, 
        const NiPoint3& kCenter);
    static NiBoundingVolume*(*ms_pfnConvertToBoxBV)(unsigned short usQuantity,
        const NiPoint3* pkVertex);
    static NiBoundingVolume*(*ms_pfnConvertToBoxBV_FromMesh)(
        NiMesh* pkMesh);

    static CreateFunction ms_apfnLoaders[MAXTYPE_BV];
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiBoundingVolume.inl"

//---------------------------------------------------------------------------

#endif

