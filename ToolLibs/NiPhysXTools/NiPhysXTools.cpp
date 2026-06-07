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


#include "NiPhysXTools.h"


//---------------------------------------------------------------------------
void NiPhysXTools::AddStateFromSceneGraph(NiPhysXProp* pkProp,
    NiFixedString& kStateName, VelocitySource eVelocitySource)
{
    NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();
    if (!pkPropDesc)
        return;
        
    float fScale = 1.0f / pkProp->GetScaleFactor();

    // Look for the state.
    NiUInt32 uiStateIndex = pkPropDesc->GetStateIndex(kStateName);
    if (uiStateIndex == 0)
        uiStateIndex = pkPropDesc->GetNumStates();
    
    // Go through all actors first
    NiUInt32 uiActorCount = pkPropDesc->GetActorCount();
    for (NiUInt32 ui = 0; ui < uiActorCount; ui++)
    {
        NiPhysXActorDesc* pkActorDesc = pkPropDesc->GetActorAt(ui);
        NIASSERT(pkActorDesc);

        NiPhysXBodyDesc* pkBodyDesc = pkActorDesc->GetBodyDesc();

        NiPhysXRigidBodySrc* pkSrc = pkActorDesc->GetSource();
        if (pkSrc)
        {
            NIASSERT(NiIsKindOf(NiPhysXDynamicSrc, pkSrc));
            AddStateFromDynamicSrc(
                pkActorDesc, (NiPhysXDynamicSrc*)pkSrc, fScale, uiStateIndex);
        }
        else 
        {
            NiPhysXRigidBodyDest* pkDest = pkActorDesc->GetDest();
            if (pkDest)
            {
                NIASSERT(NiIsKindOf(NiPhysXTransformDest, pkDest));
                AddStateFromTransformDest(pkActorDesc,
                    (NiPhysXTransformDest*)pkDest, fScale, uiStateIndex);
            }
            else
            {
                // You hit this assert if there is a non-static actor that
                // is lacking a source or destination. That is an invalid
                // state during Gamebryo-PhysX tool processing.
                NIASSERT(!pkBodyDesc);
            }
        }
        
        if (pkBodyDesc)
        {
            switch (eVelocitySource)
            {
                case Zero:
                {
                    NxVec3 kLinearV(0.0f, 0.0f, 0.0f);
                    NxVec3 kAngularV(0.0f, 0.0f, 0.0f);
                    pkBodyDesc->SetVelocities(kLinearV, kAngularV,
                        uiStateIndex, NiPhysXTypes::NXMAT33_ID);
                    break;
                }
                
                case DefaultState:
                {
                    NxVec3 kLinearV;
                    NxVec3 kAngularV;
                    pkBodyDesc->GetVelocities(kLinearV, kAngularV, 0);
                    pkBodyDesc->SetVelocities(kLinearV, kAngularV,
                        uiStateIndex, NiPhysXTypes::NXMAT33_ID);
                    break;
                }
            }
        }
    }
    
    // Do cloth
    NiUInt32 uiClothCount = pkPropDesc->GetClothCount();
    for (NiUInt32 ui = 0; ui < uiClothCount; ui++)
    {
        NiPhysXClothDesc* pkClothDesc = pkPropDesc->GetClothAt(ui);
        if (!pkClothDesc)
            continue;
        
        AddStateFromCloth(pkClothDesc, fScale, uiStateIndex);
    }

    if (uiStateIndex == pkPropDesc->GetNumStates())
    {
        // New state.
        
        // Copy materials. We have no better information.
        NiTMapIterator iter = pkPropDesc->GetFirstMaterial();
        while (iter)
        {
            NxMaterialIndex usKey;
            NiPhysXMaterialDescPtr spMaterialDesc;
            pkPropDesc->GetNextMaterial(iter, usKey, spMaterialDesc);
            NIASSERT(spMaterialDesc != 0);
            NxMaterialDesc kPhysXMaterial;
            spMaterialDesc->ToMaterialDesc(kPhysXMaterial, 0);
            spMaterialDesc->FromMaterialDesc(kPhysXMaterial, uiStateIndex);
        }
        
        
        pkPropDesc->SetStateName(uiStateIndex, kStateName);
        pkPropDesc->SetNumStates(uiStateIndex + 1);
    }
}
//---------------------------------------------------------------------------
void NiPhysXTools::AddStateFromTransformDest(NiPhysXActorDesc* pkActorDesc,
    NiPhysXTransformDest* pkDest, float fScale, NiUInt32 uiStateIndex)
{
    NiAVObject* pkTarget = pkDest->GetTarget();
    NIASSERT(pkTarget);
    
    NxMat34 kPose;
    NiPhysXTypes::NiTransformToNxMat34(pkTarget->GetWorldRotate(),
        pkTarget->GetWorldTranslate(), kPose);
    kPose.t *= fScale;
    pkActorDesc->SetPose(kPose, uiStateIndex);
}
//---------------------------------------------------------------------------
void NiPhysXTools::AddStateFromDynamicSrc(NiPhysXActorDesc* pkActorDesc,
    NiPhysXDynamicSrc* pkSrc, float fScale, NiUInt32 uiStateIndex)
{
    NiAVObject* pkSource = pkSrc->GetSource();
    NIASSERT(pkSource);
    
    NxMat34 kPose;
    NiPhysXTypes::NiTransformToNxMat34(pkSource->GetWorldRotate(),
        pkSource->GetWorldTranslate(), kPose);
    kPose.t *= fScale;
    pkActorDesc->SetPose(kPose, uiStateIndex);
}
//---------------------------------------------------------------------------
void NiPhysXTools::AddStateFromCloth(NiPhysXClothDesc* pkClothDesc,
    float fScale, NiUInt32 uiStateIndex)
{
    // You hit this assert if the cloth object has no mesh target. All cloth
    // should have something.
    NiMesh* pkMesh = pkClothDesc->GetTargetMesh();
    
    // Get world vertex locations and save them. No tearing. 
    NiUInt16 uiNumVerts;
    const NiUInt16* puiIndexMap;
    pkClothDesc->GetMeshDesc()->GetVertexMap(uiNumVerts, puiIndexMap);
    NIASSERT(uiNumVerts == pkMesh->GetVertexCount());
    NxVec3* pkNxVerts = NiAlloc(NxVec3, uiNumVerts);
    
    // Get a pointer to the vertex stream and lock it for reading.
    NiDataStreamRef* pkVerticesDSRef = pkMesh->FindStreamRef(
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3);
    NiDataStream* pkVerticesDataStream = pkVerticesDSRef->GetDataStream();

    NiPoint3* pkVertices = (NiPoint3*)pkVerticesDataStream->Lock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

    NiUInt16 uiNumNxVerts = 0;
    for (NiUInt16 ui = 0; ui < uiNumVerts; ui++)
    {
        NiPoint3 kPosn = pkMesh->GetWorldTransform() * pkVertices[ui];
        kPosn *= fScale;
        NiPhysXTypes::NiPoint3ToNxVec3(kPosn, pkNxVerts[puiIndexMap[ui]]);
        
        if (puiIndexMap[ui] > uiNumNxVerts)
            uiNumNxVerts = puiIndexMap[ui];
    }

    pkVerticesDataStream->Unlock(
        NiDataStream::LOCK_READ | NiDataStream::LOCK_TOOL_READ);

    pkClothDesc->SetVertexPositions(uiNumNxVerts + 1, pkNxVerts, uiStateIndex);
}
//---------------------------------------------------------------------------
