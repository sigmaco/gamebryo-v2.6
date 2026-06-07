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

#include <NiViewMath.h>
#include <NiPick.h>
#include <NiLines.h>
#include "MTerrainPlugin.h"

#include "MBrush.h"

//Supported Brush Shapes:
#include "MBrushTypeSquare.h"
#include "MBrushTypeCircle.h"

//Supported Brush Elements:
#include "MBrushElementSculpt.h"
#include "MBrushElementSurface.h"

//Supported Brush Operations:
#include "MBrushOperationAlign.h"
#include "MBrushOperationElevate.h"
#include "MBrushOperationFlatten.h"

//Supported Brush Falloffs:
#include "MFalloffLinear.h"
#include "MFalloffSine.h"

//Brush commands
#include "MBrushCommand.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

/// holding array for vertices to modify
NiDeformablePointSetPtr g_spActiveDataPoints = 0; 

// Static Member initialisation
void MBrush::_SDMInit(void)
{   
    //Initialise the brush types
    ms_pmRegisteredBrushTypes = 
        new System::Collections::Generic::List<MBrushType*>();

    // Sculpt Falloff Elements
    ms_pmBrushFalloffLinear = new System::String("Linear");
    ms_pmBrushFalloffSine = new System::String("Sine");
}
//---------------------------------------------------------------------------
void MBrush::_SDMShutdown(void)
{
    for (NiInt32 i  = 0; i < ms_pmRegisteredBrushTypes->Count; ++i)
    {
        ms_pmRegisteredBrushTypes->get_Item(i)->Dispose();
    }
    ms_pmRegisteredBrushTypes->Clear();
    ms_pmRegisteredBrushTypes = 0;
}
//---------------------------------------------------------------------------
void MBrush::_RegisterBrushType(MBrushType* pmBrushType)
{
    ms_pmRegisteredBrushTypes->Add(pmBrushType);
}
//---------------------------------------------------------------------------
void MBrush::RegisterCustomSettings(String* pmOptionCategoryName,
    String* pmOptionName, 
    String* pmOptionDescription,
    System::UInt32& uiValue,
    SettingChangedHandler* pmHandler)
{
    ISettingsService* pmSettingService = MGetService(ISettingsService);
    // first register the setting in case it doesn't already exist
    pmSettingService->RegisterSettingsObject(pmOptionName, 
        __box(uiValue), 
        SettingsCategory::PerUser);
    // set the event handler for when that setting changes

    if (pmHandler != NULL)
    {
        pmSettingService->SetChangedSettingHandler(pmOptionName, 
            SettingsCategory::PerUser, 
            pmHandler);
    }
    // get the value of the setting in case it existed before and cache it
    Object* pmObj;
    __box NiUInt32* pmValue;
    pmObj = pmSettingService->GetSettingsObject(pmOptionName, 
        SettingsCategory::PerUser);

    IOptionsService* pmOptionService = MGetService(IOptionsService);

    pmValue = dynamic_cast<__box NiUInt32*>(pmObj);
    if (pmValue != NULL)
    {
        uiValue = (*pmValue);
             
        pmOptionService->AddOption(pmOptionCategoryName, 
            SettingsCategory::PerUser, pmOptionName); 
        
        pmOptionService->SetHelpDescription(pmOptionCategoryName, 
            pmOptionDescription);
        
    }

    // register the brush type settings
    for (NiInt32 i = 0; i < ms_pmRegisteredBrushTypes->Count; ++i)
    {
        ms_pmRegisteredBrushTypes->get_Item(i)->RegisterBrushTypeOptions();
    }
}
//---------------------------------------------------------------------------
MBrush::MBrush() :
    m_pmActiveFalloff(NULL)
{
    g_spActiveDataPoints = NiNew NiDeformablePointSet(
        NiDeformablePoint::VERTEX);
    g_spActiveDataPoints->GrowTo(12000);
    m_pkPassPointInfo = NiNew NiBrushPassPointInfo();
    m_bCommandFrame = false;
    m_bResizeOverlaySuccess = false;

    m_pmListOfOperations = new System::
        Collections::Generic::List<MBrushOperation*>;

    m_iCurrentPosition = 0;
    m_iNumberOfOrigins = 0;
    m_iMaxOrigins = 1000;
    m_pkListOfOrigins = NiNew NiPoint3[m_iMaxOrigins];
    m_uiMaxNumberOfStrokes = 20;
    m_uiMaxStrokeSize = 1200000;
    m_uiNumWarningPoints = 800000;
    m_uiCurrentBrushType = 0;
    m_bUpdateLightingRealtime = false;

    m_pmPreviousCommand = NULL;

    // Adds an event when a set of comands are added to the redo and undo 
    // stacks. This will allow us to determine when an operation was finished.

    ICommandService* mCommandService = MGetService(ICommandService);
    mCommandService->CommandsAddedToRedoStack += new UndoRedoHandler(this,
        &MBrush::CommandsFinished);
    mCommandService->CommandsAddedToUndoStack += new UndoRedoHandler(this,
        &MBrush::CommandsFinished);

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MBrush::OnSettingChanged);
    
    RegisterCustomSettings("Terrain editing options.Terrain undo depth",
        "Terrain undo depth", 
        "Maximum number of strokes held in the undo list",
        m_uiMaxNumberOfStrokes, pmHandler);
    RegisterCustomSettings("Terrain editing options.Max points per stroke",
        "Max points per stroke", 
        "Maximum number of points modified in one stroke. The size of one "
        "point kept in the undo lost is roughly 45 bytes.",
        m_uiMaxStrokeSize, pmHandler);
    RegisterCustomSettings("Terrain editing options."
        "Number of points before warning",
        "Number of points before warning", 
        "The number of points that can be modified before the brush color"
        " changes to its warning color",
        m_uiNumWarningPoints, pmHandler);
}

