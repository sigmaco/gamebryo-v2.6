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


#ifndef NIPHYSXCLOTHDESC_H
#define NIPHYSXCLOTHDESC_H

#include "NiPhysXLibType.h"

#include <NiMeshLib.h>

#include "NiPhysXActorDesc.h"
#include "NiPhysXMeshDesc.h"
#include "NiPhysXShapeDesc.h"

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

class NiPhysXClothDest;
NiSmartPointer(NiPhysXClothDest);

/**
    This class holds a complete description of a PhysX cloth object.
    
    This class is essentially a wrapper for the NxClothDesc class, with
    additional members for the connection to Gamebryo's scene graph. It is
    used to stream a cloth description in NIF files, to support cloning of
    cloth objects, and to support multiple initial conditions with reset.

    @note This class does not cache the userData associated with the cloth.
        Applications should derive from this class if they wish to cache or
        stream such data.
*/
class NIPHYSX_ENTRY NiPhysXClothDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXClothDesc);

    /// @endcond

public:
    /**
        Class for attachments.
        
        Each instance of this class holds the attachment information for a
        cloth object. Multiple instances may be required for one piece of
        cloth if the cloth is attached to more than one shape or has both
        shape and collision type attachments.
    */
    class NIPHYSX_ENTRY ClothAttachment : public NiMemObject
    {
    public:
        /// Constructor will create an attachment of the form
        /// "attach to all shapes currently penetrating", and no flags.
        ClothAttachment();
        
        /// Destructor frees all memory.
        ~ClothAttachment();
        
        /// The shape to which the attachment is made, or NULL if it is
        /// a global attachment.
        NiPhysXShapeDescPtr m_spShape;
        
        /// The number of vertices that are attached or 0 if
        /// this is a "collision" type attachment.
        NiUInt32 m_uiVertexCount;
        
        /**
            The vertices that are attached.

            There must be m_uiVertexCount entries in this array. It should
            be allocated using NiMalloc or NiAlloc.
        */
        NiUInt32* m_puiVertexIDs;
        
        /**
            The locations of the attachment on the shape or in the world.

            There must be m_uiVertexCount entries in this array. When
            m_spShape is non-NULL, these positions are treated as points in
            the shape's local coordinate system, otherwise they are global
            locations. The array should be allocated using NiMalloc or NiAlloc.
        */
        NxVec3* m_pkPositions;
        
        /*
            Attachment flags.
            
            This array should have a single element when m_uiVertexCount
            is 0, in which case it holds the flags for a "collision" type
            attachment. When the vertex count is non-zero, there must be
            m_uiVertexCount entries in this array with flags for each vertex.
            Regardless of the array's size, it must be allocated using
            NiMalloc or NiAlloc.
        */
        NxU32* m_puiFlags;
    };

    /// Constructor sets all paramaters to their PhysX default, with no mesh
    /// data.
    NiPhysXClothDesc();
    
    /// Destructor releases all smart pointers and deletes owned memory,
    /// such as attachments.
    virtual ~NiPhysXClothDesc();
    
    /// Obtain a pointer to the Gamebryo scene graph object that the cloth is
    /// controlling.
    NiMesh* GetTargetMesh() const;
    
    /// Set the pointer to the Gamebryo scene graph object that the cloth is
    /// controlling.
    void SetTargetMesh(NiMesh* pkMesh);

    /**
        Obtain the descriptor for the parent actor of the cloth.

        The parent actor is a PhysX actor that is associated with the
        Gamebryo parent node of the cloth's target Gamebryo object. It is
        used to improve the accuracy of transformation computations, and
        should always be set if the target's parent node has a PhysX actor.
        This will typically be the case if the cloth is attached to an
        actor in the PhysX simulation and the Gamebryo node is similarly
        attached in the scene graph.
    */
    NiPhysXActorDesc* GetParentActor() const;
    
    /// Set the parent actor descriptor.
    ///
    /// See GetParentActor for a description of the parent actor.
    void SetParentActor(NiPhysXActorDesc* pkActorDesc); 
    
    /**
        @name Fixed parameter access functions.
        
        Each pair of functions provides access to a member of the
        NxClothDesc class. See the PhysX documentation for information
        on the meaning of these parameters.

        The set functions only modify the value stored in this object, which
        is used when the cloth is first created and for streaming and cloning.
        It does not modify the property of an already existing NxCloth created
        from the object. If you would like to modify an existing cloth object,
        use the GetCloth function to obtain the NxCloth object and set the
        property using a function call on that.
    */
    /// @{
    NiFixedString GetClothName() const;
    void SetClothName(NiFixedString& kClothName);
    NiPhysXMeshDescPtr GetMeshDesc();
    void SetMeshDesc(NiPhysXMeshDesc* pkMeshDesc);
    NxReal GetThickness() const;
    void SetThickness(const NxReal fThickness);
    NxReal GetDensity() const;
    void SetDensity(const NxReal fDensity);
    NxReal GetBendingStiffness() const;
    void SetBendingStiffness(const NxReal fStiffness);
    NxReal GetStretchingStiffness() const;
    void SetStretchingStiffness(const NxReal fStiffness);
    NxReal GetDampingCoefficient() const;
    void SetDampingCoefficient(const NxReal fCoefficient);
    NxReal GetFriction() const;
    void SetFriction(const NxReal fFriction);
    NxReal GetPressure() const;
    void SetPressure(const NxReal fPressure);
    NxReal GetTearFactor() const;
    void SetTearFactor(const NxReal fTearFactor);
    NxReal GetCollisionResponseCoefficient() const;
    void SetCollisionResponseCoefficient(const NxReal fCoefficient);
    NxReal GetAttachmentResponseCoefficient() const;
    void SetAttachmentResponseCoefficient(const NxReal fCoefficient);
    NxReal GetAttachmentTearFactor() const;
    void SetAttachmentTearFactor(const NxReal fFactor);
    NxReal GetToFluidResponseCoefficient() const;
    void SetToFluidResponseCoefficient(const NxReal fCoefficient);
    NxReal GetFromFluidResponseCoefficient() const;
    void SetFromFluidResponseCoefficient(const NxReal fCoefficient);
    NxReal GetMinAdhereVelocity() const;
    void SetMinAdhereVelocity(const NxReal fMinAdhereVelocity);
    NxReal GetRelativeGridSpacing() const;
    void SetRelativeGridSpacing(const NxReal fSpacing);
    NxU32 GetSolverIterations() const;
    void SetSolverIterations(const NxU32 uiIterations);
    NxVec3 GetExternalAcceleration() const;
    void SetExternalAcceleration(const NxVec3 kAcceleration);
    NxVec3 GetWindAcceleration() const;
    void SetWindAcceleration(const NxVec3 kWindAcceleration);
    NxReal GetWakeUpCounter() const;
    void SetWakeUpCounter(const NxReal fCounter);
    NxReal GetSleepLinearVelocity() const;
    void SetSleepLinearVelocity(const NxReal fVelocity);
    NxCollisionGroup GetCollisionGroup() const;
    void SetCollisionGroup(const NxCollisionGroup uiGroup);
    NxGroupsMask GetCollisionMask() const;
    void SetCollisionMask(const NxGroupsMask kMask);
    NxU16 GetForceFieldMaterial() const;
    void SetForceFieldMaterial(const NxU16 uiForceFieldMaterial);
    NxU32 GetClothFlags() const;
    void SetClothFlags(const NxU32 uiFlags);
    /// @}
    
    /**
        Set all the constant parameters in one function call.
        
        The constants parameters are those that are considered to be
        independent of changes to the initial state of the cloth, so they
        remain constant regardless of which state the cloth is reset to.
    */
    void SetConstantParameters(NiFixedString& kClothName, 
        NiPhysXMeshDesc* pkMeshDesc, const NxReal fThickness, 
        const NxReal fDensity, const NxReal fBendingStiffness, 
        const NxReal fStretchingStiffness, const NxReal fDampingCoefficient, 
        const NxReal fFriction, const NxReal fPressure,
        const NxReal fTearFactor, const NxReal fCollisionResponseCoefficient,
        const NxReal fAttachmentResponseCoefficient, 
        const NxReal fAttachmentTearFactor, 
        const NxReal fToFluidResponseCoefficient, 
        const NxReal fFromFluidResponseCoefficient, 
        const NxReal fMinAdhereVelocity, const NxReal fRelativeGridSpacing, 
        const NxU32 uiSolverIterations, const NxVec3 kExternalAcceleration,
        const NxVec3 kWindAcceleration, const NxReal fWakeUpCounter, 
        const NxReal fSleepLinearVelocity, 
        const NxCollisionGroup uiCollisionGroup, 
        const NxGroupsMask kCollisionMask, const NxU16 uiForceFieldMaterial, 
        const NxU32 uiFlags);


    /**
        @name Initial State Storage
        
        The descriptor can hold a number of initial conditions for the cloth.
        Each state is represented by a pose matrix, mapping onto the
        globalPose member of NxClothDesc, an array of cloth vertex
        locations, an ordered array of vertex indexes to tear at, and
        a corresponding array of normal vectors to use as tearing split planes
        (see the PhysX documentation on explicit cloth tearing).
        
        It is not necessary to supply both a pose and the vertex positions.
        When no vertex positions are given, the pose is used to transform the
        vertex locations in the original cooked cloth mesh to produce
        this piece of cloth's locations. If vertex positions are supplied,
        there must be as many vertex positions as the number of original
        vertices plus the number of torn vertices.
        
        Vertex positions have priority over the pose because they are set
        on the cloth after it has been created, while the pose is only used
        at creation. Typically, if the pose for a state is non-identity,
        then vertex positions will not be given and the cooked mesh vertex
        locations will be used. This is adequate if the cloth has not been
        deformed from its cooked state.
        
        The vertex positions do not modify the rest
        length of the edges in the cloth nor the angles between vertices -
        such things are cooked into the NxClothMesh object and cannot be
        changed after cooking. The distances between vertices should therefore
        be reasonably close to the rest shape of the cloth in order to avoid
        very large forces when simulation starts.
    */
    
    /// @{
    
    /// Get the number of states stored.
    NiUInt32 GetNumStates() const;
    
    /// Get the pose at a particular index.
    NxMat34 GetGlobalPose(const NiUInt32 uiIndex) const;

    /**
        Set the pose at a particular index.

        If the given index is greater than the number of stored states,
        additional states will be allocated up to the index count.
    */
    void SetGlobalPose(const NxMat34 kPose, const NiUInt32 uiIndex);
    
    /// Get the number of vertex positions in the vertex position array stored
    /// at a particular state index.
    NiUInt16 GetVertexPositionsCount(const NiUInt32 uiIndex) const;

    /// Get the vertex position array stored at a particular state index.
    const NxVec3* GetVertexPositions(const NiUInt32 uiIndex) const;

    /**
        Set the vertex position array stored at a particular state index.

        The number of vertices in the array must be given. The array is copied.
    */
    void SetVertexPositions(const NiUInt16 uiNumPosns, const NxVec3* pkPosns,
        const NiUInt32 uiIndex);

    /// Get the number of torn vertices in the torn vertex array stored at
    /// a particular state index.
    NiUInt16 GetTornVerticesCount(const NiUInt32 uiIndex) const;

    /// Get the torn vertex array stored at a particular state index.
    const NiUInt16* GetTornVertices(const NiUInt32 uiIndex) const;

    /// Get the torn vertex split planes array stored at a particular state
    /// index.
    const NxVec3* GetTornSplitPlanes(const NiUInt32 uiIndex) const;

    /// Allocate space to hold a given number of saved states.
    ///
    /// Use this function to reduce allocations when adding many states.
    virtual void PresizeStateArrays(const NiUInt32 uiRequiredCount);

    /**
        Set the torn vertex data stored at a particular state index.

        The vertices are torn, starting with the original mesh, in the
        order they appear in the array. The number of torn vertices in the
        array must be given, along with normal vectors to use for the
        split planes at each tear. The arrays are copied.
    */
    void SetTornVertices(const NiUInt16 uiNumTorn, const NiUInt16* pkIndexes,
        const NxVec3* pkNormals, const NiUInt32 uiIndex);

    /**
        Add a new state derived from the current state of the cloth.
        
        This function must be called after CreateCloth to ensure that the
        cloth object exists from which to pull state. Torn vertices are
        extracted, but the torn split planes are all set arbitrarily to the
        vector between the split vertex and it's parent. This means that the
        result of applying the stored state may not be identical to the
        cloth at the time the state is added.
    */
    virtual NiUInt32 AddState(const NxMat34& kXform);
    
    /// Remove all the stored states starting at and beyond the given index.
    virtual void RemoveStatesBeyond(const NiUInt32 uiState);

    /// Remove the stored state at the given index, and shuffle down other
    /// states to fill any hole in stored state indexes.
    virtual void RemoveState(const NiUInt32 uiState);
    
    /// @}

    /// @name Attachment functions
    /// @{

    /// Get the number of attachments for this cloth.
    NiUInt32 GetAttachmentCount() const;

    /**
        Get a pointer to the attachment at a given index.

        The index must be less than the number of attachments. The function
        could return NULL.
    */
    ClothAttachment* GetAttachmentAt(const NiUInt32 uiIndex);
    
    /**
        Add an attachment for this cloth.
        
        This function will add the attachment in PhysX if the scene has
        already been created. The cloth descriptor owns the memory for the
        passed object and will free it. The object passed in should have
        been allocated with NiNew.
    */
    void AddAttachment(ClothAttachment* pkAttachment);
    
    /**
        Remove an attachment of this cloth.
        
        This will remove the attachment in the scene if it has already been
        created. The attachment is deleted when it is removed, so
        applications should not try to access the ClothAttachment pointer
        after this function returns true (indicating that the attachment was
        found and removed).
    */
    bool RemoveAttachment(ClothAttachment* pkAttachment);

    /**
        Remove an attachment of this cloth at a specified index.
        
        The index must be less than the number of attachments.
        This will remove the attachment in the scene if it has already been
        created. The attachment is deleted when it is removed. This function
        returns true when there is an attachment at the specified index,
        false otherwise.
    */
    bool RemoveAttachmentAt(const NiUInt32 uiIndex);
    /// @}

    /// @name Interaction with the PhysX scene
    /// @{
    
    /// Check that the parameters in this descriptor are considered valid by
    /// PhysX.
    virtual bool IsValid();
    
    /**
        Create the NxCloth object in the PhysX scene.
        
        @param pkScene The PhysX scene to create in.
        @param pkCompartment The compartment to create the cloth in (see PhysX
            documentation).
        @param uiIndex The index for the initial conditions to use when
            creating the cloth.
        @param kXform An additional transformation to apply to the pose or
            vertices of the cloth.
        @param bKeepMeshes If true, the streamed mesh data for the cloth will
            be retained even after the cloth mesh is instantiated from the
            data.
            
        @return A pointer to the cloth object that is created. This pointer is
            also stored internally.
    */
    virtual NxCloth* CreateCloth(NxScene* pkScene,
        NxCompartment* pkCompartment,  const NiUInt32 uiIndex,
        const NxMat34& kXform, const bool bKeepMeshes = false);
    
    /**
        Remove the NxCloth object associated with this descriptor from
        the PhysX scene.
        
        This sets the internal pointer to the cloth object to NULL. Nothing
        is done if the pointer is already NULL.
    */
    virtual void RemoveCloth(NxScene* pkScene);

    /**
        Obtain the NxCloth object that this descriptor generated.
        
        The return value will only be non-NULL after CreateCloth has been
        called to create the PhysX cloth object, and is reset to NULL
        when RemoveCloth is invoked.
    */
    NxCloth* GetCloth() const;
    
    /**
        Set the NxCLoth object that this descriptor controls.
        
        Applications should not use this function. Rather, the parameters
        should all be set on this object and the CreateCloth function used to
        create the NxCloth object.
    */
    void SetCloth(NxCloth* pkCloth);
    
    /// @}

    /// @cond EMERGENT_INTERNAL
    
    /**
        Access functions for the NiPhysXClothDest object (now deprecated).
        
        Used only for back compatibility. Remove when pre-NiMesh NIF
        files are no longer support.
    */
    /// @{
    NiPhysXClothDest* GetDest() const;
    void SetDest(NiPhysXClothDest* pkDest);
    /// @}

    /// Indicates that it is safe to ignore this object on streaming.
    virtual bool StreamCanSkip();

    /// Manages setting of pointers to cloned objects.
    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
        
