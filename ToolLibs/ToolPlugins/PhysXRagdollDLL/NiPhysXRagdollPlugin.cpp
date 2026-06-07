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

#include "StdAfx.h"
#include "NiPhysXRagdollPlugin.h"
#include "NiPhysXRagdollDialog.h"
#include "NiPhysXRagdollPluginDefines.h"

#include "NiPhysXToolsLib.h"


// This tool plug-in relies on a naming convention used by the Maya scripts
// that create rag-dolls. Specifically, that the rigid body node for skeleton
// bone X will be named XRigidBody.


// RTTI implementation macro.
NiImplementRTTI(NiPhysXRagdollPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiPhysXRagdollPlugin::NiPhysXRagdollPlugin() : NiPlugin(
    "PhysX Rag-doll Plug-in",       // name
    "1.1",                              // version
    "Associates a rag-doll with its skeleton", // Short description
    "Associates a rag-doll created using the PhysX plug-in with the "
    "skeleton that was used to create it. This allows the rag-doll to drive "
    "the skinned character.")
{
    // If any required internal variables need to be set, set them here
}
//---------------------------------------------------------------------------
NiPhysXRagdollPlugin::~NiPhysXRagdollPlugin()
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NiPhysXRagdollPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiPhysXRagdollPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    //
    // Define all the defaults
    //
    pkPluginInfo->SetValue(PHYSX_RAGDOLL_SKELETON_NAME, "SceneNode");
    pkPluginInfo->SetValue(PHYSX_RAGDOLL_RAGDOLL_NAME, "ragdoll");
    pkPluginInfo->SetValue(PHYSX_RAGDOLL_PREFIX, "");
    pkPluginInfo->SetValue(PHYSX_RAGDOLL_POSTFIX, "RigidBody");
    pkPluginInfo->SetBool(PHYSX_RAGDOLL_KEEP, true);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiPhysXRagdollPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXRagdollPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    // Handle the user interface for manipulating the execution of this
    // NiPlugin.
    // - If the user clicks 'OK', make the input pkInitialInfo match the new
    //   set of parameters and return true.
    // - If the user clicks 'Cancel', then leave the input pkInitialInfo as
    //   it is and return false.

    NiPhysXRagdollDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch (kDialog.DoModal())
    {
        case IDOK:
            // Copy over the results.
            pkInitialInfo->RemoveAllParameters();
            kDialog.GetResults()->Clone(pkInitialInfo);
            break;
        case IDCANCEL:
            return false;
            break;
        default:
            NIASSERT(false);
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXRagdollPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the 
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "NiPhysXRagdollPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiPhysXRagdollPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    m_pkLogger = 0;
    
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList != 0);

    // Set up for logging errors
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    if (pkExporterSharedData)
    {
        if (pkExporterSharedData->GetWriteResultsToLog())
            m_pkLogger = pkExporterSharedData->GetXMLLogger();
    }
    
    // Get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXRagdollError",
                "The PhysX Ragdoll Plug-in could not find the "
                "NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The PhysX Ragdoll Plug-in could not find the "
                "NiSceneGraphSharedData!", "Scene Graph Shared Data Missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the PhysX shared data.
    NiPhysXSharedData* pkPhysXSharedData = (NiPhysXSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiPhysXSharedData));

    if (!pkPhysXSharedData)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXRagdollError",
                "The PhysX Ragdoll Plug-in could not find the "
                "NiPhysXSharedData object. Maybe this plug-in is running "
                "before the PhysX Extractor plug-in? It must run after.");
        }
        else
        {
            NiMessageBox(
                "The PhysX Ragdoll Plug-in could not find the "
                "NiPhysXSharedData object. Maybe this plug-in is running "
                "before the PhysX Extractor plug-in? It must run after.",
                "NiPhysXSharedData missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    if (!pkPhysXSharedData->GetPropCount())
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXRagdollError",
                "The PhysX Ragdoll Plug-in could not find and PhysX Props."
                "Maybe the PhysX Extractor plug-in failed?");
        }
        else
        {
            NiMessageBox(
                "The PhysX Ragdoll Plug-in could not find and PhysX Props."
                "Maybe the PhysX Extractor plug-in failed?",
                "NiPhysXSharedData missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Find the scene root node.
    NiNodePtr spSGAll = pkSGSharedData->GetFullSceneGraph();

    // Find the skeleton
    NiFixedString pkSkeletonName(
        (const char*)pkInfo->GetValue(PHYSX_RAGDOLL_SKELETON_NAME));
    NiNode* pkSkeleton = (NiNode*)spSGAll->GetObjectByName(pkSkeletonName);
    if (!pkSkeleton)
    {
        char acMsg[1024];
        NiSprintf(acMsg, 1024, "The PhysX Ragdoll Plug-in could not find "
            "the skeleton node: %s", (const char*)pkSkeletonName);
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXRagdollError", acMsg);
        }
        else
        {
            NiMessageBox(acMsg, "Missing skeleton node");
        }
        spSGAll = 0;
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Find the rag-doll
    NiFixedString pkRagdollName(
        (const char*)pkInfo->GetValue(PHYSX_RAGDOLL_RAGDOLL_NAME));
    NiNode* pkRagdoll = (NiNode*)spSGAll->GetObjectByName(pkRagdollName);
    spSGAll = 0;
    if (!pkRagdoll)
    {
        char acMsg[1024];
        NiSprintf(acMsg, 1024, "The PhysX Ragdoll Plug-in could not find "
            "the rag-doll node: %s", (const char*)pkRagdollName);
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXRagdollError", acMsg);
        }
        else
        {
            NiMessageBox(acMsg, "Missing rag-doll node");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    NiString kPrefix = pkInfo->GetValue(PHYSX_RAGDOLL_PREFIX);
    NiString kPostfix = pkInfo->GetValue(PHYSX_RAGDOLL_POSTFIX);
    
    // Get the actor and joint information out of the PhysX Props.
    unsigned int uiSize = pkPhysXSharedData->GetPropCount();
    float fInvScale = 1.0f;
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXProp* pkProp = pkPhysXSharedData->GetPropAt(ui);
        NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();
        if (!pkPropDesc)
            continue;

        CreateMaps(pkPropDesc);

        fInvScale = 1.0f / pkProp->GetScaleFactor();
    }

    if (!AttachRagdoll(pkSkeleton, pkRagdoll, kPrefix, kPostfix, fInvScale))
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXRagdollError",
                "The PhysX Ragdoll Plug-in could not attach the rag-doll");
        }
        else
        {
            NiMessageBox(
                "The PhysX Ragdoll Plug-in could not attach the rag-doll",
                "Rag-doll Failure");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    SetJointParents();
    
    bool bKeepShapes = pkInfo->GetBool(PHYSX_RAGDOLL_KEEP);
    if (!bKeepShapes)
    {
        RemoveExcessNodes();
    }
    
    // Update any additional stored poses.
    SetAdditionalPoses(spSGAll, pkPhysXSharedData);

    Reset();

    // Return success.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool NiPhysXRagdollPlugin::AttachRagdoll(NiNode* pkSkeleton,
    NiAVObject* pkRagdoll, NiString& kPrefix, NiString& kPostfix,
    const float fInvScale)
{
    NiString kNodeName((const char*)pkRagdoll->GetName());
    if (kPrefix == "" || kNodeName.Find((const char*)kPrefix) == 0)
    {
        unsigned int uiInputLength = kNodeName.Length();
        unsigned int uiPreLength = kPrefix.Length();
        unsigned int uiNewLength = uiInputLength - uiPreLength;
        NiString kPreStripped = kNodeName.Right(uiNewLength);
        
        unsigned int uiBaseEnd;
        if (kPostfix == "")
        {
            uiBaseEnd = kPreStripped.Length();
        }
        else
        {
            uiBaseEnd = kPreStripped.Find((const char*)kPostfix);
        }
        
        if (uiBaseEnd != NIPT_INVALID_INDEX)
        {
            ActorInfo* pkActorInfo;
            if (m_kActorMap.GetAt(pkRagdoll, pkActorInfo))
            {
                NiString kBaseName = kPreStripped.GetSubstring(0, uiBaseEnd);
                NiAVObject* pkNewTarget =
                    pkSkeleton->GetObjectByName((const char*)kBaseName);
                if (pkNewTarget)
                {
                    pkActorInfo->m_pkNewTarget = pkNewTarget;
                    SwitchTarget(pkActorInfo, fInvScale);
                }
            }
        }
    }
    
    if (NiIsKindOf(NiNode, pkRagdoll))
    {    
        NiNode* pkNode = (NiNode*)pkRagdoll;
        
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkObj = pkNode->GetAt(ui);
            if (!pkObj)
                continue;
            bool bSuccess =
                AttachRagdoll(pkSkeleton, pkObj, kPrefix, kPostfix, fInvScale);
            if (!bSuccess)
                return false;
        }
    }
 
    return true;    
}
//---------------------------------------------------------------------------
void NiPhysXRagdollPlugin::SwitchTarget(ActorInfo* pkActorInfo,
    const float fInvScale)
{
    NiPhysXActorDesc* pkActorDesc = pkActorInfo->m_pkActor;
    
    NiPhysXTransformDest* pkDest =
        NiDynamicCast(NiPhysXTransformDest, pkActorDesc->GetDest());
    
    NIASSERT(pkDest->GetTarget() == pkActorInfo->m_pkOldTarget);

    pkDest->SetTarget(pkActorInfo->m_pkNewTarget);
    pkActorInfo->m_pkNewTarget->SetSelectiveUpdateTransformsOverride(true);

    // Need to reset poses and shape poses.
    NxMat34 kOldPose = pkActorDesc->GetPose(0);
    
    NxMat34 kNewPose;
    NiPhysXTypes::NiTransformToNxMat34(
        pkActorInfo->m_pkNewTarget->GetWorldRotate(),
        pkActorInfo->m_pkNewTarget->GetWorldTranslate(), kNewPose);
    kNewPose.t *= fInvScale;
    pkActorDesc->SetPose(kNewPose, 0);
    
    // Old to new pose, for transforming shapes and joints
    NxMat34 kOldToNew;
    kOldToNew.multiplyInverseRTLeft(kNewPose, kOldPose);
    
    // Work through all the shapes
    NiTObjectArray<NiPhysXShapeDescPtr>& kShapes = 
        pkActorDesc->GetActorShapes();
    for (unsigned int ui = 0; ui < kShapes.GetSize(); ui++)
    {
        NiPhysXShapeDesc* pkShapeDesc = kShapes.GetAt(ui);
        NxMat34 kShapePose = pkShapeDesc->GetLocalPose();
        kShapePose = kOldToNew * kShapePose;
        pkShapeDesc->SetLocalPose(kShapePose);
    }
    
    // Work through the joints attached to this actor
    NiPhysXRagdollPlugin::JointArray* pkJointArray;
    if (m_kJointMap.GetAt(pkActorDesc, pkJointArray))
    {
        for (unsigned int ui = 0; ui < pkJointArray->GetSize(); ui++)
        {
            NiPhysXD6JointDesc* pkJointDesc =
                (NiPhysXD6JointDesc*)pkJointArray->GetAt(ui);
            unsigned int uiIndex =
                (pkActorDesc == pkJointDesc->GetActor(0) ? 0 : 1);
            
            NxD6JointDesc kNxJointDesc;
            pkJointDesc->GetD6JointDesc(kNxJointDesc,
                NiPhysXTypes::NXMAT34_ID);
            
            kNxJointDesc.localNormal[uiIndex] =
                kOldToNew.M * kNxJointDesc.localNormal[uiIndex];
            kNxJointDesc.localAxis[uiIndex] =
                kOldToNew.M * kNxJointDesc.localAxis[uiIndex];
            kNxJointDesc.localAnchor[uiIndex] =
                kOldToNew * kNxJointDesc.localAnchor[uiIndex];

            pkJointDesc->SetD6JointDesc(kNxJointDesc);
        }
    }
    
    pkActorDesc->SetActorParent(0);
}
//---------------------------------------------------------------------------
void NiPhysXRagdollPlugin::SetJointParents()
{
    // Iterate over actors, looking at their joints and determining if
    // the objects they are joined to are also their scene graph parents.
    NiTMapIterator iter = m_kJointMap.GetFirstPos();
    while (iter)
    {
        NiPhysXActorDesc* pkDesc1;
        NiPhysXRagdollPlugin::JointArray* pkArray;
        
        m_kJointMap.GetNext(iter, pkDesc1, pkArray);
        if (!pkArray)
            continue;
        
        // Work through all joints for the actor.
        for (unsigned int ui = 0; ui < pkArray->GetSize(); ui++)
        {
            NiPhysXD6JointDesc* pkJointDesc =
                (NiPhysXD6JointDesc*)pkArray->GetAt(ui);
            if (pkDesc1 != pkJointDesc->GetActor(1))
                continue;

            // Get the scene graph node for the "1" actor.
            NiAVObject* pkNode1 = 0;
            NiPhysXTransformDest* pkDest1 =
                NiDynamicCast(NiPhysXTransformDest, pkDesc1->GetDest());
            if (pkDest1 && pkDest1->GetTarget())
            {
                pkNode1 = pkDest1->GetTarget();
            }
            
            // Get the scene graph node for the "0" actor.
            NiAVObject* pkNode0 = 0;
            NiPhysXActorDesc* pkDesc0 = pkJointDesc->GetActor(0);
            NiPhysXTransformDest* pkDest0 =
                NiDynamicCast(NiPhysXTransformDest, pkDesc0->GetDest());
            if (pkDest0 && pkDest0->GetTarget())
            {
                pkNode0 = pkDest0->GetTarget();
            }

            // Check if 1 is 0's parent, and vice versa. We check both
            // directions because we cannot assume the joint has the
            // nodes in any particular order.
            if (pkNode1 && pkNode0)
            {
                if (pkNode1->GetParent() == pkNode0)
                    pkDesc1->SetActorParent(pkDesc0);
                if (pkNode0->GetParent() == pkNode1)
                    pkDesc0->SetActorParent(pkDesc1);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXRagdollPlugin::SetAdditionalPoses(NiNode* pkSceneGraph,
    NiPhysXSharedData* pkPhysXSharedData)
{
    NiMeshUpdateProcess kUpdateProcess;
    kUpdateProcess.SetUpdateControllers(true);

    NiUInt32 uiNumStates = pkPhysXSharedData->GetStateCount();
    for (NiUInt32 ui = 0; ui < uiNumStates; ui++)
    {
        NiPhysXSharedData::StateInfo& kStateInfo =
            pkPhysXSharedData->GetStateAt(ui);
        
        kUpdateProcess.SetTime(kStateInfo.m_fStateTime);
        pkSceneGraph->Update(kUpdateProcess);
        NiMesh::CompleteSceneModifiers(pkSceneGraph);

        NiUInt32 uiNumProps = pkPhysXSharedData->GetPropCount();
        for (NiUInt32 uj = 0; uj < uiNumProps; uj++)
        {
            NiPhysXProp* pkProp = pkPhysXSharedData->GetPropAt(uj);
            if (!pkProp)
                continue;

            NiPhysXTools::AddStateFromSceneGraph(pkProp,
                kStateInfo.m_kStateName);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXRagdollPlugin::CreateMaps(NiPhysXPropDesc* pkPropDesc)
{
    NiUInt32 uiActorCount = pkPropDesc->GetActorCount();
    m_kActorMap.Resize(NiTMap<NiUInt32, NiUInt32>::NextPrime(uiActorCount));
    for (unsigned int ui = 0; ui < uiActorCount; ui++)
    {
        NiPhysXActorDesc* pkActorDesc = pkPropDesc->GetActorAt(ui);
        NiPhysXTransformDest* pkDest =
            NiDynamicCast(NiPhysXTransformDest, pkActorDesc->GetDest());
        if (pkDest)
        {
            NiAVObject* pkTarget = pkDest->GetTarget();
            if (pkTarget)
            {
                ActorInfo* pkActorInfo = NiNew ActorInfo;
                pkActorInfo->m_pkActor = pkActorDesc;
                pkActorInfo->m_pkNewTarget = 0;
                pkActorInfo->m_pkOldTarget = pkTarget;
                m_kActorMap.SetAt(pkTarget, pkActorInfo);
            }
        }
    }
    
    NiUInt32 uiJointCount = pkPropDesc->GetJointCount();
    m_kJointMap.Resize(NiTMap<NiUInt32, NiUInt32>::NextPrime(uiJointCount));
    for (unsigned int ui = 0; ui < uiJointCount; ui++)
    {
        NiPhysXJointDesc* pkJointDesc = pkPropDesc->GetJointAt(ui);
        NiPhysXActorDesc* pkActorDesc;
        
        pkActorDesc = pkJointDesc->GetActor(0);
        if (pkActorDesc)
        {
            NiPhysXRagdollPlugin::JointArray* pkArray;
            bool bArrayExists = m_kJointMap.GetAt(pkActorDesc, pkArray);
            if (bArrayExists)
            {
                pkArray->Add(pkJointDesc);
            }
            else
            {
                pkArray = NiNew NiPhysXRagdollPlugin::JointArray;
                pkArray->Add(pkJointDesc);
                m_kJointMap.SetAt(pkActorDesc, pkArray);
            }
        }
        
        pkActorDesc = pkJointDesc->GetActor(1);
        if (pkActorDesc)
        {
            NiPhysXRagdollPlugin::JointArray* pkArray;
            bool bArrayExists = m_kJointMap.GetAt(pkActorDesc, pkArray);
            if (bArrayExists)
            {
                pkArray->Add(pkJointDesc);
            }
            else
            {
                pkArray = NiNew NiPhysXRagdollPlugin::JointArray;
                pkArray->Add(pkJointDesc);
                m_kJointMap.SetAt(pkActorDesc, pkArray);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXRagdollPlugin::RemoveExcessNodes()
{
    NiTMapIterator kIter = m_kActorMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkJunk;
        ActorInfo* pkActorInfo;
        m_kActorMap.GetNext(kIter, pkJunk, pkActorInfo);
        if (pkActorInfo->m_pkNewTarget &&
            pkActorInfo->m_pkOldTarget->GetParent())
        {
            pkActorInfo->m_pkOldTarget->GetParent()->
                DetachChild(pkActorInfo->m_pkOldTarget);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXRagdollPlugin::Reset()
{
    NiTMapIterator kIter = m_kActorMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkJunk;
        ActorInfo* pkActorInfo;
        m_kActorMap.GetNext(kIter, pkJunk, pkActorInfo);
        NiDelete pkActorInfo;
    }
    m_kActorMap.RemoveAll();

    kIter = m_kJointMap.GetFirstPos();
    while (kIter)
    {
        NiPhysXActorDesc* pkKey;
        JointArray* pkVal;
        
        m_kJointMap.GetNext(kIter, pkKey, pkVal);
        NiDelete pkVal;
    }
    m_kJointMap.RemoveAll();
}
//---------------------------------------------------------------------------
