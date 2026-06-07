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
#include "AssetAnalyzerPlugin.h"
#include "AssetAnalyzerDialog.h"
#include "AssetAnalyzerResultsDialog.h"
#include "AssetAnalyzerPluginDefines.h"
#include <NiAnimation.h>
#include <NiMorphWeightsController.h>
#include <NiMesh.h>

// RTTI implementation macro.
NiImplementRTTI(AssetAnalyzerPlugin, NiPlugin);

//---------------------------------------------------------------------------
AssetAnalyzerPlugin::AssetAnalyzerPlugin() : NiPlugin(
    "Asset Analyzer Process Plug-in",                // name
    "2.0",                                          // version
    "An Asset Analyzer process plug-in",              // short description
    "An Asset Analyzer process plug-in that examines the scene to "
    "determine if it meets requirements for your game.")  // long description
{

}
//---------------------------------------------------------------------------
NiPluginInfo* AssetAnalyzerPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("AssetAnalyzerPlugin");
    pkPluginInfo->SetType("PROCESS");
    

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_TRIANGLE_COUNT, 300000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_TEXTURE_SIZE, 512);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE, 65536000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE, 32768000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAX_OBJECT_COUNT, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAX_OBJECT_COUNT, 416);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_MESH_RATIO, true);
    pkPluginInfo->SetFloat(
        ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_MESH_RATIO, 20.0f);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS, true);
    pkPluginInfo->SetInt(
        ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS, 4);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT, true);
    pkPluginInfo->SetInt(
        ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT, 200);

    pkPluginInfo->SetBool(
        ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT, true);
    pkPluginInfo->SetInt(
        ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT, 16000);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH, 20);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT, true);
    pkPluginInfo->SetInt(ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT, 2);

    pkPluginInfo->SetBool(ASSET_ANALYZER_USE_REQUIRED_OBJECTS, false);
    pkPluginInfo->SetInt(ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS, 0);

    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_SPOT_LIGHTS, true);
    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_POINT_LIGHTS, true);
    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS, false);
    pkPluginInfo->SetBool(ASSET_ANALYZER_NO_AMBIENT_LIGHTS, false);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return true;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    // Handle the user interface for manipulating the execution of this
    // NiPlugin.
    // - If the user clicks 'OK', make the input pkInitialInfo match the new
    //   set of parameters and return true.
    // - If the user clicks 'Cancel', then leave the input pkInitialInfo as
    //   it is and return false.

    AssetAnalyzerDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

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
bool AssetAnalyzerPlugin::CanExecute(NiPluginInfo* pkInfo)
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

    if (pkInfo->GetClassName() == "AssetAnalyzerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr AssetAnalyzerPlugin::Execute(
    const NiPluginInfo* pkInfo)
{

    // Save the old image converter
    NiImageConverterPtr spOldConverter =
        NiImageConverter::GetImageConverter();

    // Set the converter so we can load textures
    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);


    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll likely want to use the scene graph in this process plug-in,
    // so we'll go ahead and get the scene graph shared data.
    m_pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!m_pkSGSharedData)
    {
        // If we want to, we can pop up an NiMessageBox giving a more
        // useful warning if not running in silent mode.
        NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
        NIASSERT(pkDataList);
        NiExporterOptionsSharedData* pkExporterSharedData = 
            (NiExporterOptionsSharedData*) 
            pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
        bool bWriteResultsToLog = false;
        NiXMLLogger* pkLogger = NULL;

        if (pkExporterSharedData)
        {
            bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
            pkLogger = pkExporterSharedData->GetXMLLogger();
        }

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("AssetAnalyzerError", "The Asset Analyzer "
                "Process Plug-in could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Asset Analyzer Process Plug-in could not find "
                "the NiSceneGraphSharedData!","Scene Graph Shared Data "
                "Missing");
        }

        // Reset the converter
        NiImageConverter::SetImageConverter(spOldConverter);

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);

        spRoot->UpdateProperties();
        spRoot->UpdateEffects();
    }


    // Get the actual parameters for the NiPluginInfo parameter map.
    // These functions will assert in debug mode if the key doesn't exist or
    // if the value is not convertable to the correct primitive type.
    bool bFoundError = false;

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_TRIANGLE_COUNT))
    {
        bFoundError |= MaxTriangleCount(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_TRIANGLE_COUNT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_TEXTURE_SIZE))
    {
        bFoundError |= MaxTextureSize(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAX_TEXTURE_SIZE));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_SCENE_MEMORY_SIZE))
    {
        bFoundError |= MaxSceneMemorySize(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_SCENE_MEMORY_SIZE));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_TOTAL_TEXTURE_SIZE))
    {
        bFoundError |= MaxTotalTextureSize(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_TOTAL_TEXTURE_SIZE));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAX_OBJECT_COUNT))
    {
        bFoundError |= MaxObjectCount(
            pkInfo->GetInt(ASSET_ANALYZER_MAX_OBJECT_COUNT));
    }

    if (pkInfo->GetBool(
        ASSET_ANALYZER_USE_MINIMUM_TRIANGLE_TO_MESH_RATIO))
    {
        bFoundError |= MinTriangleMeshRatio(
            pkInfo->GetFloat(
                ASSET_ANALYZER_MINIMUM_TRIANGLE_TO_MESH_RATIO));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MULTI_SUB_OBJECT_WARNINGS))
    {
        bFoundError |= MultiSubObjectWarnings(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MULTI_SUB_OBJECT_WARNINGS));
    }

    if (pkInfo->GetBool(
        ASSET_ANALYZER_USE_MAXIMUM_MORPHING_VERTEX_PER_OBJECT))
    {
        bFoundError |= MaxMorphingVertexPerObject(NULL,
            pkInfo->GetInt(
            ASSET_ANALYZER_MAXIMUM_MORPHING_VERTEX_PER_OBJECT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT))
    {
        bFoundError |= MaxTriangleCountPerObject(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAXIMUM_TRIANGLE_COUNT_PER_OBJECT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAXIMUM_SCENE_GRAPH_DEPTH))
    {
        bFoundError |= MaxSceneGraphDepth(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAXIMUM_SCENE_GRAPH_DEPTH));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_MAXIMUM_LIGHTS_PER_OBJECT))
    {
        bFoundError |= MaxLightsPerObject(NULL,
            pkInfo->GetInt(ASSET_ANALYZER_MAXIMUM_LIGHTS_PER_OBJECT));
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_USE_REQUIRED_OBJECTS))
    {
        // Build the Require Object List
        NiTPointerList<NiString*>* pkObjectList =
            NiNew NiTPointerList<NiString*>;
        BuildRequireObjectList(pkObjectList, pkInfo);

        // Search for any Missing Object
        bFoundError |= RequiredObjects(NULL, pkObjectList);

        // Clean up
        while(!pkObjectList->IsEmpty())
        {
            NiDelete pkObjectList->RemoveHead();
        }
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_SPOT_LIGHTS))
    {
        bFoundError |= NoSpotLights(NULL);
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_POINT_LIGHTS))
    {
        bFoundError |= NoPointLights(NULL);
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_DIRECTIONAL_LIGHTS))
    {
        bFoundError |= NoDirectionalLights(NULL);
    }

    if (pkInfo->GetBool(ASSET_ANALYZER_NO_AMBIENT_LIGHTS))
    {
        bFoundError |= NoAmbientLights(NULL);
    }


    // Show any errors which have occured.
    if (bFoundError)
        ShowAllErrors();

    // Reset the converter
    NiImageConverter::SetImageConverter(spOldConverter);

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTriangleCount(NiUInt32 uiMaxTriangles)
{
    NiUInt32 uiTotal = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        uiTotal += RecursiveMaxTriangleCount(spRoot);
    }


    if (uiTotal > uiMaxTriangles)
    {
        NiString* pkString = NiNew NiString("MAX TRIANGLE COUNT");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tFound %d Triangles     Limit %d", uiTotal,
            uiMaxTriangles);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::RecursiveMaxTriangleCount(NiAVObject* pkObject)
{
    NiUInt32 uiTotal = 0;

    // Total up the number of Triangles
    if (NiIsKindOf(NiMesh, pkObject))
    {
        uiTotal = GetTriangleCount((NiMesh*)pkObject);
    }

    // Loop over all children 

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                uiTotal += RecursiveMaxTriangleCount(pkChild);
            }
        }
    }

    return uiTotal;
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::GetTriangleCount(NiMesh* pkMesh)
{
    return pkMesh->GetTotalPrimitiveCount();
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTextureSize(NiAVObject* pkObject, 
    NiUInt32 uiMaxTextureSize)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxTextureSize(spRoot, uiMaxTextureSize);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("MAX TEXTURE SIZE") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));

        }

        return bFound;
    }

    // Check to see if the object has a texturing property
    NiTexturingProperty* pkTextureProp = 
        (NiTexturingProperty*)pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkTextureProp != NULL)
    {

        bFound |= CheckTextureSize(pkTextureProp->GetBaseMap(), 
            uiMaxTextureSize, "BaseMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetDarkMap(), 
            uiMaxTextureSize, "DarkMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetDetailMap(),
            uiMaxTextureSize, "DetailMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetGlossMap(),
            uiMaxTextureSize, "GlossMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetGlowMap(),
            uiMaxTextureSize, "GlowMap", pkObject->GetName());

        bFound |= CheckTextureSize(pkTextureProp->GetBumpMap(),
            uiMaxTextureSize, "BumpMap", pkObject->GetName());

        NiUInt32 uiLoop = 0;
        for(; uiLoop < pkTextureProp->GetDecalMapCount(); uiLoop++)
        {
            bFound |= CheckTextureSize(pkTextureProp->GetDecalMap(uiLoop),
                uiMaxTextureSize, "DecalMap", pkObject->GetName());
        }

        for(uiLoop = 0; uiLoop < pkTextureProp->GetDecalMapCount(); uiLoop++)
        {
            bFound |= CheckTextureSize(pkTextureProp->GetShaderMap(uiLoop),
                uiMaxTextureSize, "ShaderMap", pkObject->GetName());
        }
    }



   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxTextureSize(pkChild, uiMaxTextureSize);
            }
        }
    }


    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::CheckTextureSize(NiTexturingProperty::Map* pkMap,
       NiUInt32 uiSize, const char* pcMapName, const char* pcObjectName)
{
    // Check the Map
    if (pkMap != NULL)
    {
        NiTexture* pkTexture = pkMap->GetTexture();

        if (pkTexture == NULL)
            return false;

        if ((pkTexture->GetWidth() > uiSize) ||
             (pkTexture->GetHeight() > uiSize) )
        {
            char acBuffer[512];


            NiSprintf(acBuffer, 512, "\tObject Name:: %s", pcObjectName);
            NiString* pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);

            NiSprintf(acBuffer, 512, "\tMap Name:: %s", pcMapName);
            pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);

            NiSprintf(acBuffer, 512, "\tSize:: %d/%d  Limit %d/%d", 
                pkTexture->GetWidth(), pkTexture->GetHeight(), uiSize, uiSize);
            pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);

            m_pkSectionErrors.AddTail(NiNew NiString(""));
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxSceneMemorySize(NiUInt32 uiMaxSceneMemorySize)
{
    NiUInt32 uiTotal = 0;
    NiTPointerList<NiTexture*> kTextureList;


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);

        // Collect All of the Textures
        RecursiveMaxTotalTextureSize(spRoot, kTextureList);

        uiTotal += RecursiveMaxSceneMemorySize(spRoot);
    }

    // Add in the total size of textures
    uiTotal += ComputeTotalTextureSize(kTextureList);



    if (uiTotal > uiMaxSceneMemorySize)
    {
        NiString* pkString = NiNew NiString("MAX SCENE MEMORY SIZE");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tMemory Size %d bytes    Limit %d bytes",
            uiTotal, uiMaxSceneMemorySize);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::RecursiveMaxSceneMemorySize(NiAVObject* pkObject)
{
    NiUInt32 uiTotal = 0;

    // Determine the Mesh size
    uiTotal += ComputeMeshDataSize(pkObject);

    // Determine the size of any animations
    uiTotal += ComputeAnimationDataSize(pkObject);


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                uiTotal += RecursiveMaxSceneMemorySize(pkChild);
            }
        }
    }

    return uiTotal;
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::ComputeMeshDataSize(NiAVObject* pkObject)
{
    NiUInt32 uiTotal = 0;

    // Determine the total size of the data in all the data streams on
    // the object
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;
        const NiUInt32 uiNumStreamRefs = pkMesh->GetStreamRefCount();
        
        for (NiUInt32 ui = 0; ui < uiNumStreamRefs; ui++)
        {
            const NiDataStreamRef* pkStreamRef =
                pkMesh->GetStreamRefAt(ui);
            if (!pkStreamRef)
                continue;
            const NiDataStream* pkDS = pkStreamRef->GetDataStream();
            if (!pkDS)
                continue;

            uiTotal += pkDS->GetSize();
        }
    }

    return uiTotal;
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::ComputeAnimationDataSize(NiAVObject* pkObject)
{
    NiUInt32 uiTotal = 0;

    if (!pkObject)
        return 0;

    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        uiTotal += ComputeAnimationDataSize(pkCtlr);
        pkCtlr = pkCtlr->GetNext();
    }

    NiPropertyList& kPropList = pkObject->GetPropertyList();
    NiTListIterator kIter = kPropList.GetHeadPos();
    while (kIter)
    {
        NiProperty* pkProp = kPropList.GetNext(kIter);
        if (pkProp)
        {
            NiTimeController* pkCtlr = pkProp->GetControllers();
            while (pkCtlr)
            {
                uiTotal += ComputeAnimationDataSize(pkCtlr);
                pkCtlr = pkCtlr->GetNext();
            }
        }
    }
    
    return uiTotal;
}

