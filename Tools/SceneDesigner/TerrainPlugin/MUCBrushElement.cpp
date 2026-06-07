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
#include "MBrush.h"
#include "MUCBrushElement.h"
#include "MBrushElementSculpt.h"
#include "MBrushElementSurface.h"
#include "MBrushOperationElevate.h"
#include "MBrushOperationFlatten.h"
#include "MBrushOperationAlign.h"
#include "NiFilename.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

void MUCBrushElement::SetTitle(char* pcTitle)
{
    this->m_pmElementName->Text = pcTitle;
}
//---------------------------------------------------------------------------
void MUCBrushElement::SetOperation(MBrushOperation* pmToAdd)
{
    if(pmToAdd)
    {
        // if an operation has already been set
        if(m_pmOperation)
        {
            m_pmPropertiesPanel->Controls->Clear();
        }

        m_pmOperation = pmToAdd;

        pmToAdd->GetOperationFullPanel(m_pmPropertiesPanel,true);
        m_pmPicImage->Image = pmToAdd->m_pmOperationIcon;

        m_pmElementName->Text = String::Concat(pmToAdd->m_pmName, " ",
            pmToAdd->m_pmElement->m_pmName, " ");

        pmToAdd->Update(true);

    }

    if(!m_pmPropertiesPanel->Visible)
    {
        m_pmPropertiesPanel->Visible = true;
        m_pmBtnDropDown->Text = "<";
        this->Height += m_pmPropertiesPanel->Height;
        m_pmUnfoldToolStripMenuItem->Text = "Fold";
    }
    

}
//---------------------------------------------------------------------------
void MUCBrushElement::SetDescription(char* pcDesc)
{
    this->m_pmElementDesc->Text = pcDesc;
}
//---------------------------------------------------------------------------
void MUCBrushElement::SetPicture(char* pcPath)
{
    System::ComponentModel::ComponentResourceManager* pmResourceManager = 
        MTerrainPlugin::GetResourceManager();

    this->m_pmPicImage->Image = 
        dynamic_cast<Image*>(pmResourceManager->GetObject(pcPath));
}
//---------------------------------------------------------------------------
void MUCBrushElement::SetPicture(Image* pmImage)
{
    m_pmPicImage->Image = pmImage;
    m_pmOperation->m_pmOperationIcon = pmImage;
}
//---------------------------------------------------------------------------
void MUCBrushElement::ActivateOperation()
{
    System::Collections::Generic::List<MBrushOperation*>* pmTemp = 
        MTerrainPlugin::GetInstance()->GetBrush()->GetOperations();

    int iIndex = pmTemp->Count;
    // If an operation has already been set
    if (pmTemp->Contains(m_pmOperation))
    {
        iIndex = pmTemp->IndexOf(m_pmOperation);
        pmTemp->Remove(m_pmOperation); 
    }

    pmTemp->Insert(iIndex, m_pmOperation);
}
//---------------------------------------------------------------------------
void MUCBrushElement::RemovingTool()
{
    MTerrainPlugin::GetInstance()->GetBrush()->GetOperations()->Remove
        (m_pmOperation);
    
}
//---------------------------------------------------------------------------
MBrushOperation* MUCBrushElement::GetOperation()
{
    return m_pmOperation;
}
//---------------------------------------------------------------------------
void MUCBrushElement::Update()
{
    if(m_pmOperation)
    {
        m_pmOperation->Update(true);
        m_pmOperation->GetOptionInformation(m_pmElementDesc);
    }

}
//---------------------------------------------------------------------------
System::Void MUCBrushElement::RemoveToolStripMenuItem_Click(System::Object*  
    sender, System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    RemovingTool();
    m_pmOperation = 0;
    this->Dispose(true);
}
//---------------------------------------------------------------------------
System::Void MUCBrushElement::BtnDropDown_Click(System::Object*  sender, 
    System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
     if(!m_pmPropertiesPanel->Visible)
     {
         m_pmPropertiesPanel->Visible = true;
         m_pmBtnDropDown->Text = "<";
         this->Height += m_pmPropertiesPanel->Height;
         m_pmUnfoldToolStripMenuItem->Text = "Fold";
     }
     else
     {
         m_pmPropertiesPanel->Visible = false;
         m_pmBtnDropDown->Text = ">";
         this->Height -= m_pmPropertiesPanel->Height;
         m_pmUnfoldToolStripMenuItem->Text = "Unfold";
     }

     dynamic_cast<MUCBrushElementPanel*>
         (this->Parent->Parent)->RedrawControl();
     
 }
//---------------------------------------------------------------------------
System::Void MUCBrushElement::MUCBrushElement_MouseClick(System::Object*  
    sender, System::Windows::Forms::MouseEventArgs*  e) 
{
    NI_UNUSED_ARG(sender);
    if(e->Button == MouseButtons::Left)
    {         

        if(!m_pmPropertiesPanel->Visible)
         {
             m_pmPropertiesPanel->Visible = true;
             m_pmBtnDropDown->Text = "<";
             this->Height += m_pmPropertiesPanel->Height;
             m_pmUnfoldToolStripMenuItem->Text = "Fold";
         }
         else
         {
             m_pmPropertiesPanel->Visible = false;
             m_pmBtnDropDown->Text = ">";
             this->Height -= m_pmPropertiesPanel->Height;
             m_pmUnfoldToolStripMenuItem->Text = "Unfold";
         }

         dynamic_cast<MUCBrushElementPanel*>
             (this->Parent->Parent)->RedrawControl();

     }
    else if(e->Button == MouseButtons::Right)
    {
        m_pmContextMenu->Show(this, Point(e->X,e->Y));
    }
}
//---------------------------------------------------------------------------
System::Void MUCBrushElement::UnfoldToolStripMenuItem_Click(System::Object*  
    sender, System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    if(!m_pmPropertiesPanel->Visible)
     {
         m_pmPropertiesPanel->Visible = true;
         m_pmBtnDropDown->Text = "<";
         this->Height += m_pmPropertiesPanel->Height;
         m_pmUnfoldToolStripMenuItem->Text = "Fold";
     }
     else
     {
         m_pmPropertiesPanel->Visible = false;
         m_pmBtnDropDown->Text = ">";
         this->Height -= m_pmPropertiesPanel->Height;
         m_pmUnfoldToolStripMenuItem->Text = "Unfold";
     }
}
//---------------------------------------------------------------------------
