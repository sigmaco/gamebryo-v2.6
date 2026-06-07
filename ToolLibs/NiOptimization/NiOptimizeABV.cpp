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

#include "NiOptimizeABV.h"
#include <NiString.h>

#include "NiRTLib.h"
#include "NiMain.h"
#include "NiCollision.h"
#include "NiOptimize.h"
#include "NiTNodeTraversal.h"
#include "NiBoneLODController.h"

#include "NiMesh.h"
#include "NiDataStreamElementLock.h"

//---------------------------------------------------------------------------
void NiOptimizeABV::CreateABVFromProxy(NiAVObject* pkObject,  
    NiTPointerList<char*>* pkErrors/* = NULL*/)
{
    if (!pkObject)
        return;

    // If pkObject is a node, search for ABV mesh among it's children
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode == NULL)
        return;

    NiTransform kTransform;
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        // get a child
        NiAVObject* pkChild = pkNode->GetAt(ui);

        // continue if non-existant
        if (!pkChild) 
            continue;

        // ABVs and Multi nodes have names, so continue if no name
        if (!pkChild->GetName())
            continue;

        // get first token of the name
        char acName[NI_MAX_PATH];
        char acToken[NI_MAX_PATH];
        char acSeps[] = " \n";

        NiStrcpy(acName, NI_MAX_PATH, pkChild->GetName());

        size_t stLen = strlen(acName);

        // We need at least "NDLCD XY", also
        // empty "" strings seem to cause strtok to choke.
        if (stLen < 8)
            continue;

        char* pcContext;
        NiStrcpy(acToken, NI_MAX_PATH, NiStrtok(acName, acSeps, &pcContext));
         
        // if not an ABV description, skip this node
        if (strcmp(acToken, "NDLCD"))
            continue;

        // get second token of name, nothing to do if there is no token
        NiStrcpy(acToken, NI_MAX_PATH, NiStrtok(NULL, acSeps, &pcContext));
        if (acToken == 0)
            continue;

        // The token must be a 2-letter sequence.  The naming specifications
        // do not explicitly indicate that white space must occur after the
        // 2-letter sequence but before any additional tokens.  Some test
        // data sets do have the 2-letter sequence followed immediately by
        // a user-assigned node name, so "strlen(acToken) != 2" cannot be the
        // comparison used here.  If no white space occurs, the code that
        // parses acToken[0] will gracefully handle the situation.
        if (strlen(acToken) < 2)
        {
            // Generate Error Message
            BuildError(pkErrors, "ERROR: Incorrect ABV Name: Missing "
                "Collision Shape and Propagation Mode: ", pkChild->GetName());
            continue;
        }



        // create the ABV
        NiBoundingVolume* pkABV = 0;
        NiNode* pkABVNode = NiDynamicCast(NiNode, pkChild);

        if (pkABVNode == NULL)
        {
            // Generate Error Message
            BuildError(pkErrors, "ERROR: The ABV Proxy Object MUST be an "
                "NiNode: ", pkChild->GetName());
            continue;
        }

        NiCollisionData* pkCD = NiNew NiCollisionData(pkObject);

        switch (acToken[0])
        {
        case 'S':
            pkABV = CreateSphereABV(pkABVNode, pkErrors);
            pkCD->SetCollisionMode(NiCollisionData::USE_ABV);
            break;
        case 'C':
            pkABV = CreateCapsuleABV(pkABVNode, pkErrors);
            pkCD->SetCollisionMode(NiCollisionData::USE_ABV);
            break;
        case 'B':
            pkABV = CreateBoxABV(pkABVNode, pkErrors);
            pkCD->SetCollisionMode(NiCollisionData::USE_ABV);
            break;
        case 'U':
            pkABV = CreateUnionABV(pkABVNode, pkErrors);
            pkCD->SetCollisionMode(NiCollisionData::USE_ABV);
            break;
        case 'H':
            pkABV = CreateHalfSpaceABV(pkABVNode, pkErrors);
            pkCD->SetCollisionMode(NiCollisionData::USE_ABV);
            break;
        case 'T':
            // Set SGO to null during setup to avoid assert.
            pkCD->SetSceneGraphObject(NULL);
            pkCD->SetCollisionMode(NiCollisionData::USE_TRI);
            break;
        case 'O':
            // Set SGO to null during setup to avoid assert.
            pkCD->SetSceneGraphObject(NULL);
            pkCD->SetCollisionMode(NiCollisionData::USE_OBB);
            break;
        case 'W':
            pkCD->SetCollisionMode(NiCollisionData::USE_NIBOUND);
            break;
        case 'N':
            pkCD->SetCollisionMode(NiCollisionData::NOTEST);
            break;
        default:
            pkCD->SetCollisionMode(NiCollisionData::NOTEST);

            // Generate Error Message
            BuildError(pkErrors, "ERROR: Invalid ABV Shape: (S, C, B, U, H, "
                "T, O, W or N) On Object: ", pkChild->GetName());
            continue;
        }

        switch (acToken[1])
        {
            case 'A':
                pkCD->SetPropagationMode(NiCollisionData::PROPAGATE_ALWAYS);
                break;

            case 'N':
                pkCD->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);
                break;

            case 'S':
                pkCD->SetPropagationMode(
                    NiCollisionData::PROPAGATE_ON_SUCCESS);
                break;

            case 'F':
                pkCD->SetPropagationMode(
                    NiCollisionData::PROPAGATE_ON_FAILURE);
                break;
            default:
                pkCD->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);

                // Generate Error Message
                BuildError(pkErrors, "ERROR: Invalid ABV Propagation Mode (A, "
                    "N, S or F) On Object:", pkChild->GetName());
                break;
        }
        // apply transforms of child TriShape if not a Union, to capture
        // transforms of axes in original Max data
        if (pkABV && acToken[0] != 'U')
            ApplyMeshTransformToABV(pkABVNode, pkABV);

        // where is it attached?

        switch (acToken[0])
        {
        case 'O':
        case 'T': // attached to the mesh or multimaterial node
        {   // apply proxy transform to ABV.  Other transform made later
            // pkChild is our Collision tag...
            // we need to look for siblings that are Meshes
            for (unsigned int ui2 = 0; ui2 < pkNode->GetArrayCount(); ui2++)
            {
                // get a child
               NiAVObject* pkSibling = pkNode->GetAt(ui2);

               if (pkSibling == NULL)
                   continue;

               if (pkSibling == pkChild)
                   continue;

               if (NiDynamicCast(NiMesh, pkSibling) == NULL)
                   continue;

               pkSibling->SetCollisionObject(pkCD);
               pkNode->SetCollisionObject(NULL);
            }
            break;
        }
        case 'N':
        case 'W':
            pkObject->SetCollisionObject(pkCD);
            break;
        case 'U':
            // attach ABV to Proxy node, will move to union later
            pkObject->SetCollisionObject(pkCD);
            pkCD->SetModelSpaceABV(pkABV);
            break;
        default: // attached to the parent node
            // apply proxy transform to ABV and attach to parent
            kTransform.m_Translate = pkChild->GetTranslate();
            kTransform.m_Rotate = pkChild->GetRotate();
            kTransform.m_fScale = pkChild->GetScale();
            pkABV->UpdateWorldData(*pkABV, kTransform);

            pkObject->SetCollisionObject(pkCD);
            pkCD->SetModelSpaceABV(pkABV);
            break;  // continue to search for dummy ABVs
        }
    }

    // Now do recursion, this is not optimal as it iterates over children
    // again but it avoids any sideeffects from in place recursion above
    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
            CreateABVFromProxy(pkChild, pkErrors);
    }
}
//---------------------------------------------------------------------------
void NiOptimizeABV::RemoveByFirstToken(NiAVObject* pkObject,
    const char* pcName)
{
    RemoveByFirstTokenFunctor kFunctor(pcName);

    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
}
//---------------------------------------------------------------------------
void NiOptimizeABV::RemoveCollisionDataProxyMesh(NiAVObject* pkObject, 
    NiTPointerList<char*>* pkErrors)
{
    RemoveCollisionDataProxyMeshFunctor kFunctor(pkErrors);

    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
}
//---------------------------------------------------------------------------
NiNode* NiOptimizeABV::GetNodeByFirstToken(NiNode* pkNode,
    const char* pcToken)
{
    FindByFirstTokenFunctor kFunctor(pcToken);

    NiTNodeTraversal::DepthFirst_FirstStop(pkNode, kFunctor);

    return kFunctor.GetMatchingNode();
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiOptimizeABV::CreateSphereABV(NiNode* pkNode,
    NiTPointerList<char*>* pkErrors/* = NULL */)
{
    NiPoint3 kCenter;
    NiPoint3 kExtents;
    int aiAxisOrder[3];

    GetCenterAndExtents(pkNode, kCenter, kExtents, aiAxisOrder, pkErrors);
    // set radius equal to largest axis
    float fRadius = kExtents[aiAxisOrder[0]];  

    NiSphereBV* pkABV = NiNew NiSphereBV(fRadius, kCenter);
    return (NiBoundingVolume*)pkABV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiOptimizeABV::CreateCapsuleABV(NiNode* pkNode,
    NiTPointerList<char*>* pkErrors/* = NULL */)
{
    NiPoint3 kCenter;
    NiPoint3 kExtents;
    int aiAxisOrder[3];

    GetCenterAndExtents(pkNode, kCenter, kExtents, aiAxisOrder, pkErrors);

    // radius is second longest axis, and extent along largest axis
    float fRadius = kExtents[aiAxisOrder[1]];
    float fExtent = kExtents[aiAxisOrder[0]];
    NiPoint3 kAxis = NiPoint3::ZERO;
    kAxis[aiAxisOrder[0]] = 1.0f;

    NiCapsuleBV* pkABV = NiNew NiCapsuleBV(fRadius, fExtent, kCenter, kAxis);
    return (NiBoundingVolume*)pkABV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiOptimizeABV::CreateBoxABV(NiNode* pkNode,
    NiTPointerList<char*>* pkErrors/* = NULL */)
{
    NiPoint3 kCenter;
    NiPoint3 kExtents;
    int aiAxisOrder[3];

    GetCenterAndExtents(pkNode, kCenter, kExtents, aiAxisOrder, pkErrors);

    NiBoxBV* pkABV = NiNew NiBoxBV(kExtents, kCenter);
    return (NiBoundingVolume*)pkABV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiOptimizeABV::CreateHalfSpaceABV(NiNode* pkNode,
    NiTPointerList<char*>* pkErrors/* = NULL */)
{
    NiPoint3 kCenter;
    NiPoint3 kExtents;
    int aiAxisOrder[3];

    GetCenterAndExtents(pkNode, kCenter, kExtents, aiAxisOrder, pkErrors);

    NiPoint3 kAxis = NiPoint3::ZERO;
    kAxis[aiAxisOrder[2]] = -1.0f;

    NiHalfSpaceBV* pkABV = NiNew NiHalfSpaceBV(kCenter, kAxis);
    return (NiBoundingVolume*)pkABV;
}
//---------------------------------------------------------------------------
NiBoundingVolume* NiOptimizeABV::CreateUnionABV(NiNode* pkNode,
    NiTPointerList<char*>* pkErrors/* = NULL */)
{
    NiUnionBV* pkUnionABV = NiNew NiUnionBV;

    // Generate Error Message
    if (pkNode->GetChildCount() == 0)
    {
        BuildError(pkErrors, "ERROR: The ABV Union has NO CHILDREN : ", 
            pkNode->GetName());
    }

    int iNumABVChildren = 0;
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        // get a child
        NiAVObject* pkChild = pkNode->GetAt(ui);

        // continue if non-existant
        if (!pkChild) 
            continue;

        // ensure it's not a mesh or has no name
        if ((NiDynamicCast(NiMesh, pkChild)) || (!pkChild->GetName()))
            continue;

        // get first token of the name
        char acName[NI_MAX_PATH];
        char acToken[NI_MAX_PATH];
        char acSeps[] = " \n";
        NiStrcpy(acName, NI_MAX_PATH, pkChild->GetName());
        char* pcContext;
        NiStrcpy(acToken, NI_MAX_PATH, NiStrtok(acName, acSeps, &pcContext));

        // it shouldn't be a multimtl node, either
        NIASSERT(strcmp(acToken, "__NDL_MultiMtl_Node"));

        // Generate Error Message
        if (!strcmp(acToken, "__NDL_MultiMtl_Node"))
        {
            BuildUnionError(pkErrors, "ERROR: The ABV Union should NOT have a "
                "child named <__NDL_MultiMtl_Node>", pkNode->GetName(), 
                pkChild->GetName());
        }

         
        // if not an ABV description, skip this node
        if (strcmp(acToken, "NDLCD"))
        {
            BuildUnionError(pkErrors, "ERROR: The ABV Union should NOT have "
                "NON ABV children", pkNode->GetName(), pkChild->GetName());
            continue;
        }

        // get second token of name
        NiStrcpy(acToken, NI_MAX_PATH, NiStrtok(NULL, acSeps, &pcContext));
        NIASSERT(acToken);

        // assure name is correct format
        if ((strlen(acToken) != 2) ||
            ((acToken[1]!='A') && (acToken[1]!='N') && (acToken[1]!='S') &&
            (acToken[1]!='F')))
        {
            BuildUnionError(pkErrors, "ERROR: The ABV Union, the ABV child's "
                "name has an invalid format", pkNode->GetName(), 
                pkChild->GetName());
            continue;
        }

        // create the ABV
        NiBoundingVolume* pkABV = 0;
        NiNode* pkABVNode = NiDynamicCast(NiNode, pkChild);
        if (pkABVNode == NULL)
        {
            BuildUnionError(pkErrors, "ERROR: The ABV Union, the child ABV "
                "must be an NiNode", pkNode->GetName(), pkChild->GetName());
            continue;
        }

        switch (acToken[0])
        {
        case 'S':
            pkABV = CreateSphereABV(pkABVNode, pkErrors);
            break;
        case 'C':
            pkABV = CreateCapsuleABV(pkABVNode, pkErrors);
            break;
        case 'B':
            pkABV = CreateBoxABV(pkABVNode, pkErrors);
            break;
        case 'U':
            pkABV = CreateUnionABV(pkABVNode, pkErrors);
            break;
        case 'H':
            pkABV = CreateHalfSpaceABV(pkABVNode, pkErrors);
            break;
        default:
            // Generate Error Message
            BuildUnionError(pkErrors, "ERROR: The ABV Union should only have "
                "ABV children of a simple shape; (S)phere, (C)Apsule, (B)ox, "
                "(U)nion, (H)alfspace  :", pkNode->GetName(), 
                pkChild->GetName());
            continue;
        }

        // apply proxy transform to ABV, then Union's transform
        if (!pkABV)
            continue;

        // Increment the number of ABV Children
        iNumABVChildren++;

        ApplyNodeTransformToABV((NiNode*)pkChild, pkABV);
        
        // attach to UnionABV parent no matter what the attachment flag
        pkUnionABV->AddBoundingVolume(pkABV);
    }

    if (iNumABVChildren == 0)
    {
        BuildError(pkErrors, "ERROR: The ABV Union has NO ABV children : ", 
            pkNode->GetName());
    }
    else if (iNumABVChildren == 1)
    {
        BuildError(pkErrors, "WARNING:: The ABV Union has only 1 ABV child : ",
            pkNode->GetName());
    }

    NiTransform kTransform;
    kTransform.m_Translate = pkNode->GetTranslate();
    kTransform.m_Rotate = pkNode->GetRotate();
    kTransform.m_fScale = pkNode->GetScale();
    pkUnionABV->UpdateWorldData(*pkUnionABV, kTransform);
    return (NiBoundingVolume*)pkUnionABV;
}
//---------------------------------------------------------------------------
void NiOptimizeABV::GetCenterAndExtents(NiNode* pkNode, NiPoint3& kCenter, 
    NiPoint3& kExtents, int aiAxisOrder[3], 
    NiTPointerList<char*>* pkErrors/* = NULL */)
{
    // note that the Node should only have one child--the NiMesh that
    // describes the box, which should also be axis-aligned as created in
    // the modeling package.
    NiMesh* pkMesh = NiDynamicCast(NiMesh, pkNode->GetAt(0));
    NIASSERT(pkMesh);

    if (pkMesh == NULL)
    {
        kExtents = NiPoint3::ZERO;
        aiAxisOrder[0] = 0;
        aiAxisOrder[1] = 1;
        aiAxisOrder[2] = 2;

        // Generate Error Message
        BuildError(pkErrors, "ERROR:  An ABV node has been set up "
            "incorrectly.  It should have only its own ABV mesh beneath "
            "it and no other children.  Object: ", pkNode->GetName());

        return;
    }

    NiDataStreamElementLock kLock(pkMesh, NiCommonSemantics::POSITION(),
        0, NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ); 

    if (!kLock.IsLocked())
    {
        BuildError(pkErrors, "ERROR: Could not lock the ABV mesh node "
            "position stream. Object: ", pkNode->GetName());
        return;
    }

    NiTStridedRandomAccessIterator<NiPoint3> kBegin = kLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kEnd = kLock.end<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kIter = kBegin;
    
    NIASSERT(kBegin != kEnd);
    NiPoint3 kMin = *kIter;
    NiPoint3 kMax = *kIter;
    
    ++kIter; // Already got the first vertex.
    while (kIter != kEnd)
    {
        const NiPoint3& kVert = *kIter;

        if (kVert.x < kMin.x)
            kMin.x = kVert.x;
        if (kVert.y < kMin.y)
            kMin.y = kVert.y;
        if (kVert.z < kMin.z)
            kMin.z = kVert.z;

        if (kVert.x > kMax.x)
            kMax.x = kVert.x;
        if (kVert.y > kMax.y)
            kMax.y = kVert.y;
        if (kVert.z > kMax.z)
            kMax.z = kVert.z;

        ++kIter;
    }

    kCenter = (kMin + kMax) / 2.0;
    kExtents = kMax - kCenter;
    if (kExtents[0] > kExtents[1]) 
    {
        if (kExtents[0] > kExtents[2])
        {
            aiAxisOrder[0] = 0;
            if (kExtents[1] > kExtents[2])
            {
                aiAxisOrder[1] = 1;
                aiAxisOrder[2] = 2;
            }
            else
            {
                aiAxisOrder[1] = 2;
                aiAxisOrder[2] = 1;
            }
        }
        else 
        {
            aiAxisOrder[0] = 2;
            aiAxisOrder[1] = 0;
            aiAxisOrder[2] = 1;
        }
    }
    else 
    {
        if (kExtents[1] > kExtents[2])
        {
            aiAxisOrder[0] = 1;
            if (kExtents[0] > kExtents[2])
            {
                aiAxisOrder[1] = 0;
                aiAxisOrder[2] = 2;
            }
            else
            {
                aiAxisOrder[1] = 2;
                aiAxisOrder[2] = 0;
            }
        }
        else 
        {
            aiAxisOrder[0] = 2;
            aiAxisOrder[1] = 1;
            aiAxisOrder[2] = 0;
        }
    }

    return;
}
//---------------------------------------------------------------------------
void NiOptimizeABV::CreateWireABVProxy(NiAVObject* pkObject, 
    const NiColor& kColor, float fMult, const char* pcName)
{
    if (!pkObject)
        return;

    // ABVs and Multi nodes have names, so continue if no name
    if (pkObject->GetName())
    {
        // get first token of the name
        char acName[NI_MAX_PATH];
        char acBack[NI_MAX_PATH];
        char acToken[NI_MAX_PATH];
        char acSeps[] = " \n";
        NiStrcpy(acName, NI_MAX_PATH, pkObject->GetName());
        NiStrcpy(acBack, NI_MAX_PATH, acName);
        char* pcContext;
        NiStrcpy(acToken, NI_MAX_PATH, NiStrtok(acName, acSeps, &pcContext));
         
        // check to see if it's an ABV Proxy
        if (!strcmp(acToken, "NDLCD"))
        {
            // All ABV Proxies are NiNodes
            NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
            if (pkNode == NULL)
                return;

            // grab next token and handle if it indicates we have a UnionABV
            NiStrcpy(acToken, NI_MAX_PATH, NiStrtok(NULL, acSeps, &pcContext));
            if (acToken[0] == 'U')
            {
                unsigned int uiC;  // child counter

                // children have already had wireframes created, so move
                // these to the parent of the Union
                for (uiC = 0; uiC < pkNode->GetArrayCount(); uiC++)
                {
                    // get a child
                    NiNode* pkChild =
                        NiDynamicCast(NiNode, pkNode->GetAt(uiC));
                    
                    if (pkChild && pkChild->GetName() &&
                        !strcmp("DrawableABVProxy", pkChild->GetName()))
                    {
                        // apply transforms of the parent before moving
                        ApplyNodeTransformToObject(pkNode, pkChild);

                        NiAVObjectPtr spProxy = 
                            pkNode->DetachChild(pkChild);
                        pkNode->GetParent()->AttachChild(spProxy);
                    }
                }
                return;
            }

            NiBoundingVolume* pkABV = CreateBoxABV(pkNode);
            ApplyMeshTransformToABV(pkNode, pkABV);

            NiNode* pkWireframe = NiDrawableBV::CreateWireframeBV(pkABV, 
                kColor, fMult, pkObject);

            NiDelete pkABV;

            if (pkWireframe)
            {
                // proxy
                pkWireframe->SetTranslate(pkNode->GetTranslate());
                pkWireframe->SetRotate(pkNode->GetRotate());
                pkWireframe->SetScale(pkNode->GetScale());

                pkWireframe->SetName(pcName);
                pkNode->GetParent()->AttachChild(pkWireframe);
            }
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode == NULL)
        return;

    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            CreateWireABVProxy(pkChild, kColor, fMult, pcName);
        }
    }
}
//---------------------------------------------------------------------------
inline void NiOptimizeABV::ApplyNodeTransformToABV(NiNode* pkNode, 
    NiBoundingVolume* pkABV)
{
    NiAVObject* pkObject = pkNode->GetAt(0);
    NiTransform kTransform;
    kTransform.m_Translate = pkNode->GetTranslate() + 
        pkObject->GetTranslate();
    kTransform.m_Rotate = pkNode->GetRotate() * pkObject->GetRotate();
    kTransform.m_fScale = pkNode->GetScale() * pkObject->GetScale();
    pkABV->UpdateWorldData(*pkABV, kTransform);
}
//---------------------------------------------------------------------------
inline void NiOptimizeABV::ApplyMeshTransformToABV(NiNode* pkNode, 
    NiBoundingVolume* pkABV)
{
    NIASSERT(pkNode && pkABV);

    // apply transforms of child mesh, to capture transforms of axis
    // in original Max data
    NiMesh* pkMesh = NiDynamicCast(NiMesh, pkNode->GetAt(0));
    NIASSERT(pkMesh);
    if (pkMesh == NULL)
        return;

    NiTransform kTransform;
    kTransform.m_Translate = pkMesh->GetTranslate();
    kTransform.m_Rotate = pkMesh->GetRotate();
    kTransform.m_fScale = pkMesh->GetScale();
    pkABV->UpdateWorldData(*pkABV, kTransform);
}
//---------------------------------------------------------------------------
inline void NiOptimizeABV::ApplyNodeTransformToObject(NiNode* pkNode, 
    NiAVObject* pkObject)
{
    NIASSERT(pkNode && pkObject);

    pkObject->SetTranslate(pkNode->GetTranslate() + 
        pkObject->GetTranslate());
    pkObject->SetRotate(pkNode->GetRotate() * pkObject->GetRotate());
    pkObject->SetScale(pkNode->GetScale() * pkObject->GetScale());
}
//---------------------------------------------------------------------------
void NiOptimizeABV::CountByName(NiAVObject* pkObject, const char* pcName, 
    unsigned int& uiCount)
{
    if (!pkObject)
        return;

    if (pkObject->GetName())
    {
        if (!strcmp(pcName, pkObject->GetName()))
            uiCount++;
    }

    // If pkObject is a node, recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode == NULL)
        return;

    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            CountByName(pkChild, pcName, uiCount);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimizeABV::StoreNodesWithName(NiAVObject* pkObject,
    const char* pcName, NiNode** ppkArray, unsigned int& uiIndex)
{
    if (!pkObject)
        return;

    if (pkObject->GetName())
    {
        if (!strcmp((const char*)pcName, pkObject->GetName()))
        {
            ppkArray[uiIndex] = (NiNode*)pkObject;
            uiIndex++;
        }
    }

    // If pkObject is a node, recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode == NULL)
        return;

    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        NiAVObject* pkChild = pkNode->GetAt(i);
        if (pkChild)
        {
            StoreNodesWithName(pkChild, pcName, ppkArray, uiIndex);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimizeABV::CreateWireABV(NiAVObject* pkObject,
    const NiColor& kColor, float fMult, const char* pcName, bool bCreateOBB,
    bool bAppendAddress)
{
    if (!pkObject)
        return;

    NiCollisionData* pkData = NiGetCollisionData(pkObject);
    NiBoundingVolume* pkABV = NULL;

    if (pkData)
    {
        pkABV = pkData->GetModelSpaceABV();
    }

    if (pkABV)
    {
        NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
        if (pkNode)
        {            
            NiNode* pkDrawNode = NiDrawableBV::CreateWireframeBV(pkABV, 
                kColor, fMult, pkObject);

            if (pkDrawNode)
            {
                if (bAppendAddress)
                {
                    NiString strNewName = NULL;
                    strNewName.Format("%s_%x", pcName, pkObject);
                    pkDrawNode->SetName((const char*)strNewName);
                }
                else
                {
                    pkDrawNode->SetName(pcName);
                }
                pkNode->AttachChild(pkDrawNode);
            }
        }
    }
    else if (bCreateOBB)
    {
        NiMesh* pkMesh = NiDynamicCast(NiMesh, pkObject);
        if ((pkMesh) && (pkData))
        {
            NiCollisionData::CollisionMode eMode =
                pkData->GetCollisionMode();

            if ((eMode == NiCollisionData::USE_OBB) || 
                (eMode == NiCollisionData::USE_TRI))
            {
                NiBoundingVolume* pkBoxBV = 
                    NiBoundingVolume::ConvertToBoxBV(pkMesh);
                if (pkBoxBV)
                {
                    NiNode* pkDrawNode = NiDrawableBV::CreateWireframeBV(
                        pkBoxBV, kColor, fMult, pkObject);
                    if (pkDrawNode)
                    {
                        if (bAppendAddress)
                        {
                            NiString strNewName = NULL;
                            strNewName.Format("%s_%x", pcName, pkMesh);
                            pkDrawNode->SetName((const char*)strNewName);
                        }
                        else
                        {
                            pkDrawNode->SetName(pcName);
                        }

                        pkMesh->GetParent()->AttachChild(pkDrawNode);
                        pkDrawNode->SetTranslate(pkMesh->GetTranslate());
                        pkDrawNode->SetRotate(pkMesh->GetRotate());
                        pkDrawNode->SetScale(pkMesh->GetScale());

                    }

                    NiDelete pkBoxBV;
                    return;
                }
            }
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // store count, in case children attach any addition children
        unsigned int uiCount = pkNode->GetArrayCount();
        for (unsigned int i = 0; i < uiCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
            {
                CreateWireABV(pkChild, kColor, fMult, pcName, bCreateOBB,
                    bAppendAddress);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimizeABV::GroupNoTestNodes(NiNode* pkNode)
{
    // This routine creates a node that groups nodes with the NO_TEST
    // propagation flag.  Collision will not try to test such nodes nor
    // any children of such nodes - so it is more efficient to have these
    // nodes grouped.  In practice, the benefits are marginal since
    // the nodes underneath NO_TEST are still updated.
    
    unsigned int uiChildren = pkNode->GetArrayCount();
    unsigned uiNoTestCnt = 0;
    unsigned uiChk = 0;

    unsigned int uiI;

    for (uiI = 0; uiI < uiChildren; uiI++)
    {
        // Count to ensure we have enough nodes to warrent a grouping.

        // get a child
        NiAVObject* pkChild = pkNode->GetAt(uiI);

        if (pkChild == NULL)
            continue;

        if (NiDynamicCast(NiNode, pkChild) == NULL)
            continue;
        
        NiCollisionData* pkColData = NiGetCollisionData(pkChild);

        if (pkColData == NULL)
            continue;

        NiCollisionData::CollisionMode eMode = 
            pkColData->GetCollisionMode();
        
        if (eMode == NiCollisionData::NOTEST)
            uiNoTestCnt++;
    }

    if (uiNoTestCnt >= 2)
    {
        // Then we'll create a new node.
        // Set it to be NO_TEST.
        // Any children of pkNode that has NO_TEST will be attach to it.
        // And then this new node will be attached to pkNode.
        // The others will have their collision data removed.
        NiNodePtr spNewNode = NiNew NiNode;

        NiCollisionData* pkColData = NiNew NiCollisionData(spNewNode);
        pkColData->SetCollisionMode(NiCollisionData::NOTEST);
        spNewNode->SetCollisionObject(pkColData);

        for(uiI = 0; uiI < uiChildren; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);

            if (pkChild == NULL)
                continue;

            if (NiDynamicCast(NiNode, pkChild) == NULL)
                continue;

            pkColData = NiGetCollisionData(pkChild);

            if (pkColData == NULL)
                continue;

            if (pkColData->GetCollisionMode() ==
                NiCollisionData::NOTEST)
            {
                NiAVObjectPtr pTmp = pkNode->DetachChild( pkChild );
                spNewNode->AttachChild( pTmp, true );
                 
                pTmp->SetCollisionObject(NULL);

                uiChk++;
            }
        }

        NIASSERT(uiChk == uiNoTestCnt);
        spNewNode->SetName("NO_TEST_GROUP");
        pkNode->AttachChild(spNewNode,true);
    }

    // Traverse children
    uiChildren = pkNode->GetArrayCount();
    for(uiI = 0; uiI < uiChildren; uiI++)
    {
        // get a child
        NiAVObject* pkChild = pkNode->GetAt(uiI);

        if (pkChild == NULL)
            continue;

        if (NiDynamicCast(NiNode, pkChild) == NULL)
            continue;

        if (pkChild->GetName())
            if (strcmp("NO_TEST_GROUP",pkChild->GetName()) == 0)
                continue;

        NiOptimizeABV::GroupNoTestNodes( (NiNode*)pkChild );
    }
}
//---------------------------------------------------------------------------
const char* NiOptimizeABV::SafeErrorName(const char* pcName)
{
    static char acNoName[64] = "<<No Name>>";

    if (!pcName)
        return acNoName;
    else
        return pcName;
}
//---------------------------------------------------------------------------
void NiOptimizeABV::BuildError(NiTPointerList<char*>* pkErrors, 
    const char* pcError, const char* pcObjName )
{
    // Generate Error Message
    if (!pkErrors)
        return;

    char acBuffer[1024];
    NiSprintf(acBuffer, 1024, "%s    Object:: %s \n", pcError, 
        SafeErrorName(pcObjName));

    size_t stLen = strlen(acBuffer) + 1;
    char* pkString = NiAlloc(char, stLen);
    NiStrcpy(pkString, stLen, acBuffer);

    pkErrors->AddTail(pkString);
}
//---------------------------------------------------------------------------
void NiOptimizeABV::BuildUnionError(NiTPointerList<char*>* pkErrors, 
    const char* pcError, const char* pcUnionName, const char* pcObjName )
{
    // Generate Error Message
    if (!pkErrors)
        return;

    char acBuffer[1024];
    NiSprintf(acBuffer, 1024, "%s   Union:: %s    Object:: %s \n", pcError, 
        SafeErrorName(pcUnionName), SafeErrorName(pcObjName));

    size_t stLen = strlen(acBuffer) + 1;
    char* pkString = NiAlloc(char, stLen);
    NiStrcpy(pkString, stLen, acBuffer);

    pkErrors->AddTail(pkString);
}
//---------------------------------------------------------------------------
// MatchFirstTokenFunctor methods
//---------------------------------------------------------------------------
NiOptimizeABV::MatchFirstTokenFunctor::
    MatchFirstTokenFunctor(const char* pcFirstTokenName)
    : m_kFirstTokenName(pcFirstTokenName) 
{}
//---------------------------------------------------------------------------
bool NiOptimizeABV::MatchFirstTokenFunctor::IsMatch(NiAVObject* pkObject)
{
    // ABVs must have a name
    if (pkObject->GetName())
    {
        // get first token of the name
        char acName[NI_MAX_PATH];
        char acToken[NI_MAX_PATH];
        char acSeps[] = " \n";
        NiStrcpy(acName, NI_MAX_PATH, pkObject->GetName());

        size_t stLen = strlen(acName);

        // Empty "" strings seem to cause strtok to choke.
        if (stLen > 0)
        {
            char* pcContext;
            NiStrcpy(acToken, NI_MAX_PATH, 
                NiStrtok(acName, acSeps, &pcContext));

            // if first token equals pcName, delete
            if (!strncmp(acToken, m_kFirstTokenName, sizeof(acToken)))
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// RemoveByFirstTokenFunctor methods
//---------------------------------------------------------------------------
NiOptimizeABV::RemoveByFirstTokenFunctor
    ::RemoveByFirstTokenFunctor(const char* pcFirstTokenName)
    : MatchFirstTokenFunctor(pcFirstTokenName) 
{}
//---------------------------------------------------------------------------
void NiOptimizeABV::RemoveByFirstTokenFunctor
    ::operator() (NiAVObject* pkAVObject)
{
    if (IsMatch(pkAVObject))
    {
        pkAVObject->GetParent()->DetachChild(pkAVObject);
        pkAVObject = 0;
    }
}
//---------------------------------------------------------------------------
// RemoveCollisionDataProxyMeshFunctor methods
//---------------------------------------------------------------------------
NiOptimizeABV::RemoveCollisionDataProxyMeshFunctor
    ::RemoveCollisionDataProxyMeshFunctor(NiTPointerList<char*>* pkErrors)
    : MatchFirstTokenFunctor("NDLCD"), m_pkErrors(pkErrors) 
{}
//---------------------------------------------------------------------------
void NiOptimizeABV::RemoveCollisionDataProxyMeshFunctor
    ::operator() (NiAVObject* pkAVObject)
{
    if (IsMatch(pkAVObject))
    {
        // Collision Data Proxy Mesh should not be included in the Bone
        // LOD controller since  this will result in a NULL object reference
        // after the proxy mesh is removed. Unfortunately, there is no
        // way to remove the node from the NiBoneLODController once it has 
        // been instantiated (earlier in the tool chain). The best we can do
        // is warn the user and not remove the collision data proxy to avoid 
        // a NULL object reference (which will result in a crash when loading
        // the scene graph).
        const char* ppcTags[1] = {"NiBoneLOD#"};
        if (m_pkErrors != NULL &&
            NiOptimize::CheckForExtraDataTags(pkAVObject, ppcTags, 1) && 
            BoneLODControllerExists(pkAVObject))
        {
            NiOptimizeABV::BuildError(m_pkErrors, "ERROR: Collision data "
                "proxy mesh (NDLCD) is included in Bone LOD controller "
                "and cannot be removed. Remove \"NiBoneLOD#\" "
                "User Defined property from", pkAVObject->GetName());    
        }
        else
        {
            pkAVObject->GetParent()->DetachChild(pkAVObject);
            pkAVObject = 0;
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimizeABV::RemoveCollisionDataProxyMeshFunctor
    ::BoneLODControllerExists(NiAVObject* pkAVObject)
{
    if (pkAVObject != NULL)
    {
        NiBoneLODController* pkBLOD = 
            NiGetController(NiBoneLODController, pkAVObject);
        if (pkBLOD)
        {
            return true;
        }
        else
        {
            return BoneLODControllerExists(pkAVObject->GetParent());
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// FindByFirstTokenFunctor methods
//---------------------------------------------------------------------------
NiOptimizeABV::FindByFirstTokenFunctor
    ::FindByFirstTokenFunctor(const char* pcFirstTokenName) 
    : MatchFirstTokenFunctor(pcFirstTokenName), m_pkFound(NULL) 
{}
//---------------------------------------------------------------------------
bool NiOptimizeABV::FindByFirstTokenFunctor
    ::operator() (NiAVObject* pkAVObject)
{
    if (IsMatch( pkAVObject))
    {
        m_pkFound = NiDynamicCast(NiNode, pkAVObject);
        if (m_pkFound != NULL)
        {
            return false;
        }
    }
    return true;
}
