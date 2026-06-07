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

// MyImmerse.cpp
// =============
//      Contains the MyImmerse API including direct Exporting facilities

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "maya/MFnNonExtendedLight.h"
#include <NiToolDataStream.h>
#include <NiMeshProfileProcessor.h>
#include <NiSkinningUtilities.h>
#include <cstdlib>
#include "CreateSortAdjustNode.h"

//--------------------------------------------------------------------------

NiXMLLogger* MyImmerse::ms_pkLogger = NULL;

//---------------------------------------------------------------------------
MyImmerse::MyImmerse()
{
    m_spScene = NULL;
    m_sppLights = NULL;

    NIASSERT(m_spScene == NULL);
    m_spScene = NiNew NiNode;

    m_uiLights = 0;
    m_bExportingCharacter = false;

    // Recreate the Image Converters. This is necessary to circumvent a 
    // bug in DX9 viewer
    // Set the Image Converter to use the Quantizer
    NiImageConverter::SetImageConverter(NiDevImageQuantizer::Create());
    
    NiDevImageQuantizer* pkDevImageQuantizer = 
        (NiDevImageQuantizer*)NiImageConverter::GetImageConverter();

    // Add the HDR readers
    pkDevImageQuantizer->AddReader( NiNew NiOpenEXRReader );
    pkDevImageQuantizer->AddReader( NiNew NiHDRReader );
    pkDevImageQuantizer->AddReader( NiNew NiPNGReader );


    // Create Texture Manager.
    MyiTextureManager::Create();
    if (!MyiTextureManager::GetTextureManager()->Succeeded())
    {
        printf("MakeComponents FAILED Creating TextureManager\n");
        if (MyiTextureManager::GetTextureManager()->Error())
        {
            SetStatus(MYIOBJ_ERROR);
        }
        else if (MyiTextureManager::GetTextureManager()->Cancelled())
        {
            SetStatus(MYIOBJ_CANCEL);
        }
        return;
    }

        // Create the Texture Effect Manager
    m_pkTextureEffectManager = NiNew MyiTextureEffectManager();
    if (m_pkTextureEffectManager->Fail())
    {
        printf("MakeComponents FAILED Creating Texture Effect\n");
        SetStatus(MYIOBJ_ERROR);
        return;
    }

    if (!MakeLights())
    {
        // Status set by MakeLights if it returned false.
        return;
    }
    
    SetStatus(MYIOBJ_SUCCESS);

    CreateDefaultViewerScripts();
}
//---------------------------------------------------------------------------
MyImmerse::~MyImmerse( void )
{

    if (m_spScene)
        m_spScene = 0;


    if (m_sppLights)    
        NiDelete [] m_sppLights;
    m_sppLights = NULL;

    // Empty the selected list Selected list just 
    // points back to the scene graph so don't delete the Items
    m_pkSelectedObjects.RemoveAll();

    // Delete all of the Particle systems and remove them from the list
    while (!m_pkParticleSystems.IsEmpty())
    {
        NiDelete m_pkParticleSystems.RemoveHead();
    }

    // Delete Any Texture Effects
    if (m_pkTextureEffectManager)
        NiDelete m_pkTextureEffectManager;
    m_pkTextureEffectManager = NULL;

    // Shut Down the Texture Manager
    MyiTextureManager::Destroy();

    // Reset the User Data
    gUserData.Reset();
}
//---------------------------------------------------------------------------
bool MyImmerse::MakeLights( void )
{
    int i, iLights;
    NiLight* pkLight;

    DtLightGetCount( &iLights );

    if (iLights == 0)
        return true; // no lights need to be created

    // m_uiLights = iLights + 1; // add one for the ambient light
    m_uiLights = (unsigned short)iLights;  // Temporarily remove space for ambient light

    // Allocate holding area for Lights
    m_sppLights = NiNew NiLightPtr[m_uiLights];
    if (m_sppLights == NULL)
    {
        SetStatus(MYIOBJ_ERROR);
        return false;
    }
    
    for (i = 0; i < iLights; i++)
    {
        MyiLight kMyiLight( i );
        
        if (!kMyiLight.Succeeded())
        {
            if (kMyiLight.Error())
            {
                SetStatus(MYIOBJ_ERROR);
            }
            else if (kMyiLight.Cancelled())
            {
                SetStatus(MYIOBJ_CANCEL);
            }
            return false;
        }

        pkLight = kMyiLight.GetLight();

        m_sppLights[i] = pkLight;

        MObject lightShape;
        DtExt_LightGetShapeNode(i, lightShape);
        m_kMayaLightObjects.append(lightShape);

        MFnDependencyNode kDepFn(lightShape);

        int iTechnique;
        if(GetExtraAttribute(kDepFn, "GbShadowTechnique", true, 
            iTechnique))
        {
            NiIntegerExtraData* pShadowTechnique = NiNew 
                NiIntegerExtraData(iTechnique);

            pShadowTechnique->SetName("NiShadowTechnique");

            pkLight->AddExtraData(pShadowTechnique);
        }

        bool bStatic;
        if(GetExtraAttribute(kDepFn, "GbStaticShadows", true, bStatic))
        {
            NiBooleanExtraData* pStaticShadows = NiNew NiBooleanExtraData(
                bStatic);

            pStaticShadows->SetName("NiShadowStatic");

            pkLight->AddExtraData(pStaticShadows);
        }

        bool bUseAutomaticBias = true;
        if(GetExtraAttribute(kDepFn, "GbUseAutomaticBias", true, 
            bUseAutomaticBias))
        { 
            if(!bUseAutomaticBias)
            {
                
                if(lightShape.hasFn(MFn::kNonExtendedLight))
                {
                    MFnNonExtendedLight lightFn(lightShape);
                    MStatus kStatus = MStatus::kSuccess;

                    double fBias = lightFn.depthMapBias(&kStatus);
                    if(kStatus == MStatus::kSuccess)
                    {
                        NiFloatExtraData* pCustomBias = 
                            NiNew NiFloatExtraData((float)fBias);

                        pCustomBias->SetName("NiCustomBias");

                        pkLight->AddExtraData(pCustomBias);
                    }
                    else
                    {
                        NIASSERT(0);
                        //Should write to some sort of error log
                    }

                }

            }

        }

        bool bUseBackFaces = true;
        if(GetExtraAttribute(kDepFn, "GbUseBackFaces", true, 
            bUseBackFaces))
        {
            NiBooleanExtraData* pUseBackFaces = 
                NiNew NiBooleanExtraData(bUseBackFaces);

            pUseBackFaces->SetName("GbUseBackFaces");

            pkLight->AddExtraData(pUseBackFaces);
        }

        bool bForceMapSize = false;
        if(GetExtraAttribute(kDepFn, "GbForceMapSize", true, 
            bForceMapSize))
        {
            NiBooleanExtraData* pForceMapSize = 
                NiNew NiBooleanExtraData(bForceMapSize);

            pForceMapSize->SetName("GbForceMapSize");

            pkLight->AddExtraData(pForceMapSize);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::AttachLight( int iComponent )
{
    // Lights have already been created - we just need
    // to attach them...

    // Get light index from component.
    int iLight = gMDtObjectGetTypeIndex(iComponent);

    NiNode* pkNode = gUserData.GetNode(iComponent);
    if (pkNode == NULL)
    {
        NIASSERT(pkNode);
        printf("***WARNING*** Found Light with no Data\n");
        return true;
    }

    // Could be A Texturing Effect or a light
    NiTextureEffect* pkTextureEffect = 
        m_pkTextureEffectManager->GetTextureEffect(iLight);

    if (pkTextureEffect != NULL)
    {
        // Attach the Texture Effect
        pkNode->AttachChild(pkTextureEffect);
    }
    else
    {
        // Attach The Light
        pkNode->AttachChild(m_sppLights[iLight]);
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::AttachParticleSystemEmitter( int iComponent, 
        MyiMaterialManager* pkMaterialManager, 
        MyiTextureManager* pkTextureManager )
{
    NI_UNUSED_ARG(pkTextureManager);
    NI_UNUSED_ARG(pkMaterialManager);
    // Lights have already been created - we just need
    // to attach them...

    // Get Particle System Index from component.
    int iEmitterID = gMDtObjectGetTypeIndex(iComponent);
    NI_UNUSED_ARG(iEmitterID);
    
    NiNode* pkNode = gUserData.GetNode(iComponent);
    if (pkNode == NULL)
    {
        NIASSERT(pkNode);
        printf("***WARNING*** Emitter with no Data\n");
        return true;
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::AttachParticleSystemParticles( int iComponent,
    MyiMaterialManager* pkMaterialManager, 
    MyiTextureManager *pkTextureManager )
{
    // Lights have already been created - we just need
    // to attach them...

    // Get Particle System Index from component.
    int iParticles = gMDtObjectGetTypeIndex(iComponent);

    NiNode* pkNode = gUserData.GetNode(iComponent);
    if (pkNode == NULL)
    {
        NIASSERT(pkNode);
        printf("***WARNING*** Found Particle System with no Data\n");
        return true;
    }

    MyiParticleSystem* pkNewPSystem = NiNew MyiParticleSystem(iParticles,
        pkMaterialManager, pkTextureManager);

    pkNode->AttachChild((NiAVObject*)pkNewPSystem->GetParticleSystem());

    m_pkParticleSystems.AddTail(pkNewPSystem);

    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::MakeNodes(void)
{
    // PSEUDO-CODE

    // iChild = 0;
    // For i = 0 to number of Components
    // {
    //      level = models level
    //      if (level == 0)
    //      {
    //          pChild = (Construct NiNode in some fashion)
    //          m_spScene->SetAt( iChild++, pChild );
    //      }
    // }

    // Get the count of transform objects - shapes, lights, cameras, 
    // joints, etc.
    int iComponents = gMDtObjectGetUsed();

    if(DtExt_HaveGui())
    {
        // Update the progress window
        MProgressWindow::setProgressStatus("Creating Gamebryo Nodes");
        MProgressWindow::setProgressRange(0, iComponents);
        MProgressWindow::setProgress(0);
    }

    // Initialize our UserData area to the number of models.
    gUserData.InitSize( iComponents );  

    SetStatus(MYIOBJ_SUCCESS);

    int iComponentIndex;
    for (iComponentIndex=0; iComponentIndex < iComponents; iComponentIndex++ )
    {

        // We want to process only Parent nodes or level 0 nodes
        if ( gMDtObjectGetParentID( iComponentIndex ) >= 0)
            continue;

        // This is an empty array of strings used by the recursive call.
        MStringArray kParentAffectedLights;

        // Note:  Constructor of MyiNode will create an NiNode
        // with children nodes attached.  So we just simply attach
        // it to the scene graph with the SetAt method.
        MyiNode kMyiNode(iComponentIndex, m_pkSelectedObjects, m_sppLights, 
            m_pkTextureEffectManager, kParentAffectedLights);
        
        if (kMyiNode.Error())
        {
            SetStatus(MYIOBJ_ERROR);
            continue; // Try to work through an error on one node
        }
        else if (kMyiNode.Cancelled())
        {
            SetStatus(MYIOBJ_CANCEL);
            break; // Stop processing
        }

        NiNode* pkNode = kMyiNode.GetNode();
        if (pkNode)
            m_spScene->AttachChild(pkNode);
    }

    //Now that everything is setup we can add the shadow generators

    ConvertShadows();

    return true;
}
//---------------------------------------------------------------------------
void MyImmerse::ConvertShadows()
{
    int iLights = 0;
    DtLightGetCount(&iLights);

    for (int iLightIndex = 0; iLightIndex < iLights; iLightIndex++)
    {
        MObject kMObject;
        kMObject = m_kMayaLightObjects[iLightIndex];

        // Currently, only point, spot, and directional lights are supported
        // Any other types will be ignored for shadow conversion
        if(kMObject.hasFn(MFn::kNonExtendedLight))
        {
            MStatus status = MStatus::kSuccess;

            MFnNonExtendedLight kMFnNonExtendedLight(kMObject, &status);
            NIASSERT(status == MStatus::kSuccess);

            bool bCastsShadows = kMFnNonExtendedLight.useDepthMapShadows(
                &status);
            NIASSERT(status == MStatus::kSuccess);

            if(bCastsShadows)
            {
                NiLightPtr pkLight = m_sppLights[iLightIndex];

                NiShadowGenerator* pkShadowGenerator = NiNew NiShadowGenerator
                    (pkLight);

                //get the size of the shadow map
                short shadowMapSize = kMFnNonExtendedLight.depthMapResolution(
                    &status);

                //validate size is a power of two

                //if a number is a power of two then it all its bits are zero
                //except one. Therefore if we subtract one from the number we 
                //would get a number with 1 bits everywhere, except where 
                //there was a 1 before. Bitwise "anding" the two numbers 
                //together would give us zero. Inverting the zero gives use 
                //"true" for a power of two and "false" for any other number. 
                //We logically "and" with the number to make sure that it is 
                //not zero

                bool bIsPowerOfTwo = 
                    !(shadowMapSize & (shadowMapSize - 1)) && shadowMapSize;

                if(bIsPowerOfTwo)
                {
                    pkShadowGenerator->SetSizeHint(shadowMapSize);  
                }

                //find technique Attribute
                NiShadowTechnique* pkTechnique;

                NiIntegerExtraData* pkTechniqueData = NiDynamicCast(
                    NiIntegerExtraData, pkLight->GetExtraData(
                    "NiShadowTechnique"));

                if(pkTechniqueData)
                {
                   pkTechnique = NiShadowManager::GetKnownShadowTechnique(
                       (unsigned short)pkTechniqueData->GetValue());
                }
                else
                {
                    // try to use the first technique instead
                    pkTechnique = NiShadowManager::GetKnownShadowTechnique(0);
                }

                if(pkTechnique == NULL)
                {
                    //shadow technique could not be created
                    //we will not make a generator
                    NiDelete pkShadowGenerator;
                    break;
                }

                pkShadowGenerator->SetShadowTechnique(pkTechnique);
              
                NiBooleanExtraData* pkStaticData = NiDynamicCast(
                    NiBooleanExtraData, pkLight->GetExtraData(
                    "NiShadowStatic"));

                if(pkStaticData)
                {
                    pkShadowGenerator->SetStatic(pkStaticData->GetValue());
                }
                else
                {
                    pkShadowGenerator->SetStatic(false);
                }

                NiFloatExtraData* pkCustomBias = NiDynamicCast(
                    NiFloatExtraData, pkLight->GetExtraData(
                    "NiCustomBias"));

                NiBooleanExtraData* pkUseBackFaces = NiDynamicCast(
                    NiBooleanExtraData, pkLight->GetExtraData(
                    "GbUseBackFaces"));

                bool bUseBackFaces = true;
                if(pkUseBackFaces)
                {
                    bUseBackFaces = pkUseBackFaces ->GetValue();
                }

                pkShadowGenerator->SetRenderBackfaces(bUseBackFaces);

                if(pkCustomBias)
                {
                    pkShadowGenerator->SetDepthBias(
                        pkCustomBias ->GetValue());
                }
                else
                {
                    int iLightType;
                    DtLightGetType( iLightIndex, &iLightType);

                    NiStandardMaterial::LightType eLightType = 
                        NiStandardMaterial::LIGHT_DIR;

                    switch (iLightType)
                    {
                    case DT_POINT_LIGHT:
                        eLightType = NiStandardMaterial::LIGHT_POINT;
                        break;

                    case DT_DIRECTIONAL_LIGHT:
                        eLightType = NiStandardMaterial::LIGHT_DIR;
                        break;

                    case DT_SPOT_LIGHT:
                        eLightType = NiStandardMaterial::LIGHT_SPOT;
                        break;

                    case DT_AMBIENT_LIGHT:
                        break;
                    }

                    float fBias = pkTechnique->GetDefaultDepthBias(
                        eLightType, bUseBackFaces);
                    
                    pkShadowGenerator->SetDepthBias(fBias);
                }

                NiBooleanExtraData* pkForceMapSize = NiDynamicCast(
                    NiBooleanExtraData, pkLight->GetExtraData(
                    "GbForceMapSize"));

                if(pkForceMapSize)
                {
                    pkShadowGenerator->SetStrictlyObserveSizeHint(
                        pkForceMapSize ->GetValue());
                }

                //recursively find shadow flags
                // find the affected nodes that aren't and or receivers casters
                const NiNodeList* pkAffectedList;
                pkAffectedList = &(pkLight->GetAffectedNodeList());
                NiTListIterator kPos;
                kPos = pkAffectedList->GetHeadPos();
                while (kPos != NULL)
                {
                    NiNode* pkAffectedNode;
                    pkAffectedNode = pkAffectedList->GetNext(kPos);
                    // traverse the sub-scene and add exclusions to the 
                    //generator
                    RecursiveFindShadowFlags(pkAffectedNode, 
                        pkShadowGenerator);
                }

                NiShadowManager::AddShadowGenerator(pkShadowGenerator);
            }
        }
    }

    //remove the extra data
    CleanExtraData(m_spScene);
}
//---------------------------------------------------------------------------
void MyImmerse::RecursiveFindShadowFlags(NiNode* pkNode, NiShadowGenerator* 
    pkShadowGenerator)
{
    // this function searches the scene graph for flags indicating that an 

     // object doesn't cast or receive shadows. When it finds them, it adds
     // them to the appropriate exclusion list of the shadow generator
 
     // non-casters own a boolean extra data 'NonShadowCaster'
     NiBooleanExtraData* pkNonCaster;
     pkNonCaster = NiDynamicCast(NiBooleanExtraData, pkNode->GetExtraData(
         "NonShadowCaster"));
     if (pkNonCaster != NULL)
     {
         pkShadowGenerator->AttachUnaffectedCasterNode(pkNode);
     }
     // non-receivers own a boolean extra data 'NonShadowReceiver'
     NiBooleanExtraData* pkNonReceiver;
     pkNonReceiver = NiDynamicCast(NiBooleanExtraData, pkNode->GetExtraData(
         "NonShadowReceiver"));
     if (pkNonReceiver != NULL)
     { 
         pkShadowGenerator->AttachUnaffectedReceiverNode(pkNode);
     }
 
     // recurse over children
     unsigned int ui;
     for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
     {
         NiAVObject* pkChild = pkNode->GetAt(ui);
         if (NiIsKindOf(NiNode, pkChild))
         {
             RecursiveFindShadowFlags((NiNode*)pkChild, pkShadowGenerator);
         }
     }    
}
//---------------------------------------------------------------------------
void MyImmerse::CleanExtraData(NiObjectNET* pkObject)
{
    // this recursive function removes all of the extra data flags from the 
    // scene that were added by the plug-in to indicate that an object doesn't 
    // cast or receive shadows.

    // non-casters own a boolean extra data 'NonShadowCaster'
    NiBooleanExtraData* pkNonCaster;
    pkNonCaster = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        "NonShadowCaster"));
    if (pkNonCaster != NULL)
    {
        pkObject->RemoveExtraData("NonShadowCaster");
    }
    // non-receivers own a boolean extra data 'NonShadowReceiver'
    NiBooleanExtraData* pkNonReceiver;
    pkNonReceiver = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        "NonShadowReceiver"));
    if (pkNonReceiver != NULL)
    {
        pkObject->RemoveExtraData("NonShadowReceiver");
    }

    // recurse over children
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            CleanExtraData(pkChild);
        }
    }
}

//---------------------------------------------------------------------------
bool MyImmerse::MakeComponents(void)
{
    // Create Material Manager
    MyiMaterialManager* pkMtlManager = NiNew MyiMaterialManager();
    if (!pkMtlManager->Succeeded())
    {
        printf("MakeComponents FAILED Creating MaterialManager\n");
        SetStatus(MYIOBJ_ERROR);
        return false;
    }


    int iComponents = gMDtObjectGetUsed();
    bool bResult = false;

    if(DtExt_HaveGui())
    {
        // Update the progress window
        MProgressWindow::setProgressStatus("Creating Gamebryo Components");
        MProgressWindow::setProgressRange(0, iComponents);
        MProgressWindow::setProgress(0);
    }

    int i;
    for (i=0; i < iComponents; i++ )
    {
        if(DtExt_HaveGui())
        {
            if (MProgressWindow::isCancelled())
            {
                SetStatus(MYIOBJ_CANCEL);
                bResult = false;
                break;
            }

            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        EType eType = gMDtObjectGetType(i);
        
        switch(eType)
        {
            case ETypeShape:
                bResult = MakeMesh(i, pkMtlManager,
                    MyiTextureManager::GetTextureManager());
                break;
            case ETypeLight:
                bResult = AttachLight( i );
                break;
            case ETypeEmitter:
                bResult = AttachParticleSystemEmitter( i, pkMtlManager, 
                    MyiTextureManager::GetTextureManager() );
                break;
            case ETypeParticles:
                bResult = AttachParticleSystemParticles( i, pkMtlManager, 
                    MyiTextureManager::GetTextureManager() );
                break;
            case ETypePhysXShape:
                MyiPhysX::ProcessPhysXShape(i);
                break;
            case ETypePhysXJoint:
                MyiPhysX::ProcessPhysXJoint(i);
                break;
            case ETypePhysXClothConstr:
                // ETypePhysXClothConstr does not require any processing;
            case ETypeJoint:
            case ETypeCamera:
            case ETypeUnknown:
            default:
                // for now...
                // Normally, we would attach cameras, joints, etc. as well.
                bResult = true;
        }

        if (bResult == false)
        {
            SetStatus(MYIOBJ_ERROR);    
            break;
        }
    }


    // Post Process the new particle systems
    NiTListIterator kIter = m_pkParticleSystems.GetHeadPos();
    while (kIter != NULL)
    {
        m_pkParticleSystems.GetNext(kIter)->PostProcess(m_pkParticleSystems);
    }


    if (pkMtlManager)
        NiDelete pkMtlManager;
    pkMtlManager = NULL;

    if (bResult == true)
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool MyImmerse::MakeMesh(int iComponentNum,
    MyiMaterialManager* pkMtlManager, MyiTextureManager* pkTexManager)
{
    NiNode* pkNode = NULL;
    NiMesh* pkMesh;

    // Find Shape Number
    int iShapeNum = gMDtObjectGetTypeIndex(iComponentNum);
    int i = iShapeNum;

    // abnormal mesh test... continue if have children
    int iNumChildren = DtShapeGetChildCount(i);

    int iGroupCnt = DtGroupGetCount( i );
    if (iNumChildren > 0 && iGroupCnt < 1)
        return true;

    int iGroupID;
    for (iGroupID = 0; iGroupID < iGroupCnt; iGroupID++)
    {
        pkNode = NULL;

        // Grab Mesh through the MDtAPI interface
        // Note, we have to ensure mesh is in tris later.
        // For now, assume that mesh is already in tris.
        MyiTriMesh kMyiTriMesh(i, iGroupID, pkMtlManager, pkTexManager);

        if (!kMyiTriMesh.Succeeded())
        {
            if (kMyiTriMesh.Error())
            {
                SetStatus(MYIOBJ_ERROR);
            }
            else if (kMyiTriMesh.Cancelled())
            {
                SetStatus(MYIOBJ_CANCEL);
                return false;
            }
            return true;
            //return false; 
            // Consider if we shouldn't abort entire process...?
        }

        pkMesh = kMyiTriMesh.GetMesh();
        NIASSERT(pkNode == NULL);

        pkNode = gUserData.GetNode(iComponentNum);

        if (pkNode == NULL)
        {
            NIASSERT(pkNode);
            printf("***WARNING*** Found Shape with no Data\n");
            return true;
        }

        if (pkMesh == NULL)
        {
            printf("***WARNING*** Found NULL Shape\n");
            return true;
        }

        pkNode->AttachChild(pkMesh);
    }

    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::Export( void )
{
    // Don't do anything if the constructor did not succeed.
    if (!Succeeded())
        return false;
        
    printf("Starting MyImmerse Export::\n");

    // Initialize Exporting.
    // Export by making scene.
    bool bResult = MakeScene();

    gUserData.Reset();
    return bResult;
}
//---------------------------------------------------------------------------
bool MyImmerse::MakeScene( void )
{
    if (m_spScene == NULL)
        return false;

    if (!AttachDefaultSceneProperties())
        return false;

    // Initialize the physics extraction.    
    MyiPhysX::Initialize();

    if (!MakeNodes())
        return false;

    if (!MakeComponents())
        return false; // Status has been set

    RemoveExportOptionsNode();

    // Reorder the bones to make sure the correct update order.
    NiSkinningUtilities::ReorderBonesForSkinnedMeshes(m_spScene);

    if (gExport.m_bAutoCenterLODs)
        CenterAllLODOnBounds(m_spScene);

    if (gExport.m_bLightingOptimizeSceneRootLights)
        CheckForLightsAtSceneRoot();

    m_spScene->SetName("SceneNode");
    HWND hWndActiveWindow = GetActiveWindow();
    NI_UNUSED_ARG(hWndActiveWindow);


    // Generate Unique Names for every object in the scene
    if (gExport.m_bGuaranteeObjectNamesAreUnique)
    {
        GenerateUniqueNames(m_spScene);
        VerifyUniqueNames(m_spScene);
    }

    // Recurse the scene graph setting the animation looping controls
    SetAnimationLoopingControls(m_spScene, NiTimeController::MAX_CYCLE_TYPES);

    ShareAllDuplicateAnimationData();

    // Set the Mesh Attributes for the Scene; keep none
    SetMeshAttributes(m_spScene, 0);

    // Recurse the scene graph creating the Bone LOD Tags
    CreateBoneLODTags(m_spScene);

    NiOptimize::CleanUpAnimationKeys(m_spScene);

    // Clean up the physics stuff
    MyiPhysX::Shutdown();

    // Create an NiStringExtraData containning the original file name
    // and the version of MyImmerse used to crete this object.
    CreateSceneNameAndVersionTag();

    // Set the Selective Update Flags for the scene
    bool bSelectiveUpdate = false;
    bool bRigid = false;
    m_spScene->SetSelectiveUpdateFlags(bSelectiveUpdate, false, bRigid);

    m_spScene->Update(0.0);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();

    CreateSortAdjustNode kCreateSortAdjustNode;

    kCreateSortAdjustNode.ProcessScene(m_spScene);

    bool bResult =  ExecuteScript();

    if (bResult == false)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::AttachDefaultSceneProperties( void )
{
    // Set up a ZBuffer Property
    NiZBufferPropertyPtr spZBufferProperty;
    spZBufferProperty = NiNew NiZBufferProperty;
    if (spZBufferProperty == NULL)
        return false;
    spZBufferProperty->SetZBufferTest(TRUE);
    spZBufferProperty->SetZBufferWrite(TRUE);
    m_spScene->AttachProperty(spZBufferProperty);


    // APPLY THE SHADE PROPERTY
    NiShadePropertyPtr spShadeProperty = NiNew NiShadeProperty;
    if (spShadeProperty == NULL) 
        return false;

    m_spScene->AttachProperty(spShadeProperty);


    // Set up Vertex Color Property
    NiVertexColorPropertyPtr spVertexColorProperty;
    spVertexColorProperty = NiNew NiVertexColorProperty;
    if (spVertexColorProperty == NULL)
        return false;
    spVertexColorProperty->SetSourceMode(
        NiVertexColorProperty::SOURCE_EMISSIVE);
    spVertexColorProperty->SetLightingMode(
        NiVertexColorProperty::LIGHTING_E_A_D); 
    
    m_spScene->AttachProperty(spVertexColorProperty);

    // Set up a default Material Property
    NiMaterialPropertyPtr spMaterialProperty;
    spMaterialProperty = NiNew NiMaterialProperty;
    if (spMaterialProperty == NULL)
        return false;
    spMaterialProperty->SetAmbientColor(NiColor(0.5f, 0.5f, 0.5f));
    spMaterialProperty->SetDiffuseColor(NiColor(0.7f, 0.7f, 0.7f));
    spMaterialProperty->SetEmittance(NiColor(0.0f, 0.0f, 0.0f));
    spMaterialProperty->SetSpecularColor(NiColor(1.0f, 1.0f, 1.0f));
    m_spScene->AttachProperty(spMaterialProperty);

    return true;
}
//---------------------------------------------------------------------------
NiTextKeyExtraDataPtr MyImmerse::MergeTextKeyExtraData(NiNode* pkNode, 
    const char* pcName1, const char* pcName2)
{
    // Get the Text Keys
    NiExtraData* pkMorphExtraData = pkNode->GetExtraData(pcName1);

    // Get the Text Keys
    NiExtraData* pkTextExtraData = pkNode->GetExtraData(pcName2);

    // Check for No Keys
    if ((pkMorphExtraData == NULL) && (pkTextExtraData == NULL))
    {
        return NULL;
    }
    
    // Check for Only Morph Tags
    if (pkTextExtraData == NULL)
    {
        return NiDynamicCast(NiTextKeyExtraData, pkMorphExtraData);            
    }

    // Check for Only Text Tags
    if (pkMorphExtraData == NULL)
    {
        return NiDynamicCast(NiTextKeyExtraData, pkTextExtraData);
    }

    NiTextKeyExtraDataPtr spMorphTags = NiDynamicCast(NiTextKeyExtraData, 
        pkMorphExtraData);            

    unsigned int uiNumMorphKeys;
    NiTextKey* pkMorphKeys = spMorphTags->GetKeys(uiNumMorphKeys);

    NiTextKeyExtraDataPtr spTextKeys = NiDynamicCast(NiTextKeyExtraData, 
        pkTextExtraData);            

    unsigned int uiNumTextKeys;
    NiTextKey* pkTextKeys = spTextKeys->GetKeys(uiNumTextKeys);

    NiTextKey* pkSequenceKeys = NiNew NiTextKey[uiNumMorphKeys + 
        uiNumTextKeys];

    unsigned int uiMLoop = 0;
    unsigned int uiTLoop = 0;
    unsigned int uiSLoop = 0;

    while ((uiMLoop < uiNumMorphKeys) || (uiTLoop < uiNumTextKeys))
    {
        if ((uiMLoop < uiNumMorphKeys) &&
           ((uiTLoop >= uiNumTextKeys) || 
            (pkMorphKeys[uiMLoop].GetTime() < pkTextKeys[uiTLoop].GetTime())))
        {
            pkSequenceKeys[uiSLoop].SetText( pkMorphKeys[uiMLoop].GetText() );
            pkSequenceKeys[uiSLoop++].SetTime( 
                pkMorphKeys[uiMLoop++].GetTime() );
        }
        else
        {
            pkSequenceKeys[uiSLoop].SetText( pkTextKeys[uiTLoop].GetText() );
            pkSequenceKeys[uiSLoop++].SetTime( 
                pkTextKeys[uiTLoop++].GetTime() );
        }

    }

    // Create the Merged Extra Data
    NiTextKeyExtraDataPtr spSequenceKeys = NiNew NiTextKeyExtraData;
    spSequenceKeys->SetKeys(pkSequenceKeys, uiSLoop);

    return spSequenceKeys;
}
//---------------------------------------------------------------------------
void MyImmerse::CreateControllerExtractorSequences(MFnDagNode& dgNode,
    NiNode* pkNode, NiControllerExtractor::InfoArray& kInfoArray, 
    NiNode* pkAccumulationNode, NiNode* pkCharacterRoot)
{
    NIASSERT(pkCharacterRoot);
    NIASSERT(pkNode);

#ifdef NIDEBUG
    const char* name = dgNode.name().asChar();
    NI_UNUSED_ARG(name);
#endif

    MStatus stat;
    MString sFullPathName = dgNode.fullPathName(&stat);

    NIASSERT(stat == MS::kSuccess);

    // Collect the Number of sequences
    MString sCommand = "getAttr -s " + sFullPathName + ".Ni_Sequence";

    int iTotalSequences;
    stat = MGlobal::executeCommand(sCommand, iTotalSequences);
    NIASSERT(stat == MS::kSuccess);

    int iNumSequences = 0;

    int iValue;
    double dValue;

    int iLoop;
    for (iLoop = 0; iLoop < iTotalSequences; iLoop++)
    {
        MString sValue;

        sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + iLoop + 
            "].Ni_Sequence_Valid"; 

        stat = MGlobal::executeCommand(sCommand, iValue);
        NIASSERT(stat == MS::kSuccess);

        // Increment the Number of Sequences if we found a valid one
        if (iValue == 1)
            iNumSequences++;
    }

    if (iNumSequences == 0)
        return;


    // Remember we are exporting a character
    m_bExportingCharacter = true;

    // Guarantee the character has unique names
    VerifyUniqueNames(pkNode);

    // Merge the Morph Keys and the Text Keys for the Sequence

    // Get the Text Keys
    NiTextKeyExtraDataPtr spSequenceKeys = MergeTextKeyExtraData(pkNode, 
        "Morph Tags", "Text Keys");



    for (iLoop = 0; iLoop < iTotalSequences; iLoop++)
    {
        sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + iLoop + 
            "].Ni_Sequence_Valid"; 

        stat = MGlobal::executeCommand(sCommand, iValue);
        NIASSERT(stat == MS::kSuccess);

        // Increment the Number of Sequences if we found a valid one
        if (iValue == 1)
        {
            NiControllerExtractor::SequenceInfo* pkSequence = 
                NiNew NiControllerExtractor::SequenceInfo;

            // Setup the Character and Layer                
            pkSequence->m_spCharacterRoot = pkCharacterRoot;
            pkSequence->m_spAccumRoot = pkAccumulationNode;

            // The NiControllerExtractor ToolLib expects the Layer Root to
            // be NULL if the sequences are associated with either the
            // Character Root or the Accumulation Root. Otherwise, a 
            // NonAccumulationRoot will not be created.
            if (pkNode == pkCharacterRoot || pkNode == pkAccumulationNode)
            {
                pkSequence->m_spLayerRoot = NULL;
            }
            else
            {
                pkSequence->m_spLayerRoot = pkNode;
                SetOptimizeKeepFlag(pkNode);
            }
            
            // Load the Sequence Name
            MString sNameBuffer;
            char* pcSequenceName;

            sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + iLoop + 
                "].Ni_Sequence_Name"; 

            stat = MGlobal::executeCommand(sCommand, sNameBuffer);
            NIASSERT(stat == MS::kSuccess);

            unsigned int uiLen = sNameBuffer.length() + 1;
            pcSequenceName = NiAlloc(char, uiLen);
            NiStrcpy(pcSequenceName, uiLen, sNameBuffer.asChar());

            pkSequence->m_pcName = pcSequenceName;

            // Load the Sequence StartTime
            sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + iLoop + 
                "].Ni_Sequence_Start_Time"; 

            stat = MGlobal::executeCommand(sCommand, dValue);
            NIASSERT(stat == MS::kSuccess);

            pkSequence->m_fStartTime = (float)dValue;

            // Load the Sequence End Time
            sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + iLoop + 
                "].Ni_Sequence_End_Time"; 

            stat = MGlobal::executeCommand(sCommand, dValue);
            NIASSERT(stat == MS::kSuccess);
        
            pkSequence->m_fEndTime = (float)dValue;

            // Load the Sequence Looping
            sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + iLoop + 
                "].Ni_Sequence_Looping"; 

            stat = MGlobal::executeCommand(sCommand, iValue);
            NIASSERT(stat == MS::kSuccess);
        
            pkSequence->m_bLoop = iValue == 1 ? true : false;

            // Are we accumulating animations?
            if (pkAccumulationNode)
            {
                NiControllerExtractor::AxisFlags AccumulateAxis = 
                    NiControllerExtractor::NO_FLAGS;

                // Load the Sequence Rotation X
                sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + 
                    iLoop + "].Ni_Sequence_Rot"; 

                stat = MGlobal::executeCommand(sCommand, iValue);
                NIASSERT(stat == MS::kSuccess);

                if (iValue == 1)
                    pkSequence->m_eRotFlags = NiControllerExtractor::ALL_FLAGS;

                AccumulateAxis = NiControllerExtractor::NO_FLAGS;

                // Load the Sequence Translation X
                sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + 
                    iLoop + "].Ni_Sequence_Tran_X"; 

                stat = MGlobal::executeCommand(sCommand, iValue);
                NIASSERT(stat == MS::kSuccess);

                if (iValue == 1)
                    AccumulateAxis = (NiControllerExtractor::AxisFlags)
                        (AccumulateAxis | NiControllerExtractor::X_FLAG);

                // Load the Sequence Translation Y
                sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + 
                    iLoop + "].Ni_Sequence_Tran_Y"; 

                stat = MGlobal::executeCommand(sCommand, iValue);
                NIASSERT(stat == MS::kSuccess);

                if (iValue == 1)
                    AccumulateAxis = (NiControllerExtractor::AxisFlags)
                        (AccumulateAxis | NiControllerExtractor::Y_FLAG);

                // Load the Sequence Translation Z
                sCommand = "getAttr " + sFullPathName + ".Ni_Sequence[" + 
                    iLoop + "].Ni_Sequence_Tran_Z"; 

                stat = MGlobal::executeCommand(sCommand, iValue);
                NIASSERT(stat == MS::kSuccess);

                if (iValue == 1)
                    AccumulateAxis = (NiControllerExtractor::AxisFlags)
                        (AccumulateAxis | NiControllerExtractor::Z_FLAG);

                // Set the Rotation Axis
                pkSequence->m_uiTransFlags = AccumulateAxis;
            }


            // Create the Text Keys for this sequence
            pkSequence->m_spTextKeys = CreateSequenceTextKeys(
                *pkSequence, spSequenceKeys);

            // Add this sequence to the actor data
            kInfoArray.Add(pkSequence);
        }

    }

    return;
}
//---------------------------------------------------------------------------
NiTextKeyExtraDataPtr MyImmerse::CreateSequenceTextKeys(
    NiControllerExtractor::SequenceInfo& kSequenceInfo,
    NiTextKeyExtraDataPtr spTextKeys)
{

    unsigned int uiNumKeys = 0;
    int iNumKeysInSequence = 0;
    NiTextKey* pkKeys = NULL;


    if (spTextKeys != NULL)
    {
        pkKeys = spTextKeys->GetKeys(uiNumKeys);

        //The keys need to be in chronological order
        std::qsort(pkKeys, uiNumKeys, sizeof(NiTextKey), 
            MyImmerse::CompareNiTextKey);
    }
    
    // Count the Keys in Sequence Range
    int iLoop;
    for (iLoop = 0; iLoop < (int)uiNumKeys; iLoop++)
    {
        if ((pkKeys[iLoop].GetTime() >= kSequenceInfo.m_fStartTime) &&
            (pkKeys[iLoop].GetTime() <= kSequenceInfo.m_fEndTime))
        {
            iNumKeysInSequence++;
        }
    }
    
    // Copy the Keys in the sequence
    NiTextKey* pkSeqKeys = NiNew NiTextKey[iNumKeysInSequence + 2];
    
    // Create the default Start Key
    pkSeqKeys[0].SetTime(0.0f);
    pkSeqKeys[0].SetText("start");
    
    int iCurrentKey = 1;
    for (iLoop = 0; iLoop < (int)uiNumKeys; iLoop++)
    {
        if ((pkKeys[iLoop].GetTime() >= kSequenceInfo.m_fStartTime) &&
            (pkKeys[iLoop].GetTime() <= kSequenceInfo.m_fEndTime))
        {
            pkSeqKeys[iCurrentKey].SetTime(pkKeys[iLoop].GetTime() - 
                kSequenceInfo.m_fStartTime);

            pkSeqKeys[iCurrentKey++].SetText(pkKeys[iLoop].GetText());
        }
    }
    
    // Create the default Start Key
    pkSeqKeys[iNumKeysInSequence+1].SetTime(kSequenceInfo.m_fEndTime - 
        kSequenceInfo.m_fStartTime);

    pkSeqKeys[iNumKeysInSequence+1].SetText("end");
    
    return NiNew NiTextKeyExtraData(pkSeqKeys, iNumKeysInSequence + 2);
    
}
//---------------------------------------------------------------------------
int MyImmerse::CompareNiTextKey(const void* pArg1,
    const void* pArg2)
{
    NiTextKey* pkFirstArg = (NiTextKey*)pArg1;
    NiTextKey* pkSecondArg = (NiTextKey*)pArg2;

    if(pkFirstArg->GetTime() > pkSecondArg->GetTime())
    {
        return 1;
    }
    else if(pkFirstArg->GetTime() == pkSecondArg->GetTime())
    {
        return 0;
    }
    else if(pkFirstArg->GetTime() < pkSecondArg->GetTime())
    {
        return -1;
    }

    NIASSERT(0);
    return 0;
};

//---------------------------------------------------------------------------
void MyImmerse::BuildNameList(NiTList<NiAVObject*> &kNameList, 
    NiAVObject* pkObject)
{
    kNameList.AddTail(pkObject);

    // Check if we can have children
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        // Add all of the children to the list
        for (unsigned int iLoop = 0; iLoop < pkNode->GetArrayCount(); iLoop++)
        {
            if (pkNode->GetAt(iLoop) != NULL)
                BuildNameList(kNameList, pkNode->GetAt(iLoop));
        }
    }
}
//---------------------------------------------------------------------------
bool MyImmerse::VerifyUniqueNames(NiAVObject* pkNode)
{
    // Build the Node List
    NiTList<NiAVObject *>kNameList;
    BuildNameList(kNameList, pkNode);

    NiTListIterator kCurrent = kNameList.GetHeadPos();
    NiTListIterator kAgainst;
    bool bUniqueNames = true;

    while (kCurrent != NULL)
    {
        // Verify this object has a name
        if (kNameList.Get(kCurrent)->GetName() != NULL)
        {
            kAgainst = kNameList.GetNextPos(kCurrent);
            
            // Compare our name against all prevous in the list
            while (kAgainst != NULL)
            {
                if ((kNameList.Get(kAgainst)->GetName() != NULL) &&
                    ( strcmp(kNameList.Get(kCurrent)->GetName(), 
                        kNameList.Get(kAgainst)->GetName()) == 0 ) )
                {

                    char pcBuffer[512];
                    NiSprintf(pcBuffer, 512,
                        "ERROR:: Controller Sequences require " 
                        "unique names but a duplicate name was found. Name = "
                        "%s\n", kNameList.Get(kCurrent)->GetName() );

                    DtExt_Err(pcBuffer);
                    bUniqueNames = false;
                }
                kAgainst = kNameList.GetNextPos(kAgainst);
            }
        }

        kCurrent = kNameList.GetNextPos(kCurrent);
    }


    kNameList.RemoveAll();
    return bUniqueNames;
}
//---------------------------------------------------------------------------
void MyImmerse::ParseName(const char* pcName, char* pcBase, 
    unsigned int uiBaseLen, int& iIndex)
{

    const char *pcIndex = pcName + strlen(pcName) - 1;

    // Check for an No Index
    if ((*pcIndex < '0') ||
        (*pcIndex > '9') ||
        (pcIndex < pcName))
    {
        NiStrcpy(pcBase, uiBaseLen, pcName);
        iIndex = 0;
        return;
    }


    // Find the beginning of the UniqueIndex
    while ( (pcIndex > pcName) &&
            (*pcIndex >= '0') &&
            (*pcIndex <= '9') )
    {
        pcIndex--;
    }

    pcIndex++;

    // Copy off the base
    NiStrncpy(pcBase, uiBaseLen, pcName, pcIndex - pcName);

#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf_s(pcIndex,"%d", &iIndex);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf(pcIndex,"%d", &iIndex);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
}
//---------------------------------------------------------------------------
bool MyImmerse::CheckUniqueName(const char* pcAgainst, char* pcCurrentBase, 
                                 int iCurrentIndex, int &iMaxIndex)
{
    if (pcAgainst == NULL)
        return false;

    char acAgainstBase[512];
    int iAgainstIndex;

    ParseName( pcAgainst, acAgainstBase, 512, iAgainstIndex);


    // Compare the Base of each is the same length
    // make sure the bases are equal
    if (strcmp(acAgainstBase, pcCurrentBase) == 0)
    {
        if (iAgainstIndex > iMaxIndex) 
        {
            iMaxIndex = iAgainstIndex;
        }   

        if (iAgainstIndex == iCurrentIndex)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MyImmerse::GenerateUniqueNames(NiAVObject* pkObject)
{
    // Build the Node List
    NiTList<NiAVObject *>kNameList;

    BuildNameList(kNameList, pkObject);


    NiTListIterator kCurrent = kNameList.GetHeadPos();
    NiTListIterator kAgainst;

    while (kCurrent != NULL)
    {
        // Verify this object has a name
        if (kNameList.Get(kCurrent)->GetName() != NULL)
        {
            char acCurrentBase[512];
            int iCurrentIndex;
            int iMaxIndex = 0;
            bool bAdjustedIndex = false;

            ParseName(kNameList.Get(kCurrent)->GetName(), acCurrentBase, 512,
                iCurrentIndex);          

            kAgainst = kNameList.GetPrevPos(kCurrent);
            
            // Compare our name against all prevous in the list
            while (kAgainst != NULL)
            {

                // Check to see if we need to adjust our Index
                if (CheckUniqueName(kNameList.Get(kAgainst)->GetName(), 
                    acCurrentBase, iCurrentIndex, iMaxIndex))
                    bAdjustedIndex = true;

                kAgainst = kNameList.GetPrevPos(kAgainst);
            }

            // If we adjusted our indexes lets change the object name
            if (bAdjustedIndex)
            {
                char* szEnd = acCurrentBase + strlen(acCurrentBase);

                // Add the index to the end of the string
                NiSprintf(szEnd, 512 - strlen(acCurrentBase), "%d",
                    iMaxIndex + 1);

                kNameList.Get(kCurrent)->SetName(acCurrentBase);
            }
        }

        kCurrent = kNameList.GetNextPos(kCurrent);
    }


    kNameList.RemoveAll();
    return;
}
//---------------------------------------------------------------------------
void MyImmerse::AddSelectedObject(NiAVObject* pkObject)
{
    m_pkSelectedObjects.AddTail(pkObject);
}
//---------------------------------------------------------------------------
bool MyImmerse::HasAnimations(NiAVObject* pkObject)
{
    
    if (NiGetController(NiTransformController, pkObject) != NULL )
        return true;


    // Check if we can have children
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        // Add all of the children to the list
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            if ((pkNode->GetAt(uiLoop) != NULL) &&
                HasAnimations(pkNode->GetAt(uiLoop)) )
                return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool MyImmerse::GetAnimationTimeRange(NiAVObject* pkObject, float& fStartTime,
    float& fEndTime)
{
    bool bFoundController = false;
    NiTimeController* pkCtrl = pkObject->GetControllers();

    // Iterate throught each controller getting the unscaled Start and End Time
    while (pkCtrl != NULL)
    {
        float fControllerStart = (pkCtrl->GetBeginKeyTime() / 
                pkCtrl->GetFrequency()) + pkCtrl->GetPhase();

        float fControllerEnd = (pkCtrl->GetEndKeyTime() / 
                pkCtrl->GetFrequency()) + pkCtrl->GetPhase();

        if (fControllerStart < fStartTime)
            fStartTime = fControllerStart;

        if (fControllerEnd > fEndTime)
            fEndTime = fControllerEnd;

        bFoundController = true;

        pkCtrl = pkCtrl->GetNext();
    }


    // Check if we can have children
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        // Add all of the children to the list
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            if ((pkNode->GetAt(uiLoop) != NULL) &&
                GetAnimationTimeRange(pkNode->GetAt(uiLoop), fStartTime, 
                    fEndTime))
                bFoundController = true;
        }
    }

    // Return true if we or any of our children has a controller
    return((pkCtrl != NULL) || (bFoundController));
}
//---------------------------------------------------------------------------
void MyImmerse::CheckForLightsAtSceneRoot()
{
    // Check each light to see if all of the Children of the scene root
    // are on its affected list. If so then remove all the children and
    // add the Scene node
    int iNumSceneChildren = m_spScene->GetArrayCount();
    int iChildLoop = 0;

    int iLoop;
    for (iLoop = 0; iLoop < (int)m_uiLights; iLoop++)
    {
        bool bFound = true;
        
        // Loop through each of the Children of the Scene
        // checking for Affected node
        for (iChildLoop = 0; (iChildLoop < iNumSceneChildren) && 
            bFound; iChildLoop++)
        {
            bFound = true;

            // Only check for nodes with Mesh underneath
            if ((m_spScene->GetAt(iChildLoop) != NULL) &&
                HasMeshChild(m_spScene->GetAt(iChildLoop)))
            {
                bFound = false;
                const NiNodeList& AffectedList = 
                    m_sppLights[iLoop]->GetAffectedNodeList();

                NiTListIterator kIterator = AffectedList.GetHeadPos();

                while ((kIterator != NULL) && !bFound)
                {
                    if (AffectedList.GetNext(kIterator) == 
                            m_spScene->GetAt(iChildLoop))
                    {
                        bFound = true;
                    }
                }
            }
        }
        
        // Make Sure Every
        // If Every Affected is a child of the scene
        if (bFound)
        {
            m_sppLights[iLoop]->DetachAllAffectedNodes();
            m_sppLights[iLoop]->AttachAffectedNode(m_spScene);
        }
    }
}
//---------------------------------------------------------------------------
bool MyImmerse::HasMeshChild(NiAVObject* pkObject)
{
    // Are we a mesh object
    if (NiDynamicCast(NiMesh, pkObject))
        return true;

    NiNode* pNode = NiDynamicCast(NiNode, pkObject);

    // Are we a Node?
    if (pNode != NULL)
    {

        // Check each of our children for Mesh
        for ( int iLoop = 0; iLoop < (int)pNode->GetArrayCount(); iLoop++)
        {
            if ((pNode->GetAt(iLoop) != NULL) &&
                HasMeshChild(pNode->GetAt(iLoop)))
                return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void MyImmerse::CenterAllLODOnBounds(NiNodePtr spNode)
{

    NiLODNodePtr kLODNode = NiDynamicCast(NiLODNode, spNode);
    int iLoop;

    if (kLODNode)
    {
        // Center the LOD on the bounds of it's children
        bool bFirst = true;
        NiBound kNewBound;
        
        for (iLoop = 0; iLoop < (int)spNode->GetArrayCount(); iLoop++)
        {
            NiAVObjectPtr spChild = spNode->GetAt(iLoop);
            if (spChild != NULL)
            {
                spChild->Update(0.0f);
                NiMesh::CompleteSceneModifiers(spChild);

                if (bFirst)
                {
                    kNewBound = spChild->GetWorldBound();
                    bFirst = false;
                }
                else
                {
                    kNewBound.Merge(&spChild->GetWorldBound());
                }
            }
        }
        
        // Translate the LOD to the center of the bound;
        NiTransform kWorld;
        NiTransform kWorldInverse = spNode->GetWorldTransform();
        kWorldInverse.Invert(kWorld);
        
        NiPoint3 kDeltaCenter = (kWorldInverse * kNewBound.GetCenter()) - 
            spNode->GetTranslate();
        
        spNode->SetTranslate( kDeltaCenter + spNode->GetTranslate());
        
        for (iLoop = 0; iLoop < (int)spNode->GetArrayCount(); iLoop++)
        {
            NiAVObjectPtr spChild = spNode->GetAt(iLoop);
            if (spChild != NULL)
            {
                spChild->SetTranslate(spChild->GetTranslate() - kDeltaCenter);
            }
        }
    }
 

    // Check the Children
    for (iLoop = 0; iLoop < (int)spNode->GetArrayCount(); iLoop++)
    {
        if ((spNode->GetAt(iLoop) != NULL) &&
            NiIsKindOf(NiNode, spNode->GetAt(iLoop)))
        {
            CenterAllLODOnBounds(NiDynamicCast(NiNode, spNode->GetAt(iLoop)));
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::GetAllAnimations(NiTList<NiTimeController*>& kTList, 
    NiAVObject* pkObject)
{
    // Add to the list of 
    NiTimeController* pkController = pkObject->GetControllers();

    while ( pkController != NULL)
    {
        kTList.AddTail(pkController);
        pkController = pkController->GetNext();
    }


    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    // Only loop on children
    if (pkNode == NULL)
        return;

    // Check the Children
    int iLoop;
    for (iLoop = 0; iLoop < (int)pkNode->GetArrayCount(); iLoop++)
    {
        NiAVObject* pkChild = pkNode->GetAt(iLoop);

        if (pkChild != NULL)
        {
            GetAllAnimations(kTList, pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::ShareAllDuplicateAnimationData()
{
    // Build the List of aniumating controllers
    NiTList<NiTimeController*> kTList;
    GetAllAnimations(kTList, m_spScene);

    // Search the list sharing Data
    NiTListIterator kCurrent = kTList.GetHeadPos();

        // Found no time controllers
    if (kCurrent == NULL)
        return;

    NiTListIterator kTarget = kTList.GetNextPos(kCurrent);

    NiTimeController* pkCurrentCtrl = NULL;
    NiTimeController* pkTargetCtrl = NULL;

    if(DtExt_HaveGui())
    {   
        // Update the progress window
        MProgressWindow::setProgressStatus("Removing Duplicate Animations");
        MProgressWindow::setProgressRange(0, 
            (kTList.GetSize() * kTList.GetSize() / 2));
        MProgressWindow::setProgress(0);
    }

    while (kCurrent != NULL)
    {
        pkCurrentCtrl = kTList.Get(kCurrent);

        while (kTarget != NULL)
        {
            if(DtExt_HaveGui())
            {
                // Increment the progress window
                MProgressWindow::advanceProgress(1);
            }

            pkTargetCtrl = kTList.Get(kTarget);

            if (DuplicateAnimationData(pkCurrentCtrl,pkTargetCtrl))
            {
                // Remove the current from the list
                NiTListIterator kTemp = kTarget;
                kTarget = kTList.GetNextPos(kTarget);

                kTList.RemovePos(kTemp);
            }
            else
            {
                kTarget = kTList.GetNextPos(kTarget);
            }
        }

        kCurrent = kTList.GetNextPos(kCurrent);
        kTarget = kTList.GetNextPos(kCurrent);

    }

    kTList.RemoveAll();
}
//---------------------------------------------------------------------------
bool MyImmerse::DuplicateAnimationData(NiTimeController* pkCtrl, 
    NiTimeController* pkCtrl2)
{
    // Make sure they are of the same type
    if (pkCtrl->GetRTTI() != pkCtrl2->GetRTTI())
        return false;

    if (NiIsExactKindOf(NiTransformController, pkCtrl))
    {
        return DuplicateAnimationData((NiTransformController*)pkCtrl, 
                (NiTransformController* )pkCtrl2);
    }


    return false;
}
//---------------------------------------------------------------------------
bool MyImmerse::DuplicateAnimationData(NiTransformController* pkCtrl, 
    NiTransformController* pkCtrl2)
{
    // Compare the Key Frame Data
    NiTransformData* pkData1 = ((NiTransformInterpolator*)pkCtrl->
        GetInterpolator())->GetTransformData();

    NiTransformData* pkData2 = ((NiTransformInterpolator*)pkCtrl2->
        GetInterpolator())->GetTransformData();

    if(pkData1 != NULL && pkData2 != NULL)
    {
        if (pkData1->IsEqual(pkData2))
        {
            ((NiTransformInterpolator*)pkCtrl2->GetInterpolator())->
                SetTransformData(pkData1);
            return true;
        }
    }
    else
    {
        //NIASSERT(0);
    }
    

    return false;
}
//---------------------------------------------------------------------------
void MyImmerse::GetKeepFlags(NiNode* pkNode, unsigned int& uiKeepFlags)
{
    // Check for an Extra Attribute
    MFnDagNode dgNode;
    if (!FindDagNode(pkNode, dgNode))
    {
        return;
    }

    bool bValue = false;
    if (GetExtraAttribute(dgNode, "Ni_MeshStaticKeepPositions", true,
        bValue))
    {
        if (bValue)
            uiKeepFlags |= KEEP_XYZ;
        else
            uiKeepFlags &= ~KEEP_XYZ;
    }
    
    if (GetExtraAttribute(dgNode, "Ni_MeshStaticKeepNormals", true, 
        bValue))
    {
        if (bValue)
            uiKeepFlags |= KEEP_NORM;
        else
            uiKeepFlags &= ~KEEP_NORM;
    }
    
    if (GetExtraAttribute(dgNode, "Ni_MeshStaticKeepColors", true, 
        bValue))
    {
        if (bValue)
            uiKeepFlags |= KEEP_COLOR;
        else
            uiKeepFlags &= ~KEEP_COLOR;
    }
    
    if (GetExtraAttribute(dgNode, "Ni_MeshStaticKeepUVs", true, bValue))
    {
        if (bValue)
            uiKeepFlags |= KEEP_UV;
        else
            uiKeepFlags &= ~KEEP_UV;
    }
    
    if (GetExtraAttribute(dgNode, "Ni_MeshStaticKeepIndices", true, 
        bValue))
    {
        if (bValue)
            uiKeepFlags |= KEEP_INDICES;
        else
            uiKeepFlags &= ~KEEP_INDICES;
    }
    
    if (GetExtraAttribute(dgNode, "Ni_MeshStaticKeepBoneData", true, 
        bValue))
    {
        if (bValue)
            uiKeepFlags |= KEEP_BONEDATA;
        else
            uiKeepFlags &= ~KEEP_BONEDATA;
    }
}
//---------------------------------------------------------------------------
void MyImmerse::SetMeshAttributes(NiAVObject* pkObject, 
    unsigned int uiKeepFlags)
{
    // This function recurses over the scene and when it encounters mesh,
    // it sets the access flags based off of either artist specification or
    // mesh modifier requirements
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;

        NiDataStreamRef* pkStreamRef = NULL;
        NiDataStreamElement kElement;

        // POSITIONS
        
        if (pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_UNKNOWN, 
            pkStreamRef, kElement))
        {
            NiToolDataStream* pkPositionStream = NiVerifyStaticCast(
                NiToolDataStream, pkStreamRef->GetDataStream());

            if (uiKeepFlags & KEEP_XYZ)
                pkPositionStream->MakeReadable();
        }

        // NORMALS
        if (pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::NORMAL(), 0, NiDataStreamElement::F_UNKNOWN,
            pkStreamRef, kElement))
        {     

            NiToolDataStream* pkNormalStream = NiVerifyStaticCast(
                NiToolDataStream, pkStreamRef->GetDataStream());

            if (uiKeepFlags & KEEP_NORM)
                pkNormalStream->MakeReadable();
        }

        // COLORS
        if (pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::COLOR(), 0, NiDataStreamElement::F_UNKNOWN,
            pkStreamRef, kElement))
        {

            NiToolDataStream* pkColorStream = NiVerifyStaticCast(
                NiToolDataStream, pkStreamRef->GetDataStream());

            if (uiKeepFlags & KEEP_COLOR)
                pkColorStream->MakeReadable();
        }

        // TEXTCOORDS - iterate over each set
        NiInt32 iMaxIndex = pkMesh->GetMaxIndexBySemantic(
            NiCommonSemantics::TEXCOORD());
        if (-1 != iMaxIndex)
        {
            for (NiUInt32 ui = 0; ui <= (NiUInt32)iMaxIndex; ui++)
            {
                if (pkMesh->FindStreamRefAndElementBySemantic(
                    NiCommonSemantics::TEXCOORD(), ui, 
                    NiDataStreamElement::F_UNKNOWN, pkStreamRef, kElement))
                {
        
                    NiToolDataStream* pkTexCoordStream = NiVerifyStaticCast(
                        NiToolDataStream, pkStreamRef->GetDataStream());

                    if (uiKeepFlags & KEEP_UV)
                        pkTexCoordStream->MakeReadable();
                }
            }
        }

        // INDEX
        if (pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::INDEX(), 0, NiDataStreamElement::F_UNKNOWN,
            pkStreamRef, kElement))
        {

            NiToolDataStream* pkIndicesStream = NiVerifyStaticCast(
                NiToolDataStream, pkStreamRef->GetDataStream());

            if (uiKeepFlags & KEEP_INDICES)
                pkIndicesStream->MakeReadable();
        }

        // BLENDWEIGHT & BLENDINDICES
        if (pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::BLENDWEIGHT(), 0,
            NiDataStreamElement::F_UNKNOWN, pkStreamRef, kElement))
        {

            NiToolDataStream* pkBoneWeightStream = NiVerifyStaticCast(
                NiToolDataStream, pkStreamRef->GetDataStream());

            if (uiKeepFlags & KEEP_BONEDATA)
                pkBoneWeightStream->MakeReadable();
        }
        if (pkMesh->FindStreamRefAndElementBySemantic(
            NiCommonSemantics::BLENDINDICES(), 0,
            NiDataStreamElement::F_UNKNOWN, pkStreamRef, kElement))
        {

            NiToolDataStream* pkBlendIndicesStream = NiVerifyStaticCast(
                NiToolDataStream, pkStreamRef->GetDataStream());

            if (uiKeepFlags & KEEP_BONEDATA)
                pkBlendIndicesStream->MakeReadable();
        }
    }

    // Look for the user defined options and recurse over all of the children
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;

        // Check this node for updated flags
        GetKeepFlags(pkNode, uiKeepFlags);

        // Apply Consistency to All of the children
        int iLoop;
        for (iLoop = 0; iLoop < (int)pkNode->GetArrayCount(); iLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(iLoop);
            if (pkChild != NULL) 
            {
                SetMeshAttributes(pkChild, uiKeepFlags);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::SetAnimationLoopingControls(NiAVObject* pkObject, 
        NiTimeController::CycleType kCycleType)
{

    NiTimeController::CycleType kLocalCycleType = kCycleType;

    // Look for the user defined options
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;

        MFnDagNode dgNode;
        if (FindDagNode(pkNode, dgNode))
        {
            // Check for the looping type
            int iLoopingType = 0;
            if (GetExtraAttribute(dgNode, "Ni_AnimationLoopingType", true, 
                iLoopingType))
            {
                kLocalCycleType = (NiTimeController::CycleType)iLoopingType;
            }

            // Check to see if this should be recursed down to all children
            bool bForAllChildren = false;
            if (GetExtraAttribute(dgNode, "Ni_AnimationLoopingForAllChildren", 
                true, bForAllChildren) && bForAllChildren)
            {
                kCycleType = kLocalCycleType;
            }
        }
    }



    // Check for Time controllers and set the looping values
    if (kLocalCycleType != NiTimeController::MAX_CYCLE_TYPES)
    {
        for (NiTimeController* pkController = pkObject->GetControllers();
                pkController != NULL; pkController = pkController->GetNext())
        {
            pkController->SetCycleType(kLocalCycleType);
        }
    }


    // Recurse over the children
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;

        for (int iLoop = 0; iLoop < (int)pkNode->GetArrayCount(); iLoop++)
        {
            if (pkNode->GetAt(iLoop) != NULL)
            {
                SetAnimationLoopingControls(pkNode->GetAt(iLoop), kCycleType);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::CreateBoneLODTags(NiNode* pkNode)
{
    // Look for the pkCharacterNode
    MFnDagNode dgNode;
    if (FindDagNode(pkNode, dgNode))
    {          
        // Check for a Bone LOD Root and validate its value is true
        bool bResult;
        if (GetExtraAttribute(dgNode, "Ni_BoneLODRoot", true, bResult) 
            && bResult)
        {
            NiStringExtraData* pData = 
                NiNew NiStringExtraData("NiBoneLOD#BoneRoot#");

            pkNode->AddExtraData("BoneLODRoot", pData );
        }
        
        char acBuffer[256];
        int iBoneLODLevel;
        
        // Check for a Bone LOD Level
        if (GetExtraAttribute(dgNode, "Ni_BoneLODLevel", true, 
            iBoneLODLevel))
        {
            NiSprintf(acBuffer, 256, "NiBoneLOD#Bone#%d", iBoneLODLevel);
            NiStringExtraData* pData = NiNew NiStringExtraData(acBuffer);
            pkNode->AddExtraData("BoneLODLevel", pData );
        }

        int iSkinMinLevel;
        int iSkinMaxLevel;

        // Check for a Bone LOD SKin Level
        if (GetExtraAttribute(dgNode, "Ni_SkinLODLevelMinimum", true, 
            iSkinMinLevel) &&
           GetExtraAttribute(dgNode, "Ni_SkinLODLevelMaximum", true, 
           iSkinMaxLevel) )
        {
            NiSprintf(acBuffer, 256, "NiBoneLOD#Skin#%d#%d", iSkinMinLevel,
                iSkinMaxLevel);

            NiStringExtraData* pData = NiNew NiStringExtraData(acBuffer);
            pkNode->AddExtraData("BoneLODSkin", pData );
        }
    }


    // Recurse over the children
    int iLoop;
    for (iLoop = 0; iLoop < (int)pkNode->GetArrayCount(); iLoop++)
    {
        NiAVObject* pkObject = pkNode->GetAt(iLoop);

        if ((pkObject != NULL) &&
           NiIsKindOf(NiNode, pkObject))
        {
            CreateBoneLODTags((NiNode*)pkObject);
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::RemoveExportOptionsNode()
{
    // Check the Children
    for (int iLoop = 0; iLoop < (int)m_spScene->GetArrayCount(); iLoop++)
    {
        NiAVObjectPtr spChild = m_spScene->GetAt(iLoop);
        if (spChild != NULL) 
        {
            // Check to see if this is our options node
            if (strstr(spChild->GetName(), 
                "Please_Dont_Delete_MyImmerse_Export_Options" ) != 0)
            {
                m_spScene->DetachChildAt(iLoop);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool MyImmerse::ExecuteScript()
{
    //This needs to get shutdown I think here
    

    // Build Shared Data
    BuildSceneGraphSharedData();
    BuildControllerExtractorSharedData();
    BuildRendererSharedData();
    BuildTimerSharedData();


    // Create the script we are going to run
    NiScriptInfoPtr spScriptInfo = CreateScript();

    if (spScriptInfo == NULL)
    {
        DeleteAllSharedData();
        return false;
    }

    // Save the Script back into the scene
    SaveScriptToScene(spScriptInfo);

    // Strip Exporting plugins if we arn't saving
    if (!gExport.m_bSaveExport)
        RemoveExportPluginsForViewOnly(spScriptInfo);

    // Seperate off the final Export Plug-in
    NiScriptInfoPtr spExportOnlyScript = 
        SeperateFinalExportScripts(spScriptInfo);

    BuildExportOptionsSharedData(spScriptInfo);

    char* pcSceneName = NULL;
    DtSceneGetName(&pcSceneName);
    
    if (gExport.m_bRunSilently == true)
    {
        GetXMLLogger()->StartElement(pcSceneName);
    }
    
    // Execute the Processing 
    NiFramework& kFramework = NiFramework::GetFramework();
    NiBatchExecutionResultPtr spResult = 
        kFramework.ExecuteScript(spScriptInfo);

    if (spResult->m_eReturnCode != NiExecutionResult::EXECUTE_SUCCESS)
    {
        DeleteAllSharedData();
        printf("\nError: Could not execute script.\n");
        return false;
    }

    HandleScriptResults(spResult, spScriptInfo);

    // Set the Selective Update Flags for the scene after processing has 
    // been done
    bool bSelectiveUpdate = false;
    bool bRigid = false;
    m_spScene->SetSelectiveUpdateFlags(bSelectiveUpdate, false, bRigid);


    if (spExportOnlyScript != NULL)
    {
        // Execute the Export Script
        NiBatchExecutionResultPtr spExportResult = 
            kFramework.ExecuteScript(spExportOnlyScript);

        if (spExportResult->m_eReturnCode != 
            NiExecutionResult::EXECUTE_SUCCESS)
        {
            DeleteAllSharedData();
            printf("\nError: Could not execute export Script.\n");
            return false;
        }

        HandleScriptResults(spExportResult, spExportOnlyScript);
    }

    // Call the correct viewer
    if (gExport.m_bViewExport)
    {
        NiScriptInfo* pkViewerInfo = GetViewerScript();

        // Execute the Viewer Script
        NiBatchExecutionResultPtr spViewerResult;
        if (pkViewerInfo)
        {
           spViewerResult = kFramework.ExecuteScript(pkViewerInfo);

           HandleScriptResults(spViewerResult, pkViewerInfo);
        }

        if (spViewerResult == NULL || !pkViewerInfo ||
            spViewerResult->m_eReturnCode != 
            NiExecutionResult::EXECUTE_SUCCESS )
        {
            DeleteAllSharedData();
            printf("\nError: Could not execute viewer script.\n");
            return false;
        }

        
    }

    PostScriptProcess();

    DeleteAllSharedData();

    if (gExport.m_bRunSilently == true)
    {
        GetXMLLogger()->EndElement(pcSceneName);
    }

    return true;
}
//---------------------------------------------------------------------------
void MyImmerse::CreateFullPath(char* pcFullPath, unsigned int uiBufferLen)
{
    // Create the full export path.
    char *pcSceneName;
    DtSceneGetName(&pcSceneName);

    char *pcDirectory;
    DtGetDirectory(&pcDirectory);
    
    NiSprintf(pcFullPath, uiBufferLen, "%s/%s",pcDirectory,pcSceneName);  
    
    // APPEND .nif ONLY IF IT ISN NOT THERE ALREADY
    char acTempFileName[_MAX_PATH];
    char *pcFileExt;
    
    NiStrcpy( acTempFileName, _MAX_PATH, pcSceneName );
#if defined(_MSC_VER) && _MSC_VER >= 1400
    _strupr_s(acTempFileName, _MAX_PATH);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    strupr( acTempFileName );
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    pcFileExt = strstr( acTempFileName, ".NIF" );
    
    
    // IF THE EXTENSION ISN"T THERE OR IT ISN"T AT THE END
    if (!pcFileExt ||      // NO EXTENSION
        ( pcFileExt != 
        ( acTempFileName + strlen( acTempFileName ) - strlen(".NIF") ) ) ) 
        // AT THE END
    {
        // APPEND THE .NIF
        NiStrcat(pcFullPath, uiBufferLen, ".nif");
    }
}
//---------------------------------------------------------------------------
void MyImmerse::SaveScriptToScene(NiScriptInfo* pkScriptInfo)
{
    // Save the Active Selection List
    MSelectionList activeSelectionList;
    MGlobal::getActiveSelectionList (activeSelectionList);

    int iIntVar;

    MStatus stat;
    stat = MGlobal::executeCommand(MString(
        "MyImmerseCreateExportOptionsLocator();"));

    // Check if the Export Options Attribute Exists
    stat = MGlobal::executeCommand(MString("attributeExists "
        "MyImmerseScriptVar |Please_Dont_Delete_MyImmerse_Export_Options"), 
        iIntVar);

    NIASSERT(stat == MS::kSuccess);

    if (iIntVar == 0)
    {
        // Select the Options Locator
        stat = MGlobal::executeCommand(MString("select "
            "|Please_Dont_Delete_MyImmerse_Export_Options"));

        NIASSERT(stat == MS::kSuccess);

        stat = MGlobal::executeCommand(MString("addAttr -dt \"string\" -ln "
            "MyImmerseScriptVar"));
        NIASSERT(stat == MS::kSuccess);
    }

    NiDefaultScriptWriter kWriter;
    NiString strScript = kWriter.Encode(pkScriptInfo);
    // Need to escape '\' since Maya attribute code treats this as 
    // an escape character. Replace '\' with '\\'.
    strScript.Replace("\\", "\\\\");
    strScript.Replace("\"", "<QUOTE>");
    strScript.Replace("\n", "<NEWLINE>");
    MString sOptions((const char*)strScript);
    stat = MGlobal::executeCommand(MString("setAttr "
        "|Please_Dont_Delete_MyImmerse_Export_Options.MyImmerseScriptVar "
        "-type \"string\" \"") + sOptions + "\"");

    NIASSERT(stat == MS::kSuccess);


    // Restore the Active Selection LIst
    MGlobal::setActiveSelectionList (activeSelectionList);
}
//---------------------------------------------------------------------------
NiScriptInfoPtr MyImmerse::CreateScript()
{
    char acFullPath[_MAX_PATH];
    CreateFullPath(acFullPath, _MAX_PATH);

    NiScriptInfoPtr spScriptInfo = NULL;

    // Try and load the script from the Maya file
    NiScriptInfoPtr spSceneScriptInfo = LoadScriptFromScene();

    // Check to see if we just want the previously saved script.
    if (spSceneScriptInfo && gExport.m_bUseLastSavedScript)
    {
        spScriptInfo = spSceneScriptInfo;
    }

    // Use the "Default Export" Script if there isn't a previous script
    // or if we are forcing to the default export script.
    if (!spScriptInfo && gExport.m_bForceDefaultScript)
    {
        // Automatically choose character export if exporting a character
        if (m_bExportingCharacter)
        {
            spScriptInfo = NiScriptTemplateManager::GetInstance()->
                GetScript("Default Character Export");
        }
        else
        {
            spScriptInfo = NiScriptTemplateManager::GetInstance()->
                GetScript("Default Export");
        }
    }

    // This means we want to load the one from disk.
    if (!spScriptInfo && spSceneScriptInfo)
    {
        NiScriptInfo* pkFileScript = 
            NiScriptTemplateManager::GetInstance()->GetScript(
            (const char*)spSceneScriptInfo->GetName());

        if (pkFileScript)
        {
            spScriptInfo = pkFileScript;
        }
        else
        {
            spScriptInfo = spSceneScriptInfo;
        }
    }

    if (!spScriptInfo && !gExport.m_bRunSilently)
    {
        // Automatically choose character export if exporting a character
        if (m_bExportingCharacter)
        {
            spSceneScriptInfo = NiScriptTemplateManager::GetInstance()->
                GetScript("Default Character Export");
        }
        else
        {
            spSceneScriptInfo = NiScriptTemplateManager::GetInstance()->
                GetScript("Default Export");
        }

        // Give the User the option of selecting a different script
        spScriptInfo = NiScriptInfoDialogs::DoSelectionDialog(
            spSceneScriptInfo, NULL, GetActiveWindow(), 
            "PROCESS\nEXPORT\nVIEWER")->Clone();
    }

    // If we do not have a script load the default.
    if (!spScriptInfo)
    {
        // Automatically choose character export if exporting a character
        if (m_bExportingCharacter)
        {
            spScriptInfo = NiScriptTemplateManager::GetInstance()->
                GetScript("Default Character Export");
        }
        else
        {
            spScriptInfo = NiScriptTemplateManager::GetInstance()->
                GetScript("Default Export");
        }
    }

    if (spScriptInfo == NULL)
        return NULL;

    // Setup Plugin Info for Exporters

    // Crate the File name with the extension removed
    char* pcSceneName;
    DtSceneGetName(&pcSceneName);
    char acPluginFile[MAX_PATH];
    NiStrcpy(acPluginFile, MAX_PATH, pcSceneName);

        // Strip off the extension if it exists
    if (strlen(acPluginFile) >= strlen(".nif"))
    {
        char* pcTemp = acPluginFile + strlen(acPluginFile) - strlen(".nif");

        if (NiStricmp(pcTemp, ".nif") == 0)
            *pcTemp = '\0';
    }


    // Create the directory with extra back slash
    char *pcDirectory;
    DtGetDirectory(&pcDirectory);
    char acPluginDir[MAX_PATH];
    NiSprintf(acPluginDir, MAX_PATH, "%s\\", pcDirectory);
    MyiTexture::ReplaceChar(acPluginDir, '/', '\\');

    NiTPtrSet<NiPluginInfoPtr, NiTNewInterface<NiPluginInfoPtr> >& 
        kInfo = spScriptInfo->GetPluginSet();

    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < kInfo.GetSize(); uiLoop++)
    {
        if (NiIsKindOf(NiExportPluginInfo, kInfo.GetAt(uiLoop)))
        {
            NiPluginInfo* pkTemp = kInfo.GetAt(uiLoop);
            NiExportPluginInfo* pkExportInfo = (NiExportPluginInfo*)pkTemp;

            pkExportInfo->SetDirectory(acPluginDir);
            pkExportInfo->SetFile(acPluginFile);
            pkExportInfo->SetExtension(".nif");
        }
    }

    // We need to clone it because we don't want to modify the original.
    return spScriptInfo->Clone();
}
//---------------------------------------------------------------------------
NiScriptInfo* LoadScriptFromPreT3D()
{
    int iIntVar;

    // Check if the Locator exists
    MStatus stat = MGlobal::executeCommand(
        MString("objExists |Please_Dont_Delete_MyImmerse_Export_Options"), 
        iIntVar);

    NIASSERT(stat == MS::kSuccess);

    if (iIntVar == 0)
    {
        return NULL;
    }

    // Check if the Export Options Attribute Exists
    stat = MGlobal::executeCommand(
        MString("attributeExists MyImmerseNIFOptimizationVar "
        "|Please_Dont_Delete_MyImmerse_Export_Options"), iIntVar);

    NIASSERT(stat == MS::kSuccess);

    if (iIntVar == 0)
    {
        return NULL;
    }


    // Load the plugins we will need
    NiPlugin* pkOptimizationPlugin = NiPluginManager::GetInstance()->
        GetPluginByClass("NiSceneGraphOptimizationPlugin");

    if (pkOptimizationPlugin == NULL)
        return NULL;

    NiPlugin* pkStripifyPlugin = NiPluginManager::GetInstance()->
        GetPluginByClass("NiStripifyGeometryPlugin");

    if (pkStripifyPlugin == NULL)
        return NULL;

    NiPlugin* pkCreateABVPlugin = NiPluginManager::GetInstance()->
        GetPluginByClass("NiCreateABVPlugin");

    if (pkCreateABVPlugin == NULL)
        return NULL;

    NiPlugin* pkExportNIFPlugin = NiPluginManager::GetInstance()->
        GetPluginByClass("NifExportPlugin");

    if (pkExportNIFPlugin == NULL)
        return NULL;

    NiPluginInfo* pkOptimizationPluginInfo = 
        pkOptimizationPlugin->GetDefaultPluginInfo();

    NiPluginInfo* pkStripifyPluginInfo = 
        pkStripifyPlugin->GetDefaultPluginInfo();

    NiPluginInfo* pkCreateABVPluginInfo = 
        pkCreateABVPlugin->GetDefaultPluginInfo();

    NiPluginInfo* pkExportNIFPluginInfo = 
        pkExportNIFPlugin->GetDefaultPluginInfo();


    // Read the Attribute String
    MString sStringVar;
    stat = MGlobal::executeCommand(
        MString("getAttr |Please_Dont_Delete_MyImmerse_Export_Options."
        "MyImmerseNIFOptimizationVar"), sStringVar);

    NIASSERT(stat == MS::kSuccess);

    unsigned int uiLen = sStringVar.length() + 1;
    char* pcOptions = NiAlloc(char, uiLen);
    NiStrcpy(pcOptions, uiLen, sStringVar.asChar());

    bool bStripify = false;
    bool bCreateABVs = false;
    char* pcCurrentOption = pcOptions;
    char* pcNextOption = strstr(pcCurrentOption, ";");
    char* pcVariable;
    NiString strTrue = NiString::FromBool(true);
    NiString strFalse = NiString::FromBool(false);


    while ((pcCurrentOption != NULL) && (pcNextOption != NULL))
    {
        *pcNextOption = '\0';

        pcVariable = strstr(pcCurrentOption, ":"); 
        NIASSERT(pcVariable != NULL);

        *pcVariable++ = '\0';
        *pcNextOption++ = '\0';

        // Find and set the variable
        if (strcmp(pcCurrentOption,"ms_bRemoveDupProperties") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveDupProperties", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveDupProperties", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveChildlessNodes") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveChildlessNodes", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveChildlessNodes", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveMultiMtlNodes") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveMultiMtlNodes", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveMultiMtlNodes", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveSingleChildNodes") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveSingleChildNodes", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveSingleChildNodes", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bMergeSiblingNodes") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("MergeSiblingNodes", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("MergeSiblingNodes", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bOptimizeTriShapes") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("OptimizeTriShapes", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("OptimizeTriShapes", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bMergeSiblingTriShapes") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("MergeSiblingTriShapes", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("MergeSiblingTriShapes", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bNoMergeOnAlpha") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("NoMergeOnAlpha", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("NoMergeOnAlpha", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveExtraUVSets") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveExtraUVSets", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveExtraUVSets", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveUnnecessaryNormals") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveUnnecessaryNormals", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveUnnecessaryNormals", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,
            "ms_bRemoveUnnecessaryVisControllers") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue(
                "RemoveUnnecessaryVisControllers", strTrue);
            else
                pkOptimizationPluginInfo->SetValue(
                "RemoveUnnecessaryVisControllers", strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveNames") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveNames", strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveNames", strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bNameUnnamedObjects") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("NameUnnamedObjects", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("NameUnnamedObjects", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveBadTexturingProps") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveBadTexturingProps", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveBadTexturingProps", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveUnnecessaryExtraData") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue(
                "RemoveUnnecessaryExtraData", strTrue);
            else
                pkOptimizationPluginInfo->SetValue(
                "RemoveUnnecessaryExtraData", strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveSgoKeepTags") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("RemoveSgoKeepTags", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("RemoveSgoKeepTags", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption, 
            "ms_bRemoveUnnecessaryLookAtControllers") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue(
                "RemoveUnnecessaryLookAtControllers", strTrue);
            else
                pkOptimizationPluginInfo->SetValue(
                "RemoveUnnecessaryLookAtControllers", strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveHiddenBoneMesh") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue(
                "RemoveHiddenBoneGeometry", strTrue);
            else
                pkOptimizationPluginInfo->SetValue(
                "RemoveHiddenBoneGeometry", strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bReduceAnimationKeys") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("ReduceAnimationKeys", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("ReduceAnimationKeys", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bSetSelectiveUpdate") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("SetSelectiveUpdate", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("SetSelectiveUpdate", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,
            "ms_fAnimationKeyReductionTolerance") == 0)
        {
            pkOptimizationPluginInfo->SetValue(
                "AnimationKeyReductionTolerance", pcVariable);
        }
        else if (strcmp(pcCurrentOption,"ms_bSortByTexturingProp") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("SortByTexturingProp", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("SortByTexturingProp", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bConvertExternalTexturesToNIF")==0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue(
                "ConvertExternalTexturesToNIF", strTrue);
            else
                pkOptimizationPluginInfo->SetValue(
                "ConvertExternalTexturesToNIF", strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bStripify") == 0)
        {
            bStripify = *pcVariable == '1';
        }
        else if (strcmp(pcCurrentOption,"ms_eStripifyPlatform") == 0)
        {
            NiStripifyInterface::Platform ePlatform;

#if defined(_MSC_VER) && _MSC_VER >= 1400
            sscanf_s(pcVariable,"%d", &ePlatform);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            sscanf(pcVariable,"%d", &ePlatform);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

            switch(ePlatform)
            {
            case 0:   // NiStripifyInterface::PLATFORM_DX7 in NetImmerse 4.2.2
                pkStripifyPluginInfo->SetValue("Platform", "PC_Unaccelerated");
                break;
            case 1:   // NiStripifyInterface::PLATFORM_DX8 in NetImmerse 4.2.2
                pkStripifyPluginInfo->SetValue("Platform", 
                    "PC_NextGeneration");
                break;
            case 2:   // NiStripifyInterface::PLATFORM_OGL in NetImmerse 4.2.2
                pkStripifyPluginInfo->SetValue("Platform", "Do NOT Stripify");
                break;
            case 3:   // NiStripifyInterface::PLATFORM_NGC in NetImmerse 4.2.2
                pkStripifyPluginInfo->SetValue("Platform", "GameCube");
                break;
            case 4:   // NiStripifyInterface::PLATFORM_PS2 in NetImmerse 4.2.2
                pkStripifyPluginInfo->SetValue("Platform", "PS2");
                break;
            //case 5:  // NiStripifyInterface::PLATFORM_XBOX in NetImmerse 4.2.2
            //    pkStripifyPluginInfo->SetValue("Platform", "XBox");
            //    break;
            }
        }
        else if (strcmp(pcCurrentOption,"ms_eStripifyAlgorithm") == 0)
        {
            // Don't Do Anything
        }
        else if (strcmp(pcCurrentOption,"ms_uiStripifyCacheSize") == 0)
        {
            // Don't Do Anything
        }
        else if (strcmp(pcCurrentOption,"ms_bCreateABVs") == 0)
        {
            bCreateABVs = *pcVariable == '1';
        }
        else if (strcmp(pcCurrentOption,"ms_bRemoveABVProxyMesh") == 0)
        {
            if (*pcVariable == '1')
                pkCreateABVPluginInfo->SetValue("RemoveProxyGeometry", 
                strTrue);
            else
                pkCreateABVPluginInfo->SetValue("RemoveProxyGeometry", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bExportSoftwareSkin") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("SetSelectiveUpdate", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("SetSelectiveUpdate", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_bExportHardwareSkin") == 0)
        {
            if (*pcVariable == '1')
                pkOptimizationPluginInfo->SetValue("SetSelectiveUpdate", 
                strTrue);
            else
                pkOptimizationPluginInfo->SetValue("SetSelectiveUpdate", 
                strFalse);
        }
        else if (strcmp(pcCurrentOption,"ms_uiBonesPerPartition") == 0)
        {
            pkOptimizationPluginInfo->SetValue("BonesPerPartition", 
                pcVariable);
        }
        else 
        {
            NIASSERT(false);
        }


        pcCurrentOption = pcNextOption;
        pcNextOption = strstr(pcCurrentOption, ";");
    }

    NiFree(pcOptions);

    // Create a New Script Info
    NiScriptInfo* pkScriptInfo = NiNew NiScriptInfo;

    // Add in the Optimization plugin
    pkScriptInfo->AddPluginInfo(pkOptimizationPluginInfo);

    // Add in the stripification plugin
    if (bStripify)
    {
        pkScriptInfo->AddPluginInfo(pkStripifyPluginInfo);
    }

    // Create ABV's plugin
    if (bCreateABVs)
    {
        pkScriptInfo->AddPluginInfo(pkCreateABVPluginInfo);
    }

    // Add on the export plugin
    pkScriptInfo->AddPluginInfo(pkExportNIFPluginInfo);

    return pkScriptInfo;

}
//---------------------------------------------------------------------------
NiScriptInfo* MyImmerse::LoadScriptFromScene()
{
    int iIntVar;

    // Check if the Locator exists
    MStatus stat = MGlobal::executeCommand(
        MString("objExists |Please_Dont_Delete_MyImmerse_Export_Options"), 
        iIntVar);
    NIASSERT(stat == MS::kSuccess);

    if (iIntVar == 0)
    {
        return LoadScriptFromPreT3D();
    }

    // Check if the Export Options Attribute Exists
    stat = MGlobal::executeCommand(
        MString("attributeExists MyImmerseScriptVar "
        "|Please_Dont_Delete_MyImmerse_Export_Options"), iIntVar);

    NIASSERT(stat == MS::kSuccess);

    if (iIntVar == 0)
    {
        return NULL;
    }

    MString sOptions;
    stat = MGlobal::executeCommand(MString("getAttr "
        "|Please_Dont_Delete_MyImmerse_Export_Options.MyImmerseScriptVar"), 
        sOptions);

    NIASSERT(stat == MS::kSuccess);

    NiString strScript = sOptions.asChar();
    strScript.Replace("<QUOTE>", "\"");
    strScript.Replace("<NEWLINE>", "\n");

    NiString strErrors;
    NiDefaultScriptReader kReader;
    NiScriptInfo* pkScriptInfo = kReader.Parse(strScript, strErrors,
        gExport.m_bRunSilently);

    if (strErrors != "")
    {
        printf(strErrors);
    }

    if (!pkScriptInfo)
        return LoadScriptFromPreT3D();

    return pkScriptInfo;
}
//---------------------------------------------------------------------------
void MyImmerse::BuildControllerExtractorSharedData()
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedDataList.Lock();

    // Create the Shared Data
    NiControllerExtractorSharedData* pkControllerExtractorSharedData = 
        NiNew NiControllerExtractorSharedData;

    NiControllerExtractor::InfoArray& kInfoArray = 
        pkControllerExtractorSharedData->GetInfoArray();

    NiControllerExtractor::NiCCAGlobalInfoArray& kGlobalArray = 
        pkControllerExtractorSharedData->GetCCAGlobalInfoArray();

    NiControllerExtractor::NiCCAInfoArray& kOverrideArray = 
        pkControllerExtractorSharedData->GetCCAOverrideInfoArray();

    RecursivelyBuildControllerExtractorSharedData(kInfoArray, kGlobalArray,
        kOverrideArray, m_spScene);

    kSharedDataList.Insert(pkControllerExtractorSharedData);
    
    kSharedDataList.Unlock();
    
}
//---------------------------------------------------------------------------
void MyImmerse::RecursivelyBuildControllerExtractorSharedData(
    NiControllerExtractor::InfoArray& kInfoArray, 
    NiControllerExtractor::NiCCAGlobalInfoArray& kGlobalArray,
    NiControllerExtractor::NiCCAInfoArray& kOverrideArray,
    NiNode* pkNode)
{
    // Get the DagNode for current NiNode
    MFnDagNode dgNode;
    if (FindDagNode(pkNode, dgNode))
    {
        // Is the current node a CharacterRoot?
        if (CheckForExtraAttribute(dgNode, "Ni_CharacterRoot", true))
        {
            NiNode* pkCharacterNode = pkNode;
            SetOptimizeKeepFlag(pkCharacterNode);

            // Add global info for each character root node
            AddControllerExtractorCCAGlobalInfo(kGlobalArray, dgNode, 
                pkCharacterNode);

            // This method may return NULL if it can't find an
            // accumulation root, and that is OK since the user
            // may not have assigned an accumulation root.
            NiNode* pkAccumulationNode = RecursivelyFindFirstNodeWithAttribute(
                pkCharacterNode, "Ni_Sequence_Animation_Accumulation");
            if (pkAccumulationNode)
            {
                SetOptimizeKeepFlag(pkAccumulationNode);
            }

            RecursivelyCreateControllerExtractorSequences(kInfoArray, 
                kOverrideArray, pkNode, pkAccumulationNode, pkCharacterNode);

            // All the nodes below the character root have been processed,
            // so there is no need to continue down the hierarchy from
            // the current node (which is the character root).
            return;
        }
        // Does a node with animation sequences exist outside of a character
        // root hierarchy?
        else if (CheckForExtraAttribute(dgNode, "Ni_Sequence", true))
        {
                char pcBuffer[512];
                NiSprintf(pcBuffer, 512,
                    "ERROR:: Key Frame Sequences for Object %s are not "
                    "associated with character root. Add character root "
                    "attribute to hierarchy to fix problem. Ignoring "
                    "animation sequences for this node.\n",
                    pkNode->GetName() );

                DtExt_Err(pcBuffer);
        }
    }

    // Add all of the children to the list
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
    {
        if (pkNode->GetAt(uiLoop) != NULL)
        {
            NiNode* pkChild = NiDynamicCast(NiNode, pkNode->GetAt(uiLoop));
            
            if (pkChild != NULL)
            {
                RecursivelyBuildControllerExtractorSharedData(kInfoArray,
                    kGlobalArray, kOverrideArray, pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::RecursivelyCreateControllerExtractorSequences(
    NiControllerExtractor::InfoArray& kInfoArray,   
    NiControllerExtractor::NiCCAInfoArray& kOverrideArray,
    NiNode* pkNode, NiNode* pkAccumulationNode, NiNode* pkCharacterNode)
{
    NIASSERT(pkCharacterNode);
    NIASSERT(pkNode);

    // Get the DagNode for current NiNode
    MFnDagNode dgNode;
    if (FindDagNode(pkNode, dgNode))
    {
        if (CheckForExtraAttribute(dgNode, "Ni_Sequence", true))
        {    
            // Create the Sequence Info
            CreateControllerExtractorSequences(dgNode, pkNode, kInfoArray, 
                pkAccumulationNode, pkCharacterNode);

            AddControllerExtractorCCAInfo(kOverrideArray, dgNode, pkNode,
                pkCharacterNode);
        }
    }

    // Add all of the children to the list
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
    {
        if (pkNode->GetAt(uiLoop) != NULL)
        {
            NiNode* pkChild = NiDynamicCast(NiNode, pkNode->GetAt(uiLoop));
            
            if (pkChild != NULL)
            {
                RecursivelyCreateControllerExtractorSequences(kInfoArray,   
                    kOverrideArray, pkChild, pkAccumulationNode,
                    pkCharacterNode);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiNode* MyImmerse::RecursivelyFindFirstNodeWithAttribute(NiNode* pkNode,
    char* pcAttributeName)
{
    if (pkNode == NULL)
    {
        return NULL;
    }

    MFnDagNode dgNode;
    if (FindDagNode(pkNode, dgNode))
    {
        if (CheckForExtraAttribute(dgNode, pcAttributeName, true))
        {
            return pkNode;
        }
    }

    // Add all of the children to the list
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
    {
        if (pkNode->GetAt(uiLoop) != NULL)
        {
            NiNode* pkChild = NiDynamicCast(NiNode, pkNode->GetAt(uiLoop));
            
            if (pkChild != NULL)
            {
                NiNode* pkFoundNode = 
                    RecursivelyFindFirstNodeWithAttribute(pkChild, 
                    pcAttributeName);
                if (pkFoundNode)
                {
                    return pkFoundNode;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MyImmerse::FindDagNode(NiNode* pkNode, MFnDagNode& dgNode)
{
    if (pkNode == NULL)
    {
        return false;
    }

    int iComponentNum = gUserData.GetShapeNum(pkNode);
    if (iComponentNum >= 0)
    {
        MObject Object;
        if (gMDtObjectGetTransform(iComponentNum, Object) != 0)
        {
            dgNode.setObject(Object);
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void MyImmerse::SetOptimizeKeepFlag(NiNode* pkNode)
{
    NIASSERT(pkNode);
    if (pkNode)
    {
        if (!NiOptimize::CheckForExtraDataTags(pkNode))
        {
            NiStringExtraData* pkDontCull =  
                NiNew NiStringExtraData("NiOptimizeKeep");
            pkNode->AddExtraData("NiOptimizeKeep", pkDontCull);
        }
    }
}
//---------------------------------------------------------------------------
void MyImmerse::AddControllerExtractorCCAGlobalInfo(
    NiControllerExtractor::NiCCAGlobalInfoArray& kGlobalArray,
    MFnDagNode& dgNode, NiNode* pkCharacterNode)
{
    NiControllerExtractor::NiCCAGlobalInfo* pkGlobalInfo =
        NiNew NiControllerExtractor::NiCCAGlobalInfo;

    pkGlobalInfo->m_spCharacterRoot = pkCharacterNode;
    pkGlobalInfo->m_bUseBSplineCompression = false;
    pkGlobalInfo->m_fBSplineCompressionRatio = 1.0f;
    pkGlobalInfo->m_bUseCompactControlPoints = false;

    // Check for a character compression
    int iCompressionGlobal;
    if (GetExtraAttribute(dgNode, 
        "Ni_CharacterCompressionGlobal", true, 
        iCompressionGlobal))
    {
        // 101 indicated it will be uncompressed
        pkGlobalInfo->m_bUseBSplineCompression = (iCompressionGlobal < 101);
        pkGlobalInfo->m_fBSplineCompressionRatio = (float)iCompressionGlobal / 
            100.0f;

        // Check for the Compact Global
        GetExtraAttribute(dgNode, 
            "Ni_CharacterCompactGlobal", true, 
            pkGlobalInfo->m_bUseCompactControlPoints);
    }

    // Add in this character
    kGlobalArray.Add(pkGlobalInfo);

}
//---------------------------------------------------------------------------
void MyImmerse::AddControllerExtractorCCAInfo(
    NiControllerExtractor::NiCCAInfoArray& kInfoArray,
    MFnDagNode& dgNode, NiNode* pkCurrent, NiNode* pkCharacterNode)
{
    MStatus s;

    const char* pcdgNodeName = dgNode.name(&s).asChar();
    NI_UNUSED_ARG(pcdgNodeName);
    NIASSERT(s == MS::kSuccess);

    int iNumAttributes = dgNode.attributeCount(&s);

    // Loop through each attribute attached to this node
    // looking for attributes which start with "Ni_CharacterCompression_"
    int iLoop;
    for (iLoop = 0; iLoop < iNumAttributes; iLoop++)
    {
        MObject AttributeName = dgNode.attribute(iLoop, &s);    
        MFnAttribute Attribute;
        Attribute.setObject(AttributeName);

        bool bIsArray = Attribute.isArray(&s);
        NI_UNUSED_ARG(bIsArray);
        NIASSERT(s == MS::kSuccess);

        MPlug kMPlug(dgNode.object(), AttributeName);

        const char* pcAttributeName = Attribute.name().asChar();
        const char* pcResult = strstr( pcAttributeName, 
            "Ni_CharacterCompression_" );

        if (pcResult)
        {
            // Collect the info from the attributes
            int iCompression = 101;
            GetExtraAttribute(dgNode, (char*)pcAttributeName, true, 
                iCompression);

            char pcSequenceName[256];
            GetCharacterCompressedSequenceNameFromAttribute(pcAttributeName, 
                pcSequenceName);

            // Create the Info
            NiControllerExtractor::NiCCAInfo* pkInfo = 
                NiNew NiControllerExtractor::NiCCAInfo;

            pkInfo->m_spCharacterRoot = pkCharacterNode;
            pkInfo->m_spObject = pkCurrent;
            pkInfo->SetSequenceName(pcSequenceName);
            pkInfo->m_bUseBSplineCompression = (iCompression < 101);
            pkInfo->m_fBSplineCompressionRatio = (float)iCompression / 100.0f;

            pkInfo->m_bUseCompactControlPoints = 
                GetCompactKey(pcAttributeName, dgNode);

            kInfoArray.Add(pkInfo);
        }
    }
}
//---------------------------------------------------------------------------
bool MyImmerse::GetCompactKey(const char* pcAttributeName, MFnDagNode& dgNode)
{
    // Convert the attribute name
    char acCompactAttributeName[256] = "Ni_CharacterCompactKeys_";

    // Find the beginnning of the sequence name
    char* pcSequenceName = (char*)pcAttributeName + 
        strlen("Ni_CharacterCompression_");

    // concatinate it to te compact attribute name
    NiStrcat(acCompactAttributeName, 256, pcSequenceName);

    bool bCompactKey = false;

    // Check for the Compact Key
    GetExtraAttribute(dgNode, acCompactAttributeName, true, bCompactKey);

    return bCompactKey;
}
//---------------------------------------------------------------------------
void MyImmerse::GetCharacterCompressedSequenceNameFromAttribute(
    const char* pcAttributeName, char* pcSequenceName)
{
    // Strip off the beginning
    if (strlen(pcAttributeName) < strlen("Ni_CharacterCompression_"))
        return;

    char* pcBeginning = (char *)pcAttributeName + 
        strlen("Ni_CharacterCompression_");

    NiStrcpy(pcSequenceName, 256, pcBeginning);

    // Convert in place all "_SpAcE_" back to a simple space
    pcBeginning = strstr(pcSequenceName, "_SpAcE_");
    while (pcBeginning)
    {
        // convert to space
        *pcBeginning++ = ' ';

        // copy over the rest of the string
        NiStrcpy(pcBeginning, 256, 
            (char*)(pcBeginning + strlen("_SpAcE_") - 1));

        // Convert in place all "_SpAcE_" back to a simple space
        pcBeginning = strstr(pcSequenceName, "_SpAcE_");
    }
}
//---------------------------------------------------------------------------
void MyImmerse::BuildSceneGraphSharedData()
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedDataList.Lock();

    // Build the Shared Data
    NiSceneGraphSharedDataPtr spSceneGraph = NiNew NiSceneGraphSharedData;
    
    kSharedDataList.Insert(spSceneGraph);
    
    spSceneGraph->AddRoot(m_spScene, "", true);

    kSharedDataList.Unlock();
}
//---------------------------------------------------------------------------
void MyImmerse::BuildExportOptionsSharedData(NiScriptInfo* pkCurrentScript)
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedDataList.Lock();

    // Build the Shared Data
    NiExporterOptionsSharedDataPtr spOptions = 
        NiNew NiExporterOptionsSharedData;

    if (gExport.m_bRunSilently == true)
    {
        spOptions->SetWriteResultsToLog(true);
        NiXMLLogger* pkLogger = MyImmerse::GetXMLLogger();
        spOptions->SetXMLLogger(pkLogger);
    }

    char *pcSceneName;
    DtSceneGetName(&pcSceneName);

    // Set the Options
    spOptions->SetCurrentScript(pkCurrentScript);
    spOptions->SetViewerRun(strcmp(pcSceneName,"NifViewerMode")==0);
    spOptions->SetOptimizeMeshForViewer(gExport.m_bOptimizeMeshForViewer);
        
    kSharedDataList.Insert(spOptions);
    kSharedDataList.Unlock();
}
//---------------------------------------------------------------------------
void MyImmerse::BuildRendererSharedData()
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedDataList.Lock();

    // Build the Shared Data
    NiRendererSharedDataPtr spRenderer = NiNew NiRendererSharedData;

    // Set the Renderer Values
    spRenderer->SetBackgroundColor(NiColor(gExport.m_fRed, gExport.m_fGreen, 
        gExport.m_fBlue));

    spRenderer->SetFullscreen(gExport.m_bFullScreen);
    spRenderer->SetWindowSize(NiPoint2((float)gExport.m_iWindowWidth, (
        float)gExport.m_iWindowHeight));
   
    kSharedDataList.Insert(spRenderer);
    kSharedDataList.Unlock();
}
//---------------------------------------------------------------------------
void MyImmerse::BuildTimerSharedData()
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedDataList.Lock();

    // Build the Shared Data
    NiTimerSharedDataPtr spTimer = NiNew NiTimerSharedData;

    spTimer->SetCurrentTime(0.0f);
    spTimer->SetScaleFactor(1.0f);
    spTimer->SetTimeMode(NiTimerSharedData::LOOP);


    // Initialize the Start and End Time
    float fStartTime = NI_INFINITY;
    float fEndTime = -NI_INFINITY;

    // Determine the starting and ending time of the animation loop time
    if (gExport.m_bAnimationExportAnimationRange || 
        !GetAnimationTimeRange(m_spScene, fStartTime, fEndTime) )
    {
        // Set the start and end times based on Maya's animations slider
        spTimer->SetStartTime( (float)(gAnimControlGetMinTime() / 
            (gAnimControlGetFramesPerSecond() * 
             gAnimControlGetPlayBackSpeed())) );

        spTimer->SetEndTime( (float)(gAnimControlGetMaxTime() / 
            (gAnimControlGetFramesPerSecond() * 
             gAnimControlGetPlayBackSpeed())) );
    }
    else
    {
        // Set the start and end times based on the extreems for the scene
        spTimer->SetStartTime(fStartTime);
        spTimer->SetEndTime(fEndTime);
    }

    kSharedDataList.Insert(spTimer);
    kSharedDataList.Unlock();
}
//---------------------------------------------------------------------------
void MyImmerse::DeleteAllSharedData()
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedDataList = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedDataList.Lock();

    // Delete the SceneGraphSharedData
    kSharedDataList.DeleteAll();

    // UnLock the shared data list so other thread don't use it
    kSharedDataList.Unlock();
}
//---------------------------------------------------------------------------
void MyImmerse::CreateSceneNameAndVersionTag()
{
    // Get the Scene Name
    MString MVersionDescripter("NIF Creation Information");

    MString MSceneName;
    MStatus stat = MGlobal::executeCommand(MString("file -q -sn"), MSceneName);

    NIASSERT(stat == MS::kSuccess);
    if (stat != MS::kSuccess)
        return;

#ifdef MAYA40
#define NI_PLUGIN_NAME "MyImmerse"
#define NI_SHADER_NAME "NiMultiShader"
#endif

#ifdef MAYA45
#define NI_PLUGIN_NAME "MyImmerse45"
#define NI_SHADER_NAME "NiMultiShader45"
#endif

#ifdef MAYA50
#define NI_PLUGIN_NAME "MyImmerse50"
#define NI_SHADER_NAME "NiMultiShader50"
#endif

#ifdef MAYA60
#define NI_PLUGIN_NAME "MyImmerse60"
#define NI_SHADER_NAME "NiMultiShader60"
#endif

#ifdef MAYA65
#define NI_PLUGIN_NAME "MyImmerse65"
#define NI_SHADER_NAME "NiMultiShader65"
#endif

#ifdef MAYA70
#define NI_PLUGIN_NAME "MyImmerse70"
#define NI_SHADER_NAME "NiMultiShader70"
#endif

#ifdef MAYA80
#define NI_PLUGIN_NAME "MyImmerse80"
#define NI_SHADER_NAME "NiMultiShader80"
#endif

#ifdef MAYA85
#define NI_PLUGIN_NAME "MyImmerse85"
#define NI_SHADER_NAME "NiMultiShader85"
#endif

#ifdef MAYA2008
#define NI_PLUGIN_NAME "MyImmerse2008"
#define NI_SHADER_NAME "NiMultiShader2008"
#endif

    MString MMyImmerseVersion;
    stat = MGlobal::executeCommand(MString("pluginInfo -q -v ") +
        NI_PLUGIN_NAME, MMyImmerseVersion);

    MString MMultiShaderVersion;
    stat = MGlobal::executeCommand(MString("pluginInfo -q -v ") +
        NI_SHADER_NAME, MMultiShaderVersion);

    MString MProduct;
    stat = MGlobal::executeCommand(MString("about -p"),
        MProduct);

    MString MPlatform;
    switch(NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_GENERIC:
        MPlatform = "Platform = Generic";
        break;
    case NiSystemDesc::RENDERER_XENON:
        MPlatform = "Platform = XBox 360";
        break;
    case NiSystemDesc::RENDERER_DX9:
        MPlatform = "Platform = DX9";
        break;
    case NiSystemDesc::RENDERER_D3D10:
        MPlatform = "Platform = D3D10";
        break;
    case NiSystemDesc::RENDERER_PS3:
        MPlatform = "Platform = PS3";
        break;
    case NiSystemDesc::RENDERER_WII:
        MPlatform = "Platform = Wii";
        break;
    }
    MString MFileVersion = MVersionDescripter + MString(" >> ") + 
        MSceneName + MString(" >> ") + 
        MPlatform + MString(" >> ") +
        MMyImmerseVersion + MString(" >> ") + 
        MMultiShaderVersion + MString(" >> ") + 
        MProduct;

    // Create a Extra Data of the Scene Name
    NiStringExtraData* pData = NiNew NiStringExtraData( MFileVersion.asChar());
    m_spScene->AddExtraData("Scene Name and Version", pData );
}
//---------------------------------------------------------------------------
bool MyImmerse::PostScriptProcess()
{
    NiFramework& kFramework = NiFramework::GetFramework();
    NiSharedDataList& kSharedData = kFramework.GetSharedDataList();

    // Lock the shared data list so other thread don't use it
    kSharedData.Lock();

    kSharedData.Unlock();

    // Reset the Script Template Manager so we are using only these directories
    NiScriptTemplateManager::GetInstance()->RemoveAllScripts();
    NiScriptTemplateManager::GetInstance()->AddScriptDirectory(
        gExport.m_pcScriptTemplatePath, true, 
        gExport.m_bUseLastSavedScript);

    return true;
}
//---------------------------------------------------------------------------
bool MyImmerse::HandleScriptResults(NiBatchExecutionResult* pkResults,
    NiScriptInfo* pkScript)
{
    bool bSuccessful = true;
    NiString strResults = "The converter encountered the following errors: \n";

    if (!pkResults)
    {
        bSuccessful = false;
        strResults += "Invalid Batch Execution Result.\n";
    }
    else if (pkResults && !pkResults->WasSuccessful())
    {
        bSuccessful = false;
        unsigned int ui = 0;

        for (ui = 0; ui < pkResults->m_pkPluginResults.GetSize(); 
            ui++)
        {
            NiExecutionResult* pkExecResult = 
                pkResults->m_pkPluginResults.GetAt(ui);
            if (pkExecResult && pkExecResult->m_eReturnCode != 
                NiExecutionResult::EXECUTE_SUCCESS)
            {
                NiPluginInfo* pkInfo = pkScript->GetPluginInfoAt(ui);
                if (pkInfo)
                {
                    strResults += pkInfo->GetName() + " was not successful.\n";
                }
                else
                {
                    strResults += "Plugin #";
                    strResults += (unsigned char)ui;
                    strResults += " is not a valid Plugin\n";
                }
            }
        }
    }

    if (!bSuccessful)
    {
        NiMessageBox((const char*) strResults, "Export Failure");
    }
    
    return bSuccessful;
}
//---------------------------------------------------------------------------
NiScriptInfo* MyImmerse::GetViewerScript()
{
    // Create the Default Scripts just to make sure they haven't been deleted
    CreateDefaultViewerScripts();

    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();


    int iCurrentViewer = gExport.m_iViewer;

    // Default to Scene Viewer if we arn't viewing a character
    if (!m_bExportingCharacter && (gExport.m_iViewer == VIEWER_KFMVIEWER))
    {
        DtExt_Err("Warning: The scene contains no characters. Using Scene "
            "Viewer instead of the Animation Tool to view the scene.");
        iCurrentViewer = VIEWER_SCENEIMMERSE;
    }

    switch(iCurrentViewer)
    {
    case VIEWER_DX8:
    case VIEWER_DX9:
        return pkManager->GetScript("Viewer_Script_D3D");
        break;
    case VIEWER_KFMVIEWER:
        return pkManager->GetScript("Viewer_Script_KFM");
        break;
    case VIEWER_SCENEIMMERSE:
        return pkManager->GetScript("Viewer_Script_Scene_Viewer");
        break;
    case VIEWER_XENON:
        return pkManager->GetScript("Viewer_Script_Xbox360");
        break;
    case VIEWER_PS3:
        return pkManager->GetScript("Viewer_Script_PS3");
        break;
    case VIEWER_WII:
        return pkManager->GetScript("Viewer_Script_Wii");
        break;
    case VIEWER_D3D10:
        return pkManager->GetScript("Viewer_Script_D3D10");
        break;
    case VIEWER_CUSTOM:
        {
            NiScriptInfo* pkInfo = 
                pkManager->GetScript(gExport.m_acCustomViewerScript);
            return pkInfo;
        }
        break;
    default:
        NIASSERT(false);
        break;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MyImmerse::RemoveExportPluginsForViewOnly(NiScriptInfo* pkScriptInfo)
{
    NiTPtrSet<NiPluginInfoPtr, NiTNewInterface<NiPluginInfoPtr> >& kInfo =
        pkScriptInfo->GetPluginSet();

    // Loop Backwords because we are removing
    int iLoop;
    for (iLoop = ((int)kInfo.GetSize() - 1); iLoop >= 0; iLoop--)
    {
        if (kInfo.GetAt(iLoop)->GetType() == "EXPORT")
        {
            pkScriptInfo->RemovePlugin(kInfo.GetAt(iLoop));
        }
    }
}
//---------------------------------------------------------------------------
NiScriptInfo* MyImmerse::SeperateFinalExportScripts( NiScriptInfo*
    pkScriptInfo)
{
    // Find the last export script
    NiTPtrSet<NiPluginInfoPtr, NiTNewInterface<NiPluginInfoPtr> >& kInfo =
        pkScriptInfo->GetPluginSet();

    // Look for the empty List
    if (kInfo.GetSize() == 0)
        return NULL;

    // Look for No ending Export
    if (kInfo.GetAt(kInfo.GetSize() - 1)->GetType() != "EXPORT")
        return NULL;

    // Loop Backwords Looking for the first non export
    int iFirstEndExport;
    for (iFirstEndExport = ((int)kInfo.GetSize() - 1); 
        (iFirstEndExport >= 0) && 
        (kInfo.GetAt(iFirstEndExport)->GetType() == "EXPORT"); 
        iFirstEndExport--)
    {
    }

    NiScriptInfo* pkExportScript = NiNew NiScriptInfo;

    // Copy the Final Export Plugins
    int iLoop;
    for (iLoop = ++iFirstEndExport; iLoop < (int)kInfo.GetSize(); iLoop++)
    {
        pkExportScript->AddPluginInfo(kInfo.GetAt(iLoop));
    }

    // Remove the End Export Plugins from the original script
    for (iLoop = kInfo.GetSize()-1; iLoop >= iFirstEndExport; iLoop--)
    {
        kInfo.RemoveAt(iLoop);
    }

    return pkExportScript;
}
//---------------------------------------------------------------------------
void MyImmerse::CreateDefaultViewerScripts()
{
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();

    char acMayaLocation[MAX_PATH];

    // Get the location of maya and convert the directory seperators
#if defined(_MSC_VER) && _MSC_VER >= 1400
    unsigned int uiLen = 0;
    getenv_s(&uiLen, acMayaLocation, MAX_PATH, "MAYA_LOCATION");
    NIASSERT(uiLen <= MAX_PATH);
#else
    NiStrcpy(acMayaLocation, MAX_PATH, getenv("MAYA_LOCATION"));
#endif
    MyiTexture::ReplaceChar(acMayaLocation, '/', '\\');

    NiStrcat(acMayaLocation, MAX_PATH, "\\bin\\Plug-ins\\Gamebryo Scripts\\");

    int iLoop;
    for (iLoop = VIEWER_DX9; iLoop < VIEWER_CUSTOM; iLoop++)
    {
        NiScriptInfo* pkViewerScript = NULL;
        NiPlugin* pkViewerPlugin = NULL;
        NiString strFilePath(acMayaLocation);
         
        switch(iLoop)
        {
        case VIEWER_DX9:
            if (pkManager->GetScript("Viewer_Script_D3D") == NULL)
            {
                pkViewerPlugin = NiPluginManager::GetInstance()->
                        GetPluginByClass("NiSceneViewerD3DPlugin");

                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_D3D");
                strFilePath += "Viewer_Script_D3D.script";
            }
            break;
        case VIEWER_D3D10:
            if (pkManager->GetScript("Viewer_Script_D3D10") == NULL)
            {
                NiMessageBox("D3D10 viewer script not found.  Defaulting to DX9.",
                    "D3D10 Viewer Script Not Found");

                pkViewerPlugin = NiPluginManager::GetInstance()->
                        GetPluginByClass("NiSceneViewerD3DPlugin");
               
                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_D3D10");
                strFilePath += "Viewer_Script_D3D10.script";
            }
            break;
        case VIEWER_KFMVIEWER:
            if (pkManager->GetScript("Viewer_Script_KFM") == NULL)
            {
                pkViewerPlugin = NiPluginManager::GetInstance()->
                        GetPluginByClass("KFMViewerPlugin");
                
                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_KFM");
                strFilePath += "Viewer_Script_KFM.script";
            }
            break;
        case VIEWER_SCENEIMMERSE:
            if (pkManager->GetScript("Viewer_Script_Scene_Viewer") == NULL)
            {
                pkViewerPlugin = NiPluginManager::GetInstance()->
                    GetPluginByClass("NIFViewerPlugin");

                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_Scene_Viewer");
                strFilePath += "Viewer_Script_Scene_Viewer.script";
            }
            break;
        case VIEWER_XENON:
            if (pkManager->GetScript("Viewer_Script_Xbox360") == NULL)
            {
                pkViewerPlugin = NiPluginManager::GetInstance()->
                    GetPluginByClass("NiXbox360ViewerPlugin");

                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_Xbox360");
                strFilePath += "Viewer_Script_Xbox360.script";
            }
            break;
        case VIEWER_PS3:
            if (pkManager->GetScript("Viewer_Script_PS3") == NULL)
            {
                pkViewerPlugin = NiPluginManager::GetInstance()->
                    GetPluginByClass("NiPS3ViewerPlugin");

                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_PS3");
                strFilePath += "Viewer_Script_PS3.script";
            }
            break;
        case VIEWER_WII:
            if (pkManager->GetScript("Viewer_Script_Wii") == NULL)
            {
                pkViewerPlugin = NiPluginManager::GetInstance()->
                    GetPluginByClass("NiWiiViewerPlugin");

                pkViewerScript = NiNew NiScriptInfo;
                pkViewerScript->SetName("Viewer_Script_Wii");
                strFilePath += "Viewer_Script_Wii.script";
            }
            break;
        default:
            NIASSERT(false);
            break;
        }
        
        if (pkViewerPlugin != NULL)
        {
            pkViewerScript->AddPluginInfo(
                pkViewerPlugin->GetDefaultPluginInfo());
        
            pkViewerScript->SetFile(strFilePath);
            pkViewerScript->SetType("VIEWER");
        
            pkManager->WriteScript(pkViewerScript, strFilePath, 
                gExport.m_bRunSilently);

            /// Add a script to the list of known scripts, 
            pkManager->AddScript(pkViewerScript, true, 
                gExport.m_bRunSilently);
        }
        else
        {
            NiDelete pkViewerScript;
        }
    }
}
//---------------------------------------------------------------------------
NiXMLLogger* MyImmerse::GetXMLLogger()
{
    return ms_pkLogger;
}

NiXMLLogger* MyImmerse::InitXMLLogger()
{
    ms_pkLogger = new NiXMLLogger();
    return ms_pkLogger;
}

void MyImmerse::DestroyXMLLogger()
{
    if(ms_pkLogger != NULL)
    {
        delete ms_pkLogger;
        ms_pkLogger = NULL;
    }
    
}
