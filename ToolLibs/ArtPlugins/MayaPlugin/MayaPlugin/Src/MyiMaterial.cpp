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
#include "maya/MFnAnimCurve.h"
#include "maya/MGlobal.h"

#define NI_BONES_PER_PARTITION_KEY "NiBonesPerPartition"

//---------------------------------------------------------------------------
MyiMaterial::MyiMaterial( int iMaterialID)
{
    m_iMaterialID = iMaterialID;

    // Grab Associated TextureID
    int iTextureID;
    DtTextureGetID( iMaterialID, &iTextureID );

    // Allocate a Ni Material Property
    m_spMaterial = NiNew NiMaterialProperty;
    if (m_spMaterial == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    float afColor[3];

    char* pcMaterialName;
    DtMtlGetNameByID( iMaterialID, &pcMaterialName );

    m_spMaterial->SetName(pcMaterialName);

    DtMtlGetEmissiveClr( pcMaterialName, NULL, &afColor[0], &afColor[1], 
        &afColor[2] );

    m_spMaterial->SetEmittance(NiColor(afColor[0], afColor[1], afColor[2]));

    // Get the shader node so we can get the shader type
    MObject kShader;
    DtExt_MtlGetShader(iMaterialID, kShader);

    MStatus kStatus;
    MFnDependencyNode kDNode(kShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // If there is a texture Set the Diffus Color component to be the intinsity
    // of the Diffuse Coefficient. This way with a texture you can keep the 
    // diffuse values for the material and the texture will not appear black. 
    // With the the NiMultiShader we can still read the material color because
    // is seperate from the "color"
    if ((iTextureID != -1) && (MString("NiMultiShader") != kDNode.typeName()))
    {
        // Set the material diffuse color to be the diffuse coefficient
        DtMtlGetDiffuseCoeff( pcMaterialName, NULL, &afColor[0] );
        afColor[2] = afColor[1] = afColor[0];
    }
    else
        DtMtlGetDiffuseClr( pcMaterialName, NULL, &afColor[0], &afColor[1], 
            &afColor[2] );

    m_spMaterial->SetDiffuseColor(NiColor(afColor[0], afColor[1], afColor[2]));

    DtMtlGetAmbientClr( pcMaterialName, NULL, &afColor[0], &afColor[1], 
        &afColor[2] );

    // Check for the ambient texture option
    if (gExport.m_bDefaultAmbientInTextureMaterial &&
        (iTextureID != -1) &&
        (afColor[0] == 0.0f) &&
        (afColor[1] == 0.0f) &&
        (afColor[2] == 0.0f) )
    {
        // Set the material ambient color to be the diffuse coefficient
        DtMtlGetDiffuseCoeff( pcMaterialName, NULL, &afColor[0] );
        afColor[2] = afColor[1] = afColor[0];
    }

    m_spMaterial->SetAmbientColor(NiColor(afColor[0], afColor[1], afColor[2]));

    DtMtlGetSpecularClr( pcMaterialName, NULL, &afColor[0], &afColor[1], 
        &afColor[2]);

    m_spMaterial->SetSpecularColor(NiColor(afColor[0], afColor[1], 
        afColor[2]));
            
    float fShininess;
    DtMtlGetShininess( pcMaterialName, NULL, &fShininess );
    m_spMaterial->SetShineness (fShininess);

    float fTransparency;
    DtMtlGetTransparency( pcMaterialName, NULL, &fTransparency);
    m_spMaterial->SetAlpha(1.0f - fTransparency);
           
    pcMaterialName = NULL;

    // Create any animated material colors
    CreateAnimatedMaterialColors();

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateAnimatedMaterialColors()
{
    CreateAnimatedDiffuseColor();
    CreateAnimatedSpecularColor();
    CreateAnimatedAmbientColor();
    CreateAnimatedEmittanceColor();
    CreateAnimatedAlpha();
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateAnimatedDiffuseColor()
{
    MStatus kStat = MStatus::kSuccess;

    // Get the shader node so we can get the shader type
    MObject kShader;
    DtExt_MtlGetShader(m_iMaterialID, kShader);

    MStatus kStatus;
    MFnDependencyNode kShaderNode(kShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR;
    MPlug kPlugColorG;
    MPlug kPlugColorB;

    // Check for the NiMultiShader
    if (MString("NiMultiShader") == kShaderNode.typeName())
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("MaterialDiffuseR", &kStatus);
        NIASSERT(kStatus == MS::kSuccess);

        kPlugColorG = kShaderNode.findPlug("MaterialDiffuseG", &kStatus);
        NIASSERT(kStatus == MS::kSuccess);

        kPlugColorB = kShaderNode.findPlug("MaterialDiffuseB", &kStatus);
        NIASSERT(kStatus == MS::kSuccess);
    }
    else
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("colorR", &kStatus);
        NIASSERT(kStatus == MS::kSuccess);

        kPlugColorG = kShaderNode.findPlug("colorG", &kStatus);
        NIASSERT(kStatus == MS::kSuccess);

        kPlugColorB = kShaderNode.findPlug("colorB", &kStatus);
        NIASSERT(kStatus == MS::kSuccess);
    }

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() )
    {
        return;
    }

    unsigned int uiNumKeys;
    NiPosKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertPosAnim(kPlugColorR, kPlugColorG, kPlugColorB, uiNumKeys, 
        pkKeys, eType) == MS::kFailure)
    {
        return;
    }

    // Create the Data
    NiPosData* pkData = NiNew NiPosData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    // Create the Interpolator
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator(pkData);
    pkInterp->Collapse();

    // Create the controller
    NiMaterialColorControllerPtr spController = 
        NiNew NiMaterialColorController;

    spController->SetInterpolator(pkInterp);
    spController->ResetTimeExtrema();

    spController->SetType(NiMaterialColorController::DIFF);
    spController->SetTarget(m_spMaterial);

    //get the animation controller connected to the plugs and 
    //find out what the post behavior is
    NiTimeController::CycleType eControllerCycleTypeR = 
        GetAnimationPostCycleType(kPlugColorR, kStat);


    spController->SetCycleType(eControllerCycleTypeR);
}


//---------------------------------------------------------------------------
void MyiMaterial::CreateAnimatedSpecularColor()
{
    // Get the shader node so we can get the shader type
    MObject kShader;
    DtExt_MtlGetShader(m_iMaterialID, kShader);

    MStatus kStatus;
    MFnDependencyNode kShaderNode(kShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR;
    MPlug kPlugColorG;
    MPlug kPlugColorB;

    // Check for the NiMultiShader
    if (MString("NiMultiShader") == kShaderNode.typeName())
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("MaterialSpecularR", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorG = kShaderNode.findPlug("MaterialSpecularG", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorB = kShaderNode.findPlug("MaterialSpecularB", &kStatus);
        if (kStatus != MS::kSuccess) return;
    }
    else
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("specularColorR", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorG = kShaderNode.findPlug("specularColorG", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorB = kShaderNode.findPlug("specularColorB", &kStatus);
        if (kStatus != MS::kSuccess) return;
    }

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() )
    {
        return;
    }

    unsigned int uiNumKeys;
    NiPosKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertPosAnim(kPlugColorR, kPlugColorG, kPlugColorB, uiNumKeys, 
        pkKeys, eType) == MS::kFailure)
    {
        return;
    }

    // Create the Data
    NiPosData* pkData = NiNew NiPosData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    // Create the Interpolator
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator(pkData);
    pkInterp->Collapse();

    // Create the controller
    NiMaterialColorControllerPtr spController = 
        NiNew NiMaterialColorController;

    spController->SetInterpolator(pkInterp);
    spController->ResetTimeExtrema();

    spController->SetType(NiMaterialColorController::SPEC);
    spController->SetTarget(m_spMaterial);

    MStatus kStat = MStatus::kSuccess;

    NiTimeController::CycleType eControllerCycleTypeR = 
        GetAnimationPostCycleType(kPlugColorR, kStat);

    spController->SetCycleType(eControllerCycleTypeR);
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateAnimatedAmbientColor()
{
    // Get the shader node so we can get the shader type
    MObject kShader;
    DtExt_MtlGetShader(m_iMaterialID, kShader);

    MStatus kStatus;
    MFnDependencyNode kShaderNode(kShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR;
    MPlug kPlugColorG;
    MPlug kPlugColorB;

    // Check for the NiMultiShader
    if (MString("NiMultiShader") == kShaderNode.typeName())
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("MaterialAmbientR", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorG = kShaderNode.findPlug("MaterialAmbientG", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorB = kShaderNode.findPlug("MaterialAmbientB", &kStatus);
        if (kStatus != MS::kSuccess) return;
    }
    else
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("ambientColorR", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorG = kShaderNode.findPlug("ambientColorG", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorB = kShaderNode.findPlug("ambientColorB", &kStatus);
        if (kStatus != MS::kSuccess) return;
    }

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() )
    {
        return;
    }

    unsigned int uiNumKeys;
    NiPosKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertPosAnim(kPlugColorR, kPlugColorG, kPlugColorB, uiNumKeys, 
        pkKeys, eType) == MS::kFailure)
    {
        return;
    }

    // Create the Data
    NiPosData* pkData = NiNew NiPosData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    // Create the Interpolator
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator(pkData);
    pkInterp->Collapse();

    // Create the controller
    NiMaterialColorControllerPtr spController = 
        NiNew NiMaterialColorController;

    spController->SetInterpolator(pkInterp);
    spController->ResetTimeExtrema();

    spController->SetType(NiMaterialColorController::AMB);
    spController->SetTarget(m_spMaterial);

    MStatus kStat = MStatus::kSuccess;

    NiTimeController::CycleType eControllerCycleTypeR = 
        GetAnimationPostCycleType(kPlugColorR, kStat);

    spController->SetCycleType(eControllerCycleTypeR);
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateAnimatedEmittanceColor()
{
    // Get the shader node so we can get the shader type
    MObject kShader;
    DtExt_MtlGetShader(m_iMaterialID, kShader);

    MStatus kStatus;
    MFnDependencyNode kShaderNode(kShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR;
    MPlug kPlugColorG;
    MPlug kPlugColorB;

    // Check for the NiMultiShader
    if (MString("NiMultiShader") == kShaderNode.typeName())
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("MaterialEmittanceR", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorG = kShaderNode.findPlug("MaterialEmittanceG", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorB = kShaderNode.findPlug("MaterialEmittanceB", &kStatus);
        if (kStatus != MS::kSuccess) return;
    }
    else
    {
        // examine animatable parameters to see if they're animated
        kPlugColorR = kShaderNode.findPlug("incandescenceR", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorG = kShaderNode.findPlug("incandescenceG", &kStatus);
        if (kStatus != MS::kSuccess) return;

        kPlugColorB = kShaderNode.findPlug("incandescenceB", &kStatus);
        if (kStatus != MS::kSuccess) return;
    }

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() )
    {
        return;
    }

    unsigned int uiNumKeys;
    NiPosKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertPosAnim(kPlugColorR, kPlugColorG, kPlugColorB, uiNumKeys, 
        pkKeys, eType) == MS::kFailure)
    {
        return;
    }

    // Create the Data
    NiPosData* pkData = NiNew NiPosData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    // Create the Interpolator
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator(pkData);
    pkInterp->Collapse();

    // Create the controller
    NiMaterialColorControllerPtr spController = 
        NiNew NiMaterialColorController;

    spController->SetInterpolator(pkInterp);
    spController->ResetTimeExtrema();

    spController->SetType(NiMaterialColorController::SELF_ILLUM);
    spController->SetTarget(m_spMaterial);

    MStatus kStat = MStatus::kSuccess;

    NiTimeController::CycleType eControllerCycleTypeR = 
        GetAnimationPostCycleType(kPlugColorR, kStat);

    spController->SetCycleType(eControllerCycleTypeR);
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateAnimatedAlpha()
{
    // Get the shader node so we can get the shader type
    MObject kShader;
    DtExt_MtlGetShader(m_iMaterialID, kShader);

    MStatus kStatus;
    MFnDependencyNode kShaderNode(kShader, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR = kShaderNode.findPlug("transparencyR", &kStatus);
    if (kStatus != MS::kSuccess) return;

    if (!kPlugColorR.isConnected())
    {
        return;
    }

    // Create the keys but Invert the keys because we need to convert
    // between Maya Transparency to Gamebryo Alpha
    unsigned int uiNumKeys;
    NiFloatKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertFloatAnim(kPlugColorR, uiNumKeys, pkKeys, eType, -1.0) == 
        MS::kFailure)
    {
        return;
    }
    
    // Add 1.0 to the keys to get them back to 1.0 - 0.0
    unsigned int uiLoop;

    switch(eType)
    {
    case NiAnimationKey::LINKEY:
        for(uiLoop = 0; uiLoop < uiNumKeys; uiLoop++)
        {
            ((NiLinFloatKey*)pkKeys)[uiLoop].SetValue(
                ((NiLinFloatKey*)pkKeys)[uiLoop].GetValue() + 1.0f);
        }
        break;
    case NiAnimationKey::BEZKEY:
        for(uiLoop = 0; uiLoop < uiNumKeys; uiLoop++)
        {
            ((NiBezFloatKey*)pkKeys)[uiLoop].SetValue(
                ((NiBezFloatKey*)pkKeys)[uiLoop].GetValue() + 1.0f);
        }
        break;
    default:
        NIASSERT(0);
    }

    // Create the Data
    NiFloatData* pkData = NiNew NiFloatData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    // Create the Interpolator
    NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator(pkData);
    pkInterp->Collapse();

    // Create the controller
    NiAlphaControllerPtr spController = NiNew NiAlphaController;
    spController->SetInterpolator(pkInterp);
    spController->ResetTimeExtrema();

    spController->SetTarget(m_spMaterial);
    MStatus kStat = MStatus::kSuccess;

    NiTimeController::CycleType eControllerCycleTypeR = 
        GetAnimationPostCycleType(kPlugColorR, kStat);

    spController->SetCycleType(eControllerCycleTypeR);
}
//---------------------------------------------------------------------------
MyiPixelShader* MyiMaterial::CreatePixelShader()
{
    MyiPixelShader* pkShader = NiNew MyiPixelShader;

    MObject kMaterial;
    DtExt_MtlGetShader(m_iMaterialID, kMaterial);

    MStatus kStatus;
    MFnDependencyNode kMaterialNode(kMaterial, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // Get the PixelShaderName
    MPlug kShaderName = kMaterialNode.findPlug("PixelShaderName", &kStatus);

    if (kStatus != MS::kSuccess)
        return pkShader;

    // Get the Viewer Implementation
    MPlug kImplementation = kMaterialNode.findPlug("PixelShaderImplementation",
        &kStatus);

    if (kStatus != MS::kSuccess)
        return pkShader;

    // Get the Export Implementation
    MPlug kExportImplementation = kMaterialNode.findPlug(
        "PixelShaderExportImplementation", &kStatus);

    if (kStatus != MS::kSuccess)
        return pkShader;

    MString kMSShaderName;
    kStatus = kShaderName.getValue(kMSShaderName);

    if ((kStatus != MS::kSuccess) || (kMSShaderName == ""))
        return pkShader;

    NiShaderDesc* pkShaderDesc =
        NiMaterialToolkit::GetToolkit()->GetMaterialDesc(
        kMSShaderName.asChar());

    // Check to see if there is no shader by that name
    if (pkShaderDesc == NULL)
        return pkShader;

    char pcShaderName[256];
    NiStrcpy(pcShaderName, 256, kMSShaderName.asChar());

    pkShader->m_spMaterial = NiMaterialLibrary::CreateMaterial(pcShaderName);

    // Determine the NBT method and UV channel
    pkShader->m_eNBTMethod = pkShaderDesc->GetBinormalTangentMethod();
    pkShader->m_uiNBTUVSource = pkShaderDesc->GetBinormalTangentUVSource();

    pkShader->m_bConfigurableMaterial = pkShaderDesc->
        GetConfigurableMaterial();

    // Get the Implementation
    int iImp = 0;

    // Determine if we are viewing
    char *pcSceneName;
    DtSceneGetName(&pcSceneName);

    // Set the Options
    if (strcmp(pcSceneName,"NifViewerMode")==0)
    {
        // Get the Viewer Implementation
        kStatus = kImplementation.getValue(iImp);

        if (kStatus != MS::kSuccess)
        {
            NiDelete pkShader;
            return NULL;
        }
    }
    else
    {
        // Get the Export Implementation
        kStatus = kExportImplementation.getValue(iImp);

        if (kStatus != MS::kSuccess)
        {
            NiDelete pkShader;
            return NULL;
        }
    }

    // the Shaders are 1 based in the interface but zero based in code
    unsigned int uiImp;
    if (iImp == 0)
    {
        uiImp = (unsigned int)NiShader::DEFAULT_IMPLEMENTATION;
    }
    else
    {
        uiImp = iImp - 1;
    }

    pkShader->m_uiMaterialExtraData = uiImp;

    // Create a node to hold our extra data and controllers
    pkShader->m_spAttributeNode = NiNew NiNode();

    // Add the Bones per Partition to the Mesh
    unsigned int uiBonesPerPartition = pkShaderDesc->GetBonesPerPartition();
    NiIntegerExtraData* pkBonesPerPart = 
        NiNew NiIntegerExtraData(uiBonesPerPartition);
    pkBonesPerPart->SetName(NI_BONES_PER_PARTITION_KEY);
    pkShader->m_spAttributeNode->AddExtraData(pkBonesPerPart);

    // Create the Attributes
    for(unsigned int uiALoop = 0; 
        uiALoop < pkShaderDesc->GetNumberOfAttributes(); uiALoop++)
    {
        const NiShaderAttributeDesc* pkDesc;

        if (uiALoop == 0)
            pkDesc = pkShaderDesc->GetFirstAttribute();
        else
            pkDesc = pkShaderDesc->GetNextAttribute();
        
        // Create Attributes
        if (!CreatePixelShaderAttribute(pkShader->m_spAttributeNode,
            pkShader->m_spMaterial->GetName(), pkDesc, kMaterialNode))
        {
            NiDelete pkShader;
            return NULL;
        }
    }

    return pkShader;
}
//---------------------------------------------------------------------------
bool MyiMaterial::CreatePixelShaderAttribute(NiNodePtr spNode, 
        const char* pcShaderName, const NiShaderAttributeDesc* pkDesc,
        MFnDependencyNode& kNode)
{
    // Skip Hidden Attributes
    if (pkDesc->IsHidden())
        return true;

    // Create the Base Attribute Name
    char pcBaseName[256];
    CreateAttributeName(pcShaderName, pkDesc, pcBaseName, 256);

    MPlug kMPlug;
    MStatus kStatus;
    MString MBaseName(pcBaseName);

    switch(pkDesc->GetType())
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
        {
            // Read the Plug
            kMPlug = kNode.findPlug(MBaseName, &kStatus);

            if (kStatus != MS::kSuccess) return false;

            bool bVal;
            if (kMPlug.getValue(bVal) != MS::kSuccess)
                return false;

            // Create the Extra Data
            NiBooleanExtraData* pkData = NiNew NiBooleanExtraData(bVal);
            spNode->AddExtraData( pkDesc->GetName(), pkData );
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
        {
            // Read the Plug
            kMPlug = kNode.findPlug(MBaseName, &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MString kMString;
            if (kMPlug.getValue(kMString) != MS::kSuccess)
                return false;

            NiStringExtraData* pkData = NiNew NiStringExtraData(
                kMString.asChar());

            spNode->AddExtraData(pkDesc->GetName(), pkData);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            // Read the Plug
            kMPlug = kNode.findPlug(pcBaseName, &kStatus);

            if (kStatus != MS::kSuccess) return false;

            int iVal;
            if (kMPlug.getValue(iVal) != MS::kSuccess)
                return false;

            NiIntegerExtraData* pkData = NiNew NiIntegerExtraData(iVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);

            // Create any animation
            if (kMPlug.isConnected())
            {
                char pcBuffer[512];
                NiSprintf(pcBuffer, 512, "Warning:: Animation not supported "
                    "on unsigned int attribute.  Shader: %s    Attribute: %s."
                    " \n",pcShaderName, pkDesc->GetName());
        
                DtExt_Err(pcBuffer);
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            // Read the Plug
            kMPlug = kNode.findPlug(pcBaseName, &kStatus);

            if (kStatus != MS::kSuccess) return false;

            float fVal;
            if (kMPlug.getValue(fVal) != MS::kSuccess)
                return false;

            NiFloatExtraData* pkData = NiNew NiFloatExtraData(fVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);

            // Create any animation
            if (kMPlug.isConnected())
            {
                unsigned int uiNumKeys;
                NiFloatKey* pkKeys;
                NiAnimationKey::KeyType eType;
                if (ConvertFloatAnim(kMPlug, uiNumKeys, pkKeys, eType) == 
                    MS::kSuccess)
                {
                    // Create the controller data
                    NiFloatData* pkAnimData = NiNew NiFloatData;
                    pkAnimData->ReplaceAnim(pkKeys, uiNumKeys, eType);

                    // Create the Interpolator
                    NiFloatInterpolator* pkInterp = 
                        NiNew NiFloatInterpolator(pkAnimData);

                    // Create the controller
                    NiFloatExtraDataController* pkController = 
                        NiNew NiFloatExtraDataController(pkData->GetName());

                    pkController->SetInterpolator(pkInterp);
                    pkController->ResetTimeExtrema();

                    // Attach the Controller
                    pkController->SetTarget(spNode);
                }
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            // Read the Plugs
            MPlug Plug1 = kNode.findPlug(MBaseName + MString("_1"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            MPlug Plug2 = kNode.findPlug(MBaseName + MString("_2"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            float* pfVal = NiAlloc(float, 2);

            if (Plug1.getValue(pfVal[0]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug2.getValue(pfVal[1]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            NiFloatsExtraData* pkData = NiNew NiFloatsExtraData(2, pfVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);

            // Create the Animations
            CreateFloatsAnimation(Plug1, pkData, 0, spNode );
            CreateFloatsAnimation(Plug2, pkData, 1, spNode );
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            // Read the Plugs
            MPlug Plug1 = kNode.findPlug(MBaseName + MString("_1"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            MPlug Plug2 = kNode.findPlug(MBaseName + MString("_2"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            MPlug Plug3 = kNode.findPlug(MBaseName + MString("_3"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            float* pfVal = NiAlloc(float, 3);

            if (Plug1.getValue(pfVal[0]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug2.getValue(pfVal[1]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug3.getValue(pfVal[2]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            NiFloatsExtraData* pkData = NiNew NiFloatsExtraData(3, pfVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);

            // Create the Animations
            CreateFloatsAnimation(Plug1, pkData, 0, spNode );
            CreateFloatsAnimation(Plug2, pkData, 1, spNode );
            CreateFloatsAnimation(Plug3, pkData, 2, spNode );
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            // Read the Plugs
            MPlug Plug1 = kNode.findPlug(MBaseName + MString("_1"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            MPlug Plug2 = kNode.findPlug(MBaseName + MString("_2"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            MPlug Plug3 = kNode.findPlug(MBaseName + MString("_3"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            MPlug Plug4 = kNode.findPlug(MBaseName + MString("_4"), &kStatus);
            if (kStatus != MS::kSuccess) return false;

            float* pfVal = NiAlloc(float, 4);

            if (Plug1.getValue(pfVal[0]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug2.getValue(pfVal[1]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug3.getValue(pfVal[2]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug4.getValue(pfVal[3]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            NiFloatsExtraData* pkData = NiNew NiFloatsExtraData(4, pfVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);
            NiFree(pfVal);

            // Create the Animations
            CreateFloatsAnimation(Plug1, pkData, 0, spNode );
            CreateFloatsAnimation(Plug2, pkData, 1, spNode );
            CreateFloatsAnimation(Plug3, pkData, 2, spNode );
            CreateFloatsAnimation(Plug4, pkData, 3, spNode );
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            // Read the Plugs
            MPlug Plug00 = kNode.findPlug(MBaseName + MString("_00"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug01 = kNode.findPlug(MBaseName + MString("_01"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug02 = kNode.findPlug(MBaseName + MString("_02"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug10 = kNode.findPlug(MBaseName + MString("_10"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug11 = kNode.findPlug(MBaseName + MString("_11"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug12 = kNode.findPlug(MBaseName + MString("_12"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug20 = kNode.findPlug(MBaseName + MString("_20"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug21 = kNode.findPlug(MBaseName + MString("_21"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug22 = kNode.findPlug(MBaseName + MString("_22"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            float* pfVal = NiAlloc(float, 9);

            if (Plug00.getValue(pfVal[0]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug01.getValue(pfVal[1]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug02.getValue(pfVal[2]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug10.getValue(pfVal[3]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug11.getValue(pfVal[4]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug12.getValue(pfVal[5]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug20.getValue(pfVal[6]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug21.getValue(pfVal[7]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug22.getValue(pfVal[8]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }


            NiFloatsExtraData* pkData = NiNew NiFloatsExtraData(9, pfVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            // Read the Plugs
            MPlug Plug00 = kNode.findPlug(MBaseName + MString("_00"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug01 = kNode.findPlug(MBaseName + MString("_01"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug02 = kNode.findPlug(MBaseName + MString("_02"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug03 = kNode.findPlug(MBaseName + MString("_03"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug10 = kNode.findPlug(MBaseName + MString("_10"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug11 = kNode.findPlug(MBaseName + MString("_11"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug12 = kNode.findPlug(MBaseName + MString("_12"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug13 = kNode.findPlug(MBaseName + MString("_13"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug20 = kNode.findPlug(MBaseName + MString("_20"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug21 = kNode.findPlug(MBaseName + MString("_21"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug22 = kNode.findPlug(MBaseName + MString("_22"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug23 = kNode.findPlug(MBaseName + MString("_23"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug30 = kNode.findPlug(MBaseName + MString("_30"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug31 = kNode.findPlug(MBaseName + MString("_31"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug32 = kNode.findPlug(MBaseName + MString("_32"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug33 = kNode.findPlug(MBaseName + MString("_33"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            float* pfVal = NiAlloc(float, 16);

            if (Plug00.getValue(pfVal[0]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug01.getValue(pfVal[1]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug02.getValue(pfVal[2]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug03.getValue(pfVal[3]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug10.getValue(pfVal[4]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug11.getValue(pfVal[5]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug12.getValue(pfVal[6]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug13.getValue(pfVal[7]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug20.getValue(pfVal[8]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug21.getValue(pfVal[9]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug22.getValue(pfVal[10]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug23.getValue(pfVal[11]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug30.getValue(pfVal[12]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug31.getValue(pfVal[13]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug32.getValue(pfVal[14]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug33.getValue(pfVal[15]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }


            NiFloatsExtraData* pkData = NiNew NiFloatsExtraData(16, pfVal);
            spNode->AddExtraData(pkDesc->GetName(), pkData);
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            // Read the Plugs
            MPlug Plug1 = kNode.findPlug(MBaseName + MString("Red"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug2 = kNode.findPlug(MBaseName + MString("Green"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug3 = kNode.findPlug(MBaseName + MString("Blue"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            MPlug Plug4 = kNode.findPlug(MBaseName + MString("_Alpha"), 
                &kStatus);

            if (kStatus != MS::kSuccess) return false;

            float* pfVal = NiAlloc(float, 4);

            if (Plug1.getValue(pfVal[0]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug2.getValue(pfVal[1]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug3.getValue(pfVal[2]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            if (Plug4.getValue(pfVal[3]) != MS::kSuccess)
            {
                NiFree(pfVal);
                return false;
            }

            NiColorA kColor(pfVal[0], pfVal[1], pfVal[2], pfVal[3]);
            NiColorExtraData* pkData = NiNew NiColorExtraData(kColor);

            spNode->AddExtraData(pkDesc->GetName(), pkData);

            
            if (Plug1.isConnected() || Plug2.isConnected() || 
                Plug3.isConnected() || Plug4.isConnected())
            {
                unsigned int uiNumKeys;
                NiColorKey* pkKeys;
                NiAnimationKey::KeyType eType;
                
                if (ConvertColorAlphaAnim(Plug1, Plug2, Plug3, Plug4, 
                    uiNumKeys, pkKeys, eType) == MS::kSuccess)
                {
                    // Create the Data
                    NiColorData* pkAnimData = NiNew NiColorData;
                    pkAnimData->ReplaceAnim(pkKeys, uiNumKeys, eType);

                    // Create the Interpolator
                    NiColorInterpolator* pkInterp = 
                        NiNew NiColorInterpolator(pkAnimData);

                    // Create the Controller
                    NiColorExtraDataController* pkController = 
                        NiNew NiColorExtraDataController(pkData->GetName());

                    pkController->SetInterpolator(pkInterp);
                    pkController->ResetTimeExtrema();
                    
                        MStatus kStat = MStatus::kSuccess;

                    NiTimeController::CycleType eControllerCycleTypeR = 
                        GetAnimationPostCycleType(Plug1, kStat);

                    pkController->SetCycleType(eControllerCycleTypeR);
                    pkController->SetAnimType(NiTimeController::APP_INIT);
                    pkController->SetActive(true);

                    
                    // Attach to the object
                    pkController->SetTarget(spNode);
                }
            }

        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
        {
            // Read the Plug
            kMPlug = kNode.findPlug(MBaseName + MString("_Map"), &kStatus);

            if (kStatus != MS::kSuccess) return false;

            int iVal;
            if (kMPlug.getValue(iVal) != MS::kSuccess)
                return false;

            // Create the Extra Data
            NiIntegerExtraData* pkData = NiNew NiIntegerExtraData(iVal);
            spNode->AddExtraData( pkDesc->GetName(), pkData );
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        {
            return CreatePixelShaderArrayAttribute(spNode, pcShaderName, 
                pkDesc, kNode);
        }
        break;
    default:
        break;
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyiMaterial::CreatePixelShaderArrayAttribute(NiNodePtr spNode, 
        const char* pcShaderName, const NiShaderAttributeDesc* pkDesc,
        MFnDependencyNode& kNode)
{
    NiShaderAttributeDesc::AttributeType eSubType; 
    unsigned int uiElementSize; 
    unsigned int uiNumElements;

    if (!pkDesc->GetArrayParams(eSubType, uiElementSize, uiNumElements))
        return false;

    unsigned int uiLoop;

    // Create the Base Attribute Name
    char pcBaseName[256];
    char pcAttributeName[512];

    CreateAttributeName(pcShaderName, pkDesc, pcBaseName, 256);

    MPlug kMPlug;
    MStatus kStatus;
    MString MBaseName(pcBaseName);

    switch(eSubType)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    case NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY:
        {
            NIASSERT(0); // Unsuported
            return false;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            // Build a temp array of values
            int* pkIntValues = NiAlloc(int, uiNumElements);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                NiSprintf(pcAttributeName, 512, "%s_%d", pcBaseName, uiLoop);

                // Read the Plug
                kMPlug = kNode.findPlug(MString(pcAttributeName), &kStatus);

                if (kStatus != MS::kSuccess) 
                {
                    NiFree(pkIntValues);
                    return false;
                }

                if (kMPlug.getValue(pkIntValues[uiLoop]) != MS::kSuccess)
                {
                    NiFree(pkIntValues);
                    return false;
                }
            }

            // Create the Extra Data
            NiIntegersExtraData* pkData = NiNew NiIntegersExtraData(
                uiNumElements, pkIntValues);

            spNode->AddExtraData( pkDesc->GetName(), pkData );

            // Delete our temp array
            NiFree(pkIntValues);
            return true;
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            // Deterine the number of floats in this type
            unsigned int uiNumFloats = 1;
            switch (eSubType)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                uiNumFloats = 1;
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                uiNumFloats = 2;
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                uiNumFloats = 3;
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                uiNumFloats = 4;
                break;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                uiNumFloats = 4;
                break;
            }
            unsigned int uiFloatLoop;

            // Allocate the float array
            float* pkFloatValues = NiAlloc(float, uiNumElements * uiNumFloats);

            // Add the Default Values
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                for (uiFloatLoop = 0; uiFloatLoop < uiNumFloats; uiFloatLoop++)
                {
                    // Determine the Attribute NameName
                    if (uiNumFloats == 1)
                    {
                        NiSprintf(pcAttributeName, 512, "%s_%d", pcBaseName, 
                            uiLoop);
                    } 
                    else if (eSubType == 
                            NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
                    {
                        switch(uiFloatLoop)
                        {
                        case 0:
                            NiSprintf(pcAttributeName, 512, "%s_%dRed", 
                                pcBaseName, uiLoop);
                            break;
                        case 1:
                            NiSprintf(pcAttributeName, 512, "%s_%dGreen", 
                                pcBaseName, uiLoop);
                            break;
                        case 2:
                            NiSprintf(pcAttributeName, 512, "%s_%dBlue", 
                                pcBaseName, uiLoop);
                            break;
                        case 3:
                            NiSprintf(pcAttributeName, 512, "%s_%d_Alpha", 
                                pcBaseName, uiLoop);
                            break;
                        default:
                            NIASSERT(0); // Invalid number of color components
                        }
                    }
                    else
                    {
                        NiSprintf(pcAttributeName, 512, "%s_%d_%d", pcBaseName,
                            uiLoop, uiFloatLoop + 1);
                    }

                    // Read the Plug
                    kMPlug = kNode.findPlug(MString(pcAttributeName), 
                        &kStatus);

                    if (kStatus != MS::kSuccess) 
                    {
                        NiFree(pkFloatValues);
                        return false;
                    }

                    if (kMPlug.getValue(pkFloatValues[(uiLoop * uiNumFloats) + 
                        uiFloatLoop]) != MS::kSuccess)
                    {
                        NiFree(pkFloatValues);
                        return false;
                    }
                }
            }

            // Create the Extra Data
            NiFloatsExtraData* pkData = NiNew NiFloatsExtraData(
                uiNumElements * uiNumFloats, pkFloatValues);

            spNode->AddExtraData( pkDesc->GetName(), pkData );

            // Delete our temp array
            NiFree(pkFloatValues);

            // Create the Animations
            for (uiLoop = 0; uiLoop < uiNumElements; uiLoop++)
            {
                for (uiFloatLoop = 0; uiFloatLoop < uiNumFloats; uiFloatLoop++)
                {
                    // Determine the Attribute NameName
                    if (uiNumFloats == 1)
                    {
                        NiSprintf(pcAttributeName, 512, "%s_%d", pcBaseName, 
                            uiLoop);
                    } 
                    else if (eSubType == 
                        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR)
                    {
                        switch(uiFloatLoop)
                        {
                        case 0:
                            NiSprintf(pcAttributeName, 512, "%s_%dRed", 
                                pcBaseName, uiLoop);
                            break;
                        case 1:
                            NiSprintf(pcAttributeName, 512, "%s_%dGreen", 
                                pcBaseName, uiLoop);
                            break;
                        case 2:
                            NiSprintf(pcAttributeName, 512, "%s_%dBlue", 
                                pcBaseName, uiLoop);
                            break;
                        case 3:
                            NiSprintf(pcAttributeName, 512, "%s_%d_Alpha", 
                                pcBaseName, uiLoop);
                            break;
                        default:
                            NIASSERT(0); // Invalid number of color components
                        }
                    }
                    else
                    {
                        NiSprintf(pcAttributeName, 512, "%s_%d_%d", pcBaseName,
                            uiLoop, uiFloatLoop + 1);
                    }

                    // Read the Plug
                    kMPlug = 
                        kNode.findPlug(MString(pcAttributeName), &kStatus);

                    CreateFloatsAnimation(kMPlug, pkData, 
                        (uiLoop * uiNumFloats) + uiFloatLoop, spNode );
                }
            }
            return true;
        }
        break;
    }

    return false;
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateAttributeName(const char* pcShaderName, 
    const NiShaderAttributeDesc* pkDesc, char* pcBuffer, 
    unsigned int uiBufferSize)
{
    NiSprintf(pcBuffer, uiBufferSize, "Ni_PixelShaderAttribute_%s_%s", 
        pcShaderName, pkDesc->GetName());
}
//---------------------------------------------------------------------------
void MyiMaterial::CreateFloatsAnimation(MPlug kMPlug, 
    NiFloatsExtraData* pkData, int iIndex, NiNodePtr spNode )
{
    // Create any animation
    if (kMPlug.isConnected())
    {
        unsigned int uiNumKeys;
        NiFloatKey* pkKeys;
        NiAnimationKey::KeyType eType;
        if (ConvertFloatAnim(kMPlug, uiNumKeys, pkKeys, eType) == MS::kSuccess)
        {
            // Create the controller data
            NiFloatData* pkAnimData = NiNew NiFloatData;
            pkAnimData->ReplaceAnim(pkKeys, uiNumKeys, eType);
            
            // Create the Interpolator
            NiFloatInterpolator* pkInterp = 
                NiNew NiFloatInterpolator(pkAnimData);

            // Create the controller
            NiFloatsExtraDataController* pkController = 
                NiNew NiFloatsExtraDataController(pkData->GetName(), iIndex);

            pkController->SetInterpolator(pkInterp);
            pkController->ResetTimeExtrema();
            
            // Attach the Controller
            pkController->SetTarget(spNode);
        }
    }
}
//---------------------------------------------------------------------------
NiMaterialProperty *MyiMaterial::GetMaterial()
{
    return m_spMaterial;
}

//---------------------------------------------------------------------------
int MyiMaterial::GetShadingModel() const
{
    return m_iShadingModel;
}
//---------------------------------------------------------------------------
MyiPixelShader::MyiPixelShader()
{
    m_spMaterial = NULL;
    m_spAttributeNode = NULL;
    m_uiMaterialExtraData = 
        (unsigned int)NiMaterialInstance::DEFAULT_EXTRA_DATA;
    m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NONE;
    m_uiNBTUVSource = NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;
    m_bConfigurableMaterial = false;
}
//---------------------------------------------------------------------------
MyiPixelShader::~MyiPixelShader()
{
    m_spMaterial = NULL;
    m_spAttributeNode = NULL;
}
//---------------------------------------------------------------------------
bool MyiPixelShader::IsValid() const
{
    if (m_spMaterial == NULL)
        return false;

    return true;
}
//---------------------------------------------------------------------------
