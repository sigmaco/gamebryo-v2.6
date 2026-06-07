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

#ifndef NIPHYSXCLOTHMODIFIER_H
#define NIPHYSXCLOTHMODIFIER_H

#include "NiPhysXLibType.h"
#include "NiPhysXMeshModifier.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

class NiPhysXClothDest;
class NiPhysXClothKernel;
class NiPhysXScene;
class NxCloth;
class NxActor;

/**
    The NiPhysXClothModifier class is used to deform a Gamebryo mesh according
    to data in a NxCloth object.

    A modifier of this type must be attached to any NiMesh object that is the
    target of PhysX cloth simulation. Between simulation steps, the modifier 
    sets the data streams of the mesh so that it matches the PhysX cloth mesh
    structure and location. When the cloth is not sleeping, the transformation
    of the target Gamebryo node is not modified - everything is done by moving
    mesh vertices. If the cloth is sleeping, the Gamebryo mesh is not modified
    but the node may have its transformation modified if sleep transforms are
    active (see below).

    The sync points for this modifier depend on the structure of the main game
    loop. In cases where PhysX is simulating in parallel with rendering
    (asynchronous simulation), the modifier should be submitted when simulation
    completes (which will be early in a frame) and completed when the next
    simulation step begins, which by definition is before rendering during
    asynchronous simulation. On the other hand, when simulation is
    synchronous, the modifier is submitted when visible and completed at
    render. It is important to be certain that the modifier does not execute
    inside a PhysX simulate/fetch block.
*/
class NIPHYSX_ENTRY NiPhysXClothModifier : public NiPhysXMeshModifier
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXClothModifier);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(NiUInt8);
    /// @endcond

