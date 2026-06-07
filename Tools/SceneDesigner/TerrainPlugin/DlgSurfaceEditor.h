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
#include "NiFilename.h"
// Begin auto generated code
// This file has been auto generated

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    /// <summary> 
    /// Summary for MSampleUIEditorDialog
    ///
    /// WARNING: If you change the name of this class, you will need to change the 
    ///          'Resource File Name' property for the managed resource compiler tool 
    ///          associated with all .resx files this class depends on.  Otherwise,
    ///          the designers will not be able to interact properly with localized
    ///          resources associated with this form.
    /// </summary>
    public __gc class DlgSurfaceEditor : public System::Windows::Forms::Form
    {
    public:
        DlgSurfaceEditor(void):
            m_pkSurface(0),
            m_pkPalette(0)
        {
            InitializeComponent();
        }

    public: 


    public: 




    private: System::ComponentModel::IContainer*  components;



private: System::Windows::Forms::Button*  btCancel;

private: System::Windows::Forms::Button*  btApply;
private: System::Windows::Forms::GroupBox*  groupBox4;
private: System::Windows::Forms::PictureBox*  pbTexture;


private: System::Windows::Forms::TabPage*  tcpTags;

private: System::Windows::Forms::TabPage*  tcpTextures;



private: System::Windows::Forms::Button*  btFilename;

private: System::Windows::Forms::TextBox*  tbFilename;

private: System::Windows::Forms::TabPage*  tcpGeneral;
private: System::Windows::Forms::TextBox*  tbDescription;
private: System::Windows::Forms::Label*  label8;
private: System::Windows::Forms::TextBox*  tbName;
    private: System::Windows::Forms::ComboBox*  cbPackage;
private: System::Windows::Forms::Label*  label3;
private: System::Windows::Forms::Label*  label2;
private: System::Windows::Forms::TabControl*  tcTabs;
private: System::Windows::Forms::Label*  label9;





private: System::Windows::Forms::Label*  label5;
    private: System::Windows::Forms::NumericUpDown*  udScaleX;

    private: System::Windows::Forms::ListView*  lvTextures;
    private: System::Windows::Forms::CheckBox*  cbPreview;






private: System::Windows::Forms::ToolStripPanel*  BottomToolStripPanel;
private: System::Windows::Forms::ToolStripPanel*  TopToolStripPanel;
private: System::Windows::Forms::ToolStripPanel*  RightToolStripPanel;
private: System::Windows::Forms::ToolStripPanel*  LeftToolStripPanel;
private: System::Windows::Forms::ToolStripContentPanel*  ContentPanel;
private: System::Windows::Forms::ToolStripContainer*  toolStripContainer1;
private: System::Windows::Forms::ToolStrip*  toolStrip1;
private: System::Windows::Forms::ToolStripComboBox*  tscbTag;
private: System::Windows::Forms::ToolStripButton*  tsTagAdd;
private: System::Windows::Forms::ToolStripButton*  tsTagDelete;
private: System::Windows::Forms::ListBox*  lbTags;
    private: System::Windows::Forms::ImageList*  imgTextures;
    private: System::Windows::Forms::TabPage*  tcpMetaData;
    private: System::Windows::Forms::DataGridView*  dgMetaData;
    private: System::Windows::Forms::DataGridViewTextBoxColumn*  dgcKey;
    private: System::Windows::Forms::DataGridViewComboBoxColumn*  dgcDataType;
    private: System::Windows::Forms::DataGridViewTextBoxColumn*  dgcValue;
    private: System::Windows::Forms::ToolTip*  ttTip;






    private: System::Windows::Forms::NumericUpDown*  udOffsetY;

    private: System::Windows::Forms::NumericUpDown*  udOffsetX;

    private: System::Windows::Forms::Label*  label4;
    private: System::Windows::Forms::NumericUpDown*  udScaleY;
    private: System::Windows::Forms::Label*  label10;
    private: System::Windows::Forms::Label*  label7;
    private: System::Windows::Forms::Label*  label11;
    private: System::Windows::Forms::Label*  label12;

    private: System::Windows::Forms::NumericUpDown*  udDistStrength;

    private: System::Windows::Forms::Label*  label6;
private: System::Windows::Forms::GroupBox*  groupBox1;
























    public: 



    public: 

    public: 

    // Note that the sliders are NOT an authoritative source of size data, always read via the GET functions.

    private: System::Windows::Forms::Label*  label1;

    private: System::Void InitializeComponent() {
                 this->components = (new System::ComponentModel::Container());
                 System::ComponentModel::ComponentResourceManager*  resources = (new System::ComponentModel::ComponentResourceManager(__typeof(DlgSurfaceEditor)));
                 this->BottomToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->TopToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->RightToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->LeftToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->ContentPanel = (new System::Windows::Forms::ToolStripContentPanel());
                 this->btCancel = (new System::Windows::Forms::Button());
                 this->btApply = (new System::Windows::Forms::Button());
                 this->groupBox4 = (new System::Windows::Forms::GroupBox());
                 this->pbTexture = (new System::Windows::Forms::PictureBox());
                 this->tcpTags = (new System::Windows::Forms::TabPage());
                 this->toolStripContainer1 = (new System::Windows::Forms::ToolStripContainer());
                 this->toolStrip1 = (new System::Windows::Forms::ToolStrip());
                 this->tscbTag = (new System::Windows::Forms::ToolStripComboBox());
                 this->tsTagAdd = (new System::Windows::Forms::ToolStripButton());
                 this->tsTagDelete = (new System::Windows::Forms::ToolStripButton());
                 this->lbTags = (new System::Windows::Forms::ListBox());
                 this->tcpTextures = (new System::Windows::Forms::TabPage());
                 this->udDistStrength = (new System::Windows::Forms::NumericUpDown());
                 this->label6 = (new System::Windows::Forms::Label());
                 this->label11 = (new System::Windows::Forms::Label());
                 this->label12 = (new System::Windows::Forms::Label());
                 this->label10 = (new System::Windows::Forms::Label());
                 this->label7 = (new System::Windows::Forms::Label());
                 this->udOffsetY = (new System::Windows::Forms::NumericUpDown());
                 this->udOffsetX = (new System::Windows::Forms::NumericUpDown());
                 this->label4 = (new System::Windows::Forms::Label());
                 this->udScaleY = (new System::Windows::Forms::NumericUpDown());
                 this->cbPreview = (new System::Windows::Forms::CheckBox());
                 this->lvTextures = (new System::Windows::Forms::ListView());
                 this->imgTextures = (new System::Windows::Forms::ImageList(this->components));
                 this->udScaleX = (new System::Windows::Forms::NumericUpDown());
                 this->label5 = (new System::Windows::Forms::Label());
                 this->label9 = (new System::Windows::Forms::Label());
                 this->btFilename = (new System::Windows::Forms::Button());
                 this->tbFilename = (new System::Windows::Forms::TextBox());
                 this->groupBox1 = (new System::Windows::Forms::GroupBox());
                 this->tcpGeneral = (new System::Windows::Forms::TabPage());
                 this->tbDescription = (new System::Windows::Forms::TextBox());
                 this->label8 = (new System::Windows::Forms::Label());
                 this->tbName = (new System::Windows::Forms::TextBox());
                 this->cbPackage = (new System::Windows::Forms::ComboBox());
                 this->label3 = (new System::Windows::Forms::Label());
                 this->label2 = (new System::Windows::Forms::Label());
                 this->tcTabs = (new System::Windows::Forms::TabControl());
                 this->tcpMetaData = (new System::Windows::Forms::TabPage());
                 this->dgMetaData = (new System::Windows::Forms::DataGridView());
                 this->dgcKey = (new System::Windows::Forms::DataGridViewTextBoxColumn());
                 this->dgcDataType = (new System::Windows::Forms::DataGridViewComboBoxColumn());
                 this->dgcValue = (new System::Windows::Forms::DataGridViewTextBoxColumn());
                 this->ttTip = (new System::Windows::Forms::ToolTip(this->components));
                 this->groupBox4->SuspendLayout();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->pbTexture))->BeginInit();
                 this->tcpTags->SuspendLayout();
                 this->toolStripContainer1->BottomToolStripPanel->SuspendLayout();
                 this->toolStripContainer1->ContentPanel->SuspendLayout();
                 this->toolStripContainer1->SuspendLayout();
                 this->toolStrip1->SuspendLayout();
                 this->tcpTextures->SuspendLayout();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udDistStrength))->BeginInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udOffsetY))->BeginInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udOffsetX))->BeginInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udScaleY))->BeginInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udScaleX))->BeginInit();
                 this->groupBox1->SuspendLayout();
                 this->tcpGeneral->SuspendLayout();
                 this->tcTabs->SuspendLayout();
                 this->tcpMetaData->SuspendLayout();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->dgMetaData))->BeginInit();
                 this->SuspendLayout();
                 // 
                 // BottomToolStripPanel
                 // 
                 this->BottomToolStripPanel->Location = System::Drawing::Point(0, 0);
                 this->BottomToolStripPanel->Name = S"BottomToolStripPanel";
                 this->BottomToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
                 this->BottomToolStripPanel->Padding = System::Windows::Forms::Padding(0, 25, 0, 0);
                 this->BottomToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
                 this->BottomToolStripPanel->Size = System::Drawing::Size(0, 0);
                 // 
                 // TopToolStripPanel
                 // 
                 this->TopToolStripPanel->Location = System::Drawing::Point(0, 0);
                 this->TopToolStripPanel->Name = S"TopToolStripPanel";
                 this->TopToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
                 this->TopToolStripPanel->Padding = System::Windows::Forms::Padding(0, 0, 25, 25);
                 this->TopToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
                 this->TopToolStripPanel->Size = System::Drawing::Size(0, 0);
                 // 
                 // RightToolStripPanel
                 // 
                 this->RightToolStripPanel->Location = System::Drawing::Point(0, 0);
                 this->RightToolStripPanel->Name = S"RightToolStripPanel";
                 this->RightToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
                 this->RightToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
                 this->RightToolStripPanel->Size = System::Drawing::Size(0, 0);
                 // 
                 // LeftToolStripPanel
                 // 
                 this->LeftToolStripPanel->Location = System::Drawing::Point(0, 0);
                 this->LeftToolStripPanel->Name = S"LeftToolStripPanel";
                 this->LeftToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
                 this->LeftToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
                 this->LeftToolStripPanel->Size = System::Drawing::Size(0, 0);
                 // 
                 // ContentPanel
                 // 
                 this->ContentPanel->Size = System::Drawing::Size(273, 226);
                 // 
                 // btCancel
                 // 
                 this->btCancel->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right);
                 this->btCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
                 this->btCancel->Location = System::Drawing::Point(592, 254);
                 this->btCancel->Name = S"btCancel";
                 this->btCancel->Size = System::Drawing::Size(75, 23);
                 this->btCancel->TabIndex = 5;
                 this->btCancel->Text = S"Cancel";
                 this->btCancel->UseVisualStyleBackColor = true;
                 this->btCancel->Click += new System::EventHandler(this, &DlgSurfaceEditor::btCancel_Click);
                 // 
                 // btApply
                 // 
                 this->btApply->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right);
                 this->btApply->Location = System::Drawing::Point(511, 254);
                 this->btApply->Name = S"btApply";
                 this->btApply->Size = System::Drawing::Size(75, 23);
                 this->btApply->TabIndex = 6;
                 this->btApply->Text = S"Apply";
                 this->btApply->UseVisualStyleBackColor = true;
                 this->btApply->Click += new System::EventHandler(this, &DlgSurfaceEditor::btApply_Click);
                 // 
                 // groupBox4
                 // 
                 this->groupBox4->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->groupBox4->AutoSize = true;
                 this->groupBox4->Controls->Add(this->pbTexture);
                 this->groupBox4->Location = System::Drawing::Point(480, 12);
                 this->groupBox4->Name = S"groupBox4";
                 this->groupBox4->Size = System::Drawing::Size(206, 241);
                 this->groupBox4->TabIndex = 7;
                 this->groupBox4->TabStop = false;
                 this->groupBox4->Text = S"Preview";
                 this->groupBox4->Enter += new System::EventHandler(this, &DlgSurfaceEditor::groupBox4_Enter);
                 // 
                 // pbTexture
                 // 
                 this->pbTexture->BackColor = System::Drawing::SystemColors::ControlLight;
                 this->pbTexture->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
                 this->pbTexture->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->pbTexture->Location = System::Drawing::Point(3, 16);
                 this->pbTexture->Name = S"pbTexture";
                 this->pbTexture->Size = System::Drawing::Size(200, 222);
                 this->pbTexture->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
                 this->pbTexture->TabIndex = 2;
                 this->pbTexture->TabStop = false;
                 // 
                 // tcpTags
                 // 
                 this->tcpTags->Controls->Add(this->toolStripContainer1);
                 this->tcpTags->Location = System::Drawing::Point(4, 22);
                 this->tcpTags->Name = S"tcpTags";
                 this->tcpTags->Size = System::Drawing::Size(464, 246);
                 this->tcpTags->TabIndex = 2;
                 this->tcpTags->Text = S"Tags";
                 this->tcpTags->UseVisualStyleBackColor = true;
                 // 
                 // toolStripContainer1
                 // 
                 // 
                 // toolStripContainer1.BottomToolStripPanel
                 // 
                 this->toolStripContainer1->BottomToolStripPanel->Controls->Add(this->toolStrip1);
                 this->toolStripContainer1->BottomToolStripPanelVisible = false;
                 // 
                 // toolStripContainer1.ContentPanel
                 // 
                 this->toolStripContainer1->ContentPanel->Controls->Add(this->lbTags);
                 this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(464, 246);
                 this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->toolStripContainer1->LeftToolStripPanelVisible = false;
                 this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
                 this->toolStripContainer1->Name = S"toolStripContainer1";
                 this->toolStripContainer1->RightToolStripPanelVisible = false;
                 this->toolStripContainer1->Size = System::Drawing::Size(464, 246);
                 this->toolStripContainer1->TabIndex = 2;
                 this->toolStripContainer1->Text = S"toolStripContainer1";
                 // 
                 // toolStripContainer1.TopToolStripPanel
                 // 
                 this->toolStripContainer1->TopToolStripPanel->Padding = System::Windows::Forms::Padding(0, 0, 25, 25);
                 this->toolStripContainer1->TopToolStripPanelVisible = false;
                 // 
                 // toolStrip1
                 // 
                 this->toolStrip1->Dock = System::Windows::Forms::DockStyle::None;
                 this->toolStrip1->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
                 System::Windows::Forms::ToolStripItem* __mcTemp__1[] = new System::Windows::Forms::ToolStripItem*[3];
                 __mcTemp__1[0] = this->tscbTag;
                 __mcTemp__1[1] = this->tsTagAdd;
                 __mcTemp__1[2] = this->tsTagDelete;
                 this->toolStrip1->Items->AddRange(__mcTemp__1);
                 this->toolStrip1->LayoutStyle = System::Windows::Forms::ToolStripLayoutStyle::HorizontalStackWithOverflow;
                 this->toolStrip1->Location = System::Drawing::Point(3, 0);
                 this->toolStrip1->Name = S"toolStrip1";
                 this->toolStrip1->Size = System::Drawing::Size(172, 25);
                 this->toolStrip1->TabIndex = 1;
                 // 
                 // tscbTag
                 // 
                 this->tscbTag->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
                 this->tscbTag->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::CustomSource;
                 this->tscbTag->Name = S"tscbTag";
                 this->tscbTag->Size = System::Drawing::Size(121, 25);
                 this->tscbTag->KeyDown += new System::Windows::Forms::KeyEventHandler(this, &DlgSurfaceEditor::tscbTag_KeyDown);
                 // 
                 // tsTagAdd
                 // 
                 this->tsTagAdd->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsTagAdd->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsTagAdd.Image")));
                 this->tsTagAdd->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsTagAdd->Name = S"tsTagAdd";
                 this->tsTagAdd->Size = System::Drawing::Size(23, 22);
                 this->tsTagAdd->Text = S"Add Tag";
                 this->tsTagAdd->ToolTipText = S"Add Tag";
                 this->tsTagAdd->Click += new System::EventHandler(this, &DlgSurfaceEditor::tsTagAdd_Click);
                 // 
                 // tsTagDelete
                 // 
                 this->tsTagDelete->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsTagDelete->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsTagDelete.Image")));
                 this->tsTagDelete->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsTagDelete->Name = S"tsTagDelete";
                 this->tsTagDelete->Size = System::Drawing::Size(23, 22);
                 this->tsTagDelete->Text = S"Delete Tag";
                 this->tsTagDelete->ToolTipText = S"Delete Tag";
                 this->tsTagDelete->Click += new System::EventHandler(this, &DlgSurfaceEditor::tsTagDelete_Click);
                 // 
                 // lbTags
                 // 
                 this->lbTags->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->lbTags->FormattingEnabled = true;
                 this->lbTags->Location = System::Drawing::Point(0, 0);
                 this->lbTags->Name = S"lbTags";
                 this->lbTags->Size = System::Drawing::Size(464, 238);
                 this->lbTags->TabIndex = 1;
                 this->lbTags->SelectedIndexChanged += new System::EventHandler(this, &DlgSurfaceEditor::lbTags_SelectedIndexChanged);
                 // 
                 // tcpTextures
                 // 
                 this->tcpTextures->Controls->Add(this->udDistStrength);
                 this->tcpTextures->Controls->Add(this->label6);
                 this->tcpTextures->Controls->Add(this->label11);
                 this->tcpTextures->Controls->Add(this->label12);
                 this->tcpTextures->Controls->Add(this->label10);
                 this->tcpTextures->Controls->Add(this->label7);
                 this->tcpTextures->Controls->Add(this->udOffsetY);
                 this->tcpTextures->Controls->Add(this->label4);
                 this->tcpTextures->Controls->Add(this->udScaleY);
                 this->tcpTextures->Controls->Add(this->cbPreview);
                 this->tcpTextures->Controls->Add(this->lvTextures);
                 this->tcpTextures->Controls->Add(this->udScaleX);
                 this->tcpTextures->Controls->Add(this->label5);
                 this->tcpTextures->Controls->Add(this->label9);
                 this->tcpTextures->Controls->Add(this->btFilename);
                 this->tcpTextures->Controls->Add(this->tbFilename);
                 this->tcpTextures->Controls->Add(this->groupBox1);
                 this->tcpTextures->Location = System::Drawing::Point(4, 22);
                 this->tcpTextures->Name = S"tcpTextures";
                 this->tcpTextures->Padding = System::Windows::Forms::Padding(3);
                 this->tcpTextures->Size = System::Drawing::Size(464, 248);
                 this->tcpTextures->TabIndex = 1;
                 this->tcpTextures->Text = S"Textures";
                 this->tcpTextures->UseVisualStyleBackColor = true;
                 // 
                 // udDistStrength
                 // 
                 this->udDistStrength->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->udDistStrength->DecimalPlaces = 1;
                 System::Int32 __mcTemp__2[] = new System::Int32[4];
                 __mcTemp__2[0] = 5;
                 __mcTemp__2[1] = 0;
                 __mcTemp__2[2] = 0;
                 __mcTemp__2[3] = 65536;
                 this->udDistStrength->Increment = System::Decimal(__mcTemp__2);
                 this->udDistStrength->Location = System::Drawing::Point(282, 202);
                 System::Int32 __mcTemp__3[] = new System::Int32[4];
                 __mcTemp__3[0] = 50;
                 __mcTemp__3[1] = 0;
                 __mcTemp__3[2] = 0;
                 __mcTemp__3[3] = 0;
                 this->udDistStrength->Maximum = System::Decimal(__mcTemp__3);
                 this->udDistStrength->Name = S"udDistStrength";
                 this->udDistStrength->Size = System::Drawing::Size(57, 20);
                 this->udDistStrength->TabIndex = 56;
                 System::Int32 __mcTemp__4[] = new System::Int32[4];
                 __mcTemp__4[0] = 1;
                 __mcTemp__4[1] = 0;
                 __mcTemp__4[2] = 0;
                 __mcTemp__4[3] = 0;
                 this->udDistStrength->Value = System::Decimal(__mcTemp__4);
                 // 
                 // label6
                 // 
                 this->label6->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label6->AutoSize = true;
                 this->label6->Location = System::Drawing::Point(126, 204);
                 this->label6->Name = S"label6";
                 this->label6->Size = System::Drawing::Size(134, 13);
                 this->label6->TabIndex = 55;
                 this->label6->Text = S"Distribution Mask Strength:";
                 // 
                 // label11
                 // 
                 this->label11->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label11->AutoSize = true;
                 this->label11->Location = System::Drawing::Point(259, 175);
                 this->label11->Name = S"label11";
                 this->label11->Size = System::Drawing::Size(17, 13);
                 this->label11->TabIndex = 54;
                 this->label11->Text = S"V:";
                 // 
                 // label12
                 // 
                 this->label12->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label12->AutoSize = true;
                 this->label12->Location = System::Drawing::Point(170, 175);
                 this->label12->Name = S"label12";
                 this->label12->Size = System::Drawing::Size(18, 13);
                 this->label12->TabIndex = 53;
                 this->label12->Text = S"U:";
                 // 
                 // label10
                 // 
                 this->label10->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label10->AutoSize = true;
                 this->label10->Location = System::Drawing::Point(259, 146);
                 this->label10->Name = S"label10";
                 this->label10->Size = System::Drawing::Size(17, 13);
                 this->label10->TabIndex = 52;
                 this->label10->Text = S"V:";
                 // 
                 // label7
                 // 
                 this->label7->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label7->AutoSize = true;
                 this->label7->Location = System::Drawing::Point(170, 146);
                 this->label7->Name = S"label7";
                 this->label7->Size = System::Drawing::Size(18, 13);
                 this->label7->TabIndex = 51;
                 this->label7->Text = S"U:";
                 // 
                 // udOffsetY
                 // 
                 this->udOffsetY->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->udOffsetY->DecimalPlaces = 2;
                 System::Int32 __mcTemp__5[] = new System::Int32[4];
                 __mcTemp__5[0] = 5;
                 __mcTemp__5[1] = 0;
                 __mcTemp__5[2] = 0;
                 __mcTemp__5[3] = 131072;
                 this->udOffsetY->Increment = System::Decimal(__mcTemp__5);
                 this->udOffsetY->Location = System::Drawing::Point(282, 172);
                 System::Int32 __mcTemp__6[] = new System::Int32[4];
                 __mcTemp__6[0] = 1;
                 __mcTemp__6[1] = 0;
                 __mcTemp__6[2] = 0;
                 __mcTemp__6[3] = 0;
                 this->udOffsetY->Maximum = System::Decimal(__mcTemp__6);
                 this->udOffsetY->Name = S"udOffsetY";
                 this->udOffsetY->Size = System::Drawing::Size(57, 20);
                 this->udOffsetY->TabIndex = 50;
                 System::Int32 __mcTemp__7[] = new System::Int32[4];
                 __mcTemp__7[0] = 1;
                 __mcTemp__7[1] = 0;
                 __mcTemp__7[2] = 0;
                 __mcTemp__7[3] = 0;
                 this->udOffsetY->Value = System::Decimal(__mcTemp__7);
                 // 
                 // udOffsetX
                 // 
                 this->udOffsetX->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->udOffsetX->DecimalPlaces = 2;
                 System::Int32 __mcTemp__8[] = new System::Int32[4];
                 __mcTemp__8[0] = 5;
                 __mcTemp__8[1] = 0;
                 __mcTemp__8[2] = 0;
                 __mcTemp__8[3] = 131072;
                 this->udOffsetX->Increment = System::Decimal(__mcTemp__8);
                 this->udOffsetX->Location = System::Drawing::Point(155, 50);
                 System::Int32 __mcTemp__9[] = new System::Int32[4];
                 __mcTemp__9[0] = 1;
                 __mcTemp__9[1] = 0;
                 __mcTemp__9[2] = 0;
                 __mcTemp__9[3] = 0;
                 this->udOffsetX->Maximum = System::Decimal(__mcTemp__9);
                 this->udOffsetX->Name = S"udOffsetX";
                 this->udOffsetX->Size = System::Drawing::Size(61, 20);
                 this->udOffsetX->TabIndex = 49;
                 System::Int32 __mcTemp__10[] = new System::Int32[4];
                 __mcTemp__10[0] = 1;
                 __mcTemp__10[1] = 0;
                 __mcTemp__10[2] = 0;
                 __mcTemp__10[3] = 0;
                 this->udOffsetX->Value = System::Decimal(__mcTemp__10);
                 // 
                 // label4
                 // 
                 this->label4->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label4->AutoSize = true;
                 this->label4->Location = System::Drawing::Point(126, 174);
                 this->label4->Name = S"label4";
                 this->label4->Size = System::Drawing::Size(38, 13);
                 this->label4->TabIndex = 48;
                 this->label4->Text = S"Offset:";
                 // 
                 // udScaleY
                 // 
                 this->udScaleY->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->udScaleY->Location = System::Drawing::Point(282, 144);
                 System::Int32 __mcTemp__11[] = new System::Int32[4];
                 __mcTemp__11[0] = 2048;
                 __mcTemp__11[1] = 0;
                 __mcTemp__11[2] = 0;
                 __mcTemp__11[3] = 0;
                 this->udScaleY->Maximum = System::Decimal(__mcTemp__11);
                 System::Int32 __mcTemp__12[] = new System::Int32[4];
                 __mcTemp__12[0] = 1;
                 __mcTemp__12[1] = 0;
                 __mcTemp__12[2] = 0;
                 __mcTemp__12[3] = 0;
                 this->udScaleY->Minimum = System::Decimal(__mcTemp__12);
                 this->udScaleY->Name = S"udScaleY";
                 this->udScaleY->Size = System::Drawing::Size(57, 20);
                 this->udScaleY->TabIndex = 47;
                 System::Int32 __mcTemp__13[] = new System::Int32[4];
                 __mcTemp__13[0] = 1;
                 __mcTemp__13[1] = 0;
                 __mcTemp__13[2] = 0;
                 __mcTemp__13[3] = 0;
                 this->udScaleY->Value = System::Decimal(__mcTemp__13);
                 // 
                 // cbPreview
                 // 
                 this->cbPreview->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
                 this->cbPreview->AutoSize = true;
                 this->cbPreview->Location = System::Drawing::Point(90, 73);
                 this->cbPreview->Name = S"cbPreview";
                 this->cbPreview->Size = System::Drawing::Size(100, 17);
                 this->cbPreview->TabIndex = 41;
                 this->cbPreview->Text = S"Display preview";
                 this->cbPreview->UseVisualStyleBackColor = true;
                 this->cbPreview->CheckedChanged += new System::EventHandler(this, &DlgSurfaceEditor::cbPreview_CheckedChanged);
                 // 
                 // lvTextures
                 // 
                 this->lvTextures->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->lvTextures->HideSelection = false;
                 this->lvTextures->LargeImageList = this->imgTextures;
                 this->lvTextures->Location = System::Drawing::Point(34, 13);
                 this->lvTextures->Name = S"lvTextures";
                 this->lvTextures->Size = System::Drawing::Size(398, 58);
                 this->lvTextures->TabIndex = 40;
                 this->lvTextures->UseCompatibleStateImageBehavior = false;
                 this->lvTextures->ItemActivate += new System::EventHandler(this, &DlgSurfaceEditor::lvTextures_ItemActivate);
                 this->lvTextures->SelectedIndexChanged += new System::EventHandler(this, &DlgSurfaceEditor::lvTextures_SelectedIndexChanged);
                 // 
                 // imgTextures
                 // 
                 this->imgTextures->ColorDepth = System::Windows::Forms::ColorDepth::Depth8Bit;
                 this->imgTextures->ImageSize = System::Drawing::Size(20, 20);
                 this->imgTextures->TransparentColor = System::Drawing::Color::Transparent;
                 // 
                 // udScaleX
                 // 
                 this->udScaleX->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->udScaleX->Location = System::Drawing::Point(190, 144);
                 System::Int32 __mcTemp__14[] = new System::Int32[4];
                 __mcTemp__14[0] = 2048;
                 __mcTemp__14[1] = 0;
                 __mcTemp__14[2] = 0;
                 __mcTemp__14[3] = 0;
                 this->udScaleX->Maximum = System::Decimal(__mcTemp__14);
                 System::Int32 __mcTemp__15[] = new System::Int32[4];
                 __mcTemp__15[0] = 1;
                 __mcTemp__15[1] = 0;
                 __mcTemp__15[2] = 0;
                 __mcTemp__15[3] = 0;
                 this->udScaleX->Minimum = System::Decimal(__mcTemp__15);
                 this->udScaleX->Name = S"udScaleX";
                 this->udScaleX->Size = System::Drawing::Size(61, 20);
                 this->udScaleX->TabIndex = 39;
                 System::Int32 __mcTemp__16[] = new System::Int32[4];
                 __mcTemp__16[0] = 1;
                 __mcTemp__16[1] = 0;
                 __mcTemp__16[2] = 0;
                 __mcTemp__16[3] = 0;
                 this->udScaleX->Value = System::Decimal(__mcTemp__16);
                 // 
                 // label5
                 // 
                 this->label5->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
                 this->label5->AutoSize = true;
                 this->label5->Location = System::Drawing::Point(125, 144);
                 this->label5->Name = S"label5";
                 this->label5->Size = System::Drawing::Size(37, 13);
                 this->label5->TabIndex = 37;
                 this->label5->Text = S"Scale:";
                 // 
                 // label9
                 // 
                 this->label9->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
                 this->label9->AutoSize = true;
                 this->label9->Location = System::Drawing::Point(32, 74);
                 this->label9->Name = S"label9";
                 this->label9->Size = System::Drawing::Size(52, 13);
                 this->label9->TabIndex = 34;
                 this->label9->Text = S"Filename:";
                 // 
                 // btFilename
                 // 
                 this->btFilename->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right);
                 this->btFilename->Location = System::Drawing::Point(407, 96);
                 this->btFilename->Name = S"btFilename";
                 this->btFilename->Size = System::Drawing::Size(27, 19);
                 this->btFilename->TabIndex = 19;
                 this->btFilename->Text = S"...";
                 this->btFilename->UseVisualStyleBackColor = true;
                 this->btFilename->Click += new System::EventHandler(this, &DlgSurfaceEditor::btFilename_Click);
                 // 
                 // tbFilename
                 // 
                 this->tbFilename->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->tbFilename->Location = System::Drawing::Point(34, 96);
                 this->tbFilename->Name = S"tbFilename";
                 this->tbFilename->Size = System::Drawing::Size(367, 20);
                 this->tbFilename->TabIndex = 18;
                 this->tbFilename->TextChanged += new System::EventHandler(this, &DlgSurfaceEditor::tbFilename_TextChanged);
                 // 
                 // groupBox1
                 // 
                 this->groupBox1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->groupBox1->Controls->Add(this->udOffsetX);
                 this->groupBox1->Location = System::Drawing::Point(35, 122);
                 this->groupBox1->Name = S"groupBox1";
                 this->groupBox1->Size = System::Drawing::Size(396, 113);
                 this->groupBox1->TabIndex = 57;
                 this->groupBox1->TabStop = false;
                 this->groupBox1->Text = S"Parameters";
                 // 
                 // tcpGeneral
                 // 
                 this->tcpGeneral->Controls->Add(this->tbDescription);
                 this->tcpGeneral->Controls->Add(this->label8);
                 this->tcpGeneral->Controls->Add(this->tbName);
                 this->tcpGeneral->Controls->Add(this->cbPackage);
                 this->tcpGeneral->Controls->Add(this->label3);
                 this->tcpGeneral->Controls->Add(this->label2);
                 this->tcpGeneral->Location = System::Drawing::Point(4, 22);
                 this->tcpGeneral->Name = S"tcpGeneral";
                 this->tcpGeneral->Padding = System::Windows::Forms::Padding(3);
                 this->tcpGeneral->Size = System::Drawing::Size(464, 248);
                 this->tcpGeneral->TabIndex = 0;
                 this->tcpGeneral->Text = S"General";
                 this->tcpGeneral->UseVisualStyleBackColor = true;
                 // 
                 // tbDescription
                 // 
                 this->tbDescription->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->tbDescription->Location = System::Drawing::Point(89, 127);
                 this->tbDescription->Multiline = true;
                 this->tbDescription->Name = S"tbDescription";
                 this->tbDescription->Size = System::Drawing::Size(289, 89);
                 this->tbDescription->TabIndex = 11;
                 // 
                 // label8
                 // 
                 this->label8->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->label8->AutoSize = true;
                 this->label8->Location = System::Drawing::Point(86, 111);
                 this->label8->Name = S"label8";
                 this->label8->Size = System::Drawing::Size(63, 13);
                 this->label8->TabIndex = 10;
                 this->label8->Text = S"Description:";
                 // 
                 // tbName
                 // 
                 this->tbName->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->tbName->Location = System::Drawing::Point(89, 48);
                 this->tbName->Name = S"tbName";
                 this->tbName->Size = System::Drawing::Size(289, 20);
                 this->tbName->TabIndex = 9;
                 // 
                 // cbPackage
                 // 
                 this->cbPackage->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->cbPackage->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::CustomSource;
                 this->cbPackage->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
                 this->cbPackage->FormattingEnabled = true;
                 this->cbPackage->Location = System::Drawing::Point(89, 87);
                 this->cbPackage->Name = S"cbPackage";
                 this->cbPackage->Size = System::Drawing::Size(289, 21);
                 this->cbPackage->TabIndex = 8;
                 this->cbPackage->SelectedIndexChanged += new System::EventHandler(this, &DlgSurfaceEditor::cbPackage_SelectedIndexChanged);
                 // 
                 // label3
                 // 
                 this->label3->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->label3->AutoSize = true;
                 this->label3->Location = System::Drawing::Point(86, 71);
                 this->label3->Name = S"label3";
                 this->label3->Size = System::Drawing::Size(53, 13);
                 this->label3->TabIndex = 7;
                 this->label3->Text = S"Package:";
                 // 
                 // label2
                 // 
                 this->label2->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->label2->AutoSize = true;
                 this->label2->Location = System::Drawing::Point(86, 32);
                 this->label2->Name = S"label2";
                 this->label2->Size = System::Drawing::Size(38, 13);
                 this->label2->TabIndex = 6;
                 this->label2->Text = S"Name:";
                 // 
                 // tcTabs
                 // 
                 this->tcTabs->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->tcTabs->Controls->Add(this->tcpGeneral);
                 this->tcTabs->Controls->Add(this->tcpTextures);
                 this->tcTabs->Controls->Add(this->tcpTags);
                 this->tcTabs->Controls->Add(this->tcpMetaData);
                 this->tcTabs->Location = System::Drawing::Point(2, 3);
                 this->tcTabs->Name = S"tcTabs";
                 this->tcTabs->SelectedIndex = 0;
                 this->tcTabs->Size = System::Drawing::Size(472, 274);
                 this->tcTabs->TabIndex = 8;
                 // 
                 // tcpMetaData
                 // 
                 this->tcpMetaData->Controls->Add(this->dgMetaData);
                 this->tcpMetaData->Location = System::Drawing::Point(4, 22);
                 this->tcpMetaData->Name = S"tcpMetaData";
                 this->tcpMetaData->Padding = System::Windows::Forms::Padding(3);
                 this->tcpMetaData->Size = System::Drawing::Size(464, 246);
                 this->tcpMetaData->TabIndex = 3;
                 this->tcpMetaData->Text = S"Meta Data";
                 this->tcpMetaData->UseVisualStyleBackColor = true;
                 // 
                 // dgMetaData
                 // 
                 this->dgMetaData->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::Fill;
                 this->dgMetaData->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
                 this->dgMetaData->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
                 System::Windows::Forms::DataGridViewColumn* __mcTemp__17[] = new System::Windows::Forms::DataGridViewColumn*[3];
                 __mcTemp__17[0] = this->dgcKey;
                 __mcTemp__17[1] = this->dgcDataType;
                 __mcTemp__17[2] = this->dgcValue;
                 this->dgMetaData->Columns->AddRange(__mcTemp__17);
                 this->dgMetaData->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->dgMetaData->Location = System::Drawing::Point(3, 3);
                 this->dgMetaData->MultiSelect = false;
                 this->dgMetaData->Name = S"dgMetaData";
                 this->dgMetaData->RowHeadersWidth = 25;
                 this->dgMetaData->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
                 this->dgMetaData->ShowEditingIcon = false;
                 this->dgMetaData->Size = System::Drawing::Size(458, 240);
                 this->dgMetaData->TabIndex = 1;
                 this->dgMetaData->CellValueChanged += new System::Windows::Forms::DataGridViewCellEventHandler(this, &DlgSurfaceEditor::dgMetaData_CellValueChanged);
                 this->dgMetaData->CellValidating += new System::Windows::Forms::DataGridViewCellValidatingEventHandler(this, &DlgSurfaceEditor::dgMetaData_CellValidating);
                 this->dgMetaData->KeyDown += new System::Windows::Forms::KeyEventHandler(this, &DlgSurfaceEditor::dgMetaData_KeyDown);
                 this->dgMetaData->RowsRemoved += new System::Windows::Forms::DataGridViewRowsRemovedEventHandler(this, &DlgSurfaceEditor::dgMetaData_RowsRemoved);
                 // 
                 // dgcKey
                 // 
                 this->dgcKey->HeaderText = S"Key";
                 this->dgcKey->Name = S"dgcKey";
                 this->dgcKey->Resizable = System::Windows::Forms::DataGridViewTriState::True;
                 this->dgcKey->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
                 // 
                 // dgcDataType
                 // 
                 this->dgcDataType->HeaderText = S"DataType";
                 System::Object* __mcTemp__18[] = new System::Object*[5];
                 __mcTemp__18[0] = S"INTEGER";
                 __mcTemp__18[1] = S"FLOAT";
                 __mcTemp__18[2] = S"STRING";
                 __mcTemp__18[3] = S"FLOAT_BLENDED";
                 __mcTemp__18[4] = S"INTEGER_BLENDED";
                 this->dgcDataType->Items->AddRange(__mcTemp__18);
                 this->dgcDataType->Name = S"dgcDataType";
                 // 
                 // dgcValue
                 // 
                 this->dgcValue->HeaderText = S"Value";
                 this->dgcValue->Name = S"dgcValue";
                 // 
                 // ttTip
                 // 
                 this->ttTip->AutomaticDelay = 0;
                 this->ttTip->IsBalloon = true;
                 this->ttTip->ShowAlways = true;
                 // 
                 // DlgSurfaceEditor
                 // 
                 this->ClientSize = System::Drawing::Size(690, 278);
                 this->Controls->Add(this->tcTabs);
                 this->Controls->Add(this->groupBox4);
                 this->Controls->Add(this->btApply);
                 this->Controls->Add(this->btCancel);
                 this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
                 this->Name = S"DlgSurfaceEditor";
                 this->ShowInTaskbar = false;
                 this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
                 this->Text = S"Edit Material";
                 this->Load += new System::EventHandler(this, &DlgSurfaceEditor::DlgSurfaceEditor_Load);
                 this->groupBox4->ResumeLayout(false);
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->pbTexture))->EndInit();
                 this->tcpTags->ResumeLayout(false);
                 this->toolStripContainer1->BottomToolStripPanel->ResumeLayout(false);
                 this->toolStripContainer1->BottomToolStripPanel->PerformLayout();
                 this->toolStripContainer1->ContentPanel->ResumeLayout(false);
                 this->toolStripContainer1->ResumeLayout(false);
                 this->toolStripContainer1->PerformLayout();
                 this->toolStrip1->ResumeLayout(false);
                 this->toolStrip1->PerformLayout();
                 this->tcpTextures->ResumeLayout(false);
                 this->tcpTextures->PerformLayout();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udDistStrength))->EndInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udOffsetY))->EndInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udOffsetX))->EndInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udScaleY))->EndInit();
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->udScaleX))->EndInit();
                 this->groupBox1->ResumeLayout(false);
                 this->tcpGeneral->ResumeLayout(false);
                 this->tcpGeneral->PerformLayout();
                 this->tcTabs->ResumeLayout(false);
                 this->tcpMetaData->ResumeLayout(false);
                 (__try_cast<System::ComponentModel::ISupportInitialize*  >(this->dgMetaData))->EndInit();
                 this->ResumeLayout(false);
                 this->PerformLayout();

             };

    private: NiSurface *m_pkSurface;
    private: NiSurfacePalette *m_pkPalette;

    public: void InitialiseSurface(NiSurfacePalette *pkPalette, 
                NiSurface *pkSurface, System::String* pCurrentPackage);
    private: void ViewMapDetails(ListViewItem *pkItem);
    private: bool IsExistingMaterialName(const char* pcExistingName,
                 const char* pcMaterialName, NiSurfacePackage* pkPackage);

    // Events
    private: System::Void tsTagAdd_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsTagDelete_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void btCancel_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void btApply_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void DlgSurfaceEditor_Load(System::Object*  sender, System::EventArgs*  e);
    private: System::Void groupBox4_Enter(System::Object*  sender, System::EventArgs*  e);
    private: System::Void lbTags_SelectedIndexChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void toolStripContainer1_BottomToolStripPanel_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void lvTextures_SelectedIndexChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void lvTextures_ItemActivate(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cbPreview_CheckedChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void btFilename_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tbFilename_TextChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cbPackage_SelectedIndexChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void dgMetaData_CellValidating(System::Object*  sender, System::Windows::Forms::DataGridViewCellValidatingEventArgs*  e);
    private: System::Void dgMetaData_CellValueChanged(System::Object*  sender, System::Windows::Forms::DataGridViewCellEventArgs*  e);
    private: System::Void dgMetaData_RowsRemoved(System::Object*  sender, System::Windows::Forms::DataGridViewRowsRemovedEventArgs*  e);
    private: System::Void dgMetaData_KeyDown(System::Object* sender, System::Windows::Forms::KeyEventArgs*  e);
    private: System::Void tscbTag_KeyDown(System::Object*  sender, System::Windows::Forms::KeyEventArgs*  e);
};
}}}}
//END auto generated code
