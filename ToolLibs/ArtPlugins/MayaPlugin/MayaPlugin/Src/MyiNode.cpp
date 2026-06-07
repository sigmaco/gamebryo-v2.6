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

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "maya/MFnTransform.h"
#include "maya/MFnAttribute.h"
#include "maya/MBoundingBox.h"



//  After exparamentation this is the order which transforms seem to happen
//
//      Pivot -> Joint Orientation -> Rotate Axis -> Rotation
//
//  SO to create this in the scene graph
//
//      m_spNode -> m_spRotateAxis -> m_spPivot -> m_spOrientation
//      
//

//---------------------------------------------------------------------------
MyiNode::~MyiNode(void)
{
    ;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleLight(int iComponentNum, 
                          NiTList<NiAVObject*>& pkSelectedObjects,
                          NiLightPtr* sppLights)
{
    NIASSERT(m_pkTransformController == NULL);

    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iLightNum = gMDtObjectGetTypeIndex(iComponentNum);
    NI_UNUSED_ARG(iLightNum);

    // Determine if Light is animated...
    CheckForAnimations(iComponentNum);

    m_spNode = CreateNiNode(iComponentNum, ETypeLight);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }
    
    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeLight);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms. This sets the status.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren(iComponentNum, m_spNode, pkSelectedObjects, sppLights))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleParticleSystemEmitter(int iComponentNum, 
    NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights )
{
    NIASSERT(m_pkTransformController == NULL);

    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iParticleSystemEmitterNum = gMDtObjectGetTypeIndex(iComponentNum);
    NI_UNUSED_ARG(iParticleSystemEmitterNum);

    // Determine if Light is animated...
    CheckForAnimations(iComponentNum);

    m_spNode = CreateNiNode(iComponentNum, ETypeEmitter);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeEmitter);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms. This sets the status.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren(iComponentNum, m_spNode, pkSelectedObjects, sppLights))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleParticleSystemParticles(int iComponentNum, 
    NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights)
{
    NIASSERT(m_pkTransformController == NULL);

    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iParticleSystemParticlesNum = gMDtObjectGetTypeIndex(iComponentNum);
    NI_UNUSED_ARG(iParticleSystemParticlesNum);

    // Determine if Light is animated...
    CheckForAnimations(iComponentNum);

    m_spNode = CreateNiNode(iComponentNum, ETypeParticles);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeParticles);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren( iComponentNum, m_spNode, pkSelectedObjects, sppLights ))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleLevelOfDetail(int iComponentNum, 
                                  NiTList<NiAVObject*>& pkSelectedObjects,
                                  NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iLevelOfDetail = gMDtObjectGetTypeIndex(iComponentNum);

    // Determine if Light is animated...
    CheckForAnimations(iComponentNum);

    NiLODNode* pkLODNode = NiNew NiLODNode;
    m_spNode = (NiNode*)pkLODNode;

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }


    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeLevelOfDetail);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren( iComponentNum, m_spNode, pkSelectedObjects, sppLights ))
        return false;

    kMDtLevelOfDetail* pLOD = 
        gLevelOfDetailManager.GetLevelOfDetail(iLevelOfDetail);

    // Because of the way LOD manager gets it's data the data is ALWAYS
    //  in the current units if we want it or not.

    float fScaleForCurrentUnits = 1.0f;
    if (!gExport.m_bUseCurrentWorkingUnits)
        fScaleForCurrentUnits = 1.0f / (float)gMDtGetLinearUnitMultiplier();


    if (pLOD->m_bUseScreenLOD)
    {
        NiScreenLODData* pkScreenData = NiNew NiScreenLODData;
        pkScreenData->SetNumProportions(pLOD->m_iNumScreenThresholds);

        int iLoop;
        for (iLoop = 0; iLoop < pLOD->m_iNumScreenThresholds; iLoop++)
        {
            pkScreenData->SetProportion(iLoop, 
                pLOD->m_afScreenThresholds[iLoop]);
        }

        pkLODNode->SetLODData(pkScreenData);
    }
    else
    {

        // Now the Children are Created Create the Ranges for Each 
        NiRangeLODData* pkRangeData = NiNew NiRangeLODData;
        pkRangeData->SetNumRanges(pLOD->m_iNumThresholds + 1);

        MStatus kStat = MStatus::kSuccess;
        MFnDagNode kFnDagNode(pLOD->m_MObjectLOD);
        MBoundingBox kBoundingBox = kFnDagNode.boundingBox(&kStat);
        if(kStat != MStatus::kSuccess)
        {
            NIASSERT("Getting the bounding box failed!");
            DtExt_Err("Getting the bounding box failed for LOD node %s!",  
                pkLODNode->GetName());
            
        }

        double dDiameter = kBoundingBox.max().distanceTo(kBoundingBox.min());
        float fBoundingRadius = (float)(dDiameter / 2.0); 

        float fStartDistance = 0.0f;
        float fEndDistance = (pLOD->m_pfThresholds[0] * fScaleForCurrentUnits) 
            + fBoundingRadius;

        pkRangeData->SetRange(0, fStartDistance, fEndDistance);

        int iLoop;
        for (iLoop = 1; iLoop < pLOD->m_iNumThresholds; iLoop++)
        {
            fStartDistance = (pLOD->m_pfThresholds[iLoop - 1] * 
                fScaleForCurrentUnits) + fBoundingRadius;
            fEndDistance = (pLOD->m_pfThresholds[iLoop] * 
                fScaleForCurrentUnits) + fBoundingRadius;

            pkRangeData->SetRange(iLoop, fStartDistance, fEndDistance);

        }

        fStartDistance = (pLOD->m_pfThresholds[pLOD->m_iNumThresholds - 1] * 
            fScaleForCurrentUnits) + fBoundingRadius;
        fEndDistance = NiSqrt(NI_INFINITY);

        pkRangeData->SetRange(pLOD->m_iNumThresholds, fStartDistance, 
            fEndDistance);

        pkLODNode->SetLODData(pkRangeData);
    }

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleCamera(int iComponentNum, 
                           NiTList<NiAVObject*>& pkSelectedObjects,
                           NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iCameraNum;

    // FIND AND CREATE THE CAMERA
    int iCameraCount;
    DtCameraGetCount( &iCameraCount );

    // COMPARE EACH CAMERA TO THE CURRENT COMPONENT TO FIND THE MATCH
    int iLoop;
    for (iLoop = 0; iLoop < iCameraCount; iLoop++ )
    {
        MObject CameraTransformObject;
        MObject ComponentTransformObject;

        DtExt_CameraGetTransform( iLoop, CameraTransformObject );
        gMDtObjectGetTransform( iComponentNum, ComponentTransformObject );

        if (CameraTransformObject == ComponentTransformObject)
        {
            iCameraNum = iLoop;         
            break;
        }
    }


    // CHECK FOR DEFAULT CAMERAS
    if (!gExport.m_bExportDefaultMayaCameras &&
        ( ( strcmp( gMDtObjectGetName( iComponentNum ), "persp") == 0 ) ||
          ( strcmp( gMDtObjectGetName( iComponentNum ), "top") == 0 ) ||
          ( strcmp( gMDtObjectGetName( iComponentNum ), "front") == 0 ) ||
          ( strcmp( gMDtObjectGetName( iComponentNum ), "side") == 0 ) ) )
    {
        SetStatus(MYIOBJ_SUCCESS);
        return false;
    }


    CheckForAnimations(iComponentNum);

    // CREATE A NEW NODE FOR THE CAMERA
    m_spNode = CreateNiNode(iComponentNum, ETypeCamera);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeCamera);

        // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;


    // CREATE THE NEW CAMERA AND ADD IT TO THE CAMERA NODE
    MyiCamera kNewCamera( iCameraNum );

    m_spNode->AttachChild((NiAVObject*)kNewCamera.GetCamera());

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren( iComponentNum, m_spNode, pkSelectedObjects, 
            sppLights ))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleJointBone(int iComponentNum, 
                              NiTList<NiAVObject*>& pkSelectedObjects,
                              NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iJointNum = gMDtObjectGetTypeIndex(iComponentNum);
    NI_UNUSED_ARG(iJointNum);

    CheckForAnimations(iComponentNum);

    m_spNode = CreateNiNode(iComponentNum, ETypeJoint);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeJoint);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Check for a Negative Scale on the Joint
    float afS[3];
    gMDtObjectGetScale( iComponentNum, &afS[0], &afS[1], &afS[2] );
    
    // Check for negative Scale
    if ((afS[0] < 0.0f) || (afS[1] < 0.0f) || (afS[2] < 0.0f))
    {
        char acBuffer[1024];
        NiSprintf(acBuffer, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
            "Error:: Gamebryo does not support export of negative scale on "
            "Joints. Joint::%s\n", m_spNode->GetName());

        DtExt_Err(acBuffer);
    }


    // Set the transforms.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren( iComponentNum, GetLastNode(), pkSelectedObjects, 
            sppLights))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
