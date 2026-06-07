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

#include "MBrushElement.h"

#include "MBrushOperationFactory.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

void MBrushElement::ReadValues(MBrushElement* pmElement)
{
    m_fMinimumValue = pmElement->m_fMinimumValue;
    m_fMaximumValue = pmElement->m_fMaximumValue;
    m_fMinHeight = pmElement->m_fMinHeight;
    m_fMaxHeight = pmElement->m_fMaxHeight;
    m_fMinSlope = pmElement->m_fMinSlope;
    m_fMaxSlope = pmElement->m_fMaxSlope;
    m_bUseHeight = pmElement->m_bUseHeight;
    m_bUseSlope = pmElement->m_bUseSlope;
}
//---------------------------------------------------------------------------
bool MBrushElement::CheckProceduralFilters(const NiDeformablePoint* pkPoint)
{
    float fSlope = (180 * pkPoint->GetSlopeAngle()) / NI_PI;
    float fHeight = pkPoint->GetWorldLocation().z;
    
    if (((fHeight >= m_fMinHeight && 
        fHeight <= m_fMaxHeight) || 
        !m_bUseHeight) &&
        ((fSlope >= m_fMinSlope &&
        fSlope <= m_fMaxSlope) || 
        !m_bUseSlope))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MBrushElement::GetOperationFullPanel(
    System::Windows::Forms::Panel* pmPanel,
    bool bInList,
    MBrushOperation* pmOperation)
{
    NI_UNUSED_ARG(pmOperation);
    NI_UNUSED_ARG(bInList);

    // Create a table layout panel so the layout is compatible with all 
    // windows themes
    System::Windows::Forms::TableLayoutPanel* pmTablePanel = 
        new System::Windows::Forms::TableLayoutPanel();

    System::Windows::Forms::Label* pmLabel4 = 
        new System::Windows::Forms::Label();

    System::Windows::Forms::CheckBox* pmCheckBox1 = 
        new System::Windows::Forms::CheckBox();
    System::Windows::Forms::Label* pmLabel5 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown1 = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown2 = 
        new System::Windows::Forms::NumericUpDown();

    System::Windows::Forms::CheckBox* pmCheckBox2 = 
        new System::Windows::Forms::CheckBox();
    System::Windows::Forms::Label* pmLabel7 = 
        new System::Windows::Forms::Label();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown3 = 
        new System::Windows::Forms::NumericUpDown();
    System::Windows::Forms::NumericUpDown* pmNumericUpDown4 = 
        new System::Windows::Forms::NumericUpDown();

    int iNextPropertyY = 0;
    if (pmPanel->Controls->Count)
    {
           iNextPropertyY = pmPanel->Controls->get_Item(
                pmPanel->Controls->Count - 1)->Location.Y + 
            pmPanel->Controls->get_Item(
                pmPanel->Controls->Count - 1)->Height + 5;
    }
    int iNextPropertyX = 0;

    pmTablePanel->ColumnCount = 4;
    pmTablePanel->RowCount = 3;
    pmTablePanel->AutoSize = true;
    pmTablePanel->AutoSizeMode = 
        System::Windows::Forms::AutoSizeMode::GrowAndShrink;
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->ColumnStyles->Add(new System::Windows::Forms::ColumnStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->RowStyles->Add(new System::Windows::Forms::RowStyle());
    pmTablePanel->Name = L"ProceduralFiltersTablePanel";
    pmTablePanel->Location = 
        System::Drawing::Point(iNextPropertyX, iNextPropertyY);
    pmTablePanel->Controls->Add(pmLabel4,0,0);
    pmTablePanel->Controls->Add(pmCheckBox1,0,1);
    pmTablePanel->Controls->Add(pmNumericUpDown1,1,1);
    pmTablePanel->Controls->Add(pmLabel5,2,1);
    pmTablePanel->Controls->Add(pmNumericUpDown2,3,1);
    pmTablePanel->Controls->Add(pmCheckBox2,0,2);
    pmTablePanel->Controls->Add(pmNumericUpDown3,1,2);
    pmTablePanel->Controls->Add(pmLabel7,2,2);
    pmTablePanel->Controls->Add(pmNumericUpDown4,3,2);
    pmTablePanel->SetColumnSpan(pmLabel4,4);

    pmLabel4->Text = S"Procedural filters:";
    pmLabel4->Size = Size(80,18);
    pmLabel4->AutoSize = true;
    pmLabel4->Anchor = System::Windows::Forms::AnchorStyles::Left;
    
    pmCheckBox1->Text = S"Height Range:";
    pmCheckBox1->Name = S"UseHeight";
    pmCheckBox1->Size = Size(100,18);
    pmCheckBox1->AutoSize = true;        
    pmCheckBox1->Checked = m_bUseHeight;
    pmCheckBox1->Anchor = System::Windows::Forms::AnchorStyles::Left;
    pmCheckBox1->CheckedChanged += new System::EventHandler
        (this, &MBrushElement::CheckBox_PropertyValueChanged);

    System::Windows::Forms::AnchorStyles kNumericAnchorStyle = 
        static_cast<System::Windows::Forms::AnchorStyles>(
            System::Windows::Forms::AnchorStyles::Left | 
            System::Windows::Forms::AnchorStyles::Right);

    pmNumericUpDown1->AutoSize = true;
    pmNumericUpDown1->Width = 65;
    pmNumericUpDown1->Maximum = 999999;
    pmNumericUpDown1->Minimum = -999999;
    pmNumericUpDown1->DecimalPlaces = 0;
    pmNumericUpDown1->Value = System::Decimal(m_fMinHeight);
    pmNumericUpDown1->Name = S"MinHeight";
    pmNumericUpDown1->Enabled = m_bUseHeight;
    pmNumericUpDown1->Anchor = kNumericAnchorStyle;
    pmNumericUpDown1->ValueChanged += new System::EventHandler
        (this, &MBrushElement::NumericUpDown_PropertyValueChanged);
    
    iNextPropertyX += pmNumericUpDown1->Width;

    pmLabel5->Text = S"to:";
    pmLabel5->Size = Size(20,18);
    pmLabel5->AutoSize = true;   
    pmLabel5->Name = S"ToHeight";
    pmLabel5->Enabled = m_bUseHeight;
    pmLabel5->Anchor = System::Windows::Forms::AnchorStyles::Left;
    
    pmNumericUpDown2->AutoSize = true;
    pmNumericUpDown2->Width = 65;
    pmNumericUpDown2->Maximum = 999999;
    pmNumericUpDown2->Minimum = -999999;
    pmNumericUpDown2->DecimalPlaces = 0;
    pmNumericUpDown2->Value = System::Decimal(m_fMaxHeight);
    pmNumericUpDown2->Name = S"MaxHeight";
    pmNumericUpDown2->Enabled = m_bUseHeight;
    pmNumericUpDown2->Anchor = kNumericAnchorStyle;
    pmNumericUpDown2->ValueChanged += new System::EventHandler
        (this, &MBrushElement::NumericUpDown_PropertyValueChanged);
    
    pmCheckBox2->Text = S"Slope Range:";
    pmCheckBox2->Name = S"UseSlope";
    pmCheckBox2->Size = Size(100,18);
    pmCheckBox2->AutoSize = true;        
    pmCheckBox2->Checked = m_bUseSlope;
    pmCheckBox2->Anchor = System::Windows::Forms::AnchorStyles::Left;
    pmCheckBox2->CheckedChanged += new System::EventHandler
        (this, &MBrushElement::CheckBox_PropertyValueChanged);
    
    pmNumericUpDown3->AutoSize = true;
    pmNumericUpDown3->Width = 65;
    pmNumericUpDown3->Maximum = 90;
    pmNumericUpDown3->Minimum = 0;
    pmNumericUpDown3->DecimalPlaces = 0;
    pmNumericUpDown3->Value = System::Decimal(m_fMinSlope);
    pmNumericUpDown3->Name = S"MinSlope";
    pmNumericUpDown3->Enabled = m_bUseSlope;
    pmNumericUpDown3->Anchor = kNumericAnchorStyle;
    pmNumericUpDown3->ValueChanged += new System::EventHandler
        (this, &MBrushElement::NumericUpDown_PropertyValueChanged);
    
    pmLabel7->Text = S"to:";
    pmLabel7->Size = Size(20,18);
    pmLabel7->AutoSize = true;  
    pmLabel7->Name = S"ToSlope";
    pmLabel7->Enabled = m_bUseSlope;
    pmLabel7->Anchor = System::Windows::Forms::AnchorStyles::Left;
    
    pmNumericUpDown4->AutoSize = true;
    pmNumericUpDown4->Width = 65;
    pmNumericUpDown4->Maximum = 90;
    pmNumericUpDown4->Minimum = 0;
    pmNumericUpDown4->DecimalPlaces = 0;
    pmNumericUpDown4->Value = System::Decimal(m_fMaxSlope);
    pmNumericUpDown4->Name = S"MaxSlope";
    pmNumericUpDown4->Enabled = m_bUseSlope;
    pmNumericUpDown4->Anchor = kNumericAnchorStyle;
    pmNumericUpDown4->ValueChanged += new System::EventHandler
        (this, &MBrushElement::NumericUpDown_PropertyValueChanged);

    pmPanel->Controls->Add(pmTablePanel);
}
//---------------------------------------------------------------------------
System::Void MBrushElement::NumericUpDown_PropertyValueChanged(
    System::Object*  sender, 
    System::EventArgs*)
{
    NumericUpDown* pmControl = dynamic_cast<NumericUpDown*>(sender);
        
    if(String::Compare(pmControl->Name, S"MinHeight") == 0)
    {
        m_fMinHeight = (float)pmControl->Value;
    }
    else if(String::Compare(pmControl->Name, S"MaxHeight") == 0)
    {
        m_fMaxHeight = (float)pmControl->Value;
    }
    else if(String::Compare(pmControl->Name, S"MinSlope") == 0)
    {
        m_fMinSlope = (float)pmControl->Value;
    }
    else if(String::Compare(pmControl->Name, S"MaxSlope") == 0)
    {
        m_fMaxSlope = (float)pmControl->Value;
    }
    else
    {
        return;
    }

    if (m_pmAssociatedOperation)
    {
        MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(
            m_pmAssociatedOperation);
    }
}
//---------------------------------------------------------------------------
System::Void MBrushElement::CheckBox_PropertyValueChanged(
    System::Object*  sender, 
    System::EventArgs*  e)
{
    NI_UNUSED_ARG(e);
    CheckBox* pmControl = dynamic_cast<CheckBox*>(sender);

    if(String::Compare(pmControl->Name, S"UseSlope") == 0)
    {
        m_bUseSlope = pmControl->Checked;

        for (NiInt32 i = 0; i < pmControl->Parent->Controls->Count; i++)
        {
            Control* pmChild = dynamic_cast<Control*>
                (pmControl->Parent->Controls->get_Item(i));
            
            if (String::Compare(pmChild->Name, S"MinSlope") == 0 ||
                String::Compare(pmChild->Name, S"MaxSlope") == 0 ||
                String::Compare(pmChild->Name, S"ToSlope") == 0)
            {
                pmChild->Enabled = m_bUseSlope;
            }            
        }
    }
    else if(String::Compare(pmControl->Name, S"UseHeight") == 0)
    {
        m_bUseHeight = pmControl->Checked;

        for (NiInt32 i = 0; i < pmControl->Parent->Controls->Count; i++)
        {
            Control* pmChild = dynamic_cast<Control*>
                (pmControl->Parent->Controls->get_Item(i));
            
            if (String::Compare(pmChild->Name, S"MinHeight") == 0 ||
                String::Compare(pmChild->Name, S"MaxHeight") == 0 ||
                String::Compare(pmChild->Name, S"ToHeight") == 0)
            {
                pmChild->Enabled = m_bUseHeight;
            }            
        }
    }
    else
    {
        return;
    }

    if (m_pmAssociatedOperation)
    {
        MBrushOperationFactory::GetInstance()->UpdateTemplateFrom(
            m_pmAssociatedOperation);
    }
}
