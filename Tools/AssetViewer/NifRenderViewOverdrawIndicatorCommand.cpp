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

// NifRenderViewOverdrawIndicatorCommand.cpp

#include "stdafx.h"
#include "NifRenderViewOverdrawIndicatorCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"

NiMeshScreenElementsPtr
CNifRenderViewOverdrawIndicatorCommand::ms_spBlueLevel = NULL;
NiMeshScreenElementsPtr 
CNifRenderViewOverdrawIndicatorCommand::ms_spGreenLevel = NULL;
NiMeshScreenElementsPtr 
CNifRenderViewOverdrawIndicatorCommand::ms_spYellowLevel = NULL;
NiMeshScreenElementsPtr 
CNifRenderViewOverdrawIndicatorCommand::ms_spRedLevel = NULL;
NiMeshScreenElementsPtr 
CNifRenderViewOverdrawIndicatorCommand::ms_spWarning = NULL;
NiStencilPropertyPtr 
CNifRenderViewOverdrawIndicatorCommand::ms_spStencil = NULL;

//---------------------------------------------------------------------------
bool CNifRenderViewOverdrawIndicatorCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        m_pkView = pkView;
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            m_spScene = pkDoc->GetSceneGraph();

            if(ms_spStencil == NULL)
                CreateOverdrawIndicator();

            if(GetStencilProperty(m_spScene))
            {
                SetOverdraw(false);
            }
            else
            {
                SetOverdraw(true);
            }
            
            // recreate the render frame with new overdraw settings
            pkView->CreateFrame();
        }
        // No document == error
        else 
            return false;
    }
    // No view == error
    else 
        return false;

    return true;
}
//---------------------------------------------------------------------------
CNifRenderViewOverdrawIndicatorCommand::
    CNifRenderViewOverdrawIndicatorCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
CNifRenderViewOverdrawIndicatorCommand::
    ~CNifRenderViewOverdrawIndicatorCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
NiStencilProperty* CNifRenderViewOverdrawIndicatorCommand::GetStencilProperty(
    NiNode* scene)
{
    // properties
    NiProperty* pkProp;
    NiPropertyList* pPropList = &scene->GetPropertyList();

    NiTListIterator kIter = pPropList->GetHeadPos();
    while(kIter)
    {
        pkProp = pPropList->GetNext(kIter);
        if(NiIsKindOf(NiStencilProperty, pkProp))
            return (NiStencilProperty*)pkProp;
    }
    return NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewOverdrawIndicatorCommand::SetOverdraw(bool bEnable)
{
    if(!m_pkView)
        return;

    CNifDoc* pkDoc = m_pkView->GetDocument();
    if(!pkDoc)
        return;

    CNifCameraList* pkCamList = pkDoc->GetCameraList();
    if(!pkCamList)
        return;

    NiMeshScreenElementsArray& kScreenElements =
        pkDoc->GetScreenElementsArray();
    if (bEnable)
    {

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

        m_pkView->GetRenderer()->SetStencilClear(1);

        NiCameraInfoIndex kIndex;
        kIndex.uiCameraIndex = 0;
        kIndex.uiRootIndex = 0;

        kScreenElements.AddFirstEmpty(ms_spBlueLevel);
        kScreenElements.AddFirstEmpty(ms_spGreenLevel);
        kScreenElements.AddFirstEmpty(ms_spYellowLevel);
        kScreenElements.AddFirstEmpty(ms_spRedLevel);
        kScreenElements.AddFirstEmpty(ms_spWarning);

        if (m_spScene)
        {
            m_spScene->AttachProperty(ms_spStencil);
            m_spScene->UpdateProperties();
        }
    }
    else
    {
        NiCameraInfoIndex kIndex;
        kIndex.uiCameraIndex = 0;
        kIndex.uiRootIndex = 0;

        kScreenElements.Remove(ms_spBlueLevel);
        kScreenElements.Remove(ms_spGreenLevel);
        kScreenElements.Remove(ms_spYellowLevel);
        kScreenElements.Remove(ms_spRedLevel);
        kScreenElements.Remove(ms_spWarning);

        if (m_spScene)
        {
            m_spScene->DetachProperty(ms_spStencil);
            m_spScene->UpdateProperties();
        }
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewOverdrawIndicatorCommand::CreateOverdrawIndicator()
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

    // This test should _always_ fail.  
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
void CNifRenderViewOverdrawIndicatorCommand::CleanUp()
{
    ms_spBlueLevel = NULL;
    ms_spGreenLevel = NULL;
    ms_spYellowLevel = NULL;
    ms_spRedLevel = NULL;
    ms_spWarning = NULL;
    ms_spStencil = NULL;
}
//---------------------------------------------------------------------------