public:
    /// Constructor.
    ///
    /// The default mesh growth factor is 1.5 and the modifier is active.
    NiPhysXClothModifier();
    
    /// Destructor.
    ///
    /// Frees all data arrays and sets any smart pointers to 0.
    virtual ~NiPhysXClothModifier();

    /** 
        This function is called when there is a change in the NiPhysXScene
        object that the cloth is associated with.

        The scene pointer is cached and submission and completion points
        are set. Attach is called on the modifier - the modifier may not
        be attached already because a scene is required for attachment.
    */
    virtual void SceneChanged(NiMesh* pkMesh, NiPhysXScene* pkScene);

    /**
        Returns true if the requirements for this modifier on the given
        platform are met by the mesh.

        This modifier requires CPU readable and writable position stream
        with format F_FLOAT32_3.
        
        If normal updating is enabled, then the normal and binormal and
        tangent streams, if present, must be CPU readable and writable.
        
        If the cloth can tear, then all USAGE_VERTEX streams
        and USAGE_INDEX streams must be CPU readable and write mutable.
    */
    virtual bool AreRequirementsMet(NiMesh* pkMesh, 
        NiSystemDesc::RendererID ePlatform) const;

    /// Fill the requirements structure with this modifiers requirements.
    ///
    /// See AreRequirementsMet for a description of the requirements.
    virtual void RetrieveRequirements(NiMeshRequirements& kRequirements) const;

    /**
        Attach function to apply the modifier to a mesh.

        This function returns false (attachment fails) when either the
        NiPhysXScene object or NxCloth object associated with the modifier
        are not yet known. The modifier, therefore, will fail to attach
        at any time before the PhysX content is instantiated.

        Any functions implemented by this class that modify the NiPhysXScene
        object or the NxCloth object will attempt to attach the modifier in
        case it was not previously attached (or detach if the scene or cloth
        are set to null).
    */
    virtual bool Attach(NiMesh* pkMesh);
    
    /// Detaches the modifier from the mesh.
    virtual bool Detach(NiMesh* pkMesh);

    /**
        Submit the Floodgate tasks for the modifier.

        A task is submitted if the sync points are correct, the modifier is
        set to be active, or if the update is forced. The forced flag is
        set when the NiPhysXScene object is forced to update.

        The Submit function does any work to process tearing,
        then submits a Floodgate task to perform the deformation of the mesh.

        This function assumes that the pkArgs argument is of type
        NiPhysXSyncArgs when the submission sync point is
        SYNC_PHYSICS_COMPLETED.
    */
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);
    
    /**
        Completes any Floodgate tasks and does post-deformation work.

        This function waits on any outstanding tasks and then recomputes
        normals and BT streams (if present on the mesh). Finally, it updates
        the mesh bound.
    */
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs);

    /// Return the NxCloth object that is driving the mesh, or 0 if none.
    NxCloth* GetCloth();

    /**
        Set the PhysX cloth object that is driving the mesh.

        In addition to the cloth object, the function also takes an array of
        vertex indicies and the mesh that the modifier is associated with. The
        index array is a mapping from Gamebryo vertex indices to PhysX cloth
        indices: the ith entry in the array gives the PhysX mesh vertex index
        correspinding to the ith Gamebryo mesh vertex. This mapping may
        be may-to-one depending on mesh topology.

        The arrays in the NxMeshData object associated with the cloth are
        allocated by this function.

        When the cloth is set on the modifier, the data streams on the mesh may
        be resized to accomodate growth from torn vertices. The final size
        is determined by the mesh growth factor (see SetMeshGrowFactor).
        If the data streams are resized, all of the modifiers on the mesh are
        reset. Regardless, the Attach function is called on this modifier
        because it may not have succeeded previously.

        The cloth object may be set to NULL, in which case the modifier is
        detached.
    */
    void SetCloth(NxCloth* pkCloth, const NiUInt16* puiVertexMap, 
        NiMesh* pkMesh);
    
    /**
        Return the cloth mesh data associated with the modifier.

        It is an internal object, owned by this object, for transferring the
        state out of the PhysX simulation and into the application space.
        Access is provided primarily for debugging purposes, and applications
        should never modify the contents of this structure.
    */
    NxMeshData& GetClothData();
    
    /**
        Return the parent actor associated with the cloth.
        
        The parent actor is a PhysX rigid body that corresponds to the
        scene graph parent of th ecloth mesh. It is used to improve
        coordinate system transformations, and should always be set if
        the mesh's parent node has a PhysX actor. This will typically be
        the case if the cloth is attached to an actor in the PhysX simulation
        and the Gamebryo node is similarly attached in the scene graph.
    */
    NxActor* GetParentActor();

    /// Set the parent actor.
    ///
    /// See GetParentActor.
    void SetParentActor(NxActor* pkActor);

    /// Get the Active flag.
    ///
    /// Cloth data is only pushed to the mesh when the modifier is active.
    bool GetActive() const;

    /// Set the status of the Active flag (default is true).
    void SetActive(const bool bActive);

    /// Get the Update Normals flag.
    ///
    /// Normals, bitangents and tangents are only updated if the flag is true.
    bool GetUpdateNormals() const;
    
    /// Set the UpdateNormals flag.
    ///
    /// Default is true.
    void SetUpdateNormals(const bool bUpdate);

    /// Get the mesh growth factor.
    ///
    /// See SetMeshGrowFactor. 
    float GetMeshGrowFactor() const;

    /**
        Set the mesh growth factor.

        When a cloth object is set on the modifier, extra space may be
        allocated for all USAGE_VERTEX data streams on the mesh to make space
        for vertices added through tearing. This value controls how much
        space is added: a value of 1.0 means no additional space, while
        1.5 means 50% more space and 2.0 means twice as much
        space. Set this value to 1.0 if the cloth will never tear.

        The default is 1.5 if the cloth has tearing enabled, or 1.0 if tearing
        is disabled on the cloth when the SetCloth function is called
        (including when a prop containing the cloth is instantiated in a
        PhysX scene). If a value is explicitly specified before the cloth is
        set, that value is retained. The value from the original cloth is
        used if the cloth object is replaced.
        
        This function may need to be used when the tearing
        flags on the cloth are changed after the modifier is set up with the
        cloth, particularly if going from non-tearable to tearable. The cost
        of a change is significant because data streams are replaced on the
        target NiMesh object.
    */
    void SetMeshGrowFactor(const float fGrowFactor);
    
    /// Get the Tearable Hint.
    ///
    /// See SetTearableHint.
    bool GetTearableHint() const;
    
    /**
        Set the Tearable Hint.
        
        This hint should be set to true in the art tools if the cloth is
        expected to tear. It is used only in determining the mesh modifier
        requirements in the RetrieveRequirements function and
        in AreRequirementsMet if the mesh grow factor has not been set.
    */
    void SetTearableHint(const bool bTearable);

    /**
        Get the flag controlling sleep behavior.

        When PhysX cloth is sleeping, there is no need to update the Gamebryo
        mesh geometry. However, if the Gamebryo node to which the cloth is
        attached is moving, or the Gamebryo-PhysX scene transformation is
        animated, then the rendered cloth will move incorrectly. If sleep
        transforms are active, the Gamebryo node transformation is updated to
        correct for its relative motion. The default is false. Set it to true
        if you are animating the Gamebryo node to which the cloth is
        associated or if you are dynamically modifying the transformation
        between the PhysX scene and the Gamebryo world.
    */
    bool GetDoSleepXforms() const;

    /// Set the flag controlling sleep transforms.
    ///
    /// See GetDoSleepXforms.
    void SetDoSleepXforms(const bool bDoSleepXforms);
    
    /// Get the texture set associated with NBT coordinates.
    NiUInt16 GetNBTSet() const;
        
    /**
        Set the texture set associated with NBT coordinate generation.
        
        This is only necessary if the Gamebryo mesh shader is using NBT frames.
        Applications will almost never set this - it is done in the art tools.
        The default is 0.
    */
    void SetNBTSet(const NiUInt16 uiSet);

    /// Returns true if the modifier thinks the lcoth is sleeping, false
    /// otherwise.
    bool GetSleeping() const;

    /// @cond EMERGENT_INTERNAL

    virtual void ProcessClone(NiCloningProcess& kCloning);

    virtual bool StreamCanSkip();

    /// @endcond

