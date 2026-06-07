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
#include "MTerrainPlugin.h"
#include "NiFilename.h"
#include "MBrushOperationElevate.h"
#include "MBrushType.h"

#include "MBrushOperationFactory.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

MBrushOperationElevate::MBrushOperationElevate() : MBrushOperation()
{
    m_fStrength = 10.0f;
    m_fPercent = 0.15f;
}
//---------------------------------------------------------------------------
MBrushOperationElevate::MBrushOperationElevate(
    MBrushOperationElevate* pkCopy) :
    MBrushOperation(pkCopy)
{
    m_fElevationDirection = pkCopy->m_fElevationDirection;
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationElevate::CloneOperation()
{
    MBrushOperation* pmToReturn = new MBrushOperationElevate(this);
    return pmToReturn;
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::StartOperation(
    NiPoint3& kOrigin, 
    NiBrushPassPointInfo* pkActivePoints, 
    IInteractionMode::MouseButtonType eType)
{
    NI_UNUSED_ARG(kOrigin);
    NI_UNUSED_ARG(pkActivePoints);
    switch(eType)
    {
        
        case IInteractionMode::MouseButtonType::LeftButton:
            // raise height on left mouse button
            //m_pListOfOrigins = NiNew NiPoint3[m_maxOrigins];
            m_fElevationDirection = 1;
            break;
        case IInteractionMode::MouseButtonType::RightButton:
            // lower height on right mouse button
           // m_pListOfOrigins = NiNew NiPoint3[m_maxOrigins];
            m_fElevationDirection = -1;
            break;
    }
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::ApplyOperation
    (NiBrushPassPointInfo* pkActivePoints, NiPoint3* pkOrigin, float fRadius,
    float fMinHeight, float fMaxHeight)
{
    NI_UNUSED_ARG(fRadius);
    NI_UNUSED_ARG(pkOrigin);
    float fFactor = m_fElevationDirection * 
        ((m_fStrength * 0.01f * (fMaxHeight - fMinHeight)) * m_fPercent);

    //Apply the standard elevation algorithm to all inner points:
    NiUInt32 uiNumPoints = pkActivePoints->m_kInnerPoints.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumPoints; ui++)  
    {
        NiDeformablePoint* pkPickWTV = 
            pkActivePoints->m_kInnerPoints.GetAt(ui);
       
       if (pkPickWTV->IsActive())
       {
           m_pmElement->IncreaseBy(pkPickWTV, fFactor,fMinHeight, fMaxHeight);
       }
    }
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::ApplyFalloff(NiBrushPassPointInfo* pkPoints,
    float fMinHeight, float fMaxHeight)
{
    float fFactor = m_fElevationDirection * 
        ((m_fStrength * 0.01f * (fMaxHeight - fMinHeight)) * m_fPercent);

    NiUInt32 uiNumPoints = pkPoints->m_kOuterPoints.GetSize();
    for (unsigned int ui = 0; ui < uiNumPoints; ++ui)
    {
        const NiBrushPassPointInfo::FalloffData& kFalloffData = 
            pkPoints->m_kOuterPoints.GetAt(ui);
        
        NiDeformablePoint* pkPoint = kFalloffData.m_pkActual;
        if (pkPoint->IsActive())
        {
            m_pmElement->IncreaseBy(pkPoint,
                fFactor * kFalloffData.m_fInterpolatedModifier, 
                fMinHeight, fMaxHeight);
        }
    }
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::StopOperation()
{
    m_fElevationDirection = 0.0f;
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList
    )
{
    // Strength
    System::Windows::Forms::Label* pmLabel1 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown = 
        new System::Windows::Forms::NumericUpDown();    
    System::Windows::Forms::TrackBar* pmTrackBar = 
        new System::Windows::Forms::TrackBar();
    // Percentage
    System::Windows::Forms::Label* pmLabel2 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::TrackBar* pmTrackBar2 = 
        new System::Windows::Forms::TrackBar();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown2 = 
        new System::Windows::Forms::NumericUpDown();
    
    m_bInList = bInList;

    pmLabel1->Text = S"Strength:";
    pmLabel1->Size = Size(55,0);
    pmLabel1->AutoSize = true;
    pmLabel1->Location = Point(m_iNextPropertyX, m_iNextPropertyY);

    m_iNextPropertyX = pmLabel1->Width + 5;

    pmNumericUpDown->Width = 65;
    pmNumericUpDown->Maximum = 999999;
    pmNumericUpDown->Minimum = -999999;
    pmNumericUpDown->DecimalPlaces = 2;
    pmNumericUpDown->Value = System::Decimal(m_fStrength);
    pmNumericUpDown->Name = S"Strength";
    pmNumericUpDown->Location = Point(m_iNextPropertyX + pmLabel1->Width, 
        m_iNextPropertyY);
    pmNumericUpDown->ValueChanged += new System::EventHandler
        (this, &MBrushOperationElevate::NumericUpDown_PropertyValueChanged);
    
    m_iNextPropertyY += pmNumericUpDown->Height + 5;
    m_iNextPropertyX = 0;  

    pmTrackBar->AutoSize = false;
    pmTrackBar->Width = 150;
    pmTrackBar->Height = 20;
    pmTrackBar->Maximum = 100;
    pmTrackBar->Minimum = -100;
    pmTrackBar->Value = (int)m_fStrength;
    pmTrackBar->Location = Point(m_iNextPropertyX, m_iNextPropertyY);
    pmTrackBar->BackColor = SystemColors::ControlLightLight;
    pmTrackBar->ForeColor = pmPanel->ForeColor;
    pmTrackBar->Name = S"StrengthSlider";
    pmTrackBar->TickStyle = TickStyle::None;
    pmTrackBar->ValueChanged += new System::EventHandler
        (this, &MBrushOperationElevate::TrackBar_ValueChanged);

    m_iNextPropertyY += pmTrackBar->Height + 5;
    m_iNextPropertyX = 0;  

    pmLabel2->Text = S"Percentage:";
    pmLabel2->Size = Size(55,0);
    pmLabel2->AutoSize = true;
    pmLabel2->Location = Point(m_iNextPropertyX, m_iNextPropertyY);

    m_iNextPropertyX = pmLabel2->Width + 5;

    pmNumericUpDown2->Width = 65;
    pmNumericUpDown2->Maximum = 100;
    pmNumericUpDown2->Minimum = 1;
    pmNumericUpDown2->DecimalPlaces = 0;
    pmNumericUpDown2->Value = System::Decimal(m_fPercent * 100);
    pmNumericUpDown2->Name = S"Percentage";
    pmNumericUpDown2->Location = Point(m_iNextPropertyX + pmLabel1->Width, 
        m_iNextPropertyY);
    pmNumericUpDown2->ValueChanged += new System::EventHandler
        (this, &MBrushOperationElevate::NumericUpDown_PropertyValueChanged);
    
    m_iNextPropertyY += pmNumericUpDown->Height + 5;
    m_iNextPropertyX = 0;  

    pmTrackBar2->AutoSize = false;
    pmTrackBar2->Width = 150;
    pmTrackBar2->Height = 20;
    pmTrackBar2->Maximum = 100;
    pmTrackBar2->Minimum = 1;
    pmTrackBar2->Value = (int)(m_fPercent * 100);
    pmTrackBar2->Location = Point(m_iNextPropertyX, m_iNextPropertyY);
    pmTrackBar2->BackColor = SystemColors::ControlLightLight;
    pmTrackBar2->ForeColor = pmPanel->ForeColor;
    pmTrackBar2->Name = S"PercentSlider";
    pmTrackBar2->TickStyle = TickStyle::None;
    pmTrackBar2->ValueChanged += new System::EventHandler
        (this, &MBrushOperationElevate::TrackBar_ValueChanged);

    m_iNextPropertyY += pmTrackBar->Height + 5;
    m_iNextPropertyX = 0;
             
    pmPanel->Controls->Add(pmLabel1);
    pmPanel->Controls->Add(pmNumericUpDown);
    pmPanel->Controls->Add(pmTrackBar);    
    pmPanel->Controls->Add(pmLabel2);
    pmPanel->Controls->Add(pmNumericUpDown2);
    pmPanel->Controls->Add(pmTrackBar2);
    
    m_pmElement->GetOperationFullPanel(pmPanel, bInList, this);
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::ReadValues(MBrushOperation* pmOperation)
{
    MBrushOperation::ReadValues(pmOperation);
    
    MBrushOperationElevate* pmOperationElevate = 
        dynamic_cast<MBrushOperationElevate*>(pmOperation);

    if (!pmOperationElevate)
        return;

    m_fElevationDirection = pmOperationElevate->m_fElevationDirection;
}
//---------------------------------------------------------------------------
void MBrushOperationElevate::GetOptionInformation(
    System::Windows::Forms::Label* pmElementDesc)
{
    pmElementDesc->Text = String::Concat(S"Strength: ", 
        System::Decimal(m_fStrength).ToString(), S" ");
    m_pmElement->GetOptionInformation(pmElementDesc);
}

//===========================================================================
// Event Handlers
//===========================================================================

System::Void MBrushOperationElevate::NumericUpDown_PropertyValueChanged
(System::Object*  sender, System::EventArgs*)
{
    NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(sender);
        
    if(String::Compare(pmControl->Name, S"Strength") == 0)
    {
        TrackBar* pmTrack = dynamic_cast<TrackBar*>
            (pmControl->Parent->Controls->get_Item(S"StrengthSlider"));

        if(pmTrack->Value != pmControl->Value)
        {
            if(abs((double)pmControl->Value) <= 100)
            {
                pmTrack->Value = (int)pmControl->Value;
            }
            else 
            {
                if(pmControl->Value < 0)
                    pmTrack->Value = -100;
                else
                    pmTrack->Value = 100;
            }

        }
        
        m_fStrength = (float)pmControl->Value;
    }
    else if(String::Compare(pmControl->Name, S"Percentage") == 0)
    {
        TrackBar* pmTrack = dynamic_cast<TrackBar*>
            (pmControl->Parent->Controls->get_Item(S"PercentSlider"));

        if(pmTrack->Value != pmControl->Value)
        {
            if(abs((double)pmControl->Value) <= 100)
            {
                pmTrack->Value = (int)pmControl->Value;
            }
           
        }
        
        m_fPercent = (float)(pmControl->Value / 100.0f);
    }
    else
    {
        return;
    }

    MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(this);
}
//---------------------------------------------------------------------------
System::Void MBrushOperationElevate::CheckBox_CheckChanged
    (System::Object*  sender, System::EventArgs*)
{
    CheckBox* pmCheckBox = dynamic_cast<CheckBox*>(sender);
    m_bUseAllVertices = pmCheckBox->Checked;
    
    MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(this);
}
//---------------------------------------------------------------------------
System::Void MBrushOperationElevate::TrackBar_ValueChanged(System::Object*  
    sender,    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    TrackBar* pmTrackBar = dynamic_cast<TrackBar*>(sender);

    if(String::Compare(pmTrackBar->Name, S"StrengthSlider") == 0)
    {

        NumericUpDown* pmNum = dynamic_cast<NumericUpDown*>
            (pmTrackBar->Parent->Controls->get_Item(S"Strength"));

        if(abs((double)pmNum->Value) <= 100) 
        {
            pmNum->Value = pmTrackBar->Value;
        }
        else if(abs((double)pmNum->Value) >= 100 && 
            abs((double)pmTrackBar->Value) < 100)
        {
            pmNum->Value = pmTrackBar->Value;
        }
    }
    else if(String::Compare(pmTrackBar->Name, S"PercentSlider") == 0)
    {
        NumericUpDown* pmNum = dynamic_cast<NumericUpDown*>
            (pmTrackBar->Parent->Controls->get_Item(S"Percentage"));

        if(abs((double)pmNum->Value) <= 100)
        {
            pmNum->Value = pmTrackBar->Value;
        }
        else if(abs((double)pmNum->Value) >= 100 && 
            abs((double)pmTrackBar->Value) < 100)
        {
            pmNum->Value = pmTrackBar->Value;
        }
    }
    else
    {
        return;
    }

    MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(this);
}