//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::ComputeAnimationDataSize(
    NiTimeController* pkCtlr)
{
    if (NiIsKindOf(NiInterpController, pkCtlr))
    {
        NiInterpController* pkInterpCtlr = (NiInterpController*) pkCtlr;
        for (NiUInt16 us = 0; us <  pkInterpCtlr->GetInterpolatorCount(); us++)
        {
            NiInterpolator* pkInterp = pkInterpCtlr->GetInterpolator(us);
            if (NiIsKindOf(NiKeyBasedInterpolator, pkInterp))
            {
                NiKeyBasedInterpolator* pkKeyInterp =
                    (NiKeyBasedInterpolator*) pkInterp;
                /*for (NiUInt16 usc = 0;
                    usc < pkKeyInterp->GetKeyChannelCount(); usc++)
                {
                    return pkKeyInterp->GetAllocatedSize(usc);
                }*/
                NiUInt16 usc = 0;
                return pkKeyInterp->GetAllocatedSize(usc);
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTotalTextureSize(NiUInt32 uiMaxTotalTextureSize)
{
    NiTPointerList<NiTexture*> kTextureList;


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        RecursiveMaxTotalTextureSize(spRoot, kTextureList);
    }

    NiUInt32 uiTotal = ComputeTotalTextureSize(kTextureList);

    if (uiTotal > uiMaxTotalTextureSize)
    {
        NiString* pkString = NiNew NiString("MAX SCENE TEXTURE SIZE");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tScene Texture Size %d bytes    Limit %d "
            "bytes", uiTotal, uiMaxTotalTextureSize);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::RecursiveMaxTotalTextureSize(NiAVObject* pkObject,
    NiTPointerList<NiTexture*>& kTextureList)
{

    // Check to see if the object has a texturing property
    NiTexturingProperty* pkTextureProp =
        (NiTexturingProperty*)pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkTextureProp != NULL)
    {

        InsertUniqueTexture(kTextureList, pkTextureProp->GetBaseMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetDarkMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetDetailMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetGlossMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetGlowMap());

        InsertUniqueTexture(kTextureList, pkTextureProp->GetBumpMap());

        unsigned int uiLoop = 0;
        for(; uiLoop < pkTextureProp->GetDecalMapCount(); uiLoop++)
        {
            InsertUniqueTexture(kTextureList,
                pkTextureProp->GetDecalMap(uiLoop));
        }

        for(uiLoop = 0; uiLoop < pkTextureProp->GetShaderArrayCount();
            uiLoop++)
        {
            if (pkTextureProp->GetShaderMap(uiLoop) != NULL) 
            {
                InsertUniqueTexture(kTextureList,
                    pkTextureProp->GetShaderMap(uiLoop));
            }
        }


        // Check for Flip Controllers
        NiTimeController* pkController = pkTextureProp->GetControllers();

        while (pkController != NULL)
        {
            NiFlipController* pkFlip = NiDynamicCast(NiFlipController,
                pkController);
            if (pkFlip != NULL)
            {
                for (unsigned int uiLoop = 0;
                    uiLoop < pkFlip->GetTextureArraySize(); uiLoop++)
                {
                    InsertUniqueTexture(kTextureList,
                        pkFlip->GetTextureAt(uiLoop));
                }
            }

            pkController = pkController->GetNext();
        }

    }
    
    // Check for A Texture Effect
    if (NiIsKindOf(NiTextureEffect, pkObject))
    {
        InsertUniqueTexture(kTextureList,
            ((NiTextureEffect*)pkObject)->GetEffectTexture());
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveMaxTotalTextureSize( pkChild, kTextureList);
            }
        }
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::InsertUniqueTexture(
    NiTPointerList<NiTexture*>& kTextureList, NiTexture* pkTexture)
{
    if (pkTexture != NULL)
    {
        
        // Scan the list to make sure the texture isn't already there
        NiTListIterator kIter = kTextureList.GetHeadPos();
        while (kIter != NULL)
        {
            // Quit if we found it
            if (kTextureList.GetNext(kIter) == pkTexture)
            {
                return;
            }
        }
        
        // Add to the list
        kTextureList.AddTail(pkTexture);
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::InsertUniqueTexture(
    NiTPointerList<NiTexture*>& kTextureList, NiTexturingProperty::Map* pkMap)
{
    if (pkMap != NULL)
    {
        NiTexture* pkTexture = pkMap->GetTexture();
        if (pkTexture != NULL)
        {
            InsertUniqueTexture(kTextureList, pkTexture);
        }
    }
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::ComputeTotalTextureSize(
    NiTPointerList<NiTexture*>& kTextureList)
{
    NiUInt32 uiTotal = 0;

    NiTListIterator kIter = kTextureList.GetHeadPos();

    // Loop though all of the textures
    while (kIter != NULL)
    {
        NiTexture* kTexture = kTextureList.GetNext(kIter);

        // Total up the size of source textures
        if (NiIsKindOf(NiSourceTexture, kTexture))
        {
            NiSourceTexture* pkSourceTexture = (NiSourceTexture*)kTexture;
            NiPixelData* pkData = pkSourceTexture->GetSourcePixelData();

            // Check for Pixel Data
            if (pkData != NULL)
            {
                uiTotal += (NiUInt32)pkData->GetTotalSizeInBytes();
            }
            else
            {
                // Attempt to Load the Pixel Data
                pkSourceTexture->LoadPixelDataFromFile();

                pkData = pkSourceTexture->GetSourcePixelData();

                // Check for Pixel Data
                if (pkData != NULL)
                {
                    uiTotal += (NiUInt32)pkData->GetTotalSizeInBytes();
                    pkSourceTexture->DestroyAppPixelData();
                }
                else
                {
                    NIASSERT(false);
                }
            }
        }
    }

    return uiTotal;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxObjectCount(NiUInt32 uiMaxObjectCount)
{
    NiUInt32 uiTotal = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        uiTotal += RecursiveMaxObjectCount(spRoot);
    }

    if (uiTotal > uiMaxObjectCount)
    {
        NiString* pkString = NiNew NiString("MAX OBJECT COUNT");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tObject Count %d     Limit %d", uiTotal,
            uiMaxObjectCount);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiUInt32 AssetAnalyzerPlugin::RecursiveMaxObjectCount(NiAVObject* pkObject)
{
    NiUInt32 uiTotal = 0;

    // Check for a mesh-based object
    if (NiIsKindOf(NiMesh, pkObject))
    {
        uiTotal++;
    }

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                uiTotal += RecursiveMaxObjectCount(pkChild);
            }
        }
    }

    return uiTotal;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MinTriangleMeshRatio(float fRatio)
{
    NiUInt32 uiTriangles = 0;
    NiUInt32 uiMeshes = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        RecursiveMinTriangleMeshRatio(spRoot, uiTriangles, uiMeshes);
    }


    float fTriangleToMesh = (float)uiTriangles / (float)uiMeshes;

    if (fTriangleToMesh < fRatio)
    {
        NiString* pkString = 
            NiNew NiString("MINIMUM TRIANGLE TO MESH RATIO");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tRatio Found %f     Limit %f", 
            fTriangleToMesh, fRatio);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::RecursiveMinTriangleMeshRatio(
    NiAVObject* pkObject, NiUInt32& uiTriangles, NiUInt32& uiMeshes)
{

    // Determine the number of triangle meshes drawing
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;
        NiUInt32 uiLocalMeshCount = 0;

        if (pkMesh->GetPrimitiveType() == NiPrimitiveType::PRIMITIVE_TRIANGLES)
        {
            uiLocalMeshCount = 1;
        }
        else if (pkMesh->GetPrimitiveType() == 
            NiPrimitiveType::PRIMITIVE_TRISTRIPS)
        {
            NiDataStreamRef* pkIndexDSRef = pkMesh->FindStreamRef(
                NiCommonSemantics::INDEX(), 0);
            if (!pkIndexDSRef)
                return;
            NiDataStream* pkIndexDS = pkIndexDSRef->GetDataStream();
            if (pkIndexDS)
                uiLocalMeshCount = pkIndexDS->GetRegionCount();
        }

        // Add in the number of triangles
        NiUInt32 uiLocalTriCount = GetTriangleCount(pkMesh);

        // Multiply counts by instancing active count
        if (pkMesh->GetInstanced())
        {
            NiUInt32 uiActive =
                NiInstancingUtilities::GetActiveInstanceCount(pkMesh);
            uiLocalMeshCount *= uiActive;
            uiLocalTriCount *= uiActive;
        }
            
        uiMeshes += uiLocalMeshCount;
        uiTriangles += uiLocalTriCount;
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveMinTriangleMeshRatio(pkChild, uiTriangles,
                    uiMeshes);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MinTriangleToStripRatio(float fRatio)
{
    NiUInt32 uiTriangles = 0;
    NiUInt32 uiStrips = 0;

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
        
        RecursiveMinTriangleToStripRatio(spRoot, uiTriangles, uiStrips);
    }


    float fVertexToStrip = (float)uiTriangles / (float)uiStrips;

    if (fVertexToStrip < fRatio)
    {
        NiString* pkString = NiNew NiString(
            "MINIMUM TRIANGLE TO STRIP RATIO");
        m_pkErrors.AddTail(pkString);

        char acBuffer[256];
        NiSprintf(acBuffer, 256, "\tRatio Found %f     Limit %f", 
            fVertexToStrip, fRatio);

        pkString = NiNew NiString(acBuffer);
        m_pkErrors.AddTail(pkString);
        m_pkErrors.AddTail(NiNew NiString(""));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::RecursiveMinTriangleToStripRatio(
    NiAVObject* pkObject, NiUInt32& uiTriangles, NiUInt32& uiStrips)
{
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;

        NiUInt32 uiLocalStripCount = 0;
        if (pkMesh->GetPrimitiveType() == NiPrimitiveType::PRIMITIVE_TRISTRIPS)
        {
            NiDataStreamRef* pkIndexDSRef = pkMesh->FindStreamRef(
                NiCommonSemantics::INDEX(), 0);
            if (!pkIndexDSRef)
                return;

            NiDataStream* pkIndexDS = pkIndexDSRef->GetDataStream();
            if (pkIndexDS)
                uiLocalStripCount = pkIndexDS->GetRegionCount();
        }

        // Add in the number of triangles
        NiUInt32 uiLocalTriCount = GetTriangleCount(pkMesh);

        // Multiply counts by instancing active count
        if (pkMesh->GetInstanced())
        {
            NiUInt32 uiActive =
                NiInstancingUtilities::GetActiveInstanceCount(pkMesh);
            uiLocalStripCount *= uiActive;
            uiLocalTriCount *= uiActive;
        }
            
        uiTriangles += uiLocalTriCount;
        uiStrips += uiLocalStripCount;
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveMinTriangleToStripRatio(pkChild, uiTriangles,
                    uiStrips);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MultiSubObjectWarnings(NiAVObject* pkObject, 
    NiUInt32 uiMaxSubObjects)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MultiSubObjectWarnings(spRoot, uiMaxSubObjects);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString("EXCESSIVE MULTI SUB OBJECTS"));
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


    // Check this object for Multi-Sub Objects
    bFound = CheckForMultiSubObject(pkObject, uiMaxSubObjects);

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MultiSubObjectWarnings(pkChild, uiMaxSubObjects);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::CheckForMultiSubObject(NiAVObject* pkObject, 
    NiUInt32 uiMaxSubObjects)
{
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode == NULL)
        return false;

    NiUInt32 uiSubObjectChildren = 0;

    // This is the Maya way of doing Multi Sub Objects
    char acBaseName[512] = "";

    // Count the Children with Name:Number
    for(NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiLoop); 

        if ((pkChild != NULL) &&
            NiIsKindOf(NiMesh, pkChild) &&
            (pkChild->GetName().Exists()) &&
            (pkNode->GetName().Exists())) 
        {

            // Find the possible base name
            if (!strcmp(acBaseName, ""))
            {
                // Clear off the : and end of the string
                if (strstr(pkChild->GetName(), ":") != NULL)
                {
                    NiStrcpy(acBaseName, 512, pkChild->GetName());
                    
                    char* pcTemp = strstr(acBaseName, ":");
                    *pcTemp = '\0';
                }
            }

            // Node Name is at the front
            if (strstr(pkChild->GetName(), acBaseName) == (NiString) 
                pkChild->GetName())
            {
                const char* pcTemp = ((const char*)pkChild->GetName())
                    + strlen(acBaseName);

                // Check for the :
                if (*pcTemp++ == ':')
                {

                    // Make sure the rest of the string is a number
                    NiString kNumberString(pcTemp);
                    int iInt;

                    if (kNumberString.ToInt(iInt))
                    {
                        uiSubObjectChildren++;
                    }
                }
            }
        }
    }

    if (uiSubObjectChildren > uiMaxSubObjects)
    {
        char acBuffer[512];
    
        NiSprintf(acBuffer, 512, "\tObject:: %s    Has %d SubObjects    "
            "Limit %d", pkNode->GetName(), uiSubObjectChildren,
            uiMaxSubObjects);
        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxMorphingVertexPerObject(NiAVObject* pkObject, 
    NiUInt32 uiMaxVertexes)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxMorphingVertexPerObject(spRoot, uiMaxVertexes);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString(
                "MAX MORPHING VERTEX PER OBJECT"));
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


    // Check for a Morpher Controller
    NiMorphWeightsController* pkMorph =
        (NiMorphWeightsController*)pkObject->
        GetController(&NiMorphWeightsController::ms_RTTI);

    if (pkMorph != NULL)
    {
        if (NiIsKindOf(NiMesh, pkObject))
        {
            NiMesh* pkMesh = (NiMesh*)pkObject;

            if (pkMesh->GetVertexCount() > uiMaxVertexes)
            {
                char acBuffer[512];
                
                NiSprintf(acBuffer, 512, "\tMorphing Object:: %s    Has %d "
                    "Vertices    Limit %d", pkObject->GetName(), 
                    pkMesh->GetVertexCount(), uiMaxVertexes );
                NiString* pkString = NiNew NiString(acBuffer);
                m_pkSectionErrors.AddTail(pkString);
                bFound = true;
            }
        }
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxMorphingVertexPerObject(pkChild, uiMaxVertexes);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxTriangleCountPerObject(NiAVObject* pkObject, 
    NiUInt32 uiMaxTriangles)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (unsigned int ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxTriangleCountPerObject(spRoot, uiMaxTriangles);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString(
                "MAX TRIANGLE COUNT PER OBJECT"));
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


    // Add in the number of triangles
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;

        NiUInt32 uiTriangles = GetTriangleCount(pkMesh);

        if (uiTriangles > uiMaxTriangles)
        {
            char acBuffer[512];
            
            NiSprintf(acBuffer, 512, "\tObject:: %s    Has %d Triangles    "
                "Limit %d", pkObject->GetName(), uiTriangles, uiMaxTriangles);
            NiString* pkString = NiNew NiString(acBuffer);
            m_pkSectionErrors.AddTail(pkString);
            bFound = true;
        }
    }



    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxTriangleCountPerObject(pkChild, uiMaxTriangles);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxSceneGraphDepth(NiAVObject* pkObject, 
    NiUInt32 uiMaxDepth)
{
    // Return if we have gotten down far enough
    if (uiMaxDepth == 0)
        return true;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        bool bFound = false;

        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxSceneGraphDepth(spRoot, uiMaxDepth);
        }

        if (bFound)
        {
            m_pkErrors.AddTail(NiNew NiString("MAX SCENE GRAPH DEPTH")); 

            char acBuffer[256];
            NiSprintf(acBuffer, 256, "\tExceeds Depth Limit %d", uiMaxDepth);
            m_pkErrors.AddTail(NiNew NiString(acBuffer));
            m_pkErrors.AddTail(NiNew NiString(""));
        }

        return bFound;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if ((pkChild != NULL) &&
                MaxSceneGraphDepth(pkChild, uiMaxDepth - 1))
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::MaxLightsPerObject(NiAVObject* pkObject, 
    NiUInt32 uiMaxLights)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= MaxLightsPerObject(spRoot, uiMaxLights);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("MAX LIGHTS PER OBJECT") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }


    NiMesh* pkMesh = NiDynamicCast(NiMesh, pkObject);
    if (pkMesh != NULL)
    {
        // Count the attached lights
        NiUInt32 uiLights = 0;
        NiDynamicEffectState* pkState = pkMesh->GetEffectState();

        if (pkState != NULL)
        {
            for (NiDynEffectStateIter kCurrLight = pkState->GetLightHeadPos();
                kCurrLight != NULL; pkState->GetNextLight(kCurrLight))
            {
                uiLights++;
            }
        }

        if (uiLights > uiMaxLights)
        {
            char acBuffer[256];
            NiSprintf(acBuffer, 256, "\tObject %s \tHas %d Lights \tLimit %d",
                pkObject->GetName(), uiLights, uiMaxLights);
            m_pkSectionErrors.AddTail(NiNew NiString(acBuffer));
            m_pkSectionErrors.AddTail(NiNew NiString(""));
            bFound = true;
        }
    }

   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= MaxLightsPerObject(pkChild, uiMaxLights);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::RequiredObjects(NiAVObject* pkObject, 
    NiTPointerList<NiString*>* pkObjectList)
{
    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            RequiredObjects(spRoot, pkObjectList);
        }

        if (pkObjectList->IsEmpty())
            return false;

        m_pkErrors.AddTail( NiNew NiString("MISSING REQUIRED OBJECTS") );

        // Loop through all of the object not found
        NiTListIterator kIter = pkObjectList->GetHeadPos();
        while(kIter != NULL)
        {
            NiString* kString = NiNew NiString("\t");
            *kString += *pkObjectList->GetNext(kIter);
            m_pkErrors.AddTail(kString);
        }

        m_pkErrors.AddTail(NiNew NiString(""));
        

        return true;
    }



    // Look to see if it is in the list
    if (pkObject->GetName().Exists())
    {
        NiTListIterator kIter = pkObjectList->GetHeadPos();
        while(kIter != NULL)
        {
            // check for a matching name
            if ((*pkObjectList->Get(kIter)) == pkObject->GetName())
            {
                // Remove from the list and quit looking
                pkObjectList->RemovePos(kIter);
                kIter = NULL;
            }
            else
            {
                // Increment the iterator
                kIter = pkObjectList->GetNextPos(kIter);
            }
        }
    }

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RequiredObjects(pkChild, pkObjectList);

                // Quit because we are out of stuff to find
                if (pkObjectList->IsEmpty())
                    return false;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::BuildRequireObjectList(
    NiTPointerList<NiString*>* pkObjectList, const NiPluginInfo* pkInfo)
{

    NiUInt32 uiNumRequiredObjects = pkInfo->GetInt(
        ASSET_ANALYZER_NUMBER_OF_REQUIRED_OBJECTS);
    
    for (NiUInt32 uiLoop = 0; uiLoop < uiNumRequiredObjects; uiLoop++)
    {
        // Create the Key
        NiString kKey(ASSET_ANALYZER_REQUIRED_OBJECTS);
        kKey += NiString::FromInt(uiLoop);

        // Read the Value
        NiString* pkValue = NiNew NiString(pkInfo->GetValue(kKey));

        pkObjectList->AddTail(pkValue);
    }

}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoSpotLights(NiAVObject* pkObject)
{
    bool bFound = false;


    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoSpotLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO SPOT LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }


    // Check for a light
    if (NiIsExactKindOf(NiSpotLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiSpotLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiSpotLight:: ** NO NAME **", 
                pkObject->GetName());
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoSpotLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoPointLights(NiAVObject* pkObject)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoPointLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO POINT LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }

    // Check for a light
    if (NiIsExactKindOf(NiPointLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiPointLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiPointLight:: ** NO NAME **", 
                pkObject->GetName());
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoPointLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoDirectionalLights(NiAVObject* pkObject)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoDirectionalLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO DIRECTIONAL LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }

    // Check for a light
    if (NiIsExactKindOf(NiDirectionalLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiDirectionalLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiDirectionalLight:: ** NO NAME **",
                pkObject->GetName());
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoDirectionalLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool AssetAnalyzerPlugin::NoAmbientLights(NiAVObject* pkObject)
{
    bool bFound = false;

    // Start the Recursion.
    if (pkObject == NULL)
    {
        // While 3ds max and Maya by default only generate one root, other 
        // plug-ins in the pipeline could potentially add new roots.
        for (NiUInt32 ui = 0; ui < m_pkSGSharedData->GetRootCount(); ui++)
        {
            NiNodePtr spRoot = m_pkSGSharedData->GetRootAt(ui);
            
            bFound |= NoAmbientLights(spRoot);
        }

        if (bFound)
        {
            m_pkErrors.AddTail( NiNew NiString("NO AMBIENT LIGHTS") );
            AppendAllSectionErrors();
            m_pkErrors.AddTail(NiNew NiString(""));
         }

        return bFound;
    }

    // Check for a light
    if (NiIsExactKindOf(NiAmbientLight, pkObject)) 
    {
        char acBuffer[512];
        
        if (pkObject->GetName().Exists())
        {
            NiSprintf(acBuffer, 512, "\tNiAmbientLight:: %s", 
                pkObject->GetName());
        }
        else
        {
            NiSprintf(acBuffer, 512, "\tNiAmbientLight:: ** NO NAME **");
        }

        NiString* pkString = NiNew NiString(acBuffer);
        m_pkSectionErrors.AddTail(pkString);
        bFound = true;
    }


   // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for (NiUInt32 uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                bFound |= NoAmbientLights(pkChild);
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::ClearAllErrors()
{
    // remove all errors deleting the strings
    while (!m_pkErrors.IsEmpty())
    {
        NiDelete m_pkErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::ClearAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        NiDelete m_pkSectionErrors.RemoveHead();
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::AppendAllSectionErrors()
{
    // remove all errors deleting the strings
    while (!m_pkSectionErrors.IsEmpty())
    {
        m_pkErrors.AddTail( m_pkSectionErrors.RemoveHead() );;
    }
}
//---------------------------------------------------------------------------
void AssetAnalyzerPlugin::ShowAllErrors()
{
    if (m_pkErrors.IsEmpty())
        return;
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if (bWriteResultsToLog && pkLogger)
    {
        // Convert to a single string
        NiString kErrorText;
        NiTListIterator kIter = m_pkErrors.GetHeadPos();

        while (kIter != NULL)
        {
            // Append into the Text String
            kErrorText += *m_pkErrors.GetNext(kIter);
            kErrorText += "\r\n";
        }

        pkLogger->LogElement("AssetAnalyzerResults", kErrorText);
    }
    else
    {
        AssetAnalyzerResultsDialog kDialog(m_hModule, NULL, &m_pkErrors );

        kDialog.DoModal();
    }

    ClearAllErrors();
    ClearAllSectionErrors();
}
//---------------------------------------------------------------------------
