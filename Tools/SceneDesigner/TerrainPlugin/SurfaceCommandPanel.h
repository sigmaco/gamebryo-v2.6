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
    public __gc class SurfaceCommandPanel : public System::Windows::Forms::Form
    {
    private:

        // Data Types:
        __gc class SurfaceListViewColumnSorter : public IComparer
        {
            public:
                // Specifies the column to sort by
                int SortColumn;
                
                // Specifies the order in which to sort ('Ascending' or 'Descending').
                SortOrder Order;
                
            private:
                // Case insensitive comparer object.
                CaseInsensitiveComparer *ObjectCompare;

            public:
                SurfaceListViewColumnSorter(void);
                ~SurfaceListViewColumnSorter(void);
                int Compare(Object *x, Object *y);
        };  

    public:
        SurfaceCommandPanel(void):
          m_pkPalette(0)
        {
            InitializeComponent();

            m_lvwColumnSorter = new SurfaceListViewColumnSorter();
            listSurface->ListViewItemSorter = m_lvwColumnSorter;
            m_kMode = S"By Package";

            m_panelBorderPen = new System::Drawing::Pen(System::Drawing::SystemColors::HotTrack);

            // Setup an extra button in the list of tags:
            m_btMoreTags = new System::Windows::Forms::LinkLabel();
            m_btMoreTags->Name = "__addTag";
            m_btMoreTags->Text = "...";
            m_btMoreTags->Margin = System::Windows::Forms::Padding(0,2,0,0);
            m_btMoreTags->Padding = System::Windows::Forms::Padding(0);
            m_btMoreTags->AutoSize = true;
            m_btMoreTags->Click += new System::EventHandler(
                this, &SurfaceCommandPanel::btMoreTags_Click);    
            flowTags->Controls->Add(m_btMoreTags);

            // Setup a combobox to turn up with the tags listed to add to the list
            m_cbMoreTags = new System::Windows::Forms::ComboBox();
            m_cbMoreTags->Name = "__addTagList";
            m_cbMoreTags->Text = "";
            m_cbMoreTags->Margin = System::Windows::Forms::Padding(0);
            m_cbMoreTags->Padding = System::Windows::Forms::Padding(0);
            m_cbMoreTags->Location = Point(0,-7);
            m_cbMoreTags->AutoCompleteMode = AutoCompleteMode::SuggestAppend;
            m_cbMoreTags->AutoCompleteSource = AutoCompleteSource::CustomSource;
            m_cbMoreTags->LostFocus += new System::EventHandler(
                this, &SurfaceCommandPanel::cbMoreTags_Blur); 

        }

    private: System::Windows::Forms::ImageList*  imgSurfaceList;
    private: System::ComponentModel::IContainer*  components;
    private: System::Windows::Forms::ToolStripMenuItem*  cmSurfaceDetailView;
    private: System::Windows::Forms::ToolStripMenuItem*  cmSurfaceLargeIcon;
    private: System::Windows::Forms::ToolStripMenuItem*  cmSurfaceListView;
    private: System::Windows::Forms::ImageList*  imgSurfaceListLarge;
    private: System::Windows::Forms::ImageList*  imgGeneral;
    private: System::Windows::Forms::ToolStripMenuItem*  cmSurfaceEdit;
    private: System::Windows::Forms::ToolStripSeparator*  cmSurfaceEditSeparator;
    private: System::Windows::Forms::ToolStripMenuItem*  cmSurfaceDelete;
    private: System::Windows::Forms::ToolStripPanel*  BottomToolStripPanel;
    private: System::Windows::Forms::ToolStripPanel*  TopToolStripPanel;
    private: System::Windows::Forms::ToolStripPanel*  RightToolStripPanel;
    private: System::Windows::Forms::ToolStripPanel*  LeftToolStripPanel;
    private: System::Windows::Forms::ToolStripContentPanel*  ContentPanel;
    private: System::Windows::Forms::SplitContainer*  splitModeSurfaces;
    private: System::Windows::Forms::ToolStripContainer*  toolStripContainer1;
    private: System::Windows::Forms::GroupBox*  groupBox1;
    private: System::Windows::Forms::TableLayoutPanel*  tableLayoutPanel1;
    private: System::Windows::Forms::ListView*  listSurface;
    private: System::Windows::Forms::ColumnHeader*  chSurfaceName;
    private: System::Windows::Forms::ColumnHeader*  chSurfacePackage;
    private: System::Windows::Forms::ColumnHeader*  chSurfaceDescription;
    private: System::Windows::Forms::Panel*  panel1;
    private: System::Windows::Forms::Label*  lbDescription;
    private: System::Windows::Forms::ToolStrip*  tsSurface;
    private: System::Windows::Forms::ToolStripButton*  tsSurfaceNew;
    private: System::Windows::Forms::ToolStripButton*  tsSurfaceEdit;
    private: System::Windows::Forms::ToolStripButton*  tsSurfaceDelete;
    private: System::Windows::Forms::Panel*  panSearch;
    private: System::Windows::Forms::ToolStrip*  tsPackage;
    private: System::Windows::Forms::ToolStripLabel*  toolStripLabel1;
    private: System::Windows::Forms::ToolStripComboBox*  cbSelectedPackage;
    private: System::Windows::Forms::ToolStripButton*  tsPackageNew;
    private: System::Windows::Forms::ToolStripButton*  tsPackageLoad;
    private: System::Windows::Forms::ToolStripButton*  tsPackageSave;
    private: System::Windows::Forms::ToolStripButton*  tsPackageClose;
    private: System::Windows::Forms::Label*  lbWithTags;
    private: System::Windows::Forms::FlowLayoutPanel*  flowTags;
    private: System::Windows::Forms::ToolStripButton*  tsPackageSaveAll;
    private: System::Windows::Forms::GroupBox*  groupBox2;
    private: System::Windows::Forms::Panel*  panTags;
    private: System::Windows::Forms::Label*  lbTagSummary;
    private: System::Windows::Forms::ListView*  listTags;
    private: System::Windows::Forms::ColumnHeader*  chName;
    private: System::Windows::Forms::ColumnHeader*  chDetails;
    private: System::Windows::Forms::ToolStripMenuItem*  toolStripMenuItem1;
    private: System::Windows::Forms::ToolStripMenuItem*  currentTerrainToolStripMenuItem;
    private: System::Windows::Forms::ToolStripMenuItem*  allLoadedTerrainsToolStripMenuItem;
    private: System::Windows::Forms::Label*  label1;
    private: bool m_bDirty;

    public: 
    public: void SetDirty();
    public: void SetClean();
    public: bool IsDirty();


    // Note that the sliders are NOT an authoritative source of size data, always read via the GET functions.
    private: System::Void InitializeComponent() {
                 this->components = (new System::ComponentModel::Container());
                 System::Windows::Forms::ContextMenuStrip*  cmSurfaceMenu;
                 System::Windows::Forms::ToolStripMenuItem*  cmSurfaceNew;
                 System::Windows::Forms::ToolStripSeparator*  cmSurfaceViewSeparator;
                 System::ComponentModel::ComponentResourceManager*  resources = (new System::ComponentModel::ComponentResourceManager(__typeof(SurfaceCommandPanel)));
                 this->cmSurfaceEdit = (new System::Windows::Forms::ToolStripMenuItem());
                 this->toolStripMenuItem1 = (new System::Windows::Forms::ToolStripMenuItem());
                 this->currentTerrainToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
                 this->allLoadedTerrainsToolStripMenuItem = (new System::Windows::Forms::ToolStripMenuItem());
                 this->cmSurfaceDelete = (new System::Windows::Forms::ToolStripMenuItem());
                 this->cmSurfaceEditSeparator = (new System::Windows::Forms::ToolStripSeparator());
                 this->cmSurfaceDetailView = (new System::Windows::Forms::ToolStripMenuItem());
                 this->cmSurfaceLargeIcon = (new System::Windows::Forms::ToolStripMenuItem());
                 this->cmSurfaceListView = (new System::Windows::Forms::ToolStripMenuItem());
                 this->imgGeneral = (new System::Windows::Forms::ImageList(this->components));
                 this->imgSurfaceListLarge = (new System::Windows::Forms::ImageList(this->components));
                 this->imgSurfaceList = (new System::Windows::Forms::ImageList(this->components));
                 this->BottomToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->TopToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->RightToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->LeftToolStripPanel = (new System::Windows::Forms::ToolStripPanel());
                 this->ContentPanel = (new System::Windows::Forms::ToolStripContentPanel());
                 this->splitModeSurfaces = (new System::Windows::Forms::SplitContainer());
                 this->groupBox2 = (new System::Windows::Forms::GroupBox());
                 this->panTags = (new System::Windows::Forms::Panel());
                 this->lbTagSummary = (new System::Windows::Forms::Label());
                 this->listTags = (new System::Windows::Forms::ListView());
                 this->chName = (new System::Windows::Forms::ColumnHeader());
                 this->chDetails = (new System::Windows::Forms::ColumnHeader());
                 this->groupBox1 = (new System::Windows::Forms::GroupBox());
                 this->tableLayoutPanel1 = (new System::Windows::Forms::TableLayoutPanel());
                 this->panSearch = (new System::Windows::Forms::Panel());
                 this->tsPackage = (new System::Windows::Forms::ToolStrip());
                 this->toolStripLabel1 = (new System::Windows::Forms::ToolStripLabel());
                 this->cbSelectedPackage = (new System::Windows::Forms::ToolStripComboBox());
                 this->tsPackageNew = (new System::Windows::Forms::ToolStripButton());
                 this->tsPackageLoad = (new System::Windows::Forms::ToolStripButton());
                 this->tsPackageClose = (new System::Windows::Forms::ToolStripButton());
                 this->tsPackageSave = (new System::Windows::Forms::ToolStripButton());
                 this->tsPackageSaveAll = (new System::Windows::Forms::ToolStripButton());
                 this->flowTags = (new System::Windows::Forms::FlowLayoutPanel());
                 this->lbWithTags = (new System::Windows::Forms::Label());
                 this->listSurface = (new System::Windows::Forms::ListView());
                 this->chSurfaceName = (new System::Windows::Forms::ColumnHeader());
                 this->chSurfacePackage = (new System::Windows::Forms::ColumnHeader());
                 this->chSurfaceDescription = (new System::Windows::Forms::ColumnHeader());
                 this->panel1 = (new System::Windows::Forms::Panel());
                 this->tsSurface = (new System::Windows::Forms::ToolStrip());
                 this->tsSurfaceNew = (new System::Windows::Forms::ToolStripButton());
                 this->tsSurfaceEdit = (new System::Windows::Forms::ToolStripButton());
                 this->tsSurfaceDelete = (new System::Windows::Forms::ToolStripButton());
                 this->lbDescription = (new System::Windows::Forms::Label());
                 this->toolStripContainer1 = (new System::Windows::Forms::ToolStripContainer());
                 cmSurfaceMenu = (new System::Windows::Forms::ContextMenuStrip(this->components));
                 cmSurfaceNew = (new System::Windows::Forms::ToolStripMenuItem());
                 cmSurfaceViewSeparator = (new System::Windows::Forms::ToolStripSeparator());
                 cmSurfaceMenu->SuspendLayout();
                 this->splitModeSurfaces->Panel1->SuspendLayout();
                 this->splitModeSurfaces->Panel2->SuspendLayout();
                 this->splitModeSurfaces->SuspendLayout();
                 this->groupBox2->SuspendLayout();
                 this->panTags->SuspendLayout();
                 this->groupBox1->SuspendLayout();
                 this->tableLayoutPanel1->SuspendLayout();
                 this->panSearch->SuspendLayout();
                 this->tsPackage->SuspendLayout();
                 this->panel1->SuspendLayout();
                 this->tsSurface->SuspendLayout();
                 this->toolStripContainer1->ContentPanel->SuspendLayout();
                 this->toolStripContainer1->SuspendLayout();
                 this->SuspendLayout();
                 // 
                 // cmSurfaceMenu
                 // 
                 cmSurfaceMenu->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
                 System::Windows::Forms::ToolStripItem* __mcTemp__1[] = new System::Windows::Forms::ToolStripItem*[9];
                 __mcTemp__1[0] = this->cmSurfaceEdit;
                 __mcTemp__1[1] = this->toolStripMenuItem1;
                 __mcTemp__1[2] = this->cmSurfaceDelete;
                 __mcTemp__1[3] = this->cmSurfaceEditSeparator;
                 __mcTemp__1[4] = cmSurfaceNew;
                 __mcTemp__1[5] = cmSurfaceViewSeparator;
                 __mcTemp__1[6] = this->cmSurfaceDetailView;
                 __mcTemp__1[7] = this->cmSurfaceLargeIcon;
                 __mcTemp__1[8] = this->cmSurfaceListView;
                 cmSurfaceMenu->Items->AddRange(__mcTemp__1);
                 cmSurfaceMenu->Name = S"cmSurfaceMenu";
                 cmSurfaceMenu->Size = System::Drawing::Size(166, 170);
                 // 
                 // cmSurfaceEdit
                 // 
                 this->cmSurfaceEdit->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
                 this->cmSurfaceEdit->Name = S"cmSurfaceEdit";
                 this->cmSurfaceEdit->Size = System::Drawing::Size(165, 22);
                 this->cmSurfaceEdit->Text = S"Edit Material";
                 this->cmSurfaceEdit->Click += new System::EventHandler(this, &SurfaceCommandPanel::cmSurfaceEdit_Click);
                 // 
                 // toolStripMenuItem1
                 // 
                 System::Windows::Forms::ToolStripItem* __mcTemp__2[] = new System::Windows::Forms::ToolStripItem*[2];
                 __mcTemp__2[0] = this->currentTerrainToolStripMenuItem;
                 __mcTemp__2[1] = this->allLoadedTerrainsToolStripMenuItem;
                 this->toolStripMenuItem1->DropDownItems->AddRange(__mcTemp__2);
                 this->toolStripMenuItem1->Name = S"toolStripMenuItem1";
                 this->toolStripMenuItem1->Size = System::Drawing::Size(165, 22);
                 this->toolStripMenuItem1->Text = S"Remove Material";
                 // 
                 // currentTerrainToolStripMenuItem
                 // 
                 this->currentTerrainToolStripMenuItem->Name = S"currentTerrainToolStripMenuItem";
                 this->currentTerrainToolStripMenuItem->Size = System::Drawing::Size(176, 22);
                 this->currentTerrainToolStripMenuItem->Text = S"Current Terrain";
                 this->currentTerrainToolStripMenuItem->Click += new System::EventHandler(this, &SurfaceCommandPanel::cmSurfaceRemoveCurrent_Click);
                 // 
                 // allLoadedTerrainsToolStripMenuItem
                 // 
                 this->allLoadedTerrainsToolStripMenuItem->Name = S"allLoadedTerrainsToolStripMenuItem";
                 this->allLoadedTerrainsToolStripMenuItem->Size = System::Drawing::Size(176, 22);
                 this->allLoadedTerrainsToolStripMenuItem->Text = S"All Loaded Terrains";
                 this->allLoadedTerrainsToolStripMenuItem->Click += new System::EventHandler(this, &SurfaceCommandPanel::cmSurfaceRemoveAll_Click);
                 // 
                 // cmSurfaceDelete
                 // 
                 this->cmSurfaceDelete->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
                 this->cmSurfaceDelete->Name = S"cmSurfaceDelete";
                 this->cmSurfaceDelete->Size = System::Drawing::Size(165, 22);
                 this->cmSurfaceDelete->Text = S"Delete Material";
                 this->cmSurfaceDelete->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsSurfaceDelete_Click);
                 // 
                 // cmSurfaceEditSeparator
                 // 
                 this->cmSurfaceEditSeparator->Name = S"cmSurfaceEditSeparator";
                 this->cmSurfaceEditSeparator->Size = System::Drawing::Size(162, 6);
                 // 
                 // cmSurfaceNew
                 // 
                 cmSurfaceNew->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
                 cmSurfaceNew->Name = S"cmSurfaceNew";
                 cmSurfaceNew->Size = System::Drawing::Size(165, 22);
                 cmSurfaceNew->Text = S"New Material";
                 cmSurfaceNew->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsSurfaceNew_Click);
                 // 
                 // cmSurfaceViewSeparator
                 // 
                 cmSurfaceViewSeparator->Name = S"cmSurfaceViewSeparator";
                 cmSurfaceViewSeparator->Size = System::Drawing::Size(162, 6);
                 // 
                 // cmSurfaceDetailView
                 // 
                 this->cmSurfaceDetailView->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
                 this->cmSurfaceDetailView->Name = S"cmSurfaceDetailView";
                 this->cmSurfaceDetailView->Size = System::Drawing::Size(165, 22);
                 this->cmSurfaceDetailView->Text = S"Detail View";
                 this->cmSurfaceDetailView->Click += new System::EventHandler(this, &SurfaceCommandPanel::cmSurfaceDetailView_Click);
                 // 
                 // cmSurfaceLargeIcon
                 // 
                 this->cmSurfaceLargeIcon->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
                 this->cmSurfaceLargeIcon->Name = S"cmSurfaceLargeIcon";
                 this->cmSurfaceLargeIcon->Size = System::Drawing::Size(165, 22);
                 this->cmSurfaceLargeIcon->Text = S"Large Icon View";
                 this->cmSurfaceLargeIcon->Click += new System::EventHandler(this, &SurfaceCommandPanel::cmSurfaceLargeIcon_Click);
                 // 
                 // cmSurfaceListView
                 // 
                 this->cmSurfaceListView->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
                 this->cmSurfaceListView->Name = S"cmSurfaceListView";
                 this->cmSurfaceListView->Size = System::Drawing::Size(165, 22);
                 this->cmSurfaceListView->Text = S"List View";
                 this->cmSurfaceListView->Click += new System::EventHandler(this, &SurfaceCommandPanel::cmSurfaceSmallIcon_Click);
                 // 
                 // imgGeneral
                 // 
                 this->imgGeneral->ImageStream = (__try_cast<System::Windows::Forms::ImageListStreamer*  >(resources->GetObject(S"imgGeneral.ImageStream")));
                 this->imgGeneral->TransparentColor = System::Drawing::Color::Transparent;
                 this->imgGeneral->Images->SetKeyName(0, S"Package");
                 this->imgGeneral->Images->SetKeyName(1, S"Tag");
                 // 
                 // imgSurfaceListLarge
                 // 
                 this->imgSurfaceListLarge->ColorDepth = System::Windows::Forms::ColorDepth::Depth24Bit;
                 this->imgSurfaceListLarge->ImageSize = System::Drawing::Size(24, 24);
                 this->imgSurfaceListLarge->TransparentColor = System::Drawing::Color::Transparent;
                 // 
                 // imgSurfaceList
                 // 
                 this->imgSurfaceList->ColorDepth = System::Windows::Forms::ColorDepth::Depth24Bit;
                 this->imgSurfaceList->ImageSize = System::Drawing::Size(16, 16);
                 this->imgSurfaceList->TransparentColor = System::Drawing::Color::Transparent;
                 // 
                 // BottomToolStripPanel
                 // 
                 this->BottomToolStripPanel->Location = System::Drawing::Point(0, 0);
                 this->BottomToolStripPanel->Name = S"BottomToolStripPanel";
                 this->BottomToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
                 this->BottomToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
                 this->BottomToolStripPanel->Size = System::Drawing::Size(0, 0);
                 // 
                 // TopToolStripPanel
                 // 
                 this->TopToolStripPanel->Dock = System::Windows::Forms::DockStyle::Top;
                 this->TopToolStripPanel->Location = System::Drawing::Point(0, 0);
                 this->TopToolStripPanel->Name = S"TopToolStripPanel";
                 this->TopToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
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
                 this->ContentPanel->AutoScroll = true;
                 this->ContentPanel->Size = System::Drawing::Size(404, 359);
                 // 
                 // splitModeSurfaces
                 // 
                 this->splitModeSurfaces->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->splitModeSurfaces->Location = System::Drawing::Point(0, 0);
                 this->splitModeSurfaces->Name = S"splitModeSurfaces";
                 // 
                 // splitModeSurfaces.Panel1
                 // 
                 this->splitModeSurfaces->Panel1->Controls->Add(this->groupBox2);
                 this->splitModeSurfaces->Panel1->Tag = S"";
                 // 
                 // splitModeSurfaces.Panel2
                 // 
                 this->splitModeSurfaces->Panel2->Controls->Add(this->groupBox1);
                 this->splitModeSurfaces->Size = System::Drawing::Size(627, 262);
                 this->splitModeSurfaces->SplitterDistance = 124;
                 this->splitModeSurfaces->TabIndex = 18;
                 // 
                 // groupBox2
                 // 
                 this->groupBox2->Controls->Add(this->panTags);
                 this->groupBox2->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->groupBox2->Location = System::Drawing::Point(0, 0);
                 this->groupBox2->Name = S"groupBox2";
                 this->groupBox2->Size = System::Drawing::Size(124, 262);
                 this->groupBox2->TabIndex = 19;
                 this->groupBox2->TabStop = false;
                 this->groupBox2->Text = S"Refine Search";
                 // 
                 // panTags
                 // 
                 this->panTags->BackColor = System::Drawing::SystemColors::Window;
                 this->panTags->Controls->Add(this->lbTagSummary);
                 this->panTags->Controls->Add(this->listTags);
                 this->panTags->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->panTags->Location = System::Drawing::Point(3, 16);
                 this->panTags->Name = S"panTags";
                 this->panTags->Size = System::Drawing::Size(118, 243);
                 this->panTags->TabIndex = 4;
                 this->panTags->Paint += new System::Windows::Forms::PaintEventHandler(this, &SurfaceCommandPanel::panTags_Paint);
                 this->panTags->Resize += new System::EventHandler(this, &SurfaceCommandPanel::panTags_Resize);
                 // 
                 // lbTagSummary
                 // 
                 this->lbTagSummary->AutoEllipsis = true;
                 this->lbTagSummary->AutoSize = true;
                 this->lbTagSummary->Font = (new System::Drawing::Font(S"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
                     (System::Byte)0));
                 this->lbTagSummary->ForeColor = System::Drawing::SystemColors::ControlDark;
                 this->lbTagSummary->Location = System::Drawing::Point(3, 7);
                 this->lbTagSummary->Margin = System::Windows::Forms::Padding(3);
                 this->lbTagSummary->Name = S"lbTagSummary";
                 this->lbTagSummary->Size = System::Drawing::Size(81, 13);
                 this->lbTagSummary->TabIndex = 5;
                 this->lbTagSummary->Text = S"No Tags Found";
                 // 
                 // listTags
                 // 
                 this->listTags->AllowDrop = true;
                 this->listTags->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
                     | System::Windows::Forms::AnchorStyles::Left) 
                     | System::Windows::Forms::AnchorStyles::Right);
                 this->listTags->BorderStyle = System::Windows::Forms::BorderStyle::None;
                 System::Windows::Forms::ColumnHeader* __mcTemp__3[] = new System::Windows::Forms::ColumnHeader*[2];
                 __mcTemp__3[0] = this->chName;
                 __mcTemp__3[1] = this->chDetails;
                 this->listTags->Columns->AddRange(__mcTemp__3);
                 this->listTags->Location = System::Drawing::Point(1, 23);
                 this->listTags->Name = S"listTags";
                 this->listTags->ShowItemToolTips = true;
                 this->listTags->Size = System::Drawing::Size(116, 221);
                 this->listTags->SmallImageList = this->imgGeneral;
                 this->listTags->Sorting = System::Windows::Forms::SortOrder::Ascending;
                 this->listTags->TabIndex = 4;
                 this->listTags->UseCompatibleStateImageBehavior = false;
                 this->listTags->View = System::Windows::Forms::View::List;
                 this->listTags->DoubleClick += new System::EventHandler(this, &SurfaceCommandPanel::listTags_DoubleClick);
                 // 
                 // groupBox1
                 // 
                 this->groupBox1->Controls->Add(this->tableLayoutPanel1);
                 this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->groupBox1->Location = System::Drawing::Point(0, 0);
                 this->groupBox1->Name = S"groupBox1";
                 this->groupBox1->Size = System::Drawing::Size(499, 262);
                 this->groupBox1->TabIndex = 0;
                 this->groupBox1->TabStop = false;
                 this->groupBox1->Text = S"Found Material";
                 // 
                 // tableLayoutPanel1
                 // 
                 this->tableLayoutPanel1->ColumnCount = 2;
                 this->tableLayoutPanel1->ColumnStyles->Add((new System::Windows::Forms::ColumnStyle()));
                 this->tableLayoutPanel1->ColumnStyles->Add((new System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 100)));
                 this->tableLayoutPanel1->Controls->Add(this->panSearch, 0, 0);
                 this->tableLayoutPanel1->Controls->Add(this->listSurface, 0, 1);
                 this->tableLayoutPanel1->Controls->Add(this->panel1, 0, 2);
                 this->tableLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->tableLayoutPanel1->Location = System::Drawing::Point(3, 16);
                 this->tableLayoutPanel1->Name = S"tableLayoutPanel1";
                 this->tableLayoutPanel1->RowCount = 3;
                 this->tableLayoutPanel1->RowStyles->Add((new System::Windows::Forms::RowStyle()));
                 this->tableLayoutPanel1->RowStyles->Add((new System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 100)));
                 this->tableLayoutPanel1->RowStyles->Add((new System::Windows::Forms::RowStyle()));
                 this->tableLayoutPanel1->RowStyles->Add((new System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
                 this->tableLayoutPanel1->Size = System::Drawing::Size(493, 243);
                 this->tableLayoutPanel1->TabIndex = 0;
                 // 
                 // panSearch
                 // 
                 this->panSearch->AutoSize = true;
                 this->panSearch->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
                 this->tableLayoutPanel1->SetColumnSpan(this->panSearch, 2);
                 this->panSearch->Controls->Add(this->tsPackage);
                 this->panSearch->Controls->Add(this->flowTags);
                 this->panSearch->Controls->Add(this->lbWithTags);
                 this->panSearch->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->panSearch->Location = System::Drawing::Point(0, 0);
                 this->panSearch->Margin = System::Windows::Forms::Padding(0);
                 this->panSearch->Name = S"panSearch";
                 this->panSearch->Size = System::Drawing::Size(493, 39);
                 this->panSearch->TabIndex = 23;
                 // 
                 // tsPackage
                 // 
                 this->tsPackage->BackColor = System::Drawing::SystemColors::Control;
                 this->tsPackage->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
                 System::Windows::Forms::ToolStripItem* __mcTemp__4[] = new System::Windows::Forms::ToolStripItem*[7];
                 __mcTemp__4[0] = this->toolStripLabel1;
                 __mcTemp__4[1] = this->cbSelectedPackage;
                 __mcTemp__4[2] = this->tsPackageNew;
                 __mcTemp__4[3] = this->tsPackageLoad;
                 __mcTemp__4[4] = this->tsPackageClose;
                 __mcTemp__4[5] = this->tsPackageSave;
                 __mcTemp__4[6] = this->tsPackageSaveAll;
                 this->tsPackage->Items->AddRange(__mcTemp__4);
                 this->tsPackage->Location = System::Drawing::Point(0, 0);
                 this->tsPackage->Name = S"tsPackage";
                 this->tsPackage->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
                 this->tsPackage->Size = System::Drawing::Size(493, 25);
                 this->tsPackage->TabIndex = 26;
                 // 
                 // toolStripLabel1
                 // 
                 this->toolStripLabel1->Name = S"toolStripLabel1";
                 this->toolStripLabel1->Size = System::Drawing::Size(61, 22);
                 this->toolStripLabel1->Text = S"Materials in";
                 // 
                 // cbSelectedPackage
                 // 
                 this->cbSelectedPackage->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
                 this->cbSelectedPackage->Name = S"cbSelectedPackage";
                 this->cbSelectedPackage->Size = System::Drawing::Size(121, 25);
                 this->cbSelectedPackage->SelectedIndexChanged += new System::EventHandler(this, &SurfaceCommandPanel::cbSelectedPackage_SelectedIndexChanged);
                 // 
                 // tsPackageNew
                 // 
                 this->tsPackageNew->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsPackageNew->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsPackageNew.Image")));
                 this->tsPackageNew->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsPackageNew->Name = S"tsPackageNew";
                 this->tsPackageNew->Size = System::Drawing::Size(23, 22);
                 this->tsPackageNew->Text = S"tsPackageNew";
                 this->tsPackageNew->ToolTipText = S"New Package";
                 this->tsPackageNew->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsPackageNew_Click);
                 // 
                 // tsPackageLoad
                 // 
                 this->tsPackageLoad->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsPackageLoad->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsPackageLoad.Image")));
                 this->tsPackageLoad->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsPackageLoad->Name = S"tsPackageLoad";
                 this->tsPackageLoad->Size = System::Drawing::Size(23, 22);
                 this->tsPackageLoad->Text = S"tsPackageLoad";
                 this->tsPackageLoad->ToolTipText = S"Load Package";
                 this->tsPackageLoad->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsPackageLoad_Click);
                 // 
                 // tsPackageClose
                 // 
                 this->tsPackageClose->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsPackageClose->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsPackageClose.Image")));
                 this->tsPackageClose->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsPackageClose->Name = S"tsPackageClose";
                 this->tsPackageClose->Size = System::Drawing::Size(23, 22);
                 this->tsPackageClose->Text = S"tsPackageClose";
                 this->tsPackageClose->ToolTipText = S"Close Package";
                 this->tsPackageClose->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsPackageClose_Click);
                 // 
                 // tsPackageSave
                 // 
                 this->tsPackageSave->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsPackageSave->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsPackageSave.Image")));
                 this->tsPackageSave->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsPackageSave->Name = S"tsPackageSave";
                 this->tsPackageSave->Size = System::Drawing::Size(23, 22);
                 this->tsPackageSave->Text = S"tsPackageSave";
                 this->tsPackageSave->ToolTipText = S"Save Package";
                 this->tsPackageSave->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsPackageSave_Click);
                 // 
                 // tsPackageSaveAll
                 // 
                 this->tsPackageSaveAll->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsPackageSaveAll->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsPackageSaveAll.Image")));
                 this->tsPackageSaveAll->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsPackageSaveAll->Name = S"tsPackageSaveAll";
                 this->tsPackageSaveAll->Size = System::Drawing::Size(23, 22);
                 this->tsPackageSaveAll->Text = S"Save All";
                 this->tsPackageSaveAll->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsPackageSaveAll_Click);
                 // 
                 // flowTags
                 // 
                 this->flowTags->AutoSize = true;
                 this->flowTags->BackColor = System::Drawing::SystemColors::Control;
                 this->flowTags->Location = System::Drawing::Point(55, 24);
                 this->flowTags->Margin = System::Windows::Forms::Padding(0);
                 this->flowTags->Name = S"flowTags";
                 this->flowTags->Size = System::Drawing::Size(129, 13);
                 this->flowTags->TabIndex = 19;
                 // 
                 // lbWithTags
                 // 
                 this->lbWithTags->AutoSize = true;
                 this->lbWithTags->Location = System::Drawing::Point(3, 26);
                 this->lbWithTags->Name = S"lbWithTags";
                 this->lbWithTags->Size = System::Drawing::Size(52, 13);
                 this->lbWithTags->TabIndex = 20;
                 this->lbWithTags->Text = S"with tags:";
                 // 
                 // listSurface
                 // 
                 this->listSurface->BackgroundImageTiled = true;
                 System::Windows::Forms::ColumnHeader* __mcTemp__5[] = new System::Windows::Forms::ColumnHeader*[3];
                 __mcTemp__5[0] = this->chSurfaceName;
                 __mcTemp__5[1] = this->chSurfacePackage;
                 __mcTemp__5[2] = this->chSurfaceDescription;
                 this->listSurface->Columns->AddRange(__mcTemp__5);
                 this->tableLayoutPanel1->SetColumnSpan(this->listSurface, 2);
                 this->listSurface->ContextMenuStrip = cmSurfaceMenu;
                 this->listSurface->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->listSurface->FullRowSelect = true;
                 this->listSurface->HideSelection = false;
                 this->listSurface->LargeImageList = this->imgSurfaceListLarge;
                 this->listSurface->Location = System::Drawing::Point(0, 39);
                 this->listSurface->Margin = System::Windows::Forms::Padding(0);
                 this->listSurface->MultiSelect = false;
                 this->listSurface->Name = S"listSurface";
                 this->listSurface->ShowItemToolTips = true;
                 this->listSurface->Size = System::Drawing::Size(493, 173);
                 this->listSurface->SmallImageList = this->imgSurfaceList;
                 this->listSurface->TabIndex = 24;
                 this->listSurface->UseCompatibleStateImageBehavior = false;
                 this->listSurface->SelectedIndexChanged += new System::EventHandler(this, &SurfaceCommandPanel::listSurface_SelectedIndexChanged);
                 this->listSurface->DoubleClick += new System::EventHandler(this, &SurfaceCommandPanel::listSurface_DoubleClick);
                 this->listSurface->Click += new System::EventHandler(this, &SurfaceCommandPanel::listSurface_Click);
                 // 
                 // chSurfaceName
                 // 
                 this->chSurfaceName->Text = S"Name";
                 this->chSurfaceName->Width = 50;
                 // 
                 // chSurfacePackage
                 // 
                 this->chSurfacePackage->Text = S"Package";
                 this->chSurfacePackage->Width = 50;
                 // 
                 // chSurfaceDescription
                 // 
                 this->chSurfaceDescription->Text = S"Description";
                 this->chSurfaceDescription->Width = 50;
                 // 
                 // panel1
                 // 
                 this->panel1->AutoSize = true;
                 this->panel1->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
                 this->tableLayoutPanel1->SetColumnSpan(this->panel1, 2);
                 this->panel1->Controls->Add(this->tsSurface);
                 this->panel1->Controls->Add(this->lbDescription);
                 this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->panel1->Location = System::Drawing::Point(3, 215);
                 this->panel1->Name = S"panel1";
                 this->panel1->Size = System::Drawing::Size(487, 25);
                 this->panel1->TabIndex = 27;
                 // 
                 // tsSurface
                 // 
                 this->tsSurface->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
                 this->tsSurface->BackColor = System::Drawing::SystemColors::Control;
                 this->tsSurface->Dock = System::Windows::Forms::DockStyle::None;
                 this->tsSurface->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
                 System::Windows::Forms::ToolStripItem* __mcTemp__6[] = new System::Windows::Forms::ToolStripItem*[3];
                 __mcTemp__6[0] = this->tsSurfaceNew;
                 __mcTemp__6[1] = this->tsSurfaceEdit;
                 __mcTemp__6[2] = this->tsSurfaceDelete;
                 this->tsSurface->Items->AddRange(__mcTemp__6);
                 this->tsSurface->Location = System::Drawing::Point(418, 0);
                 this->tsSurface->Name = S"tsSurface";
                 this->tsSurface->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
                 this->tsSurface->Size = System::Drawing::Size(72, 25);
                 this->tsSurface->TabIndex = 28;
                 this->tsSurface->Text = S"toolStrip1";
                 // 
                 // tsSurfaceNew
                 // 
                 this->tsSurfaceNew->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsSurfaceNew->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsSurfaceNew.Image")));
                 this->tsSurfaceNew->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsSurfaceNew->Name = S"tsSurfaceNew";
                 this->tsSurfaceNew->Size = System::Drawing::Size(23, 22);
                 this->tsSurfaceNew->Text = S"tsSurfaceNew";
                 this->tsSurfaceNew->ToolTipText = S"New Material";
                 this->tsSurfaceNew->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsSurfaceNew_Click);
                 // 
                 // tsSurfaceEdit
                 // 
                 this->tsSurfaceEdit->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsSurfaceEdit->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsSurfaceEdit.Image")));
                 this->tsSurfaceEdit->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsSurfaceEdit->Name = S"tsSurfaceEdit";
                 this->tsSurfaceEdit->Size = System::Drawing::Size(23, 22);
                 this->tsSurfaceEdit->Text = S"tsSurfaceEdit";
                 this->tsSurfaceEdit->ToolTipText = S"Edit Material";
                 this->tsSurfaceEdit->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsSurfaceEdit_Click);
                 // 
                 // tsSurfaceDelete
                 // 
                 this->tsSurfaceDelete->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
                 this->tsSurfaceDelete->Image = (__try_cast<System::Drawing::Image*  >(resources->GetObject(S"tsSurfaceDelete.Image")));
                 this->tsSurfaceDelete->ImageTransparentColor = System::Drawing::Color::Magenta;
                 this->tsSurfaceDelete->Name = S"tsSurfaceDelete";
                 this->tsSurfaceDelete->Size = System::Drawing::Size(23, 22);
                 this->tsSurfaceDelete->Text = S"tsSurfaceDelete";
                 this->tsSurfaceDelete->ToolTipText = S"Delete Material";
                 this->tsSurfaceDelete->Click += new System::EventHandler(this, &SurfaceCommandPanel::tsSurfaceDelete_Click);
                 // 
                 // lbDescription
                 // 
                 this->lbDescription->AutoSize = true;
                 this->lbDescription->Location = System::Drawing::Point(3, 0);
                 this->lbDescription->Name = S"lbDescription";
                 this->lbDescription->Size = System::Drawing::Size(100, 13);
                 this->lbDescription->TabIndex = 27;
                 this->lbDescription->Text = S"Material Description";
                 // 
                 // toolStripContainer1
                 // 
                 this->toolStripContainer1->BottomToolStripPanelVisible = false;
                 // 
                 // toolStripContainer1.ContentPanel
                 // 
                 this->toolStripContainer1->ContentPanel->AutoScroll = true;
                 this->toolStripContainer1->ContentPanel->Controls->Add(this->splitModeSurfaces);
                 this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(627, 262);
                 this->toolStripContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
                 this->toolStripContainer1->LeftToolStripPanelVisible = false;
                 this->toolStripContainer1->Location = System::Drawing::Point(0, 0);
                 this->toolStripContainer1->Name = S"toolStripContainer1";
                 this->toolStripContainer1->RightToolStripPanelVisible = false;
                 this->toolStripContainer1->Size = System::Drawing::Size(627, 287);
                 this->toolStripContainer1->TabIndex = 20;
                 this->toolStripContainer1->Text = S"toolStripContainer1";
                 // 
                 // SurfaceCommandPanel
                 // 
                 this->ClientSize = System::Drawing::Size(627, 287);
                 this->Controls->Add(this->toolStripContainer1);
                 this->Icon = (__try_cast<System::Drawing::Icon*>(resources->GetObject(S"$this.Icon")));
                 this->Name = S"SurfaceCommandPanel";
                 this->Text = S"Material Palette";
                 this->Load += new System::EventHandler(this, &SurfaceCommandPanel::SurfaceCommandPanel_Load);
                 cmSurfaceMenu->ResumeLayout(false);
                 this->splitModeSurfaces->Panel1->ResumeLayout(false);
                 this->splitModeSurfaces->Panel2->ResumeLayout(false);
                 this->splitModeSurfaces->ResumeLayout(false);
                 this->groupBox2->ResumeLayout(false);
                 this->panTags->ResumeLayout(false);
                 this->panTags->PerformLayout();
                 this->groupBox1->ResumeLayout(false);
                 this->tableLayoutPanel1->ResumeLayout(false);
                 this->tableLayoutPanel1->PerformLayout();
                 this->panSearch->ResumeLayout(false);
                 this->panSearch->PerformLayout();
                 this->tsPackage->ResumeLayout(false);
                 this->tsPackage->PerformLayout();
                 this->panel1->ResumeLayout(false);
                 this->panel1->PerformLayout();
                 this->tsSurface->ResumeLayout(false);
                 this->tsSurface->PerformLayout();
                 this->toolStripContainer1->ContentPanel->ResumeLayout(false);
                 this->toolStripContainer1->ResumeLayout(false);
                 this->toolStripContainer1->PerformLayout();
                 this->ResumeLayout(false);

             }
    private:
        // Data Members:
        System::String* m_kMode;
        NiSurfacePalette* m_pkPalette;
        SurfaceListViewColumnSorter *m_lvwColumnSorter;
        System::Drawing::Pen *m_panelBorderPen;
        System::Windows::Forms::LinkLabel *m_btMoreTags;
        System::Windows::Forms::ComboBox *m_cbMoreTags;

    // Common functions:
    public: void RefreshPackages();
    public: void UseSurfacePalette(NiSurfacePalette* pkPalette);
    private: void PopulatePackageList(const NiSurfacePackage* pSelectedPackage);
    private: void PopulateSurfaceList(void);
    private: void NotifyPaletteUpdate();

    // Surface Panel and global events
    private: System::Void EntitySelected(MEntity* pmEntity);
    private: System::Void SurfaceCommandPanel_Load(System::Object*  sender, System::EventArgs*  e);
    
    // Surface Modification Buttons
    private: System::Void tsSurfaceNew_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsSurfaceEdit_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsSurfaceDelete_Click(System::Object*  sender, System::EventArgs*  e);
    
    // Package Modification Buttons
    private: System::Void tsPackageNew_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsPackageLoad_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsPackageSave_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsPackageClose_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void tsPackageSaveAll_Click(System::Object*  sender, System::EventArgs*  e);
    public: System::Void SaveAll();

    // Various lists and their interaction events   
    private: System::Void listTags_DoubleClick(System::Object*  sender, System::EventArgs*  e);
    private: System::Void listSurface_SelectedIndexChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void listSurface_DoubleClick(System::Object*  sender, System::EventArgs*  e);
    private: System::Void listSurface_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void listSurface_ColumnClick(System::Object*  sender, System::Windows::Forms::ColumnClickEventArgs*  e);
    
    // Context Menu events
    private: System::Void cmSurfaceDetailView_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cmSurfaceLargeIcon_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cmSurfaceSmallIcon_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cmSurfaceEdit_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cmSurfaceDelete_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cmSurfaceRemoveCurrent_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cmSurfaceRemoveAll_Click(System::Object*  sender, System::EventArgs*  e);
    
    // Tag List Events
    private: System::Void btMoreTags_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cbMoreTags_Blur(System::Object*  sender, System::EventArgs*  e);
    private: System::Void cbMoreTags_Enter(System::Object*  sender, System::EventArgs*  e);
    private: System::Void listSelectedTags_Click(System::Object*  sender, System::EventArgs*  e);
    private: System::Void panTags_Paint(System::Object*  sender, System::Windows::Forms::PaintEventArgs*  e);
    private: System::Void cbSelectedPackage_SelectedIndexChanged(System::Object*  sender, System::EventArgs*  e);
    private: System::Void panTags_Resize(System::Object*  sender, System::EventArgs*  e);

};
}}}}
//END auto generated code
