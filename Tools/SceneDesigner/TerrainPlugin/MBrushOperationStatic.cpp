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
#include "MBrushOperationStatic.h"
#include "MBrushOperationFactory.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

//---------------------------------------------------------------------------
MBrushOperationStatic::MBrushOperationStatic() : MBrushOperation()
{
    m_fStrength = 500;
    m_fPercent = 1;
}
//---------------------------------------------------------------------------
MBrushOperationStatic::MBrushOperationStatic(MBrushOperationStatic* pmCopy):
    MBrushOperation(pmCopy)
{
    
}
//---------------------------------------------------------------------------
MBrushOperation* MBrushOperationStatic::CloneOperation()
{
    MBrushOperation* pmToReturn = new MBrushOperationStatic(this);
    return pmToReturn;
}
//---------------------------------------------------------------------------
void MBrushOperationStatic::StartOperation(NiPoint3& kOrigin,
    NiBrushPassPointInfo* pkActivePoints, 
    IInteractionMode::MouseButtonType eType)
{
    NI_UNUSED_ARG(kOrigin);
    NI_UNUSED_ARG(eType);
    NI_UNUSED_ARG(pkActivePoints);
    srand(GetTickCount());
}
//---------------------------------------------------------------------------
void MBrushOperationStatic::ApplyOperation(
    NiBrushPassPointInfo* pkActivePoints, NiPoint3* pkOrigin, float fRadius,
    float fMinHeight, float fMaxHeight)
{
    NI_UNUSED_ARG(fRadius);
    NI_UNUSED_ARG(pkOrigin);
    NiDeformablePoint* pkPickWTV;
    float fMax = 0;
    float fMin = 0;

    m_pmElement->GetMinAndMax(fMin, fMax);
    //Apply the standard elevation algorithm to all inner points:
    unsigned int uiSize = pkActivePoints->m_kInnerPoints.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ++ui)  
    {
        pkPickWTV = pkActivePoints->m_kInnerPoints.GetAt(ui);
        if (!pkPickWTV->IsActive())
            continue;

        int iSign =(4599845* abs(rand())) % 100;

        if(iSign >= 50)
            iSign = -1;
        else
            iSign = 1;
        
        float fValue = iSign * (((float)
            ((4599845 * abs(rand()) % (NiInt32
            (m_fStrength * m_fPercent * 100))))/100.0f));

        if(fMax != 0)
        {
            fValue = fValue * 255.0f /(10.0f * 100.0f);
            fValue += pkPickWTV->GetValue();
        }
        else
        {
            fValue = fValue / (10.0f * 100.0f);
            fValue += pkPickWTV->GetValue();
        }

        m_pmElement->SetValue(pkPickWTV, NiClamp(fValue, fMinHeight,
            fMaxHeight));
    }
}
//---------------------------------------------------------------------------
void MBrushOperationStatic::ApplyFalloff(NiBrushPassPointInfo* pkPoints,
    float fMinHeight, float fMaxHeight)
{
    unsigned int uiNumPoints = pkPoints->m_kOuterPoints.GetSize();
    NiBrushPassPointInfo::FalloffData kFalloffData;
    float fValue;
    int iIncrement;
    float fMax = 0;
    float fMin = 0;
           
    NiDeformablePoint* pkPoint;
    m_pmElement->GetMinAndMax(fMin, fMax);

    for (unsigned int ui = 0; ui < uiNumPoints; ++ui)
    {
        kFalloffData = pkPoints->m_kOuterPoints.GetAt(ui);
        
        pkPoint = kFalloffData.m_pkActual;
        if (!pkPoint->IsActive())
            continue;
        
        int iSign =(4599845* abs(rand())) % 100;

        if(iSign >= 50)
            iSign = -1;
        else
            iSign = 1;
        
        iIncrement = (NiInt32)(m_fStrength * m_fPercent * 
            kFalloffData.m_fInterpolatedModifier * 100);
        
        if(iIncrement != 0)
        {
            fValue = iSign * (((float)
                ((4599845 * abs(rand()) % (iIncrement)))/100.0f));

            if(fMax != 0)
            {
                fValue = fValue * 255.0f / (10.0f * 100.0f);
                fValue += pkPoint->GetValue();
            }
            else
            {
                fValue = fValue / (10.0f * 100.0f);
                fValue += pkPoint->GetValue();
            }

            m_pmElement->SetValue(pkPoint, NiClamp(fValue, fMinHeight,
                fMaxHeight));
        }
        
    }
}
//---------------------------------------------------------------------------
void MBrushOperationStatic::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList
    )
{
    System::Windows::Forms::Label* pmLabel1 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown = 
        new System::Windows::Forms::NumericUpDown();    
    System::Windows::Forms::TrackBar* pmTrackBar = 
        new System::Windows::Forms::TrackBar();

    System::Windows::Forms::Label* pmLabel2 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown2 = 
        new System::Windows::Forms::NumericUpDown();    
    System::Windows::Forms::TrackBar* pmTrackBar2 = 
        new System::Windows::Forms::TrackBar();
    
    
    m_bInList = bInList;

    pmLabel1->Text = S"Strength:";
    pmLabel1->Size = Size(55,0);
    pmLabel1->AutoSize = true;
    pmLabel1->Location = Point(m_iNextPropertyX, m_iNextPropertyY);

    m_iNextPropertyX = pmLabel1->Width + 5;

    pmNumericUpDown->Width = 65;
    pmNumericUpDown->Maximum = 999999;
    pmNumericUpDown->Minimum = 1;
    pmNumericUpDown->DecimalPlaces = 1;
    pmNumericUpDown->Value = System::Decimal(m_fStrength);
    pmNumericUpDown->Name = S"Strength";
    pmNumericUpDown->Location = Point(m_iNextPropertyX + pmLabel1->Width, 
        m_iNextPropertyY);
    pmNumericUpDown->ValueChanged += new System::EventHandler
        (this, &MBrushOperationStatic::NumericUpDown_PropertyValueChanged);
    
    m_iNextPropertyY += pmNumericUpDown->Height + 5;
    m_iNextPropertyX = 0;

    pmTrackBar->AutoSize = false;
    pmTrackBar->Width = 150;
    pmTrackBar->Height = 20;
    pmTrackBar->Maximum = 1000;
    pmTrackBar->Minimum = 1;
    pmTrackBar->Value = (int)m_fStrength;
    pmTrackBar->Location = Point(m_iNextPropertyX, m_iNextPropertyY);
    pmTrackBar->BackColor = SystemColors::ControlLightLight;
    pmTrackBar->ForeColor = pmPanel->ForeColor;
    pmTrackBar->Name = S"StrengthSlider";
    pmTrackBar->TickStyle = TickStyle::None;
    pmTrackBar->ValueChanged += new System::EventHandler
        (this, &MBrushOperationStatic::TrackBar_ValueChanged);

    m_iNextPropertyY += pmTrackBar->Height + 5;
    m_iNextPropertyX = 0;

    pmLabel2->Text = S"Percentage:";
    pmLabel2->Size = Size(55,0);
    pmLabel2->AutoSize = true;
    pmLabel2->Location = Point(m_iNextPropertyX, m_iNextPropertyY);

   // m_NextPropertyY += pkLabel2->Height + 5;
    m_iNextPropertyX = pmLabel2->Width + 5;

    pmNumericUpDown2->Width = 65;
    pmNumericUpDown2->Maximum = 100;
    pmNumericUpDown2->Minimum = 1;
    pmNumericUpDown2->DecimalPlaces = 0;
    pmNumericUpDown2->Value = System::Decimal(m_fPercent * 100.0f);
    pmNumericUpDown2->Name = S"Percentage";
    pmNumericUpDown2->Location = Point(m_iNextPropertyX + pmLabel1->Width, 
        m_iNextPropertyY);
    pmNumericUpDown2->ValueChanged += new System::EventHandler
        (this, &MBrushOperationStatic::NumericUpDown_PropertyValueChanged);
    
    m_iNextPropertyY += pmNumericUpDown->Height + 5;
    m_iNextPropertyX = 0;  

    pmTrackBar2->AutoSize = false;
    pmTrackBar2->Width = 150;
    pmTrackBar2->Height = 20;
    pmTrackBar2->Maximum = 100;
    pmTrackBar2->Minimum = 1;
    pmTrackBar2->Value = (int)(m_fPercent * 100.0f);
    pmTrackBar2->Location = Point(m_iNextPropertyX, m_iNextPropertyY);
    pmTrackBar2->BackColor = SystemColors::ControlLightLight;
    pmTrackBar2->ForeColor = pmPanel->ForeColor;
    pmTrackBar2->Name = S"PercentSlider";
    pmTrackBar2->TickStyle = TickStyle::None;
    pmTrackBar2->ValueChanged += new System::EventHandler
        (this, &MBrushOperationStatic::TrackBar_ValueChanged);

    m_iNextPropertyY += pmTrackBar->Height + 5;
    m_iNextPropertyX = 0;
        
    pmPanel->Controls->Add(pmLabel1);
    pmPanel->Controls->Add(pmNumericUpDown);
    pmPanel->Controls->Add(pmTrackBar);
    pmPanel->Controls->Add(pmLabel2);
    pmPanel->Controls->Add(pmNumericUpDown2);
    pmPanel->Controls->Add(pmTrackBar2);

    m_pmElement->GetOperationFullPanel(pmPanel,bInList, this);
}
//---------------------------------------------------------------------------
void MBrushOperationStatic::GetOptionInformation(
    System::Windows::Forms::Label* pmElementDesc)
{
    pmElementDesc->Text = String::Concat(S"Strength: ",
        (System::Decimal(m_fStrength)).ToString(), S" ");
    m_pmElement->GetOptionInformation(pmElementDesc);
}

