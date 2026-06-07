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

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Diagnostics;
using Microsoft.Win32;
using WeifenLuo.WinFormsUI;
using NiManagedToolInterface;



namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for MainForm.
    /// </summary>
    public class MainForm : System.Windows.Forms.Form
    {
        private EditViewForm m_kEditView;
        private OutputViewForm m_kOutputView;
        private ProjectViewForm m_kProjectView;
        private RenderForm m_kRenderView;
        private TransitionsViewForm m_kTransitionsView;
        private PhysicsViewForm m_kPhysicsView;
        public string m_strAutoLoadFilename;
        private string m_strRegistryPath;
        private const string ms_strWindowSubPath = "\\Window";
        private System.Windows.Forms.MainMenu menuApp;
        private System.Windows.Forms.MenuItem miFileMenu;
        private System.Windows.Forms.MenuItem miNew;
        private System.Windows.Forms.MenuItem miOpen;
        private System.Windows.Forms.MenuItem miFileMRU;
        private System.Windows.Forms.MenuItem miSeparator1;
        private System.Windows.Forms.MenuItem miSave;
        private System.Windows.Forms.MenuItem miSaveAs;
        private System.Windows.Forms.MenuItem miSeparator2;
        private System.Windows.Forms.MenuItem miExit;
        private System.Windows.Forms.MenuItem miEditMenu;
        private System.Windows.Forms.MenuItem miSaveDockConfig;
        private System.Windows.Forms.MenuItem miTransitionTableSettings;
        private System.Windows.Forms.MenuItem miResetMessageBoxes;
        private System.Windows.Forms.MenuItem miViewMenu;
        private System.Windows.Forms.MenuItem miHelpMenu;
        private System.Windows.Forms.MenuItem miAbout;
        private System.Windows.Forms.ToolBar tbApp;
        private System.Windows.Forms.ToolBarButton tbbNew;
        private System.Windows.Forms.ToolBarButton tbbOpen;
        private System.Windows.Forms.ToolBarButton tbbSave;
        private System.Windows.Forms.ToolBarButton tbbSeparator1;
        private System.Windows.Forms.ToolBarButton tbbAddSequences;
        private System.Windows.Forms.ToolBarButton tbbSeparator2;
        private System.Windows.Forms.ToolBarButton tbbProjectView;
        private System.Windows.Forms.ToolBarButton tbbEditView;
        private System.Windows.Forms.ToolBarButton tbbRenderView;
        private System.Windows.Forms.ToolBarButton tbbTransitionsView;
        private System.Windows.Forms.ToolBarButton tbbOutputView;
        private System.Windows.Forms.ToolBarButton tbbPhysicsView;
        private System.Windows.Forms.ImageList ilApp;
        private WeifenLuo.WinFormsUI.DockPanel dpDockManager;
        private System.Windows.Forms.MenuItem miProjectView;
        private System.Windows.Forms.MenuItem miEditView;
        private System.Windows.Forms.MenuItem miRenderView;
        private System.Windows.Forms.MenuItem miTransitionsView;
        private System.Windows.Forms.MenuItem miOutputView;
        private System.Windows.Forms.MenuItem miPhysicsView;
        private System.Windows.Forms.MenuItem miSceneOptions;
        private string m_strBaseAppTitle;
        private System.Windows.Forms.MenuItem miResetDockConfig;
        private System.Windows.Forms.ToolBarButton tbbInteractivePreview;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem miInteractivePreview;
        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.MenuItem miHelpContents;
        private System.Windows.Forms.MenuItem miHelpIndex;
        private System.Windows.Forms.MenuItem miHelpSearch;
        private System.Windows.Forms.MenuItem miMsgBoxManagement;
        private System.Windows.Forms.MenuItem miConfigureMsgBoxes;
        private System.Windows.Forms.MenuItem miDockingManagement;

        private System.ComponentModel.IContainer components;
        
        public MainForm()
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);

            InitializeComponent();

            m_kEditView = new EditViewForm();
            m_kOutputView = new OutputViewForm();
            m_kProjectView = new ProjectViewForm();
            m_kRenderView = new RenderForm();
            m_kTransitionsView = new TransitionsViewForm();
            if (MFramework.Instance.Physics.Present)
            {
                m_kPhysicsView = new PhysicsViewForm();
            }
            else
            {
                miPhysicsView.Checked = false;
                miPhysicsView.Enabled = false;
                tbbPhysicsView.Pushed = false;
                tbbPhysicsView.Enabled = false;
            }

            string strURL = Environment.GetEnvironmentVariable("EGB_PATH");
            if (strURL != null)
                strURL = strURL + @"\Documentation\HTML\Gamebryo.chm";
            else
                strURL = null;

            string strBaseAnimationToolHelpPath = 
                @"/Tool_Manuals/Gamebryo_Animation_Tool/";
            HelpManager.Create(strURL, strBaseAnimationToolHelpPath);

            // Set Handlers...
            m_kEditView.SetHandlers();
            m_strAutoLoadFilename = "";
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if (components != null) 
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
            this.menuApp = new System.Windows.Forms.MainMenu();
            this.miFileMenu = new System.Windows.Forms.MenuItem();
            this.miNew = new System.Windows.Forms.MenuItem();
            this.miOpen = new System.Windows.Forms.MenuItem();
            this.miFileMRU = new System.Windows.Forms.MenuItem();
            this.miSeparator1 = new System.Windows.Forms.MenuItem();
            this.miSave = new System.Windows.Forms.MenuItem();
            this.miSaveAs = new System.Windows.Forms.MenuItem();
            this.miSeparator2 = new System.Windows.Forms.MenuItem();
            this.miExit = new System.Windows.Forms.MenuItem();
            this.miEditMenu = new System.Windows.Forms.MenuItem();
            this.miDockingManagement = new System.Windows.Forms.MenuItem();
            this.miSaveDockConfig = new System.Windows.Forms.MenuItem();
            this.miResetDockConfig = new System.Windows.Forms.MenuItem();
            this.miMsgBoxManagement = new System.Windows.Forms.MenuItem();
            this.miConfigureMsgBoxes = new System.Windows.Forms.MenuItem();
            this.miResetMessageBoxes = new System.Windows.Forms.MenuItem();
            this.miSceneOptions = new System.Windows.Forms.MenuItem();
            this.miTransitionTableSettings = new System.Windows.Forms.MenuItem();
            this.miViewMenu = new System.Windows.Forms.MenuItem();
            this.miProjectView = new System.Windows.Forms.MenuItem();
            this.miEditView = new System.Windows.Forms.MenuItem();
            this.miRenderView = new System.Windows.Forms.MenuItem();
            this.miTransitionsView = new System.Windows.Forms.MenuItem();
            this.miOutputView = new System.Windows.Forms.MenuItem();
            this.miPhysicsView = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.miInteractivePreview = new System.Windows.Forms.MenuItem();
            this.miHelpMenu = new System.Windows.Forms.MenuItem();
            this.miHelpContents = new System.Windows.Forms.MenuItem();
            this.miHelpIndex = new System.Windows.Forms.MenuItem();
            this.miHelpSearch = new System.Windows.Forms.MenuItem();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.miAbout = new System.Windows.Forms.MenuItem();
            this.tbApp = new System.Windows.Forms.ToolBar();
            this.tbbNew = new System.Windows.Forms.ToolBarButton();
            this.tbbOpen = new System.Windows.Forms.ToolBarButton();
            this.tbbSave = new System.Windows.Forms.ToolBarButton();
            this.tbbSeparator1 = new System.Windows.Forms.ToolBarButton();
            this.tbbAddSequences = new System.Windows.Forms.ToolBarButton();
            this.tbbInteractivePreview = new System.Windows.Forms.ToolBarButton();
            this.tbbSeparator2 = new System.Windows.Forms.ToolBarButton();
            this.tbbProjectView = new System.Windows.Forms.ToolBarButton();
            this.tbbEditView = new System.Windows.Forms.ToolBarButton();
            this.tbbRenderView = new System.Windows.Forms.ToolBarButton();
            this.tbbTransitionsView = new System.Windows.Forms.ToolBarButton();
            this.tbbOutputView = new System.Windows.Forms.ToolBarButton();
            this.tbbPhysicsView = new System.Windows.Forms.ToolBarButton();
            this.ilApp = new System.Windows.Forms.ImageList(this.components);
            this.dpDockManager = new WeifenLuo.WinFormsUI.DockPanel();
            this.SuspendLayout();
            // 
            // menuApp
            // 
            this.menuApp.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                    this.miFileMenu,
                                                                                    this.miEditMenu,
                                                                                    this.miViewMenu,
                                                                                    this.miHelpMenu});
            // 
            // miFileMenu
            // 
            this.miFileMenu.Index = 0;
            this.miFileMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miNew,
                                                                                       this.miOpen,
                                                                                       this.miFileMRU,
                                                                                       this.miSeparator1,
                                                                                       this.miSave,
                                                                                       this.miSaveAs,
                                                                                       this.miSeparator2,
                                                                                       this.miExit});
            this.miFileMenu.Text = "&File";
            // 
            // miNew
            // 
            this.miNew.Index = 0;
            this.miNew.Text = "&New\tCtrl+N";
            this.miNew.Click += new System.EventHandler(this.miNew_Click);
            // 
            // miOpen
            // 
            this.miOpen.Index = 1;
            this.miOpen.Text = "&Open...\tCtrl+O";
            this.miOpen.Click += new System.EventHandler(this.miOpen_Click);
            // 
            // miFileMRU
            // 
            this.miFileMRU.Index = 2;
            this.miFileMRU.Text = "&Recent Files";
            // 
            // miSeparator1
            // 
            this.miSeparator1.Index = 3;
            this.miSeparator1.Text = "-";
            // 
            // miSave
            // 
            this.miSave.Index = 4;
            this.miSave.Text = "&Save\tCtrl+S";
            this.miSave.Click += new System.EventHandler(this.miSave_Click);
            // 
            // miSaveAs
            // 
            this.miSaveAs.Index = 5;
            this.miSaveAs.Text = "Save &As...";
            this.miSaveAs.Click += new System.EventHandler(this.miSaveAs_Click);
            // 
            // miSeparator2
            // 
            this.miSeparator2.Index = 6;
            this.miSeparator2.Text = "-";
            // 
            // miExit
            // 
            this.miExit.Index = 7;
            this.miExit.Text = "E&xit";
            this.miExit.Click += new System.EventHandler(this.miExit_Click);
            // 
            // miEditMenu
            // 
            this.miEditMenu.Index = 1;
            this.miEditMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miDockingManagement,
                                                                                       this.miMsgBoxManagement,
                                                                                       this.miSceneOptions,
                                                                                       this.miTransitionTableSettings});
            this.miEditMenu.Text = "&Edit";
            // 
            // miDockingManagement
            // 
            this.miDockingManagement.Index = 0;
            this.miDockingManagement.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                                this.miSaveDockConfig,
                                                                                                this.miResetDockConfig});
            this.miDockingManagement.Text = "Docking Management";
            // 
            // miSaveDockConfig
            // 
            this.miSaveDockConfig.Index = 0;
            this.miSaveDockConfig.Text = "Save &Dock Configuration";
            this.miSaveDockConfig.Click += new System.EventHandler(this.miSaveDockConfig_Click);
            // 
            // miResetDockConfig
            // 
            this.miResetDockConfig.Index = 1;
            this.miResetDockConfig.Text = "Reset Do&ck Configuration";
            this.miResetDockConfig.Click += new System.EventHandler(this.miResetDockConfig_Click);
            // 
            // miMsgBoxManagement
            // 
            this.miMsgBoxManagement.Index = 1;
            this.miMsgBoxManagement.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                               this.miConfigureMsgBoxes,
                                                                                               this.miResetMessageBoxes});
            this.miMsgBoxManagement.Text = "Message Box Management";
            // 
            // miConfigureMsgBoxes
            // 
            this.miConfigureMsgBoxes.Index = 0;
            this.miConfigureMsgBoxes.Text = "Manage Message Boxes";
            this.miConfigureMsgBoxes.Click += new System.EventHandler(this.miConfigureMsgBoxes_Click);
            // 
            // miResetMessageBoxes
            // 
            this.miResetMessageBoxes.Index = 1;
            this.miResetMessageBoxes.Text = "&Reset Message Boxes";
            this.miResetMessageBoxes.Click += new System.EventHandler(this.miResetMessageBoxes_Click);
            // 
            // miSceneOptions
            // 
            this.miSceneOptions.Index = 2;
            this.miSceneOptions.Text = "Scene &Options...";
            this.miSceneOptions.Click += new System.EventHandler(this.miSceneOptions_Click);
            // 
            // miTransitionTableSettings
            // 
            this.miTransitionTableSettings.Index = 3;
            this.miTransitionTableSettings.Text = "&Transition Table Settings...";
            this.miTransitionTableSettings.Click += new System.EventHandler(this.miTransitionTableSettings_Click);
            // 
            // miViewMenu
            // 
            this.miViewMenu.Index = 2;
            this.miViewMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miProjectView,
                                                                                       this.miEditView,
                                                                                       this.miRenderView,
                                                                                       this.miTransitionsView,
                                                                                       this.miOutputView,
                                                                                       this.miPhysicsView,
                                                                                       this.menuItem1,
                                                                                       this.miInteractivePreview});
            this.miViewMenu.Text = "&View";
            // 
            // miProjectView
            // 
            this.miProjectView.Checked = true;
            this.miProjectView.Index = 0;
            this.miProjectView.Text = "&Project View\tAlt+1";
            this.miProjectView.Click += new System.EventHandler(this.miProjectView_Click);
            // 
            // miEditView
            // 
            this.miEditView.Checked = true;
            this.miEditView.Index = 1;
            this.miEditView.Text = "&Edit View\tAlt+2";
            this.miEditView.Click += new System.EventHandler(this.miEditView_Click);
            // 
            // miRenderView
            // 
            this.miRenderView.Checked = true;
            this.miRenderView.Index = 2;
            this.miRenderView.Text = "&Render View\tAlt+3";
            this.miRenderView.Click += new System.EventHandler(this.miRenderView_Click);
            // 
            // miTransitionsView
            // 
            this.miTransitionsView.Checked = true;
            this.miTransitionsView.Index = 3;
            this.miTransitionsView.Text = "&Transitions View\tAlt+4";
            this.miTransitionsView.Click += new System.EventHandler(this.miTransitionsView_Click);
            // 
            // miOutputView
            // 
            this.miOutputView.Checked = true;
            this.miOutputView.Index = 4;
            this.miOutputView.Text = "&Output View\tAlt+5";
            this.miOutputView.Click += new System.EventHandler(this.miOutputView_Click);
            // 
            // miPhysicsView
            // 
            this.miPhysicsView.Checked = true;
            this.miPhysicsView.Index = 5;
            this.miPhysicsView.Text = "Ph&ysics View\tAlt+6";
            this.miPhysicsView.Click += new System.EventHandler(this.miPhysicsView_Click);
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 6;
            this.menuItem1.Text = "-";
            // 
            // miInteractivePreview
            // 
            this.miInteractivePreview.Index = 7;
            this.miInteractivePreview.Text = "Interactive Preview";
            this.miInteractivePreview.Click += new System.EventHandler(this.miInteractivePreview_Click);
            // 
            // miHelpMenu
            // 
            this.miHelpMenu.Index = 3;
            this.miHelpMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miHelpContents,
                                                                                       this.miHelpIndex,
                                                                                       this.miHelpSearch,
                                                                                       this.menuItem2,
                                                                                       this.miAbout});
            this.miHelpMenu.Text = "&Help";
            // 
            // miHelpContents
            // 
            this.miHelpContents.Index = 0;
            this.miHelpContents.Text = "Contents";
            this.miHelpContents.Click += new System.EventHandler(this.miHelpContents_Click);
            // 
            // miHelpIndex
            // 
            this.miHelpIndex.Index = 1;
            this.miHelpIndex.Text = "Index";
            this.miHelpIndex.Click += new System.EventHandler(this.miHelpIndex_Click);
            // 
            // miHelpSearch
            // 
            this.miHelpSearch.Index = 2;
            this.miHelpSearch.Text = "Search";
            this.miHelpSearch.Click += new System.EventHandler(this.miHelpSearch_Click);
            // 
            // menuItem2
            // 
            this.menuItem2.Index = 3;
            this.menuItem2.Text = "-";
            // 
            // miAbout
            // 
            this.miAbout.Index = 4;
            this.miAbout.Text = "&About Animation Tool...";
            this.miAbout.Click += new System.EventHandler(this.miAbout_Click);
            // 
            // tbApp
            // 
            this.tbApp.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
                                                                                     this.tbbNew,
                                                                                     this.tbbOpen,
                                                                                     this.tbbSave,
                                                                                     this.tbbSeparator1,
                                                                                     this.tbbAddSequences,
                                                                                     this.tbbInteractivePreview,
                                                                                     this.tbbSeparator2,
                                                                                     this.tbbProjectView,
                                                                                     this.tbbEditView,
                                                                                     this.tbbRenderView,
                                                                                     this.tbbTransitionsView,
                                                                                     this.tbbOutputView,
                                                                                     this.tbbPhysicsView});
            this.tbApp.ButtonSize = new System.Drawing.Size(24, 24);
            this.tbApp.DropDownArrows = true;
            this.tbApp.ImageList = this.ilApp;
            this.tbApp.Location = new System.Drawing.Point(0, 0);
            this.tbApp.Name = "tbApp";
            this.tbApp.ShowToolTips = true;
            this.tbApp.Size = new System.Drawing.Size(1272, 36);
            this.tbApp.TabIndex = 0;
            this.tbApp.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.tbApp_ButtonClick);
            // 
            // tbbNew
            // 
            this.tbbNew.ImageIndex = 0;
            this.tbbNew.ToolTipText = "Create a new Animation Tool project.";
            // 
            // tbbOpen
            // 
            this.tbbOpen.ImageIndex = 1;
            this.tbbOpen.ToolTipText = "Open an existing Animation Tool project.";
            // 
            // tbbSave
            // 
            this.tbbSave.ImageIndex = 2;
            this.tbbSave.ToolTipText = "Save the current Animation Tool project.";
            // 
            // tbbSeparator1
            // 
            this.tbbSeparator1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbbAddSequences
            // 
            this.tbbAddSequences.ImageIndex = 3;
            this.tbbAddSequences.ToolTipText = "Add one or more sequences to the current Animation Tool project.";
            // 
            // tbbInteractivePreview
            // 
            this.tbbInteractivePreview.ImageIndex = 9;
            this.tbbInteractivePreview.ToolTipText = "Activate Interactive Preview Mode";
            // 
            // tbbSeparator2
            // 
            this.tbbSeparator2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbbProjectView
            // 
            this.tbbProjectView.ImageIndex = 4;
            this.tbbProjectView.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbProjectView.ToolTipText = "Toggle the visibility of the Project View.";
            // 
            // tbbEditView
            // 
            this.tbbEditView.ImageIndex = 5;
            this.tbbEditView.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbEditView.ToolTipText = "Toggle the visibilty of the Edit View.";
            // 
            // tbbRenderView
            // 
            this.tbbRenderView.ImageIndex = 6;
            this.tbbRenderView.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbRenderView.ToolTipText = "Toggle the visibility of the Render View.";
            // 
            // tbbTransitionsView
            // 
            this.tbbTransitionsView.ImageIndex = 7;
            this.tbbTransitionsView.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbTransitionsView.ToolTipText = "Toggle the visibility of the Transitions View.";
            // 
            // tbbOutputView
            // 
            this.tbbOutputView.ImageIndex = 8;
            this.tbbOutputView.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbOutputView.ToolTipText = "Toggle the visibility of the Output View.";
            // 
            // tbbPhysicsView
            // 
            this.tbbPhysicsView.ImageIndex = 10;
            this.tbbPhysicsView.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbPhysicsView.ToolTipText = "Toggle the visibility of the Physics View.";
            // 
            // ilApp
            // 
            this.ilApp.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
            this.ilApp.ImageSize = new System.Drawing.Size(24, 24);
            this.ilApp.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilApp.ImageStream")));
            this.ilApp.TransparentColor = System.Drawing.Color.Fuchsia;
            // 
            // dpDockManager
            // 
            this.dpDockManager.ActiveAutoHideContent = null;
            this.dpDockManager.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dpDockManager.Font = new System.Drawing.Font("Microsoft Sans Serif", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            this.dpDockManager.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.dpDockManager.Location = new System.Drawing.Point(0, 36);
            this.dpDockManager.Name = "dpDockManager";
            this.dpDockManager.Size = new System.Drawing.Size(1272, 939);
            this.dpDockManager.TabIndex = 0;
            this.dpDockManager.Paint += new System.Windows.Forms.PaintEventHandler(this.dpDockManager_OnPaint);
            // 
            // MainForm
            // 
            this.AllowDrop = true;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(1272, 975);
            this.Controls.Add(this.dpDockManager);
            this.Controls.Add(this.tbApp);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.KeyPreview = true;
            this.Menu = this.menuApp;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Gamebryo Animation Tool";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.MainForm_Closing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.MainForm_KeyUp);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.MainForm_DragDrop);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.MainForm_OnPaint);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.MainForm_DragEnter);
            this.ResumeLayout(false);

        }
        #endregion

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args) 
        {
            MFramework.Init(Application.StartupPath + "//");
            Invalidator.Init();

            MainForm form = new MainForm();

            // Process command-line arguments.
            bool bFoundAutoLoadFile = false;
            foreach (string strArg in args)
            {
                string strUpperArg = strArg.ToUpper();
                if (strUpperArg.IndexOf("-D3D10") > -1)
                {
                    MFramework.Instance.Renderer.UseD3D10();
                }
                else if (!bFoundAutoLoadFile && !strArg.StartsWith("-"))
                {
                    form.m_strAutoLoadFilename = strArg;
                    bFoundAutoLoadFile = true;
                }
            }

            Application.Idle += new EventHandler(form.Application_Idle);
            Application.Run(form);
        
            Invalidator.Shutdown();
            MFramework.Shutdown();           
        }
        
        private void Application_Idle(object sender, EventArgs e)
        {
            if (this.Visible &&
                this.WindowState !=  FormWindowState.Minimized &&
                Form.ActiveForm == this)
            {
                Invalidator.Instance.Update(true);
            }
        }

        private void MainForm_Load(object sender, System.EventArgs e)
        {
            string strCompanyName = Application.CompanyName;
            string strProductName = Application.ProductName;
            m_strRegistryPath = "Software\\" + strCompanyName + "\\" +
                strProductName ;

            ProjectData.RegistryPath = m_strRegistryPath;

            // Restore window location and state.
            RegistryKey key = Registry.CurrentUser.OpenSubKey(
                m_strRegistryPath + ms_strWindowSubPath);
            if (key != null)
            {
                int iX = (int) key.GetValue("Location.X", int.MinValue);
                int iY = (int) key.GetValue("Location.Y", int.MinValue);
                if (iX != int.MinValue && iY != int.MinValue)
                {
                    this.Location = new Point(iX, iY);
                }

                int iWidth = (int) key.GetValue("Size.Width", int.MinValue);
                int iHeight = (int) key.GetValue("Size.Height", int.MinValue);
                if (iWidth != int.MinValue && iHeight != int.MinValue)
                {
                    this.Size = new Size(iWidth, iHeight);
                }

                string strWindowState = (string) key.GetValue("WindowState",
                    "None");
                if (strWindowState != "None")
                {
                    FormWindowState ws = new FormWindowState();
                    foreach (FieldInfo fi in ws.GetType().GetFields())
                    {
                        if (fi.Name == strWindowState)
                        {
                            ws = (FormWindowState) fi.GetValue(null);
                            break;
                        }
                    }
                    this.WindowState = ws;
                }
            }

            MessageBoxManager.Create(m_strRegistryPath + "\\MessageBoxes\\");
            MessageBoxManager.AddAssemblyResources(
                Assembly.GetExecutingAssembly(),
                "AnimationToolUI.Resources.MessageBoxes.");

            // Associate .KFM file with AnimationTool in Windows shell
            // If user started AnimationTool with D3D10 option, include
            // that option in the shell association.
            string strOptions = MFramework.Instance.Renderer.D3D10 ?
                "-D3D10" : "";            

            bool bSetRegKey = AssociateFileTypes.Associate(".kfm", 
                Application.ExecutablePath, strOptions, "KFM.Document",
                "Gamebryo KFM File", Application.StartupPath +
                @"\..\..\AnimationToolDoc.ico");
         
            // Register a call back for whenever the File menu is
            // opened so that we know to populate or most recent files

            MRUManager.SetMRURegPath(m_strRegistryPath + "\\MRU");
            ((MenuItem)miFileMRU.Parent).Popup += 
                new EventHandler(this.OnFileMenuOpened);

            LoadDockConfig();

            ResetViewUIStates(null, null);
            m_kRenderView.DockStateChanged += new EventHandler(
                ResetViewUIStates);
            m_kOutputView.DockStateChanged += new EventHandler(
                ResetViewUIStates);
            m_kProjectView.DockStateChanged += new EventHandler(
                ResetViewUIStates);
            m_kEditView.DockStateChanged += new EventHandler(
                ResetViewUIStates);
            m_kTransitionsView.DockStateChanged += new EventHandler(
                ResetViewUIStates);
            if (m_kPhysicsView != null)
            {
                m_kPhysicsView.DockStateChanged += new EventHandler(
                    ResetViewUIStates);
            }

            m_strBaseAppTitle = Text;
            MFramework.Instance.Animation.OnKFMLoaded += 
                new NiManagedToolInterface.MAnimation.__Delegate_OnKFMLoaded(
                Animation_OnKFMLoaded);
            MFramework.Instance.Animation.OnNewKFM += 
                new NiManagedToolInterface.MAnimation.__Delegate_OnNewKFM(
                Animation_OnNewKFM);
            MFramework.Instance.Animation.OnKFMSaved += 
                new NiManagedToolInterface.MAnimation.__Delegate_OnKFMSaved(
                Animation_OnKFMSaved);
            MFramework.Instance.Animation.OnLongOperationStarted +=
                new NiManagedToolInterface.MAnimation.
                __Delegate_OnLongOperationStarted(
                Animation_OnLongOperationStarted);
            MFramework.Instance.Animation.OnLongOperationCompleted +=
                new NiManagedToolInterface.MAnimation.
                __Delegate_OnLongOperationCompleted(
                Animation_OnLongOperationCompleted);

            MFramework.Instance.Animation.OnModelNIFLoadFailed += 
                new NiManagedToolInterface.
                MAnimation.__Delegate_OnModelNIFLoadFailed(
                Animation_OnModelNIFLoadFailed);
            MFramework.Instance.Animation.OnKFLoadFailed += 
                new NiManagedToolInterface.
                MAnimation.__Delegate_OnKFLoadFailed(
                Animation_OnKFLoadFailed);

            MFramework.Instance.Renderer.OnRendererCreatedEvent += 
                new RendererCreatedEvent(Renderer_OnRendererCreatedEvent);

            ProjectData.Create();
            ProjectData.Instance.LoadRegistrySettings();

            MFramework.Instance.Animation.NewKFM();
        
            //if (m_strAutoLoadFilename.Length != 0)
            //{
            //    AnimationHelpers.OpenKFMFile(m_strAutoLoadFilename);
            //}
            

            if (!HelpManager.HelpExists())
            {
                miHelpContents.Enabled = false;
                miHelpIndex.Enabled = false;
                miHelpSearch.Enabled = false;
            }
        }

        void Renderer_OnRendererCreatedEvent()
        {
            MFramework.Instance.Input.Init();
            ProjectData.Create();
            ProjectData.Instance.LoadRegistrySettings();
            ProjectData.InitBackground();

            if (m_strAutoLoadFilename.Length != 0)
            {
                AnimationHelpers.OpenKFMFile(m_strAutoLoadFilename);
                m_strAutoLoadFilename = String.Empty;
            }
        }

        private void ResetDefaults()
        {
            MFramework.Instance.Animation.DefaultSyncTransDuration = 0.25f;
            MFramework.Instance.Animation.DefaultNonSyncTransDuration = 0.25f;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.RESET_BACKGROUND_HEIGHT);
            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.ZOOM_EXTENTS);

        }

        private void MainForm_OnPaint(object sender,
            System.Windows.Forms.PaintEventArgs e)
        {
        }

        private void dpDockManager_OnPaint(object sender,
            System.Windows.Forms.PaintEventArgs e)
        {
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
        }

        private DockContent FindContent(string text)
        {
            DockContentCollection contents = dpDockManager.Contents;
            foreach (DockContent d in contents)
                if (d.Text == text)
                    return d;

            return null;
        }

        public void OpenMRUFile(string fileName)
        {
            OpenFile(fileName);
        }

        private IDockContent GetContentFromPersistString(string persistString)
        {
            //return FindContent(persistString);

            if (persistString == typeof(EditViewForm).ToString())
                return m_kEditView;
            else if (persistString == typeof(OutputViewForm).ToString())
                return m_kOutputView;
            else if (persistString == typeof(ProjectViewForm).ToString())
                return m_kProjectView;
            else if (persistString == typeof(RenderForm).ToString())
                return m_kRenderView;
            else if (persistString == typeof(TransitionsViewForm).ToString())
                return m_kTransitionsView;
            else if (persistString == typeof(PhysicsViewForm).ToString())
                return m_kPhysicsView;
            return null;
        }

        private void MainForm_Closing(object sender,
            System.ComponentModel.CancelEventArgs e)
        {
            if (AnimationHelpers.UserSaveOrCancel() == false)
            {
                e.Cancel = true;
                return;
            }

            if (this.WindowState != FormWindowState.Minimized)
            {
                RegistryKey key = Registry.CurrentUser.CreateSubKey(
                    m_strRegistryPath + ms_strWindowSubPath);

                if (this.WindowState != FormWindowState.Maximized)
                {
                    key.SetValue("Location.X", this.Location.X);
                    key.SetValue("Location.Y", this.Location.Y);
                    key.SetValue("Size.Width", this.Size.Width);
                    key.SetValue("Size.Height", this.Size.Height);
                }
                key.SetValue("WindowState", this.WindowState);
            }
        }

        private void Animation_OnKFMLoaded()
        {
            ResetAppTitleBar();
        }

        private void Animation_OnNewKFM()
        {
            ResetDefaults();
            ResetAppTitleBar();
        }

        private void Animation_OnKFMSaved()
        {
            ResetAppTitleBar();
        }

        private void Animation_OnLongOperationStarted()
        {
            this.Cursor = Cursors.WaitCursor;
        }

        private void Animation_OnLongOperationCompleted()
        {
            this.Cursor = Cursors.Default;
        }

        private void Animation_OnModelNIFLoadFailed(string strKFMFilename, 
            ModelNIFFailedLoadArgs Args)
        {
            string[] astrVariables = new string[1];
            astrVariables[0] = MessageBoxManager.ConvertFilename(
                Args.strNIFFilename);
            MessageBoxManager.DoMessageBox("CannotFindResourceError.rtf",
                "Cannot Find Project Resource File", MessageBoxButtons.OK,
                MessageBoxIcon.Error, astrVariables);

            bool bResult = false;
            bool bCancel = false;
            while (bResult == false)
            {
                bResult = AnimationHelpers.GetModelAndRootNames(
                    this, ref Args.strNIFFilename, ref Args.strModelRootName,
                    ref bCancel);

                if (bCancel)
                {
                    Args.bCancel = true;
                    return;
                }
            }

            Args.bCancel = false;
        }

        private void Animation_OnKFLoadFailed(KFFFailedLoadArgs pkArgs)
        {
            bool bFirst = true;
            foreach (MSequence.MInvalidSequenceInfo kInvalidInfo 
                in pkArgs.aMissingSequences)
            {
                string resourceName;
                if (kInvalidInfo.SequenceName == null)
                {
                    resourceName = "AnimIndex = " +
                        kInvalidInfo.AnimIndex;
                }
                else
                {
                    resourceName = "SequenceName = " +
                        kInvalidInfo.SequenceName;
                }

                string[] astrVariables = new string[2];
                astrVariables[0] = resourceName;
                astrVariables[1] = 
                    MessageBoxManager.ConvertFilename(kInvalidInfo.Filename);
                MessageBoxManager.DoMessageBox(
                    "CannotFindKFResourceError.rtf",
                    "Cannot Find Project Resource File", MessageBoxButtons.OK,
                    MessageBoxIcon.Error, astrVariables);

                string strFilename = kInvalidInfo.Filename;
                bool bCancel = false;
                while(true)
                {
                    string missingName;
                    if (kInvalidInfo.SequenceName == null)
                    {
                        missingName = "at old index " + 
                            kInvalidInfo.AnimIndex + " in file " + 
                            kInvalidInfo.Filename;
                    }
                    else
                    {
                        missingName = "named " + kInvalidInfo.SequenceName + 
                            " from file " + kInvalidInfo.Filename;
                    }

                    if (AnimationHelpers.DoMissingKFDialog(
                        this, bFirst, ref strFilename, ref bCancel))
                    {
                        string strSequenceName = kInvalidInfo.SequenceName;
                        bool bCancelSequence = false;
                        if (AnimationHelpers.DoMissingSequenceDialog(this,
                            missingName, strFilename, ref strSequenceName, 
                            ref bCancelSequence))
                        {
                            // success
                            kInvalidInfo.Filename = strFilename;
                            kInvalidInfo.SequenceName = strSequenceName;
                            kInvalidInfo.AnimIndex = -1;
                            break;
                        }
                    }
                    else if (bCancel)
                    {
                        pkArgs.bCancel = true;
                        return;
                    }

                    bFirst = false;
                }
            }
        }

        public void ResetAppTitleBar()
        {
            string strFilename = MFramework.Instance.Animation.KFMFilename;
            if (strFilename != null && strFilename != "")
            {
                strFilename = Path.GetFileName(strFilename);
                Text = strFilename + " - " + m_strBaseAppTitle;
            }
            else
            {
                Text = m_strBaseAppTitle;
            }
        }

        #region Command Functions
        private void NewFile()
        {
            AnimationHelpers.NewKFMFile();
        }

        private void OpenFile()
        {
            AnimationHelpers.OpenKFMDlg(this);
        }

        private void OpenFile(string strFilename)
        {
            AnimationHelpers.OpenKFMFile(strFilename);
        }

        private void SaveFile(bool bSaveAs)
        {
            AnimationHelpers.SaveKFMDlg(this, bSaveAs);
        }

        private string GetDockConfigFilename()
        {
            string strDataPath = Application.UserAppDataPath.Replace(
                Application.ProductVersion, string.Empty);
            return Path.Combine(strDataPath, "DockManager.config");
        }

        private void SaveDockConfig()
        {
            dpDockManager.SaveAsXml(GetDockConfigFilename());
        }

        private void LoadDockConfig()
        {
            string strDataPath = Application.UserAppDataPath.Replace(
                Application.ProductVersion, string.Empty);
            string configFile = Path.Combine(strDataPath,
                "DockManager.config");
            
            m_kProjectView.Text = "Project View";
            m_kEditView.Text = "Edit View";
            m_kRenderView.Text = "Render View";
            m_kOutputView.Text = "Output View";
            m_kTransitionsView.Text = "Transitions View";
            if (m_kPhysicsView != null)
            {
                m_kPhysicsView.Text = "Physics View";
            }

            bool bFileLoaded = false;

            if (File.Exists(configFile))
            {
                string strMsg = "Loading ConfigFile: " + configFile;
                MLogger.LogGeneral(strMsg);
                try 
                {
                    dpDockManager.LoadFromXml(configFile,
                        new DeserializeDockContent(
                        GetContentFromPersistString));
                    // It is necessary to refresh the data the first time 
                    // so that the channel colors are correct.
                    m_kOutputView.RefreshData();
                    bFileLoaded = true;
                }
                catch (ArgumentException)
                {
                    MLogger.LogWarning("The dock configuration could not be: " 
                        + "loaded" + ". Loading default dock configuration.");
                }
            
            }
            else
            {
                MLogger.LogWarning("No dock configuration file at: " +
                    configFile + ". Loading default dock configuration.");

            }

            if (!bFileLoaded)
            {
                Stream stream = Assembly.GetExecutingAssembly()
                    .GetManifestResourceStream("AnimationToolUI.Resources" +
                    ".DefaultDockManager.config");
                try
                {
                    dpDockManager.LoadFromXml(stream,
                        new DeserializeDockContent(
                        GetContentFromPersistString));
                }
                catch (ArgumentException)
                {
                    MessageBox.Show(this, 
                        "The dock configuration could not be loaded.",
                        "Dock Configuration Error", MessageBoxButtons.OK,
                        MessageBoxIcon.Information);                    
                }
            }
            
            dpDockManager.Refresh(); 
        }

        private void ResetDockConfig()
        {
            FileInfo fi = new FileInfo(GetDockConfigFilename());
            fi.Delete();
            MessageBox.Show(this, "The default dock configuration will be " +
                "used the next time you run the application.",
                "Dock Configuration Reset", MessageBoxButtons.OK,
                MessageBoxIcon.Information);
        }

        private void DisplayAboutBox()
        {
            AboutBoxForm dlgAboutBox = new AboutBoxForm();
            dlgAboutBox.ShowDialog(this);
        }

        private void DisplayTransitionSettingsDialog()
        {
            TransitionTableSettingsForm dlg =
                new TransitionTableSettingsForm();
            dlg.ShowDialog(this);
            if (m_kTransitionsView != null)
                m_kTransitionsView.Invalidate(true);
        }

        private void ResetMessageBoxes()
        {
            MessageBoxManager.ResetAllEntries();
            MessageBox.Show(this, "Saved settings for all message boxes have "
                + "been reset. All\ntypes of message boxes will be " +
                "displayed when appropriate.", "Message Boxes Reset",
                MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void AddSequences()
        {
            AnimationHelpers.OpenMultipleKFsDialog(this);
        }

        private void DisplaySceneOptionsDialog()
        {
            SceneOptionsForm dlg = new SceneOptionsForm();
            dlg.ShowDialog(this);
        }
        #endregion

        #region View UI Functions
        private void UpdateViewUIState(DockContent theView, 
            System.Windows.Forms.MenuItem theMenuItem,
            System.Windows.Forms.ToolBarButton theToolBarButton)
        {
            if (theView == null)
            {
                return;
            }

            bool bVisible =(theView.DockState != DockState.Unknown) && 
                !theView.IsHidden;
            theMenuItem.Checked = bVisible;
            theToolBarButton.Pushed = bVisible;
        }

        private void ResetViewUIStates(object sender, EventArgs e)
        {
            UpdateViewUIState(m_kRenderView, miRenderView, 
                tbbRenderView);

            UpdateViewUIState(m_kProjectView, miProjectView, 
                tbbProjectView);

            UpdateViewUIState(m_kOutputView, miOutputView, 
                tbbOutputView);

            UpdateViewUIState(m_kEditView, miEditView, 
                tbbEditView);

            UpdateViewUIState(m_kTransitionsView,
                miTransitionsView,
                tbbTransitionsView);

            UpdateViewUIState(m_kPhysicsView,
                miPhysicsView,
                tbbPhysicsView);
        }

        private void ToggleViewVisibleState(DockContent theView)
        {
            if (theView == null)
            {
                return;
            }

            bool bVisible = (theView.DockState != DockState.Unknown) 
                && !theView.IsHidden;
            SetViewVisibleState(theView, !bVisible);    
        }

        private void SetViewVisibleState(DockContent TheView, bool bVisible)
        {
            if (TheView == null)
            {
                return;
            }

            if (bVisible)
            {
                // Would like to just set IsHidden = false, but doing that
                // can cause the app to crash if ProjectView.Show is not
                // called. So we save the active view and reshow it as a way
                // around the issue.
                // I was going to use ActiveView = 
                //      dpDockManager.ActiveDocument;
                // but this is unrelyable when doc tabs are at bottom...
                // so we search and find which are not hidden and it.
                DockContent ActiveView = TheView;
                DockContentCollection contents = dpDockManager.Contents;
                foreach (DockContent d in contents)
                    if (d.IsHidden == false)
                        ActiveView = d;

                TheView.Show(dpDockManager);
                ActiveView.Show(dpDockManager);
            }
            else
            {
                try { TheView.IsHidden = true; }
                catch { }
            }

            this.Invalidate(true);
        }
        #endregion

        #region Toolbar Handlers
        private void tbApp_ButtonClick(object sender,
            System.Windows.Forms.ToolBarButtonClickEventArgs e)
        {
            if (e.Button == tbbNew)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_NEW);
                MLogger.LogGeneral(strLog);

                NewFile();
            }
            else if (e.Button == tbbOpen)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_OPEN);
                MLogger.LogGeneral(strLog);

                OpenFile();
            }
            else if (e.Button == tbbSave)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_SAVE);
                MLogger.LogGeneral(strLog);

                SaveFile(false);
            }
            else if (e.Button == tbbAddSequences)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_ADDSEQ);
                MLogger.LogGeneral(strLog);

                AddSequences();
            }
            else if (e.Button == tbbProjectView)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_PROJECT_VIEW);
                MLogger.LogGeneral(strLog);

                ToggleViewVisibleState(m_kProjectView);
            }
            else if (e.Button == tbbRenderView)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_RENDER_VIEW);
                MLogger.LogGeneral(strLog);

                ToggleViewVisibleState(m_kRenderView);
            }
            else if (e.Button == tbbOutputView)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_OUTPUT_VIEW);
                MLogger.LogGeneral(strLog);

                ToggleViewVisibleState(m_kOutputView);
            }
            else if (e.Button == tbbEditView)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_EDIT_VIEW);
                MLogger.LogGeneral(strLog);

                ToggleViewVisibleState(m_kEditView);
            }
            else if (e.Button == tbbTransitionsView)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_TRANSITIONS_VIEW);
                MLogger.LogGeneral(strLog);

                ToggleViewVisibleState(m_kTransitionsView);
            }
            else if (e.Button == tbbPhysicsView)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_PHYSICS_VIEW);
                MLogger.LogGeneral(strLog);
                
                ToggleViewVisibleState(m_kPhysicsView);
            }
            else if (e.Button == tbbInteractivePreview)
            {
                string strLog = UserLogs.GetLogString(
                    UserLogs.LogType.ICON_INTERACTIVE);
                MLogger.LogGeneral(strLog);

                ProjectTreeNode node = 
                    ProjectData.Instance.GetInteractivePreviewNode();
                AppEvents.RaiseProjectViewSelectionChanged(node);
            }
        }
        #endregion

        #region Menu Handlers
        private void miEditView_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_EDIT_VIEW);
            MLogger.LogGeneral(strLog);

            ToggleViewVisibleState(m_kEditView);
        }

        private void miRenderView_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_RENDER_VIEW);
            MLogger.LogGeneral(strLog);

            ToggleViewVisibleState(m_kRenderView);
        }

        private void miTransitionsView_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_TRANSITIONS_VIEW);
            MLogger.LogGeneral(strLog);

            ToggleViewVisibleState(m_kTransitionsView);
        }

        private void miOutputView_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_OUTPUT_VIEW);
            MLogger.LogGeneral(strLog);

            ToggleViewVisibleState(m_kOutputView);
        }

        private void miPhysicsView_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_PHYSICS_VIEW);
            MLogger.LogGeneral(strLog);

            ToggleViewVisibleState(m_kPhysicsView);
        }

        private void miProjectView_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_PROJECT_VIEW);
            MLogger.LogGeneral(strLog);

            ToggleViewVisibleState(m_kProjectView);
        }

        private void miNew_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.FILE_NEW);
            MLogger.LogGeneral(strLog);

            NewFile();
        }

        private void miOpen_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(UserLogs.LogType.FILE_OPEN);
            MLogger.LogGeneral(strLog);

            OpenFile();
        }

        private void miSaveDockConfig_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.EDIT_SAVE_DOCK);
            MLogger.LogGeneral(strLog);

            SaveDockConfig();
        }

        private void miResetDockConfig_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.EDIT_RESET_DOCK);
            MLogger.LogGeneral(strLog);

            ResetDockConfig();
        }

        private void miExit_Click(object sender, System.EventArgs e)
        {
            Close();
        }

        private void miAbout_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.HELP_ABOUT);
            MLogger.LogGeneral(strLog);

            DisplayAboutBox();
        }

        private void miSave_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.FILE_SAVE);
            MLogger.LogGeneral(strLog);

            SaveFile(false);
        }

        private void miSaveAs_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.FILE_SAVEAS);
            MLogger.LogGeneral(strLog);

            SaveFile(true);
        }

        private void miTransitionTableSettings_Click(object sender,
            System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.EDIT_TRANSITION_TABLE_SETTINGS);
            MLogger.LogGeneral(strLog);

            DisplayTransitionSettingsDialog();
        }

        private void miResetMessageBoxes_Click(
            object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.EDIT_RESET_MESSAGE_BOXES);
            MLogger.LogGeneral(strLog);

            ResetMessageBoxes();
        }

        private void miSceneOptions_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.EDIT_SCENE_OPTIONS);
            MLogger.LogGeneral(strLog);

            DisplaySceneOptionsDialog();
        }

        private void miInteractivePreview_Click(
            object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.VIEW_INTERACTIVE);
            MLogger.LogGeneral(strLog);

            ProjectTreeNode node = 
                ProjectData.Instance.GetInteractivePreviewNode();
            AppEvents.RaiseProjectViewSelectionChanged(node);
        }

        private void miHelpContents_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.HELP_CONTENTS);
            MLogger.LogGeneral(strLog);

            HelpManager.DoHelpAndTOC(this, 
                @"Basics/Introduction_to_Animation_Tool.htm");
        }

        
        private void miHelpIndex_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(UserLogs.LogType.HELP_INDEX);
            MLogger.LogGeneral(strLog);

            HelpManager.DoHelpAndIndex(this, 
                @"Basics/Introduction_to_Animation_Tool.htm");
        }

        private void miHelpSearch_Click(object sender, System.EventArgs e)
        {
            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.HELP_SEARCH);
            MLogger.LogGeneral(strLog);

            HelpManager.DoHelpAndSearch(this, 
                @"Basics/Introduction_to_Animation_Tool.htm");
        }

        #endregion

        #region Keyboard Handlers
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            Debug.Assert(msg.Msg == 0x0100 || msg.Msg == 0x0104,
                "ProcessCmdKey message is not WM_KEYDOWN or WM_SYSKEYDOWN.");
            if (InputManager.Instance.OnKeyDown(keyData))
            {
                return true;
            }

            Keys keyCode = keyData & Keys.KeyCode;
            bool bCtrl = (keyData & Keys.Control) != 0;
            bool bAlt = (keyData & Keys.Alt) != 0;
            bool bShift = (keyData & Keys.Shift) != 0;

            if (bCtrl && !bAlt && !bShift)
            {
                switch (keyCode)
                {
                    case Keys.N:
                        NewFile();
                        return true;
                    case Keys.O:
                        OpenFile();
                        return true;
                    case Keys.S:
                        SaveFile(false);
                        return true;
                }
            }
            else if (bAlt && !bCtrl && !bShift)
            {
                switch (keyCode)
                {
                    case Keys.D1:
                        ToggleViewVisibleState(m_kProjectView);
                        return true;
                    case Keys.D2:
                        ToggleViewVisibleState(m_kEditView);
                        return true;
                    case Keys.D3:
                        ToggleViewVisibleState(m_kRenderView);
                        return true;
                    case Keys.D4:
                        ToggleViewVisibleState(m_kTransitionsView);
                        return true;
                    case Keys.D5:
                        ToggleViewVisibleState(m_kOutputView);
                        return true;
                    case Keys.D6:
                        ToggleViewVisibleState(m_kPhysicsView);
                        return true;
                }
            }

            return base.ProcessCmdKey (ref msg, keyData);
        }

        private void MainForm_KeyUp(object sender, KeyEventArgs e)
        {
            if (InputManager.Instance.OnKeyUp(e.KeyCode))
            {
                e.Handled = true;
            }
        }
        #endregion

        #region File Drag n' Drop
        private void MainForm_DragEnter(
            object sender, System.Windows.Forms.DragEventArgs e)
        {
            if( e.Data.GetDataPresent(DataFormats.FileDrop, false) == true )
            {
                bool bContinue = true;
                int iNumKFMs = 0;
                int iNumKFs = 0;
                int iNumNIFs = 0;
                foreach (string strFilename in (string[])e.Data.GetData(
                    DataFormats.FileDrop))
                {
                    string strExt = Path.GetExtension(strFilename);
                    strExt = strExt.ToUpper();
                    if (strExt == ".KFM")
                    {
                        iNumKFMs++;
                    }
                    else if (strExt == ".KF")
                    {
                        iNumKFs++;
                    }
                    else if (strExt == ".NIF")
                    {
                        iNumNIFs++;
                    }
                    else
                    {
                        bContinue = false;
                        break;
                    }
                }

                if (iNumKFMs > 1)
                    bContinue = false;
                if (iNumNIFs > 1)
                    bContinue = false;
                if (iNumKFMs != 0 && iNumKFs > 0)
                    bContinue = false;
                if (iNumKFMs != 0 && iNumNIFs > 0)
                    bContinue = false;

                if (bContinue == true)
                    e.Effect = DragDropEffects.All;
            }
        }

        private void MainForm_DragDrop(
            object sender, System.Windows.Forms.DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            string strKFM = null;
            string strNIF = null;
            int iNumKFM = 0;
            int iNumNIF = 0;
            
            foreach (string strFilename in 
                (string[])e.Data.GetData(DataFormats.FileDrop))
            {
                string strExt = Path.GetExtension(strFilename);
                strExt = strExt.ToUpper();
                if (strExt == ".KFM")
                {
                    strKFM = strFilename;
                    iNumKFM++;
                }
                else if (strExt == ".NIF")
                {
                    strNIF = strFilename;
                    iNumNIF++;
                }
            }

            if (strKFM != null)
            {
                OpenFile(strKFM);
                return;
            }

            if (strNIF != null)
            {
                AnimationHelpers.OpenModelFile(strNIF, this);
            }

            if (files.Length - iNumKFM - iNumNIF == 0)
                return;

            string [] strKFFiles = 
                new string[files.Length  - iNumKFM - iNumNIF];
            int i = 0;
            foreach (string strFilename in 
                (string[])e.Data.GetData(DataFormats.FileDrop))
            {
                string strExt = Path.GetExtension(strFilename);
                strExt = strExt.ToUpper();
                if (strExt == ".KF")
                {
                    strKFFiles[i] = strFilename;
                    i++;
                }
            }

            if (strKFFiles.Length != 0) 
                AnimationHelpers.OpenMultipleKFFiles(strKFFiles);
        }
        #endregion

        private void miConfigureMsgBoxes_Click(
            object sender, System.EventArgs e)
        {
            MessageBoxRegistryEdit dlgEditMsgBoxes = 
                new MessageBoxRegistryEdit();
            dlgEditMsgBoxes.ShowDialog(this);       
        }

        private void OnFileMenuOpened(object sender, System.EventArgs e)
        {
            // Whenever the file menu is accessed, we get this callback.
            // We now populate the submenu with the most recently used files
            ArrayList alMRU = MRUManager.GetMRUList();

            if (alMRU == null)
                return;
            
            MRUFileSelectedCB cbMRU = new MRUFileSelectedCB(OnMRUFileSelected);
            MRUManager.PopulateRecentFiles(
                miFileMRU, // Menu index to populate with recently used files
                alMRU,      // List of MRU files
                cbMRU);     // Callback to handle MRU selection
        }

        public void OnMRUFileSelected(object sender, EventArgs e)
        {
            MenuItem miFile = (MenuItem)sender;

            if (miFile == null)
                return;

            string strFile = miFile.Text;

            string strLog = UserLogs.GetLogString(
                UserLogs.LogType.FILE_RECENT);
            MLogger.LogGeneral(strLog + "'" + strFile + "'");
                    
            OpenMRUFile(strFile);
        }
    }
}