//===========================================================================
// MDispose implementation 
//===========================================================================

void MBrush::Do_Dispose(bool bDisposing)
{
    __super::Do_Dispose(bDisposing);
    g_spActiveDataPoints = 0;    
    NiDelete(m_pkPassPointInfo);
    
    if (m_pmActiveFalloff)
    {
        m_pmActiveFalloff->Dispose();
        m_pmActiveFalloff = 0;
    }

    if (bDisposing)
    {
        m_pmListOfOperations->Clear();
        m_pmListOfOperations = 0;
    }

    NiDelete [] m_pkListOfOrigins;
}
//---------------------------------------------------------------------------
bool MBrush::SetBrushType(NiUInt32 uiBrushTypeIndex, 
    const NiFixedString& kBrushType)
{
    if ((int)uiBrushTypeIndex >= ms_pmRegisteredBrushTypes->Count)
    {
        NiMessageBox(NiString("Unknown Brush type: ") + kBrushType, 
            "MBrush::SetBrushType");
        return false;
    }
    
    m_uiCurrentBrushType = uiBrushTypeIndex;

    return true;
}
//---------------------------------------------------------------------------
bool MBrush::SetSculptFalloff(const NiFixedString& kFalloffType)
{ 
    // set the fall off method for sculpting
    if (kFalloffType == MStringToCharPointer(ms_pmBrushFalloffLinear))
    {
        m_pmActiveFalloff = new MFalloffLinear();
    }
    else if (kFalloffType == MStringToCharPointer(ms_pmBrushFalloffSine))
    {
        m_pmActiveFalloff = new MFalloffSine();
    }
    else
    {
        NiMessageBox("Unknown Sculpt Falloff type", 
            "MBrush::SetSculptFalloff");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
MBrushType* MBrush::GetBrushType()
{
    if ((int)m_uiCurrentBrushType >= ms_pmRegisteredBrushTypes->Count)
        return NULL;

    return ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType);
}
//---------------------------------------------------------------------------
System::Collections::Generic::List<MBrushOperation*>* MBrush::GetOperations()
{
    return m_pmListOfOperations;
}
//---------------------------------------------------------------------------
void MBrush::CommandsFinished(ICommand* amCommands[])
{
    bool bFound = false;

    for(int i = 0; i < amCommands->Count; i++)
    {
        if(amCommands[i]->Name->Equals("Brush Operation"))
        {
            bFound = true;
            break;
        }
    }

    if(bFound)
    {
        // Update the normals and tangents
        MEntity* pmEntity = 0;
        NiTerrainInteractor* pkInteractor = 0;
        size_t fSize = 0;
        
        MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
        if(pmEntity)
        {
            pmEntity->GetNiEntityInterface()->
                GetPropertyData("Interactor", (void*&)pkInteractor, fSize, 0);

            pkInteractor->RebuildChangedLighting();
            pmEntity->Update(0.0f, MFramework::Instance->ExternalAssetManager);
        }
    }
}
//---------------------------------------------------------------------------
bool MBrush::MoveOperationTo(MBrushOperation* pmOperation, int iIndex)
{
    int iWhereItIs = m_pmListOfOperations->IndexOf(pmOperation);

    if(iWhereItIs != iIndex - 1)
    {
        m_pmListOfOperations->Remove(pmOperation);
        m_pmListOfOperations->Insert(iIndex - 1, pmOperation);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
// just find where the ray has hit the terrain.
NiBool MBrush::GetTerrainIntersection(float fPixelX, float fPixelY, 
    NiPoint3* pkIntersection)
{
    NiBool bTerrainSelected = false;
    NiPoint3 kOrigin, kDir;
    NiPoint3 kClosestIntersectionNormal;

    if(!m_pmActiveViewport)
    {
        m_pmActiveViewport = 
            MFramework::Instance->ViewportManager->ActiveViewport;
    }
   
    NiViewMath::MouseToRay((float) fPixelX, (float) fPixelY, 
        m_pmActiveViewport->Width,
        m_pmActiveViewport->Height,
        m_pmActiveViewport->GetNiCamera(), 
        kOrigin, kDir);
    MEntity* pmEntity = 0;
    bool bFound = false;
    
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    
    // setup a terrain ray pointer
    NiRay kRay(kOrigin, kDir);

    // see if terrain ray collided with ray
    if (pmEntity)
    {        
        NiTerrainInteractor* pkInteractor = 0;
        size_t fSize = 0;
                
        NiEntityInterface* pkTemp = pmEntity->GetNiEntityInterface();
        if(pkTemp)
        {
            pkTemp->GetPropertyData(NiTerrainComponent::ms_kPropInteractor,
                (void*&)pkInteractor, fSize, 0);

            bTerrainSelected = pkInteractor->Collide(kRay);
            if(bTerrainSelected)
            {
                // ray hit terrain. 
                // get intesection point                
                kRay.GetIntersection(*pkIntersection, 
                    kClosestIntersectionNormal);
                bFound = true;
            }
        }
    }
    
    if(!bFound)
    {
        NiPoint3 kClosestIntersection;
        NiPoint3 kLatestIntersection;
        NiPoint3 kLatestIntersectionNormal;

        NiTerrainInteractor* pkInteractor = 0;
        size_t fSize = 0;
        float fSqrDistanceToIntersection = FLT_MAX;

        //Get the scene:
        MScene* pmScene = MFramework::Instance->Scene;
        MEntity* apmEntities[] = pmScene->GetEntities();

        // Loop through all the components and find one that intersects:
        for(unsigned int i = 0; i < pmScene->EntityCount; ++i)
        {
            MEntity* pmCurEntity = (apmEntities)[i];

            pmCurEntity->GetNiEntityInterface()->
                GetPropertyData(NiTerrainComponent::ms_kPropInteractor,
                (void*&)pkInteractor, fSize, 0);

            if (pkInteractor == 0)
                continue;
            
            bTerrainSelected = pkInteractor->Collide(kRay);
            if(bTerrainSelected)
            {
                // ray hit terrain. 
                // get intesection point
                kRay.GetIntersection(kLatestIntersection, 
                    kLatestIntersectionNormal);

                // Check how far the intersection was
                float fSqrDistance = 
                    (kLatestIntersection - kOrigin).SqrLength();
                
                if(fSqrDistance < fSqrDistanceToIntersection)
                {
                    fSqrDistanceToIntersection = fSqrDistance;
                    kClosestIntersection = kLatestIntersection;
                    kClosestIntersectionNormal = kLatestIntersectionNormal;
                    pmEntity = pmCurEntity;
                }
            }
        }
        *pkIntersection = kClosestIntersection;
        bTerrainSelected = !(fSqrDistanceToIntersection >= FLT_MAX);
        if(pmEntity && bTerrainSelected)
        {
            pmEntity = MFramework::Instance->EntityFactory->Get
                (pmEntity->GetNiEntityInterface());
        }

        MTerrainPlugin::GetInstance()->SetTerrainEntity(pmEntity);
    }

    if (bTerrainSelected)
    {
        m_fIntersectionHeight = pkIntersection->z;

        kClosestIntersectionNormal.Unitize();
        float fSlope = NiACos(
            kClosestIntersectionNormal.Dot(NiPoint3::UNIT_Z));
        if (fSlope > NI_HALF_PI)
            fSlope = NI_PI - fSlope;
        m_fIntersectionSlope = fSlope;
    }
    
    return bTerrainSelected;
}
//---------------------------------------------------------------------------
void MBrush::StartPaint(NiPoint3 &kIntersection, 
    IInteractionMode::MouseButtonType eType)
{
    //Don't bother doing anything if variables aren't setup:
    if (m_pmListOfOperations->Count == 0)
    {    
        NiMessageBox("Element, Brush or Operation is not selected", 
            "MBrush::StartPaint");

        return;
    }

    if(!GetBrushType()->ValidateBrushType())
    {
        NiMessageBox(
            "The brush type is not valid. Make sure the inner radius is "
            "not 0 and that other variables are set appropriately.", 
            "MBrush::StartPaint");

        return;
    }

    m_iCurrentPosition = 0;
    m_iNumberOfOrigins = 0;
    m_pmPreviousCommand = 0;
    m_uiStrokeSize = 0;

    for(int i = 0; i<m_pmListOfOperations->Count; i++)
    {
        // Make sure that each operation is up to date
        m_pmListOfOperations->get_Item(i)->Update(false);

        m_pmListOfOperations->get_Item(i)->StartOperation(kIntersection, 
            m_pkPassPointInfo, eType);
    }
}
//---------------------------------------------------------------------------
void MBrush::Paint(NiPoint3& kIntersection, bool)
{
    // Don't bother doing anything if variables arn't setup:
    if (m_pmListOfOperations->Count == 0 || 
        !GetBrushType()->ValidateBrushType())
    {
        //Don't message, we already messaged on "StartPaint"
        return;
    }

    MEntity* pmEntity = 0;
    NiTerrainInteractor* pkInteractor = 0;
    size_t fSize = 0;

    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    pmEntity->GetNiEntityInterface()->
        GetPropertyData("Interactor", (void*&)pkInteractor, fSize, 0);

    // Parse through all operations
    for(int i = 0; i<m_pmListOfOperations->Count; i++)
    {
        MBrushOperation* pmOperation = m_pmListOfOperations->get_Item(i);

        // For each operation create a command
        MBrushCommand* pmCommand = new MBrushCommand(pmEntity);

        pmCommand->InitializeCommandInterface(
            pkInteractor, 
            g_spActiveDataPoints,
            GetBrushType(),
            pmOperation,
            m_pmActiveFalloff,
            m_pkPassPointInfo,
            kIntersection,
            GetSizeOuter(),
            GetScale(),
            m_pkListOfOrigins,
            m_iNumberOfOrigins,
            m_iCurrentPosition - 1);

        if (m_uiStrokeSize < m_uiMaxStrokeSize)
        {
            ICommandService* mCommandService = MGetService(ICommandService);
            mCommandService->ExecuteCommand(pmCommand, true);
            m_uiStrokeSize += pmCommand->GetSize();

            if(m_pmPreviousCommand)
            {
                m_pmPreviousCommand->SetNextCommand(pmCommand);
                pmCommand->SetPreviousCommand(m_pmPreviousCommand);
            }

            m_pmPreviousCommand = pmCommand;
            
            if (m_uiStrokeSize >= m_uiNumWarningPoints)
            {
                ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
                    SetColor("Brush warning color");
            }
        }
        else
        {
            ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
                SetColor("Brush stop color");
            delete (pmCommand);
        }
    }

    // Fills in values for the used origin
    if(m_iMaxOrigins > m_iNumberOfOrigins)
    {
        m_pkListOfOrigins[m_iNumberOfOrigins] = 
            NiPoint3(kIntersection.x, kIntersection.y, kIntersection.z);
        m_iNumberOfOrigins++;
        m_iCurrentPosition++;
    }
    else
    {
        if(m_iMaxOrigins < m_iCurrentPosition + 1)
            m_iCurrentPosition = 0;

        m_pkListOfOrigins[m_iCurrentPosition] = 
            NiPoint3(kIntersection.x, kIntersection.y, kIntersection.z);
        m_iCurrentPosition++;
    }

    // We now randomize the brush
    GetBrushType()->Randomize();
    // Update the lighting if required:
    if (m_bUpdateLightingRealtime)
        pkInteractor->RebuildChangedLighting();
}
//---------------------------------------------------------------------------
void MBrush::StopPaint()
{
    ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
        SetColor("Brush allowed color");

    //Don't bother doing anything if variables arn't setup:
    if(m_pmListOfOperations->Count == 0)
    {
        //Don't message, we already messaged on "StartPaint"
        return;
    }

    //Tell each element to stop painting:
    for(int i = 0; i<m_pmListOfOperations->Count; i++)
    {
        m_pmListOfOperations->get_Item(i)->StopOperation();
    }

    // check the number of operation brush undo command are in the list and 
    // delete the earlier ones
    ICommandService* mCommandService = MGetService(ICommandService);
    ICommand* pmCommands[] = mCommandService->GetUndoCommands();

    unsigned int uiNumBrushOp = 0;
    for (unsigned int i = 0; i < mCommandService->UndoStackCount; i++)
    {
        if (String::Compare(pmCommands[i]->Name, "Brush Operation") == 0)
        {
            uiNumBrushOp++;
            if (uiNumBrushOp >= m_uiMaxNumberOfStrokes)
            {
                mCommandService->RemoveFromUndoRedoStack(pmCommands[i]);
            }

        }
    }
}
//---------------------------------------------------------------------------
String* MBrush::get_Name()
{
    return "MBrush";
}
//---------------------------------------------------------------------------
bool MBrush::Initialize()
{
    return true;
}
//---------------------------------------------------------------------------
void MBrush::RenderGizmo(MRenderingContext* pmRenderingContext)
{
    MEntity* pmEntity = 0;

    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);

    NiPoint3 kIntersection;

    m_bTerrainSelected = GetTerrainIntersection((float) m_iLastX, 
        (float) m_iLastY, &kIntersection);

    if (pmEntity && GetBrushType() && m_bTerrainSelected)
    { // draw the brush helper at plane of where mouse hits terrain

        NiTerrainInteractor* pkInteractor = 0;
        size_t fSize = 0;

        pmEntity->GetNiEntityInterface()->
        GetPropertyData("Interactor", (void*&)pkInteractor, fSize, 0);

        if (!m_bResizeOverlaySuccess)
            UpdateOverlay();

        ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            UpdateBrushOverlay(
            kIntersection, 
            pkInteractor, 
            m_pkPassPointInfo);

        NiEntityRenderingContext* pkRenderingContext = 
            pmRenderingContext->GetRenderingContext();

        pkRenderingContext->m_pkCullingProcess->Cull(
            pkRenderingContext->m_pkCamera, 
            ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            GetBrushOverlay(),
            pkRenderingContext->m_pkCullingProcess->GetVisibleSet());

        NiDrawScene(pkRenderingContext->m_pkCamera, 
            ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            GetBrushOverlay(),
            *pkRenderingContext->m_pkCullingProcess);
    }
}
//---------------------------------------------------------------------------
void MBrush::MouseDown(MouseButtonType eType, int iX, int iY)
{ // int iX, int iY viewport coordinates

    // set drag flag
    __super::MouseDown(eType, iX, iY);

    // check mouse button
    if(eType == MouseButtonType::LeftButton)
    {
        m_bLeftDown = true;
        OnLeftClick(iX, iY); 
    }
}
//---------------------------------------------------------------------------
void MBrush::MouseUp(MouseButtonType eType, int iX, int iY)
{
    __super::MouseUp(eType, iX, iY);
    // Stop the batch command
    if(eType == MouseButtonType::LeftButton)
    {
        m_bLeftDown = false;
        if(m_bCommandFrame)
        {
            ICommandService* pmCommandService = MGetService(ICommandService);
            pmCommandService->EndUndoFrame(true);
            m_bCommandFrame = false;
        }

        // release drag flag
        if (GetBrushType())
        {
            StopPaint();
        }
    }
}
//---------------------------------------------------------------------------
void MBrush::MouseMove(int iX, int iY)
{    
    // get intersection point on object
    NiPoint3 kIntersection;
    MEntity* pmEntity;

    m_pmActiveViewport = MFramework::Instance->ViewportManager->ActiveViewport;
    
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmEntity);
    m_bTerrainSelected = GetTerrainIntersection((float) iX, 
        (float) iY, &kIntersection);

    if ((m_bTerrainSelected) && (m_bLeftDown) && 
        ((m_iStartX != iX) || (m_iStartY != iY)))
    {
        // dragging
        if (m_bTerrainSelected && GetBrushType())
        {
            Paint(kIntersection,true);
            pmEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
                MFramework::Instance->ExternalAssetManager);

            // Update the terrain
            if(pmEntity)
            {
                pmEntity->Update(0.0, 
                MFramework::Instance->ExternalAssetManager);
            }            
        }
    }
    else 
    {
        __super::MouseMove(iX,iY);
        
    }

    m_iLastX = iX;
    m_iLastY = iY;    
    MFramework::Instance->Update();
    
}
//---------------------------------------------------------------------------
void MBrush::OnLeftClick(int iX, int iY)
{
    // Capture key state
    short sShiftKeyDown = (GetAsyncKeyState(0x10) & 0x8000); //Shift
    short sCtrlKeyDown = (GetAsyncKeyState(0x11) & 0x8000); //Control

    m_iStartX = iX;
    m_iStartY = iY;

    MEntity* pmTerrain = NULL;
    MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrain);
    if (!pmTerrain)
    {
        MessageBox::Show("The terrain entity was invalid.", "Terrain Error",
            MessageBoxButtons::OK, MessageBoxIcon::Error);
        m_bLeftDown = false;
        return;
    }

    // Check all brushes are set
    if (!GetBrushType() || !m_pmActiveFalloff || 
        m_pmListOfOperations->Count == 0)  
    {
        MessageBox::Show("Brush type not set.", "Terrain Error",
            MessageBoxButtons::OK, MessageBoxIcon::Error);
        m_bLeftDown = false;
        return;
    }
    
    // translate the mouse coords in the current viewport to 3D space co-ords
    m_pmActiveViewport = MFramework::Instance->ViewportManager->ActiveViewport;
    
    // get intersection point on object, if on terrain
    NiPoint3 kIntersection;    
    m_bTerrainSelected = GetTerrainIntersection((float) iX, (float) iY, 
        &kIntersection);

    if (m_bTerrainSelected ) 
    {
        // Start the batch command
        if(!m_bCommandFrame)
        {
            ICommandService* pmCommandService = MGetService(ICommandService);
            pmCommandService->BeginUndoFrame(S"Brush Operation");
            m_bCommandFrame = true;
        }

        if(!sCtrlKeyDown && !sShiftKeyDown)
        {
            StartPaint(kIntersection,MouseButtonType::LeftButton);
            Paint(kIntersection,false);
        }
        else if(sCtrlKeyDown && !sShiftKeyDown)
        {
            StartPaint(kIntersection,MouseButtonType::RightButton);
            Paint(kIntersection,false);
        }
        else if(!sCtrlKeyDown && sShiftKeyDown)
        {
            StartPaint(kIntersection,MouseButtonType::MiddleButton);
            Paint(kIntersection,false);
        }
    }
}
//----------------------------------------------------------------------------
void MBrush::OnSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    NI_UNUSED_ARG(pmSender);
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    ISettingsService* pmSettingsService = MGetService(
        ISettingsService);

    Object* pmObj = pmSettingsService->GetSettingsObject(
            strSetting, eCategory);
    __box NiUInt32* pmValue;
    pmValue = dynamic_cast<__box NiUInt32*>(pmObj);
    if (pmValue != NULL)
    {        
        if (String::Compare(strSetting,"Terrain undo depth")==0)
        {
            m_uiMaxNumberOfStrokes = (*pmValue);
        }
        else if (String::Compare(strSetting,"Max points per stroke")==0)
        {
            m_uiMaxStrokeSize = (*pmValue);
        }
        else if (String::Compare(strSetting,"Number of points to warning")==0)
        {
            m_uiNumWarningPoints = (*pmValue);
        }
        
    }
}
//----------------------------------------------------------------------------
void MBrush::UpdateOverlay()
{
    MEntity* pmTerrain;
        MTerrainPlugin::GetInstance()->GetTerrainEntity(pmTerrain);
    
    if (GetBrushType() && pmTerrain)
    {        
        float fScale = 1.0f;
        pmTerrain->GetNiEntityInterface()->
            GetPropertyData("Scale", fScale, 0);
        m_fCurrentScale = fScale;

        m_bResizeOverlaySuccess = 
            ms_pmRegisteredBrushTypes->get_Item(m_uiCurrentBrushType)->
            ResizeBrushOverlay(m_fCurrentScale);
    }
}
//----------------------------------------------------------------------------
void MBrush::Update(float fTime)
{
    __super::Update(fTime);

    // Update the Hover Text if there is any displaying
    if (m_bHovering && m_bTerrainSelected)
    {
        NiString kHoverText;

        float fSlopeDegrees = m_fIntersectionSlope * (180.0f / NI_PI);
        kHoverText.Format("Height: %.1f, Slope: %.1f", 
            m_fIntersectionHeight, fSlopeDegrees);
        m_strHoverText = kHoverText;
    }
}
//----------------------------------------------------------------------------
