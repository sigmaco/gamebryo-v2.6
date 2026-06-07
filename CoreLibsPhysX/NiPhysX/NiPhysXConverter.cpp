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

#include "NiPhysXPCH.h"
#include <NiSystem.h>

#include "NiPhysXConverter.h"

#include "NiPhysXProp.h"
#include "NiPhysXAccumInterpolatorDest.h"
#include "NiPhysXAccumTransformDest.h"
#include "NiPhysXClothDest.h"
#include "NiPhysXClothDesc.h"
#include "NiPhysXClothModifier.h"
#include "NiPhysXInterpolatorDest.h"
#include "NiPhysXScene.h"
#include "NiPhysXTransformDest.h"

//-------------------------------------------------------------------------
NiPhysXConverter::NiPhysXConverter()
{
}
//---------------------------------------------------------------------------
NiPhysXConverter::~NiPhysXConverter()
{
}
//---------------------------------------------------------------------------
void NiPhysXConverter::ConvertToNiMesh(NiStream& kStream,
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
        return;

    NiPhysXConverter kConverter;
    NiPhysXScenePtr m_spPhysScene = 0;
    NiTObjectArray<NiPhysXPropPtr> kSceneProps;
    for (NiUInt32 i = 0; i < kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = kTopObjects.GetAt(i);
        if (NiIsKindOf(NiPhysXProp, pkObject))
        {
            NiPhysXProp* pkProp = (NiPhysXProp*)pkObject;
            kConverter.RecursiveConvert(pkProp, kStream.GetConversionMap());
        }
        else if (NiIsKindOf(NiPhysXScene, pkObject))
        {
            m_spPhysScene = (NiPhysXScene*)pkObject;

            // If we loaded a scene then we are dealing with an older NIF file
            // and the props are not in the top level objects but in the scene.
            // Here we pull them out so we can remove them from the scene and
            // put them in the top level objects.
            for (NiUInt32 uj = 0; uj < m_spPhysScene->GetPropCount(); uj++)
            {
                kSceneProps.Add(m_spPhysScene->GetPropAt(uj));
            }
        }
    }

    for (NiUInt32 ui = 0; ui < kSceneProps.GetSize(); ui++)
    {
        NiPhysXPropPtr spProp = kSceneProps.GetAt(ui);
        kConverter.RecursiveConvert(spProp, kStream.GetConversionMap());
        NiObjectPtr spObject = NiDynamicCast(NiObject, spProp);
        kTopObjects.Add(spObject);
        m_spPhysScene->RemoveProp(spProp);
    }
    kSceneProps.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXConverter::RecursiveConvert(NiPhysXProp* pkProp,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    if (!pkProp)
        return;

    ConvertClothes(pkProp, kConversionMap);

    ConvertSources(pkProp, kConversionMap);
    
    ConvertDestinations(pkProp, kConversionMap);
}
//---------------------------------------------------------------------------
void NiPhysXConverter::ConvertClothes(NiPhysXProp* pkProp,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();
    if (!pkPropDesc)
        return;

    NiUInt32 uiClothCount = pkPropDesc->GetClothCount();    
    for (NiUInt32 uiIndex = 0; uiIndex < uiClothCount; uiIndex++)
    {
        NiPhysXClothDesc* pkClothDesc = pkPropDesc->GetClothAt(uiIndex);

        NiPhysXClothDestPtr spClothDest = pkClothDesc->GetDest();
        if (!spClothDest)
            continue;
            
        NiAVObject* pkOldTarget = spClothDest->GetTarget();
        
        NiAVObject* pkNewTarget = 0;
        if (!kConversionMap.GetAt(pkOldTarget, pkNewTarget))
        {
            NILOG("NiPhysXConverter - NiPhysXClothDest: Failed to find the "
                "new target for target with adress: 0x%x\n", pkOldTarget);
            continue;
        }

        NIASSERT(NiIsKindOf(NiMesh, pkNewTarget));
        NiMesh* pkMesh = (NiMesh*)pkNewTarget;
        pkClothDesc->SetTargetMesh(pkMesh);
        pkClothDesc->SetDest(0);

        NiPhysXClothModifier* pkClothModifier = NiNew NiPhysXClothModifier();
        NIASSERT(pkClothModifier);
        
        pkClothModifier->SetActive(true);
        pkClothModifier->SetUpdateNormals(true);
        pkClothModifier->SetMeshGrowFactor(spClothDest->GetMeshGrowFactor());
        pkClothModifier->SetDoSleepXforms(spClothDest->GetDoSleepXforms());
        pkClothModifier->SetNBTSet(spClothDest->GetNBTSet());
        
        pkMesh->AddModifier(pkClothModifier);
        
        pkProp->DeleteDestination(spClothDest);
        
        pkProp->AddModifiedMesh(pkMesh);
        
        spClothDest = 0;
    }
}
//---------------------------------------------------------------------------
void NiPhysXConverter::ConvertSources(NiPhysXProp* pkProp,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiUInt32 uiSrcCount = pkProp->GetSourcesCount();    
    for (NiUInt32 uiIndex = 0; uiIndex < uiSrcCount; uiIndex++)
    {
        NiPhysXSrc* pkPhysXSrc = pkProp->GetSourceAt(uiIndex);

        if (NiIsKindOf(NiPhysXRigidBodySrc, pkPhysXSrc))
        {
            NiPhysXRigidBodySrc* pkRigidBodySrc =
                (NiPhysXRigidBodySrc*)pkPhysXSrc;
            NiAVObject* pkCurrentNiSrc = pkRigidBodySrc->GetSource();
            NiAVObject* pkNewNiSrc = 0;
            if (kConversionMap.GetAt(pkCurrentNiSrc, pkNewNiSrc))
            {
                pkRigidBodySrc->m_pkSource = pkNewNiSrc;
            }
            else if (NiIsKindOf(NiGeometry, pkCurrentNiSrc))
            {
                NILOG("NiPhysXConverter - NiPhysXRigidBodySrc: Failed to find "
                    "the new source for source with address: 0x%x\n", 
                    pkCurrentNiSrc);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXConverter::ConvertDestinations(NiPhysXProp* pkProp,
    NiTPointerMap<const NiAVObject*, NiAVObject*>& kConversionMap)
{
    NiUInt32 uiDestCount = pkProp->GetDestinationsCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiDestCount; uiIndex++)
    {
        NiPhysXDest* pkPhysXDest = pkProp->GetDestinationAt(uiIndex);

        if (NiIsKindOf(NiPhysXAccumInterpolatorDest, pkPhysXDest))
        {
            NiPhysXAccumInterpolatorDest* pkAccumInterpolatorDest =
                (NiPhysXAccumInterpolatorDest*)pkPhysXDest;
            
            NiNode* pkCurrentSceneGraphParent =
                pkAccumInterpolatorDest->m_pkSceneParent;
            NiAVObject* pkNewSceneGraphParentSrc = 0;
            if (kConversionMap.GetAt(pkCurrentSceneGraphParent,
                pkNewSceneGraphParentSrc))
            {
                NIASSERT(NiIsKindOf(NiNode, pkNewSceneGraphParentSrc));
                pkAccumInterpolatorDest->SetAccumTarget(
                    pkAccumInterpolatorDest->GetAccumTarget(),
                    (NiNode*)pkNewSceneGraphParentSrc);
            }
            else if (NiIsKindOf(NiGeometry, pkCurrentSceneGraphParent))
            {
                NILOG("NiPhysXConverter - NiPhysXAccumInterpolatorDest: "
                    "Failed to find the new scene parent for parent with "
                    "adress: 0x%x\n", pkCurrentSceneGraphParent);
            }
        }
        else if (NiIsKindOf(NiPhysXAccumTransformDest, pkPhysXDest))
        {
            NiPhysXAccumTransformDest* pkAccumTransformDest =
                (NiPhysXAccumTransformDest*)pkPhysXDest;
            
            NiAVObject* pkCurrentAccumTarget =
                pkAccumTransformDest->GetAccumTarget();
            NiAVObject* pkNewAccumTarget = 0;
            if (kConversionMap.GetAt(pkCurrentAccumTarget, pkNewAccumTarget))
            {
                pkAccumTransformDest->SetAccumTarget(pkNewAccumTarget);
            }
            else if (NiIsKindOf(NiGeometry, pkCurrentAccumTarget))
            {
                NILOG("NiPhysXConverter - NiPhysXAccumTransformDest: Failed "
                    "to find the new accum target for target with adress: "
                    "0x%x\n", pkCurrentAccumTarget);
            }
            
            NiAVObject* pkCurrentNonAccumTarget =
                pkAccumTransformDest->GetNonAccumTarget();
            NiAVObject* pkNewNonAccumTarget = 0;
            if (kConversionMap.GetAt(pkCurrentNonAccumTarget,
                pkNewNonAccumTarget))
            {
                pkAccumTransformDest->SetNonAccumTarget(pkNewNonAccumTarget);
            }
            else if (NiIsKindOf(NiGeometry, pkCurrentNonAccumTarget))
            {
                NILOG("NiPhysXConverter - NiPhysXAccumTransformDest: Failed "
                    "to find the new non accum target for target with adress: "
                    "0x%x\n", pkCurrentNonAccumTarget);
            }
        }
        else if (NiIsKindOf(NiPhysXInterpolatorDest, pkPhysXDest))
        {
            NiPhysXInterpolatorDest* pkInterpolatorDest =
                (NiPhysXInterpolatorDest*)pkPhysXDest;
            
            NiNode* pkCurrentSceneGraphParent =
                pkInterpolatorDest->m_pkSceneParent;
            NiAVObject* pkNewSceneGraphParentSrc = 0;
            if (kConversionMap.GetAt(pkCurrentSceneGraphParent,
                pkNewSceneGraphParentSrc))
            {
                NIASSERT(NiIsKindOf(NiNode, pkNewSceneGraphParentSrc));
                pkInterpolatorDest->SetTarget(pkInterpolatorDest->GetTarget(),
                    (NiNode*)pkNewSceneGraphParentSrc);
            }
            else if (NiIsKindOf(NiGeometry, pkCurrentSceneGraphParent))
            {
                NILOG("NiPhysXConverter - NiPhysXInterpolatorDest: Failed to "
                    "find the new scenegraph parent for scenegraph parent "
                    "with adress: 0x%x\n", pkCurrentSceneGraphParent);
            }
        }
        else if (NiIsKindOf(NiPhysXTransformDest, pkPhysXDest))
        {
            NiPhysXTransformDest* pkTransformDest =
                (NiPhysXTransformDest*)pkPhysXDest;
            
            NiAVObject* pkCurrentTarget = pkTransformDest->GetTarget();
            NiAVObject* pkNewTarget = 0;
            if (kConversionMap.GetAt(pkCurrentTarget, pkNewTarget))
            {
                pkTransformDest->SetTarget(pkNewTarget);
            }
            else if (NiIsKindOf(NiGeometry, pkCurrentTarget))
            {
                NILOG("NiPhysXConverter - NiPhysXTransformDest: Failed to "
                    "find the new target for target with address: 0x%x\n", 
                    pkCurrentTarget);
            }
        }
    }
}
//---------------------------------------------------------------------------
