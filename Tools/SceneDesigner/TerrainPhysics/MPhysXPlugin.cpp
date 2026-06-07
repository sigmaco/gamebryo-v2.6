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

#include "PhysXPluginPCH.h"

#include "MPhysXPlugin.h"
#include <NiTerrainLib.h>

#if _MSC_VER == 1310
#pragma unmanaged
#include <windows.h>
#include <_vcclrit.h>
#pragma managed
#endif

#include <NiFilename.h>
#include <NiPhysXSDM.h>

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Reflection;
using namespace Emergent::Gamebryo::SceneDesigner::PhysXPlugin;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

//---------------------------------------------------------------------------
MPhysXPlugin::MPhysXPlugin(void)
{
    MAssert(ms_pmInstance == 0);
    ms_pmInstance = this;
}
//---------------------------------------------------------------------------
void MPhysXPlugin::Load(int, int)
{        
    // Initialise the custom components
    NiPhysXSDM::Init();
}
//---------------------------------------------------------------------------
void MPhysXPlugin::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        NiPhysXSDM::Shutdown();
    }
}
//---------------------------------------------------------------------------
String* MPhysXPlugin::get_Name()
{
    return "Terrain Physics";
}
//---------------------------------------------------------------------------
System::Version* MPhysXPlugin::get_Version()
{
    return Assembly::GetCallingAssembly()->GetName()->Version;
}
//---------------------------------------------------------------------------
System::Version* MPhysXPlugin::get_ExpectedVersion()
{
    return new System::Version(2, 2);
}
//---------------------------------------------------------------------------
IService* MPhysXPlugin::GetProvidedServices()[]
{
    return new IService*[];
}
//---------------------------------------------------------------------------
void MPhysXPlugin::Start()
{
    IUICommandService* pmCommandService = MGetService(IUICommandService);
    pmCommandService->BindCommands(this);
    UICommand* pmCommand = 
        pmCommandService->GetCommand(PONG_TERRAIN_PHYSX_PLUGIN);
    if (pmCommand)
        pmCommand->DoClick(0,0);

    RegisterEvents();
}
//----------------------------------------------------------------------------
void MPhysXPlugin::AddToolBarButtons(
    IUICommandService* pmUICommandService)
{
    NI_UNUSED_ARG(pmUICommandService);
}
//---------------------------------------------------------------------------
void MPhysXPlugin::OnPingPhysXPlugin(System::Object* sender, 
    EventArgs* e)
{
    // PhysX Plugin respond with a PONG command
    IUICommandService* pmCommandService = MGetService(IUICommandService);
    UICommand* pmCommand = 
        pmCommandService->GetCommand(PONG_TERRAIN_PHYSX_PLUGIN);
    if (pmCommand)
    {       
        pmCommand->DoClick(sender, e);
    }
}
//---------------------------------------------------------------------------
void MPhysXPlugin::OnCookPhysXData(System::Object*, 
    EventArgs* pmEventArgs)
{
    // PhysX Plugin to get the Terrain Plugin and save the physX data of the
    // currently selected terrain.
    UICommandEventArgs* pmCommandEventArgs = 
        static_cast<UICommandEventArgs*>(pmEventArgs);
    String *pmEntityName = pmCommandEventArgs->GetValue("Entity");

    bool bForced = pmCommandEventArgs->GetValue("Forced") != 0;

    MScene* pmScene = MFramework::Instance->Scene;
    MEntity* pmEntity = pmScene->GetEntityByName(pmEntityName);
    NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();

    NiObject* pkObject = 0;
    pkEntity->GetPropertyData(NiTerrainComponent::ms_kPropSceneRoot, 
        (NiObject*&)pkObject, 0);
    NiTerrain* pkTerrain = NiDynamicCast(NiTerrain, pkObject);

    if (pkTerrain)
    {
        NiTerrainSector* pkSector = pkTerrain->GetSector(0, 0);
        NIASSERT(pkSector);
        NiFixedString kPath = pkSector->GetArchivePath();

        String* pmMessage = String::Concat(
            S"This operation will cook the PhysX data of ",
            pmEntityName, S" into the following directory:\n",
            new String(kPath), S"\n\nDo you wish to proceed?");

        String* pmCaption = String::Concat(S"Cooking Terrain PhysX Data - ", 
            pmEntityName);

        if (kPath.GetLength() == 0 || !kPath.Exists())
        {
            // No archive path set
            System::Windows::Forms::MessageBox::Show(
                "The selected terrain entity does not have an archive path "
                "set.\nPlease select an appropriate archive path before "
                "cooking the PhysX data", pmCaption, MessageBoxButtons::OK, 
                MessageBoxIcon::Exclamation);
        }
        else if (!bForced || 
            System::Windows::Forms::MessageBox::Show(pmMessage, 
            pmCaption, MessageBoxButtons::YesNo, MessageBoxIcon::Question) == 
            DialogResult::Yes)
        {
            NiTerrainPhysXUtils kTerrainPhysXUtils; 
            if (!kTerrainPhysXUtils.SavePhysXData(pkSector))
            {
               // Failed Save. Notify the user.
                System::Windows::Forms::MessageBox::Show(
                "Cooking the PhysX data has failed. Please check for "
                "read-only files in the archive path.", 
                pmCaption, MessageBoxButtons::OK, 
                MessageBoxIcon::Exclamation);
            }
        }
    }
}
//==========================================================================
// Plug in event registration
//===========================================================================
void MPhysXPlugin::RegisterEvents()
{
}
//---------------------------------------------------------------------------
System::ComponentModel::ComponentResourceManager* 
    MPhysXPlugin::GetResourceManager()
{
    MPhysXPlugin* pmInstance = GetInstance();

    if (!pmInstance->m_pmResourceManager)
    {        
        pmInstance->m_pmResourceManager = 
            new System::ComponentModel::ComponentResourceManager(
            __typeof(MPhysXPlugin));
    }

    return pmInstance->m_pmResourceManager;
}
//---------------------------------------------------------------------------
MPhysXPlugin* MPhysXPlugin::GetInstance()
{
    assert(ms_pmInstance);
    return ms_pmInstance;
}
//---------------------------------------------------------------------------

//The following is related to a bug in VC7.1/.Net CLR 1.1
// see http://support.microsoft.com/?id=814472

void MPhysXPlugin::InitStatics()
{
#if _MSC_VER == 1310
    __crt_dll_initialize();
#endif
}
//---------------------------------------------------------------------------
void MPhysXPlugin::ShutdownStatics()
{
#if _MSC_VER == 1310
    //__crt_dll_terminate();
#endif
}
//---------------------------------------------------------------------------
