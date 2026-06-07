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
#include "NiPhysXCleanerPlugin.h"
#include "NiPhysXCleanerPluginDefines.h"

// RTTI implementation macro.
NiImplementRTTI(NiPhysXCleanerPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiPhysXCleanerPlugin::NiPhysXCleanerPlugin() : NiPlugin(
    "PhysX Cleaner Plug-in",          // name
    "2.0",                              // version
    "PhysX content removal plug-in", // Short description, next is long
    "Removes all the PhysX extra data in a scene graph.")
{
}
//---------------------------------------------------------------------------
NiPhysXCleanerPlugin::~NiPhysXCleanerPlugin()
{
}
//---------------------------------------------------------------------------
NiPluginInfo* NiPhysXCleanerPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiPhysXCleanerPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiPhysXCleanerPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return false;
}
//---------------------------------------------------------------------------
bool NiPhysXCleanerPlugin::DoManagementDialog(NiPluginInfo*,
    NiWindowRef)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiPhysXCleanerPlugin::CanExecute(NiPluginInfo* pkInfo)
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

    if (pkInfo->GetClassName() == "NiPhysXCleanerPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiPhysXCleanerPlugin::Execute(
    const NiPluginInfo*)
{
    m_pkLogger = 0;
    
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    // Set up for logging errors
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    if (pkExporterSharedData)
    {
        if (pkExporterSharedData->GetWriteResultsToLog())
            m_pkLogger = pkExporterSharedData->GetXMLLogger();
    }
    
    // Get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXCleanerError",
                "The PhysX Cleaner Plug-in could not find the "
                "NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The PhysX Cleaner Plug-in could not find the "
                "NiSceneGraphSharedData!","Scene Graph Shared Data Missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Find the scene root node. We could later replace this with a named
    // node or other method of specifying a PhysX scene root.
    NiNodePtr spSGAll = pkSGSharedData->GetFullSceneGraph();
    NiNode* pkSceneRoot = (NiNode*)spSGAll->GetObjectByName("Scene Root");
    if (!pkSceneRoot)
    {
        pkSceneRoot = (NiNode*)spSGAll->GetObjectByName("SceneNode");
    }
    spSGAll = 0;

    if (!pkSceneRoot)
    {
        if (m_pkLogger)
        {
            m_pkLogger->LogElement("PhysXCleanerError",
                "The PhysX Cleaner Plug-in could not find the "
                "scene root node.");
        }
        else
        {
            NiMessageBox("The PhysX Cleaner Plug-in could not find the "
                "scene root node.", "Missing scene root");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }
    
    // Do the work
    RemoveED(pkSceneRoot);

    // Return success.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void NiPhysXCleanerPlugin::RemoveED(NiAVObject* pkAVObj)
{
    if (pkAVObj->RemoveExtraData(ED_ACTOR_TYPE))
    {
        // If we have this, we might have many others
        pkAVObj->RemoveExtraData(ED_MASS);
        pkAVObj->RemoveExtraData(ED_DENSITY);
        pkAVObj->RemoveExtraData(ED_LINEAR_VELOCITY);
        pkAVObj->RemoveExtraData(ED_ANGULAR_VELOCITY);
        pkAVObj->RemoveExtraData(ED_DISABLE_GRAVITY);
        pkAVObj->RemoveExtraData(ED_LINEAR_DAMPING);
        pkAVObj->RemoveExtraData(ED_ANGULAR_DAMPING);
        pkAVObj->RemoveExtraData(ED_SOLVER_ITERATIONS);
        pkAVObj->RemoveExtraData(ED_ACTOR_GROUP);
        pkAVObj->RemoveExtraData(ED_PROXY_TARGET);
        pkAVObj->RemoveExtraData(ED_PARTICLE_ACTOR);
        pkAVObj->RemoveExtraData(ED_CLOTH_PRESSURE);
        pkAVObj->RemoveExtraData(ED_CLOTH_STATIC);
        pkAVObj->RemoveExtraData(ED_CLOTH_DISABLE_COLL);
        pkAVObj->RemoveExtraData(ED_CLOTH_SELF_COLLIDE);
        pkAVObj->RemoveExtraData(ED_CLOTH_GRAVITY);
        pkAVObj->RemoveExtraData(ED_CLOTH_BENDING);
        pkAVObj->RemoveExtraData(ED_CLOTH_ORTHO_BEND);
        pkAVObj->RemoveExtraData(ED_CLOTH_DAMPING);
        pkAVObj->RemoveExtraData(ED_CLOTH_TWO_COLLIDE);
        pkAVObj->RemoveExtraData(ED_CLOTH_TRI_COLLIDE);
        pkAVObj->RemoveExtraData(ED_CLOTH_TEARABLE);
        pkAVObj->RemoveExtraData(ED_CLOTH_HARDWARE);
        pkAVObj->RemoveExtraData(ED_CLOTH_THICKNESS);
        pkAVObj->RemoveExtraData(ED_CLOTH_DENSITY);
        pkAVObj->RemoveExtraData(ED_CLOTH_BEND_STIFF);
        pkAVObj->RemoveExtraData(ED_CLOTH_STR_STIFF);
        pkAVObj->RemoveExtraData(ED_CLOTH_DAMPING_COEF);
        pkAVObj->RemoveExtraData(ED_CLOTH_FRICTION);
        pkAVObj->RemoveExtraData(ED_CLOTH_PRESSURE_VAL);
        pkAVObj->RemoveExtraData(ED_CLOTH_TEAR_FACTOR);
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_TEAR);
        pkAVObj->RemoveExtraData(ED_CLOTH_COLLIDE_RESP);
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_RESP);
        pkAVObj->RemoveExtraData(ED_CLOTH_EXT_ACCEL);
        pkAVObj->RemoveExtraData(ED_CLOTH_WAKE_COUNTER);
        pkAVObj->RemoveExtraData(ED_CLOTH_SLEEP_VEL);
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACHMENT);
        pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_POSN);
        pkAVObj->RemoveExtraData(ED_CLOTH_UPDATE_NBT);

    }

    if (pkAVObj->RemoveExtraData(ED_JOINT_CHILD))
    {
        pkAVObj->RemoveExtraData(ED_JOINT_POSN1);
        pkAVObj->RemoveExtraData(ED_JOINT_AXIS1);
        pkAVObj->RemoveExtraData(ED_JOINT_NORMAL1);
        pkAVObj->RemoveExtraData(ED_JOINT_POSN2);
        pkAVObj->RemoveExtraData(ED_JOINT_AXIS2);
        pkAVObj->RemoveExtraData(ED_JOINT_NORMAL2);
        pkAVObj->RemoveExtraData(ED_JOINT_LOCKS);
        pkAVObj->RemoveExtraData(ED_JOINT_LIMITS);
        pkAVObj->RemoveExtraData(ED_JOINT_RESTITUTIONS);
        pkAVObj->RemoveExtraData(ED_JOINT_SPRINGS);
        pkAVObj->RemoveExtraData(ED_JOINT_DAMPINGS);
        pkAVObj->RemoveExtraData(ED_JOINT_MAX_FORCE);
        pkAVObj->RemoveExtraData(ED_JOINT_MAX_TORQUE);
        pkAVObj->RemoveExtraData(ED_JOINT_NAME);
        pkAVObj->RemoveExtraData(ED_JOINT_COLLIDES);
        pkAVObj->RemoveExtraData(ED_JOINT_PROJ_MODE);
        pkAVObj->RemoveExtraData(ED_JOINT_PROJ_DIST);
        pkAVObj->RemoveExtraData(ED_JOINT_PROJ_ANGLE);
        pkAVObj->RemoveExtraData(ED_JOINT_GEARING);
        pkAVObj->RemoveExtraData(ED_JOINT_GEAR_RATIO);
        pkAVObj->RemoveExtraData(ED_MAX_JOINT_POSN);
        pkAVObj->RemoveExtraData(ED_MAX_JOINT_AXIS);
        pkAVObj->RemoveExtraData(ED_MAX_JOINT_NORMAL);
        pkAVObj->RemoveExtraData(ED_MAX_JOINT_LOCKS);
        pkAVObj->RemoveExtraData(ED_MAX_JOINT_LIMITS);
    }
    
    pkAVObj->RemoveExtraData(ED_PROXY_SOURCE);
    pkAVObj->RemoveExtraData(ED_SHAPE_COMPOUND_ID);
    pkAVObj->RemoveExtraData(ED_SHAPE_COMPOUND);
    pkAVObj->RemoveExtraData(ED_SHAPE_GROUP);
    pkAVObj->RemoveExtraData(ED_SHAPE_OFFSET);
    pkAVObj->RemoveExtraData(ED_MATERIAL_TYPE);
    pkAVObj->RemoveExtraData(ED_RESTITUTION);
    pkAVObj->RemoveExtraData(ED_DYNAMIC_FRICTION);
    pkAVObj->RemoveExtraData(ED_STATIC_FRICTION);
    pkAVObj->RemoveExtraData(ED_SHAPE_TYPE);
    pkAVObj->RemoveExtraData(ED_SHAPE_POSE);
    pkAVObj->RemoveExtraData(ED_SHAPE_RAD);
    pkAVObj->RemoveExtraData(ED_SHAPE_DIM);
    pkAVObj->RemoveExtraData(ED_SHAPE_HEIGHT);
    pkAVObj->RemoveExtraData(ED_SHAPE_MESH_PARENT);
    pkAVObj->RemoveExtraData(ED_SHAPE_MESH_CHILD);
    pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_PARENT);
    pkAVObj->RemoveExtraData(ED_SHAPE_BASIC_CHILD);
    pkAVObj->RemoveExtraData(ED_JOINT_PARENT);
    pkAVObj->RemoveExtraData(ED_CLOTH_ATTACH_SHAPE);

    if (NiIsKindOf(NiNode, pkAVObj))
    {    
        NiNode* pkNode = (NiNode*)pkAVObj;
        
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkObj = pkNode->GetAt(ui);
            if (!pkObj)
                continue;
            RemoveED(pkObj);
        }
    }   
}
//---------------------------------------------------------------------------