protected:
    /// Push the contents of this object into a NxClothDesc object.
    virtual bool ToClothDesc(NxClothDesc& kClothDesc,  const NiUInt32 uiIndex,
        const NxMat34& kNxMat34, const bool bKeepMeshes,
        const bool bCheckPlatform = false);
    
    /// Copy all of the saved states from this object to another.
    virtual void CopyStates(NiPhysXClothDesc* pkDest);

    /// Parameters from NxClothDesc class
    /// @{
    NiFixedString m_kClothName;
    NiPhysXMeshDescPtr m_spClothMesh;
    NxReal m_fThickness;
    NxReal m_fDensity;
    NxReal m_fBendingStiffness;
    NxReal m_fStretchingStiffness;
    NxReal m_fDampingCoefficient;
    NxReal m_fFriction;
    NxReal m_fPressure;
    NxReal m_fTearFactor;
    NxReal m_fCollisionResponseCoefficient;
    NxReal m_fAttachmentResponseCoefficient;
    NxReal m_fAttachmentTearFactor;    
    NxReal m_fToFluidResponseCoefficient;
    NxReal m_fFromFluidResponseCoefficient;
    NxReal m_fMinAdhereVelocity;
    NxReal m_fRelativeGridSpacing;
    NxU32 m_uiSolverIterations;
    NxVec3 m_kExternalAcceleration;
    NxVec3 m_kWindAcceleration;
    NxReal m_fWakeUpCounter;
    NxReal m_fSleepLinearVelocity;
    NxCollisionGroup m_uiCollisionGroup;
    NxU32 m_auiCollisionBits[4];
    NxU16 m_uiForceFieldMaterial;
    NxU32 m_uiFlags;
    /// @)

    /// Array of attachements.    
    NiTPrimitiveArray<ClothAttachment*> m_kAttachments;
    
    /// PhysX cloth object.
    NxCloth* m_pkCloth;

    /// Parent actor descriptor.
    NiPhysXActorDesc* m_pkParentActor;
    
    /// Mesh we are controlling.
    NiMesh* m_pkTargetMesh;

    /// Storage for initial conditions.
    /// @{
    NiUInt32 m_uiNumStates;
    NxMat34* m_pkPoses;
    NiUInt16* m_puiNumVertexPosns;
    NxVec3** m_ppkVertexPosns;
    NiUInt16* m_puiNumTearIndices;
    NiUInt16** m_ppuiTearIndices;
    NxVec3** m_ppkTearSplitPlanes;
    /// @}
    
    /// For back compatibility of pre 20,4,0,0 NIF files. Remove when no
    /// longer needed for streaming these older files.
    NiPhysXClothDestPtr m_spDest;
};

NiSmartPointer(NiPhysXClothDesc);

#include "NiPhysXClothDesc.inl"

#endif // #ifdef NIPHYSXCLOTHDESC_H
