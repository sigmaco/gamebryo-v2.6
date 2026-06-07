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
#include "NiPhysXMeshConverterPlugin.h"

#include <CookedMeshReader.h>

// RTTI implementation macro.
NiImplementRTTI(NiPhysXMeshConverterPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiPhysXMeshConverterPlugin::NiPhysXMeshConverterPlugin() : NiPlugin(
    "PhysX Mesh Converter Plug-in",          // name
    "1.0",                              // version
    "Converts previous versions of PhysX collision meshes",
    "Converts PhysX collision meshes from versions prior to 2.7.0 to\n"
    "the format found in 2.7.0 and later. This tool plug-in is designed\n"
    "to be used with the ToolPluginBatch application to convert old\n"
    "NIF files, and should not be used when exporting content directly\n"
    "from an art tool."), m_kConvertedMap(131)
{
}
//---------------------------------------------------------------------------
NiPhysXMeshConverterPlugin::~NiPhysXMeshConverterPlugin()
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NiPhysXMeshConverterPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiPhysXMeshConverterPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return false;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::DoManagementDialog(
    NiPluginInfo*, NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiPhysXMeshConverterPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiPhysXMeshConverterPlugin::Execute(
    const NiPluginInfo*)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    // Get the exporter options shared data
    NiExporterOptionsSharedData* pkEOSharedData =
        (NiExporterOptionsSharedData*)pkDataList->Get(
        NiGetSharedDataType(NiExporterOptionsSharedData));
    if (!pkEOSharedData)
    {
        // Nothing to do, no extra objects in the NIF.
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    NiXMLLogger* pkLogger = 0;
    if (pkEOSharedData->GetWriteResultsToLog())
    {
        pkLogger = pkEOSharedData->GetXMLLogger();
    }

    // Get the extra objects shared data.
    NiExtraObjectsSharedData* pkExtraSharedData = (NiExtraObjectsSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiExtraObjectsSharedData));

    if (!pkExtraSharedData)
    {
        // Nothing to do, no extra objects in the NIF.
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
    }

    // Prepare for mesh cooking
    NxCookingInterface* pkCookingInterface =
        NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
    NIASSERT(pkCookingInterface != 0);

    NiPhysXAllocator* pkAllocator = NiNew NiPhysXAllocator;
    NiPhysXUserOutput* pkOutput = NiNew NiPhysXUserOutput;
    pkCookingInterface->NxInitCooking(pkAllocator, pkOutput);

    NxCookingParams kCookingParams;
    switch(NiSystemDesc::GetSystemDesc().GetToolModeRendererID())
    {
    case NiSystemDesc::RENDERER_XENON:
        kCookingParams.targetPlatform = PLATFORM_XENON;
        break;
    case NiSystemDesc::RENDERER_PS3:
        kCookingParams.targetPlatform = PLATFORM_PLAYSTATION3;
        break;
    case NiSystemDesc::RENDERER_GENERIC:
    case NiSystemDesc::RENDERER_DX9:
    case NiSystemDesc::RENDERER_D3D10:
        kCookingParams.targetPlatform = PLATFORM_PC;
        break;
    }
    pkCookingInterface->NxSetCookingParams(kCookingParams);

    NiUInt32 uiCount = pkExtraSharedData->GetObjectCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiObject* pkObject = pkExtraSharedData->GetObjectAt(ui);
        
        if (!pkObject)
            continue;
            
        if (NiIsKindOf(NiPhysXProp, pkObject))
        {
            NiPhysXProp* pkProp = NiDynamicCast(NiPhysXProp, pkObject);
            NIASSERT(pkProp);

            if (!ProcessProp(pkProp, pkCookingInterface, pkLogger))
            {
                pkCookingInterface->NxCloseCooking();
                return NiNew
                    NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
            }        
        }
    }

    pkCookingInterface->NxCloseCooking();

    NiDelete pkAllocator;
    NiDelete pkOutput;

    // Return success.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::ProcessProp(NiPhysXProp* pkProp,
    NxCookingInterface* pkCookingInterface, NiXMLLogger* pkLogger)
{
    NiPhysXPropDesc* pkPropDesc = pkProp->GetSnapshot();

    if (pkPropDesc)
    {
        // Work through all actors, looking for mesh shapes.
        NiUInt32 uiActorCount = pkPropDesc->GetActorCount();
        for (NiUInt32 ui = 0; ui < uiActorCount; ui++)
        {
            NiPhysXActorDesc* pkActorDesc = pkPropDesc->GetActorAt(ui);
            if (!pkActorDesc)
                continue;
                
            if (!ProcessActor(pkActorDesc, pkCookingInterface, pkLogger))
                return false;
        }
        
        // Work through cloth
        NiUInt32 uiClothCount = pkPropDesc->GetClothCount();
        for (NiUInt32 ui = 0; ui < uiClothCount; ui++)
        {
            NiPhysXClothDesc* pkClothDesc = pkPropDesc->GetClothAt(ui);
            
            if (!pkClothDesc)
                continue;
                
            NiPhysXMeshDesc* pkMeshDesc = pkClothDesc->GetMeshDesc();
            if (!ConvertCloth(pkMeshDesc, pkLogger))
            {
                return false;
            }
        }
    }
    
    // Check for particle system actors
    if (NiIsKindOf(NiPhysXPSParticleSystemProp, pkProp))
    {
        NiPhysXPSParticleSystemProp* pkPSysProp =
            (NiPhysXPSParticleSystemProp*)pkProp;
        for (NiUInt32 ui = 0; ui < pkPSysProp->GetSystemCount(); ui++)
        {
            NiPhysXPSParticleSystem* pkPSys = pkPSysProp->GetSystemAt(ui);
            if (pkPSys)
            {
                NiPhysXActorDesc* pkActorDesc = pkPSys->GetActorDesc();
                if (!ProcessActor(pkActorDesc, pkCookingInterface, pkLogger))
                    return false;
            }
        }
    }
    if (NiIsKindOf(NiPhysXPSMeshParticleSystemProp, pkProp))
    {
        NiPhysXPSMeshParticleSystemProp* pkPSysProp =
            (NiPhysXPSMeshParticleSystemProp*)pkProp;
        for (NiUInt32 ui = 0; ui < pkPSysProp->GetSystemCount(); ui++)
        {
            NiPhysXPSMeshParticleSystem* pkPSys = pkPSysProp->GetSystemAt(ui);
            if (pkPSys)
            {
                for (NiUInt32 uj = 0; uj < pkPSys->GetActorDescCount(); uj++)
                {
                    NiPhysXActorDesc* pkActorDesc = pkPSys->GetActorDescAt(uj);
                    if (!ProcessActor(pkActorDesc, pkCookingInterface,
                        pkLogger))
                    {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::ProcessActor(NiPhysXActorDesc* pkActorDesc,
    NxCookingInterface* pkCookingInterface, NiXMLLogger* pkLogger)
{
    NiTObjectArray<NiPhysXShapeDescPtr>& kShapesArray =
        pkActorDesc->GetActorShapes();

    NiUInt32 uiShapeCount = kShapesArray.GetSize();
    for (NiUInt32 uj = 0; uj < uiShapeCount; uj++)
    {
        NiPhysXShapeDesc* pkShapeDesc = kShapesArray.GetAt(uj);
        if (!pkShapeDesc)
            continue;
        
        if (pkShapeDesc->GetType() == NX_SHAPE_MESH ||
            pkShapeDesc->GetType() == NX_SHAPE_CONVEX)
        {
            NiPhysXMeshDesc* pkMeshDesc = pkShapeDesc->GetMeshDesc();
            NIASSERT(pkMeshDesc);
            
            if (!ConvertMesh(pkMeshDesc, pkCookingInterface, pkLogger))
            {
                return false;
            }
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::ConvertMesh(NiPhysXMeshDesc* pkMeshDesc,
    NxCookingInterface* pkCookingInterface, NiXMLLogger*)
{
    NiUInt8* pkData = 0;
    size_t uiDataSize = 0;
    pkMeshDesc->GetData(uiDataSize, &pkData);
    
    // Thanks to the way that the PhysX converter works, we need to create
    // a file that it can then read. DO it inside local scope to close
    // the file when we're done.
    {
        NiFile kOutFile("MeshConverterData.tmp", NiFile::WRITE_ONLY);
        
        NIASSERT(kOutFile);
        
        kOutFile.BinaryWrite((void*)pkData, (unsigned int)uiDataSize, 0);
    }

    CmMeshData kMeshDesc;
    CmMeshType kMeshType;
    bool bHintCollisionSpeed;
    if (!GetCookedData("MeshConverterData.tmp", kMeshType, kMeshDesc,
        bHintCollisionSpeed))
    {
        return false;
    }

    NxCookingParams kCookingParams = pkCookingInterface->NxGetCookingParams();
    kCookingParams.hintCollisionSpeed = bHintCollisionSpeed;
    pkCookingInterface->NxSetCookingParams(kCookingParams);

    bool bResult = true;
    NiPhysXMemStream* pkCookStream = NiNew NiPhysXMemStream();

    switch (kMeshType)
    {
        case MT_TRIANGLE_MESH:
        {
            NxTriangleMeshDesc kTriMeshDesc;
                
            kTriMeshDesc.numVertices = kMeshDesc.numVertices;
            kTriMeshDesc.numTriangles = kMeshDesc.numTriangles;
            kTriMeshDesc.pointStrideBytes = kMeshDesc.pointStrideBytes;
            kTriMeshDesc.triangleStrideBytes = kMeshDesc.triangleStrideBytes;
            kTriMeshDesc.points    = kMeshDesc.points;
            kTriMeshDesc.triangles = kMeshDesc.triangles;
            kTriMeshDesc.flags = kMeshDesc.flags;

            kTriMeshDesc.materialIndexStride = kMeshDesc.materialIndexStride;
            kTriMeshDesc.materialIndices = kMeshDesc.materialIndices;
            kTriMeshDesc.heightFieldVerticalAxis =
                (NxHeightFieldAxis)kMeshDesc.heightFieldVerticalAxis;
            kTriMeshDesc.heightFieldVerticalExtent =
                kMeshDesc.heightFieldVerticalExtent;
            kTriMeshDesc.convexEdgeThreshold = kMeshDesc.convexEdgeThreshold;

            // cook mesh again to get new tree format
            bResult = pkCookingInterface->NxCookTriangleMesh(kTriMeshDesc,
                *pkCookStream);
        } break;
        
        case MT_CONVEX_MESH:
        {
            NxConvexMeshDesc kConvMeshDesc;

            kConvMeshDesc.numVertices = kMeshDesc.numVertices;
            kConvMeshDesc.numTriangles = kMeshDesc.numTriangles;
            kConvMeshDesc.pointStrideBytes = kMeshDesc.pointStrideBytes;
            kConvMeshDesc.triangleStrideBytes = kMeshDesc.triangleStrideBytes;
            kConvMeshDesc.points = kMeshDesc.points;
            kConvMeshDesc.triangles    = kMeshDesc.triangles;
            kConvMeshDesc.flags    = kMeshDesc.flags;

            // cook mesh again to get new tree format
            bResult = pkCookingInterface->NxCookConvexMesh(kConvMeshDesc,
                *pkCookStream);
        } break;
    }
    
    if (bResult)
    {
        pkMeshDesc->SetData(pkCookStream->GetSize(),
            (NiUInt8*)pkCookStream->GetBuffer());
    }
    
    NiDelete pkCookStream;

    return bResult;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshConverterPlugin::ConvertCloth(NiPhysXMeshDesc* pkMeshDesc,
    NiXMLLogger* pkLogger)
{
    NiUInt8* pkData = 0;
    size_t uiDataSize = 0;
    pkMeshDesc->GetData(uiDataSize, &pkData);

    NiUInt32 uiOldPhysXVersion = (2 << 24) + (6 << 16) + (4 << 8);
    NiUInt32 uiNewVersion = 3;
    NiUInt32 uiDeformableType = 1;
    float fWeldDistance = 0.0f;
    NiUInt8 uiMismatch = pkData[3] & 1;
    
    char* pcOldVersionAsChar = (char*)&uiOldPhysXVersion;
    char* pcNewVersionAsChar = (char*)&uiNewVersion;
    char* pcDeformableAsChar = (char*)&uiDeformableType;
    char* pcWeldDistanceAsChar = (char*)&fWeldDistance;
    if (NiSystemDesc::GetSystemDesc().IsLittleEndian() && (uiMismatch == 0))
    {
        // Target endian not same as this platform. Flip
        NiEndian::Swap32(pcOldVersionAsChar, 1);
        NiEndian::Swap32(pcNewVersionAsChar, 1);
        NiEndian::Swap32(pcDeformableAsChar, 1);
        NiEndian::Swap32(pcWeldDistanceAsChar, 1);
    }
    
    if (pkData[8] != ((unsigned char*)pcOldVersionAsChar)[0] ||
        pkData[9] != ((unsigned char*)pcOldVersionAsChar)[1] ||
        pkData[10] != ((unsigned char*)pcOldVersionAsChar)[2] ||
        pkData[11] != ((unsigned char*)pcOldVersionAsChar)[3])
    {
        char pcMsg[1024];
        NiSprintf(pcMsg, 1024, "NiPhysXMeshConverterPlugin::ConvertCloth: "
            "Expected to find stashed mesh version for Gamebryo 2.3.0.1, or "
            "PhysX 2.6.4.");
        if (pkLogger)
        {
            pkLogger->LogElement("NiPhysXMeshConverterPlugin", pcMsg);
        }
        else
        {
            NiMessageBox(pcMsg, "PhysX Mesh Converter Error");
        }
        return false;
    }
    
    NiUInt8* pkNewData = NiAlloc(NiUInt8, uiDataSize + 8);
    
    NiUInt32 uiPosn = 0;
    for (; uiPosn < 8; uiPosn++)
    {
        pkNewData[uiPosn] = pkData[uiPosn];
    }
    pkNewData[uiPosn++] = ((NiUInt8*)pcNewVersionAsChar)[0];
    pkNewData[uiPosn++] = ((NiUInt8*)pcNewVersionAsChar)[1];
    pkNewData[uiPosn++] = ((NiUInt8*)pcNewVersionAsChar)[2];
    pkNewData[uiPosn++] = ((NiUInt8*)pcNewVersionAsChar)[3];
    pkNewData[uiPosn++] = ((NiUInt8*)pcDeformableAsChar)[0];
    pkNewData[uiPosn++] = ((NiUInt8*)pcDeformableAsChar)[1];
    pkNewData[uiPosn++] = ((NiUInt8*)pcDeformableAsChar)[2];
    pkNewData[uiPosn++] = ((NiUInt8*)pcDeformableAsChar)[3];
    for (NiUInt32 ui = 0; ui < 4; ui++)
    {
        pkNewData[uiPosn++] = pkData[uiPosn - 4];
    }
    pkNewData[uiPosn++] = ((NiUInt8*)pcWeldDistanceAsChar)[0];
    pkNewData[uiPosn++] = ((NiUInt8*)pcWeldDistanceAsChar)[1];
    pkNewData[uiPosn++] = ((NiUInt8*)pcWeldDistanceAsChar)[2];
    pkNewData[uiPosn++] = ((NiUInt8*)pcWeldDistanceAsChar)[3];
    for (; uiPosn < uiDataSize + 8; uiPosn++)
    {
        pkNewData[uiPosn] = pkData[uiPosn - 8];
    }

    pkMeshDesc->SetData(uiDataSize + 8, pkNewData);

    return true;
}
//---------------------------------------------------------------------------
