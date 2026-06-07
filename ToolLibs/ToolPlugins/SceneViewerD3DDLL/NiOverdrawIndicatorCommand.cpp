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
#include "NiOverdrawIndicatorCommand.h"
#include "NiMain.h"

#if defined (WIN32)
#include <windows.h>
#endif

NiCommandID NiOverdrawIndicatorCommand::ms_kID = 0;
NiWindowRef NiOverdrawIndicatorCommand::m_kWnd = 0;
int NiOverdrawIndicatorCommand::m_iMsgID = 0;
NiMeshScreenElementsPtr NiOverdrawIndicatorCommand::ms_spBlueLevel = NULL;
NiMeshScreenElementsPtr NiOverdrawIndicatorCommand::ms_spGreenLevel = NULL;
NiMeshScreenElementsPtr NiOverdrawIndicatorCommand::ms_spYellowLevel = NULL;
NiMeshScreenElementsPtr NiOverdrawIndicatorCommand::ms_spRedLevel = NULL;
NiMeshScreenElementsPtr NiOverdrawIndicatorCommand::ms_spWarning = NULL;
NiStencilPropertyPtr NiOverdrawIndicatorCommand::ms_spStencil = NULL;
//---------------------------------------------------------------------------
NiOverdrawIndicatorCommand::NiOverdrawIndicatorCommand()
{
    if (ms_spStencil == NULL)
        CreateOverdrawIndicator();
}
//---------------------------------------------------------------------------
bool NiOverdrawIndicatorCommand::Apply(NiSceneViewer* pkViewer)
{
    if (GetStencilProperty(pkViewer->GetMasterScene()) != NULL)
        SetOverdraw(false, pkViewer);
    else
        SetOverdraw(true, pkViewer);

    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiOverdrawIndicatorCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiOverdrawIndicatorCommand::~NiOverdrawIndicatorCommand()
{

}
//---------------------------------------------------------------------------
NiStencilProperty* NiOverdrawIndicatorCommand::GetStencilProperty(
    NiNode* pkScene)
{
    // properties
    NiProperty* pkProp;
    NiPropertyList* pkPropList;

    pkPropList = &pkScene->GetPropertyList();
    NiTListIterator kIter = pkPropList->GetHeadPos();
    while (kIter)
    {
        pkProp = pkPropList->GetNext(kIter);
        if (NiIsKindOf(NiStencilProperty, pkProp))
            return (NiStencilProperty*)pkProp;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiSceneCommand* NiOverdrawIndicatorCommand::Create(NiSceneCommandInfo*)
{
    return NiNew NiOverdrawIndicatorCommand();
}
//---------------------------------------------------------------------------
void NiOverdrawIndicatorCommand::SetOverdraw(bool bEnable, 
    NiSceneViewer* pkSceneViewer)
{
    if (pkSceneViewer == NULL)
        return;
    NiNode* pkMasterScene = pkSceneViewer->GetMasterScene();
    
    if (bEnable)
    {

#if defined (WIN32)
        if (m_kWnd != 0)
            SendMessage(m_kWnd, (UINT) m_iMsgID, TRUE , 0);
#endif
        // We clear to 1 so that we can use 0 as a "known unused" value
        // This is important below when we draw the warning polygon to
        // any pixels that equate to the (unused) 0 stencil value.  In other
        // words, any pixel that reports that it is equal to a stencil value
        // of 0 shows that stencil buffering is not supported.  So, we draw
        // a warning texture to those pixels.  In general, either all pixels
        // or no pixels will equate to 0.
        // The depth complexity at a pixel is equal to the stencil value - 1,
        // unless you consider clearing to be a pixel write, in which case 
        // the stencil value equals the depth complexity

        pkSceneViewer->GetRenderer()->SetStencilClear(1);

        pkSceneViewer->AttachScreenElements(ms_spBlueLevel);
        pkSceneViewer->AttachScreenElements(ms_spGreenLevel);
        pkSceneViewer->AttachScreenElements(ms_spYellowLevel);
        pkSceneViewer->AttachScreenElements(ms_spRedLevel);
        pkSceneViewer->AttachScreenElements(ms_spWarning);

        if (pkMasterScene)
        {
            pkMasterScene->AttachProperty(ms_spStencil);
            pkMasterScene->UpdateProperties();
        }
    }
    else
    {
#if defined (WIN32)
        if (m_kWnd != 0)
            SendMessage(m_kWnd, (UINT) m_iMsgID, FALSE , 0);
#endif

        pkSceneViewer->DetachScreenElements(ms_spBlueLevel);
        pkSceneViewer->DetachScreenElements(ms_spGreenLevel);
        pkSceneViewer->DetachScreenElements(ms_spYellowLevel);
        pkSceneViewer->DetachScreenElements(ms_spRedLevel);
        pkSceneViewer->DetachScreenElements(ms_spWarning);

        if (pkMasterScene)
        {
            pkMasterScene->DetachProperty(ms_spStencil);
            pkMasterScene->UpdateProperties();
        }
    }
}
//---------------------------------------------------------------------------
void NiOverdrawIndicatorCommand::CreateOverdrawIndicator()
{
    // The material color is to be used directly
    NiVertexColorProperty* pkVert = NiNew NiVertexColorProperty;
    pkVert->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    pkVert->SetSourceMode(NiVertexColorProperty::SOURCE_IGNORE);

    ms_spBlueLevel = NiNew NiMeshScreenElements(false, false, 0);
    ms_spBlueLevel->Insert(4);
    ms_spBlueLevel->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    ms_spBlueLevel->UpdateBound();

    ms_spGreenLevel = NiNew NiMeshScreenElements(false, false, 0);
    ms_spGreenLevel->Insert(4);
    ms_spGreenLevel->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    ms_spGreenLevel->UpdateBound();

    ms_spYellowLevel = NiNew NiMeshScreenElements(false, false, 0);
    ms_spYellowLevel->Insert(4);
    ms_spYellowLevel->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    ms_spYellowLevel->UpdateBound();

    ms_spRedLevel = NiNew NiMeshScreenElements(false, false, 0);
    ms_spRedLevel->Insert(4);
    ms_spRedLevel->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    ms_spRedLevel->UpdateBound();

    ms_spWarning = NiNew NiMeshScreenElements(false, false, 1);
    ms_spWarning->Insert(4);
    ms_spWarning->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    ms_spWarning->UpdateBound();
    ms_spWarning->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);

    NiMaterialPropertyPtr spMaterial = NiNew NiMaterialProperty;
    spMaterial->SetEmittance(NiColor(0.0f, 0.0f, 1.0f));

    // This screen polygon will be drawn to any pixels that have 1 or
    // fewer 3D polygon writes from the scene
    NiStencilPropertyPtr spStencil = NiNew NiStencilProperty;
    spStencil->SetStencilOn(true);
    spStencil->SetStencilReference(3);
    spStencil->SetStencilFunction(NiStencilProperty::TEST_GREATER);
    spStencil->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassZFailAction(NiStencilProperty::ACTION_KEEP);

    ms_spBlueLevel->AttachProperty(spMaterial);
    ms_spBlueLevel->AttachProperty(spStencil);
    ms_spBlueLevel->AttachProperty(pkVert);

    spMaterial = NiNew NiMaterialProperty;
    spMaterial->SetEmittance(NiColor(0.0f, 1.0f, 0.0f));

    // This screen polygon will be drawn to any pixels that have 2 or
    // more 3D polygon writes from the scene
    spStencil = NiNew NiStencilProperty;
    spStencil->SetStencilOn(true);
    spStencil->SetStencilReference(3);
    spStencil->SetStencilFunction(NiStencilProperty::TEST_LESSEQUAL);
    spStencil->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassZFailAction(NiStencilProperty::ACTION_KEEP);

    ms_spGreenLevel->AttachProperty(spMaterial);
    ms_spGreenLevel->AttachProperty(spStencil);
    ms_spGreenLevel->AttachProperty(pkVert);

    spMaterial = NiNew NiMaterialProperty;
    spMaterial->SetEmittance(NiColor(1.0f, 1.0f, 0.0f));

    // This screen polygon will be drawn to any pixels that have 3 or
    // more 3D polygon writes from the scene
    spStencil = NiNew NiStencilProperty;
    spStencil->SetStencilOn(true);
    spStencil->SetStencilReference(4);
    spStencil->SetStencilFunction(NiStencilProperty::TEST_LESSEQUAL);
    spStencil->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassZFailAction(NiStencilProperty::ACTION_KEEP);

    ms_spYellowLevel->AttachProperty(spMaterial);
    ms_spYellowLevel->AttachProperty(spStencil);
    ms_spYellowLevel->AttachProperty(pkVert);

    spMaterial = NiNew NiMaterialProperty;
    spMaterial->SetEmittance(NiColor(1.0f, 0.0f, 0.0f));

    // This screen polygon will be drawn to any pixels that have 4 or
    // more 3D polygon writes from the scene
    spStencil = NiNew NiStencilProperty;
    spStencil->SetStencilOn(true);
    spStencil->SetStencilReference(5);
    spStencil->SetStencilFunction(NiStencilProperty::TEST_LESSEQUAL);
    spStencil->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassZFailAction(NiStencilProperty::ACTION_KEEP);

    ms_spRedLevel->AttachProperty(spMaterial);
    ms_spRedLevel->AttachProperty(spStencil);
    ms_spRedLevel->AttachProperty(pkVert);

    // texture for screen space polygon
    NiTexturingProperty* pkText 
        = NiNew NiTexturingProperty("Data/stencilwarn.BMP");
    pkText->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    NiTexturingProperty::Map* pkBase = pkText->GetBaseMap();
    pkBase->SetFilterMode(NiTexturingProperty::FILTER_BILERP);
    pkBase->SetClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    ms_spWarning->AttachProperty(pkText);

    // This test should _always_ fail.  However, if the HW doesn't
    // really support stencil buffering, the polygon will be drawn
    // and it will show the "No Stencil Support" warning on the screen
    spStencil = NiNew NiStencilProperty;
    spStencil->SetStencilOn(true);
    spStencil->SetStencilReference(0);
    spStencil->SetStencilFunction(NiStencilProperty::TEST_EQUAL);
    spStencil->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
    spStencil->SetStencilPassZFailAction(NiStencilProperty::ACTION_KEEP);
    ms_spWarning->AttachProperty(spStencil);

    // The stencil buffer is incremented for all visible pixel writes
    ms_spStencil = NiNew NiStencilProperty;
    ms_spStencil->SetStencilOn(true);
    ms_spStencil->SetStencilFunction(NiStencilProperty::TEST_ALWAYS);
    ms_spStencil->SetStencilFailAction(NiStencilProperty::ACTION_KEEP);
    ms_spStencil->SetStencilPassAction(NiStencilProperty::ACTION_INCREMENT);
    ms_spStencil->SetStencilPassZFailAction(
        NiStencilProperty::ACTION_INCREMENT);

    ms_spBlueLevel->UpdateProperties();
    ms_spBlueLevel->Update(0.0f);        
    NiMesh::CompleteSceneModifiers(ms_spBlueLevel);

    ms_spGreenLevel->UpdateProperties();
    ms_spGreenLevel->Update(0.0f);        
    NiMesh::CompleteSceneModifiers(ms_spGreenLevel);

    ms_spYellowLevel->UpdateProperties();
    ms_spYellowLevel->Update(0.0f);        
    NiMesh::CompleteSceneModifiers(ms_spYellowLevel);

    ms_spRedLevel->UpdateProperties();
    ms_spRedLevel->Update(0.0f);        
    NiMesh::CompleteSceneModifiers(ms_spRedLevel);

    ms_spWarning->UpdateProperties();
    ms_spWarning->Update(0.0f);        
    NiMesh::CompleteSceneModifiers(ms_spWarning);

}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiOverdrawIndicatorCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Overdraw");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiOverdrawIndicatorCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("OVERDRAW",
        NiOverdrawIndicatorCommand::Create);
}
//---------------------------------------------------------------------------
void NiOverdrawIndicatorCommand::RegisterListener(NiWindowRef kWnd, 
    int iMsgID)
{
    m_kWnd = kWnd;
    m_iMsgID = iMsgID;
}
//---------------------------------------------------------------------------
void NiOverdrawIndicatorCommand::CleanUp()
{
    ms_spBlueLevel = NULL;
    ms_spGreenLevel = NULL;
    ms_spYellowLevel = NULL;
    ms_spRedLevel = NULL;
    ms_spWarning = NULL;
    ms_spStencil = NULL;
}
//---------------------------------------------------------------------------
