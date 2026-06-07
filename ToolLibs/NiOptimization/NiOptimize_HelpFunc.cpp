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

#include "NiOptimize.h"
#include <NiBooleanExtraData.h>
#include <NiStringExtraData.h>
#include <NiLookAtInterpolator.h>
#include <NiTransformController.h>
#include <NiLinRotKey.h>
#include <NiLinPosKey.h>
#include <NiLinFloatKey.h>
#include <NiEulerRotKey.h>
#include <NiBezRotKey.h>
#include <NiBezPosKey.h>
#include <NiBezFloatKey.h>
#include <NiIntegerExtraData.h>
#include <NiMesh.h>
#include <NiMeshUtilities.h>
#include <NiMeshTools.h>
#include <NiToolDataStream.h>
#include <NiParticle.h>
#include <NiShadowGenerator.h>
#include <NiSkinningMeshModifier.h>
#include <NiTSimpleArray.h>
#include <NiSkinningUtilities.h>
#include <NiDataStreamElementLock.h>
#include <NiDataStreamLock.h>
#include <NiProcessor.h>
#include <NiMorphMeshModifier.h>
#include <NiSkinningLODController.h>
#include <NiTNodeTraversal.h>

//---------------------------------------------------------------------------
// Helper functions.
//---------------------------------------------------------------------------
bool NiOptimize::CheckForExtraDataTags(NiAVObject* pkObject)
{
    // Checks for extra data strings that indicate an object should not
    // be removed.

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    const char* ppcTags[4] = {"fiducial", "sgoKeep", "NiOptimizeKeep",
        "NiBoneLOD#"};

    return CheckForExtraDataTags(pkObject, ppcTags, 4);
}
//---------------------------------------------------------------------------
bool NiOptimize::CheckForExtraDataTags(NiAVObject* pkObject, 
    const char* ppcTags[], unsigned int uiNumTags)
{
    // Checks for the string extra data tags in ppcTags

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    for (unsigned int i=0; i < pkObject->GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtraData = pkObject->GetExtraDataAt((unsigned short)i);

        NiStringExtraData* pkStrExData = NiDynamicCast(NiStringExtraData,
                pkExtraData);
        if (pkStrExData)
        {
            const char* pcString = pkStrExData->GetValue();
            if (pcString)
            {
                for(unsigned int uiLoop = 0; uiLoop < uiNumTags; uiLoop++)
                {
                    if (strstr(pcString, ppcTags[uiLoop]))
                    {
                        return true;   
                    }
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::BuildHashTable(NiAVObject* pkObject,
    NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Builds a hash table containing the pointers of objects that should
    // not be removed by removal or merge functions. The following objects
    // are added to kMap:
    // - objects referenced by particle systems
    // - affected nodes of dynamic effects
    // - all bones
    // - Biped Footsteps
    // - Objects with ABV's
    // - Objects used with the NiPortal system

    // Add LookAt target.
    NiTransformController* pkTransformCtlr = NiGetController(
        NiTransformController, pkObject);
    if (pkTransformCtlr)
    {
        NiInterpolator* pkInterp = pkTransformCtlr->GetInterpolator(0);
        if (pkInterp && NiIsKindOf(NiLookAtInterpolator, pkInterp))
        {
            NiLookAtInterpolator* pkLookAtInterp = (NiLookAtInterpolator*)
                pkInterp;
            NiAVObject* pkLookAt = pkLookAtInterp->GetLookAt();
            if (pkLookAt)
                kMap.SetAt(pkLookAt, true);
        }
    }

    // Add objects referenced by particle systems.
    if (NiIsKindOf(NiPSParticleSystem, pkObject))
    {
        NiPSParticleSystem* pkPSystem = (NiPSParticleSystem*) pkObject;

        // Grab pointers to simulation steps
        NiPSSimulatorForcesStep* pkForcesStep = NULL;
        NiPSSimulatorCollidersStep* pkCollidersStep = NULL;
        NiUInt32 uiStepCount = pkPSystem->GetSimulator()->GetStepCount();
        for (NiUInt32 ui = 0; ui < uiStepCount; ui++)
        {
            NiPSSimulatorStep* pkStep =
                pkPSystem->GetSimulator()->GetStepAt(ui);
            if (NiIsExactKindOf(NiPSSimulatorForcesStep, pkStep))
            {
                pkForcesStep = (NiPSSimulatorForcesStep*) pkStep;
            }
            else if (NiIsExactKindOf(NiPSSimulatorCollidersStep, pkStep))
            {
                pkCollidersStep = (NiPSSimulatorCollidersStep*) pkStep;
            }
        }
        
        NiUInt32 uiEmitterCount = pkPSystem->GetEmitterCount();
        for (NiUInt32 ui = 0; ui < uiEmitterCount; ui++)
        {
            NiPSEmitter* pkEmitter = pkPSystem->GetEmitterAt(ui);
            
            if (NiIsKindOf(NiPSMeshEmitter, pkEmitter))
            {
                NiPSMeshEmitter* pkMeshEmitter = (NiPSMeshEmitter*)pkEmitter;
                NiUInt32 uiMeshCount = pkMeshEmitter->GetMeshEmitterCount();
                for (NiUInt32 uj = 0; uj < uiMeshCount; uj++)
                {
                    kMap.SetAt(pkMeshEmitter->GetMeshEmitter(uj), true);
                }
            }
            else if (NiIsKindOf(NiPSVolumeEmitter, pkEmitter))
            {
                kMap.SetAt(((NiPSVolumeEmitter*)pkEmitter)->GetEmitterObj(),
                    true);
            }
        }
        
        NiUInt32 uiForceCount =
            pkForcesStep ? pkForcesStep->GetForcesCount() : 0;
        for (NiUInt32 ui = 0; ui < uiForceCount; ui++)
        {
            NiPSForce* pkForce = pkForcesStep->GetForceAt(ui);
            
            if (NiIsKindOf(NiPSFieldForce, pkForce))
            {
                kMap.SetAt(((NiPSFieldForce*)pkForce)->GetFieldObj(), true);
            }
            else if (NiIsKindOf(NiPSBombForce, pkForce))
            {
                kMap.SetAt(((NiPSBombForce*)pkForce)->GetBombObj(), true);
            }
            else if (NiIsKindOf(NiPSDragForce, pkForce))
            {
                kMap.SetAt(((NiPSDragForce*)pkForce)->GetDragObj(), true);
            }
            else if (NiIsKindOf(NiPSGravityForce, pkForce))
            {
                kMap.SetAt(((NiPSGravityForce*)pkForce)->GetGravityObj(),
                    true);
            }
        }

        NiUInt32 uiColliderCount =
            pkCollidersStep ? pkCollidersStep->GetCollidersCount() : 0;
        for (NiUInt32 ui = 0; ui < uiColliderCount; ui++)
        {
            NiPSCollider* pkCollider = pkCollidersStep->GetColliderAt(ui);
            
            if (NiIsKindOf(NiPSPlanarCollider, pkCollider))
            {
                kMap.SetAt(
                    ((NiPSPlanarCollider*)pkCollider)->GetColliderObj(), true);
            }
            else if (NiIsKindOf(NiPSSphericalCollider, pkCollider))
            {
                kMap.SetAt(
                    ((NiPSSphericalCollider*)pkCollider)->GetColliderObj(),
                    true);
            }
        }

        NiPSMeshParticleSystem* pkMeshPSystem = NiDynamicCast(
            NiPSMeshParticleSystem, pkPSystem);
        if (pkMeshPSystem)
        {
            for (NiUInt32 uiGen = 0;
                uiGen < pkMeshPSystem->GetNumGenerations(); uiGen++)
            {
                kMap.SetAt(pkMeshPSystem->GetMasterParticle((NiUInt16)uiGen),
                    true);
            }
        }
    }

    // Add NiAVObjects with Collision Object's
    if (pkObject->GetCollisionObject())
    {
        kMap.SetAt(pkObject, true);
    }

    // Add affected nodes of dynamic effects.
    if (NiIsKindOf(NiDynamicEffect, pkObject))
    {
        NiTListIterator kPos = ((NiDynamicEffect*)
            pkObject)->GetAffectedNodeList().GetHeadPos();
        while (kPos)
        {
            NiNode* pkAffectedNode = ((NiDynamicEffect*)
                pkObject)->GetAffectedNodeList().GetNext(kPos);
            if (pkAffectedNode)
                kMap.SetAt(pkAffectedNode, true);
        }

    
        // Add unaffected nodes of dynamic effects.
        kPos = ((NiDynamicEffect*)
            pkObject)->GetUnaffectedNodeList().GetHeadPos();
        while (kPos)
        {
            NiNode* pkUnaffectedNode = ((NiDynamicEffect*)
                pkObject)->GetUnaffectedNodeList().GetNext(kPos);
            if (pkUnaffectedNode)
                kMap.SetAt(pkUnaffectedNode, true);
        }

        // Add targets of shadow generator node lists
        NiShadowGenerator* pkShadow = 
            ((NiDynamicEffect*)pkObject)->GetShadowGenerator();
        if (pkShadow != NULL)
        {
            // Add unaffected casters
            const NiNodeList& pkCasterNodeList = 
                pkShadow->GetUnaffectedCasterNodeList();
            kPos = pkCasterNodeList.GetHeadPos();
            while (kPos)
            {
                NiNode* pkUnaffectedNode = pkCasterNodeList.GetNext(kPos);
                if (pkUnaffectedNode != NULL)
                    kMap.SetAt(pkUnaffectedNode, true);
            }

            // Add unaffected receivers
            const NiNodeList& pkReceiverNodeList = 
                pkShadow->GetUnaffectedReceiverNodeList();
            kPos = pkReceiverNodeList.GetHeadPos();
            while (kPos)
            {
                NiNode* pkUnaffectedNode = pkReceiverNodeList.GetNext(kPos);
                if (pkUnaffectedNode != NULL)
                    kMap.SetAt(pkUnaffectedNode, true);
            }
        }
    }

    // Add bones referenced by a skin
    NiSkinningMeshModifier* pkSkin = NULL;
    NiMesh* pkMesh = NiDynamicCast(NiMesh, pkObject);
    if (pkMesh)
        pkSkin = NiGetModifier(NiSkinningMeshModifier, pkMesh);

    if (pkSkin)
    {
        NiAVObject* pkRootParent = pkSkin->GetRootBoneParent();
        if (pkRootParent)
            kMap.SetAt(pkRootParent, true);
        NiUInt32 uiBones = pkSkin->GetBoneCount();
        NiAVObject** ppkBones = pkSkin->GetBones();

        for (NiUInt32 ui = 0; ui < uiBones; ui++)
        {
            NiAVObject* pkBone = ppkBones[ui];
            if (pkBone)
                kMap.SetAt(pkBone, true);
        }
    }

    // Add objects used with the NiPortal system.
    const char* pcName = pkObject->GetName();
    if (pcName && (strstr(pcName, "NDLPR") || strstr(pcName, "NDLRG") ||
        strstr(pcName, "NDLRM") || strstr(pcName, "NDLWL")))
    {
        kMap.SetAt(pkObject, true);
    }

    // Add Objects used with ABV collision
    if (pcName && (strstr(pcName, "NDLCD ")))
    {
        kMap.SetAt(pkObject, true);
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // Add biped footsteps node.
        NiNode* pkParent = pkNode->GetParent();
        if (pkParent && pkParent->GetName())
        {
            const char* pcParent = pkParent->GetName();

            size_t stLen = strlen(pcParent) + 11;
            char* pcFootsteps = NiAlloc(char, stLen);
            NiSprintf(pcFootsteps, stLen, "%s Footsteps", pcParent);
            if (pkNode->GetName() &&
                strcmp(pcFootsteps, pkNode->GetName()) == 0)
            {
                kMap.SetAt(pkNode, true);
            }
            NiFree(pcFootsteps);
        }

        // Recurse over children.
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                BuildHashTable(pkChild, kMap);
        }
    }
}
//---------------------------------------------------------------------------
class AddAllMeshesToMapFunctor
{

public:
    AddAllMeshesToMapFunctor(NiTPointerMap<NiAVObject*, bool>& kMap)
        : m_kMap(kMap) {};

    void operator () (NiAVObject* pkAVObject)
    {
        if (NiIsKindOf(NiMesh, pkAVObject))
        {
            m_kMap.SetAt(pkAVObject, true);
        }
    }

private:
    // Private assignment operator that's not defined, to address benign
    // compiler warning C4512.
    void operator = (const AddAllMeshesToMapFunctor &Rhs); 

    NiTPointerMap<NiAVObject*, bool>& m_kMap;
};
//---------------------------------------------------------------------------
void NiOptimize::AddBoneLODSkinsToHashTable(NiAVObject* pkObject,
    NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // The following logic applies if this method is called BEFORE the 
    // NiOptimize::CreateSkinningLODControllers() method. 
    // E.g. "Remove Hidden Objects" plug-in is before 
    // "Scene Graph Optimization" plug-in during export process
    // Don't remove any mesh descendants of a "NiBoneLOD#Skin" tagged node
    const char* ppcTags[1] = {"NiBoneLOD#Skin"};
    if (NiOptimize::CheckForExtraDataTags(pkObject, ppcTags, 1))
    {
        AddAllMeshesToMapFunctor kFunctor(kMap);
        NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
        // No need to recurse the tree below this node since we've added 
        // every NiMesh below this node, just return.
        return;
    }

    // The following logic applies if this method is called AFTER the 
    // NiOptimize::CreateSkinningLODControllers() method. 
    // E.g. "Scene Graph Optimization" plug-in is before
    // "Remove Hidden Objects" plug-in during export process
    // Don't remove any mesh objects referred to by a NiSkinningLODController
    NiSkinningLODController* pkSkinLODController = 
        NiGetController(NiSkinningLODController, pkObject);
    if (pkSkinLODController)
    {
        NiUInt32 uiNumBoneLODs = pkSkinLODController->GetNumberOfBoneLODs();
        for(NiUInt32 uiBoneLOD=0; uiBoneLOD < uiNumBoneLODs; ++uiBoneLOD)
        {   
            NiUInt32 uiNumSkins = 
                pkSkinLODController->GetNumberOfSkins(uiBoneLOD);
            for(NiUInt32 uiSkin=0; uiSkin < uiNumSkins; ++uiSkin)
            {
                NiMesh* pkSkin = pkSkinLODController->GetSkin(uiBoneLOD,
                    uiSkin);
                if (pkSkin)
                {
                    kMap.SetAt(pkSkin, true);
                }
            }
        }
    }

    // Recurse the scene graph
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                AddBoneLODSkinsToHashTable(pkChild, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::CollectProperties(NiAVObject* pkObject,
    NiPropertyArray& apkProps, unsigned int uiType)
{
    // Recursively collects all properties of the specified type and stores
    // pointers to them in apkProps.

    // Add this object's property to apkMaterials.
    NiProperty* pkProp = pkObject->GetProperty(uiType);
    if (pkProp)
        apkProps.Add(pkProp);

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if(pkChild)
                CollectProperties(pkChild, apkProps, uiType);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::PropagateProperties(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Propagates properties from pkObj1 to pkObj2.

    // Loop through the properties in the two objects.  If pkObj1 has
    // a property that pkObj2 doesn't, attach that property to pkObj2.
    for (unsigned int ui = 0; ui < NiProperty::MAX_TYPES; ui++)
    {
        NiProperty* pkProp1 = pkObj1->GetProperty(ui);
        NiProperty* pkProp2 = pkObj2->GetProperty(ui);
        if (pkProp1 && !pkProp2)
        {
            // Attach the property to the pkObj2.
            pkObj2->AttachProperty(pkProp1);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::PropagateTransforms(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Propagates transforms from pkObj1 to pkObj2.

    // Get the transforms from the two objects.
    NiPoint3 kTrans1 = pkObj1->GetTranslate();
    NiPoint3 kTrans2 = pkObj2->GetTranslate();
    NiMatrix3 kRot1 = pkObj1->GetRotate();
    NiMatrix3 kRot2 = pkObj2->GetRotate();
    float fScale1 = pkObj1->GetScale();
    float fScale2 = pkObj2->GetScale();

    // Combine the transforms to get the new transforms.
    NiMatrix3 kNewRot = kRot1 * kRot2;
    float fNewScale = fScale1 * fScale2;
    NiPoint3 kNewTrans = kTrans1 + fScale1 * (kRot1 * kTrans2);

    // Set the new transforms.
    pkObj2->SetTranslate(kNewTrans);
    pkObj2->SetRotate(kNewRot);
    pkObj2->SetScale(fNewScale);
}
//---------------------------------------------------------------------------
bool NiOptimize::CompareProperties(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Checks whether or not two objects have the same properties.

    // Returns: true, the properties are the same.
    //          false, the properties are not the same.

    // Loop through all the properties, testing for equality with each one.
    for (unsigned int ui = 0; ui < NiProperty::MAX_TYPES; ui++)
    {
        NiProperty* pkProp1 = pkObj1->GetProperty(ui);
        NiProperty* pkProp2 = pkObj2->GetProperty(ui);

        if (pkProp1 && pkProp2)
        {
            // This may return false even if the two properties are
            // functionally equivalent.  It will always return false if the
            // properties are not the same, though.
            if (!pkProp1->IsEqual(pkProp2))
            {
                return false;
            }
        }
        else if (!pkProp1 && !pkProp2)
        {
            // If neither node has the property, do nothing.
        }
        else
        {
            // If one of the properties is NULL and the other is not,
            // the properties cannot be the same, so return false.
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOptimize::CompareTransforms(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Checks whether or not two objects have the same transforms.

    // Returns: true, the transforms are the same.
    //          false, the transforms are not the same.

    // Get the transforms from the two nodes.
    NiPoint3 kTrans1 = pkObj1->GetTranslate();
    NiPoint3 kTrans2 = pkObj2->GetTranslate();
    NiMatrix3 kRot1 = pkObj1->GetRotate();
    NiMatrix3 kRot2 = pkObj2->GetRotate();
    float fScale1 = pkObj1->GetScale();
    float fScale2 = pkObj2->GetScale();

    // Return true if all of the transforms are equal.
    if (kTrans1 == kTrans2 &&
        kRot1 == kRot2 &&
        fScale1 == fScale2)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiOptimize::CanMergeMeshData(NiMesh* pkMeshA, NiMesh* pkMeshB)
{
    NIASSERT(pkMeshA && pkMeshB);

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMeshA) &&
        !NiIsKindOf(NiPSParticleSystem, pkMeshB));

    if (NiMeshTools::SharesDataStreams(pkMeshA, pkMeshB))
        return false;
    
    if (pkMeshA->GetSubmeshCount() != 1 || pkMeshB->GetSubmeshCount() != 1)
        return false;

    NiUInt32 uiStreamRefCountA = pkMeshA->GetStreamRefCount();
    if (uiStreamRefCountA != pkMeshB->GetStreamRefCount())
        return false;

    for (NiUInt32 ui = 0; ui < uiStreamRefCountA; ui++)
    {
        NiDataStreamRef* pkStreamRefA =
            pkMeshA->GetStreamRefAt(ui);

        NiDataStreamRef* pkStreamRefB = 
            pkMeshB->GetStreamRefAt(ui);

        if (pkStreamRefA->GetElementDescCount() != 
            pkStreamRefB->GetElementDescCount())
        {
            return false;
        }

        if (pkStreamRefA->GetElementDescCount() != 1)
            return false;

        if (pkStreamRefA->IsPerInstance() != pkStreamRefB->IsPerInstance())
            return false;

        if (pkStreamRefA->GetAccessMask() != pkStreamRefB->GetAccessMask())
            return false;

        if (pkStreamRefA->GetUsage() != pkStreamRefB->GetUsage())
            return false;

        if (pkStreamRefA->GetStride() != pkStreamRefB->GetStride())
            return false;

        for (NiUInt32 j = 0; j < pkStreamRefA->GetElementDescCount(); j++)
        {
            if (pkStreamRefA->GetSemanticNameAt(j) != 
                pkStreamRefB->GetSemanticNameAt(j))
            {
                return false;
            }

            if (pkStreamRefA->GetSemanticIndexAt(j) != 
                pkStreamRefB->GetSemanticIndexAt(j))
            {
                return false;
            }

            if (!pkStreamRefA->GetElementDescAt(j).IsEqual(
                pkStreamRefB->GetElementDescAt(j)))
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOptimize::GetNoOptimizeMeshExtraData(NiAVObject* pkObject)
{
    if (pkObject == NULL)
        return false;

    const char* ppcTags[1] = {"NiNoOptimizeMesh"};

    return CheckForExtraDataTags(pkObject, ppcTags, 1);
}
//---------------------------------------------------------------------------
bool NiOptimize::CanMergeMesh(NiMesh* pkObj1, NiMesh* pkObj2)
{
    // Tests whether or not the two mesh objects can be merged. They can
    // be merged if the following is true:
    // They have:
    //     the same set of materials,
    //     the same number of properties,
    //     the same set of property pointers,    
    //     the same set of extra data objects,
    //     app culled flags are equal   
    //     data streams are merge friendly
    // and they don't have:
    //     ABV's 
    //     "ABV" in the object name
    //     "NiWalkabout" in the extra data tags
    //     "NiOptimizeDontMerge" in the extra data tags

    // Returns: true, the two objects can be merged.
    //          false, the two objects cannot be merged.

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkObj1) &&
        !NiIsKindOf(NiPSParticleSystem, pkObj2));

    if (pkObj1->GetPrimitiveType() != pkObj2->GetPrimitiveType())
        return false;

    if (pkObj1->GetAppCulled() != pkObj2->GetAppCulled())
        return false;    

    if (!CanMergeMeshData(pkObj1, pkObj2))
        return false;

    // Must have the same set of materials.
    bool bMaterialsDifferent = false;
    for (unsigned int uiIndex1 = 0; !bMaterialsDifferent &&
        uiIndex1 < pkObj1->GetMaterialCount(); uiIndex1++)
    {
        const NiMaterialInstance* pkInstance1 = pkObj1->GetMaterialInstance(
            uiIndex1);
        NIASSERT(pkInstance1);
        const unsigned int uiMaterialCount2 = pkObj2->GetMaterialCount();
        unsigned int uiIndex2;
        for (uiIndex2 = 0; uiIndex2 < uiMaterialCount2; uiIndex2++)
        {
            const NiMaterialInstance* pkInstance2 =
                pkObj2->GetMaterialInstance(uiIndex2);
            NIASSERT(pkInstance2);
            if (pkInstance1->GetMaterial() == pkInstance2->GetMaterial() &&
                pkInstance1->GetMaterialExtraData() ==
                pkInstance2->GetMaterialExtraData())
            {
                break;
            }
        }
        if (uiIndex2 == uiMaterialCount2)
        {
            bMaterialsDifferent = true;
        }
    }
    for (unsigned int uiIndex2 = 0; !bMaterialsDifferent &&
        uiIndex2 < pkObj2->GetMaterialCount(); uiIndex2++)
    {
        const NiMaterialInstance* pkInstance2 = pkObj2->GetMaterialInstance(
            uiIndex2);
        NIASSERT(pkInstance2);
        const unsigned int uiMaterialCount1 = pkObj1->GetMaterialCount();
        unsigned int uiIndex1;
        for (uiIndex1 = 0; uiIndex1 < uiMaterialCount1; uiIndex1++)
        {
            const NiMaterialInstance* pkInstance1 =
                pkObj1->GetMaterialInstance(uiIndex1);
            NIASSERT(pkInstance1);
            if (pkInstance2->GetMaterial() == pkInstance1->GetMaterial() &&
                pkInstance2->GetMaterialExtraData() ==
                pkInstance1->GetMaterialExtraData())
            {
                break;
            }
        }
        if (uiIndex1 == uiMaterialCount1)
        {
            bMaterialsDifferent = true;
        }
    }
    if (bMaterialsDifferent)
    {
        return false;
    }

    // Must have the same set of extra data objects.
    bool bExtraDataDifferent = false;
    for (NiUInt32 usIndex1 = 0; !bExtraDataDifferent &&
        usIndex1 < pkObj1->GetExtraDataSize(); usIndex1++)
    {
        NiExtraData* pkExtraData1 = pkObj1->GetExtraDataAt(
            (unsigned short)usIndex1);
        const NiUInt32 usExtraDataSize2 = pkObj2->GetExtraDataSize();
        NiUInt32 usIndex2;
        for (usIndex2 = 0; usIndex2 < usExtraDataSize2; usIndex2++)
        {
            if (pkExtraData1->IsEqual(pkObj2->GetExtraDataAt(
                (unsigned short)usIndex2)))
            {
                break;
            }
        }
        if (usIndex2 == usExtraDataSize2)
        {
            bExtraDataDifferent = true;
        }
    }
    for (NiUInt32 usIndex2 = 0; !bExtraDataDifferent &&
        usIndex2 < pkObj2->GetExtraDataSize(); usIndex2++)
    {
        NiExtraData* pkExtraData2 = pkObj2->GetExtraDataAt(
            (unsigned short)usIndex2);
        const NiUInt32 usExtraDataSize1 = pkObj1->GetExtraDataSize();
        NiUInt32 usIndex1;
        for (usIndex1 = 0; usIndex1 < usExtraDataSize1; usIndex1++)
        {
            if (pkExtraData2->IsEqual(pkObj1->GetExtraDataAt(
                (unsigned short)usIndex1)))
            {
                break;
            }
        }
        if (usIndex1 == usExtraDataSize1)
        {
            bExtraDataDifferent = true;
        }
    }
    if (bExtraDataDifferent)
    {
        return false;
    } 

    // Property lists must have same number of elements.
    if (pkObj1->GetPropertyList().GetSize() !=
        pkObj2->GetPropertyList().GetSize())
    {
        return false;
    }

    // Two AVObjects can merge if they have the same set of property
    // *pointers*.
    NiTListIterator kPos1 = pkObj1->GetPropertyList().GetHeadPos();
    while (kPos1)
    {
        NiPropertyPtr spP1 = pkObj1->GetPropertyList().GetNext(kPos1);
        NiPropertyPtr spP2;

        NiTListIterator kPos2 = pkObj2->GetPropertyList().GetHeadPos();
        while (kPos2)
        {
            spP2 = pkObj2->GetPropertyList().GetNext(kPos2);
            
            if (spP1 && (spP1 == spP2))
                break;
        }
        
        if (spP1 != spP2)
            return false;
    }

    // Objects must both have no Collision Objects
    if(pkObj1->GetCollisionObject() != NULL || 
       pkObj2->GetCollisionObject() != NULL)
        return false;
   
    const char* pcName = pkObj1->GetName();
    if (pcName && strncmp(pcName, "ABV", 3) == 0)
        return false;
    pcName = pkObj2->GetName();
    if (pcName && strncmp(pcName, "ABV", 3) == 0)
        return false;

    const char* ppcTags[2] = { "NiWalkabout" , 
        "NiOptimizeDontMerge" };

    if (CheckForExtraDataTags(pkObj1, ppcTags, 2) ||
        CheckForExtraDataTags(pkObj2, ppcTags, 2))
    {
        return false;
    }

    // If the above tests are all passed, the two objects can be merged.
    return true;
}
//---------------------------------------------------------------------------
void NiOptimize::MergeMeshes(NiMesh* pkMesh, 
    const NiMeshPtrArray& aspMergeMeshes) 
{
    // Merges all the NiMesh objects in aspMergeMeshes and stores the result
    // in the specified mesh.
    // Transforms on all NiMesh objects are pushed into the vertex and
    // normal data.

    NIASSERT(pkMesh);
    if (aspMergeMeshes.GetEffectiveSize() == 0)
        return;

    CollapseMeshTransform(pkMesh);

    unsigned int ui;
    for (ui = 0; ui < aspMergeMeshes.GetEffectiveSize(); ui++)
    {
        NiMeshPtr pkMeshToMerge = aspMergeMeshes.GetAt(ui);
        NIASSERT(pkMeshToMerge);

        // This function should not be called on particle systems.
        NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMeshToMerge));

        CollapseMeshTransform(pkMeshToMerge);

        NiUInt32 uiNumVerts = pkMesh->GetTotalCount(
            NiCommonSemantics::POSITION(), 0);

        NiUInt32 uiStreamRefCount = pkMeshToMerge->GetStreamRefCount();
        for (NiUInt32 uj = 0; uj < uiStreamRefCount; uj++)
        {
            // For each stream in the mesh, we need to merge the data streams.
            // Note, at this point, the stream and element refs are the same
            // size, stride, etc. so what we are really doing is just
            // concatenating all data streams on the two meshes.
            NiDataStreamRef* pkStreamRefSrc = 
                pkMeshToMerge->GetStreamRefAt(uj);

            NiDataStreamRef* pkStreamRefDst =
                pkMesh->GetStreamRefAt(uj);

            NIASSERT(pkStreamRefSrc->GetElementDescCount() == 1);        
            NIASSERT(pkStreamRefDst->GetElementDescCount() == 1);

            NIASSERT(pkStreamRefSrc->GetSemanticNameAt(0) == 
                pkStreamRefDst->GetSemanticNameAt(0) && 
                pkStreamRefSrc->GetSemanticIndexAt(0) == 
                pkStreamRefDst->GetSemanticIndexAt(0));

            NiUInt32 uiSrcElemCount = pkMeshToMerge->GetTotalCount(
                pkStreamRefSrc->GetSemanticNameAt(0),
                pkStreamRefSrc->GetSemanticIndexAt(0));
            NiUInt32 uiDstElemCount = pkMesh->GetTotalCount(
                pkStreamRefDst->GetSemanticNameAt(0),
                pkStreamRefDst->GetSemanticIndexAt(0));

            NiToolDataStream* pkDstDS = NiVerifyStaticCast(NiToolDataStream,
                pkStreamRefDst->GetDataStream());

            NiToolDataStream* pkSrcDS = NiVerifyStaticCast(NiToolDataStream,
                pkStreamRefSrc->GetDataStream());
        
            pkDstDS->Resize((uiSrcElemCount + uiDstElemCount) * 
                pkDstDS->GetStride());

            void* pvSrcData = pkSrcDS->LockRegion(0, 
                NiDataStream::LOCK_TOOL_READ);
            NIASSERT(pvSrcData);
            if (pkStreamRefDst->GetSemanticNameAt(0) == 
                NiCommonSemantics::INDEX())
            {
                // Need to adjust indices to point to the correct location
                // in the vertex buffer since we are merging streams.
                NiUInt32 uiIndex = 0;
                NiUInt32* puiIndices = (NiUInt32*)pvSrcData;

                while (uiIndex < uiSrcElemCount)
                {
                    *puiIndices += uiNumVerts;
                    puiIndices++;
                    uiIndex++;
                }
            }

            char* pcDstData = (char*)pkDstDS->LockRegion(0, 
                NiDataStream::LOCK_TOOL_WRITE);
            
            NIASSERT(pcDstData);

            pcDstData += (pkDstDS->GetStride() * uiDstElemCount);
            NiMemcpy(pcDstData, pvSrcData, uiSrcElemCount * 
                pkDstDS->GetStride());

            pkDstDS->Unlock(NiDataStream::LOCK_TOOL_WRITE);
            pkSrcDS->Unlock(NiDataStream::LOCK_TOOL_READ);

            NIASSERT(pkDstDS->GetRegionCount() == 1);
            NiDataStream::Region& kRegion = pkDstDS->GetRegion(0);
            kRegion.SetRange(kRegion.GetRange() + uiSrcElemCount);
        }
    }

    RecomputeMeshBounds(pkMesh);
    pkMesh->SetTranslate(NiPoint3::ZERO);
    pkMesh->SetRotate(NiMatrix3::IDENTITY);
    pkMesh->SetScale(1.0f);
    NIASSERT(pkMesh->IsValid());
}
//---------------------------------------------------------------------------
void NiOptimize::CollapseMeshTransform(NiMesh* pkMesh)
{
    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    NiUInt32 uiStreamRefCount = pkMesh->GetStreamRefCount();

    for (NiUInt32 ui = 0; ui < uiStreamRefCount; ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);

        NIASSERT(pkStreamRef->GetElementDescCount() == 1);        

        NiUInt32 uiElemCount = pkMesh->GetTotalCount(
            pkStreamRef->GetSemanticNameAt(0), 
            pkStreamRef->GetSemanticIndexAt(0));

        // Get the data stream from the mesh.
        NiToolDataStream* pkDS = NiVerifyStaticCast(NiToolDataStream, 
            pkStreamRef->GetDataStream());

        // Copy the data stream so that it is unique and we don't inadvertently
        // change the data for any other NiMesh objects sharing this data
        // stream.
        NiToolDataStream* pkDSCopy = NiVerifyStaticCast(NiToolDataStream,
            pkDS->CreateFullCopy());
        pkStreamRef->SetDataStream(pkDSCopy);
        pkDS = pkDSCopy;

        void* pvData = pkDS->LockRegion(0, NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE);

        NIASSERT(pvData);
        NiUInt32 uiStride = pkDS->GetStride();
        
        const NiFixedString& kSemantic = pkStreamRef->GetSemanticNameAt(0);
        if (kSemantic == NiCommonSemantics::POSITION())
        {   
            NiTransform kTransform;
            kTransform.m_Translate = pkMesh->GetTranslate();
            kTransform.m_Rotate = pkMesh->GetRotate();
            kTransform.m_fScale = pkMesh->GetScale();
            if (kTransform.m_Translate != NiPoint3::ZERO ||
                kTransform.m_Rotate != NiMatrix3::IDENTITY ||
                kTransform.m_fScale != 1.0f)
            {
                NiPoint3* pkPoints = NiNew NiPoint3[uiElemCount];
                NIASSERT(uiStride == sizeof(NiPoint3));

                NiProcessorSpecificCode::TransformPoints(uiElemCount, 
                    (const float*)pvData, (float*)&pkPoints[0], &kTransform);

                // Copy the transformed data back to source data stream.
                NiMemcpy(pvData, &pkPoints[0], uiElemCount * 
                    uiStride);

                NiDelete[] pkPoints;
            }                    
        }
        else if (kSemantic == NiCommonSemantics::NORMAL() || 
            kSemantic == NiCommonSemantics::TANGENT() || 
            kSemantic == NiCommonSemantics::BINORMAL())
        {
            NiMatrix3 kRotate = pkMesh->GetRotate();
            if (kRotate != NiMatrix3::IDENTITY)
            {
                NiPoint3* pkVecs = NiNew NiPoint3[uiElemCount];
                NIASSERT(uiStride == sizeof(NiPoint3));

                NiProcessorSpecificCode::TransformVectors(uiElemCount, 
                    (const float*)pvData, (float*)&pkVecs[0], &kRotate);

                // Copy the transformed data back to source data stream.
                NiMemcpy(pvData, &pkVecs[0], uiElemCount * 
                    uiStride);

                NiDelete[] pkVecs;
            }                
        }

        pkDS->Unlock(NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE);
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RecomputeMeshBounds(NiMesh* pkMesh)
{
    NIASSERT(pkMesh);

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    // Compute the bounds from the mesh
    // Force tool lock since this is a tool library
    NiMeshUtilities::ComputeBoundsFromMesh(pkMesh, true);
}
//---------------------------------------------------------------------------
NiTexturingProperty* NiOptimize::FindTexturingProperty(NiAVObject* pkObject)
{
    // Finds the NiTexturingProperty attached to the object or one
    // of its ancestors.

    // Returns: a pointer to the NiTexturingProperty found or NULL if one
    //          was not found.

    NiProperty* pkProp = pkObject->GetProperty(NiProperty::TEXTURING);
    if (!pkProp)
    {
        NiNode* pkParent = pkObject->GetParent();
        if (pkParent)
            return FindTexturingProperty(pkParent);
        else
            return NULL;
    }

    return (NiTexturingProperty*) pkProp;
}
//---------------------------------------------------------------------------
void NiOptimize::BuildTexturePropertyMeshMap(NiAVObject* pkRoot, 
    TextureMeshMap& kTextureMeshMap)
{
    if (NiIsKindOf(NiMesh, pkRoot) &&
        !NiIsKindOf(NiPSParticleSystem, pkRoot) &&
        !CheckForExtraDataTags(pkRoot))
    {
        NiTexturingProperty* pkTexProp = FindTexturingProperty(pkRoot);
        NiMesh* pkMesh = (NiMesh*)pkRoot;

        if (pkTexProp)
        {
            // See if there is already a mesh for this texture property.
            NiTPrimitiveArray<NiMesh*>* pkMeshes;
            if (!kTextureMeshMap.GetAt(pkTexProp, pkMeshes))
            {
                pkMeshes = NiNew NiTPrimitiveArray<NiMesh*>();
                kTextureMeshMap.SetAt(pkTexProp, pkMeshes);
            }

            pkMeshes->Add(pkMesh);
        }
        else
        {            
            // Remove all uv sets since there is not texturing property on 
            // the mesh or its ancestors.
            unsigned int uiCurrStreamRef = 0; 
            
            while (uiCurrStreamRef < pkMesh->GetStreamRefCount())
            {
                NiDataStreamRef* pkStreamRef = 
                    pkMesh->GetStreamRefAt(uiCurrStreamRef);
     
                NIASSERT(pkStreamRef->GetElementDescCount() != 0);
     
                // If the data stream is packed, we aren't going to re-pack it.
                if (pkStreamRef->GetElementDescCount() > 1 || 
                    pkStreamRef->GetSemanticNameAt(0) != 
                    NiCommonSemantics::TEXCOORD())
                {
                    uiCurrStreamRef++;
                }
                else 
                {
                    pkStreamRef->SetDataStream(NULL);
                    pkMesh->RemoveStreamRef(pkStreamRef);
                }
            }

            pkMesh->ResetModifiers();
        }
    }
    else if (NiIsKindOf(NiNode, pkRoot))
    {
        // Recurse over children.
        NiNode* pkNode = (NiNode*)pkRoot;
        unsigned int uiNumChildren = pkNode->GetArrayCount();
        for (unsigned int ui = 0; ui < uiNumChildren; ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                BuildTexturePropertyMeshMap(pkChild, kTextureMeshMap);
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CheckForTransformControllers(NiAVObject* pkObject,
    bool bCheckUpTree)
{
    // Searches for keyframe controllers on the current
    // object and, if bCheckUpTree is true, on its ancestors.

    // Returns: true, if keyframe controllers exist on
    //              this object or anywhere above it if bCheckUpTree is
    //              specified.
    //          false, otherwise.

    NiTransformController* pkKFCtlr =
        NiGetController(NiTransformController, pkObject);

    if (pkKFCtlr)
        return true;

    // Search up the scene graph if desired.
    if (bCheckUpTree)
    {
        NiNode* pkParent = pkObject->GetParent();
        if (pkParent)
        {
            if (CheckForTransformControllers(pkParent, true))
                return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceLinRotKeys(unsigned int uiNumKeys, NiRotKey* pkKeys,
    unsigned int& uiNumNewKeys, NiRotKey*& pkNewKeys, float fRotationTolerance,
    NiAVObject*)
{
    // Removes redundant NiLinRotKeys.

    // Rotation keys featuring quaternions can only be checked for exact
    // redundancies. A tolerance cannot be specified.

    NiTPrimitiveSet<NiLinRotKey*>  apkNewKeys;
    NiLinRotKey* pkPrevKey = NULL;
    NiLinRotKey* pkNextKey = NULL;
    unsigned char ucSize = NiRotKey::GetKeySize(NiRotKey::LINKEY);
    
    // Keep the first key.
    NiLinRotKey* pkKey = (NiLinRotKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiLinRotKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiLinRotKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Check for exact duplicates.
        if (QuatCloseTo(pkPrevKey->GetQuaternion(), pkKey->GetQuaternion(), 
            fRotationTolerance) &&
            QuatCloseTo(pkKey->GetQuaternion(), pkNextKey->GetQuaternion(), 
            fRotationTolerance))
        {
            // Duplicate key; don't keep.
            continue;
        }

        // Check against interpolated rotation.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio = (pkKey->GetTime() - pkPrevKey->GetTime()) /
            fTimeDiff31;

        // if the angle between the opposing quaternions is obtuse, we cannot
        // remove this key
        bool bAcute;
        bAcute = (NiQuaternion::Dot(pkPrevKey->GetQuaternion(), 
            pkNextKey->GetQuaternion()) > 0.0f);

        NiQuaternion kInterpQuat = NiQuaternion::Slerp(fRatio,
            pkPrevKey->GetQuaternion(), pkNextKey->GetQuaternion());
        if (QuatCloseTo(pkKey->GetQuaternion(), kInterpQuat, 
            fRotationTolerance) && bAcute)
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiLinRotKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Reduce keys if only two and identical.
    if (apkNewKeys.GetSize() == 2)
    {
        if (QuatCloseTo(apkNewKeys.GetAt(0)->GetQuaternion(),
            apkNewKeys.GetAt(1)->GetQuaternion(), 
            fRotationTolerance))
        {
            apkNewKeys.RemoveAt(1);
        }
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiLinRotKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::ROTKEY,
            NiAnimationKey::LINKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceLinPosKeys(unsigned int uiNumKeys, NiPosKey* pkKeys,
    unsigned int& uiNumNewKeys, NiPosKey*& pkNewKeys, float fTolerance,
    NiAVObject*)
{
    // Removes redundant NiLinPosKeys.

    NiTPrimitiveSet<NiLinPosKey*>  apkNewKeys;
    NiLinPosKey* pkPrevKey = NULL;
    NiLinPosKey* pkNextKey = NULL;

    unsigned char ucSize = NiPosKey::GetKeySize(NiPosKey::LINKEY);

    // Keep the first key.
    NiLinPosKey* pkKey = (NiLinPosKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);


    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiLinPosKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiLinPosKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Check for exact duplicates.
        if (Point3CloseTo(pkPrevKey->GetPos(), pkKey->GetPos()) &&
            Point3CloseTo(pkKey->GetPos(), pkNextKey->GetPos()))
        {
            // Duplicate key; don't keep.
            continue;
        }

        // Check against interpolated position.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio = (pkKey->GetTime() - pkPrevKey->GetTime()) /
            fTimeDiff31;
        NiPoint3 kPosDiff31 = pkNextKey->GetPos() - pkPrevKey->GetPos();
        NiPoint3 kInterpPos = pkPrevKey->GetPos() + fRatio * kPosDiff31;
        if ((pkKey->GetPos() - kInterpPos).Length() <=
            fTolerance * kPosDiff31.Length())
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiLinPosKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Reduce keys if only two and identical.
    if (apkNewKeys.GetSize() == 2)
    {
        if (Point3CloseTo(apkNewKeys.GetAt(0)->GetPos(),
            apkNewKeys.GetAt(1)->GetPos()))
        {
            apkNewKeys.RemoveAt(1);
        }
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiLinPosKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::POSKEY,
            NiAnimationKey::LINKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceLinFloatKeys(unsigned int uiNumKeys,
    NiFloatKey* pkKeys, unsigned int& uiNumNewKeys,
    NiFloatKey*& pkNewKeys, float fTolerance)
{
    // Removes redundant NiLinFloatKeys.

    NiTPrimitiveSet<NiLinFloatKey*> apkNewKeys;
    NiLinFloatKey* pkPrevKey = NULL;
    NiLinFloatKey* pkNextKey = NULL;

    unsigned char ucSize = NiFloatKey::GetKeySize(NiFloatKey::LINKEY);

    // Keep the first key.
    NiLinFloatKey* pkKey = (NiLinFloatKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiLinFloatKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiLinFloatKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Check for exact duplicates.
        if (CloseTo(pkPrevKey->GetValue(), pkKey->GetValue()) &&
            CloseTo(pkKey->GetValue(), pkNextKey->GetValue()))
        {
            // Duplicate key; don't keep.
            continue;
        }

        // Check against interpolated value.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio = (pkKey->GetTime() - pkPrevKey->GetTime()) /
            fTimeDiff31;
        float fValueDiff31 = pkNextKey->GetValue() - pkPrevKey->GetValue();
        float fInterpValue = pkPrevKey->GetValue() + fRatio * fValueDiff31;
        if (CloseTo(pkKey->GetValue(), fInterpValue, fTolerance *
            (float) fabs(fValueDiff31)))
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiLinFloatKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Reduce keys if only two and identical.
    if (apkNewKeys.GetSize() == 2)
    {
        if (CloseTo(apkNewKeys.GetAt(0)->GetValue(),
            apkNewKeys.GetAt(1)->GetValue()))
        {
            apkNewKeys.RemoveAt(1);
        }
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiLinFloatKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::FLOATKEY,
            NiAnimationKey::LINKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize),
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceEulerRotKeys(unsigned int uiNumKeys,
    NiRotKey* pkKeys, float fTolerance)
{
    NI_UNUSED_ARG(uiNumKeys);
    unsigned char ucSize = NiRotKey::GetKeySize(NiRotKey::EULERKEY);

    // Removes redundant NiEulerRotKeys.
    NIASSERT(uiNumKeys == 1);
    NiEulerRotKey* pkKey = (NiEulerRotKey*) pkKeys->GetKeyAt(0, ucSize);

    for (unsigned char ucIndex = 0; ucIndex < 3; ucIndex++)
    {
        bool bKeysReduced = false;
        unsigned int uiNumFloatKeys = pkKey->GetNumKeys(ucIndex);
        NiFloatKey* pkFloatKeys = pkKey->GetKeys(ucIndex);
        NiFloatKey::KeyType eFloatKeyType = pkKey->GetType(ucIndex);
        unsigned int uiNumNewFloatKeys = 0;
        NiFloatKey* pkNewFloatKeys = NULL;

        if (uiNumFloatKeys > 1)
        {
            switch(eFloatKeyType)
            {
                case NiFloatKey::LINKEY:
                    ReduceLinFloatKeys(uiNumFloatKeys, pkFloatKeys,
                        uiNumNewFloatKeys, pkNewFloatKeys, fTolerance);
                    bKeysReduced = true;
                    break;
                case NiFloatKey::BEZKEY:
                    ReduceBezFloatKeys(uiNumFloatKeys, pkFloatKeys,
                        uiNumNewFloatKeys, pkNewFloatKeys, fTolerance);
                    bKeysReduced = true;
                    break;
                default:
                    break;
            }
        }
        if (bKeysReduced)
        {
            pkKey->ReplaceKeys(ucIndex, uiNumNewFloatKeys, pkNewFloatKeys);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceBezRotKeys(unsigned int uiNumKeys, NiRotKey* pkKeys,
    unsigned int& uiNumNewKeys, NiRotKey*& pkNewKeys, float fRotationTolerance,
    NiAVObject*)    
{
    // Removes redundant NiBezRotKeys.

    // Rotation keys featuring quaternions can only be checked for exact
    // redundancies. A tolerance cannot be specified.

    NiTPrimitiveSet<NiBezRotKey*> apkNewKeys;
    NiBezRotKey* pkPrevKey = NULL;
    NiBezRotKey* pkNextKey = NULL;

    // Get fill derived values function.
    NiRotKey::FillDerivedValsFunction pfnFillDerived =
        NiRotKey::GetFillDerivedFunction(NiAnimationKey::BEZKEY);
    NIASSERT(pfnFillDerived);

    unsigned char ucSize = NiRotKey::GetKeySize(NiRotKey::BEZKEY);

    // Keep the first key.
    NiBezRotKey* pkKey = (NiBezRotKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiBezRotKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiBezRotKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Interpolate seven times into original curve.
        float fTimeDiff21 = pkKey->GetTime() - pkPrevKey->GetTime();
        float fTimeDiff32 = pkNextKey->GetTime() - pkKey->GetTime();
        float fHalfTimeDiff21 = fTimeDiff21 * 0.5f;
        float fHalfTimeDiff32 = fTimeDiff32 * 0.5f;
        float fQuarterTimeDiff21 = fHalfTimeDiff21 * 0.5f;
        float fQuarterTimeDiff32 = fHalfTimeDiff32 * 0.5f;
        unsigned int uiTemp;
        NiQuaternion kOrigInterp1 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp2 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp3 = NiRotKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp4 = pkKey->GetQuaternion();
        NiQuaternion kOrigInterp5 = NiRotKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp6 = NiRotKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp7 = NiRotKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);

        // Construct new array with just prev and next keys.
        NiBezRotKey akTempKeys[2];
        akTempKeys[0].SetTime(pkPrevKey->GetTime());
        akTempKeys[0].SetQuaternion(pkPrevKey->GetQuaternion());
        akTempKeys[1].SetTime(pkNextKey->GetTime());
        akTempKeys[1].SetQuaternion(pkNextKey->GetQuaternion());
        pfnFillDerived(&akTempKeys[0], 2, ucSize);
        
        // Interpolate seven times into the new curve.
        NiQuaternion kNewInterp1 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp2 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp3 = NiRotKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp4 = NiRotKey::GenInterp(pkKey->GetTime(),
            &akTempKeys[0], NiAnimationKey::BEZKEY, 2, uiTemp = 0, ucSize);
        NiQuaternion kNewInterp5 = NiRotKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp6 = NiRotKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp7 = NiRotKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);

        // Compare the interpolated values.
        if (QuatCloseTo(kOrigInterp1, kNewInterp1, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp2, kNewInterp2, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp3, kNewInterp3, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp4, kNewInterp4, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp5, kNewInterp5, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp6, kNewInterp6, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp7, kNewInterp7, fRotationTolerance))
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiBezRotKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiBezRotKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::ROTKEY,
            NiAnimationKey::BEZKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceBezPosKeys(unsigned int uiNumKeys, NiPosKey* pkKeys,
    unsigned int& uiNumNewKeys, NiPosKey*& pkNewKeys, float fTolerance,
    NiAVObject*)
{
    // Removes redundant NiBezPosKeys.

    NiTPrimitiveSet<NiBezPosKey*>  apkNewKeys;
    NiBezPosKey* pkPrevKey = NULL;
    NiBezPosKey* pkNextKey = NULL;

    // Get fill derived values function.
    NiPosKey::FillDerivedValsFunction pfnFillDerived =
        NiPosKey::GetFillDerivedFunction(NiAnimationKey::BEZKEY);
    NIASSERT(pfnFillDerived);

    unsigned char ucSize = NiPosKey::GetKeySize(NiPosKey::BEZKEY);

    // Keep the first key.
    NiBezPosKey* pkKey = (NiBezPosKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);


    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiBezPosKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiBezPosKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Interpolate seven times into original curve.
        float fTimeDiff21 = pkKey->GetTime() - pkPrevKey->GetTime();
        float fTimeDiff32 = pkNextKey->GetTime() - pkKey->GetTime();
        float fHalfTimeDiff21 = fTimeDiff21 * 0.5f;
        float fHalfTimeDiff32 = fTimeDiff32 * 0.5f;
        float fQuarterTimeDiff21 = fHalfTimeDiff21 * 0.5f;
        float fQuarterTimeDiff32 = fHalfTimeDiff32 * 0.5f;
        unsigned int uiTemp;
        NiPoint3 kOrigInterp1 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp2 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp3 = NiPosKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp4 = pkKey->GetPos();
        NiPoint3 kOrigInterp5 = NiPosKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp6 = NiPosKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp7 = NiPosKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);

        // Construct new array with just prev and next keys.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio12 = fTimeDiff21 / fTimeDiff31;
        float fRatio23 = fTimeDiff32 / fTimeDiff31;
        NiBezPosKey akTempKeys[2];
        akTempKeys[0].SetTime(pkPrevKey->GetTime());
        akTempKeys[0].SetPos(pkPrevKey->GetPos());
        akTempKeys[0].SetOutTan(pkPrevKey->GetOutTan() * fRatio12);
        akTempKeys[1].SetTime(pkNextKey->GetTime());
        akTempKeys[1].SetPos(pkNextKey->GetPos());
        akTempKeys[1].SetInTan(pkNextKey->GetInTan() * fRatio23);
        pfnFillDerived(&akTempKeys[0], 2, ucSize);
        
        // Interpolate seven times into the new curve.
        NiPoint3 kNewInterp1 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp2 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp3 = NiPosKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp4 = NiPosKey::GenInterp(pkKey->GetTime(),
            &akTempKeys[0], NiAnimationKey::BEZKEY, 2, uiTemp = 0, ucSize);
        NiPoint3 kNewInterp5 = NiPosKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp6 = NiPosKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp7 = NiPosKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);

        // Compare the interpolated values.
        NiPoint3 kPosDiff31 = pkNextKey->GetPos() - pkPrevKey->GetPos();
        float fPosTolerance = fTolerance * kPosDiff31.Length();
        if ((kOrigInterp1 - kNewInterp1).Length() <= fPosTolerance &&
            (kOrigInterp2 - kNewInterp2).Length() <= fPosTolerance &&
            (kOrigInterp3 - kNewInterp3).Length() <= fPosTolerance &&
            (kOrigInterp4 - kNewInterp4).Length() <= fPosTolerance &&
            (kOrigInterp5 - kNewInterp5).Length() <= fPosTolerance &&
            (kOrigInterp6 - kNewInterp6).Length() <= fPosTolerance &&
            (kOrigInterp7 - kNewInterp7).Length() <= fPosTolerance)
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiBezPosKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiBezPosKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::POSKEY,
            NiAnimationKey::BEZKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceBezFloatKeys(unsigned int uiNumKeys,
    NiFloatKey* pkKeys, unsigned int& uiNumNewKeys,
    NiFloatKey*& pkNewKeys, float fTolerance)
{
    // Removes redundant NiBezFloatKeys.

    NiTPrimitiveSet<NiBezFloatKey*>  apkNewKeys;
    NiBezFloatKey* pkPrevKey = NULL;
    NiBezFloatKey* pkNextKey = NULL;

    unsigned char ucSize = NiFloatKey::GetKeySize(NiFloatKey::BEZKEY);

    // Keep the first key.
    NiBezFloatKey* pkKey = (NiBezFloatKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiBezFloatKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiBezFloatKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Interpolate seven times into original curve.
        float fTimeDiff21 = pkKey->GetTime() - pkPrevKey->GetTime();
        float fTimeDiff32 = pkNextKey->GetTime() - pkKey->GetTime();
        float fHalfTimeDiff21 = fTimeDiff21 * 0.5f;
        float fHalfTimeDiff32 = fTimeDiff32 * 0.5f;
        float fQuarterTimeDiff21 = fHalfTimeDiff21 * 0.5f;
        float fQuarterTimeDiff32 = fHalfTimeDiff32 * 0.5f;
        unsigned int uiTemp;
        float fOrigInterp1 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp2 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp3 = NiFloatKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp4 = pkKey->GetValue();
        float fOrigInterp5 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp6 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp7 = NiFloatKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);

        // Construct new array with just prev and next keys.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio12 = fTimeDiff21 / fTimeDiff31;
        float fRatio23 = fTimeDiff32 / fTimeDiff31;
        NiBezFloatKey akTempKeys[2];
        akTempKeys[0].SetTime(pkPrevKey->GetTime());
        akTempKeys[0].SetValue(pkPrevKey->GetValue());
        akTempKeys[0].SetOutTan(pkPrevKey->GetOutTan() * fRatio12);
        akTempKeys[1].SetTime(pkNextKey->GetTime());
        akTempKeys[1].SetValue(pkNextKey->GetValue());
        akTempKeys[1].SetInTan(pkNextKey->GetInTan() * fRatio23);
        // There are no derived values to fill here.
        
        // Interpolate seven times into the new curve.
        float fNewInterp1 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp2 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp3 = NiFloatKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp4 = NiFloatKey::GenInterp(pkKey->GetTime(),
            &akTempKeys[0], NiAnimationKey::BEZKEY, 2, uiTemp = 0, ucSize);
        float fNewInterp5 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp6 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp7 = NiFloatKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);

        // Compare the interpolated values.
        float fValueDiff31 = pkNextKey->GetValue() - pkPrevKey->GetValue();
        float fValueTolerance = fTolerance * (float) fabs(fValueDiff31);
        if (CloseTo(fOrigInterp1, fNewInterp1, fValueTolerance) &&
            CloseTo(fOrigInterp2, fNewInterp2, fValueTolerance) &&
            CloseTo(fOrigInterp3, fNewInterp3, fValueTolerance) &&
            CloseTo(fOrigInterp4, fNewInterp4, fValueTolerance) &&
            CloseTo(fOrigInterp5, fNewInterp5, fValueTolerance) &&
            CloseTo(fOrigInterp6, fNewInterp6, fValueTolerance) &&
            CloseTo(fOrigInterp7, fNewInterp7, fValueTolerance))
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiBezFloatKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiBezFloatKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::FLOATKEY,
            NiAnimationKey::BEZKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::CollectBoneChildren(NiAVObject* pkObject,
    NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Stores a pointer to each child of bones in the scene to kMap.

    NiSkinningMeshModifier* pkSkin = NULL;
    NiMesh* pkMesh = NiDynamicCast(NiMesh, pkObject);
    if (pkMesh)
    {
        pkSkin = NiGetModifier(NiSkinningMeshModifier, pkMesh);
    }

    if (pkSkin)
    {
        NiUInt32 uiBones = pkSkin->GetBoneCount();
        NiAVObject*const* ppkBones = pkSkin->GetBones();
        
        for (NiUInt32 ui = 0; ui < uiBones; ui++)
        {
            NiAVObject* pkBone = ppkBones[ui];
            if (pkBone && NiIsKindOf(NiNode, pkBone))
            {
                NiNode* pkNode = (NiNode*) pkBone;
                for (unsigned int uj = 0; uj < pkNode->GetArrayCount(); uj++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(uj);
                    if (pkChild)
                    {
                        kMap.SetAt(pkChild, true);
                    }
                }
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                CollectBoneChildren(pkChild, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
void RemoveBySemantic(NiToolPipelineCloneHelper& kCloneHelper,
    NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet, 
    const NiFixedString& kSemantic)
{
    NiMesh* pkMesh = spCloneSet->GetAt(0);

    NiInt32 uiSemanticIndexCount = pkMesh->GetMaxIndexBySemantic(
        kSemantic);

    if(uiSemanticIndexCount == -1)
        return;

    for(NiInt32 uiSemanticIndex = 0; uiSemanticIndex <= uiSemanticIndexCount;
        uiSemanticIndex++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->FindStreamRef(kSemantic, uiSemanticIndex);

        if(pkStreamRef != NULL)
        {
            kCloneHelper.ReleaseStreamRef(pkStreamRef);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::OptimizeSkinModifier(NiToolPipelineCloneHelper& kCloneHelper,
    NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
    NiTPrimitiveSet<NiSkinningMeshModifier*>& kSkinModList,
    NiTPrimitiveSet<NiMorphMeshModifier*>& kMorphModList, bool bHardwareSkin,
    bool bSoftwareSkin, unsigned int uiBonesPerPartition, bool)
{
    // Assume all mesh in cloneset have same requirements / modifiers
    NiMesh* pkMesh = spCloneSet->GetAt(0);
    NIASSERT(pkMesh->IsValid());

    // Creates a skin partition and removes bone vert data, if appropriate.

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    unsigned int uiMaterialCount = pkMesh->GetMaterialCount();
    if ((uiMaterialCount != 0) &&
        (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() != 
        NiSystemDesc::RENDERER_WII))
    {
        // By default shaders hardware skin, unless exporting for Wii
        bHardwareSkin = true;
        bSoftwareSkin = false;

        // For mesh-clones, it is assumed that NI_BONES_PER_PARTITION_KEY has
        // the same value across all clones.
        //
        // Force Software Skinning IF the bones per partition are 0
        // It is assumed the extra data is the same on any mesh clone
        NiIntegerExtraData* pkIntED = NiDynamicCast(NiIntegerExtraData, 
            pkMesh->GetExtraData(NI_BONES_PER_PARTITION_KEY));
        if (pkIntED && (pkIntED->GetValue() <= 0))
        {
            bHardwareSkin = false;
            bSoftwareSkin = true;
        }
    }

    // For mesh-clones, it is assumed that the return value of GetInstanced 
    // is the same value across all clones.
    //
    // Force software skinned if we are instanced.
    if (pkMesh->GetInstanced()) // Assume same for all clonesets
    {
        bHardwareSkin = false;
        bSoftwareSkin = true;
    }

    NIASSERT(bSoftwareSkin != bHardwareSkin);

    if (bSoftwareSkin)
    {
        NiUInt32 uiSkinModCnt = kSkinModList.GetSize();
        for(NiUInt32 uiSkin=0; uiSkin < uiSkinModCnt; uiSkin++)
        {
            NiSkinningMeshModifier* pkSkinMod = kSkinModList.GetAt(uiSkin);
            NIASSERT(pkSkinMod);
            pkSkinMod->SetSoftwareSkinned(true);
        }

        NiUInt32 uiMorphModCnt = kMorphModList.GetSize();
        for(NiUInt32 uiMorph=0; uiMorph < uiMorphModCnt; uiMorph++)
        {
            NiMorphMeshModifier* pkMorphMod = kMorphModList.GetAt(uiMorph);
            NIASSERT(pkMorphMod);
            pkMorphMod->SetSWSkinned(true);
        }

        //we need to make changes to the streams so we can read them on the
        //cpu side

        //we need to change NBT and positions to writable.

        MakeWritableFromSemantic(NiCommonSemantics::POSITION(), pkMesh);
        MakeWritableFromSemantic(NiCommonSemantics::NORMAL(), pkMesh);
        MakeWritableFromSemantic(NiCommonSemantics::TANGENT(), pkMesh);
        MakeWritableFromSemantic(NiCommonSemantics::BINORMAL(), pkMesh);

        MakeReadableFromSemantic(NiCommonSemantics::BLENDINDICES(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::BLENDWEIGHT(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::BONE_PALETTE(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::BONEMATRICES(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::NORMAL_BP(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::TANGENT_BP(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::BINORMAL_BP(), pkMesh);
        MakeReadableFromSemantic(NiCommonSemantics::POSITION_BP(), pkMesh);
    }

    if (bHardwareSkin)
    {
        // For mesh-clones, it is assumed that NI_BONES_PER_PARTITION_KEY has
        // the same value across all clones.
        NiIntegerExtraData* pkIntED = NiDynamicCast(NiIntegerExtraData, 
            pkMesh->GetExtraData(NI_BONES_PER_PARTITION_KEY));
        if (pkIntED)
        {
            unsigned int uiTemp = pkIntED->GetValue();
            if (uiTemp > 0)
                uiBonesPerPartition = uiTemp;
        }

        NiUInt32 uiSkinModCnt = kSkinModList.GetSize();
        for(NiUInt32 uiSkin=0; uiSkin < uiSkinModCnt; uiSkin++)
        {
            NiSkinningMeshModifier* pkSkinMod = kSkinModList.GetAt(uiSkin);
            NIASSERT(pkSkinMod);
            pkSkinMod->SetSoftwareSkinned(false);
        }

        NiUInt32 uiMorphModCnt = kMorphModList.GetSize();
        for(NiUInt32 uiMorph=0; uiMorph < uiMorphModCnt; uiMorph++)
        {
            NiMorphMeshModifier* pkMorphMod = kMorphModList.GetAt(uiMorph);
            NIASSERT(pkMorphMod);
            pkMorphMod->SetSWSkinned(false);
        }

        // Inherit the access mask from the renderable data streams. This is to
        // make sure we preserve any access masks that were previously set.
        // Mesh emitters need this to have the streams remain CPU_READable.
        InheritAccessMaskFromSemantic(NiCommonSemantics::POSITION(),
            NiCommonSemantics::POSITION_BP(), pkMesh);
        InheritAccessMaskFromSemantic(NiCommonSemantics::POSITION(),
            NiCommonSemantics::BLENDINDICES(), pkMesh);
        InheritAccessMaskFromSemantic(NiCommonSemantics::POSITION(),
            NiCommonSemantics::BLENDWEIGHT(), pkMesh);
        InheritAccessMaskFromSemantic(NiCommonSemantics::NORMAL(),
            NiCommonSemantics::NORMAL_BP(), pkMesh);
        InheritAccessMaskFromSemantic(NiCommonSemantics::TANGENT(),
            NiCommonSemantics::TANGENT_BP(), pkMesh);
        InheritAccessMaskFromSemantic(NiCommonSemantics::BINORMAL(),
            NiCommonSemantics::BINORMAL_BP(), pkMesh);

        NIVERIFY(NiSkinningUtilities::MakeSubmeshesForGPUSkinning(kCloneHelper,
            spCloneSet, (unsigned char)uiBonesPerPartition));

        //We remove the position and nbt's stream because the _BP ones 
        //are used instead
        RemoveBySemantic(kCloneHelper, spCloneSet,
            NiCommonSemantics::POSITION());
        RemoveBySemantic(kCloneHelper, spCloneSet,
            NiCommonSemantics::NORMAL());
        RemoveBySemantic(kCloneHelper, spCloneSet,
            NiCommonSemantics::BINORMAL());
        RemoveBySemantic(kCloneHelper, spCloneSet,
            NiCommonSemantics::TANGENT());

    }
    NIASSERT(pkMesh->IsValid());
}
//---------------------------------------------------------------------------
void NiOptimize::MakeReadableFromSemantic(const NiFixedString& kSemantic, 
    NiMesh* pkMesh)
{
    //get the old access flags
    NiInt32 uiSemanticIndexCount = pkMesh->GetMaxIndexBySemantic(
        kSemantic);

    if(uiSemanticIndexCount == -1)
        return;

    for(NiInt32 uiSemanticIndex = 0; uiSemanticIndex <= uiSemanticIndexCount;
        uiSemanticIndex++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->FindStreamRef(kSemantic, uiSemanticIndex);

        if(pkStreamRef != NULL)
        {
            NiDataStream* pkStream = pkStreamRef->GetDataStream();
            NiToolDataStream* pkToolDataStream = NiDynamicCast(
                NiToolDataStream, pkStream);
            if(pkToolDataStream != NULL)
            {
                pkToolDataStream->MakeReadable();
            }

        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::MakeWritableFromSemantic(const NiFixedString& kSemantic, 
    NiMesh* pkMesh)
{
    //get the old access flags
    NiInt32 uiSemanticIndexCount = pkMesh->GetMaxIndexBySemantic(
        kSemantic);

    if(uiSemanticIndexCount == -1)
        return;

    for(NiInt32 uiSemanticIndex = 0; uiSemanticIndex <= uiSemanticIndexCount;
        uiSemanticIndex++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->FindStreamRef(kSemantic, uiSemanticIndex);

        if(pkStreamRef != NULL)
        {
            NiDataStream* pkStream = pkStreamRef->GetDataStream();
            NiToolDataStream* pkToolDataStream = NiDynamicCast(
                NiToolDataStream, pkStream);
            if(pkToolDataStream != NULL)
            {
                pkToolDataStream->MakeWritable();
            }

        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::InheritAccessMaskFromSemantic(
    const NiFixedString& kBaseSemantic, const NiFixedString& kSemantic, 
    NiMesh* pkMesh)
{
    NiInt32 uiBaseSemanticIndexCount = pkMesh->GetMaxIndexBySemantic(
        kBaseSemantic);

    //get the old access flags
    NiInt32 uiSemanticIndexCount = pkMesh->GetMaxIndexBySemantic(
        kSemantic);

    NIASSERT(!(uiBaseSemanticIndexCount == -1 && uiSemanticIndexCount > -1))
    if(uiBaseSemanticIndexCount == -1)
        return;

    if(uiSemanticIndexCount == -1)
        return;

    // check for morphing to see if we need to make data mutable
    NiMorphMeshModifier* pkMorphModifier =
        NiGetModifier(NiMorphMeshModifier, pkMesh);

    for(NiInt32 uiSemanticIndex = 0; uiSemanticIndex <= uiSemanticIndexCount;
        uiSemanticIndex++)
    {
        // Get the base semantic index if it is within range.
        NiUInt32 uiBaseSemanticIndex = 
            (uiSemanticIndex <= uiBaseSemanticIndexCount) ?
                uiSemanticIndex : 0;

        NiDataStreamRef* pkBaseStreamRef = 
            pkMesh->FindStreamRef(kBaseSemantic, uiBaseSemanticIndex);

        if(pkBaseStreamRef != NULL)
        {
            NiDataStreamRef* pkStreamRef = 
                pkMesh->FindStreamRef(kSemantic, uiSemanticIndex);

            if(pkStreamRef != NULL)
            {
                NiDataStream* pkStream = pkStreamRef->GetDataStream();
                NiToolDataStream* pkToolDataStream = NiDynamicCast(
                    NiToolDataStream, pkStream);
                if(pkToolDataStream != NULL)
                {
                    NiUInt8 uiAccessMask = pkBaseStreamRef->GetAccessMask();
                    uiAccessMask &= ~NiDataStream::ACCESS_CPU_WRITE_ANY;
                    uiAccessMask |= (pkMorphModifier == NULL) ?
                        NiDataStream::ACCESS_CPU_WRITE_STATIC :
                        NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
                    pkToolDataStream->SetAccessMask(uiAccessMask);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CalculateBoneBounds(NiMesh* pkMesh)
{
    // this function will calculate the bounds of each bone that is influencing
    // a given mesh. It does this by finding each vert that is influenced
    // by a particular bone and creating a bound that contains them all.
    // It makes the following assumptions:
    //  * The mesh has a filled in Position data stream 
    //  * The mesh has a skin modifier attached
    //  * The mesh has a filled in bone indices data stream
    //  * The indices data stream
    //  * The skin modifier has a bone list and skin to bone transform list set
    //  * There are 4 bones per vertex

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    NiUInt32 uiBonesPerVert = 4;

    // make sure the mesh has a skinning modifier and grab it
    NiSkinningMeshModifier* pkSkin = NiGetModifier(NiSkinningMeshModifier, 
        pkMesh);
    if (!pkSkin)
        return false;

    // get the relevant data from the skin modifier
    NiUInt32 uiBoneCount = pkSkin->GetBoneCount();
    NiTransform* pkSkinToBoneXForms = pkSkin->GetSkinToBoneTransforms();

    // get iterators for the position, bone index, and weight data streams
    NiDataStreamElementLock kPositionLock = NiDataStreamElementLock(pkMesh,
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_TOOL_READ);

    if (!kPositionLock.IsLocked())
        return false;

    NiDataStreamElementLock kIndexLock = NiDataStreamElementLock(pkMesh,
        NiCommonSemantics::BLENDINDICES(), 0, NiDataStreamElement::F_INT16_4,
        NiDataStream::LOCK_TOOL_READ);

    if (!kIndexLock.IsLocked())
        return false;
    
    NiDataStreamElementLock kWeightLock = NiDataStreamElementLock(pkMesh,
        NiCommonSemantics::BLENDWEIGHT(), 0, NiDataStreamElement::F_FLOAT32_4,
        NiDataStream::LOCK_TOOL_READ);

    if (!kWeightLock.IsLocked())
        return false;
    
    // this keeps track of all the verts that a bone references
    NiTObjectSet<NiPoint3>* pkVertsPerBone = NiNew 
        NiTObjectSet<NiPoint3>[uiBoneCount];

    // Add the data per submesh
    for (NiUInt32 uiSubmeshIdx = 0; 
        uiSubmeshIdx < kPositionLock.GetSubmeshCount(); uiSubmeshIdx++)
    {
        NiTStridedRandomAccessIterator<NiPoint3> kPositions =
            kPositionLock.begin<NiPoint3>(uiSubmeshIdx);
        
        NiTStridedRandomAccessIterator<NiTSimpleArray<NiInt16, 4> > kIndices =
            kIndexLock.begin<NiTSimpleArray<NiInt16, 4> >(uiSubmeshIdx);

        NiTStridedRandomAccessIterator<NiTSimpleArray<float, 4> > kWeights =
            kWeightLock.begin<NiTSimpleArray<float, 4> >(uiSubmeshIdx);

        // iterate over each vertex and add itself to the verts per bone for  
        // each bone that it references
        for (NiUInt32 uiVert = 0; uiVert < kPositionLock.count(uiSubmeshIdx);
            uiVert++)
        {
            for (NiUInt32 uiBone = 0; uiBone < uiBonesPerVert; uiBone++)
            {
                // only consider bones that have nonzero influence
                if (kWeights[uiVert][uiBone] > 0.0f)
                {
                    pkVertsPerBone[kIndices[uiVert][uiBone]].Add(
                        kPositions[uiVert]);
                }
            }
        }
    }

    // Allocate the bounds array
    pkSkin->EnableDynamicBounds();

    NiBound* pkBounds = pkSkin->GetBoneBounds();

    // now iterate over each bone and figure out its bounds now that we have
    // collected all of the points that the bone influences
    for (NiUInt32 uiBone = 0; uiBone < uiBoneCount; uiBone++)
    {
        pkBounds[uiBone].ComputeFromData(pkVertsPerBone[uiBone].GetSize(),
            pkVertsPerBone[uiBone].GetBase());
        pkBounds[uiBone].Update(pkBounds[uiBone], pkSkinToBoneXForms[uiBone]);
        pkVertsPerBone[uiBone].RemoveAll();
    }

    NiDelete[] pkVertsPerBone;
    return true;
}
//---------------------------------------------------------------------------
bool NiOptimize::NonDegenerateTriangle(unsigned int uiI0, unsigned uiI1,
    unsigned int uiI2, const NiPoint3& kP0, const NiPoint3& kP1, 
    const NiPoint3& kP2)
{
    // check for degenerate connectivity
    if (uiI0 == uiI1 || uiI0 == uiI2 || uiI1 == uiI2)
    {
         return false;
    }

    // check for short edges
    const float fMinLenSqr = 1e-30f;

    NiPoint3 e10 = kP1 - kP0;
    float fLenSqrE10 = e10.SqrLength();
    if (fLenSqrE10 < fMinLenSqr)
        return false;

    NiPoint3 e20 = kP2 - kP0;
    float fLenSqrE20 = e20.SqrLength();
    if (fLenSqrE20 < fMinLenSqr)
        return false;

    NiPoint3 e21 = kP2 - kP1;
    float fLenSqrE21 = e21.SqrLength();
    if (fLenSqrE21 < fMinLenSqr)
        return false;

    // check for slivers
    const float fMinEdgeRatio = 1e-7f;
    const float fMaxEdgeRatio = 1e+7f;

    float fEdgeRatio = fLenSqrE10 / fLenSqrE20;
    if (fEdgeRatio < fMinEdgeRatio || fEdgeRatio > fMaxEdgeRatio)
        return false;

    // check for small area triangles
    const float fMinSineSqr = 1e-7f;

    NiPoint3 cross = e10.Cross(e20);
    float fLenSqrCross = cross.SqrLength();
    if (fLenSqrCross < fLenSqrE10 * fLenSqrE20 * fMinSineSqr)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiOptimize::BuildElementDataSet(NiMesh* pkMesh, 
    NiTObjectSet<NiDataStreamPtr>& kLockSet, 
    NiTObjectSet<NiOptimize::ElementData>& kDataSet)
{
    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    // this function fills in the parallel lock and element data arrays using
    // the information contained in the mesh
    for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);
        if (pkStreamRef == NULL)
            continue;

        NIASSERT(pkStreamRef->GetElementDescCount() == 1);
        if (pkStreamRef->GetElementDescCount() != 1)
            return false;

        NiDataStream* pkStream = pkStreamRef->GetDataStream();
        NIASSERT(pkStreamRef->GetDataStream());
        if (pkStreamRef->GetDataStream()->GetUsage() == 
            NiDataStream::USAGE_VERTEX && !pkStreamRef->IsPerInstance())
        {
            // if we've found a vertex stream, add it to the sets
            void* pvData = pkStream->Lock(NiDataStream::LOCK_TOOL_READ);
            kLockSet.Add(pkStream);

            NiOptimize::ElementData kElementData;
            kElementData.kBegin = NiTStridedRandomAccessIterator<NiUInt8>(
                (NiUInt8*) pvData, pkStream->GetStride());
            kElementData.kEnd = NiTStridedRandomAccessIterator<NiUInt8>(
                ((NiUInt8*) pvData) + pkStream->GetTotalCount() * 
                pkStream->GetStride(), pkStream->GetStride());

            NiDataStreamElement kElement = pkStream->GetElementDescAt(0);
            kElementData.eFormat = kElement.GetFormat();
            kDataSet.Add(kElementData);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
NiInt32 NiOptimize::VertexCompare(NiUInt32 ui0, NiUInt32 ui1, 
    NiTObjectSet<NiOptimize::ElementData>& kDataSet)
{
    // This function must return -1 when a vert 0 is "less than" vert 1, 
    // return 1 when 0 is "greater than" 1 and return 0 when they are equal

    for (NiUInt32 ui = 0; ui < kDataSet.GetSize(); ui++)
    {
        NiOptimize::ElementData kElement = kDataSet.GetAt(ui);
        NiInt32 iResult = memcmp((void*)&kElement.kBegin[ui0], 
            (void*)&kElement.kBegin[ui1], 
            NiDataStreamElement::SizeOfFormat(kElement.eFormat));
        if (iResult > 0) 
            return 1;
        else if (iResult < 0)
            return -1;
    }

    return 0;
}
//---------------------------------------------------------------------------
void NiOptimize::IndexQSort(NiUInt32* puiIndex, NiInt32 iL, NiInt32 iR,
    NiTObjectSet<NiOptimize::ElementData>& kDataSet)
{
    if (iR > iL)
    {
        int iLower = iL - 1;
        int iUpper = iR + 1;
        int iPivot = ChoosePivot(puiIndex, iL, iR, kDataSet);

        for ( ; ; )
        {
            do 
            {
                iUpper--;
            }
            while (VertexCompare(iPivot, puiIndex[iUpper], kDataSet) < 0);

            do
            {
                iLower++;
            }
            while (VertexCompare(puiIndex[iLower], iPivot, kDataSet) < 0);

            if (iLower < iUpper)
            {
                unsigned int uiTmp = puiIndex[iLower];
                puiIndex[iLower] = puiIndex[iUpper];
                puiIndex[iUpper] = uiTmp;
            }
            else
            {
                break;
            }
        }

        if (iUpper == iR)
        {
            IndexQSort(puiIndex, iL, iUpper - 1, kDataSet);
        }
        else
        {
            IndexQSort(puiIndex, iL, iUpper, kDataSet);
            IndexQSort(puiIndex, iUpper + 1, iR, kDataSet);
        }
    }
}
//---------------------------------------------------------------------------
NiInt32 NiOptimize::ChoosePivot(NiUInt32* puiIndex, NiInt32 iL, NiInt32 iR,
    NiTObjectSet<NiOptimize::ElementData>& kDataSet)
{
    // Check the first, middle, and last element. Choose the one which falls
    // between the other two. This has a good chance of discouraging 
    // quadratic behavior from qsort.  In the case when all three are equal,
    // this code chooses the middle element, which will prevent quadratic
    // behavior for a list with all elements equal.

    int iM = (iL + iR) >> 1;

    if (VertexCompare(puiIndex[iL], puiIndex[iM], kDataSet) < 0)
    {
        if (VertexCompare(puiIndex[iM], puiIndex[iR], kDataSet) < 0)
            return puiIndex[iM];
        else if (VertexCompare(puiIndex[iL], puiIndex[iR], kDataSet) < 0)
            return puiIndex[iR];
        else
            return puiIndex[iL];
    }
    else
    {
        if (VertexCompare(puiIndex[iL], puiIndex[iR], kDataSet) < 0)
            return puiIndex[iL];
        else if (VertexCompare(puiIndex[iM], puiIndex[iR], kDataSet) < 0)
            return puiIndex[iR];
        else
            return puiIndex[iM];
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RebuildMesh(NiMesh* pkMesh, NiUInt32* puiNewIndices, 
    NiUInt32* puiOldToNew, NiUInt32 uiNewVertCount)
{
    NIASSERT(pkMesh->IsValid());
    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    // in the case where degenerate verts were removed, we must re-allocate
    // the data arrays so that there are no duplicates

    NiUInt32 uiOldVertCount = pkMesh->GetVertexCount();
    for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);
        if (pkStreamRef == NULL)
            continue;

        NIASSERT(pkStreamRef->GetElementDescCount() == 1);
        if (pkStreamRef->GetElementDescCount() != 1)
            return false;

        NiDataStream* pkStream = pkStreamRef->GetDataStream();
        NIASSERT(pkStreamRef->GetDataStream());
        if (pkStream->GetUsage() == NiDataStream::USAGE_VERTEX &&
            !pkStreamRef->IsPerInstance())
        {
            //get the old access flags
            NiUInt8 uiAccessFlags = pkStream->GetAccessMask();

            // for each vertex stream, create a new re-ordered stream
            NiDataStream* pkNewStream = 
                NiDataStream::CreateSingleElementDataStream(
                pkStream->GetElementDescAt(0).GetFormat(), uiNewVertCount, 
                uiAccessFlags, NiDataStream::USAGE_VERTEX, NULL, true, true);

            // Lock the new data for write
            NiDataStreamLock kNewLock(pkNewStream, 0, 
                NiDataStream::LOCK_TOOL_WRITE);
            NiTStridedRandomAccessIterator<NiUInt8> kNewData = 
                kNewLock.begin<NiUInt8>();

            // Lock the existing data for read
            NiDataStreamLock kOldLock(pkStream, 0, 
                NiDataStream::LOCK_TOOL_READ);
            NiTStridedRandomAccessIterator<NiUInt8> kOldData = 
                kOldLock.begin<NiUInt8>();

            // format tells us how to copy data from one stream to the other
            NiDataStreamElement::Format eFormat = 
                pkStream->GetElementDescAt(0).GetFormat();

            for (NiUInt32 uiV0 = 0; uiV0 < uiOldVertCount; uiV0++)
            {
                NiUInt32 uiV1 = puiOldToNew[uiV0];
                if (uiV1 != UINT_MAX)
                {
                    // uiV0 is the index into the original data
                    // uiV1 is what index to put the old data at in the 
                    // new buffer

                    NiMemcpy((void*)&kNewData[uiV1], (void*)&kOldData[uiV0], 
                        NiDataStreamElement::SizeOfFormat(eFormat));
                }
            }

            // Replace the old data stream with the new one
            pkStreamRef->SetDataStream(pkNewStream);
        }
    }

    // finally, we have to replace the old index list with the new one
    NiDataStreamElementLock kOldIndexLock(pkMesh, NiCommonSemantics::INDEX(),
        0, NiDataStreamElement::F_UNKNOWN, NiDataStream::LOCK_TOOL_READ);
    NIASSERT(kOldIndexLock.IsLocked());
    NiUInt32 uiListLength = kOldIndexLock.count();

    //get the old access flags
    NiDataStreamRef* pkOldIndexStreamRef = 
        pkMesh->FindStreamRef(NiCommonSemantics::INDEX());

    NiUInt8 uiAccessFlags;
    NiUInt32 uiStride;
    if(pkOldIndexStreamRef != NULL)
    {
        NiDataStream* pkIndexStream = pkOldIndexStreamRef->GetDataStream();
        uiAccessFlags = pkIndexStream->GetAccessMask();
        uiStride = pkIndexStream->GetStride();
    }
    else
    {
        uiAccessFlags = NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC;
        uiStride = sizeof(NiUInt32);
    }

    NiDataStreamPtr spNewIndexStream = 
        NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1, uiListLength, uiAccessFlags, 
        NiDataStream::USAGE_VERTEX_INDEX, NULL, true, true);
    NiDataStreamLock kNewIndexLock(spNewIndexStream, 0,
        NiDataStream::LOCK_TOOL_WRITE);

    NIASSERT(kNewIndexLock.IsLocked());

    NiTStridedRandomAccessIterator<NiUInt32> puiIndices = 
        kNewIndexLock.begin<NiUInt32>();
    for (NiUInt32 uiIndex = 0; uiIndex < uiListLength; uiIndex++)
    {
        puiIndices[uiIndex] = puiNewIndices[uiIndex];
    }

    NiDataStreamRef* pkIndexStreamRef;
    NiDataStreamElement kElement;
    pkMesh->FindStreamRefAndElementBySemantic(NiCommonSemantics::INDEX(), 0,
        NiDataStreamElement::F_UNKNOWN, pkIndexStreamRef, kElement);

    pkIndexStreamRef->SetDataStream(spNewIndexStream);

    pkMesh->ResetModifiers();

    NIASSERT(pkMesh->IsValid());
    return true;
}
//---------------------------------------------------------------------------
bool NiOptimize::CanRemoveNormals(NiMesh* pkObject)
{   
    // Do not remove normals from particle systems.
    if (NiIsKindOf(NiPSParticleSystem, pkObject))
    {
        return false;
    }

    // Make sure we are using standard material else return false.
    // This handles the cases for custom shaders.
    if (pkObject->GetMaterialCount() > 0)
    {
        return false;
    }

    // If the object has an environment map, do not remove normals.
    NiDynamicEffectState* pkEffectState = pkObject->GetEffectState();
    if (pkEffectState && pkEffectState->GetEnvironmentMap())
    {
        return false;
    }

    // If the object has an NiTexturingProperty attached that has a base map
    // and an apply mode of APPLY_REPLACE, normals can be removed.
    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkTexProp && pkTexProp->GetBaseMap() && pkTexProp->GetApplyMode() ==
        NiTexturingProperty::APPLY_REPLACE)
    {
        return true;
    }

    // If the object has an NiVertexColorProperty attached whose lighting
    // mode is emissive, normals can be removed.
    NiVertexColorProperty* pkVertColProp = (NiVertexColorProperty*)
        pkObject->GetProperty(NiProperty::VERTEX_COLOR);
    if (pkVertColProp && pkVertColProp->GetLightingMode() ==
        NiVertexColorProperty::LIGHTING_E)
    {
        return true;
    }

    // Otherwise, do not remove normals.
    return false;
}
//---------------------------------------------------------------------------
bool NiOptimize::OptimizeWiiSkin(NiMesh* pkMesh)
{
    // This function will reorder vertex data to improve skinning cache
    // performance on the Wii. It makes several assumptions
    //  * incoming meshes only have 1 region
    //  * All the meshes data streams have only 1 element at index 0
    //  * All data is in either 32-bit floats or 32-bit ints

    class VertexInfo : public NiMemObject
    {
    public:
        VertexInfo() :
            m_uiOriginalIndex(0xFFFFFFFF),
            m_uiNewIndex(0xFFFFFFFF),
            m_uiNumBones(0)
        {
            for (int i = 0; i < 4; ++i)
            {
                m_kBoneData[i].m_uiIndex = 0;
                m_kBoneData[i].m_fWeight = 0.0f;
            }
        }

        static NiInt32 CompareWeightAndBoneIndices(
            const void* pv1, const void* pv2)
        {
            VertexInfo* pkV1 = (VertexInfo*)pv1;
            VertexInfo* pkV2 = (VertexInfo*)pv2;

            // Compare first by the number of weights.  
            if (pkV1->m_uiNumBones < pkV2->m_uiNumBones)
                return -1;
            else if (pkV1->m_uiNumBones > pkV2->m_uiNumBones)
                return 1;

            // Weight sort is equal, so now sort by boneIndex for each of
            // the weights valid for both verts. This will help improve 
            // cache performance during skinning.
            for (NiUInt32 i = 0; i < pkV1->m_uiNumBones; ++i)
            {
                if (pkV1->m_kBoneData[i].m_uiIndex < 
                    pkV2->m_kBoneData[i].m_uiIndex)
                {
                    return -1;
                }
                else if (pkV1->m_kBoneData[i].m_uiIndex > 
                    pkV2->m_kBoneData[i].m_uiIndex)
                {
                    return 1;
                }
            }

            // If none of the above conditions apply, consider the verts equal.
            return 0;
        }

        static NiInt32 CompareOriginalIndexInRegion(
            const void* pv1, const void* pv2)
        {
            VertexInfo* pkV1 = (VertexInfo*)pv1;
            VertexInfo* pkV2 = (VertexInfo*)pv2;

            NiUInt32 ui1 = pkV1->m_uiOriginalIndex;
            NiUInt32 ui2 = pkV2->m_uiOriginalIndex;
            if (ui1 < ui2) 
                return -1;
            else if (ui1 == ui2) 
                return 0;
            else 
                return 1;
        }

        static NiInt32 CompareBoneData(const void* pv1,
            const void* pv2)
        {
           BoneData* p1 = (BoneData*)pv1;
           BoneData* p2 = (BoneData*)pv2;

           return (NiInt32)p1->m_uiIndex - (NiInt32)p2->m_uiIndex;
        }

        typedef struct 
        {
            NiUInt16 m_uiIndex;
            float m_fWeight;
        } BoneData;

        NiTSimpleArray<BoneData, 4> m_kBoneData;
        NiUInt32 m_uiOriginalIndex;
        NiUInt32 m_uiNewIndex;
        NiUInt32 m_uiNumBones;
    };

    NIASSERT(pkMesh);
    NIASSERT(pkMesh->GetSubmeshCount() == 1);
    NIASSERT(pkMesh->IsValid());

    // This function should not be called on particle systems.
    NIASSERT(!NiIsKindOf(NiPSParticleSystem, pkMesh));

    // Get arrays/iterators for the index
    NiDataStreamElementLock kIndexLock(pkMesh, NiCommonSemantics::INDEX(),
        0, NiDataStreamElement::F_UINT32_1, NiDataStream::LOCK_TOOL_READ);
    NIASSERT(kIndexLock.IsLocked());
    NiTStridedRandomAccessIterator<NiUInt32>
        kIndexIter = kIndexLock.begin<NiUInt32>();

    // Get arrays/iterators for the blend indices
    NiDataStreamElementLock kBlendIndicesLock(pkMesh, 
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_INT16_4, 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    NIASSERT(kBlendIndicesLock.IsLocked());
    NiTStridedRandomAccessIterator<NiTSimpleArray<NiUInt16, 4> >
        kBlendIndicesIter = kBlendIndicesLock.begin<
            NiTSimpleArray<NiUInt16, 4> >();

    // Get arrays/iterators for the blend weights
    NiDataStreamElementLock kBlendWeightsLock(pkMesh, 
        NiCommonSemantics::BLENDWEIGHT(), 0, 
        NiDataStreamElement::F_FLOAT32_4, 
        NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    NIASSERT(kBlendWeightsLock.IsLocked());
    NiTStridedRandomAccessIterator<NiTSimpleArray<float, 4> >
        kBlendWeightsIter = kBlendWeightsLock.begin<
            NiTSimpleArray<float, 4> >();

    NiUInt32 uiNumVerts = pkMesh->GetVertexCount();

    // Build a list of verts.
    NiTObjectSet<VertexInfo> kVertexList(uiNumVerts);
    for (NiUInt32 uiVertex = 0; uiVertex < uiNumVerts; ++uiVertex)
    {
        VertexInfo kInfo;
        kInfo.m_uiOriginalIndex = uiVertex;
        kInfo.m_uiNumBones = 0;

        float afWeights[4];
        afWeights[0] = kBlendWeightsIter[uiVertex][0];
        afWeights[1] = kBlendWeightsIter[uiVertex][1];
        afWeights[2] = kBlendWeightsIter[uiVertex][2];
        afWeights[3] = kBlendWeightsIter[uiVertex][3]; 

        for (NiUInt32 i = 0; i < 4; ++i)
        {
            if (afWeights[i] != 0.0f)
            {
                kInfo.m_kBoneData[kInfo.m_uiNumBones].m_uiIndex =
                    kBlendIndicesIter[uiVertex][i];
                kInfo.m_kBoneData[kInfo.m_uiNumBones].m_fWeight =
                    afWeights[i];
                ++kInfo.m_uiNumBones;
            }
        }

        // Sort the bone indices list by boneindex.
        qsort(&kInfo.m_kBoneData, kInfo.m_uiNumBones, 
            sizeof(VertexInfo::BoneData), VertexInfo::CompareBoneData);
        kVertexList.Add(kInfo);

        for (NiUInt32 i = 0; i < 4; ++i)
        {
            kBlendWeightsIter[uiVertex][i] = 
                kInfo.m_kBoneData[i].m_fWeight;
            kBlendIndicesIter[uiVertex][i] = 
                kInfo.m_kBoneData[i].m_uiIndex;
        }
    }
    NIASSERT(kVertexList.GetSize() == uiNumVerts);

    // Sort the vertex list using number of weights and boneindex.
    qsort(kVertexList.GetBase(), uiNumVerts, 
        sizeof(VertexInfo), VertexInfo::CompareWeightAndBoneIndices);

    for (NiUInt32 uiVertex = 0; uiVertex < uiNumVerts; ++uiVertex)
    {
        VertexInfo& kInfo = kVertexList.GetAt(uiVertex);
        kInfo.m_uiNewIndex = uiVertex;
    }

    // Resort the vertex info by the original index.
    qsort(kVertexList.GetBase(), uiNumVerts, 
        sizeof(VertexInfo), VertexInfo::CompareOriginalIndexInRegion);

    // Update the index stream based on the new order.
    NiUInt32 uiIndexCount = kIndexLock.count();
    NiUInt32* puiNewIndices = NiAlloc(NiUInt32, uiIndexCount);
    for (NiUInt32 uiIndex = 0; uiIndex < uiIndexCount; ++uiIndex)
    {
        puiNewIndices[uiIndex] = 
            kVertexList.GetAt(kIndexIter[uiIndex]).m_uiNewIndex;
    }

    NiUInt32* puiOldToNew = NiAlloc(NiUInt32, uiNumVerts);
    for (NiUInt32 uiVertex = 0; uiVertex < uiNumVerts; ++uiVertex)
    {
        NIASSERT(uiVertex == 
            kVertexList.GetAt(uiVertex).m_uiOriginalIndex);
        puiOldToNew[uiVertex] =
            kVertexList.GetAt(uiVertex).m_uiNewIndex;
    }

    kBlendIndicesLock.Unlock();
    kBlendWeightsLock.Unlock();
    kIndexLock.Unlock();

    RebuildMesh(pkMesh, puiNewIndices, puiOldToNew, uiNumVerts);

    NiFree(puiOldToNew);
    NiFree(puiNewIndices);
    NIASSERT(pkMesh->IsValid());

    return true;
}
//---------------------------------------------------------------------------
