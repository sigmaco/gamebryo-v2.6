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
#include "MDtLight.h"

extern bool DtLightGetShapeNode(int lightID, MObject& node);


//---------------------------------------------------------------------------
NiLight* MyiCreateLight( int iLightType )
{
    switch( iLightType )
    {
        case DT_POINT_LIGHT:        return NiNew NiPointLight;
        case DT_DIRECTIONAL_LIGHT:  return NiNew NiDirectionalLight;
        case DT_SPOT_LIGHT:         return NiNew NiSpotLight;
        case DT_AMBIENT_LIGHT:      return NiNew NiAmbientLight;
        default:                    return NULL;
    }
}
//---------------------------------------------------------------------------
MyiLight::MyiLight( int iLightID )
{
    SetStatus(MYIOBJ_ERROR);

    m_iLightID = iLightID;
    int iLightType;
    DtLightGetType( m_iLightID,&iLightType );

    m_spLight = MyiCreateLight(iLightType);
    
    if (m_spLight == NULL)
        return;
    
    float fIntensity;
    DtLightGetIntensity(m_iLightID, &fIntensity);

    // In Maya, you can have negative intensities.
    if (fIntensity < 0) 
        fIntensity = 0;
    
    // fIntensity can range above 1.0 in Maya. This is allowed in Gamebryo,
    // though the typical range is 0.0 to 1.0. We allow the artist to do this.
    m_spLight->SetDimmer(fIntensity); 

    // Set the Color for this light
    SetColors();

    SetAttenuation();

    m_spLight->SetName(DtLightGetName( m_iLightID ));

    switch (iLightType)
    {
        case DT_POINT_LIGHT:
            if (!MakePointLight())
                return;
            break;
        case DT_SPOT_LIGHT:
            if (!MakeSpotLight())
                return;
            break;
        case DT_DIRECTIONAL_LIGHT:
            if (!MakeInfiniteLight())
                return;
            break;
        case DT_AMBIENT_LIGHT:
            if (!MakeAmbientLight())
                return;
            break;
        default:
            NIASSERT(0);
            break;
    }

    // Check to see if the light has Gamebryo special attribtes
    CheckAttributes();

    // Check for Animated colors
    CheckForAnimatedColor();

    m_spLight->Update(0.0);
    NiMesh::CompleteSceneModifiers(m_spLight);

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
MyiLight::MyiLight()
{
    // There are no lights defined in the Maya scene, so create a Gamebryo
    // light that mimics the default Maya light.

    m_spLight = MyiCreateLight(DT_AMBIENT_LIGHT);
    m_iLightID = -1;
    
    if (m_spLight == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return;
    }
    
    m_spLight->SetDiffuseColor(NiColor(1.0f, 1.0f, 1.0f));
    m_spLight->SetAmbientColor(NiColor(0.325f, 0.325f, 0.325f));
    m_spLight->SetSpecularColor(NiColor(1.0f, 1.0f, 1.0f));

    m_spLight->SetTranslate(NiPoint3(-100.0f, -100.0f, -100.0f));

    m_spLight->Update(0.0);
    NiMesh::CompleteSceneModifiers(m_spLight);

    SetStatus(MYIOBJ_SUCCESS);
}
//---------------------------------------------------------------------------
bool MyiLight::MakePointLight()
{
    // No Extra work for point Lights
    return true;
}
//---------------------------------------------------------------------------
bool MyiLight::MakeSpotLight()
{
    float fAngle;
    DtLightGetCutOffAngle(m_iLightID,&fAngle);

    float fPenumbraAngle;
    DtLightGetPenumbraAngle(m_iLightID, &fPenumbraAngle);

    // fAngle is in radians and is twice radius of the cone.
    // For Gamebryo, we need to half this and convert to degrees.
    float fToDegrees = 180.0f/3.14159265359f;
    float fDegrees = fAngle * fToDegrees;
    fDegrees /= 2.0f;

    float fPenumbraDegrees = fPenumbraAngle * fToDegrees;
    fPenumbraDegrees /= 2.0f;

    // We know the light should be a spot light, do we can perform 
    // a simple cast
    NiSpotLight *pSpotLight = NiDynamicCast(NiSpotLight, m_spLight);
    pSpotLight->SetSpotAngle(fDegrees);
    pSpotLight->SetInnerSpotAngle(fPenumbraDegrees);
    
    // Maya starts lights looking down -z axis...
    NiMatrix3 rot;
    rot.MakeYRotation(-1.5707963f);

    m_spLight->SetRotate(rot);

    return true;
}
//---------------------------------------------------------------------------
bool MyiLight::MakeInfiniteLight()
{
    // the direction that an infinite light shines in is the negation of
    // of the light's postion

    // Maya starts lights looking down -z axis...
    NiMatrix3 rot;
    rot.MakeYRotation(-1.5707963f);

    m_spLight->SetRotate(rot);

    return true;
}
//---------------------------------------------------------------------------
bool MyiLight::MakeAmbientLight()
{
    // No Extra work for Ambient Lights
    return true;
}
//---------------------------------------------------------------------------
void MyiLight::CheckAttributes()
{
    MObject MObj;

    DtLightGetShapeNode(m_iLightID, MObj);

    MFnDagNode dgNode;
    dgNode.setObject(MObj);

    float fValue;
    float fRed;
    float fGreen;
    float fBlue;

    if (GetExtraAttribute(dgNode, "Ni_AmbientColorRed", true, fRed) &&
        GetExtraAttribute(dgNode, "Ni_AmbientColorGreen", true, fGreen) &&
        GetExtraAttribute(dgNode, "Ni_AmbientColorBlue", true, fBlue) )
    {
        m_spLight->SetAmbientColor(NiColor(fRed, fGreen, fBlue));
    }

    if (GetExtraAttribute(dgNode, "Ni_SpecularColorRed", true, fRed) &&
        GetExtraAttribute(dgNode, "Ni_SpecularColorGreen", true, fGreen) &&
        GetExtraAttribute(dgNode, "Ni_SpecularColorBlue", true, fBlue) )
    {
        m_spLight->SetSpecularColor(NiColor(fRed, fGreen, fBlue));
    }

    bool bUseMayaDecayRate = false;

     // Check for defaulting to the Maya decay Rate
    GetExtraAttribute(dgNode, "Ni_UseMayaDecayRate", true, bUseMayaDecayRate);

    if (!bUseMayaDecayRate &&
        GetExtraAttribute(dgNode, "Ni_Dimmer", true, fValue))
    {
        m_spLight->SetDimmer(fValue);
    }

    NiPointLightPtr spPointLight = NiDynamicCast(NiPointLight, m_spLight);

    if (spPointLight != NULL)
    {
        if (!bUseMayaDecayRate &&
            GetExtraAttribute(dgNode, "Ni_ConstantAttenuation", true, fValue))
        {
            spPointLight->SetConstantAttenuation(fValue);
        }

        if (!bUseMayaDecayRate &&
            GetExtraAttribute(dgNode, "Ni_LinearAttenuation", true, fValue))
        {
            spPointLight->SetLinearAttenuation(fValue / 
                gExport.m_fLinearUnitMultiplier);
        }

        if (!bUseMayaDecayRate &&
            GetExtraAttribute(dgNode, "Ni_QuadraticAttenuation", true, fValue))
        {
            spPointLight->SetQuadraticAttenuation(fValue / 
                (gExport.m_fLinearUnitMultiplier * 
                    gExport.m_fLinearUnitMultiplier));
        }
    }
}
//---------------------------------------------------------------------------
void MyiLight::SetAttenuation()
{
    // Curently Attenuation only exist for Point & SpotLights
    if (!NiIsKindOf(NiPointLight, m_spLight))
        return;

    NiPointLight* pPointLight = NiDynamicCast(NiPointLight,m_spLight);

    // Give it something by 
    pPointLight->SetLinearAttenuation(0.0f);
    pPointLight->SetQuadraticAttenuation(0.0f);
    pPointLight->SetConstantAttenuation(1.0f);

    MObject MLightShape;

    // Get the Shape Node
    if (DtExt_LightGetShapeNode( m_iLightID, MLightShape ) == 0)
        return;

    MStatus kStatus;

    MFnNonAmbientLight kLight(MLightShape, &kStatus);

    NIASSERT(kStatus == MS::kSuccess);

    int iDecayRate = kLight.decayRate(&kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    switch(iDecayRate)
    {
    case 0:
        pPointLight->SetConstantAttenuation(1.0f);
        pPointLight->SetLinearAttenuation(0.0f);
        pPointLight->SetQuadraticAttenuation(0.0f);
        break;
    case 1:
        pPointLight->SetConstantAttenuation(0.0f);
        pPointLight->SetLinearAttenuation(1.0f / 
            gExport.m_fLinearUnitMultiplier);
        pPointLight->SetQuadraticAttenuation(0.0f);
        break;
    case 2:
    case 3:
        pPointLight->SetConstantAttenuation(0.0f);
        pPointLight->SetLinearAttenuation(0.0f);
        pPointLight->SetQuadraticAttenuation(1.0f / 
            (gExport.m_fLinearUnitMultiplier * 
                gExport.m_fLinearUnitMultiplier));
        break;
    default:
        NIASSERT(0);
    }
}
//---------------------------------------------------------------------------
void MyiLight::SetColors()
{
    float afColor[3];
    DtLightGetColor( m_iLightID, &afColor[0], &afColor[1], &afColor[2]); 
    NiColor color(afColor[0], afColor[1], afColor[2]);

    // Check for Ambient Lights
    if (NiIsKindOf(NiAmbientLight, m_spLight))
    {
        m_spLight->SetDiffuseColor(NiColor::BLACK);
        m_spLight->SetSpecularColor(NiColor::BLACK);
        m_spLight->SetAmbientColor(color);
        return;
    }

    MStatus kStatus;
    MObject MLightShape;

    if (DtExt_LightGetShapeNode(m_iLightID, MLightShape) == 0)
        return;

    MFnNonAmbientLight kLight(MLightShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // Check if this light will illuminate the Diffuse channel
    if (kLight.lightDiffuse(&kStatus))
    {
        m_spLight->SetDiffuseColor(color);
    }
    else
    {
        m_spLight->SetDiffuseColor(NiColor::BLACK);
    }
    NIASSERT(kStatus == MS::kSuccess);

    // Check if this light will illuminate the Specular channel
    if (kLight.lightSpecular(&kStatus))
    {
        m_spLight->SetSpecularColor(color);
    }
    else
    {
        m_spLight->SetSpecularColor(NiColor::BLACK);
    }
    NIASSERT(kStatus == MS::kSuccess);

    // Check if this light will illuminate the Ambient channel
    if (kLight.lightAmbient(&kStatus))
    {
        m_spLight->SetAmbientColor(color);
    }
    else
    {
        m_spLight->SetAmbientColor(NiColor::BLACK);
    }
    NIASSERT(kStatus == MS::kSuccess);

    // Small Ambient Light.
    if (gExport.m_bAddDefaultAmbientForMayaLights)
    {
        m_spLight->SetAmbientColor(NiColor(0.125f, 0.125f, 0.125f));
    }
}
//---------------------------------------------------------------------------
void MyiLight::CheckForAnimatedColor()
{
    MObject MLightShape;

    // Get the Shape Node
    if (DtExt_LightGetShapeNode( m_iLightID, MLightShape ) == 0)
        return;

    MStatus kStatus;

    // get place2dTexture object for this texture
    MFnDependencyNode kShape(MLightShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR = kShape.findPlug("colorR", &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MPlug kPlugColorG = kShape.findPlug("colorG", &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MPlug kPlugColorB = kShape.findPlug("colorB", &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() )
    {
        return;
    }

    unsigned int uiNumKeys;
    NiPosKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if ( ConvertPosAnim(kPlugColorR, kPlugColorG, kPlugColorB, uiNumKeys, 
        pkKeys, eType) == MS::kFailure)
    {
        return;
    }

    // Create the Data
    NiPosData* pkData = NiNew NiPosData();
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    // Create the interpolator
    NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator(pkData);
    pkInterp->Collapse();

    // Create the Controller
    NiLightColorControllerPtr spController = NiNew NiLightColorController;
    spController->SetInterpolator(pkInterp);
    spController->ResetTimeExtrema();

    spController->SetAmbient(NiIsKindOf(NiAmbientLight, m_spLight));
    spController->SetTarget(m_spLight);

    NiTimeController::CycleType kCycleTypeR = GetAnimationPostCycleType(
        kPlugColorR, kStatus);

    NiTimeController::CycleType kCycleTypeB = GetAnimationPostCycleType(
        kPlugColorB, kStatus);

    NiTimeController::CycleType kCycleTypeG = GetAnimationPostCycleType(
        kPlugColorG, kStatus);

    if(kCycleTypeR != kCycleTypeB || kCycleTypeR != kCycleTypeG)
    {
        MString kName = kShape.name();

        //they need to be the same.
        MString kWarningString = "Different cycle types for color channels of"
            "light node " + kName + ". Defaulting to the red channel cycle "
            "type.";

        MGlobal::displayWarning(kWarningString);
        
    }

    spController->SetCycleType(kCycleTypeR);
}
//---------------------------------------------------------------------------
