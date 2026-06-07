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

#include "TerrainPluginPCH.h"

#if _MSC_VER == 1310
#pragma unmanaged
#include <windows.h>
#include <_vcclrit.h>
#pragma managed
#endif

#include <NiFilename.h>
#include "MBrushOperationFactory.h"
#include "MTerrainPlugin.h"
#include "ManagedMacros.h"
#include <NiTerrainComponent.h>
#include <NiWaterComponent.h>
#include <NiTerrainBlock.h>
#include <NiTerrainSDM.h>

#include "MBrushElementSculpt.h"
#include "MBrushElementSurface.h"
#include "MTerrainDirectoryPathEditor.h"
#include "MImageFileNameEditor.h"
#include "MTerrainTextureSizeDropDown.h"
// -- Include custom brush elements header files here

// -- End custom elments header inclusion

#include "MBrushOperationAlign.h"
#include "MBrushOperationElevate.h"
#include "MBrushOperationFlatten.h"
// -- Include custom brush operations header files here
#include "MBrushOperationSmooth.h"
#include "MBrushOperationStatic.h"

// -- End custom operations header inclusion

// -- Include Custom brush types header here
#include "MBrushTypeCircle.h"
#include "MBrushTypeMask.h"
// -- End custom brush type header inclusion

//#using <mscorlib.dll>

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Reflection;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

static const char* BACKUP_DIRECTORY = "\\.autosave";

//---------------------------------------------------------------------------
MTerrainPlugin::MTerrainPlugin(void)
{
    assert(ms_pmInstance == 0);
    ms_pmInstance = this;
    m_bPhysXPluginLoaded = false;
}