private:
    bool IsRequirementMet(NiMesh* pkMesh,
        const NiFixedString& kSemantic, bool bMustExist,
        NiUInt8 uiReadMask, NiUInt8 uiWriteMask,
        const char* acWriteDesc, const char* acCondition,
        NiDataStreamElement::Format kFormat = NiDataStreamElement::F_UNKNOWN)
        const;

    void SetSleeping(bool bSleeping);

    void ProcessBasicTearing(NiMesh* pkMesh);
    void ProcessGeneralTearing(NiMesh* pkMesh);
    void CopyVertexData(NiUInt32 uiSrc, NiUInt32 uiDest, NiMesh* pkMesh);

    void RestoreTriangleIndices(NiMesh* pkMesh);

    void AllocateClothData(const NiUInt16* puiVertexMap, NiMesh* pkMesh);
    void DeallocateClothData();

    // A map from split vertices to their original Gamebryo vertex. Assumes
    // the most of the time only a few vertices are split. Otherwise it would
    // be faster to use a straight parent vertex array. In other words, if
    // your cloth is getting torn to shreds you might want to change
    // this implementation.
    NiTMap<NiUInt16, NiUInt16> m_kSplitParentMap;

    // The cloth that we are working from.
    NxCloth* m_pkCloth;
    
    // The mesh data for the cloth. We have it here for memory management
    NxMeshData m_kClothData;

    // The amount by which the number of vertices is allowed to grow as
    // cloth tears.
    float m_fMeshGrowFactor;

    // The mapping from vertices in the shape to vertices in the PhysX mesh.
    NiUInt16* m_puiVertexMap;
    
    // A pointer to the scene we are in
    NiPhysXScene* m_pkScene;
    
    // An optional pointer to a NxActor object that is driving a
    // Gamebryo node that is rigidly attached to the Gamebryo node
    // for the cloth mesh. This can be used to more accurately extract
    // transforms.
    NxActor* m_pkParentActor;

    // The world transform of the target node when the cloth goes to sleep,
    // expressed in PhysX world coordinates. If the cloth is sleeping, this
    // should remain constant, so we set the local transform on the
    // Gamebryo node to give this world pose.
    NiTransform m_kWorldXform;

    /// Transform to use as fixed input for kernel
    NiTransform m_kXform;

    /// Boolean that indicates if the cloth position data has been updated.
    bool m_bUpdatedPositionData;

    // Internal previous vertex count, needed to process tears.
    NiUInt16 m_uiPreviousVertexCount;

    // The original number of vertices in the target shape. Set at
    // construction and never changed after that.
    NiUInt16 m_uiOriginalVertexCount;
    
    static NiUInt16 ms_uiInvalidVertexCount;
    
    // The texture set to use for NBT generation, if required
    NiUInt16 m_uiNBTSet;
    
    // True if we have started a forced update but not yet finished it
    bool m_bForceCached;
    
    enum ClothModifierFlags
    {
        // Whether or not to update transforms when the cloth is sleeping.
        // If the m_bDoSleepXforms is false, we assume that the Gamebryo
        // node is static with respect to the PhysX coordinate system and
        // hence no update is necessary when the cloth is sleeping. True
        // by default.
        DO_SLEEP_XFORMS_MASK = 0x01,
        
        // Whether or not the cloth is sleeping _and_ has been updated to it's
        // sleeping state. Read only.
        SLEEPING_MASK = 0x02,

        // Are we active?
        ACTIVE_MASK = 0x04,
        
        // Update normals?
        UPDATE_NORMALS_MASK = 0x08,
        
        // Tearable hint
        TEARABLE_HINT_MASK = 0x10
    };
};

// Define a NiPhysXClothModifier smart pointer
NiSmartPointer(NiPhysXClothModifier);

#include "NiPhysXClothModifier.inl"

#endif // #ifndef NIPHYSXCLOTHMODIFIER_H
