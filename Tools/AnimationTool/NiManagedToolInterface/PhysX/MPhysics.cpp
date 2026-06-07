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

#include "stdafx.h"
#include "MLogger.h"
#include "MPhysics.h"
#include "MSequence.h"
#include "MSequenceGroup.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;

#if defined(EE_PHYSX_BUILD)

#pragma warning (disable:4714)
//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
MPhysics::MPhysics() :
    m_spPhysicsScene(0), m_pkPhysicsActor(0), m_aSequences(0),
    m_fGravityX(0.0f), m_fGravityY(0.0f), m_fGravityZ(-9.8f),
    m_fTimestep(0.01f), m_bDebugState(false), m_fDebugScale(1.0f),
    m_fGroundFriction(0.3f), m_fGroundBounce(0.3f),
    m_bNeedToSave(false), m_pkGroundActor(0), m_spGroundActorSrc(0),
    m_pkForceSets(0), m_pkForceTarget(0), m_uiLastForceSetUsed(0),
    m_fForceExponent(1.0f), m_fResetTime(0.0f), m_pkSimFetchSection(0)
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Initiates the phsyics system. This includes initializing
// the SDK. This function returns true if the
// physics was successfully initialized or false otherwise.
//---------------------------------------------------------------------------
bool MPhysics::Init()
{
    // Set up the SDK.
    NiPhysXManager* pkPhysManager = NiPhysXManager::GetPhysXManager();

    m_pkOutputObject = NiNew OutputStream;

    NxSDKCreateError eErrorCode;
    if (!pkPhysManager->Initialize(NULL, m_pkOutputObject,
        NiPhysXTypes::NXPHYSICSSDKDESC_DEFAULT, &eErrorCode))
    {
        char acMsg[1024];
        NiSprintf(acMsg, 1024,
            "Unable to initialize PhysX SDK version %d.\n"
            "This may mean you don't have PhysX installed.\n"
            "Have you installed PhysX System Software and Core?\n"
            "The message from the PhysX SDK is:\n"
            "\" %s\"\n",
            NX_SDK_VERSION_NUMBER,
            NiPhysXManager::GetSDKCreateErrorString(&eErrorCode)
            );
        MessageBox(NULL, acMsg, "PhysX SDK Failed", MB_OK | MB_ICONERROR);
        MLogger::LogWarning(acMsg);

        return false;
    }

        
    pkPhysManager->m_pkPhysXSDK->
        setParameter(NX_VISUALIZE_BODY_AXES, 1.0f);
    pkPhysManager->m_pkPhysXSDK->
        setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
    pkPhysManager->m_pkPhysXSDK->
        setParameter(NX_VISUALIZE_JOINT_LIMITS, 1.0f); 
    pkPhysManager->m_pkPhysXSDK->
        setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1.0f);

    // At this point we have every reason to believe we have a PhysX SDK
    // available.
    
    m_aSequences = NiExternalNew ArrayList;
    
    m_pkForceSets = NiExternalNew ArrayList;
    m_pkForceTarget = 0; // Non-null when the user is dragging to apply a force
    
    m_pkSimFetchSection = NiExternalNew NiCriticalSection;    

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cleans up and releases all physics objects. Releases the
// scene and SDK.
//---------------------------------------------------------------------------
void MPhysics::Shutdown()
{
    if (!Present)
        return;
    
    RemoveGroundPlane();
    
    DeleteContents();
    
    // Make sure we are not simulating.
    m_pkSimFetchSection->Lock();
    
    NiPhysXManager::GetPhysXManager()->Shutdown();
    NiDelete m_pkOutputObject;
    
    m_pkSimFetchSection->Unlock();
    
    m_aSequences = 0;

    ClearForces(); // Deletes everything associated with forces
    m_pkForceSets = 0;
    
    NiExternalDelete m_pkSimFetchSection;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Deletes anything we're holding
void MPhysics::DeleteContents()
{
    if (!Present)
        return;

    // Make sure we are not simulating.
    m_pkSimFetchSection->Lock();

    m_aSequences->Clear();

    RemoveGroundPlane();
    ClearForces();
    
    if (m_pkPhysicsActor)
    {
        NiDelete m_pkPhysicsActor;
        m_pkPhysicsActor = 0;
    }
    
    if (m_spPhysicsScene)
    {
        m_spPhysicsScene->DecRefCount();
        m_spPhysicsScene = 0;
    }
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Returns true if the Physics SDK has been successfully initialized,
// false otherwise. The functions in MPhysics will always do their
// own checking for a valid PhysX scene, but other application code
// may also want to know if physics is available (to control UI
// elements, for instance).
//---------------------------------------------------------------------------
bool MPhysics::get_Present()
{
    return NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK != 0;
}
//---------------------------------------------------------------------------
bool MPhysics::get_ScenePresent()
{
    return m_spPhysicsScene != 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Takes a time and performs any steps required before scene graph
// updating. This includes calling update sources on the physics
// scene, applying forces, simulating, getting the results, and
// updating destinations.
//---------------------------------------------------------------------------
void MPhysics::UpdateSources(float fT)
{
    if (!m_spPhysicsScene)
        return;

    m_pkSimFetchSection->Lock();
    m_spPhysicsScene->UpdateSources(fT);
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::SimulateStep(float fT)
{
    if (!m_spPhysicsScene)
        return;

    m_pkSimFetchSection->Lock();
    ApplyForce(fT);
    
    if (m_spPhysicsScene->Simulate(fT))
    {
        m_spPhysicsScene->FetchResults(fT, true);
    }
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::UpdateDestinations(float fT)
{
    if (!m_spPhysicsScene)
        return;

    m_pkSimFetchSection->Lock();
    m_spPhysicsScene->UpdateDestinations(fT);
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Resets the physics system back to its initial conditions. The
// only argument is the time to which the simulation fetch time
// should be set. This is the start time for Update calls that follow
// this call. Not all physics object need support this kind of reset, in
// which case they will remain in their existing state.
//---------------------------------------------------------------------------
void MPhysics::ResetPhysics(const NiFixedString& kStateName, float fT)
{
    // Iterate over all props, restoring from snapshot 0
    if (!m_spPhysicsScene)
        return;

    // Every time PhysX is reset we also must reset accumulation, because that
    // is the only way we can reasonably ensure that PhysX state matches
    // Gamebryo state. It is the caller's responsibility to ensure this is
    // true (to avoid wasteful multiple calls).

    m_pkSimFetchSection->Lock();

    ResetForces();
    m_spPhysicsScene->ClearDebugGeometry();

    m_spPhysicsScene->RestoreSnapshotState(kStateName);
    m_spPhysicsScene->SetFetchTime(fT);
    m_spPhysicsScene->UpdateDestinations(fT, true);
    m_spPhysicsScene->UpdateSources(fT, true);
    
    m_fResetTime = fT;
    
    // Release the locks
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Reset the internal physics time, but do not reset the state.
//---------------------------------------------------------------------------
void MPhysics::ResetTime(float fT)
{
    if (!m_spPhysicsScene)
        return;

    m_pkSimFetchSection->Lock();

    ResetForces();

    m_spPhysicsScene->SetFetchTime(fT);
    m_spPhysicsScene->UpdateDestinations(fT, true);
    m_spPhysicsScene->UpdateSources(fT, true);

    m_fResetTime = fT;
    
    // Release the locks
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Adds a sequence to physics control. This will add the sequence
// to the list of managed sequences and to the NiPhysXSequenceManager
// object. It will also do any set up required for management, and
// send out an event. Returns true if the sequence was successfully
// added.
//---------------------------------------------------------------------------
bool MPhysics::AddSequence(MSequence* pkSequence)
{
    if (!m_spPhysicsScene)
        return false;

    m_pkSimFetchSection->Lock();

    if (!m_pkPhysicsActor->
        MakeSequencePhysical(pkSequence->GetSequenceData()))
    {
        m_pkSimFetchSection->Unlock();
        return false;
    }

    m_pkSimFetchSection->Unlock();
    
    m_aSequences->Add(pkSequence);

    NeedToSave = true;

    OnAddPhysicsSequence(pkSequence);
    return true;
}
//---------------------------------------------------------------------------
bool MPhysics::RemoveSequence(MSequence* pkSequence)
{
    if (!m_spPhysicsScene)
        return false;

    m_pkSimFetchSection->Lock();

    if (!m_pkPhysicsActor->
        RevertPhysicalSequence(pkSequence->GetSequenceData()))
    {
        m_pkSimFetchSection->Unlock();
        return false;
    }

    m_pkSimFetchSection->Unlock();
    
    m_aSequences->Remove(pkSequence);

    NeedToSave = true;

    OnRemovePhysicsSequence(pkSequence);
    
    return true;
}
//---------------------------------------------------------------------------
bool MPhysics::IsSequencePhysical(MSequence* pkSequence)
{
    if (!m_aSequences)
        return false;
        
    return m_aSequences->Contains(pkSequence);
}
//---------------------------------------------------------------------------
bool MPhysics::IsSequenceGroupPhysical(MSequenceGroup* pkSeqGroup)
{
    if (!pkSeqGroup || !m_aSequences)
        return false;
    
    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSequence =
            dynamic_cast<MSequence*>(m_aSequences->get_Item(i));
        if (pkSeqGroup->GetSequenceInfoIndex(pkSequence->SequenceID) != -1)
            return true;
    }
       
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Called when a new NIF file is loaded, this function finds all the
// physics content in the file and adds it to the scene so that it
// will be simulated. The function should be called after the actor
// manager has operated on the file, so that it can add the controller
// manager to the NiPhysXSequenceManager object.
bool MPhysics::ProcessStream(NiStream* pkStream)
{
    // Make sure we are not simulating.
    m_pkSimFetchSection->Lock();

    bool bHadGroundPlane = m_pkGroundActor != 0;    
    if (bHadGroundPlane)
        RemoveGroundPlane();
    ClearForces();
    
    if (m_pkPhysicsActor)
    {
        NiDelete m_pkPhysicsActor;
        m_pkPhysicsActor = 0;
    }
    
    if (m_spPhysicsScene)
    {
        m_spPhysicsScene->DecRefCount();
        m_spPhysicsScene = 0;
    }

    NiTObjectArray<NiPhysXPropPtr> kProps;
    for (unsigned int ui = 0; ui < pkStream->GetObjectCount(); ui++)
    {
        NiObject *pkObject;

        pkObject = pkStream->GetObjectAt(ui);

        if (ui == 0)
        {
            NIASSERT(NiIsKindOf(NiNode, pkObject));
            m_pkDebugParent = (NiNode*) pkObject;
        }
        
        if (NiIsKindOf(NiPhysXProp, pkObject))
        {
            NiPhysXProp* pkProp = (NiPhysXProp*)pkObject;
            kProps.Add(pkProp);
            pkProp->SetAllDestInterp(true);
            pkProp->SetAllSrcInterp(true);
            
            // Over-ride keep flags.
            pkProp->GetSnapshot()->SetKeepAllStates(true);
        }
    }
    
    // Create the PhysX content
    if (!CreatePhysX(kProps))
    {
        kProps.RemoveAll();
        if (m_spPhysicsScene)
            m_spPhysicsScene->DecRefCount();
        m_spPhysicsScene = 0;
        m_pkSimFetchSection->Unlock();
        return false;
    }
    kProps.RemoveAll();
       
    if (bHadGroundPlane)
        AddGroundPlane();

    m_pkSimFetchSection->Unlock();
    return true;
}
//---------------------------------------------------------------------------
bool MPhysics::CreatePhysX(NiTObjectArray<NiPhysXPropPtr>& kProps)
{
    NiPhysXManager* pkPhysicsManager = NiPhysXManager::GetPhysXManager();
    
    // check for hardware
    bool bHaveHardware = pkPhysicsManager->m_pkPhysXSDK->getHWVersion() > 0;
    
    // Do we need hardware? Do we have fluids?
    bool bNeedHardware = false;
    bool bWantHardware = false;

    TraverseProps(kProps, bNeedHardware, bWantHardware);
    
    if (bNeedHardware && !bHaveHardware)
    {
        const char* pcWarning =
            "Fluid scene detected, but no hardware found.\n"
            "Physics simulation may be very slow.";
        MessageBox(NULL, pcWarning, "Fluid Scene without hardware",
            MB_OK | MB_ICONERROR);
        MLogger::LogWarning(pcWarning);
    }

    if (kProps.GetSize())
    {
        NxSceneDesc kNxSceneDesc;
        kNxSceneDesc.simType =
            bWantHardware ? NX_SIMULATION_HW : NX_SIMULATION_SW;
        kNxSceneDesc.gravity.set(0.0f, 0.0f, -9.8f);

        NiPhysXProp* pkProp = kProps.GetAt(0);
        
        m_spPhysicsScene = NiNew NiPhysXScene();
        m_spPhysicsScene->IncRefCount();
        
        m_spPhysicsScene->SetScaleFactor(pkProp->GetScaleFactor());
        
        this->DebugState = m_bDebugState;

        NxScene* pkNxScene =
            pkPhysicsManager->m_pkPhysXSDK->createScene(kNxSceneDesc);
        if (!pkNxScene && kNxSceneDesc.simType == NX_SIMULATION_HW)
        {
            kNxSceneDesc.simType = NX_SIMULATION_SW;
            pkNxScene =
                pkPhysicsManager->m_pkPhysXSDK->createScene(kNxSceneDesc);
            const char* pcWarning =
                "Could not create hardware scene.\n"
                "Switching to software scene.\n";
            MLogger::LogWarning(pcWarning);
            bHaveHardware = false;
        }
        
        if (!pkNxScene)
        {
            const char* pcWarning = "Unable to create PhysX scene.\n";
            MLogger::LogWarning(pcWarning);
            return false;
        }
        
        m_spPhysicsScene->SetPhysXScene(pkNxScene);
    }
    
    for (unsigned int ui = 0; ui < kProps.GetSize(); ui++)
    {
        NiPhysXProp* pkProp = kProps.GetAt(ui);
        unsigned int uiIndex = m_spPhysicsScene->AddProp(pkProp);
        if (uiIndex == (unsigned int)-1)
        {
            const char* pcWarning = "Unable to create PhysX prop.\n";
            MLogger::LogWarning(pcWarning);
            return false;
        }
    }
    
    if (m_spPhysicsScene)
    {
        m_spPhysicsScene->SetDoFixedStep(true);
        m_spPhysicsScene->SetTimestep(1.0f / 100.0f);
        m_spPhysicsScene->SetNumSubSteps(1);
        m_spPhysicsScene->SetMaxSubSteps(10);
        m_spPhysicsScene->SetUpdateDest(true);
        m_spPhysicsScene->SetUpdateSrc(true);

        NxScene* pkNxScene = m_spPhysicsScene->GetPhysXScene();
        if (!pkNxScene)
        {
            const char* pcWarning = "Unable to create PhysX scene.\n";
            MLogger::LogWarning(pcWarning);
            return false;
        }
        NxMaterial* pkMaterial = pkNxScene->getMaterialFromIndex(0);
        pkMaterial->setStaticFriction(m_fGroundFriction);
        pkMaterial->setDynamicFriction(m_fGroundFriction);
        pkMaterial->setRestitution(m_fGroundBounce);
    }
    
    return true;
}        
//---------------------------------------------------------------------------
void MPhysics::TraverseProps(NiTObjectArray<NiPhysXPropPtr>& kProps,
    bool& bNeedHardware, bool& bWantHardware)
{
    bNeedHardware = false;
    bWantHardware = false;
    
    for (unsigned int ui = 0; ui < kProps.GetSize(); ui++)
    {
        NiPhysXProp* pkProp = kProps.GetAt(ui);
        if (!pkProp)
            continue;
            
        NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();
        if (!pkPropDesc)
            continue;
        
        bWantHardware = bWantHardware || pkPropDesc->GetUseHardware();
        
        // Put everything in the default compartment. We don't have enough
        // information to do anything else
        pkPropDesc->SetRBCompartmentID(0);
        pkPropDesc->SetClothCompartmentID(0);
        pkPropDesc->SetFluidCompartmentID(0);

        unsigned int uiNumActors = pkPropDesc->GetActorCount();
        for (unsigned int uj = 0; uj < uiNumActors; uj++)
        {
            NiPhysXActorDesc* pkActor = pkPropDesc->GetActorAt(uj);
            
            // Work through shapes looking for meshes cooked for hardware
            NiTObjectArray<NiPhysXShapeDescPtr>&
                kShapes = pkActor->GetActorShapes();
            for (unsigned int uk = 0; uk < kShapes.GetSize(); uk++)
            {
                NiPhysXShapeDesc* pkShape = kShapes.GetAt(uk);
                if (!pkShape)
                    continue;
                NiPhysXMeshDesc* pkMesh = pkShape->GetMeshDesc();
                if (!pkMesh)
                    continue;
                bWantHardware = bWantHardware || pkMesh->GetHardware();
            }
        }

        unsigned int uiNumClothes = pkPropDesc->GetClothCount();
        for (unsigned int uj = 0; uj < uiNumClothes; uj++)
        {
            NiPhysXClothDesc* pkCloth = pkPropDesc->GetClothAt(uj);
            
            // Look for the hardware flag
            if (pkCloth->GetClothFlags() & NX_CLF_HARDWARE)
                bNeedHardware = true;
        }

        if (NiIsKindOf(NiPhysXFluidPropDesc, pkPropDesc))
        {
            bNeedHardware = true;
        }
    }
    
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Called when all the sequences in an actor manager have been set up,
// or when the NIF file has been reloaded, this sets up the sequences
// for physics control.
//---------------------------------------------------------------------------
bool MPhysics::AttachPhysics(NiControllerManager* pkController)
{
    if (!m_spPhysicsScene)
        return false;

    m_pkSimFetchSection->Lock();

    // We have to assume that the first prop is the one with the
    // character on it.
    NiPhysXProp* pkProp = m_spPhysicsScene->GetPropAt(0);

    if (!m_pkPhysicsActor)
        m_pkPhysicsActor = NiNew NiPhysXSequenceManager;
    m_pkPhysicsActor->SetManager(pkController, pkProp);

    // Add all the physical sequences we know about.
    if (m_aSequences)
    {
        for (int i = 0; i < m_aSequences->Count; i++)
        {
            MSequence* pkSequence =
                dynamic_cast<MSequence*>(m_aSequences->get_Item(i));
            if (!m_pkPhysicsActor->
                MakeSequencePhysical(pkSequence->GetSequenceData()))
            {
                m_pkSimFetchSection->Unlock();
                return false;
            }
        }
    }

    m_pkSimFetchSection->Unlock();

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Called when an individual KF file is loaded and the sequences in it need
// to be loaded separately
//---------------------------------------------------------------------------
bool MPhysics::AddKeySequence(NiSequenceData* pkSeqData)
{
    if (!m_spPhysicsScene)
        return false;

    m_pkSimFetchSection->Lock();

    if (!m_pkPhysicsActor->AddSequence(pkSeqData))
    {
        m_pkSimFetchSection->Unlock();
        return false;
    }

    m_pkSimFetchSection->Unlock();
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Ground plane functions
//---------------------------------------------------------------------------
// Creates a ground plane object. Raises an event, or does nothing if
// the plane already exists. Return value is true if the plane was
// added.
//---------------------------------------------------------------------------
bool MPhysics::AddGroundPlane()
{
    if (m_pkGroundActor)
        return false;

    if (!m_spPhysicsScene)
        return false;

    MAssert(m_spPhysicsScene->GetPhysXScene() != 0);
    if (!CreateGroundActor())
        return false;
        
    return true;
}
//---------------------------------------------------------------------------
// Removes the ground plane. Raises an event. Returns true if the
// plane was removed. False if was already removed or something
// went wrong.
//---------------------------------------------------------------------------
bool MPhysics::RemoveGroundPlane()
{
    if (!m_pkGroundActor)
        return false;

    RemoveGroundActor();
        
    return true;
}
//---------------------------------------------------------------------------
// Adjust the height of the ground plane. This copies the state from
// the ground plane object in the scene.
//---------------------------------------------------------------------------
bool MPhysics::AdjustGroundPlane()
{
    if (!m_pkGroundActor)
        return false;

    // We need to change the size of the ground plane shape to match the
    // scaled Gamebryo ground plane.
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    float fScale =
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX)->GetWorldScale();
    pkSharedData->Unlock();    

    // Get the shape
    NxBoxShape* pkShape = m_pkGroundActor->getShapes()[0]->isBox();
    NIASSERT(pkShape != 0);
    NxVec3 kDimensions(
        50.0f * fScale / m_spPhysicsScene->GetScaleFactor(),
        50.0f * fScale / m_spPhysicsScene->GetScaleFactor(),
        50.0f * fScale / m_spPhysicsScene->GetScaleFactor());
    pkShape->setDimensions(kDimensions);
    NxVec3 kPosition(0.0f, 0.0f, -kDimensions.z);
    pkShape->setLocalPosition(kPosition);

    return true;  
}
//---------------------------------------------------------------------------
// Computes the pose matrix for the ground plane.
//---------------------------------------------------------------------------
bool MPhysics::CreateGroundActor()
{
    // Create the actor. If the ground plane NIF ever changes, this
    // should also be changed to reflect the size of the new ground plane.
    NxBoxShapeDesc kBoxDesc;
    kBoxDesc.dimensions.set(
        50.0f / m_spPhysicsScene->GetScaleFactor(),
        50.0f / m_spPhysicsScene->GetScaleFactor(),
        50.0f / m_spPhysicsScene->GetScaleFactor());
    kBoxDesc.localPose.t.set(
        0.0f, 0.0f, -50.0f / m_spPhysicsScene->GetScaleFactor());
    
    NxBodyDesc kBodyDesc;
    kBodyDesc.flags |= NX_BF_KINEMATIC;
    kBodyDesc.mass = 1.0f;
    
    NxActorDesc kActorDesc;
    kActorDesc.body = &kBodyDesc;
    kActorDesc.shapes.pushBack(&kBoxDesc);
    kActorDesc.name = "AnimationToolGroundPlane";
    
    // Get the pose
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiAVObject* pkBackground =
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);
    
    NiPoint3 kTranslate = pkBackground->GetWorldTranslate();
    NiMatrix3 kRotate = pkBackground->GetWorldRotate();

    NiPhysXTypes::NiMatrix3ToNxMat33(kRotate, kActorDesc.globalPose.M);
    NiPhysXTypes::NiPoint3ToNxVec3(kTranslate, kActorDesc.globalPose.t);
    kActorDesc.globalPose.t /= m_spPhysicsScene->GetScaleFactor();
    
    pkSharedData->Unlock();

    NIASSERT(kActorDesc.isValid());

    m_pkSimFetchSection->Lock();
    
    m_pkGroundActor =
        m_spPhysicsScene->GetPhysXScene()->createActor(kActorDesc);
    
    if (m_pkGroundActor != NULL)
    {
        m_spGroundActorSrc =
            NiNew NiPhysXDynamicSrc(pkBackground, m_pkGroundActor);
        m_spGroundActorSrc->IncRefCount();

        m_spPhysicsScene->AddSource(m_spGroundActorSrc);
        m_spGroundActorSrc->SetActive(true);
    }

    m_pkSimFetchSection->Unlock();
    
    return m_pkGroundActor != NULL;
}
//---------------------------------------------------------------------------
void MPhysics::RemoveGroundActor()
{
    m_pkSimFetchSection->Lock();
        
    // If we have a ground plane, we must have a scene.
    MAssert(m_spPhysicsScene->GetPhysXScene() != 0);
    m_spPhysicsScene->GetPhysXScene()->releaseActor(*m_pkGroundActor);
    m_pkGroundActor = 0;
    
    m_spPhysicsScene->DeleteSource(m_spGroundActorSrc);
    m_spGroundActorSrc->DecRefCount();
    m_spGroundActorSrc = 0;

    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Force functions
//---------------------------------------------------------------------------
// Initiates a force recording sequence. This clears any existing
// data. A ray in world space is given, and this is intersected with
// the physics scene to find an actor that is hit and its hit point.
// If an actor is hit, returns true, otherwise false.
//---------------------------------------------------------------------------
bool MPhysics::InitiateForces(NiPoint3& kOrigin, NiPoint3& kDir)
{
    // If there's not a PhysX scene, we can't pick any actor.
    if (!m_spPhysicsScene)
        return false;

    m_pkSimFetchSection->Lock();

    // Close out any existing force. In theory, this expression should never
    // be true, but better safethan sorry.
    if (m_pkForceTarget)
    {
        FinishForce();
    }

    // Ray is in PhysX space, which is the same as Gamebryo World space
    NxScene* pkScene = m_spPhysicsScene->GetPhysXScene();
    NxRay kRay;
    NiPhysXTypes::NiPoint3ToNxVec3(kOrigin, kRay.orig);
    kRay.orig /= m_spPhysicsScene->GetScaleFactor();
    NiPhysXTypes::NiPoint3ToNxVec3(kDir, kRay.dir);

    // Check for an actor under the mouse.
    NxRaycastHit kHitInfo;
    NxShape* pkHitShape;
    float fDistance = 1.0e20f;
    bool bHitSomething = false;
    pkHitShape =
        pkScene->raycastClosestShape(kRay, NX_DYNAMIC_SHAPES, kHitInfo);
    if (pkHitShape)
    {
        // Find the actor
        NxActor& kActor = pkHitShape->getActor();
        if (!kActor.readBodyFlag(NX_BF_KINEMATIC))
        {
            fDistance = kHitInfo.distance;

            NxMat34 kActorPose = kActor.getGlobalPose();
            NxVec3 kPoint;
            kActorPose.multiplyByInverseRT(kHitInfo.worldImpact, kPoint);

            // raycastClosestShape returns just a single shape, so if we got
            // any hit we know it's the best candidate for forces.
            m_pkForceTarget = NiExternalNew ForceSet(
                &kActor, 0, kPoint.x, kPoint.y, kPoint.z, 0);
        }
    }
    
    // Check for cloth under the mouse
    unsigned int uiNumCloth = pkScene->getNbCloths();
    NxCloth** ppkCloth = pkScene->getCloths();
    for (unsigned int ui = 0; ui < uiNumCloth; ui++)
    {
        NxCloth* pkCloth = ppkCloth[ui];
        NxVec3 kHit;
        NxU32 uiID;
        if (pkCloth->raycast(kRay, kHit, uiID))
        {
            float fThisDist = kHit.distance(kRay.orig);
            if (!bHitSomething || fThisDist < fDistance)
            {
                if (m_pkForceTarget)
                {
                    // Hit an actor above, or have hit some other cloth.
                    // Switch existing target to use this cloth instead.
                    m_pkForceTarget->m_pkActor = 0;
                    m_pkForceTarget->m_pkCloth = pkCloth;
                    m_pkForceTarget->m_uiVertex = uiID;
                }
                else
                {
                    // Haven't hit anything before
                    m_pkForceTarget = NiExternalNew ForceSet(
                        0, pkCloth, 0.0f, 0.0f, 0.0f, uiID);
                }
                fDistance = fThisDist;
            }
        }
    }

    m_pkSimFetchSection->Unlock();
    
    return m_pkForceTarget != 0; // m_pkForceTarget only non-null if we hit
}   
//---------------------------------------------------------------------------
// Records a force at a given time and magnitude.
// Input is the world space force.
//---------------------------------------------------------------------------
void MPhysics::AddForce(float fT, NiPoint3& kForce)
{
    if (!m_pkForceTarget) // Can only add a force if we have a target
        return;

    m_pkSimFetchSection->Lock();
    
    // Remove the time offsets from when phsyics started
    fT -= m_fResetTime;
    
    // Scale the force by non-object-specific factors
    float fScale = FORCE_SPRING_CONST
        * NiPow(2.0f, m_fForceExponent) / m_spPhysicsScene->GetScaleFactor();
    kForce *= fScale;

    // Add the force to the existing target
    m_pkForceTarget->AddForce(fT, kForce);
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
// Finish off a force sequence.
//---------------------------------------------------------------------------
void MPhysics::FinishForce()
{
    if (!m_pkForceTarget)
        return; // No active force to finish

    m_pkSimFetchSection->Lock();

    // Check for force sets with no data. The constructor for the set
    // sets the end time to be before the start time, but the first piece
    // of data changes it.
    if (m_pkForceTarget->m_fEnd < m_pkForceTarget->m_fStart)
    {
        m_pkForceTarget = 0;
        m_pkSimFetchSection->Unlock();
        return;
    }

    // Now we need to insert the new set, the m_pkForceTarget, into the
    // ordered array of force sets. There are two invariants on the array:
    // * Force sets in the array are ordered on their start time.
    // * Force sets in the array are strictly non-intersecting in time. So
    //   one force set in the array must end before the next force set in
    //   the array begins.

    // Check for no existing force sets
    if (m_pkForceSets->Count == 0)
    {
        // No existing forces. Add to end.
        m_pkForceSets->Add(m_pkForceTarget);

        m_pkForceTarget = 0;
        m_pkSimFetchSection->Unlock();
        return;
    }

    // Figure out where to insert it into the force sets. The strategy is to
    // first determine where in the existign set of forces the new one should
    // go, based on its start time. Then deal with modifying the existing
    // sets to avoid overlaps in time.
    
    // We want the index of the existing force set that should be before the
    // new one in the array (or -1 if it's first) and the index of the set
    // that comes after (or -1 if it's last)
    int iBefore = 0;
    int iAfter = m_pkForceSets->Count - 1;
    ForceSet* pkBefore =
        dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(iBefore));
    ForceSet* pkAfter =
        dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(iAfter));
    
    if (pkAfter->m_fStart < m_pkForceTarget->m_fStart)
    {
        // Last force set starts strictly earlier than the new one. So the new
        // one goes at the end.
        iAfter = -1;
        iBefore = m_pkForceSets->Count - 1;
    }
    else if (pkBefore->m_fStart >= m_pkForceTarget->m_fStart)
    {
        // First force set starts later or at the same time as the new one.
        // So the new one goes at the beginning. Note that this maintains the
        // invariant that the sort order is strictly increasing - later we
        // will deal with sets that start at the same time, but we want them
        // to appear AFTER the new set.
        iAfter = 0;
        iBefore = -1;
    }
    else
    {
        // Right now we know that we have at least before and after and
        // they are not the same thing (otherwise one of the cases immediately
        // above would have hit). Binary search to bound the target index.
        NIASSERT(iAfter - iBefore > 0);
        
        while (iAfter - iBefore > 1)
        {
            int iMid = (iAfter + iBefore) >> 1;
            ForceSet* pkMid =
                dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(iMid));
            if (pkMid->m_fStart >= m_pkForceTarget->m_fStart)
            {
                // Note equality test ensures that equal start time sets will
                // appear after the new set.
                iAfter = iMid;
                pkAfter = pkMid;
            }
            else
            {
                iBefore = iMid;
                pkBefore = pkMid;
            }
        }
    }

    // At this stage, we need to check if any of the existing force
    // sets must be removed, truncated or broken into two pieces.
    
    // There can be only one set that comes before and overlaps, because we
    // started with non-intersecting sets
    if (iBefore >= 0 && pkBefore->m_fEnd >= m_pkForceTarget->m_fStart)
    {
        // The set before finishes after this set starts, so we need to
        // either truncate it or split it into two pieces. Note the equality
        // test above: we want to truncate if the end time of the previous
        // set if the same as the start time of this set, to maintain strict
        // non-intersection.
        if (pkBefore->m_fEnd > m_pkForceTarget->m_fEnd)
        {
            // The previous set ends after the new one ends, so we split
            // it into two pieces: the part that comes before the new one, and
            // the part that comes after.
            ForceSet* pkNewChunk = pkBefore->Split(m_pkForceTarget->m_fStart,
                m_pkForceTarget->m_fEnd);
                
            // The part that comes after is inserted after the new chunk.
            // That is, in the old iAfter location.
            if (iAfter >= 0)
            {
                m_pkForceSets->Insert(iAfter, pkNewChunk);
            }
            else
            {
                m_pkForceSets->Add(pkNewChunk);
                iAfter = m_pkForceSets->Count - 1;
            }
            pkAfter = pkNewChunk;
        }
        else
        {
            // The previous set ends before the new one ends, so we truncate
            pkBefore->TruncateEnd(m_pkForceTarget->m_fStart);
        }
    }

    // There could be many sets that come after and overlap, because we don't
    // know how long the new set is.
    while (iAfter >= 0 && pkAfter->m_fEnd <= m_pkForceTarget->m_fEnd)
    {
        // A set is removed if it starts after the new set and finishes before
        // the new set finishes.
        m_pkForceSets->RemoveAt(iAfter);
        if (iAfter == m_pkForceSets->Count)
        {
            iAfter = -1;
        }
        else
        {
            pkAfter = dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(iAfter));
        }
    }

    // A set is truncated if it starts before this one ends. There can be
    // only one of these thanks to the non-overlap invariant.
    if (iAfter >= 0 && pkAfter->m_fStart <= m_pkForceTarget->m_fEnd)
    {
        pkAfter->TruncateStart(m_pkForceTarget->m_fEnd);
    }
    
    // Insert at the after location
    if (iAfter < 0)        
        m_pkForceSets->Add(m_pkForceTarget);
    else
        m_pkForceSets->Insert(iAfter, m_pkForceTarget);
    m_pkForceTarget = 0;

    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
// Given a time, interpolates the forces for that time and
// applies the resulting force to the actor. If the time is
// before or after the range of forces stored, then the force is
// zero. Returns true if a force was applied. Assume this is called from
// inside the critical section for access to the SDK.
//---------------------------------------------------------------------------
bool MPhysics::ApplyForce(float fT)
{
    fT -= m_fResetTime;
    
    // If we are currently gathering forces, use those on preference to
    // any stored forces.
    if (m_pkForceTarget)
    {
        return m_pkForceTarget->ApplyForce(fT);
    }
    
    // Simplest is no existing forces
    int iExistingCount = m_pkForceSets->Count;
    if (iExistingCount == 0)
    {
        return false;
    }

    // Check if we are before the earliest set
    ForceSet* pkForceSet = dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(0));
    if (fT < pkForceSet->m_fStart)
    {
        return false;
    }

    // Check for after the latest set
    pkForceSet =
        dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(iExistingCount - 1));
    if (fT > pkForceSet->m_fEnd)
    {
        return false;
    }
    
    // m_uiLastForceSetUsed is a hint as to where we will find the right
    // force set. The assumption is that the forces will be applied in
    // in order as time passes, so the last set we used is probably the same
    // as the set we need now, or at least close.
    unsigned int uiSetIndex = m_uiLastForceSetUsed;
    if (uiSetIndex >= (unsigned int)iExistingCount)
        uiSetIndex = (unsigned int)iExistingCount - 1;
    pkForceSet = dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(uiSetIndex));
    while (fT < pkForceSet->m_fStart)
    {
        // Search down for a set that starts before or equal to the time.
        // We know one exists because we did not exit early above.
        uiSetIndex -= 1;
        pkForceSet =
            dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(uiSetIndex));
    }
    while (fT > pkForceSet->m_fEnd)
    {
        // Search up for a set that starts before or equal to the time.
        // We know one exists because we did not exit early above.
        uiSetIndex += 1;
        pkForceSet =
            dynamic_cast<ForceSet*>(m_pkForceSets->get_Item(uiSetIndex));
    }
    if (pkForceSet->m_fStart > fT || pkForceSet->m_fEnd < fT)
    {
        // This deals with the case where the time is between that of
        // two sets.
        m_uiLastForceSetUsed = uiSetIndex;
        return false;
    }
    
    // We know that the time is within the range for this force set.
    pkForceSet->ApplyForce(fT);
    
    m_uiLastForceSetUsed = uiSetIndex;

    return true;
}
//---------------------------------------------------------------------------
// Clears all recorded data and sets the actor to 0.
//---------------------------------------------------------------------------
void MPhysics::ClearForces()
{
    if (m_pkForceTarget)
        FinishForce();
    
    m_pkForceSets->Clear();
    
    m_uiLastForceSetUsed = 0;
}
//---------------------------------------------------------------------------
// Reset the index for force application back to the beginning
//---------------------------------------------------------------------------
void MPhysics::ResetForces()
{
    // Put a stop to any drag sequence. This handles the situation in
    // which a tranition preview loops while the user is dragging.
    if (m_pkForceTarget)
        FinishForce();

    m_uiLastForceSetUsed = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties. Get functions.
//---------------------------------------------------------------------------
float MPhysics::get_GravityX()
{
    return m_fGravityX;
}
//---------------------------------------------------------------------------
float MPhysics::get_GravityY()
{
    return m_fGravityY;
}
//---------------------------------------------------------------------------
float MPhysics::get_GravityZ()
{
    return m_fGravityZ;
}
//---------------------------------------------------------------------------
float MPhysics::get_TimeStepLength()
{
    return m_fTimestep;
}
//---------------------------------------------------------------------------
bool MPhysics::get_DebugState()
{
    return m_bDebugState;
}
//---------------------------------------------------------------------------
float MPhysics::get_DebugScale()
{
    return m_fDebugScale;
}
//---------------------------------------------------------------------------
float MPhysics::get_GroundFriction()
{
    return m_fGroundFriction;
}
//---------------------------------------------------------------------------
float MPhysics::get_GroundBounce()
{
    return m_fGroundBounce;
}
//---------------------------------------------------------------------------
float MPhysics::get_ForceExponent()
{
    return m_fForceExponent;
}
//---------------------------------------------------------------------------
ArrayList* MPhysics::get_PhysicalSequences()
{
    return m_aSequences;
}
//---------------------------------------------------------------------------
bool MPhysics::get_NeedToSave()
{
    return m_bNeedToSave;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties. Set functions.
//---------------------------------------------------------------------------
void MPhysics::set_GravityX(float fX)
{
    m_fGravityX = fX;
    
    if (!m_spPhysicsScene)
        return;
    
    // This operation must not be done while doing in a sim/fetch block
    m_pkSimFetchSection->Lock();
        
    NxVec3 kVec(m_fGravityX, m_fGravityY, m_fGravityZ);        
    m_spPhysicsScene->GetPhysXScene()->setGravity(kVec);
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_GravityY(float fY)
{
    m_fGravityY = fY;
    
    if (!m_spPhysicsScene)
        return;
    
    // This operation must not be done while doing in a sim/fetch block
    m_pkSimFetchSection->Lock();
        
    NxVec3 kVec(m_fGravityX, m_fGravityY, m_fGravityZ);        
    m_spPhysicsScene->GetPhysXScene()->setGravity(kVec);
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_GravityZ(float fZ)
{
    m_fGravityZ = fZ;
    
    if (!m_spPhysicsScene)
        return;
    
    // This operation must not be done while doing in a sim/fetch block
    m_pkSimFetchSection->Lock();
        
    NxVec3 kVec(m_fGravityX, m_fGravityY, m_fGravityZ);        
    m_spPhysicsScene->GetPhysXScene()->setGravity(kVec);
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_TimeStepLength(float fStep)
{
    m_fTimestep = fStep;
    
    if (!m_spPhysicsScene)
        return;
    
    // This operation must not be done while doing in a sim/fetch block
    m_pkSimFetchSection->Lock();
        
    m_spPhysicsScene->SetTimestep(fStep);
    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_DebugState(bool bState)
{
    m_bDebugState = bState;
    
    if (!m_spPhysicsScene)
        return;
        
    m_pkSimFetchSection->Lock();

    NIASSERT(m_pkDebugParent);
    m_spPhysicsScene->SetDebugRender(m_bDebugState, m_pkDebugParent);

    if (bState)
    {
        NiPhysXManager::GetPhysXManager()->
            m_pkPhysXSDK->setParameter(NX_VISUALIZATION_SCALE, m_fDebugScale);
    }
    else
    {
        NiPhysXManager::GetPhysXManager()->
            m_pkPhysXSDK->setParameter(NX_VISUALIZATION_SCALE, 0.0f);
    }    
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_DebugScale(float fScale)
{
    m_fDebugScale = fScale;
    
    if (!m_spPhysicsScene)
        return;
        
    m_pkSimFetchSection->Lock();

    if (m_spPhysicsScene->GetDebugRender())
    {
        NiPhysXManager::GetPhysXManager()->
            m_pkPhysXSDK->setParameter(NX_VISUALIZATION_SCALE, m_fDebugScale);
    }
 
    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_GroundFriction(float fValue)
{
    m_fGroundFriction = fValue;
    
    if (!m_spPhysicsScene)
        return;
        
    m_pkSimFetchSection->Lock();

    NxScene* pkScene = m_spPhysicsScene->GetPhysXScene();
    NxMaterial* pkMaterial = pkScene->getMaterialFromIndex(0);
    pkMaterial->setStaticFriction(m_fGroundFriction);
    pkMaterial->setDynamicFriction(m_fGroundFriction);
    m_pkGroundActor->getShapes()[0]->setMaterial(0);

    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_GroundBounce(float fValue)
{
    m_fGroundBounce = fValue;
    
    if (!m_spPhysicsScene)
        return;
        
    m_pkSimFetchSection->Lock();

    NxScene* pkScene = m_spPhysicsScene->GetPhysXScene();
    NxMaterial* pkMaterial = pkScene->getMaterialFromIndex(0);
    pkMaterial->setRestitution(m_fGroundBounce);
    m_pkGroundActor->getShapes()[0]->setMaterial(0);

    m_pkSimFetchSection->Unlock();
}
//---------------------------------------------------------------------------
void MPhysics::set_ForceExponent(float fExp)
{
    m_fForceExponent = fExp;
    NeedToSave = true;
}
//---------------------------------------------------------------------------
void MPhysics::set_NeedToSave(bool bNeedToSave)
{
    m_bNeedToSave = bNeedToSave;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Force storage classes, constructors
//---------------------------------------------------------------------------
MPhysics::Force::Force(float fT, const NiPoint3& kForce)
{
    m_fT = fT;
    m_fForceX = kForce.x;
    m_fForceY = kForce.y;
    m_fForceZ = kForce.z;
}
//---------------------------------------------------------------------------
MPhysics::ForceSet::ForceSet(NxActor* pkActor, NxCloth* pkCloth,
    const float fPointX, const float fPointY, const float fPointZ,
    const unsigned int uiVertex)
{
    m_pkForces = NiExternalNew ArrayList;

    m_fStart = 1.0e20f; // Start is after end in the constructed object
    m_fEnd = -1.0e20f;

    m_pkActor = pkActor;
    m_pkCloth = pkCloth;

    m_fPointX = fPointX;
    m_fPointY = fPointY;
    m_fPointZ = fPointZ;
    m_uiVertex = uiVertex;
    
    m_uiLastForceUsed = 0;
}
//---------------------------------------------------------------------------
MPhysics::ForceSet::~ForceSet()
{
    m_pkForces = 0;
}
//---------------------------------------------------------------------------
void MPhysics::ForceSet::AddForce(float fT, NiPoint3& kForce)
{
    // Assumes forces are added in increasing time order, which seems
    // reasonable given the that user is doing a mouse drag and we explicitly
    // finish any force application whenever time makes a jump.

    // Apparently we can register multiple forces at the same time, so only
    // keep the first one
    int iCount = m_pkForces->Count;
    if (iCount > 0 &&
        (dynamic_cast<Force*>(m_pkForces->get_Item(iCount - 1)))->m_fT == fT)
    {
        return;
    }
    
    if (fT < m_fStart)
        m_fStart = fT;
    if (fT > m_fEnd)
        m_fEnd = fT;

    if (m_pkActor)
    {
        kForce *= m_pkActor->getMass();
    }
    else if (m_pkCloth)
    {
        NiPhysXClothMesh* pkMeshDesc = NiPhysXManager::GetPhysXManager()->
            GetClothMesh(m_pkCloth->getClothMesh());
        kForce *= m_pkCloth->getDensity() * pkMeshDesc->GetArea();
    }

    Force* pkForceEntry = NiExternalNew Force(fT, kForce);
    m_pkForces->Add(pkForceEntry);
}
//---------------------------------------------------------------------------
void MPhysics::ForceSet::TruncateStart(const float fT)
{
    // Find the new start index. Callign code ensures that the end time for
    // this set is strictly greater than fT. We must remove all forces
    // from the start of this set that have times less than or equal to fT.
    int iStart = 0;
    while ((dynamic_cast<Force*>(m_pkForces->get_Item(iStart)))->m_fT <= fT)
    {
        iStart++;
    }
    m_pkForces->RemoveRange(0, iStart);
    m_fStart = (dynamic_cast<Force*>(m_pkForces->get_Item(0)))->m_fT;
}
//---------------------------------------------------------------------------
void MPhysics::ForceSet::TruncateEnd(const float fT)
{
    // Find the new end index. Calling code ensures that the start time for
    // this set is strictly less than fT.  We must remove all forces
    // from the end of this set that have times greater than or equal to fT.
    int iEnd = m_pkForces->Count - 1;
    int iCount = 0;
    while ((dynamic_cast<Force*>(m_pkForces->get_Item(iEnd)))->m_fT >= fT)
    {
        iEnd--;
        iCount++;
    }
    m_pkForces->RemoveRange(iEnd + 1, iCount);
    m_fEnd = (dynamic_cast<Force*>(m_pkForces->get_Item(iEnd)))->m_fT;
}
//---------------------------------------------------------------------------
MPhysics::ForceSet* MPhysics::ForceSet::Split(
    const float fLowT, const float fHighT)
{
    // Find the first index that we will not be needing. That is, the
    // first index with time greater than or equal to the low time.
    int iStart = 0;
    while ((dynamic_cast<Force*>(m_pkForces->get_Item(iStart)))->m_fT < fLowT)
    {
        iStart++;
    }
    
    // Find the first index to copy into the new chunk (the first oen we need
    // to keep). That is the first index ith time strictly greater than
    // the end time.
    int iEnd = iStart;
    while ((dynamic_cast<Force*>(m_pkForces->get_Item(iEnd)))->m_fT <= fHighT)
    {
        iEnd++;
    }
    
    ForceSet* pkNewChunk = NiExternalNew ForceSet(m_pkActor, m_pkCloth,
        m_fPointX, m_fPointY, m_fPointZ, m_uiVertex);
    for (int i = iEnd; i < m_pkForces->Count; i++)
    {
        Force* pkForce = dynamic_cast<Force*>(m_pkForces->get_Item(i));
        if (pkForce->m_fT < pkNewChunk->m_fStart)
            pkNewChunk->m_fStart = pkForce->m_fT;
        if (pkForce->m_fT > pkNewChunk->m_fEnd)
            pkNewChunk->m_fEnd = pkForce->m_fT;
        pkNewChunk->m_pkForces->Add(pkForce);
    }
    
    m_pkForces->RemoveRange(iStart, m_pkForces->Count - iStart);
    m_fEnd = (dynamic_cast<Force*>(m_pkForces->get_Item(iStart - 1)))->m_fT;
    
    return pkNewChunk;
}
//---------------------------------------------------------------------------
bool MPhysics::ForceSet::ApplyForce(float fT)
{
    // Simplest is no existing forces
    int iExistingCount = m_pkForces->Count;
    if (iExistingCount == 0)
    {
        return false;
    }
    
    // Check if we're outside bounds
    if (fT >
        (dynamic_cast<Force*>(m_pkForces->get_Item(iExistingCount - 1)))->m_fT)
    {
        // Calling code ensures that this is only the case if we are pulling
        // forces from an active drag. The drag isn't done yet, so take the
        // latest force we have
        Force* pkForceData =
            dynamic_cast<Force*>(m_pkForces->get_Item(iExistingCount - 1));
        NxVec3 kForce(pkForceData->m_fForceX, pkForceData->m_fForceY,
            pkForceData->m_fForceZ);
        
        ApplyForceToObject(kForce);

        m_uiLastForceUsed = (unsigned int)(iExistingCount - 1);

        return true;
    }
    
    // Check last force used is in range
    if (m_uiLastForceUsed >= (unsigned int)iExistingCount)
        m_uiLastForceUsed = (unsigned int)iExistingCount - 1;

    // Upper and lower bound indexes.
    unsigned int uiLower;
    unsigned int uiUpper;
    
    // Find upper bound: has time greater than or equal to requested. If, for
    // some reason, time flowed backwards from m_uiLastForceUsed, then this
    // upper bound may not be very tight.
    uiUpper = m_uiLastForceUsed;
    while (uiUpper < (unsigned int)iExistingCount - 1 &&
        (dynamic_cast<Force*>(m_pkForces->get_Item(uiUpper)))->m_fT < fT)
    {
        uiUpper += 1;
    }
    
    // Find lower bound
    uiLower = uiUpper;
    while (uiLower > 0 &&
        (dynamic_cast<Force*>(m_pkForces->get_Item(uiLower)))->m_fT > fT)
    {
        uiLower -= 1;
    }
    
    // Figure out force by interpolating upper and lower
    NxVec3 kForce;
    Force* pkDataLow = dynamic_cast<Force*>(m_pkForces->get_Item(uiLower));
    Force* pkDataHigh = dynamic_cast<Force*>(m_pkForces->get_Item(uiUpper));
    float fLowT = pkDataLow->m_fT;
    float fHighT = pkDataHigh->m_fT;
    float fDt = fHighT - fLowT;
    if (uiUpper == uiLower || NiAbs(fDt) < 0.001f)
    {
        kForce.set(
            pkDataLow->m_fForceX, pkDataLow->m_fForceY, pkDataLow->m_fForceZ);
    }
    else
    {
        uiLower = uiUpper - 1;
        NxVec3 kLower;
        kLower.set(pkDataLow->m_fForceX, pkDataLow->m_fForceY,
            pkDataLow->m_fForceZ);
        NxVec3 kUpper;
        kUpper.set(pkDataHigh->m_fForceX, pkDataHigh->m_fForceY,
            pkDataHigh->m_fForceZ);
        float fU = (fT - fLowT) / fDt;
        kForce = (1 - fU) * kLower + fU * kUpper;
    }

    ApplyForceToObject(kForce);
    
    m_uiLastForceUsed = uiLower;

    return true;
}           
//---------------------------------------------------------------------------
void MPhysics::ForceSet::ApplyForceToObject(NxVec3& kForce)
{
    // Get the force application point
    if (m_pkActor)
    {
        NxVec3 kNxPt(m_fPointX, m_fPointY, m_fPointZ);
        if (!m_pkActor->readBodyFlag(NX_BF_KINEMATIC))
        {
            m_pkActor->addForceAtLocalPos(kForce, kNxPt);
        }
    }
    else if (m_pkCloth)
    {
        m_pkCloth->addForceAtVertex(kForce, m_uiVertex);
    }
}
//---------------------------------------------------------------------------
// Functions for the PhysX error streaming interface
//---------------------------------------------------------------------------
void MPhysics::OutputStream::reportError(NxErrorCode code, const char *message,
    const char *file, int line)
{
    char* pcErrorText = "";
    switch (code)
    {
        case NXE_NO_ERROR:
            pcErrorText = "No Error";
            break;
        case NXE_INVALID_PARAMETER:
            pcErrorText = "Invalid Parameter";
            break;
        case NXE_INVALID_OPERATION:
            pcErrorText = "Invalid Operation";
            break;
        case NXE_OUT_OF_MEMORY:
            pcErrorText = "Out of Memory";
            break;
        case NXE_INTERNAL_ERROR:
            pcErrorText = "Internal Error";
            break;
        case NXE_ASSERTION:
            pcErrorText = "Assertion";
            break;
        case NXE_DB_INFO:
            pcErrorText = "Debug Info";
            break;
        case NXE_DB_WARNING:
            pcErrorText = "Debug Warning";
            break;
        case NXE_DB_PRINT:
            pcErrorText = "Debug Print";
            break;
    }
    size_t stLength = strlen(pcErrorText) + strlen(message)
        + strlen(file) + 40;
    char* pcString = (char*)NiMalloc(stLength);
    
    NiSprintf(pcString, stLength, "PhysX Error: %s: %s in %s line %d\n",
        pcErrorText, message, file, line);

    MLogger::LogWarning(pcString);
            
    NiFree(pcString);
}
//--------------------------------------------------------------------------- 
NxAssertResponse MPhysics::OutputStream::reportAssertViolation (
    const char *message, const char *file, int line)
{
    size_t stLength = strlen(message) + strlen(file) + 40;
    char* pcString = (char*)NiMalloc(stLength);
    
    NiSprintf(pcString, stLength, "PhysX Assert Violation: %s in %s line %d\n",
        message, file, line);
    
    MLogger::LogWarning(pcString);
    
    NiFree(pcString);
    
    NIASSERT(false);

    return NX_AR_CONTINUE;
}
//--------------------------------------------------------------------------- 
void MPhysics::OutputStream::print(const char *message)
{
    MLogger::LogWarning(message);
}   
//--------------------------------------------------------------------------- 

//--------------------------------------------------------------------------- 
#pragma warning (default:4714)

#else // PHYSX build not defined
//--------------------------------------------------------------------------- 

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
MPhysics::MPhysics() :
    m_fGravityX(0.0f), m_fGravityY(0.0f), m_fGravityZ(-9.8f),
    m_fTimestep(0.01f), m_bDebugState(false), m_fDebugScale(1.0f),
    m_fGroundFriction(0.3f), m_fGroundBounce(0.3f), m_bNeedToSave(false)
{
}
//---------------------------------------------------------------------------
bool MPhysics::AddSequence(MSequence*)
{
    return false;
}
//---------------------------------------------------------------------------
bool MPhysics::RemoveSequence(MSequence*)
{
    return false;
}
//---------------------------------------------------------------------------
bool MPhysics::IsSequencePhysical(MSequence*)
{
    return false;
}
//---------------------------------------------------------------------------
bool MPhysics::IsSequenceGroupPhysical(MSequenceGroup*)
{
    return false;
}
//---------------------------------------------------------------------------
void MPhysics::ResetTime(float)
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties. Get functions.
//---------------------------------------------------------------------------
bool MPhysics::get_Present()
{
    return false;
}
//---------------------------------------------------------------------------
bool MPhysics::get_ScenePresent()
{
    return false;
}
//---------------------------------------------------------------------------
float MPhysics::get_GravityX()
{
    return m_fGravityX;
}
//---------------------------------------------------------------------------
float MPhysics::get_GravityY()
{
    return m_fGravityY;
}
//---------------------------------------------------------------------------
float MPhysics::get_GravityZ()
{
    return m_fGravityZ;
}
//---------------------------------------------------------------------------
float MPhysics::get_TimeStepLength()
{
    return m_fTimestep;
}
//---------------------------------------------------------------------------
bool MPhysics::get_DebugState()
{
    return m_bDebugState;
}
//---------------------------------------------------------------------------
float MPhysics::get_DebugScale()
{
    return m_fDebugScale;
}
//---------------------------------------------------------------------------
float MPhysics::get_GroundFriction()
{
    return m_fGroundFriction;
}
//---------------------------------------------------------------------------
float MPhysics::get_GroundBounce()
{
    return m_fGroundBounce;
}
//---------------------------------------------------------------------------
float MPhysics::get_ForceExponent()
{
    return m_fForceExponent;
}
//---------------------------------------------------------------------------
ArrayList* MPhysics::get_PhysicalSequences()
{
    return m_aSequences;
}
//---------------------------------------------------------------------------
bool MPhysics::get_NeedToSave()
{
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties. Set functions.
//---------------------------------------------------------------------------
void MPhysics::set_GravityX(float fX)
{
    m_fGravityX = fX;
}
//---------------------------------------------------------------------------
void MPhysics::set_GravityY(float fY)
{
    m_fGravityY = fY;
}
//---------------------------------------------------------------------------
void MPhysics::set_GravityZ(float fZ)
{
    m_fGravityZ = fZ;
}
//---------------------------------------------------------------------------
void MPhysics::set_TimeStepLength(float fStep)
{
    m_fTimestep = fStep;
}
//---------------------------------------------------------------------------
void MPhysics::set_DebugState(bool bState)
{
    m_bDebugState = bState;
}
//---------------------------------------------------------------------------
void MPhysics::set_DebugScale(float fScale)
{
    m_fDebugScale = fScale;
}
//---------------------------------------------------------------------------
void MPhysics::set_GroundFriction(float fValue)
{
    m_fGroundFriction = fValue;
}
//---------------------------------------------------------------------------
void MPhysics::set_GroundBounce(float fValue)
{
    m_fGroundBounce = fValue;
}
//---------------------------------------------------------------------------
void MPhysics::set_ForceExponent(float fExp)
{
    m_fForceExponent = fExp;
}
//---------------------------------------------------------------------------
void MPhysics::set_NeedToSave(bool bNeedToSave)
{
    m_bNeedToSave = bNeedToSave;
}
//---------------------------------------------------------------------------
#endif // if defined(EE_PHYSX_BUILD)