//---------------------------------------------------------------------------
void MTerrainPlugin::Load(int, int)
{
    m_pmSelectedTerrain = 0;
    // Sets the resource manager to use for the icons
    System::ComponentModel::ComponentResourceManager* pmResources = 
        GetResourceManager();
        
    // Initialize the custom components
    NiTerrainSDM::Init();
    MBrush::_SDMInit();
    MBrushOperationFactory::_SDMInit();
    
    // Reference the base operations and add them to the manager
    MBrushOperationFactory* pmManager = MBrushOperationFactory::GetInstance();

    // Create base elements
    MBrushElementSculpt* pmSculptElement = new MBrushElementSculpt();
    pmSculptElement->m_pmName = S"Sculpt";
    MBrushElementSurface* pmSurfaceElement = new MBrushElementSurface();
    pmSurfaceElement->m_pmName = S"Material";
    // NOTE: ANY CUSTOM ELEMENTS SHOULD BE ADDED HERE


    // NOTE END CUSTOM ELEMENTS ADDITION SECTION

    // Create base operations Name and Icon must be set

    MBrushOperationAlign* pmAlign = new MBrushOperationAlign();
    pmAlign->m_pmElement = pmSculptElement->CloneOperation();
    pmAlign->m_pmName = S"Flatten";
    pmAlign->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"FlattenIcon")));
    
    MBrushOperationElevate* pmElevateSculpt = new MBrushOperationElevate();
    pmElevateSculpt->m_pmElement = pmSculptElement->CloneOperation();
    pmElevateSculpt->m_pmName = S"Elevation";
    pmElevateSculpt->m_fStrength = 1.0f;
    pmElevateSculpt->m_fPercent = 0.5f;
    pmElevateSculpt->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"ElevateIcon")));
    
    MBrushOperationElevate* pmElevateSurface = new MBrushOperationElevate();
    pmElevateSurface->m_pmElement = pmSurfaceElement->CloneOperation();
    pmElevateSurface->m_pmName = S"Paint";
    pmElevateSurface->m_fStrength = 50.0f;
    pmElevateSurface->m_fPercent = 1.0f;
    pmElevateSurface->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"BlendIcon")));
    
    // NOTE: ANY CUSTOM OPERATIONS SHOULD BE ADDED HERE
    // IMPORTANT: OPERATION SHOULD BE GIVEN A CLONE OF THE ELEMENT THEY USE
    MBrushOperationSmooth* pmSmoothSculpt = new MBrushOperationSmooth();
    pmSmoothSculpt->m_pmElement = pmSculptElement->CloneOperation();
    pmSmoothSculpt->m_pmName = S"Blend";
    pmSmoothSculpt->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"SmoothIcon")));

    MBrushOperationStatic* pmStaticSculpt = new MBrushOperationStatic();
    pmStaticSculpt->m_pmElement = pmSculptElement->CloneOperation();
    pmStaticSculpt->m_pmName = S"Noise";
    pmStaticSculpt->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"StaticIcon")));

    MBrushOperationSmooth* pmSmoothSurface = new MBrushOperationSmooth();
    pmSmoothSurface->m_pmElement = pmSurfaceElement->CloneOperation();
    pmSmoothSurface->m_pmName = S"Blend";
    pmSmoothSurface->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"SmoothIcon")));

    MBrushOperationStatic* pmStaticSurface = new MBrushOperationStatic();
    pmStaticSurface->m_pmElement = pmSurfaceElement->CloneOperation();
    pmStaticSurface->m_pmName = S"Noise";
    pmStaticSurface->m_pmOperationIcon = (__try_cast<System::Drawing::Image*>
        (pmResources->GetObject(S"StaticIcon")));

    // NOTE END CUSTOM OPERATIONS ADDITION SECTION

    // Add the operation to the operation manager
    pmManager->AddTemplate(pmAlign);
    pmManager->AddTemplate(pmElevateSculpt);
    pmManager->AddTemplate(pmElevateSurface);

    // NOTE: ANY CUSTOM OPERATIONS SHOULD BE ADDED TO THE MANAGER HERE
    pmManager->AddTemplate(pmSmoothSculpt);
    pmManager->AddTemplate(pmStaticSculpt);
    pmManager->AddTemplate(pmSmoothSurface);
    pmManager->AddTemplate(pmStaticSurface);
    
    // NOTE END CUSTOM OPERATIONS ADDITION SECTION

    // NOTE: ANY CUSTOM BRUSH TYPES SHOULD BE CREATED HERE
    MBrushTypeCircle* pmBrushCircle = new MBrushTypeCircle();
    MBrushTypeMask* pmBrushMask = new MBrushTypeMask();

    // NOTE: END CUSTOM BRUSH TYPES CREATION

    // NOTE: CUSTOM BRUSH TYPE SHOULD BE ADDED TO THE BRUSH HERE
    MBrush::_RegisterBrushType(pmBrushCircle);
    MBrush::_RegisterBrushType(pmBrushMask);
}
//---------------------------------------------------------------------------
void MTerrainPlugin::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        if (m_pmCommandPanel != NULL)
        {
            m_pmCommandPanel->OnBrushActivated(false);
            m_pmCommandPanel->UseSurfacePalette(0);
            m_pmCommandPanel->Dispose();
        }

        if (m_pmSurfaceCommandPanel != NULL)
        {
            m_pmSurfaceCommandPanel->UseSurfacePalette(0);
            m_pmSurfaceCommandPanel->Dispose();
        }

        if (m_pmBrush)
            m_pmBrush->Dispose();

        MFramework::Instance->PickUtility->RemovePolicy(m_pmTerrainPickPolicy);

        NiDelete m_pkMetaDataStore;

        MBrush::_SDMShutdown();
        MBrushOperationFactory::_SDMShutdown();
        NiTerrainSDM::Shutdown();
    }
}
//---------------------------------------------------------------------------
String* MTerrainPlugin::get_Name()
{
    return "Terrain Editor";
}
//---------------------------------------------------------------------------
System::Version* MTerrainPlugin::get_Version()
{
    return Assembly::GetCallingAssembly()->GetName()->Version;
}
//---------------------------------------------------------------------------
System::Version* MTerrainPlugin::get_ExpectedVersion()
{
    return new System::Version(2, 2);
}
//---------------------------------------------------------------------------
IService* MTerrainPlugin::GetProvidedServices()[]
{
    return new IService*[];
}
//---------------------------------------------------------------------------
void MTerrainPlugin::Start()
{    
    // Get property type service.
    IPropertyTypeService* pmPropertyTypeService = MGetService(
        IPropertyTypeService);
    MAssert(pmPropertyTypeService != NULL, "Service not found!");

    // Add property types
    pmPropertyTypeService->RegisterType(new PropertyType(
        "Directory Path", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MTerrainDirectoryPathEditor), NULL));

    pmPropertyTypeService->RegisterType(new PropertyType(
        "Texture Size", NiEntityPropertyInterface::PT_UINT,
        __typeof(unsigned int), NULL, __typeof(MTerrainTextureSizeDropDown)));

    pmPropertyTypeService->RegisterType(new PropertyType(
        "Image file", NiEntityPropertyInterface::PT_STRING,
        __typeof(String), __typeof(MImageFileNameEditor), NULL));

    NiTerrainInteractor::SetInToolMode(true);
    m_pkMetaDataStore = NiNew NiMetaDataStore();

    /*
     * Register the custom Terrain component
     */

    // Get component service.
    IComponentService* pmComponentService = MGetService(IComponentService);

    // Get component factory.
    MComponentFactory* pmFactory = MFramework::Instance->ComponentFactory;
    
    // Register custom components.
    pmComponentService->RegisterComponent(pmFactory->Get(
            NiNew NiTerrainComponent()
        )
    );
    
    pmComponentService->RegisterComponent(pmFactory->Get(
            NiNew NiWaterComponent()
        )
    );
    
    // Instanstiate the new AeBrush **
    m_pmBrush = new MBrush();
    m_pmBrush->RegisterSettings();

    // Initialize the IInteraction component
    m_pmInteractionModeService = MGetService(IInteractionModeService);
    m_pmInteractionModeService->AddInteractionMode(m_pmBrush);

    /*
     * Load our command panel
     */
    // Get the command panel service    
    ICommandPanelService* pmCommandPanelService = 
        MGetService(ICommandPanelService);

    m_pmCommandPanel = new TerrainCommandPanel();
    pmCommandPanelService->RegisterPanel(m_pmCommandPanel);

    m_pmSurfaceCommandPanel = new SurfaceCommandPanel();
    pmCommandPanelService->RegisterPanel(m_pmSurfaceCommandPanel);

    // setup the command service for the UIs
    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    pmUICommandService->BindCommands(this);

    AddToolBarButtons(pmUICommandService);

    EntitySelected(0);
    
    RegisterEvents();

    // Register the PickObjectPolicy for terrain picking
    m_pmTerrainPickPolicy = new MTerrainPickPolicy();
    MFramework::Instance->PickUtility->AddPolicy(m_pmTerrainPickPolicy);

    // Ping the terrain physx plugin to see if it had been loaded
    pmUICommandService->GetCommand(PING_TERRAIN_PHYSX_PLUGIN)->DoClick(0,0);
}
//----------------------------------------------------------------------------
void MTerrainPlugin::AddToolBarButtons(IUICommandService* pmUICommandService)
{
    String* pmToolbarBtnName = "Terrain Tool";
    String* pmLightToolbarBtnName = "Rebuild Terrain Lighting";
    String* pmSnapToolbarBtnName = "Move to Terrain";

    IToolBarService* pmToolBarService = MGetService(IToolBarService);
    //String* pmToolbarName = pmToolBarService->StandardToolBarName; 
    // this toolbar not exist?
    
    String* pmToolbarName = "10 Transformation Interaction Modes";
    String* pmSnapToolbarName = "12 Snap Align and Precision";
    String* pmLightToolbarName = "13 Lighting Optimization";

    // setup the command that's executed on pushing the button
    UICommand* pmUICommand = pmUICommandService->GetCommand(
        MTerrainPlugin::ActivateBrushUICommandName);
    UICommand* pmUILightCommand = pmUICommandService->GetCommand(
        MTerrainPlugin::RebuildLightCommandName);
    UICommand* pmUISnapCommand = pmUICommandService->GetCommand(
        MTerrainPlugin::MoveToTerrainUICommandName);
    
    System::ComponentModel::ComponentResourceManager* pmResources = 
        GetResourceManager();

    Image* pmButtonImage = 
        dynamic_cast<Image*>(pmResources->GetObject("TerrainEditIcon"));
    Image* pmLightButtonImage = 
        dynamic_cast<Image*>(pmResources->GetObject("TerrainLightIcon"));
    Image* pmSnapButtonImage = 
        dynamic_cast<Image*>(pmResources->GetObject("MoveToTerrainIcon"));

    if (!pmToolBarService->AddToolBarButton(pmToolbarName, pmToolbarBtnName, 
        pmButtonImage, pmUICommand))  
    {// Toolbar trying to add button to does not exist
        const char* msg = MStringToCharPointer(pmToolbarName);
        NiMessageBox(msg, "Toolbar not found");
        MFreeCharPointer(msg);
    }

    if (!pmToolBarService->AddToolBarButton(pmLightToolbarName, 
        pmLightToolbarBtnName, pmLightButtonImage, pmUILightCommand))
    {// Toolbar trying to add button to does not exist
        const char* msg = MStringToCharPointer(pmLightToolbarName);
        NiMessageBox(msg, "Lighting Toolbar not found");
        MFreeCharPointer(msg);
    }

    if (!pmToolBarService->AddToolBarButton(pmSnapToolbarName, 
        pmSnapToolbarBtnName, pmSnapButtonImage, pmUISnapCommand))
    {
        const char* msg = MStringToCharPointer(pmLightToolbarName);
        NiMessageBox(msg, "Snap Toolbar not found");
        MFreeCharPointer(msg);
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnPongPhysXPlugin(System::Object*, 
    EventArgs*)
{
    m_bPhysXPluginLoaded = true;
    if (m_pmCommandPanel)
        m_pmCommandPanel->EnablePhysXPlugin();
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnToolBarBtnTerrainToggle(System::Object*, 
    EventArgs*)
{
    if (m_pmInteractionModeService->ActiveMode != m_pmBrush)
    { // active the terrain brush
        m_pmPrevActiveMode = m_pmInteractionModeService->ActiveMode;
        m_pmInteractionModeService->ActiveMode = m_pmBrush;
        m_pmCommandPanel->OnBrushActivated(true);

        // Find the best terrain
        if (!m_pmSelectedTerrain)
            SelectClosestTerrain();
    }
    else 
    { // deactive the terrain brush
        m_pmInteractionModeService->ActiveMode = m_pmPrevActiveMode;
        m_pmCommandPanel->OnBrushActivated(false);
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::SelectClosestTerrain()
{
    MScene* pmScene = MFramework::Instance->Scene;
    if (!pmScene)
        return;

    // Cycle through each entity in the scene, looking for one which contains
    // a terrain
    MEntity* apmEntities[] = pmScene->GetEntities();
    MEntity* pmEntity;
    NiEntityInterface* pkTerrainEntity;
    MEntity* pmBestTerrain = 0;
    float fBestDistSqr = FLT_MAX;
    NiPoint3 kCameraLoc(NiPoint3::ZERO);

    NiEntityInterface* pkCameraEntity = MFramework::Instance->ViewportManager->
        ActiveViewport->CameraEntity->GetNiEntityInterface();
    pkCameraEntity->GetPropertyData("Translation", kCameraLoc, 0);

    for (int i = 0; i < apmEntities->Count; ++i)
    {
        pmEntity = apmEntities[i];
        if (pmEntity->HasProperty(S"Terrain Archive"))
        {
            pkTerrainEntity = pmEntity->GetNiEntityInterface();

            NiPoint3 kTerrainLoc(NiPoint3::ZERO);
            pkTerrainEntity->GetPropertyData("Translation", kTerrainLoc, 0);

            float fDist = (kTerrainLoc - kCameraLoc).SqrLength();
            if (fDist < fBestDistSqr)
            {
                fBestDistSqr = fDist;
                pmBestTerrain = pmEntity;
            }
        }
    }

    if (pmBestTerrain)
        SetTerrainEntity(pmBestTerrain);
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnToolBarBtnRebuildLights(System::Object*, 
    EventArgs*)
{
    if (!m_pmSelectedTerrain)
        SelectClosestTerrain();

    if (m_pmSelectedTerrain)
    {
        // Rebuild the lights
        NiTerrainInteractor* pkInteractor = 0;
        size_t stSize = 0;
            
        m_pmSelectedTerrain->GetNiEntityInterface()->
            GetPropertyData("Interactor", (void*&)pkInteractor, stSize, 0);
        pkInteractor->RebuildAllLighting();

        m_pmSelectedTerrain->Update(
            0.0f, 
            MFramework::Instance->ExternalAssetManager);
    }
    else
    {
        String* pmCaption = new String("No terrain found in current scene, "
            "cannot build lighting.");
        
        IMessageService* pmMessageService = MGetService(IMessageService);
        pmMessageService->AddMessage(MessageChannelType::Errors, 
            pmCaption);
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnToolBarBtnMoveToTerrain(System::Object* pmSender, 
    EventArgs* pmArgs)
{
    NI_UNUSED_ARG(pmArgs);
    NI_UNUSED_ARG(pmSender);
    ISelectionService* pmSelectionService = MGetService(ISelectionService);
    if (!pmSelectionService)
        return;

    // find the terrain's up axis
    NiPoint3 kUpAxis;
    MEntity* pmTerrain;
    GetTerrainEntity(pmTerrain);
    if (!pmTerrain)
        return;
    NiEntityInterface* pkTerrain = pmTerrain->GetNiEntityInterface();
    NiMatrix3 kRotation;
    if (!pkTerrain->GetPropertyData(NiFixedString("Rotation"), kRotation))
    {
        // instead of getting the setting, we will just assume Z
        kUpAxis = NiPoint3::UNIT_Z;
    }
    else
    {
        kRotation.GetCol(2, kUpAxis);
    }

    IMessageService* pmMessageService = MGetService(IMessageService);
    MAssert(pmMessageService != NULL, "Null message service!");

    // batch all of the changes into a single undo operation
    ICommandService* pmCommandService = MGetService(ICommandService);
    pmCommandService->BeginUndoFrame("Move to Terrain");
    MEntity* pmSelection[] = pmSelectionService->GetSelectedEntities();
    bool bFound = false;
    for (int i = 0; i < pmSelection->Count; i++)
    {
        MEntity* pmEntity = dynamic_cast<MEntity*>(pmSelection->Item[i]);

        if (pmEntity == pmTerrain)
        {
            continue;
        }
        else if (!pmEntity->Writable)
        {
            pmMessageService->AddMessage(MessageChannelType::Errors, new
                Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
                String::Format("Cannot move the \"{0}\" entity to the "
                "terrain.", pmEntity->Name), "The entity is read-only and "
                "cannot be modified.", pmEntity));
            continue;
        }

        NiEntityInterface* pkEntity = pmEntity->GetNiEntityInterface();

        NiPoint3 kOrigin;
        if (!pkEntity->GetPropertyData(NiFixedString("Translation"), kOrigin))
            continue;

        NiTerrainInteractor* pkInteractor = 0;
        size_t size = 0;
        pkTerrain->GetPropertyData(NiFixedString("Interactor"), 
            (void*&)pkInteractor, size, 0);

        NiBound kBound = 
            *(MFramework::Instance->BoundManager->GetSceneBound());
        float fBoundDiameter = kBound.GetRadius() * 2.0f;
        NiRay kRay = NiRay(kOrigin + kUpAxis * fBoundDiameter, -kUpAxis);

        if (pkInteractor->Collide(kRay))
        {
            bFound = true;
            NiPoint3 kPos;
            NiPoint3 kNorm;
            kRay.GetIntersection(kPos, kNorm);
            pmEntity->SetPropertyData("Translation", new MPoint3(kPos), 0, 
                true);
        }
    }
    // only add the command to the undo stack if we changed a property
    pmCommandService->EndUndoFrame(bFound);
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnValidateTerrainToggle(System::Object* pmSender, 
    UIState* pmState)
{
    NI_UNUSED_ARG(pmSender);

    ISelectionService* pmSelectionService = MGetService(ISelectionService);
    MAssert(pmSelectionService != NULL, "Null selection service!");

    pmState->Enabled = false;
    MEntity* amSelectedEntities[] = pmSelectionService->GetSelectedEntities();
    for (int i = 0; i < amSelectedEntities->Length; ++i)
    {
        MEntity* pmSelectedEntity = amSelectedEntities[i];
        if (pmSelectedEntity->HasProperty(
                NiTerrainComponent::ms_kPropStorageFileName) &&
            pmSelectedEntity->Writable)
        {
            pmState->Enabled = true;
        }
    }

    // save the state of the toolbar toogle, as when this function starts
    pmState->Checked = (m_pmInteractionModeService->ActiveMode == m_pmBrush);

    if (!pmState->Enabled && pmState->Checked)
    {
        // If the interaction mode is active when the state becomes disabled,
        // switch to the selection interaction mode.
        OnToolBarBtnTerrainToggle(NULL, NULL);
        pmState->Checked = false;
    }

    if (!pmState->Checked)
    {
        m_pmCommandPanel->OnBrushActivated(false);
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnValidateRebuildLights(System::Object* pmSender,
    UIState* pmState)
{
    NI_UNUSED_ARG(pmSender);

    ISelectionService* pmSelectionService = MGetService(ISelectionService);
    MAssert(pmSelectionService != NULL, "Null selection service!");

    pmState->Enabled = false;
    MEntity* amSelectedEntities[] = pmSelectionService->GetSelectedEntities();
    for (int i = 0; i < amSelectedEntities->Length; ++i)
    {
        MEntity* pmSelectedEntity = amSelectedEntities[i];
        if (pmSelectedEntity->HasProperty(
                NiTerrainComponent::ms_kPropStorageFileName) &&
            pmSelectedEntity->Writable)
        {
            pmState->Enabled = true;
        }
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnValidateMoveToTerrain(System::Object* pmSender,
    UIState* pmState)
{
    NI_UNUSED_ARG(pmSender);

    MEntity* pmTerrain = NULL;
    GetTerrainEntity(pmTerrain);

    ISelectionService* pmSelectionService = MGetService(ISelectionService);
    MAssert(pmSelectionService != NULL, "Null selection service!");

    pmState->Enabled = (pmTerrain != NULL &&
        pmSelectionService->NumSelectedEntities > 0);
}
//---------------------------------------------------------------------------
void MTerrainPlugin::ActivateInteractionMode(System::Object* pmSender,
    EventArgs* pmArgs)
{
    NI_UNUSED_ARG(pmArgs);
    NI_UNUSED_ARG(pmSender);
    if (m_pmBrush && m_pmInteractionModeService->ActiveMode != m_pmBrush)
    {
        m_pmPrevActiveMode = m_pmInteractionModeService->ActiveMode;
        m_pmInteractionModeService->ActiveMode = m_pmBrush;        
        m_pmCommandPanel->OnBrushActivated(true);
    }
}

//==========================================================================
// Plug in event registration
//===========================================================================
void MTerrainPlugin::RegisterEvents()
{
    MFramework::Instance->EventManager->SceneClosing += new 
        MEventManager::__Delegate_SceneClosing(this,
        &MTerrainPlugin::SceneClosing);

    MFramework::Instance->EventManager->ApplicationClosing += new 
        MEventManager::__Delegate_ApplicationClosing(this,
        &MTerrainPlugin::ApplicationClosing);

    MFramework::Instance->EventManager->LayerRenamed += new 
        MEventManager::__Delegate_LayerRenamed(this,
        &MTerrainPlugin::OnLayerRenamed);

    MFramework::Instance->EventManager->LayerAdded += new 
        MEventManager::__Delegate_LayerAdded(this,
        &MTerrainPlugin::OnLayerAdded);

    MFramework::Instance->EventManager->NewSceneLoaded += new 
        MEventManager::__Delegate_NewSceneLoaded(this,
        &MTerrainPlugin::OnLoadingScene);

    MFramework::Instance->EventManager->EntityPropertyChanged += new 
        MEventManager::__Delegate_EntityPropertyChanged(this,
        &MTerrainPlugin::OnEntityPropertyChanged);

    MFramework::Instance->EventManager->RequestAddCloneToScene += new
        MEventManager::__Delegate_RequestAddCloneToScene(this,
        &MTerrainPlugin::OnRequestAddCloneToScene);

    MFramework::Instance->EventManager->EntityAddedToScene += new
        MEventManager::__Delegate_EntityAddedToScene(this,
        &MTerrainPlugin::OnEntityAddedToScene);

    MFramework::Instance->EventManager->EntityRemovedFromScene += new
        MEventManager::__Delegate_EntityRemovedFromScene(this,
        &MTerrainPlugin::OnEntityRemovedFromScene);

    MFramework::Instance->EventManager->PaletteAdded += new
        MEventManager::__Delegate_PaletteAdded(this, 
        &MTerrainPlugin::OnPaletteAdded);

    MFramework::Instance->EventManager->LayerSaving += new
        MEventManager::__Delegate_LayerSaving(this,
        &MTerrainPlugin::OnLayerSaving);

    MFramework::Instance->EventManager->LayerSaved += new
        MEventManager::__Delegate_LayerSaved(this,
        &MTerrainPlugin::OnLayerSaved);

    MFramework::Instance->EventManager->DeletedAutoSave += new
        MEventManager::__Delegate_DeletedAutoSave(this,
        &MTerrainPlugin::OnDeletedAutoSave);

}
//---------------------------------------------------------------------------
void MTerrainPlugin::SceneClosing(MScene*)
{
    EntitySelected(NULL);
}
//---------------------------------------------------------------------------
void MTerrainPlugin::ApplicationClosing()
{
    if (GetSurfaceCommandPanel()->IsDirty())
    {
        if (System::Windows::Forms::MessageBox::Show(
            "One or more surface palettes have been modified without " \
            "saving.\nWould you like to save all surface palettes now?",
            "Surface palettes require saving",
            MessageBoxButtons::YesNo) == DialogResult::No)
        {
            return;
        }

        GetSurfaceCommandPanel()->SaveAll();
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnLayerRenamed(MLayer* pmLayer, 
                                    String* /*strOldFilename*/)
{
    // Clear the "Terrain Archive" property 
    // This will force it to be set appropriately in the OnLayerSaving function
    MEntity* apmEntities[] = pmLayer->GetEntities();
    MEntity* pmEntity;
    NiEntityInterface* pkTerrainEntity = NULL;

    for (int i = 0; i < apmEntities->Count; ++i)
    {
        pmEntity = apmEntities[i];
        if (pmEntity->HasProperty(S"Terrain Archive"))
        {
            NIASSERT(pkTerrainEntity == NULL); // only support one terrain.
            pmEntity->SetPropertyData(S"Terrain Archive", NULL, false);
            pkTerrainEntity = pmEntity->GetNiEntityInterface();
        }
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnLoadingScene(MScene* pmScene)
{
    LoadTerrainArchives(pmScene->GetEntities());
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnLayerAdded(MLayer* pmLayer, MLayer* /* pmParent */)
{
    LoadTerrainArchives(pmLayer->GetEntities());
}
//---------------------------------------------------------------------------
void MTerrainPlugin::LoadTerrainArchives(MEntity* pkEntities[])
{
    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Get count of all the terrain components    
    NiUInt32 uiEntityCount = pkEntities->Length;
    List<String*>* pmFilesWithTerrain = new List<String*>();

    for (NiUInt32 uiEntity = 0; uiEntity < uiEntityCount; uiEntity++)
    {
        // Make sure the entity doesn't have any deprecated components
        HandleComponentDeprecation(pkEntities[uiEntity]);

        NiTerrainComponent* pkComponent = 
            NiTerrainComponent::FindTerrainComponent(
            pkEntities[uiEntity]->GetNiEntityInterface());
        if (pkComponent)
        {
            String* pFilename = pkEntities[uiEntity]->SourceFilename;
            if (!pmFilesWithTerrain->Contains(pFilename))
                pmFilesWithTerrain->Add(pFilename);

            if (!pkComponent->IsTemplate())
            {
                NiFixedString kArchiveName;
                pkComponent->GetPropertyData(
                    NiTerrainComponent::ms_kPropStorageFileName, kArchiveName);

                NiBool bResult = pkComponent->Load(kArchiveName,
                    spErrors);
                if (!bResult)
                {
                    spErrors->ReportError("Failed to load terrain "
                        "archive. Ensure archive location exists.",
                        "MTerrainPlugin::OnLoadingScene", 
                        pkComponent->GetName(), NULL);
                }

                // Check to see if we just loaded an autosaved archive
                String* pmName = kArchiveName;
                String* pmBackup = BACKUP_DIRECTORY;
                if (pmName->Contains(pmBackup))
                {
                    kArchiveName = MStringToCharPointer(
                        pmName->Substring(0, pmName->get_Length() - 
                        pmBackup->get_Length()));

                    pkComponent->SetPropertyData(
                        NiTerrainComponent::ms_kPropStorageFileName,
                        kArchiveName);
                }
            }
        }
    }

    // update the surface panel for any loaded surfaces
    m_pmSurfaceCommandPanel->RefreshPackages();

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
    if ( pmFilesWithTerrain->Count > 1)
    {
        String* alertMsg = "There are too many terrain entities in this "
            "scene.  Your scene will not display\ncorrectly until you reduce "
            "the number of terrain entities to one.  The following\nfiles "
            "contain terrain entities:\n";
        List<String*>::Enumerator mEnum = pmFilesWithTerrain->GetEnumerator(); 
        while (mEnum.MoveNext())
        {
            String* roFile = mEnum.Current;
            alertMsg = String::Concat(alertMsg, "\n", roFile);
        }        
        MessageBox::Show(alertMsg, "Too Many Terrain Entities", 
            MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
}

//---------------------------------------------------------------------------
void MTerrainPlugin::OnEntityPropertyChanged(
    MEntity* pmEntity, String* pmPropertyName, unsigned int uiPropIndex, 
    bool bInBatch)
{
    // Check to see if we just changed the archive directory.
    if (pmPropertyName->CompareTo(new String(
        NiTerrainComponent::ms_kPropStorageFileName)) == 0)
    {
        NiEntityInterface* pkInterface = pmEntity->GetNiEntityInterface();

        NiFixedString kArchiveName;
        pkInterface->GetPropertyData(
            NiTerrainComponent::ms_kPropStorageFileName, kArchiveName);

        if (kArchiveName)
        {
            String* pmName = (const char*)kArchiveName;

            // Validate the archive directory
            while (!bInBatch &&
                !ValidateArchiveDirectory((const char*)kArchiveName))
            {
                MTerrainDirectoryPathEditor* pmPath = 
                    new MTerrainDirectoryPathEditor();
                kArchiveName = 
                    MStringToCharPointer(dynamic_cast<String*>
                    (pmPath->EditValue(NULL, NULL, pmName)));

                pmName = (const char*)kArchiveName;
            }

            pkInterface->SetPropertyData(
                NiTerrainComponent::ms_kPropStorageFileName, kArchiveName);
        }
    }

    if (pmPropertyName->CompareTo(new String(
        NiTerrainComponent::ms_kPropSectorList)) == 0)
    {
        NiEntityInterface* pkInterface = pmEntity->GetNiEntityInterface();
        NiObject* pkObject = 0;
        pkInterface->GetPropertyData(NiTerrainComponent::ms_kPropSceneRoot,
            (NiObject*&)pkObject);

        NiPoint2 kIndex;
        pkInterface->GetPropertyData(NiTerrainComponent::ms_kPropSectorList,
            kIndex, uiPropIndex);
        
        NiTerrain* pkTerrain = NiDynamicCast(NiTerrain, pkObject);
        if (pkTerrain)
        {
            pmEntity->Update(0.0f, MFramework::Instance->ExternalAssetManager);
            InitialiseNewSector(pkTerrain, (NiInt16)kIndex.x, 
                (NiInt16)kIndex.y);
        }
    }

    // is the entity a template?
    if (pmEntity->GetNiEntityInterface()->GetMasterEntity() == NULL)
        return;

    // If it is a terrain, or a sector entity, trigger an update of the surface
    // panel to ensure all loaded surfaces are displayed
    if (pmEntity->HasProperty(S"Parent Terrain ID"))
    {
        // Force the loading of any surfaces
        pmEntity->Update(0.0f, MFramework::Instance->ExternalAssetManager);

        // Now update the panel
        if (m_pmSurfaceCommandPanel)
        {
            NiTerrainComponent* pkTerrainComponent = 0;
            NiEntityInterface* pkEntityInterface = 
                pmEntity->GetNiEntityInterface();
            for (unsigned int i = 0; 
                i < pkEntityInterface->GetComponentCount() ; i++)
            {
                NiEntityComponentInterface* pkComponent = 
                    pkEntityInterface->GetComponentAt(i);
                if (pkComponent->GetClassName() == 
                    NiFixedString("NiTerrainComponent"))
                {
                    pkTerrainComponent = (NiTerrainComponent*)pkComponent;
                    break;
                } 
            }

            if (pkTerrainComponent)
            {
                m_pmSurfaceCommandPanel->UseSurfacePalette(
                    pkTerrainComponent->GetActiveSurfacePalette());
            }
            else
            {
                m_pmSurfaceCommandPanel->UseSurfacePalette(
                    NiSurfacePalette::GetInstance());
            }
        }
    }

    // If this is the currently selected terrain, we may need to update the
    // scale
    if (pmEntity == m_pmSelectedTerrain)
    {
        // Set the brush scale. Assumes that if the terrain doesn't have the
        // 'scale' property, it wont change the value of fScale from 1.0f.
        float fScale = 1.0f;
        pmEntity->GetNiEntityInterface()->GetPropertyData("Scale", fScale, 0);

        GetBrush()->SetScale(fScale);
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::InitialiseNewSector(NiTerrain* pkTerrain, 
    NiInt16 iIndexX, NiInt16 iIndexY)
{
    NiTerrainSector* pkSector = pkTerrain->GetSector(iIndexX, iIndexY);
    if (pkSector && pkSector->GetSectorData()->GetHighestLoadedLOD() == -1)
    {
        pkSector->SetTargetLoadedLOD(
            pkSector->GetSectorData()->GetNumLOD(), false);

        if (!pkSector->Load())
        {
            pkSector->CreateBlankGeometry();
        }
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnRequestAddCloneToScene(MScene* pmScene,
    MEntity* pmSourceEntity, bool __gc* bDoNotAdd)
{
    if (!pmSourceEntity->HasProperty(
        NiTerrainComponent::ms_kPropStorageFileName))
    {
        return;
    }

    MFramework* FW = MFramework::Instance;
    IMessageService* pmMessageService = MGetService(IMessageService);
    MAssert(pmMessageService != NULL, "Null message service!");

    if (FW->PrefabManager->GetPrefabByFilename(FW->LayerManager->DefaultLayer
        ->Name))
    {
        pmMessageService->AddMessage(MessageChannelType::Errors, new
            Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
            String::Format("Cannot add clone of \"{0}\" terrain entity to the "
            "scene.", pmSourceEntity->Name), "The default layer is also being "
            "used as a prefab. Prefabs cannot contain a terrain entity.",
            pmSourceEntity));
        *bDoNotAdd = true;
        return;
    }

    MEntity* amEntities[] = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MComponent* pmComponents[] = amEntities[i]->GetComponents();
        int iComponentCount = pmComponents->Count;
        for (int iComponentIndex = 0; iComponentIndex < iComponentCount; 
            iComponentIndex++)
        {
            MComponent* pmComponent = pmComponents[iComponentIndex];
            NiEntityComponentInterface* pkComponent = 
                pmComponent->GetNiEntityComponentInterface();
            NiFixedString kClassName = pkComponent->GetClassName();
            if (kClassName == "NiTerrainComponent")
            {
                pmMessageService->AddMessage(MessageChannelType::Errors, new
                    Emergent::Gamebryo::SceneDesigner::PluginAPI::Message(
                    String::Format("Cannot add clone of \"{0}\" terrain "
                    "entity to the scene.", pmSourceEntity->Name), "The scene "
                    "already contains a terrain entity. Only one terrain "
                    "entity may be present in a scene.", pmSourceEntity));
                *bDoNotAdd = true;
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    if (!EnsureValidSceneForEntity(pmEntity))
    {
        pmScene->RemoveEntity(pmEntity, false);
        pmEntity->Dispose();
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnEntityRemovedFromScene(MScene* pmScene,
    MEntity* pmEntity)
{
    NI_UNUSED_ARG(pmScene);
    if (pmEntity->HasProperty(NiTerrainComponent::ms_kPropSectorList) ==
        false)
        return;

    m_pmSelectedTerrain = 0;
}
//---------------------------------------------------------------------------
bool MTerrainPlugin::ValidateArchiveDirectory(const char* pcArchiveDirectory)
{
    if (!pcArchiveDirectory)
        return false;

    String* astrDirectories __gc[] = 
        Directory::GetDirectories(pcArchiveDirectory, 
        "Sector*");
    if (astrDirectories->Length > 0)
    {
        if (MessageBox::Show("Selected location already "
            "contains a terrain archive. Do you want to "
            "overwrite the archive at that location? Choose "
            "No to select a different directory.", 
            "Archive conflict", MessageBoxButtons::YesNo,
            MessageBoxIcon::Question) == DialogResult::No)
        {
            // Not valid
            return false;
        }
    }

    // is valid
    return true;
}
//---------------------------------------------------------------------------
namespace
{
    // Find 
    MEntity* FindTerrainEntity(MTerrainPlugin* plugin, MLayer* pmLayer)
    {
        MEntity* pmTerrain = NULL;
        plugin->GetTerrainEntity(pmTerrain);
        if (pmTerrain == NULL || pmTerrain->Layer != pmLayer)
        {
            return NULL;
        }
        return pmTerrain;
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnLayerSaving(MLayer* pmLayer, bool bAutoSave)
{
    // Save packages?
    if (!bAutoSave && GetSurfaceCommandPanel() &&
        GetSurfaceCommandPanel()->IsDirty())
    {
        GetSurfaceCommandPanel()->SaveAll();
    }

    MEntity* pmTerrain = FindTerrainEntity(this, pmLayer);
    if (pmTerrain == NULL)
    {
        return;
    }

    String* pmFilename = pmLayer->Name;
    String* pmSeparator = S"\\";
    String* pmFilePath __gc[] = pmFilename->Split(pmSeparator->ToCharArray());
    pmSeparator = "";
    for (int i = 0; i < pmFilePath->Length - 1 ; ++i)
    {
        pmSeparator = String::Concat(pmSeparator, pmFilePath[i], S"\\");
    }

    NiFixedString kTerrainComponentClassName("NiTerrainComponent");
    NiTerrainComponent* pkComponent = NULL;
    NiEntityInterface* pkTerrain = pmTerrain->GetNiEntityInterface();
    const unsigned int uiComponentCount = pkTerrain->GetComponentCount();
    for (unsigned int ui = 0; ui < uiComponentCount; ++ui)
    {
        if (pkTerrain->GetComponentAt(ui)->GetClassName() ==
            kTerrainComponentClassName)
        {
            pkComponent = (NiTerrainComponent*) pkTerrain->GetComponentAt(ui);
            break;
        }
    }
    NIASSERT(pkComponent);

    NiFixedString kArchiveName;
    NIVERIFY(pkComponent->GetPropertyData(
        NiTerrainComponent::ms_kPropStorageFileName, kArchiveName));
    if (!kArchiveName)
    {
        // Set the archive to the default one
        kArchiveName = MStringToCharPointer(pmSeparator);
        if (!bAutoSave)
        {
            while (!ValidateArchiveDirectory(
                (const char*)kArchiveName))
            {
                MTerrainDirectoryPathEditor* pmPath = 
                    new MTerrainDirectoryPathEditor();
                kArchiveName = 
                    MStringToCharPointer(dynamic_cast<String*>
                    (pmPath->EditValue(NULL, NULL, pmSeparator)));
            }

            pkComponent->SetPropertyData(
                NiTerrainComponent::ms_kPropStorageFileName,
                kArchiveName);
        }
    }
    NIASSERT(kArchiveName);

    if (bAutoSave)
    {
        kArchiveName = MStringToCharPointer(
            String::Concat(kArchiveName, BACKUP_DIRECTORY));

        pkComponent->SetPropertyData(
            NiTerrainComponent::ms_kPropStorageFileName, kArchiveName);
    }

    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    NiBool bResult = pkComponent->Save(kArchiveName, spErrors);
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        spErrors);

    bool bCookPhysXData;
    pkComponent->GetPropertyData(
        NiTerrainComponent::ms_kPropCookPhysXData, 
        (bool&)bCookPhysXData, 0);

    if (bCookPhysXData)
    {
        if (m_bPhysXPluginLoaded)
        {
            IUICommandService* pmCommandService = 
                MGetService(IUICommandService);
            UICommand* pmCommand = 
                pmCommandService->GetCommand(COOK_TERRAIN_PHYSX_DATA);
            UICommandEventArgs* pmCommandEventArgs = 
                new UICommandEventArgs;
            
            pmCommandEventArgs->SetValue(S"Entity", 
                pkComponent->GetUniqueComponentName());
            if (pmCommand)
            {
                pmCommand->DoClick(0, pmCommandEventArgs);
            }
        }
        else
        {
            IMessageService* pmMessages = MGetService(IMessageService);
            pmMessages->AddMessage(MessageChannelType::General, 
                "Could not save Terrain PhysX data as the PhysX plugin"
                " is not available");
        }
    }
    
    if (bResult && !bAutoSave)
    {
        DeleteBackupFiles(pkComponent);
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnLayerSaved(MLayer* pmLayer, bool bAutoSave)
{
    // If this was an autosave change the property back
    if (bAutoSave)
    {
        MEntity* pmTerrain = FindTerrainEntity(this, pmLayer);
        if (pmTerrain == NULL)
        {
            return;
        }

        NiFixedString kTerrainComponentClassName("NiTerrainComponent");
        NiTerrainComponent* pkComponent = NULL;
        NiEntityInterface* pkTerrain = pmTerrain->GetNiEntityInterface();
        const unsigned int uiComponentCount = pkTerrain->GetComponentCount();
        for (unsigned int ui = 0; ui < uiComponentCount; ++ui)
        {
            if (pkTerrain->GetComponentAt(ui)->GetClassName() ==
                kTerrainComponentClassName)
            {
                pkComponent = (NiTerrainComponent*) pkTerrain->GetComponentAt(
                    ui);
                break;
            }
        }
        NIASSERT(pkComponent);

        NiFixedString kArchiveName;
        NIVERIFY(pkComponent->GetPropertyData(
            NiTerrainComponent::ms_kPropStorageFileName, kArchiveName));

        // Check to see if we just loaded an autosaved archive
        String* pmName = kArchiveName;
        String* pmBackup = BACKUP_DIRECTORY;
        if (pmName->Contains(pmBackup))
        {
            kArchiveName = MStringToCharPointer(
                pmName->Substring(0, pmName->get_Length() - 
                pmBackup->get_Length()));

            pkComponent->SetPropertyData(
                NiTerrainComponent::ms_kPropStorageFileName,
                kArchiveName);
        }
    }
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnDeletedAutoSave(String* /*pmFilename*/)
{
    MEntity* pmTerrain = NULL;
    GetTerrainEntity(pmTerrain);
    if (pmTerrain == NULL)
    {
        return;
    }

    NiFixedString kTerrainComponentClassName("NiTerrainComponent");
    NiTerrainComponent* pkComponent = NULL;
    NiEntityInterface* pkTerrain = pmTerrain->GetNiEntityInterface();
    const unsigned int uiComponentCount = pkTerrain->GetComponentCount();
    for (unsigned int ui = 0; ui < uiComponentCount; ++ui)
    {
        if (pkTerrain->GetComponentAt(ui)->GetClassName() ==
            kTerrainComponentClassName)
        {
            pkComponent = (NiTerrainComponent*) pkTerrain->GetComponentAt(
                ui);
            break;
        }
    }
    NIASSERT(pkComponent);

    DeleteBackupFiles(pkComponent);
}
//---------------------------------------------------------------------------
void MTerrainPlugin::OnPaletteAdded(MPalette* pmPalette)
{
    // We want to make sure the palette we load does not contain an entity 
    // with a NiTerrainSectorComponent.
    MEntity* pmEntities[] = pmPalette->GetEntities();
    bool bChanged = false;

    MPaletteManager* pmPaletteManager = 
        MFramework::Instance->PaletteManager;        

    for (NiUInt32 i = 0; i < pmPalette->EntityCount; ++i)
    {    
        if (HandleComponentDeprecation(pmEntities[i]))
        {
            bChanged = true;
        }
    }
    
    if (bChanged)
    {
        pmPalette->Scene->Dirty = true;

        MPalette* pmOldPalette = pmPaletteManager->ActivePalette;
        pmPaletteManager->ActivePalette = pmPalette;
        pmPaletteManager->SavePalette(pmPalette, 
            String::Concat(pmPaletteManager->PaletteFolder, pmPalette->Name,
            S".pal"), S"GSA");
        pmPaletteManager->ActivePalette = pmOldPalette;
        
        String* pmMsgText(S"A template using the "
            S"deprecated NiTerrainSectorComponent was found in a palette.");
        String* pmDescText(S"References to the NiTerrainSectorComponent has "
            S"been removed from the palette and the palette was saved."
            );
        Message* pmMessage = new Message(pmMsgText, pmDescText, 
            pmPalette->Name);
        
        IMessageService* pmMessageService = MGetService(IMessageService);
        pmMessageService->AddMessage(MessageChannelType::Errors, 
            pmMessage);
    }

    

}
//---------------------------------------------------------------------------
bool MTerrainPlugin::EnsureValidSceneForEntity(MEntity* pmEntity)
{
    if (pmEntity->HasProperty(NiTerrainSectorComponent::ms_kPropTerrainID))
    {
        NiTerrainComponent* pkThisComponent = 
            NiTerrainComponent::FindTerrainComponent(
            pmEntity->GetNiEntityInterface());
        if (pkThisComponent && pkThisComponent->IsTemplate())
            return true;

        // Loop through all terrain components
        NiTListIterator iterator =
            NiTerrainComponent::GetTerrainComponents()->GetHeadPos();

        while(iterator)
        {
            // Find the first terrain component in the scene and add set
            // the sector property accordingly
            NiTerrainComponent* pkComponent = 
                NiTerrainComponent::GetTerrainComponents()->GetNext(iterator);
            
            if (!pkComponent->IsTemplate())
            {
                pmEntity->GetNiEntityInterface()->SetPropertyData(
                    NiTerrainSectorComponent::ms_kPropTerrainID,
                    pkComponent->GetUniqueComponentName());

                // Update to add the sector component to the terrain component
                pmEntity->Update(0.0, 
                    MFramework::Instance->ExternalAssetManager);

                MComponent* pmComponents[] = pmEntity->GetComponents();
                int iComponentCount = pmComponents->Count;
                for (int iComponentIndex = 0; 
                    iComponentIndex < iComponentCount; 
                    iComponentIndex++)
                {
                    MComponent* pmComponent = pmComponents[iComponentIndex];
                    NiEntityComponentInterface* pkComponent = 
                        pmComponent->GetNiEntityComponentInterface();
                    NiFixedString kClassName = pkComponent->GetClassName();
                    if (kClassName == "NiTerrainSectorComponent")
                    {
                        NiTerrainSectorComponent* pkSectorComponent =
                            (NiTerrainSectorComponent*)pkComponent;

                        pkSectorComponent->GetSector()->CreateBlankGeometry();
                    }
                }
               
                return true;
            }          
        }
        // if we get here, there are no terrain component in the scene
        // Therefore we will not create a terrain
        return false;
    }
    
    if (pmEntity->HasProperty(NiTerrainComponent::ms_kPropSectorList))
    {
        NiEntityInterface* pkInterface = pmEntity->GetNiEntityInterface();
        NiObject* pkObject = 0;
        pkInterface->GetPropertyData(NiTerrainComponent::ms_kPropSceneRoot,
            (NiObject*&)pkObject);

        NiTerrain* pkTerrain = NiDynamicCast(NiTerrain, pkObject);
        if (pkTerrain)
        {
            pmEntity->Update(0.0f, MFramework::Instance->ExternalAssetManager);

            NiUInt32 uiCount = pmEntity->GetElementCount(
                NiTerrainComponent::ms_kPropSectorList);
            for (NiUInt32 uiIndex = 0; uiIndex < uiCount; ++uiIndex)
            {
                NiPoint2 kIndex;
                pkInterface->GetPropertyData(
                    NiTerrainComponent::ms_kPropSectorList,
                    kIndex, uiIndex);

                InitialiseNewSector(pkTerrain, (NiInt16)kIndex.x, 
                    (NiInt16)kIndex.y);
            }    
        }
        return true;
    }

    return true;
}
//---------------------------------------------------------------------------
System::Void MTerrainPlugin::EntitySelected(MEntity* pmEntity)
{
    if(pmEntity)
    {
        NiTerrainComponent *pkTerrainComponent = 0;
        NiEntityInterface *pkEntityInterface = 
            pmEntity->GetNiEntityInterface();
        for(unsigned int i=0;i<pkEntityInterface->GetComponentCount();i++)
        {
            NiEntityComponentInterface *pkComponent = 
                pkEntityInterface->GetComponentAt(i);
            if(pkComponent->GetClassName() == 
                NiFixedString("NiTerrainComponent"))
            {
                pkTerrainComponent = (NiTerrainComponent*)pkComponent;
                break;
            } 
        }
        
        if(pkTerrainComponent)
        {
            m_pmSelectedTerrain = pmEntity;
            
            // Enable the terrain command panel and brush:
            m_pmCommandPanel->SetEntityLabel(pmEntity->Name, Color::Black);
            m_pmCommandPanel->UseSurfacePalette
                (pkTerrainComponent->GetActiveSurfacePalette());
            MBrushOperationFactory::GetInstance()->Update();

            m_pmSurfaceCommandPanel->UseSurfacePalette
                (pkTerrainComponent->GetActiveSurfacePalette());
        }
        else
        {
            m_pmCommandPanel->SetEntityLabel(pmEntity->Name, Color::Red);
        }

        // Set the brush scale. Assumes that if the terrain doesn't have the
        // 'scale' property, it wont change the value of fScale from 1.0f.
        float fScale = 1.0f;
        pkEntityInterface->GetPropertyData("Scale", fScale, 0);

        GetBrush()->SetScale(fScale);
    }
    else
    {
        m_pmCommandPanel->SetEntityLabel(S"None", Color::Red);
        m_pmSelectedTerrain = 0;
    }
}
//---------------------------------------------------------------------------
bool MTerrainPlugin::HandleComponentDeprecation(MEntity* pmEntity)
{
    bool bResult = false;
    NiTerrainSectorComponent* pkSectorComponent = 
        NiNew NiTerrainSectorComponent();

    NiTerrainSectorComponent* pkComponent = 
        (NiTerrainSectorComponent*)(
        pmEntity->GetNiEntityInterface()->GetComponentByTemplateID(
        pkSectorComponent->GetTemplateID()));        

    if (pkComponent)
    {   
        char acMsg[512];
        NiSprintf(acMsg, 512, "A deprecated component was found in entity "
            "%s in %s and has automatically been removed.\n "
            "Would you like to save the file to reflect this change?\n"
            "It is recommended to back up the file before saving.",
            pmEntity->Name, pmEntity->SourceFilename);
       
        if (MessageBox::Show(acMsg, "Deprecated component found", 
            MessageBoxButtons::YesNo,
            MessageBoxIcon::Question) == DialogResult::Yes)
        {
            // The return value is set to true so we know to save the changes
            bResult = true;
        }

        // Move sector component properties to the terrain component
        NiUInt32 uiMaskSize;
        pkComponent->GetPropertyData(
            NiTerrainComponent::ms_kPropMaskSize, uiMaskSize);
        pmEntity->GetNiEntityInterface()->SetPropertyData(
            NiTerrainComponent::ms_kPropMaskSize, uiMaskSize);

        // Now remove the sector component
        pmEntity->GetNiEntityInterface()->RemoveComponent(pkComponent, false); 
        pmEntity->Dirty = true;   
    }

    NiDelete pkSectorComponent;
    return bResult;
}
//---------------------------------------------------------------------------
//This function returns a pointer to this brush.
MBrush* MTerrainPlugin::GetBrush()
{
    assert(m_pmBrush);
    return m_pmBrush;
}
//---------------------------------------------------------------------------
TerrainCommandPanel* MTerrainPlugin::GetTerrainCommandPanel()
{
    assert(m_pmCommandPanel);
    return m_pmCommandPanel;
}
//---------------------------------------------------------------------------
SurfaceCommandPanel* MTerrainPlugin::GetSurfaceCommandPanel()
{
    assert(m_pmSurfaceCommandPanel);
    return m_pmSurfaceCommandPanel;
}
//---------------------------------------------------------------------------
System::ComponentModel::ComponentResourceManager* 
    MTerrainPlugin::GetResourceManager()
{
    MTerrainPlugin* pmInstance = GetInstance();

    if (!pmInstance->m_pmResourceManager)
    {        
        pmInstance->m_pmResourceManager = 
            new System::ComponentModel::ComponentResourceManager(
            __typeof(MTerrainPlugin));
    }

    return pmInstance->m_pmResourceManager;
}
//---------------------------------------------------------------------------
MTerrainPlugin* MTerrainPlugin::GetInstance()
{
    assert(ms_pmInstance);
    return ms_pmInstance;
}
//---------------------------------------------------------------------------
void MTerrainPlugin::GetTerrainEntity(MEntity*& pmEntity)
{
    if (!m_pmSelectedTerrain)
        SelectClosestTerrain();

    pmEntity = m_pmSelectedTerrain;    
}

//---------------------------------------------------------------------------
void MTerrainPlugin::SetTerrainEntity(MEntity* pmEntity)
{
    if (pmEntity != m_pmSelectedTerrain)
    {
        EntitySelected(pmEntity);
    }
}

//---------------------------------------------------------------------------

//The following is related to a bug in VC7.1/.Net CLR 1.1
// see http://support.microsoft.com/?id=814472

void MTerrainPlugin::InitStatics()
{
#if _MSC_VER == 1310
    __crt_dll_initialize();
#endif
}
//---------------------------------------------------------------------------
void MTerrainPlugin::ShutdownStatics()
{
#if _MSC_VER == 1310
    //__crt_dll_terminate();
#endif
}
//---------------------------------------------------------------------------
void MTerrainPlugin::DeleteBackupFiles(NiTerrainComponent* pkComponent)
{
    // Delete all the backup files 
    DirectoryInfo* pmDir;
    NiFixedString kArchiveName;
    if (pkComponent)
    {
        pkComponent->GetPropertyData(
            NiTerrainComponent::ms_kPropStorageFileName, kArchiveName);

        // Delete autosave
        if (kArchiveName)
        {
            String* pmPath = String::Concat(kArchiveName, BACKUP_DIRECTORY);
            pmDir = new DirectoryInfo(pmPath);

            if (pmDir->Exists)
            {
                pmDir->Delete(true);
            }
            pmDir = 0;
        }
    }
}
//---------------------------------------------------------------------------
