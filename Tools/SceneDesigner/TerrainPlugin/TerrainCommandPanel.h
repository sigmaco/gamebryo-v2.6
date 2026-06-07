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

#pragma once

#include "MBrush.h"
#include "MUCBrushElement.h"
#include "MUCBrushElementPanel.h"

// This file has been auto generated

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    public __gc class TerrainCommandPanel : public System::Windows::Forms::Form
    {
        
    public: 
        TerrainCommandPanel(void)
        {
            m_pkPalette = 0;
            m_SelectedElement = 0;
            m_SelectedOperation = 0;
            InitializeComponent();
            BuildUtilityInterface();
            BuildElementBrowserInterface();            
        }
        
    private: 
        //SurfaceCommandPanel* m_pkSurfacePanel;
        NiSurfacePalette* m_pkPalette;
        bool m_bBrushActive;
        String* m_SelectedElement;
        String* m_SelectedOperation;
        
        unsigned int m_uiXOperationPos;
        unsigned int m_uiYOperationPos;

        System::Drawing::Rectangle m_kDragingBox;

        bool m_bDragging;
        System::Windows::Forms::Panel* m_pkFlowPanel;

    #pragma region Brush options widgets
        /// Brushtype group 
        System::Windows::Forms::GroupBox*  gbBrushShape;
        System::Windows::Forms::Panel* m_pkBrushPanel;
        System::Windows::Forms::Label* m_pmShapeLabel;
        System::Windows::Forms::ComboBox*  BrushType;
        System::Windows::Forms::Panel* m_pkBrushOptionPanel;        
        
#pragma endregion

    #pragma region terrain specific widgets

        /// Terrain widget (entity selected + debug buttons) 
        System::Windows::Forms::ToolStrip* m_pkDebugButtonPanel;
        System::Windows::Forms::ToolStripButton*  btImport;
        System::Windows::Forms::ToolStripButton*  btExport;
        System::Windows::Forms::ToolStripButton*  btSavePhysXData;
        System::Windows::Forms::ToolStripButton*  cbUpdateLighting;

#pragma endregion

    #pragma region brush display

        /// New Brush display
        System::Windows::Forms::GroupBox*  groupBox1;
        MUCBrushElementPanel* kBrushListElements;

#pragma endregion

    #pragma region New element selection panel

        /// New Element base for dynamically loaded controls
        System::Windows::Forms::GroupBox* m_pkElementBrowserGroup;
        System::Windows::Forms::TabControl* m_pkElementBrowserTab;

#pragma endregion

    #pragma region Generated initialisation code

    private: System::Void InitializeComponent() 
    {
        System::ComponentModel::ComponentResourceManager*  resources = (new System::ComponentModel::ComponentResourceManager(__typeof(TerrainCommandPanel)));
        this->m_pkFlowPanel = new Panel();
        this->gbBrushShape = (new System::Windows::Forms::GroupBox());
        this->m_pkBrushPanel = new Panel();
        this->m_pmShapeLabel = new System::Windows::Forms::Label;
        this->BrushType = (new System::Windows::Forms::ComboBox());
        this->m_pkBrushOptionPanel = new Panel();          

        this->m_pkDebugButtonPanel = 
            (new System::Windows::Forms::ToolStrip());
        this->btImport = (new System::Windows::Forms::ToolStripButton());
        this->btExport = (new System::Windows::Forms::ToolStripButton());
        this->btSavePhysXData = 
            (new System::Windows::Forms::ToolStripButton());
        this->cbUpdateLighting = 
            (new System::Windows::Forms::ToolStripButton());
        this->groupBox1 = (new System::Windows::Forms::GroupBox());
        this->kBrushListElements = (new MUCBrushElementPanel());
        this->gbBrushShape->SuspendLayout();
            
        this->groupBox1->SuspendLayout();
        //Dynamically loaded element browser
        this->m_pkElementBrowserGroup = new System::Windows::Forms::GroupBox();
        this->m_pkElementBrowserTab = new System::Windows::Forms::TabControl();

        this->SuspendLayout();

        //
        // m_pkFlowPanel
        //    
        this->m_pkFlowPanel->Location = System::Drawing::Point(0,0);
        this->m_pkFlowPanel->Size = System::Drawing::Size(150,700);
        this->m_pkFlowPanel->Dock = DockStyle::Fill;
        this->m_pkFlowPanel->Name = S"MainPanel";
        this->m_pkFlowPanel->AutoScroll = true;
        this->m_pkFlowPanel->Controls->Add(this->m_pkDebugButtonPanel);
        this->m_pkFlowPanel->Controls->Add(gbBrushShape);
        this->m_pkFlowPanel->Controls->Add(groupBox1);
        this->m_pkFlowPanel->Controls->Add(m_pkElementBrowserGroup);
        this->m_pkFlowPanel->SizeChanged += new EventHandler(this,
            &TerrainCommandPanel::ControlSizeChanged);
        //
        // m_pkDebugButtonPanel
        //
        // 
        this->m_pkDebugButtonPanel->GripStyle = 
            System::Windows::Forms::ToolStripGripStyle::Hidden;
        this->m_pkDebugButtonPanel->ImageScalingSize = 
            System::Drawing::Size(32, 32);
        this->m_pkDebugButtonPanel->RenderMode = 
            System::Windows::Forms::ToolStripRenderMode::System;
        this->m_pkDebugButtonPanel->Location = System::Drawing::Point(0,0);
        this->m_pkDebugButtonPanel->Size = System::Drawing::Size(150, 40);
        this->m_pkDebugButtonPanel->Name = S"DebugButtonPanel";
        this->m_pkDebugButtonPanel->Items->Add(this->btImport);
        this->m_pkDebugButtonPanel->Items->Add(this->btExport);
        this->m_pkDebugButtonPanel->Items->Add(this->btSavePhysXData);
        this->m_pkDebugButtonPanel->Items->Add(this->cbUpdateLighting);
        // 
        // gbBrushShape
        // 
        this->gbBrushShape->Controls->Add(m_pkBrushPanel);        
        this->gbBrushShape->Location = System::Drawing::Point(0, 60);
        this->gbBrushShape->Name = S"gbBrushShape";
        this->gbBrushShape->Size = System::Drawing::Size(288, 152);
        this->gbBrushShape->TabIndex = 68;
        this->gbBrushShape->TabStop = false;
        this->gbBrushShape->Text = S"Brush Options:";
        //
        // m_pkBrushPanel
        //
        this->m_pkBrushPanel->Controls->Add(m_pmShapeLabel);
        this->m_pkBrushPanel->Controls->Add(BrushType);
        this->m_pkBrushPanel->Controls->Add(m_pkBrushOptionPanel);
        this->m_pkBrushPanel->Location = System::Drawing::Point(0,0);
        this->m_pkBrushPanel->Dock = DockStyle::Fill;
        this->m_pkBrushPanel->Name = S"BrushPanel";
        this->m_pkBrushPanel->SizeChanged += new EventHandler(this,
            &TerrainCommandPanel::ControlSizeChanged);
        //
        // Shape label
        //
        this->m_pmShapeLabel->Text = S"Select brush type:";
        this->m_pmShapeLabel->Location = System::Drawing::Point(7, 2);
        this->m_pmShapeLabel->Size = System::Drawing::Size(95, 13);
        this->m_pmShapeLabel->AutoSize = true;
        // 
        // BrushType
        // 
        this->BrushType->DropDownStyle = 
            System::Windows::Forms::ComboBoxStyle::DropDownList;
        this->BrushType->FormattingEnabled = true;
        System::Object* __mcTemp__1[] = new System::Object*[
                MBrush::ms_pmRegisteredBrushTypes->Count];
        for (Int32 i = 0; i < MBrush::ms_pmRegisteredBrushTypes->Count; ++i)
        {
            __mcTemp__1[i] = MBrush::ms_pmRegisteredBrushTypes->get_Item(i)->
                m_pmBrushTypeName;
        }
        this->BrushType->Items->AddRange(__mcTemp__1);
        this->BrushType->Location = System::Drawing::Point(105, 0);
        this->BrushType->Name = S"BrushType";
        this->BrushType->Size = System::Drawing::Size(87, 21);
        this->BrushType->TabIndex = 32;
        this->BrushType->SelectedIndexChanged += new System::EventHandler(
            this, &TerrainCommandPanel::BrushType_SelectedIndexChange);
        //
        // m_pkBrushOptionPanel
        //
        this->m_pkBrushOptionPanel->Size = System::Drawing::Size(313, 110);
        this->m_pkBrushOptionPanel->Location = System::Drawing::Point(7, 25);
        this->m_pkBrushOptionPanel->Name = S"BrushProperties";
        this->m_pkBrushOptionPanel->AutoScroll = true;
        // 
        // btImport
        // 
        this->btImport->Name = S"btImport";
        this->btImport->Size = System::Drawing::Size(47, 36);
        this->btImport->Text = S"Import from Heightmap";
        this->btImport->Click += new System::EventHandler(
            this, &TerrainCommandPanel::btImport_Click);
        this->btImport->BackgroundImageLayout = 
            System::Windows::Forms::ImageLayout::Stretch;
        this->btImport->DisplayStyle = 
            System::Windows::Forms::ToolStripItemDisplayStyle::Image;
        //
        // btExport
        //
        this->btExport->Name = S"btExport";
        this->btExport->Size = System::Drawing::Size(47, 36);
        this->btExport->Text = S"Export to Heightmap";
        this->btExport->Click += new System::EventHandler(
            this, &TerrainCommandPanel::btExport_Click);
        this->btExport->BackgroundImageLayout = 
            System::Windows::Forms::ImageLayout::Stretch;
        this->btExport->DisplayStyle = 
            System::Windows::Forms::ToolStripItemDisplayStyle::Image;
        //
        // btSavePhysXData
        //
        this->btSavePhysXData->Name = S"btSavePhysXData";
        this->btSavePhysXData->Size = System::Drawing::Size(47, 36);
        this->btSavePhysXData->Text = S"Force cooking of PhysX data";
        this->btSavePhysXData->Enabled = false;
        this->btSavePhysXData->Click += new System::EventHandler(
            this, &TerrainCommandPanel::btSavePhysXData_Click);
        this->btSavePhysXData->BackgroundImageLayout = 
            System::Windows::Forms::ImageLayout::Stretch;
        this->btSavePhysXData->DisplayStyle = 
            System::Windows::Forms::ToolStripItemDisplayStyle::Image;
        //
        // cbUpdateLighting
        //
        this->cbUpdateLighting->CheckOnClick = true;
        this->cbUpdateLighting->Name = S"cbUpdateLighting";
        this->cbUpdateLighting->Size = System::Drawing::Size(47, 36);
        this->cbUpdateLighting->Text = S"Update lighting whilst painting";
        this->cbUpdateLighting->Checked = false;
        this->cbUpdateLighting->CheckedChanged += new System::EventHandler(
            this, &TerrainCommandPanel::cbUpdateLighting_Changed);
        this->cbUpdateLighting->BackgroundImageLayout = 
            System::Windows::Forms::ImageLayout::Stretch;
        this->cbUpdateLighting->DisplayStyle = 
            System::Windows::Forms::ToolStripItemDisplayStyle::Image;
        // 
        // groupBox1
        // 
        this->groupBox1->Controls->Add(this->kBrushListElements);
        this->groupBox1->Location = System::Drawing::Point(0, 230);
        this->groupBox1->Name = S"groupBox1";
        this->groupBox1->Size = System::Drawing::Size(288, 280);
        this->groupBox1->TabIndex = 71;
        this->groupBox1->TabStop = false;
        this->groupBox1->Text = S"Brush Elements";
        // 
        // kBrushListElements
        // 
        this->kBrushListElements->AllowDrop = true;
        this->kBrushListElements->Dock = DockStyle::Fill;
        this->kBrushListElements->Location = System::Drawing::Point(3, 23);
        this->kBrushListElements->Name = S"kBrushListElements";
        this->kBrushListElements->Size = System::Drawing::Size(280, 250);
        this->kBrushListElements->TabIndex = 0;
        //
        // m_pkElementBrowserGroup
        //
        this->m_pkElementBrowserGroup->Controls->Add(
            this->m_pkElementBrowserTab);
        this->m_pkElementBrowserGroup->Location = 
            System::Drawing::Point(0, 475);
        this->m_pkElementBrowserGroup->Text = S"Element Browser";
        this->m_pkElementBrowserGroup->Size = System::Drawing::Size(285, 130);
        //
        // m_pkElementBrowserTab
        //
        this->m_pkElementBrowserTab->Location = System::Drawing::Point(4, 25);
        this->m_pkElementBrowserTab->Name = S"m_pkElementBrowserTab";
        this->m_pkElementBrowserTab->Size = System::Drawing::Size(280, 95);
        this->m_pkElementBrowserTab->Dock = DockStyle::Fill;
        this->m_pkElementBrowserTab->SelectedIndex = 0;      
        // 
        // TerrainCommandPanel
        // 
        this->AutoScroll = true;
        this->ClientSize = System::Drawing::Size(367, 601);
        this->Controls->Add(m_pkFlowPanel);
        this->Icon = (__try_cast<System::Drawing::Icon*>(resources->GetObject(S"$this.Icon")));
        this->Name = S"TerrainCommandPanel";
        this->Text = S"Terrain";
        this->Load += new System::EventHandler(
            this, &TerrainCommandPanel::OnLoad);
        this->VisibleChanged += new System::EventHandler(
            this, &TerrainCommandPanel::OnVisibleChanged);
        this->gbBrushShape->ResumeLayout(false);
        this->gbBrushShape->PerformLayout();
                
        this->groupBox1->ResumeLayout(false);
        this->ResumeLayout(false);
        this->PerformLayout();

    }
#pragma endregion

    #pragma region Event Handlers

    private: 
            
        //GUI Methods:
        System::Void BrushType_SelectedIndexChange(
            System::Object*  sender, System::EventArgs*  e);
        System::Void OnLoad(System::Object*  sender, System::EventArgs*  e);
        System::Void OnVisibleChanged(
            System::Object*  sender, System::EventArgs*  e);
        System::Void btImport_Click(
            System::Object*  sender, System::EventArgs*  e);
        System::Void btExport_Click(
            System::Object*  sender, System::EventArgs*  e);
        System::Void btSavePhysXData_Click(
            System::Object*  sender, System::EventArgs*  e);
        System::Void cbUpdateLighting_Changed(
            System::Object*  sender, System::EventArgs*  e);
        
        System::Void OperationButton_Click(System::Object*  sender, 
            System::EventArgs*  e);    
        System::Void OperationButton_MouseDown(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e);
        System::Void OperationButton_MouseMove(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e);
        System::Void OperationButton_MouseUp(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e);

        System::Void ControlSizeChanged(System::Object* sender,
            System::EventArgs*  e);
#pragma endregion
//
// ------------------------------------------------------------------------
//
    public:
        System::Void SetEntityLabel(String* pName, Color color);
        System::Void EnablePhysXPlugin();

    public:

    
    //Helper Methods:
    private:
        void BuildElementBrowserInterface();
        void BuildUtilityInterface();

        bool ReadHeightMap(String* pFileName, float* pMap, int& numVal,
            int& imageWidth, int minHeight, int maxHeight);

        bool LoadFromRAW(NiFile* pSource, float* pMap, NiUInt32& numVal,
            NiUInt16 numLOD, NiUInt32 uiBlockSize, NiInt32 iMinHeight, 
            NiInt32 iMaxHeight);

    public:
        System::Void EnableTerrainCommandPanel(bool bEnable);
        void Surface_DoubleClick(NiSurface* pkSelection);
        void Surface_Click(NiSurface* pkSelection);
        void Surface_Remove(NiSurface* pkSelection);
        void Palette_Update();
        void UseSurfacePalette(NiSurfacePalette* pkPalette);

        void Update();
        
    //Event Handlers:
        void OnBrushActivated(bool bActive);
        void HideSelection(bool bHideSelection);
    };
}
}}}