//===========================================================================
// Event Handlers
//===========================================================================

System::Void MBrushOperationStatic::NumericUpDown_PropertyValueChanged
(System::Object*  sender, System::EventArgs*)
{
    NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(sender);
        
    if(String::Compare(pmControl->Name, S"Strength") == 0)
    {
        TrackBar* pmTrack = dynamic_cast<TrackBar*>
            (pmControl->Parent->Controls->get_Item(S"StrengthSlider"));

        if(pmTrack->Value != pmControl->Value)
        {
            if(abs((double)pmControl->Value) <= 1000)
            {
                pmTrack->Value = (int)pmControl->Value;
            }
            else 
            {
                if(pmControl->Value <= 1)
                    pmTrack->Value = 1;
                else
                    pmTrack->Value = 1000;
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
System::Void MBrushOperationStatic::TrackBar_ValueChanged(System::Object*  
    sender,    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);

    TrackBar* pmTrackBar = dynamic_cast<TrackBar*>(sender);

    if(String::Compare(pmTrackBar->Name, S"StrengthSlider") == 0)
    {

        NumericUpDown* pmNum = dynamic_cast<NumericUpDown*>
            (pmTrackBar->Parent->Controls->get_Item(S"Strength"));

        if(abs((double)pmNum->Value) <= 1000) 
        {
            pmNum->Value = pmTrackBar->Value;
        }
        else if(abs((double)pmNum->Value) >= 1000 && 
            abs((double)pmTrackBar->Value) < 1000)
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
