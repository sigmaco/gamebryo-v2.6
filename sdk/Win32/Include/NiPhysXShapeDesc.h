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


#ifndef NIPHYSXSHAPEDESC_H
#define NIPHYSXSHAPEDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXMeshDesc.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated

#pragma warning(disable: 4244) // conversion from 'type' to 'type',
                               // possible loss of data

#pragma warning(disable: 4245) // conversion from 'type' to 'type',
                               // signed/unsigned mismatch

#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXShapeDesc objects are for caching snapshots of NxShapeDesc and
    derived class objects, used for streaming, cloning and reset of simulation
    state.

    An instance of this class stores all the necessary data to recreate a
    PhysX shape derived from NxShape. It stores mesh data in the form
    of a smart pointer to an NiPhysXMeshDesc object. Interpretation of the
    data is dependent on the type of shape.
    
    A pointer to an NxShape is stored when a PhysX shape is created from the
    data.

    NxWheelShape objects and NxHeightFieldShape objects are not supported.
*/
class NIPHYSX_ENTRY NiPhysXShapeDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXShapeDesc);

    /// @endcond

public:
    /// Constructor sets all data to PhysX defaults for NxShape.
    NiPhysXShapeDesc();
    
    /// Destructor releases any pointers.
    ~NiPhysXShapeDesc();
    
    /// Return the type of this shape.
    NxShapeType GetType() const;
    
    /// Set the type for this shape.
    void SetType(const NxShapeType eType);

    /// Get the local pose for the shape (its transformation relative to its
    /// actor). 
    NxMat34 GetLocalPose() const;

    /// Set the local pose for the shape.
    void SetLocalPose(const NxMat34& kPose);

    /// Get the generic shape flags.
    NxU32 GetShapeFlags() const;
    
    /// Set the generic shape flags.
    void SetShapeFlags(const NxU32 uiShapeFlags);
    
    /// Get the shape collision group.
    NxCollisionGroup GetShapeGroup() const;
    
    /// Set the shape collision group.
    void SetShapeGroup(const NxCollisionGroup uiShapeGroup);
    
    /// Get the material index for the shape.
    NxMaterialIndex GetMaterialIndex() const;
    
    /// Set the material index.
    void SetMaterialIndex(const NxMaterialIndex uiMaterialIndex);
    
    /// Get the density.
    NxReal GetDensity() const;
    
    /// Set the density.
    ///
    /// See the PhysX documentation for rules regarding valid values.
    void SetDensity(const NxReal fDensity);
    
    /// Get the mass.
    NxReal GetMass() const;
    
    /// Set the mass.
    ///
    /// See the PhysX documentation for rules regarding valid values.
    void SetMass(const NxReal fMass);
    
    /// Get the skin width.
    NxReal GetSkinWidth() const;
    
    /// Set the skin width.
    void SetSkinWidth(const NxReal fSkinWidth);
    
    /// Get the shape's name.
    const NiFixedString& GetName() const;
    
    /// Set the shape's name.
    void SetName(const NiFixedString& kShapeName);

    /// Get the shape's non interacting component type.
    NxU32 GetNonInteractingComponentType() const;

    /// Set the shape's non interacting component type.
    void SetNonInteractingComponentType(const NxU32 uiNonInteractingCompType);

    
    /// Get the collision bits.
    ///
    /// The given array is filled.
    void GetCollisionBits(NxU32 auiCollisionBits[4]) const;

    /// Set the collision bits.
    void SetCollisionBits(const NxU32 auiCollisionBits[4]);

    /// Get the plane-specific data.
    void GetPlaneShapeData(NxVec3& kNormal, NxReal& kD) const;

    /// Set the plane-specific data.
    void SetPlaneShapeData(const NxVec3& kNormal, const NxReal kD);

    /// Get the sphere-specific data.
    NxReal GetSphereShapeRadius() const;

    /// Set the sphere-specific data.
    void SetSphereShapeRadius(const NxReal kRadius);

    /// Get the box-specific data.
    NxVec3 GetBoxShapeDimensions() const;

    /// Set the box-specific data.
    void SetBoxShapeDimensions(const NxVec3& kDimensions);

    /// Get the capsule-specific data.
    void GetCapsuleShapeData(NxReal& fRadius, NxReal& fHeight, NxU32& uiFlags)
        const;

    /// Set the capsule-specific data.
    void SetCapsuleShapeData(const NxReal& fRadius, const NxReal& fHeight,
        const NxU32& uiFlags);

    /**
        Set the mesh descriptor for the shape.
        
        This data is used for convex and triangle mesh objects.
        The pointer passed in by SetMeshDesc is owned by the shape and should
        not be deleted by the application. 
    */
    void SetMeshDesc(NiPhysXMeshDesc* pkMeshDesc);
    
    /// Get the mesh descriptor (could be null).
    NiPhysXMeshDescPtr GetMeshDesc();

    /**
        Push state into a generic shape descriptor.
        
        If this object does not already have a specific type, the type
        is taken from the argument. This function copies all the state that
        is common to all shapes, and none of the state that is shape specific.
    */
    virtual void ToShapeDesc(NxShapeDesc& kShapeDesc) const;

    /**
        Set state based on a generic shape descriptor.
        
        The type is not set unless this object has no type, in which case
        the type is taken from the argument. This function copies all the
        state that is common to all shapes, and none of the state that is
        shape specific.
        
        Note that it is not possible to instantiate a NxShapeDesc object,
        so you must create a derived object and pass that in. As stated above,
        the type of that object does not matter because it typically will not
        be copied.
    */
    virtual void FromShapeDesc(const NxShapeDesc& kShapeDesc);

    /// Push state to a PhysX plane descriptor.
    ///
    /// The type of this object must be NX_SHAPE_PLANE.
    virtual void ToPlaneDesc(NxPlaneShapeDesc& kPlaneDesc) const;

    /// Pull state from a PhysX plane descriptor.
    ///
    /// Sets the type of this object to NX_SHAPE_PLANE.
    virtual void FromPlaneDesc(const NxPlaneShapeDesc& kPlaneDesc);
    
    /// Push state to a PhysX sphere descriptor.
    ///
    /// The type of this object must be NX_SHAPE_SPHERE.
    virtual void ToSphereDesc(NxSphereShapeDesc& kSphereDesc) const;

    /// Pull state from a PhysX sphere descriptor.
    ///
    /// Sets the type of this object to NX_SHAPE_SPHERE.
    virtual void FromSphereDesc(const NxSphereShapeDesc& kSphereDesc);

    /// Push state to a PhysX box descriptor.
    ///
    /// The type of this object must be NX_SHAPE_BOX.
    virtual void ToBoxDesc(NxBoxShapeDesc& kBoxDesc) const;

    /// Pull state from a PhysX box descriptor.
    ///
    /// Sets the type of this object to NX_SHAPE_BOX.
    virtual void FromBoxDesc(const NxBoxShapeDesc& kBoxDesc);

    /// Push state to a PhysX capsule descriptor.
    ///
    /// The type of this object must be NX_SHAPE_CAPSULE.
    virtual void ToCapsuleDesc(NxCapsuleShapeDesc& kCapsuleDesc) const;

    /// Pull state from a PhysX capsule descriptor.
    ///
    /// Sets the type of this object to NX_SHAPE_CAPSULE.
    virtual void FromCapsuleDesc(const NxCapsuleShapeDesc& kCapsuleDesc);

    /**
        Push state to a PhysX convex descriptor.

        This function will cause the mesh to be created if it does not
        already exist in the PhysX SDK object. Much of the work for this
        function is done by NiPhysXMeshDesc::ToConvexDesc - see the
        NiPhysXMeshDesc documentation.
        
        The type of this object must be NX_SHAPE_CONVEX.

        @param kConvexDesc The descriptor to write state to.
        @param bKeepMesh Controls deletion of mesh data - see NiPhysXMeshDesc.
        @param bCheckPlatform If true, this function will check that the
            platform on which it is executing is the same as that for which
            the mesh was cooked.
    */
    virtual void ToConvexDesc(NxConvexShapeDesc& kConvexDesc,
        const bool bKeepMesh, const bool bCheckPlatform = false);

    /**
        Push state to a PhysX mesh descriptor.

        This function will cause the mesh to be created if it does not
        already exist in the PhysX SDK object. Much of the work for this
        function is done by NiPhysXMeshDesc::ToTriMeshDesc - see the
        NiPhysXMeshDesc documentation.
        
        The type of this object must be NX_SHAPE_MESH.

        @param kTriMeshDesc The descriptor to write state to.
        @param bKeepMesh Controls deletion of mesh data - see NiPhysXMeshDesc.
        @param bCheckPlatform If true, this function will check that the
            platform on which it is executing is the same as that for which
            the mesh was cooked.
    */
    virtual void ToTriMeshDesc(NxTriangleMeshShapeDesc& kTriMeshDesc,
        const bool bKeepMesh, const bool bCheckPlatform = false);

    /// Get the PhysX shape that this descriptor is associated with.
    NxShape* GetShape() const;

    /// Set the PhysX shape that this descriptor is associated with.
    void SetShape(NxShape* pkShape);

    /// @cond EMERGENT_INTERNAL
    
    virtual bool StreamCanSkip();
    
    /// @endcond

protected:

    // All the stuff in common to all shapes.
    NxShapeType m_eType;
    NxMat34 m_kLocalPose;
    NxU32 m_uiShapeFlags;
    NxCollisionGroup m_uiShapeGroup;
    NxMaterialIndex m_uiMaterialIndex;
    NxReal m_fDensity;
    NxReal m_fMass;
    NxReal m_fSkinWidth;
    NiFixedString m_kShapeName;
    NxU32 m_uiNonInteractingCompartmentTypes;
    NxU32 m_auiCollisionBits[4];

    // Used to encode the individual shapes.
    NxReal m_fVal1;
    NxReal m_fVal2;
    NxVec3 m_kPoint1;
    NxU32 m_uiCapsuleFlags;
    
    // Mesh, if there is one
    NiPhysXMeshDescPtr m_spMeshDesc;
    
    // Shape created from the descriptor.
    NxShape* m_pkShape;
};

NiSmartPointer(NiPhysXShapeDesc);

#include "NiPhysXShapeDesc.inl"

#endif // NIPHYSXSHAPEDESC_H