void MyiNode::AttachUserDefinedAttributeDataToNode(int iComponentNum, 
    EType eType)
{
    // The node that is being created is for a shape, a light, or a
    // joint-bone, or something... We want to see if there is any
    // user data assigned to the component, and we'll attach it
    // to the components *** node ***.
    MObject mObj = MObject::kNullObj;
    NiObjectNET *pObject;
    MFnDagNode dgNode;

    int iNum = gMDtObjectGetTypeIndex(iComponentNum);
    
        // Apply any user data on the Shape
    switch(eType)
    {
        case ETypeShape: DtExt_ShapeGetShapeNode(iNum, mObj); break;
        case ETypeLight: DtExt_LightGetShapeNode(iNum, mObj); break;
        case ETypeCamera:DtExt_CameraGetShapeNode(iNum, mObj); break;
        case ETypeJoint: mObj = gJointGetJointNode(iNum); break;
        case ETypeEmitter: DtExt_EmitterGetShapeNode(iNum, mObj); break;
        case ETypeParticles: DtExt_ParticlesGetShapeNode(iNum, mObj); break;
        case ETypeLevelOfDetail:
        default:
            break;
    }

    // Only attach attributes if the mObj is valid
    if (mObj != MObject::kNullObj)
    {
        dgNode.setObject( mObj );
        pObject = (NiObjectNET*)m_spNode;
        AttachUserDefinedAttributes( dgNode, pObject );

        // Clear the mObj for the next Object
        mObj = MObject::kNullObj;
    }


        // Apply any user data on the Transform node as well
    switch(eType)
    {
        case ETypeShape: 
            DtExt_ShapeGetTransform(iNum, mObj); 
            break;
        case ETypeLight: 
            DtExt_LightGetTransform(iNum, mObj); 
            break;
        case ETypeCamera:
            DtExt_CameraGetTransform(iNum, mObj);
            break;
        case ETypeParticles: 
            DtExt_ParticlesGetTransform(iNum, mObj); 
            break;
        case ETypeLevelOfDetail: 
            DtExt_LevelOfDetailGetTransform(iNum, mObj); 
            break;
        case ETypeEmitter:      
        default:
            break;;
    }

    // Only attach attributes if the mObj is valid
    if (mObj != MObject::kNullObj)
    {
        dgNode.setObject( mObj );
        pObject = (NiObjectNET*)m_spNode;
        AttachUserDefinedAttributes( dgNode, pObject );
    }

}
//---------------------------------------------------------------------------
NiNodePtr MyiNode::CreateNiNode(int iComponentNum, EType eType)
{
    // Look at the DagNode to determine if there are the correct 
    // Extra User Data fields to control what type of NiNode we create
    MObject mObj;
    MFnDagNode dgNode;

    int iNum = gMDtObjectGetTypeIndex(iComponentNum);
    NiNodePtr spNewNode = NULL;
    
    switch(eType)
    {
        case ETypeShape:
            DtExt_ShapeGetTransform( iNum, mObj );
            break;
        case ETypePhysXShape:
            DtExt_PhysXGetTransform( iNum, mObj );
            break;
        case ETypeLight:
            DtExt_LightGetTransform( iNum, mObj );
            break;
        case ETypeCamera:
            DtExt_CameraGetTransform(iNum, mObj );
            break;
        case ETypePhysXJoint:  // Empty node
        case ETypePhysXClothConstr:
        default: {
            // Can't get the MObject so don't check for special user data
            return NiNew NiNode;
        }
    }

    dgNode.setObject( mObj );

    // Determine the correct type of Node to create
    if (!CreateBillboardNode(spNewNode, dgNode))
    {
        if (!CreateSwitchNode(spNewNode, dgNode))
        {
        }
    }

        // Check to see if none of the specialty nodes were created
    if (spNewNode == NULL)
    {
        spNewNode = NiNew NiNode;
    }

    if (eType == ETypeShape) // Even pure transforms are shapes
        AddPhysicsProperties(spNewNode, dgNode);

    AddZBufferProperties(spNewNode, dgNode);
    AddSgoProperties(spNewNode, dgNode);
    AddWireFrameProperties(spNewNode, dgNode);
    AddShaderProperties(spNewNode, dgNode);
    AddStencilProperties(spNewNode, dgNode);
    AddNoStripifyAttributes(spNewNode, dgNode);

    return spNewNode;
}
//---------------------------------------------------------------------------
bool MyiNode::CreateBillboardNode(NiNodePtr &spNewNode, MFnDagNode& dgNode)
{

    int iBillboardType;
    if (GetExtraAttribute(dgNode, "Ni_BillboardNode", true, iBillboardType))
    {
        NiBillboardNodePtr pTemp = NiNew NiBillboardNode();
        
        switch(iBillboardType)
        {
        case 0:
            pTemp->SetMode(NiBillboardNode::ALWAYS_FACE_CAMERA);
            break;
        case 1:
            pTemp->SetMode(NiBillboardNode::ROTATE_ABOUT_UP);
            break;
        case 2:
            pTemp->SetMode(NiBillboardNode::RIGID_FACE_CAMERA);
            break;
        case 3:
            pTemp->SetMode(NiBillboardNode::ALWAYS_FACE_CENTER);
            break;
        case 4:
            pTemp->SetMode(NiBillboardNode::RIGID_FACE_CENTER);
            break;
        }
        spNewNode = pTemp;
        return true;
    }

    
// *****************************************************************
// OLD METHOD MAINTAINED FOR BACKWARD COMPATABILITY
// *****************************************************************

        // Check for a billboard nodes
    if (CheckForExtraAttribute(dgNode,"Billboard",true) ||
        CheckForExtraAttribute(dgNode,"Ni_Billboard",true))
    {
        NiBillboardNodePtr pTemp = NiNew NiBillboardNode();
        pTemp->SetMode(NiBillboardNode::ALWAYS_FACE_CAMERA);
        spNewNode = pTemp;
        return true;
    }

    if (CheckForExtraAttribute(dgNode,"BillboardUp",true) ||
            CheckForExtraAttribute(dgNode,"Ni_BillboardUp",true))
    {
        NiBillboardNodePtr pTemp = NiNew NiBillboardNode();
        pTemp->SetMode(NiBillboardNode::ROTATE_ABOUT_UP);
        spNewNode = pTemp;
        return true;
    }

    if (CheckForExtraAttribute(dgNode,"BillboardRigid",true) ||
            CheckForExtraAttribute(dgNode,"Ni_BillboardRigid",true))
    {
        NiBillboardNodePtr pTemp = NiNew NiBillboardNode();
        pTemp->SetMode(NiBillboardNode::RIGID_FACE_CAMERA);
        spNewNode = pTemp;
        return true;
    }

// *****************************************************************
// OLD METHOD MAINTAINED FOR BACKWARD COMPATABILITY
// *****************************************************************

    return false;
}
//---------------------------------------------------------------------------
// This feature has been removed from Gamebryo. Code is left here
// in case it ever re-appears.
bool MyiNode::CreateSwitchNode(NiNodePtr& spNewNode, MFnDagNode& dgNode)
{
    if (CheckForExtraAttribute(dgNode, "Ni_SwitchNode", true))
    {
        spNewNode = NiNew NiSwitchNode();

        
        //bool bUpdateAll = false;
        //GetExtraAttribute(dgNode, "Ni_SwitchUpdateAll", true, bUpdateAll);
        //spNewNode->UpdateOnlyActiveChild(!bUpdateAll);

        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void MyiNode::AddSwitchNodeProperties(NiNode* pNewNode, int iComponentNum)
{

    // Look at the DagNode to determine if there are the correct 
    // Extra User Data fields to control what type of NiNode we create
    MObject mObj;
    MFnDagNode dgNode;

    int iNum = gMDtObjectGetTypeIndex(iComponentNum);

    DtExt_ShapeGetTransform( iNum, mObj );
    dgNode.setObject( mObj );

    // Check for both Switch Nodes and Flt Animation nodes
    if (CheckForExtraAttribute(dgNode,"Ni_SwitchNode", true))
    {

        // Only set the Index if we have children
        int iNumChildren = pNewNode->GetArrayCount();

        // Only set the Index if we have children
        if (iNumChildren > 0)
        {

            // Initialize to the First Child
            ((NiSwitchNode*)pNewNode)->SetIndex(0);

            int iIndex;

            // Find out what child does it want to show.
            if (GetExtraAttribute(dgNode, "Ni_SwitchChildIndex",
                true, iIndex))
            {               
                // Only set in the range of children
                if ((iIndex > 0) && (iIndex <= iNumChildren))
                {
                    // The Artist number is 1 based
                    ((NiSwitchNode*)pNewNode)->SetIndex(iIndex-1);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyiNode::AddPhysicsProperties(NiNode* pkNewNode, MFnDagNode& dgNode)
{
    // Look at the DagNode to find out if it is in fact a physics node
    MStatus kStatus;
    unsigned int uiTypeId = dgNode.typeId().id(&kStatus);
    NI_UNUSED_ARG(uiTypeId);
    if (dgNode.typeId().id(&kStatus) != 0x0010BC04) // The NxRigidBodyNode type
        return;

    MyiPhysX::ProcessActor(pkNewNode, dgNode);
}
//---------------------------------------------------------------------------
void MyiNode::AddZBufferProperties(NiAVObject* pkObject, MFnDagNode& dgNode)
{
            // ***** Check for a Z Buffer Property *****
    int iZBuffer = -1;
    
    GetExtraAttribute(dgNode, "Ni_ZBufferProperty", true, iZBuffer);

    if ((iZBuffer == 3) ||
        CheckForExtraAttribute(dgNode,"zMode00",true) ||
       CheckForExtraAttribute(dgNode,"ZBufferTestOffWriteOff",true) ||
       CheckForExtraAttribute(dgNode,"Ni_ZBufferTestOffWriteOff",true))
    {
        // Create a ZBuffer Property for this Node
        NiZBufferProperty* pZBufferProperty = 
            (NiZBufferProperty*)pkObject->GetProperty(
                NiZBufferProperty::GetType());
        if (!pZBufferProperty)
        {
            pZBufferProperty = NiNew NiZBufferProperty();
            pkObject->AttachProperty(pZBufferProperty);
        }
        pZBufferProperty->SetZBufferTest(false);
        pZBufferProperty->SetZBufferWrite(false);
    }

    if ((iZBuffer == 1) ||
        CheckForExtraAttribute(dgNode,"zMode10",true) ||
       CheckForExtraAttribute(dgNode,"ZBufferTestOnWriteOff",true) ||
       CheckForExtraAttribute(dgNode,"Ni_ZBufferTestOnWriteOff",true))
    {
        // Create a ZBuffer Property for this Node
        NiZBufferProperty* pZBufferProperty = 
            (NiZBufferProperty*)pkObject->GetProperty(
            NiZBufferProperty::GetType());
        if (!pZBufferProperty)
        {
            pZBufferProperty = NiNew NiZBufferProperty();
            pkObject->AttachProperty(pZBufferProperty);
        }

        pZBufferProperty->SetZBufferTest(true);
        pZBufferProperty->SetZBufferWrite(false);
        
    }

    if ((iZBuffer == 2) ||
        CheckForExtraAttribute(dgNode,"zMode01",true) ||
       CheckForExtraAttribute(dgNode,"ZBufferTestOffWriteOn",true) ||
       CheckForExtraAttribute(dgNode,"Ni_ZBufferTestOffWriteOn",true))
    {
        // Create a ZBuffer Property for this Node
        NiZBufferProperty* pZBufferProperty = 
            (NiZBufferProperty*)pkObject->GetProperty(
            NiZBufferProperty::GetType());
        if (!pZBufferProperty)
        {
            pZBufferProperty = NiNew NiZBufferProperty();
            pkObject->AttachProperty(pZBufferProperty);
        }

        pZBufferProperty->SetZBufferTest(false);
        pZBufferProperty->SetZBufferWrite(true);
    }

    if ((iZBuffer == 0) ||
        CheckForExtraAttribute(dgNode,"zMode11",true) ||
       CheckForExtraAttribute(dgNode,"ZBufferTestOnWriteOn",true) ||
       CheckForExtraAttribute(dgNode,"Ni_ZBufferTestOnWriteOn",true))
    {
        // Create a ZBuffer Property for this Node
        NiZBufferProperty* pZBufferProperty = 
            (NiZBufferProperty*)pkObject->GetProperty(
            NiZBufferProperty::GetType());
        if (!pZBufferProperty)
        {
            pZBufferProperty = NiNew NiZBufferProperty();
            pkObject->AttachProperty(pZBufferProperty);
        }

        pZBufferProperty->SetZBufferTest(true);
        pZBufferProperty->SetZBufferWrite(true);
    }

}
//---------------------------------------------------------------------------
void MyiNode::AddSgoProperties(NiNode* pNewNode, MFnDagNode& dgNode)
{
    // Check for an Attribute for keeping the node from being
    // removed by SgoOptimize
    if (CheckForExtraAttribute(dgNode,"Ni_SgoKeepNode",true) &&
        (pNewNode->GetExtraData("sgoKeep") == NULL))
    {
        NiStringExtraData* pDontCull = NiNew NiStringExtraData("sgoKeep");

        pNewNode->AddExtraData("sgoKeep", pDontCull);
    }
}
//---------------------------------------------------------------------------
void MyiNode::AddWireFrameProperties(NiNode* pNewNode, MFnDagNode& dgNode)
{
    // Check for an Attribute to add a wire frame property
    if (CheckForExtraAttribute(dgNode,"Ni_WireFrame",true))
    {

        bool bValue;
        if (GetExtraAttribute(dgNode, "Ni_WireFrame", true, bValue))
        {

            NiWireframePropertyPtr pWireframeProperty = 
                NiNew NiWireframeProperty();

            pWireframeProperty->SetWireframe(bValue);

            pNewNode->AttachProperty(pWireframeProperty);
        }
        else
        {
                        // Echo to the Output Window
            char acBuffer[512];

            NiSprintf(acBuffer, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                "Error Reading the Ni_WireFrame extra attribute"
                "on %s      Is it a Boolean?\n", 
                dgNode.fullPathName().asChar());

            DtExt_Err(acBuffer);

        }
    }
}
//---------------------------------------------------------------------------
void MyiNode::AddShaderProperties(NiNode* pNewNode, MFnDagNode& dgNode)
{
    // Check for an Attribute to add a Shading property
    if (CheckForExtraAttribute(dgNode,"Ni_GouraudShading",true) ||
        CheckForExtraAttribute(dgNode,"Ni_FlatShading",true))
    {
        // APPLY THE SHADE PROPERTY
        NiShadePropertyPtr spShadeProperty = NiNew NiShadeProperty;
        if (spShadeProperty == NULL) 
            return;
        
        spShadeProperty->SetSmooth(CheckForExtraAttribute(dgNode, 
            "Ni_GouraudShading", true));

        pNewNode->AttachProperty(spShadeProperty);
    }
}
//---------------------------------------------------------------------------
void MyiNode::AddStencilProperties(NiNode* pkNewNode, MFnDagNode &dgNode)
{
    // Check for an Attribute to add a Double Sided property
    if (CheckForExtraAttribute(dgNode,"Ni_DoubleSidedNode",true) )
    {
        bool bValue;
        if (GetExtraAttribute(dgNode, "Ni_DoubleSidedNode", true, bValue))
        {
            NiStencilPropertyPtr spStencil = NiNew NiStencilProperty;

            if (bValue)
                spStencil->SetDrawMode(NiStencilProperty::DRAW_BOTH);

            pkNewNode->AttachProperty(spStencil);
        }
    }
}
//---------------------------------------------------------------------------
void MyiNode::AddNoStripifyAttributes(NiNode* pkNewNode, MFnDagNode& dgNode)
{
    if (CheckForExtraAttribute(dgNode, "Ni_No_Stripify", true))
    {
        bool bValue;
        if (GetExtraAttribute(dgNode, "Ni_No_Stripify", true, bValue))
        {
            if (bValue)
            {
                NiStringExtraData* pkSData = NiNew NiStringExtraData(
                    "NiNoStripify");
                pkNewNode->AddExtraData("NiNoStripify", 
                    (NiExtraData*)pkSData);       
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyiNode::HandleComponent(int iComponentNum, EType eType, 
    NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights)
{
    bool bResult; 

    switch(eType)
    {
        case ETypeEmitter: 
            bResult = HandleParticleSystemEmitter( iComponentNum, 
                pkSelectedObjects, sppLights); 
            break;
        case ETypeParticles: 
            bResult = HandleParticleSystemParticles(iComponentNum, 
                pkSelectedObjects, sppLights); 
            break;
        case ETypeShape: 
            bResult = HandleShape(iComponentNum, pkSelectedObjects, 
                sppLights); 
            break;
        case ETypeLight: 
            bResult = HandleLight(iComponentNum, pkSelectedObjects, 
                sppLights); 
            break;
        case ETypeCamera:
            bResult = HandleCamera(iComponentNum, pkSelectedObjects, 
                sppLights);
            break;
        case ETypeJoint: 
            bResult = HandleJointBone(iComponentNum, pkSelectedObjects, 
                sppLights);
            break;
        case ETypeLevelOfDetail: 
            bResult = HandleLevelOfDetail(iComponentNum, pkSelectedObjects, 
                sppLights); 
            break;
        case ETypePhysXShape: 
            bResult = HandlePhysXShape(iComponentNum, pkSelectedObjects, 
                sppLights); 
            break;
        case ETypePhysXJoint: 
        case ETypePhysXClothConstr: 
            bResult = HandlePhysXNode(iComponentNum, eType, pkSelectedObjects,
                sppLights); 
            break;
        default: 
            bResult = HandleUnknown(iComponentNum, pkSelectedObjects, 
                sppLights); 
            break;
    }

    if (bResult)
        AttachUserDefinedAttributeDataToNode(iComponentNum, eType);
}
//---------------------------------------------------------------------------
bool MyiNode::HandleUnknown(int iComponentNum, 
                            NiTList<NiAVObject*>& pkSelectedObjects,
                            NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";

    m_spNode = CreateNiNode(iComponentNum, ETypeUnknown);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeUnknown);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    gUserData.SetNode( iComponentNum, m_spNode);

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);

    if (!MakeChildren( iComponentNum, m_spNode, pkSelectedObjects, 
            sppLights ))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandlePhysXShape(int iComponentNum, 
    NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";

    m_spNode = CreateNiNode(iComponentNum, ETypePhysXShape);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeUnknown);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;
        
    // We don't want this to be optimized away, because we need the
    // transform when we extract the physics
    NiStringExtraData* pDontCull = NiNew NiStringExtraData("NiOptimizeKeep");
    m_spNode->AddExtraData("NiOptimizeKeep", pDontCull);

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);

    if (!MakeChildren( iComponentNum, m_spNode, pkSelectedObjects, 
            sppLights ))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandlePhysXNode(int iComponentNum, EType eType,
    NiTList<NiAVObject*>& pkSelectedObjects, NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";

    m_spNode = CreateNiNode(iComponentNum, eType);

    if (m_spNode == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeUnknown);

    // SET THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms. We might have children, but not sure why.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);

    if (!MakeChildren( iComponentNum, m_spNode, pkSelectedObjects, 
            sppLights ))
        return false;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::HandleShape(int iComponentNum, 
                          NiTList<NiAVObject*>& pkSelectedObjects,
                          NiLightPtr* sppLights)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    int iShapeNum = gMDtObjectGetTypeIndex(iComponentNum);
    NI_UNUSED_ARG(iShapeNum);

    // Shape May have a Skeleton... or it may have an Animation.
    // It will not have both since, if it has a skeleton, then
    // the Animation will be applied to the skeleton, not the
    // shape.

    // Determine if shape is animated...
    CheckForAnimations(iComponentNum);

    m_spNode = CreateNiNode(iComponentNum, ETypeShape);

    if (m_spNode == NULL)
        return false;

    AddABVNamingConvention(acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        iComponentNum, ETypeShape);

    // ADD IN THE OBJECT NAME
    NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
        gMDtObjectGetName( iComponentNum ) );

    m_spNode->SetName(acName);

    // Set the transforms.
    if (!CreateTransformsAndAnimation(iComponentNum))
        return false;

    //Tag stuff for shadows
    //Ideally more tagging would happen and transformation steps
    //would occur in the process steps.
    
    MObject kShapeObject;
    int iNum = gMDtObjectGetTypeIndex(iComponentNum);
    DtExt_ShapeGetTransform( iNum, kShapeObject);
    MFnDagNode kDagFn(kShapeObject);

    MDagPath kDagPath;
    kDagFn.getPath(kDagPath);
    kDagPath.extendToShape();
    kDagFn.setObject(kDagPath);

    //There is no way to get this info from the api it seems.
    MString kName = kDagFn.fullPathName();
    const char* pcName = kName.asChar();
    NI_UNUSED_ARG(pcName);
    MString kCommand = "objectType -isType \"mesh\"" + kName + ";";

    int iIsAMeshShape = false;
    MGlobal::executeCommand(kCommand, iIsAMeshShape, false, false);

    if(iIsAMeshShape)
    {
        kCommand = "getAttr " + kName + ".receiveShadows;";
        const char* debugCommand = kCommand.asChar();
        NI_UNUSED_ARG(debugCommand);
        int iReceivesShadows = true;
        MGlobal::executeCommand(kCommand, iReceivesShadows, false, false);
        if(!iReceivesShadows)
        {
            NiBooleanExtraData* pNonShadowReceiver = NiNew NiBooleanExtraData(
                true);
            pNonShadowReceiver->SetName("NonShadowReceiver");
            m_spNode->AddExtraData(pNonShadowReceiver);
        }

        kCommand = "getAttr " + kName + ".castsShadows;";
        int iCastsShadows = true;
        MGlobal::executeCommand(kCommand, iCastsShadows, false, false);
        if(!iCastsShadows)
        {
            NiBooleanExtraData* pNonShadowCaster = NiNew NiBooleanExtraData(
                true);
            pNonShadowCaster->SetName("NonShadowCaster");
            m_spNode->AddExtraData(pNonShadowCaster);
        }
    }

    // If this object is selected add it to the selected list
    if (gMDtObjectIsSelected(iComponentNum))
        pkSelectedObjects.AddTail((NiAVObject*)m_spNode);

    ApplyLightInclusionList(iComponentNum, sppLights);
    ApplyTextureEffects(iComponentNum, sppLights);
    CreateMorphTagExtraData(iComponentNum);
    CreateTextKeyExtraData(iComponentNum);

    if (!MakeChildren( iComponentNum, GetLastNode(), pkSelectedObjects, 
        sppLights ))
        return false;

    // Handle adding switch nodes after the children are attached
    AddSwitchNodeProperties(GetLastNode(), iComponentNum);

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
MyiNode::MyiNode(int iComponentNum, NiTList<NiAVObject*>& pkSelectedObjects,
    NiLightPtr* sppLights, MyiTextureEffectManager* pkTextureEffectManager,
    MStringArray& akParentLights)
{
    if(DtExt_HaveGui())
    {
        if (MProgressWindow::isCancelled())
        {
            SetStatus(MYIOBJ_CANCEL);
            return;
        }

        // Increment the progress window
        MProgressWindow::advanceProgress(1);
    }

    m_pkTransformController = NULL;
    m_bAnimated = false;
    m_bAnyPosAnimations = false;
    m_bAnyRotAnimations = false;

    m_afPivot[0] = 0.0f;
    m_afPivot[1] = 0.0f;
    m_afPivot[2] = 0.0f;

    m_afPivotTranslation[0] = 0.0f;
    m_afPivotTranslation[1] = 0.0f;
    m_afPivotTranslation[2] = 0.0f;

    m_afOrientation[0] = 0.0f;
    m_afOrientation[1] = 0.0f;
    m_afOrientation[2] = 0.0f;

    m_afScalePivot[0] = 0.0f;
    m_afScalePivot[1] = 0.0f;
    m_afScalePivot[2] = 0.0f;

    m_afScalePivotTranslation[0] = 0.0f;
    m_afScalePivotTranslation[1] = 0.0f;
    m_afScalePivotTranslation[2] = 0.0f;

    m_afRotateAxis[0] = 0.0f;
    m_afRotateAxis[1] = 0.0f;
    m_afRotateAxis[2] = 0.0f;
    m_bABVNode = false;

    // Copy over the parent lights
    m_akParentLights = akParentLights;

    m_pkTextureEffectManager = pkTextureEffectManager;

    // Determine iComponent Type
    EType eType = gMDtObjectGetType(iComponentNum);

    // Determine if this will be an ABV Node.
    SetABV(iComponentNum, eType);

    // HandleComponent function sets the status bits as it processes the
    // particular node type.
    HandleComponent(iComponentNum, eType, pkSelectedObjects, sppLights);
}
//---------------------------------------------------------------------------
void MyiNode::GetPivot(int iComponentNum)
{
    // There are not pivots on Joints
    if (gMDtObjectGetType(iComponentNum) == ETypeJoint)
        return;

    gMDtObjectGetPivot(iComponentNum, &m_afPivot[0], &m_afPivot[1], 
        &m_afPivot[2]);

    gMDtObjectGetPivotTranslation(iComponentNum, &m_afPivotTranslation[0], 
        &m_afPivotTranslation[1], &m_afPivotTranslation[2]);

    // Modify the Object by the scale
    ModifyTranslationByScale(iComponentNum, m_afPivot);
    ModifyTranslationByScale(iComponentNum, m_afPivotTranslation);

    // Check for the current working units
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        m_afPivot[0] *= gExport.m_fLinearUnitMultiplier;
        m_afPivot[1] *= gExport.m_fLinearUnitMultiplier;
        m_afPivot[2] *= gExport.m_fLinearUnitMultiplier;

        m_afPivotTranslation[0] *= gExport.m_fLinearUnitMultiplier;
        m_afPivotTranslation[1] *= gExport.m_fLinearUnitMultiplier;
        m_afPivotTranslation[2] *= gExport.m_fLinearUnitMultiplier;
    }
}
//---------------------------------------------------------------------------
void MyiNode::GetScalePivot(int iComponentNum)
{
    // There are not pivots on Joints
    if (gMDtObjectGetType(iComponentNum) == ETypeJoint)
        return;


    gMDtObjectGetScalePivot(iComponentNum, &m_afScalePivot[0], 
        &m_afScalePivot[1], &m_afScalePivot[2]);

    gMDtObjectGetScalePivotTranslation(iComponentNum, 
        &m_afScalePivotTranslation[0], &m_afScalePivotTranslation[1], 
        &m_afScalePivotTranslation[2]);


    // Modify the Object by the scale
    ModifyTranslationByScale(iComponentNum, m_afScalePivot);
    ModifyTranslationByScale(iComponentNum, m_afScalePivotTranslation);

    // Check for the current working units
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        m_afScalePivot[0] *= gExport.m_fLinearUnitMultiplier;
        m_afScalePivot[1] *= gExport.m_fLinearUnitMultiplier;
        m_afScalePivot[2] *= gExport.m_fLinearUnitMultiplier;

        m_afScalePivotTranslation[0] *= gExport.m_fLinearUnitMultiplier;
        m_afScalePivotTranslation[1] *= gExport.m_fLinearUnitMultiplier;
        m_afScalePivotTranslation[2] *= gExport.m_fLinearUnitMultiplier;
    }
}
//---------------------------------------------------------------------------
void MyiNode::GetOrientations(int iComponentNum)
{
    if (gMDtObjectGetType(iComponentNum) != ETypeJoint)
        return;

    gMDtObjectGetJointOrientation(iComponentNum, &m_afOrientation[0], 
        &m_afOrientation[1], &m_afOrientation[2]);
}
//---------------------------------------------------------------------------
void MyiNode::GetRotateAxis(int iComponentNum)
{
    gMDtObjectGetRotateAxis(iComponentNum, &m_afRotateAxis[0], 
        &m_afRotateAxis[1], &m_afRotateAxis[2]);
}
//---------------------------------------------------------------------------
bool MyiNode::UsesQuaternionKeys(int iComponentNum)
{
    // 10=>Quaternion
    int iKeyCount= gAnimDataGetNumKeyFrames( iComponentNum, 10); 

    if(iKeyCount != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void MyiNode::HandlePivots()
{
    if (!AnyPivots())
        return;

    NiNode* pkLastNode = GetLastNode();
    NI_UNUSED_ARG(pkLastNode);

    // Apply the Local Pivot
    if (!EquivalentToZero(m_afPivot))
    {
       
        // Compress pivot into the Scale's translate or create a new node
        if (AnyPivots() && !m_bAnyPosAnimations)
        {
            NiPoint3 kTranslate = m_spNode->GetTranslate();
            kTranslate.x += m_afPivot[0];
            kTranslate.y += m_afPivot[1];
            kTranslate.z += m_afPivot[2];
            m_spNode->SetTranslate(kTranslate);

        }

        // Set the -Pivot Translation
        NiPoint3 kTranslate = m_spPivot->GetTranslate();
        kTranslate.x = -m_afPivot[0];
        kTranslate.y = -m_afPivot[1];
        kTranslate.z = -m_afPivot[2];
        m_spPivot->SetTranslate(kTranslate);

    }

    // Apply the World Pivot
    if (!m_bAnyPosAnimations &&
        !EquivalentToZero(m_afPivotTranslation))
    {
        // APPLY the pivot to the base node
        NiPoint3 kTranslate = m_spNode->GetTranslate();
        kTranslate.x += m_afPivotTranslation[0];
        kTranslate.y += m_afPivotTranslation[1];
        kTranslate.z += m_afPivotTranslation[2];
        m_spNode->SetTranslate(kTranslate);
    }

}
//---------------------------------------------------------------------------
void MyiNode::HandleScalePivots(int iComponentNum)
{
    if (!AnyScalePivots())
        return;

    NiNode* pkLastNode = GetLastNode();
    NI_UNUSED_ARG(pkLastNode);

    // Apply the Local Pivot
    if (!EquivalentToZero(m_afScalePivot))
    {
        // Apply the Scale Pivot
        NiPoint3 kTranslate = m_spPivot->GetTranslate();
        kTranslate.x += m_afScalePivot[0];
        kTranslate.y += m_afScalePivot[1];
        kTranslate.z += m_afScalePivot[2];
        m_spPivot->SetTranslate(kTranslate);

        // Remove the Scale Pivot

        // Find the Non-Uniform Scale for this Node so we can add
        // it into the Scale Pivot
        float afScale[3];
        
        gMDtObjectGetScale( iComponentNum, &afScale[0], &afScale[1], 
            &afScale[2] );

        // Only add Non Uniform Scale for the inverse scale pivot
        if (Equivalent(afScale[0], afScale[1], afScale[2]))
        {
            afScale[0] = 1.0f;
            afScale[1] = 1.0f;
            afScale[2] = 1.0f;
        }

        NiPoint3 kTransform = m_spScalePivot->GetTranslate();
        kTransform.x -= m_afScalePivot[0] * afScale[0];
        kTransform.y -= m_afScalePivot[1] * afScale[1];
        kTransform.z -= m_afScalePivot[2] * afScale[2];
        m_spScalePivot->SetTranslate(kTransform);
    }


    // Apply the World Scale Pivot
    NiPoint3 kTransform = m_spPivot->GetTranslate();
    kTransform.x += m_afScalePivotTranslation[0];
    kTransform.y += m_afScalePivotTranslation[1];
    kTransform.z += m_afScalePivotTranslation[2];
    m_spPivot->SetTranslate(kTransform);
}
//---------------------------------------------------------------------------
void MyiNode::HandleOrientations()
{
    if (!(AnyOrientations() && m_bAnyRotAnimations) || m_bIsQuaternion)
        return;

    NiMatrix3 kRotateX;
    NiMatrix3 kRotateY;
    NiMatrix3 kRotateZ;
    kRotateX.MakeXRotation(-m_afOrientation[0]);
    kRotateY.MakeYRotation(-m_afOrientation[1]);
    kRotateZ.MakeZRotation(-m_afOrientation[2]);
    NiMatrix3 kRotate(kRotateZ * kRotateY * kRotateX);
    
    m_spJointOrientation->SetRotate(kRotate);
    
    m_spJointOrientation->AttachChild(m_spNode);


    // Move translation to the Joint Orientation 
    // *** THIS MUST BE DONE AFTER ALL PIVOTS HAVE BEEN TAKEN CARE OF ***
    m_spJointOrientation->SetTranslate(m_spNode->GetTranslate());
    m_spNode->SetTranslate(NiPoint3::ZERO);

}
//---------------------------------------------------------------------------
int MyiNode::SetRotations(int iComponentNum,NiNode* pkNode, bool bFinal)
{
    int result;
    float afRotate[3];

    if (bFinal)
    {
        result = gMDtObjectGetRotation( iComponentNum, &afRotate[0], 
            &afRotate[1], &afRotate[2]);
    }
    else
    {
        result = gMDtObjectGetRotationWOOrientation( iComponentNum, 
            &afRotate[0], &afRotate[1], &afRotate[2]);
    }

    if (result)
    {
        NiMatrix3 kRotateX;
        NiMatrix3 kRotateY;
        NiMatrix3 kRotateZ;
        kRotateX.MakeXRotation(-afRotate[0]);
        kRotateY.MakeYRotation(-afRotate[1]);
        kRotateZ.MakeZRotation(-afRotate[2]);
        NiMatrix3 kRotate(kRotateZ * kRotateY * kRotateX);

        pkNode->SetRotate(kRotate);
    }
    else
    {
        pkNode->SetRotate(NiMatrix3::IDENTITY);
    }

    return result;
}
//---------------------------------------------------------------------------
int MyiNode::SetPivots(NiNode *pkNode, float fX, float fY, float fZ)
{
    pkNode->SetTranslate( fX, fY, fZ);
    return true;
}
//---------------------------------------------------------------------------
int MyiNode::SetTranslations(int iComponentNum, NiNode* pkNode, bool bFinal)
{
    int result;

    // Find the Final translation. This is the the translation if nothing is
    // animating and all of the pivots and scales are compressed into one
    // translation
    float afTranslate[3];


    if (bFinal)
    {
        // the Final translation. This is the the translation if nothing is
        // animating and all of the pivots and scales are compressed into one
        // translation
        result = gMDtObjectGetFinalTranslation( iComponentNum, &afTranslate[0],
                    &afTranslate[1], &afTranslate[2] );
    }
    else
    {
        // This is the translation if something is animating. It is just the
        // Translation values we will need to do the pivoting by hand.
        result = gMDtObjectGetTranslation( iComponentNum, &afTranslate[0], 
                    &afTranslate[1], &afTranslate[2] );
    }
    
    if (result)
    {

        // Check for the current working units
        if (gExport.m_bUseCurrentWorkingUnits)
        {
            afTranslate[0] *= gExport.m_fLinearUnitMultiplier;
            afTranslate[1] *= gExport.m_fLinearUnitMultiplier;
            afTranslate[2] *= gExport.m_fLinearUnitMultiplier;
        }

        // Modify the translation by the scale
        ModifyTranslationByScale(iComponentNum, afTranslate);

        pkNode->SetTranslate( afTranslate[0], afTranslate[1], afTranslate[2]);
    }
    else
    {
        pkNode->SetTranslate(NiPoint3::ZERO);
    }

    return result;
}
//---------------------------------------------------------------------------
void MyiNode::SetTransforms(int iComponentNum, NiNode* pkNode)
{
    int result = 1;
    result &= SetRotations(iComponentNum, pkNode, true);
    result &= SetTranslations(iComponentNum, pkNode, true);

    // Set the Scale if it is uniform
    float afS[3];
    result &= gMDtObjectGetScale( iComponentNum, &afS[0], &afS[1], &afS[2] );

    if (!result)
    {
        pkNode->SetScale(1.0f);
    }
    
    MStatus s;
    MObject kMObject;
    gMDtObjectGetTransform(iComponentNum, kMObject);
    MFnTransform kTransform(kMObject, &s);

    if (s != MS::kSuccess)
        return;

    if (Equivalent(afS[0], afS[1], afS[2]) )
    {
        pkNode->SetScale(afS[0]);
        HandleSegmentScaleCompensate(kTransform, pkNode);
    }
    else
    {
        char* acBuffer = new char[kTransform.fullPathName().length() + 200];

        NiSprintf(acBuffer, kTransform.fullPathName().length() + 200,
            "Warning:: Object ( %s ) has non-uniform or negative scales " \
            "which is not supported by Gamebryo. Attempting to bake scale " \
            "into vertices.\n", kTransform.fullPathName().asChar());

        DtExt_Err(acBuffer);
        delete[] acBuffer;
    }

    // Check for non-xyz rotations

    if ((kTransform.rotationOrder() != MTransformationMatrix::kXYZ))
    {
        char* acBuffer = new char[kTransform.fullPathName().length() + 200];

        NiSprintf(acBuffer, kTransform.fullPathName().length() + 200,
            "Error:: Object ( %s ) contains invalid rotation order."
            "Gamebryo only "
            "supports XYZ\n", kTransform.fullPathName().asChar());

        DtExt_Err(acBuffer);
        delete[] acBuffer;
    }


    NIASSERT(result==1);
}
//---------------------------------------------------------------------------
void MyiNode::HandleSegmentScaleCompensate(MFnTransform& kTransform,
    NiNode* pkNode)
{
    // Check for Segment Scale Compensate on Joints
    MStatus kStatus;
    MPlug kCompensate = kTransform.findPlug("segmentScaleCompensate", 
        &kStatus);

    if (kStatus != MS::kSuccess)
        return;

    MPlug kInverseScaleX = kTransform.findPlug("inverseScaleX", &kStatus);
    if (kStatus != MS::kSuccess)
        return;

    MPlug kInverseScaleY = kTransform.findPlug("inverseScaleY", &kStatus);
    if (kStatus != MS::kSuccess)
        return;

    MPlug kInverseScaleZ = kTransform.findPlug("inverseScaleZ", &kStatus);
    if (kStatus != MS::kSuccess)
        return;

    bool bCompensate = false;

    // Check to see if segment scale compensate is true
    if ((kCompensate.getValue(bCompensate) != MS::kSuccess) || 
        (bCompensate == false))
        return;
    
    // See what the inverse scale is connected to.
    MPlugArray kaDestinationParentConnections;
    MPlugArray kaParentScale;
    MPlug kInverseScale = kTransform.findPlug("inverseScale", &kStatus);
    if (kStatus != MS::kSuccess)
        return;

    kInverseScale.connectedTo(kaParentScale, true, false, &kStatus);
    int iLength = kaParentScale.length();
    NI_UNUSED_ARG(iLength);
    if ((kStatus != MS::kSuccess) || (kaParentScale.length() == 0))
        return;

    // Make sure the parent scale isn't animating
    kaParentScale[0].connectedTo(kaDestinationParentConnections, true, false 
        &kStatus);

    if ((kStatus != MS::kSuccess) || 
        (kaDestinationParentConnections.length() != 0))
    {
        DtExt_Err("Error:: Object ( %s ) Segment Scale Compensate"
            " unsupported when the parent scale is animated.\n",
            kTransform.fullPathName().asChar()); 
        return;
    }


    // Read in the scale values
    float fScaleX;
    float fScaleY;
    float fScaleZ;
    if (kInverseScaleX.getValue(fScaleX) != MS::kSuccess)
        return;

    if (kInverseScaleY.getValue(fScaleY) != MS::kSuccess)
        return;

    if (kInverseScaleZ.getValue(fScaleZ) != MS::kSuccess)
        return;

    // Only Compensate for Uniform Scale
    if (!Equivalent(fScaleX, fScaleY, fScaleZ))
    {
        DtExt_Err("Error:: Object ( %s ) Segment Scale Compensate"
            " unsupported when the parent has non-uniform scale.\n",
            kTransform.fullPathName().asChar()); 
    }

    // Apply the inverse scale of the connected object
    pkNode->SetScale( pkNode->GetScale() / fScaleX);
}
//---------------------------------------------------------------------------
void MyiNode::SetInverseTransforms(int iComponentNum, NiNode* pkNode)
{
    int result = 1;
    result &= SetInverseRotations(iComponentNum, pkNode);
    result &= SetInverseTranslations(iComponentNum, pkNode);
    NIASSERT(result==1);
}
//---------------------------------------------------------------------------
int MyiNode::SetInverseRotations(int iComponentNum,NiNode* pkNode)
{
    int result;

    float afRotate[3];
    result = gMDtObjectGetRotation( iComponentNum, &afRotate[0], &afRotate[1],
        &afRotate[2]);
    
    NiMatrix3 kRotateX;
    NiMatrix3 kRotateY;
    NiMatrix3 kRotateZ;
    kRotateX.MakeXRotation(afRotate[0]);
    kRotateY.MakeYRotation(afRotate[1]);
    kRotateZ.MakeZRotation(afRotate[2]);
    NiMatrix3 kRotate(kRotateZ * kRotateY * kRotateX);
    pkNode->SetRotate(kRotate);

    return result;
}
//---------------------------------------------------------------------------
int MyiNode::SetInverseTranslations(int iComponentNum,NiNode* pkNode)
{
    int result;

    // Add the Translation Component
    float afTranslate[3];
    result = gMDtObjectGetTranslation( iComponentNum, &afTranslate[0], 
        &afTranslate[1], &afTranslate[2] );

    // Check for the current working units
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        afTranslate[0] *= gExport.m_fLinearUnitMultiplier;
        afTranslate[1] *= gExport.m_fLinearUnitMultiplier;
        afTranslate[2] *= gExport.m_fLinearUnitMultiplier;
    }

    // Modify the translation by the scale
    ModifyTranslationByScale(iComponentNum, afTranslate);

    pkNode->SetTranslate( -afTranslate[0], -afTranslate[1], -afTranslate[2]);

    return result;
}
//---------------------------------------------------------------------------
bool MyiNode::MakeChildren( int iComponentNum, NiNode* pkNode, 
                           NiTList<NiAVObject*>& pkSelectedObjects,
                           NiLightPtr* sppLights)
{
    int iNumChildren;
    int *piChildren;
    
    gMDtObjectGetChildren(iComponentNum,&iNumChildren,&piChildren);

    if (iNumChildren < 1)
    {
        SetStatus(MYIOBJ_SUCCESS);
        return true;
    }
    
    for (int i = 0; i < iNumChildren; i++)
    {
        // Note that recursion occurs in the construction
        MyiNode kMyiNode(piChildren[i], pkSelectedObjects, sppLights, 
            m_pkTextureEffectManager, m_akAffectedLights); 

        NiNode *pChild;
        
        if (!kMyiNode.Succeeded())
        {
            NiFree(piChildren);
            piChildren = NULL;
            
            // Copy the flags - could be cancel or error.
            if (kMyiNode.Error())
                SetStatus(MYIOBJ_ERROR);
            else if (kMyiNode.Cancelled())
                SetStatus(MYIOBJ_CANCEL);
            return true;
        }

        pChild = kMyiNode.GetNode();   
        
        pkNode->AttachChild(pChild);
    }

    NiFree(piChildren);
    piChildren = NULL;

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
bool MyiNode::CreateAnimations(int iComponent, NiNodePtr spNode)
{
    char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
    NI_UNUSED_ARG(acName);
    GetOrientations(iComponent);

    GetPivot(iComponent);
    GetScalePivot(iComponent);
    GetRotateAxis(iComponent);

    // Reset the Translation to be NON final translation
    SetTranslations(iComponent, m_spNode, false);

    m_bIsQuaternion = UsesQuaternionKeys(iComponent);

    // Create the Pivot Node if we need one. 
    // If there is a pivot the scale will be placed there.
    if (!EquivalentToZero(m_afPivot) ||
        AnyScalePivots())
    {
        m_spPivot = NiNew NiNode;
        
        // ADD IN THE OBJECT NAME
        char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
        NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
            gMDtObjectGetName( iComponent ) );
        NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, "_Pivot");
        m_spPivot->SetName(acName);
        m_spNode->AttachChild(m_spPivot);

        // Move the Scale to the pivot
        m_spPivot->SetScale(m_spNode->GetScale());
        m_spNode->SetScale(1.0f);
    }

    // Create the ScalePivot Nodes
    if (AnyScalePivots())
    {
        m_spScalePivot = NiNew NiNode;
        
        // ADD IN THE OBJECT NAME
        char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
        NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, 
            gMDtObjectGetName( iComponent ) );
        NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH, "_ScalePivot");
        m_spScalePivot->SetName(acName);
        m_spPivot->AttachChild(m_spScalePivot);
    }

    // Create the Rotate Axis
    if (AnyRotateAxis() && !m_bIsQuaternion)
    {
        m_spRotateAxis = NiNew NiNode;
        
        // ADD IN THE OBJECT NAME
        char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
        NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
            gMDtObjectGetName( iComponent ) );
        NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
            "_RotateAxis");
        m_spRotateAxis->SetName(acName);
        m_spNode->AttachChild(m_spRotateAxis);

        // Attach in the pivot if one exists
        if (m_spPivot)
            m_spRotateAxis->AttachChild(m_spPivot);
    }



    if (m_bAnyRotAnimations)
    {
        // Create a KF Controller if one is not defined yet
        if (m_pkTransformController == NULL)
        {
            // Create the Data
            NiTransformData* pkData = NiNew NiTransformData;

            // Create theInterpolator
            NiTransformInterpolator* pkInterp = 
                NiNew NiTransformInterpolator(pkData);

            // Create the Controller
            m_pkTransformController = NiNew NiTransformController;
            m_pkTransformController->SetInterpolator(pkInterp);

            SetAnimations(m_pkTransformController);
            m_pkTransformController->SetTarget(spNode);
        }

        MyiRotData kRotData(iComponent);

        if (!kRotData.Succeeded())
        {
            SetStatus(MYIOBJ_ERROR);
            return false;
        }

        // Get the Interpolator
        NiTransformInterpolator* pkInterp =(NiTransformInterpolator*)
            m_pkTransformController->GetInterpolator();
        
        // Get the Data
        NiTransformData* pkData = pkInterp->GetTransformData();

        if(m_bIsQuaternion)
        {
            NiRotKey* pkKeys = kRotData.GetQuatData();
            int iNumKeys = kRotData.GetQuatKeyCount();

            pkData->ReplaceRotAnim(pkKeys, iNumKeys, NiRotKey::LINKEY);
            m_pkTransformController->ResetTimeExtrema();
        }
        else
        {
            if (AnyOrientations())
            {
                m_spJointOrientation = NiNew NiNode;

                // ADD IN THE OBJECT NAME
                char acName[NI_MAX_MAYA_OBJECT_NAME_LENGTH] = "";
                NiStrcpy( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                    gMDtObjectGetName( iComponent ) );
                m_spJointOrientation->SetName(acName);

                m_spJointOrientation->AttachChild(m_spNode);

                // The top of this object needs to have the name so we need
                // to rename base node.
                NiStrcpy( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                    gMDtObjectGetName( iComponent ) );
                NiStrcat( acName, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
                    "_Base");
                m_spNode->SetName(acName);
            }

            // Get the Rotation Key
            NiEulerRotKey *pkEulerRot;
            pkEulerRot = kRotData.GetRotData();
            NIASSERT(pkEulerRot);

            // Replace the Rotation Key
            pkData->ReplaceRotAnim(pkEulerRot, 1, NiRotKey::EULERKEY);
            m_pkTransformController->ResetTimeExtrema();
        }

    }

    // For now, assume a translation component
    if (m_bAnyPosAnimations)
    {

        MyiPosData kPosData( iComponent );

        if (!kPosData.Succeeded())
        {
            SetStatus(MYIOBJ_ERROR);
            return false;
        }
    
        NiPosKey *pkPos;
        int iKeys;
    
        kPosData.GetPosData(pkPos, iKeys);

        if (AnyOrientations() && m_bAnyRotAnimations && !m_bIsQuaternion)
        {
            // Create the Data
            NiTransformData* pkData = NiNew NiTransformData();
            pkData->ReplacePosAnim(pkPos, iKeys, NiPosKey::BEZKEY);
        
            // Create the Interpolator
            NiTransformInterpolator* pkInterp = 
                NiNew NiTransformInterpolator(pkData);

            pkInterp->Collapse();

            // Create the Controller
            NiTransformController* pkTransformController2 = 
                NiNew NiTransformController;

            pkTransformController2->SetInterpolator(pkInterp);
            pkTransformController2->ResetTimeExtrema();

            SetAnimations(pkTransformController2);
            pkTransformController2->SetTarget(m_spJointOrientation);

            m_spJointOrientation->AttachChild(m_spNode);

            // Clear the Translation on the Base Node
            m_spNode->SetTranslate(NiPoint3::ZERO);

        }
        else
        {

            // Create a KF Controller if one is not defined yet
            if (m_pkTransformController == NULL)
            {
                // Create the Data
                NiTransformData* pkData = NiNew NiTransformData;

                // Create theInterpolator
                NiTransformInterpolator* pkInterp = 
                    NiNew NiTransformInterpolator(pkData);

                // Create the Controller
                m_pkTransformController = NiNew NiTransformController;
                m_pkTransformController->SetInterpolator(pkInterp);

                SetAnimations(m_pkTransformController);
                m_pkTransformController->SetTarget(spNode);
            }


            // Get the Interpolator
            NiTransformInterpolator* pkInterp =(NiTransformInterpolator*)
                m_pkTransformController->GetInterpolator();

            // Get the Data
            NiTransformData* pkData = pkInterp->GetTransformData();

            // Add in the position keys.
            pkData->ReplacePosAnim(pkPos, iKeys, NiPosKey::BEZKEY);
            m_pkTransformController->ResetTimeExtrema();

        }

    }


    // Animate Scale
    if (m_bAnyScaleAnimations)
    {
        MyiScaleData kScaleData(iComponent);
        if (!kScaleData.Succeeded())
        {
            SetStatus(MYIOBJ_ERROR);
            return false;
        }
            
        if (kScaleData.GetNumKeys() > 0)
        {
            NiBezFloatKey *pkScale;
            int iKeys;

                // Check to see if there are rotation pivots
            if (!EquivalentToZero(m_afPivot) || AnyScalePivots())
            {
                // Get the Scale Data
                kScaleData.GetScaleData(pkScale, iKeys);

                // Create the Data
                NiTransformData* pkData = NiNew NiTransformData;
                pkData->ReplaceScaleAnim(pkScale, iKeys, NiFloatKey::BEZKEY);

                // Create theInterpolator
                NiTransformInterpolator* pkInterp = 
                    NiNew NiTransformInterpolator(pkData);

                pkInterp->Collapse();

                // Create the Controller
                NiTransformController* pkTransformController2 = 
                    NiNew NiTransformController;

                pkTransformController2->SetInterpolator(pkInterp);
                pkTransformController2->ResetTimeExtrema();

                SetAnimations(pkTransformController2);
                pkTransformController2->SetTarget(m_spPivot);
            }
            else
            {
                // JUST ADD THE SCALE ANIMATION TO THE BASE NODE
                // BECAUSE THER ARE NO SCALE OR ROTATION PIVOTS
                
                // Create a KF Controller if one is not defined yet
                if (m_pkTransformController == NULL)
                {
                    // Create the Data
                    NiTransformData* pkData = NiNew NiTransformData;

                    // Create theInterpolator
                    NiTransformInterpolator* pkInterp = 
                        NiNew NiTransformInterpolator(pkData);

                    // Create the Controller
                    m_pkTransformController = NiNew NiTransformController;
                    m_pkTransformController->SetInterpolator(pkInterp);

                    SetAnimations(m_pkTransformController);
                    m_pkTransformController->SetTarget(spNode);
                }
                
                kScaleData.GetScaleData(pkScale, iKeys);

                // Get the Interpolator
                NiTransformInterpolator* pkInterp =(NiTransformInterpolator*)
                    m_pkTransformController->GetInterpolator();

                // Get the Data
                NiTransformData* pkData = pkInterp->GetTransformData();

                // Add in the position keys.
                pkData->ReplaceScaleAnim(pkScale, iKeys, NiFloatKey::BEZKEY);
                m_pkTransformController->ResetTimeExtrema();
            }
        }
    }

    // Collapse the transform interpolator if there is any animation
    if (m_pkTransformController)
    {
        // Get the Interpolator
        NiTransformInterpolator* pkInterp =(NiTransformInterpolator*)
            m_pkTransformController->GetInterpolator();

        // Collapse and Reset the Extrema
        pkInterp->Collapse();
        m_pkTransformController->ResetTimeExtrema();
    }

    // Animate Visibility
    MyiVisibleData kVisibleData(iComponent);
    if (!kVisibleData.Succeeded())
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }
    if (kVisibleData.GetNumKeys() > 0)
    {
        
        // Create the Data
        NiBoolData* pkData = NiNew NiBoolData();
        pkData->ReplaceAnim(kVisibleData.GetKeys(), kVisibleData.GetNumKeys(),
            NiAnimationKey::STEPKEY);

        // Create the Interpolator
        NiBoolInterpolator* pkInterp = NiNew NiBoolInterpolator(pkData);
        pkInterp->Collapse();

        // Create the Controller
        NiVisController* pkVisController = NiNew NiVisController;
        pkVisController->SetInterpolator(pkInterp);
        pkVisController->ResetTimeExtrema();

        pkVisController->SetTarget(spNode);
        SetAnimations(pkVisController);
    }
    return true;
}
//---------------------------------------------------------------------------
void SetAnimations(NiTimeController* pkCtrl)
{
    // This function sets up the playback settings for the NiTimeController.
    // Currently these are set based on Maya global playback settings.
    
    pkCtrl->SetAnimType(NiTransformController::APP_TIME);
    
    // Set frequency
    float fPlaybackTime = (float)gAnimControlGetPlayBackSpeed();
    // Check for a zero playback frame rate.
    if (fPlaybackTime == 0.0f )
    {
        // Echo to the Output Window
        // DtExt_Err("Animation Export Error: \n\tFor animation the Playback
        // Speed is set to 0.0 ( free ). \n\tThis is currently not 
        // supported.n");
        
        // no error, just set frequency to 1
        fPlaybackTime = 1.0f;
    }

    pkCtrl->SetFrequency(fPlaybackTime);
    
    //      0=kPlaybackOnce 
    //      1=kPlaybackLoop 
    //      2=kPlaybackOscillate => We'll just make this Reverse...  
    int iMode = gAnimControlGetPlayBackMode();
    if      (iMode == 0) 
        pkCtrl->SetCycleType(NiTransformController::CLAMP);
    else if (iMode == 2) 
        pkCtrl->SetCycleType(NiTransformController::REVERSE);
    else 
        pkCtrl->SetCycleType(NiTransformController::LOOP);

    // Explicit SetBeginKeyTime and SetEndKeyTime calls
    // are not necessary, since ResetKeyTimeExtrema is called when
    // the keyframe data is called.
}
//---------------------------------------------------------------------------
void MyiNode::ApplyLightInclusionList(int iComponent, NiLightPtr* sppLights)
{
//lightlink -q -object group1 -shp false -h false -sets false
//DtLightFindByTransformName


    // Find the light list for this object
    MStatus s;
    MObject Transform;
    gMDtObjectGetTransform(iComponent, Transform);

    MFnDagNode dgNode;

    s = dgNode.setObject(Transform);

    // Create the command to get our Attribute
    MString command = MString("lightlink -q -object ") + 
        dgNode.fullPathName(&s) + 
        MString(" -shp false -h false -sets false;");


    s = MGlobal::executeCommand(command, m_akAffectedLights);

    if (s != MS::kSuccess)
        return;

    MStringArray akLightList = m_akAffectedLights;

    // Remove the parent lights from the child lights
    unsigned int uiParentLight;
    for (uiParentLight = 0; uiParentLight < 
        m_akParentLights.length(); uiParentLight++)
    {
        unsigned int uiChildLight;
        for (uiChildLight = 0; uiChildLight < 
            akLightList.length(); uiChildLight++)
        {
            if (m_akParentLights[uiParentLight] == akLightList[uiChildLight])
            {
                akLightList.remove(uiChildLight);
                break;
            }
        }
    }


    // Loop through all the lights applied to this object
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < akLightList.length(); uiLoop++)
    {
        int iLightID = DtLightFindByTransformName(akLightList[uiLoop]);
        
        // It is possible that during an export selected
        // the light will not be included
        if (iLightID >= 0)
        {
            sppLights[iLightID]->AttachAffectedNode(m_spNode);
        }
    }
}
//---------------------------------------------------------------------------
void MyiNode::ApplyTextureEffects(int iComponent, NiLightPtr* sppLights)
{
    NI_UNUSED_ARG(sppLights);
    NI_UNUSED_ARG(iComponent);
//lightlink -q -object group1 -shp false -h false -sets false
//DtLightFindByTransformName

    MStringArray akLightList = m_akAffectedLights;

    // Remove the parent lights from the child lights
    unsigned int uiParentLight;
    for (uiParentLight = 0; uiParentLight < m_akParentLights.length(); 
        uiParentLight++)
    {
        unsigned int uiChildLight;
        for (uiChildLight = 0; uiChildLight < akLightList.length(); 
            uiChildLight++)
        {
            if (m_akParentLights[uiParentLight] == akLightList[uiChildLight])
            {
                akLightList.remove(uiChildLight);
                break;
            }
        }
    }

    // Loop through all the lights applied to this object
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < akLightList.length(); uiLoop++)
    {
        int iLightID = DtLightFindByTransformName(akLightList[uiLoop]);
        
        // It is possible that during an export selected
        // the light will not be included
        if (iLightID >= 0)
        {
            NiTextureEffect *pkTextureEffect = 
                m_pkTextureEffectManager->GetTextureEffect(iLightID);
            
            // Check to see if this light is actually a Texture Effect
            if (pkTextureEffect != NULL)
            {
                // Attach This Node to the Texture Effect
                pkTextureEffect->AttachAffectedNode(m_spNode);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyiNode::CheckForAnimations(int iComponentNum)
{
    // ABV nodes should never be animated.
    if (m_bABVNode)
        return;

    // Determine if Light is animated...
    bool abPosAnimated[3], abRotAnimated[3], abScaleAnimated[3];

    m_bAnimated = gAnimDataIsObjectAnimated(iComponentNum, abPosAnimated, 
        abRotAnimated, abScaleAnimated, &m_bVisibleAnimation);

    m_bAnyPosAnimations = abPosAnimated[0] || abPosAnimated[1] || 
        abPosAnimated[2];

    m_bAnyRotAnimations = abRotAnimated[0] || abRotAnimated[1] || 
        abRotAnimated[2];

    m_bAnyScaleAnimations = abScaleAnimated[0] || abScaleAnimated[1] || 
        abScaleAnimated[2];
}
//---------------------------------------------------------------------------
void MyiNode::HandleRotateAxis()
{
    if (!AnyRotateAxis() || m_bIsQuaternion)
        return;

    // Build the rotation Matrix and apply to the pivot node
    NiMatrix3 rotatex, rotatey, rotatez;
    rotatex.MakeXRotation(-m_afRotateAxis[0]);
    rotatey.MakeYRotation(-m_afRotateAxis[1]);
    rotatez.MakeZRotation(-m_afRotateAxis[2]);
    NiMatrix3 rotateOrient(rotatez*rotatey*rotatex);
    m_spRotateAxis->SetRotate(rotateOrient);
}
//---------------------------------------------------------------------------
NiNode* MyiNode::GetLastNode()
{
    if (m_spScalePivot)
        return m_spScalePivot;

    if (m_spPivot)
        return m_spPivot;

    if (m_spRotateAxis)
        return m_spRotateAxis;
   
    return m_spNode;
}
//---------------------------------------------------------------------------
void MyiNode::CreateMorphTagExtraData(int iComponentNum)
{
    MObject Object;
    if (gMDtObjectGetTransform(iComponentNum, Object) == 0)
        return;


    MFnDagNode dgNode;
    dgNode.setObject(Object);


        // Check for a Sequence
    if (!CheckForExtraAttribute(dgNode, "Ni_MorphTag", true))
        return;

    MStatus stat;
    MString sFullPathName = dgNode.fullPathName(&stat);

    NIASSERT(stat == MS::kSuccess);

    // Collect the Number of MorphTags
    MString sCommand = "getAttr -s " + sFullPathName + ".Ni_MorphTag";

    int iTotalMorphTags;
    stat = MGlobal::executeCommand(sCommand, iTotalMorphTags);
    NIASSERT(stat == MS::kSuccess);

    int iNumMorphTags = 0;

    int iValue;
    double dValue;
    
    int iLoop;
    for (iLoop = 0; iLoop < iTotalMorphTags; iLoop++)
    {
        MString sValue;

        sCommand = "getAttr " + sFullPathName + ".Ni_MorphTag[" + iLoop + 
            "].Ni_MorphTag_Valid"; 

        stat = MGlobal::executeCommand(sCommand, iValue);
        NIASSERT(stat == MS::kSuccess);

        // Increment the Number of MorphTags if we found a valid one
        if (iValue == 1)
            iNumMorphTags++;
    }

    if (iNumMorphTags == 0)
        return;


    // Allocate the New Sequence Info
    NiTextKey* pkTextKeys = NiNew NiTextKey[iNumMorphTags];
    int iCurrentMorphTag = 0;

    for (iLoop = 0; iLoop < iTotalMorphTags; iLoop++)
    {
        sCommand = "getAttr " + sFullPathName + ".Ni_MorphTag[" + iLoop + 
            "].Ni_MorphTag_Valid"; 

        stat = MGlobal::executeCommand(sCommand, iValue);
        NIASSERT(stat == MS::kSuccess);

        // Increment the Number of MorphTags if we found a valid one
        if (iValue == 1)
        {
            // Load the Sequence Name
            MString sNameBuffer;
            char* pcTagName;

            sCommand = "getAttr " + sFullPathName + ".Ni_MorphTag[" + iLoop + 
                "].Ni_MorphTag_Name"; 

            stat = MGlobal::executeCommand(sCommand, sNameBuffer);
            NIASSERT(stat == MS::kSuccess);

            unsigned int uiLen = sNameBuffer.length() + strlen("morph:") + 1;
            pcTagName = NiAlloc(char, uiLen);
            NiStrcpy(pcTagName, uiLen, "morph:");
            NiStrcat(pcTagName, uiLen, sNameBuffer.asChar());

            pkTextKeys[iCurrentMorphTag].SetText( pcTagName );
            NiFree(pcTagName);

            // Load the Sequence StartTime
            sCommand = "getAttr " + sFullPathName + ".Ni_MorphTag[" + iLoop + 
                "].Ni_MorphTag_Time"; 

            stat = MGlobal::executeCommand(sCommand, dValue);
            NIASSERT(stat == MS::kSuccess);

            pkTextKeys[iCurrentMorphTag].SetTime((float)dValue);

            // Increment the current Morph Tag.
            iCurrentMorphTag++;
        }
    }

    NiTextKeyExtraData* pkExtraMorphTextKeys = 
        NiNew NiTextKeyExtraData(pkTextKeys, iNumMorphTags);

    m_spNode->AddExtraData("Morph Tags", pkExtraMorphTextKeys );
}
//---------------------------------------------------------------------------
void MyiNode::CreateTextKeyExtraData(int iComponentNum)
{
    MObject Object;
    if (gMDtObjectGetTransform(iComponentNum, Object) == 0)
        return;

    MFnDagNode dgNode;
    dgNode.setObject(Object);

        // Check for a Sequence
    if (!CheckForExtraAttribute(dgNode, "Ni_TextKey", true))
        return;

    MStatus stat;
    MString sFullPathName = dgNode.fullPathName(&stat);

    NIASSERT(stat == MS::kSuccess);

    // Collect the Number of TextKeys
    MString sCommand = "getAttr -s " + sFullPathName + ".Ni_TextKey";

    int iTotalTextKeys;
    stat = MGlobal::executeCommand(sCommand, iTotalTextKeys);
    NIASSERT(stat == MS::kSuccess);

    int iNumTextKeys = 0;

    int iValue;
    double dValue;

    int iLoop;
    for (iLoop = 0; iLoop < iTotalTextKeys; iLoop++)
    {
        MString sValue;

        sCommand = "getAttr " + sFullPathName + ".Ni_TextKey[" + iLoop + 
            "].Ni_TextKey_Valid"; 

        stat = MGlobal::executeCommand(sCommand, iValue);
        NIASSERT(stat == MS::kSuccess);

        // Increment the Number of TextKeys if we found a valid one
        if (iValue == 1)
            iNumTextKeys++;
    }

    if (iNumTextKeys == 0)
        return;

    // Allocate the New Text Keys Info
    NiTextKey* pkTextKeys = NiNew NiTextKey[iNumTextKeys];
    int iCurrentTextKey = 0;

    for (iLoop = 0; iLoop < iTotalTextKeys; iLoop++)
    {
        sCommand = "getAttr " + sFullPathName + ".Ni_TextKey[" + iLoop + 
            "].Ni_TextKey_Valid"; 

        stat = MGlobal::executeCommand(sCommand, iValue);
        NIASSERT(stat == MS::kSuccess);

        // Increment the Number of TextKeys if we found a valid one
        if (iValue == 1)
        {
            // Load the Sequence Name
            MString sNameBuffer;
            char* pcTagName;

            sCommand = "getAttr " + sFullPathName + ".Ni_TextKey[" + iLoop + 
                "].Ni_TextKey_Name"; 

            stat = MGlobal::executeCommand(sCommand, sNameBuffer);
            NIASSERT(stat == MS::kSuccess);

            unsigned int uiLen = sNameBuffer.length() + 1;
            pcTagName = NiAlloc(char, uiLen);
            NiStrcpy(pcTagName, uiLen, sNameBuffer.asChar());
            pkTextKeys[iCurrentTextKey].SetText( pcTagName );
            NiFree(pcTagName);

            // Load the Sequence StartTime
            sCommand = "getAttr " + sFullPathName + ".Ni_TextKey[" + iLoop + 
                "].Ni_TextKey_Time"; 

            stat = MGlobal::executeCommand(sCommand, dValue);
            NIASSERT(stat == MS::kSuccess);

            pkTextKeys[iCurrentTextKey].SetTime((float)dValue);

            // Increment the current Morph Tag.
            iCurrentTextKey++;
        }
    }

    NiTextKeyExtraData* pkExtraTextKeys = 
        NiNew NiTextKeyExtraData(pkTextKeys, iNumTextKeys);

    m_spNode->AddExtraData("Text Keys", pkExtraTextKeys );
}
//---------------------------------------------------------------------------
void MyiNode::SetABV(int iComponentNum, EType eType)
{
    // Look at the DagNode to determine if there are the correct 
    // Extra User Data fields to control what type of NiNode we create
    MObject mObj;
    MFnDagNode dgNode;

    int iNum = gMDtObjectGetTypeIndex(iComponentNum);
    NiNodePtr spNewNode = NULL;
    
    switch(eType)
    {
        case ETypeShape: DtExt_ShapeGetTransform( iNum, mObj ); break;
        case ETypeLight: DtExt_LightGetTransform( iNum, mObj ); break;
        case ETypeCamera:DtExt_CameraGetTransform(iNum, mObj ); break;
        default:

            // Can't get the MObject so don't check for special user data
            return;
    }

    dgNode.setObject( mObj );

    // Check for the New Collision Data
    GetExtraAttribute(dgNode, "Ni_EnableABVCollisionData", true, 
            m_bABVNode);
}
//---------------------------------------------------------------------------
void MyiNode::AddABVNamingConvention(char* pcName, unsigned int uiNameLen,
    int iComponentNum, EType eType)
{
    // Look at the DagNode to determine if there are the correct 
    // Extra User Data fields to control what type of NiNode we create
    MObject mObj;
    MFnDagNode dgNode;

    int iNum = gMDtObjectGetTypeIndex(iComponentNum);
    NiNodePtr spNewNode = NULL;
    
    switch(eType)
    {
        case ETypeShape: DtExt_ShapeGetTransform( iNum, mObj ); break;
        case ETypeLight: DtExt_LightGetTransform( iNum, mObj ); break;
        case ETypeCamera:DtExt_CameraGetTransform(iNum, mObj ); break;
        default:

            // Can't get the MObject so don't check for special user data
            return;
    }

    dgNode.setObject( mObj );

    bool bEnableABV;
    int iPropogate;
    int iShape;
    int iAttach;

    // Check for Old Style Collision Data
    if (GetExtraAttribute(dgNode, "Ni_EnableAlternateBoundingVolumes", true, 
            bEnableABV) &&
        GetExtraAttribute(dgNode, "Ni_ABVPropogateMode", true, iPropogate) &&
        GetExtraAttribute(dgNode, "Ni_ABVShape", true, iShape) &&
        GetExtraAttribute(dgNode, "Ni_ABVAttachType", true, iAttach))
    {
        char acBuffer[1024];
        NiSprintf(acBuffer, NI_MAX_MAYA_OBJECT_NAME_LENGTH,
            "Error:: Object ( %s ) contains Old Style ABV "
            "information. It will be Ignored\n", pcName);

        DtExt_Err(acBuffer);
    }


    // Check for the New Collision Data
    if (GetExtraAttribute(dgNode, "Ni_EnableABVCollisionData", true, 
            bEnableABV) &&
        GetExtraAttribute(dgNode, "Ni_ABV_Propogate", true, iPropogate) &&
        GetExtraAttribute(dgNode, "Ni_ABV_CollisionShape", true, iShape))
    {
        if (bEnableABV)
        {
            m_bABVNode = true;

            NiStrcat(pcName, uiNameLen, "NDLCD ");

            // Handle the Shape
            switch(iShape)
            {
            case 0:
                NiStrcat(pcName, uiNameLen, "C");
                break;
            case 1:
                NiStrcat(pcName, uiNameLen, "S");
                break;
            case 2:
                NiStrcat(pcName, uiNameLen, "H");
                break;
            case 3:
                NiStrcat(pcName, uiNameLen, "B");
                break;
            case 4:
                NiStrcat(pcName, uiNameLen, "U");
                break;
            case 5:
                NiStrcat(pcName, uiNameLen, "T");
                break;
            case 6:
                NiStrcat(pcName, uiNameLen, "O");
                break;
            case 7:
                NiStrcat(pcName, uiNameLen, "N");
                break;
            case 8:
                NiStrcat(pcName, uiNameLen, "W");
                break;
            default:
                NIASSERT(false);
            }

            // Handle the Propogation
            switch(iPropogate)
            {
            case 0:
                NiStrcat(pcName, uiNameLen, "S");
                break;
            case 1:
                NiStrcat(pcName, uiNameLen, "F");
                break;
            case 2:
                NiStrcat(pcName, uiNameLen, "A");
                break;
            case 3:
                NiStrcat(pcName, uiNameLen, "N");
                break;
            default:
                NIASSERT(false);
            }

            // Add in an additional space
            NiStrcat(pcName, uiNameLen, " ");
        }
    }
}
//---------------------------------------------------------------------------
bool MyiNode::Equivalent(float fValue1, float fValue2, float fValue3)
{
    // Check to see if these three values are equivelent
    float fEpsilon = fValue1 * 0.001f;

    // fEpsilon is expected to be a positive number. If it turns out to be
    // negative, the CloseTo functions will fail. This is really what we want,
    // since even if the 3 values are equal, if they are negative, we want
    // to treat this as if it is non-uniform since Gamebryo can not handle
    // uniform negative scales.
    if (fEpsilon < 0.0f)
        return false;

    return( NiOptimize::CloseTo(fValue1, fValue2, fEpsilon) &&
            NiOptimize::CloseTo(fValue1, fValue3, fEpsilon) );
}
//---------------------------------------------------------------------------
bool MyiNode::Equivalent(float* pfValue)
{
    return Equivalent(pfValue[0], pfValue[1], pfValue[2]);
}
//---------------------------------------------------------------------------
bool MyiNode::EquivalentToZero(float fValue1, float fValue2, float fValue3)
{
    float afEpsilon[3];
    afEpsilon[0] = fValue1 * 0.001f;
    afEpsilon[1] = fValue2 * 0.001f;
    afEpsilon[2] = fValue3 * 0.001f;

    return( NiOptimize::CloseTo(fValue1, 0.0f, afEpsilon[0]) &&
            NiOptimize::CloseTo(fValue2, 0.0f, afEpsilon[1]) &&
            NiOptimize::CloseTo(fValue3, 0.0f, afEpsilon[2]));
}
//---------------------------------------------------------------------------
bool MyiNode::EquivalentToZero(float* pfValue)
{
    return EquivalentToZero(pfValue[0], pfValue[1], pfValue[2]);
}
//---------------------------------------------------------------------------
void MyiNode::ModifyTranslationByScale(int iComponentNum, float* pfTranslate)
{
    int iParent = gMDtObjectGetParentID( iComponentNum );
    if (iParent == -1)
        return;

    // This shape has a parent.. we will modify translations to compensate
    // for all the (possibly non-uniform) scaling.
    float afScale[3];
    afScale[0] = afScale[1] = afScale[2] = 1.0f;
    gMDtObjectGetFinalNonAnimatingNotUniformScale( iComponentNum, &afScale[0], 
        &afScale[1], &afScale[2] );
    
    
    // Remove the current NOT animated Scale
    if (!gMDtObjectGetAnimatedScale(iComponentNum))
    {
        float fS[3];
        
        if (gMDtObjectGetScale( iComponentNum, &fS[0], &fS[1], &fS[2] ))
        {
            // Only Remove Non Uniform Scale for NonUniformScale
            if (!Equivalent(fS[0], fS[1], fS[2]))
            {
                if (fS[0] != 0) afScale[0] /= fS[0];
                if (fS[1] != 0) afScale[1] /= fS[1];
                if (fS[2] != 0) afScale[2] /= fS[2];
            }
        }
    }

    // Modify the translate by the scale
    pfTranslate[0] *= afScale[0];
    pfTranslate[1] *= afScale[1];
    pfTranslate[2] *= afScale[2];
}
//---------------------------------------------------------------------------
bool MyiNode::CreateTransformsAndAnimation(int iComponentNum)
{
    // Set the transforms.
    SetTransforms(iComponentNum, m_spNode);

    // Set the AppCulled Flag for this node
    m_spNode->SetAppCulled(!gMDtObjectIsVisible(iComponentNum));

    if (m_bAnimated)
    {
        if (!CreateAnimations(iComponentNum, m_spNode))
        {
            // Status set if this returns false.
            return false;
        }

        HandleRotateAxis();
        HandlePivots();
        HandleScalePivots(iComponentNum);

        //*** This must happen after any pivots ***
        HandleOrientations();
    }
    gUserData.SetNode( iComponentNum, GetLastNode());

    SetStatus(MYIOBJ_SUCCESS);
    return true;
}
//---------------------------------------------------------------------------
