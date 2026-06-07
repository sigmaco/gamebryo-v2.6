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

using WeifenLuo.WinFormsUI;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for RenderForm.
    /// </summary>
    public class RenderForm : DockContent
    {
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.ContextMenu cmRenderMenu;
        private AnimationToolUI.RenderPanel ctlRenderPanel;
        private AnimationToolUI.TimeSlider ctlTimeSlider;
        private System.Windows.Forms.MenuItem miUIState;
        private System.Windows.Forms.MenuItem miOrbitMode;
        private System.Windows.Forms.MenuItem miFreeLookMode;
        private System.Windows.Forms.MenuItem miAdjustSceneHeightMode;
        private System.Windows.Forms.MenuItem miWireframe;
        private System.Windows.Forms.MenuItem miSceneLights;
        private System.Windows.Forms.MenuItem miBackgroundScene;
        private System.Windows.Forms.MenuItem miFrameRate;
        private System.Windows.Forms.MenuItem miReloadShaders;
        private System.Windows.Forms.MenuItem miZoomExtentsCamera;
        private System.Windows.Forms.MenuItem miCollisionABVs;
        private System.Windows.Forms.MenuItem miResetActorPos;
        private System.Windows.Forms.MenuItem miResetOrbitPoint;
        private System.Windows.Forms.ImageList ilNavigationImages;
        private System.Windows.Forms.MenuItem miCommands;
        private System.Windows.Forms.MenuItem miSeparator2;
        private System.Windows.Forms.MenuItem miSeparator1;
        private System.Windows.Forms.ToolBar tbNavigationToolbar;
        private System.Windows.Forms.ToolBarButton tbbOrbitButton;
        private System.Windows.Forms.ToolBarButton tbbFreeCameraButton;
        private System.Windows.Forms.ToolBarButton tbbAdjustSceneHeightButton;
        private System.Windows.Forms.ToolBarButton tbbSeperator1;
        private System.Windows.Forms.ToolBarButton tbbCameraZoomExtentsButton;
        private System.Windows.Forms.ToolBarButton tbbIncBoneLODButton;
        private System.Windows.Forms.ToolBarButton tbbDecBoneLODButton;
        private System.Windows.Forms.ToolBarButton tbbSeperator2;
        private System.Windows.Forms.ToolBarButton tbbEnableWireframeButton;
        private 
            System.Windows.Forms.ToolBarButton tbbEnableDefaultLightsButton;
        private System.Windows.Forms.ToolBarButton tbbRenderBackgroundButton;
        private System.Windows.Forms.ToolBarButton tbbFrameRateButton;
        private System.Windows.Forms.ToolBarButton tbbCollisionABVButton;
        private System.Windows.Forms.ToolBarButton tbbTrajectoryButton;
        private System.Windows.Forms.ToolBarButton tbbSeperator3;
        private System.Windows.Forms.ToolBarButton tbbResetShadersButton;
        private System.Windows.Forms.ToolBarButton tbbResetActorPosButton;
        private System.Windows.Forms.ToolBarButton tbbSceneOptionsButton;
        private System.Windows.Forms.ToolBarButton tbbResetOrbitPointButton;
        private System.Windows.Forms.Panel pnlRendererContainer;
        private System.Windows.Forms.ColorDialog ColorDialog_Dlg;
        private System.Windows.Forms.MenuItem miBackgroundColor;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem miHelpRoot;
        private System.Windows.Forms.MenuItem miHelpToolbars;
        private System.Windows.Forms.MenuItem miHelpRender;
        private System.Windows.Forms.MenuItem miHelpTimeSlider;
        private System.Windows.Forms.ToolBarButton tbbTrackActorButton;
        private System.Windows.Forms.MenuItem miTrackActor;
        private System.Windows.Forms.MenuItem miSceneOptions;

        public RenderForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                ctlTimeSlider.DeleteContents();
                Invalidator.Instance.UnregisterControl(ctlTimeSlider);
                Invalidator.Instance.UnregisterControl(ctlRenderPanel);
                InputManager.Instance.RemoveForm(this);
                if(components != null)
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(RenderForm));
            this.cmRenderMenu = new System.Windows.Forms.ContextMenu();
            this.miUIState = new System.Windows.Forms.MenuItem();
            this.miOrbitMode = new System.Windows.Forms.MenuItem();
            this.miFreeLookMode = new System.Windows.Forms.MenuItem();
            this.miAdjustSceneHeightMode = new System.Windows.Forms.MenuItem();
            this.miCommands = new System.Windows.Forms.MenuItem();
            this.miTrackActor = new System.Windows.Forms.MenuItem();
            this.miWireframe = new System.Windows.Forms.MenuItem();
            this.miSceneLights = new System.Windows.Forms.MenuItem();
            this.miBackgroundScene = new System.Windows.Forms.MenuItem();
            this.miFrameRate = new System.Windows.Forms.MenuItem();
            this.miCollisionABVs = new System.Windows.Forms.MenuItem();
            this.miSeparator2 = new System.Windows.Forms.MenuItem();
            this.miZoomExtentsCamera = new System.Windows.Forms.MenuItem();
            this.miReloadShaders = new System.Windows.Forms.MenuItem();
            this.miResetActorPos = new System.Windows.Forms.MenuItem();
            this.miResetOrbitPoint = new System.Windows.Forms.MenuItem();
            this.miSeparator1 = new System.Windows.Forms.MenuItem();
            this.miSceneOptions = new System.Windows.Forms.MenuItem();
            this.miBackgroundColor = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.miHelpRoot = new System.Windows.Forms.MenuItem();
            this.miHelpToolbars = new System.Windows.Forms.MenuItem();
            this.miHelpRender = new System.Windows.Forms.MenuItem();
            this.miHelpTimeSlider = new System.Windows.Forms.MenuItem();
            this.ctlRenderPanel = new AnimationToolUI.RenderPanel();
            this.ctlTimeSlider = new AnimationToolUI.TimeSlider();
            this.tbNavigationToolbar = new System.Windows.Forms.ToolBar();
            this.tbbOrbitButton = new System.Windows.Forms.ToolBarButton();
            this.tbbFreeCameraButton = new System.Windows.Forms.ToolBarButton();
            this.tbbAdjustSceneHeightButton = new System.Windows.Forms.ToolBarButton();
            this.tbbSeperator1 = new System.Windows.Forms.ToolBarButton();
            this.tbbCameraZoomExtentsButton = new System.Windows.Forms.ToolBarButton();
            this.tbbDecBoneLODButton = new System.Windows.Forms.ToolBarButton();
            this.tbbIncBoneLODButton = new System.Windows.Forms.ToolBarButton();
            this.tbbSeperator2 = new System.Windows.Forms.ToolBarButton();
            this.tbbTrackActorButton = new System.Windows.Forms.ToolBarButton();
            this.tbbEnableWireframeButton = new System.Windows.Forms.ToolBarButton();
            this.tbbEnableDefaultLightsButton = new System.Windows.Forms.ToolBarButton();
            this.tbbRenderBackgroundButton = new System.Windows.Forms.ToolBarButton();
            this.tbbFrameRateButton = new System.Windows.Forms.ToolBarButton();
            this.tbbCollisionABVButton = new System.Windows.Forms.ToolBarButton();
            this.tbbTrajectoryButton = new System.Windows.Forms.ToolBarButton();
            this.tbbSeperator3 = new System.Windows.Forms.ToolBarButton();
            this.tbbResetShadersButton = new System.Windows.Forms.ToolBarButton();
            this.tbbResetActorPosButton = new System.Windows.Forms.ToolBarButton();
            this.tbbResetOrbitPointButton = new System.Windows.Forms.ToolBarButton();
            this.tbbSceneOptionsButton = new System.Windows.Forms.ToolBarButton();
            this.ilNavigationImages = new System.Windows.Forms.ImageList(this.components);
            this.pnlRendererContainer = new System.Windows.Forms.Panel();
            this.ColorDialog_Dlg = new System.Windows.Forms.ColorDialog();
            this.pnlRendererContainer.SuspendLayout();
            this.SuspendLayout();
            // 
            // cmRenderMenu
            // 
            this.cmRenderMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                         this.miUIState,
                                                                                         this.miCommands,
                                                                                         this.miSeparator1,
                                                                                         this.miSceneOptions,
                                                                                         this.miBackgroundColor,
                                                                                         this.menuItem1,
                                                                                         this.miHelpRoot});
            this.cmRenderMenu.Popup += new System.EventHandler(this.cmRenderMenu_Popup);
            // 
            // miUIState
            // 
            this.miUIState.Index = 0;
            this.miUIState.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                      this.miOrbitMode,
                                                                                      this.miFreeLookMode,
                                                                                      this.miAdjustSceneHeightMode});
            this.miUIState.Text = "UI State";
            // 
            // miOrbitMode
            // 
            this.miOrbitMode.Index = 0;
            this.miOrbitMode.Text = "Orbit";
            this.miOrbitMode.Click += new System.EventHandler(this.miOrbitMode_Click);
            // 
            // miFreeLookMode
            // 
            this.miFreeLookMode.Index = 1;
            this.miFreeLookMode.Text = "Free Look";
            this.miFreeLookMode.Click += new System.EventHandler(this.miFreeLookMode_Click);
            // 
            // miAdjustSceneHeightMode
            // 
            this.miAdjustSceneHeightMode.Index = 2;
            this.miAdjustSceneHeightMode.Text = "Adjust Scene Height";
            this.miAdjustSceneHeightMode.Click += new System.EventHandler(this.miAdjustSceneHeightMode_Click);
            // 
            // miCommands
            // 
            this.miCommands.Index = 1;
            this.miCommands.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miTrackActor,
                                                                                       this.miWireframe,
                                                                                       this.miSceneLights,
                                                                                       this.miBackgroundScene,
                                                                                       this.miFrameRate,
                                                                                       this.miCollisionABVs,
                                                                                       this.miSeparator2,
                                                                                       this.miZoomExtentsCamera,
                                                                                       this.miReloadShaders,
                                                                                       this.miResetActorPos,
                                                                                       this.miResetOrbitPoint});
            this.miCommands.Text = "Commands";
            // 
            // miTrackActor
            // 
            this.miTrackActor.Index = 0;
            this.miTrackActor.Text = "Track Actor";
            this.miTrackActor.Click += new System.EventHandler(this.miTrackActor_Click);
            // 
            // miWireframe
            // 
            this.miWireframe.Index = 1;
            this.miWireframe.Text = "Wireframe";
            this.miWireframe.Click += new System.EventHandler(this.miWireframe_Click);
            // 
            // miSceneLights
            // 
            this.miSceneLights.Index = 2;
            this.miSceneLights.Text = "Scene Lights";
            this.miSceneLights.Click += new System.EventHandler(this.miSceneLights_Click);
            // 
            // miBackgroundScene
            // 
            this.miBackgroundScene.Index = 3;
            this.miBackgroundScene.Text = "Background Scene";
            this.miBackgroundScene.Click += new System.EventHandler(this.miBackgroundScene_Click);
            // 
            // miFrameRate
            // 
            this.miFrameRate.Index = 4;
            this.miFrameRate.Text = "FPS Counter";
            this.miFrameRate.Click += new System.EventHandler(this.miFrameRate_Click);
            // 
            // miCollisionABVs
            // 
            this.miCollisionABVs.Index = 5;
            this.miCollisionABVs.Text = "Collision ABVs";
            this.miCollisionABVs.Click += new System.EventHandler(this.miCollisionABVs_Click);
            // 
            // miSeparator2
            // 
            this.miSeparator2.Index = 6;
            this.miSeparator2.Text = "-";
            // 
            // miZoomExtentsCamera
            // 
            this.miZoomExtentsCamera.Index = 7;
            this.miZoomExtentsCamera.Text = "Camera Zoom Extents";
            this.miZoomExtentsCamera.Click += new System.EventHandler(this.miZoomExtentsCamera_Click);
            // 
            // miReloadShaders
            // 
            this.miReloadShaders.Index = 8;
            this.miReloadShaders.Text = "Reload Shaders";
            this.miReloadShaders.Click += new System.EventHandler(this.miReloadShaders_Click);
            // 
            // miResetActorPos
            // 
            this.miResetActorPos.Index = 9;
            this.miResetActorPos.Text = "Reset Actor Position";
            this.miResetActorPos.Click += new System.EventHandler(this.miResetActorPos_Click);
            // 
            // miResetOrbitPoint
            // 
            this.miResetOrbitPoint.Index = 10;
            this.miResetOrbitPoint.Text = "Reset Orbit Point";
            this.miResetOrbitPoint.Click += new System.EventHandler(this.miResetOrbitPoint_Click);
            // 
            // miSeparator1
            // 
            this.miSeparator1.Index = 2;
            this.miSeparator1.Text = "-";
            // 
            // miSceneOptions
            // 
            this.miSceneOptions.Index = 3;
            this.miSceneOptions.Text = "Scene Options";
            this.miSceneOptions.Click += new System.EventHandler(this.miSceneOptions_Click);
            // 
            // miBackgroundColor
            // 
            this.miBackgroundColor.Index = 4;
            this.miBackgroundColor.Text = "Edit Background Color";
            this.miBackgroundColor.Click += new System.EventHandler(this.miBackgroundColor_Click);
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 5;
            this.menuItem1.Text = "-";
            // 
            // miHelpRoot
            // 
            this.miHelpRoot.Index = 6;
            this.miHelpRoot.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                       this.miHelpToolbars,
                                                                                       this.miHelpRender,
                                                                                       this.miHelpTimeSlider});
            this.miHelpRoot.Text = "Help...";
            // 
            // miHelpToolbars
            // 
            this.miHelpToolbars.Index = 0;
            this.miHelpToolbars.Text = "Toolbars";
            this.miHelpToolbars.Click += new System.EventHandler(this.miHelpToolbars_Click);
            // 
            // miHelpRender
            // 
            this.miHelpRender.Index = 1;
            this.miHelpRender.Text = "Render Preview";
            this.miHelpRender.Click += new System.EventHandler(this.miHelpRender_Click);
            // 
            // miHelpTimeSlider
            // 
            this.miHelpTimeSlider.Index = 2;
            this.miHelpTimeSlider.Text = "Time Slider";
            this.miHelpTimeSlider.Click += new System.EventHandler(this.miHelpTimeSlider_Click);
            // 
            // ctlRenderPanel
            // 
            this.ctlRenderPanel.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.ctlRenderPanel.ContextMenu = this.cmRenderMenu;
            this.ctlRenderPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ctlRenderPanel.Location = new System.Drawing.Point(0, 0);
            this.ctlRenderPanel.Name = "ctlRenderPanel";
            this.ctlRenderPanel.Size = new System.Drawing.Size(560, 404);
            this.ctlRenderPanel.TabIndex = 0;
            // 
            // ctlTimeSlider
            // 
            this.ctlTimeSlider.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.ctlTimeSlider.Location = new System.Drawing.Point(0, 438);
            this.ctlTimeSlider.Name = "ctlTimeSlider";
            this.ctlTimeSlider.Size = new System.Drawing.Size(560, 88);
            this.ctlTimeSlider.TabIndex = 1;
            // 
            // tbNavigationToolbar
            // 
            this.tbNavigationToolbar.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
                                                                                                   this.tbbOrbitButton,
                                                                                                   this.tbbFreeCameraButton,
                                                                                                   this.tbbAdjustSceneHeightButton,
                                                                                                   this.tbbSeperator1,
                                                                                                   this.tbbCameraZoomExtentsButton,
                                                                                                   this.tbbDecBoneLODButton,
                                                                                                   this.tbbIncBoneLODButton,
                                                                                                   this.tbbSeperator2,
                                                                                                   this.tbbTrackActorButton,
                                                                                                   this.tbbEnableWireframeButton,
                                                                                                   this.tbbEnableDefaultLightsButton,
                                                                                                   this.tbbRenderBackgroundButton,
                                                                                                   this.tbbFrameRateButton,
                                                                                                   this.tbbCollisionABVButton,
                                                                                                   this.tbbTrajectoryButton,
                                                                                                   this.tbbSeperator3,
                                                                                                   this.tbbResetShadersButton,
                                                                                                   this.tbbResetActorPosButton,
                                                                                                   this.tbbResetOrbitPointButton,
                                                                                                   this.tbbSceneOptionsButton});
            this.tbNavigationToolbar.ButtonSize = new System.Drawing.Size(24, 24);
            this.tbNavigationToolbar.Divider = false;
            this.tbNavigationToolbar.DropDownArrows = true;
            this.tbNavigationToolbar.ImageList = this.ilNavigationImages;
            this.tbNavigationToolbar.Location = new System.Drawing.Point(0, 0);
            this.tbNavigationToolbar.Name = "tbNavigationToolbar";
            this.tbNavigationToolbar.ShowToolTips = true;
            this.tbNavigationToolbar.Size = new System.Drawing.Size(560, 34);
            this.tbNavigationToolbar.TabIndex = 0;
            this.tbNavigationToolbar.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.tbNavigationToolbar_ButtonClick);
            // 
            // tbbOrbitButton
            // 
            this.tbbOrbitButton.ImageIndex = 0;
            this.tbbOrbitButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbOrbitButton.ToolTipText = "Orbit the camera around the character.";
            // 
            // tbbFreeCameraButton
            // 
            this.tbbFreeCameraButton.ImageIndex = 1;
            this.tbbFreeCameraButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbFreeCameraButton.ToolTipText = "Pan the camera.";
            // 
            // tbbAdjustSceneHeightButton
            // 
            this.tbbAdjustSceneHeightButton.ImageIndex = 2;
            this.tbbAdjustSceneHeightButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbAdjustSceneHeightButton.ToolTipText = "Adjust the height of the scene relative to the currently loaded character.";
            // 
            // tbbSeperator1
            // 
            this.tbbSeperator1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbbCameraZoomExtentsButton
            // 
            this.tbbCameraZoomExtentsButton.ImageIndex = 3;
            this.tbbCameraZoomExtentsButton.ToolTipText = "Zoom the camera out to the extents of the current scene.";
            // 
            // tbbDecBoneLODButton
            // 
            this.tbbDecBoneLODButton.ImageIndex = 5;
            this.tbbDecBoneLODButton.ToolTipText = "Decrement the current Bone LOD.";
            // 
            // tbbIncBoneLODButton
            // 
            this.tbbIncBoneLODButton.ImageIndex = 4;
            this.tbbIncBoneLODButton.ToolTipText = "Increment the current Bone LOD.";
            // 
            // tbbSeperator2
            // 
            this.tbbSeperator2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbbTrackActorButton
            // 
            this.tbbTrackActorButton.ImageIndex = 6;
            this.tbbTrackActorButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbTrackActorButton.ToolTipText = "Toggle the camera so that it follows the character or is free to move.";
            // 
            // tbbEnableWireframeButton
            // 
            this.tbbEnableWireframeButton.ImageIndex = 7;
            this.tbbEnableWireframeButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbEnableWireframeButton.ToolTipText = "Toggle wireframe rendering mode.";
            // 
            // tbbEnableDefaultLightsButton
            // 
            this.tbbEnableDefaultLightsButton.ImageIndex = 8;
            this.tbbEnableDefaultLightsButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbEnableDefaultLightsButton.ToolTipText = "Toggle the usage of default scene lights.";
            // 
            // tbbRenderBackgroundButton
            // 
            this.tbbRenderBackgroundButton.ImageIndex = 9;
            this.tbbRenderBackgroundButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbRenderBackgroundButton.ToolTipText = "Toggle the visibility of the background scene.";
            // 
            // tbbFrameRateButton
            // 
            this.tbbFrameRateButton.ImageIndex = 10;
            this.tbbFrameRateButton.Pushed = true;
            this.tbbFrameRateButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbFrameRateButton.ToolTipText = "Toggle the display of screen text in the viewport.";
            // 
            // tbbCollisionABVButton
            // 
            this.tbbCollisionABVButton.ImageIndex = 11;
            this.tbbCollisionABVButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbCollisionABVButton.ToolTipText = "Toggle collision ABV volume visibility.";
            // 
            // tbbTrajectoryButton
            // 
            this.tbbTrajectoryButton.ImageIndex = 12;
            this.tbbTrajectoryButton.Style = System.Windows.Forms.ToolBarButtonStyle.ToggleButton;
            this.tbbTrajectoryButton.ToolTipText = "Toggle the display of an animation accumulation trail.";
            // 
            // tbbSeperator3
            // 
            this.tbbSeperator3.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // tbbResetShadersButton
            // 
            this.tbbResetShadersButton.ImageIndex = 13;
            this.tbbResetShadersButton.ToolTipText = "Reload all of the pixel/vertex shaders for the current character and background.";
            // 
            // tbbResetActorPosButton
            // 
            this.tbbResetActorPosButton.ImageIndex = 14;
            this.tbbResetActorPosButton.ToolTipText = "Reset the position of the character to the origin.";
            // 
            // tbbResetOrbitPointButton
            // 
            this.tbbResetOrbitPointButton.ImageIndex = 15;
            this.tbbResetOrbitPointButton.ToolTipText = "Reset the orbit point for the orbit camera mode.";
            // 
            // tbbSceneOptionsButton
            // 
            this.tbbSceneOptionsButton.ImageIndex = 16;
            this.tbbSceneOptionsButton.ToolTipText = "Edit the options for the current scene.";
            // 
            // ilNavigationImages
            // 
            this.ilNavigationImages.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
            this.ilNavigationImages.ImageSize = new System.Drawing.Size(24, 24);
            this.ilNavigationImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilNavigationImages.ImageStream")));
            this.ilNavigationImages.TransparentColor = System.Drawing.Color.Fuchsia;
            // 
            // pnlRendererContainer
            // 
            this.pnlRendererContainer.Controls.Add(this.ctlRenderPanel);
            this.pnlRendererContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlRendererContainer.Location = new System.Drawing.Point(0, 34);
            this.pnlRendererContainer.Name = "pnlRendererContainer";
            this.pnlRendererContainer.Size = new System.Drawing.Size(560, 404);
            this.pnlRendererContainer.TabIndex = 3;
            // 
            // ColorDialog_Dlg
            // 
            this.ColorDialog_Dlg.AnyColor = true;
            this.ColorDialog_Dlg.FullOpen = true;
            // 
            // RenderForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(560, 526);
            this.Controls.Add(this.pnlRendererContainer);
            this.Controls.Add(this.ctlTimeSlider);
            this.Controls.Add(this.tbNavigationToolbar);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "RenderForm";
            this.Text = "RenderForm";
            this.Load += new System.EventHandler(this.RenderForm_Load);
            this.pnlRendererContainer.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
        }

        private void RenderForm_Load(object sender, System.EventArgs e)
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);

            if (!MFramework.InstanceIsValid())
                return;

             ctlRenderPanel.ParentHandle = Handle;

            Invalidator.Instance.RegisterControl(ctlRenderPanel);
            Invalidator.Instance.RegisterControl(ctlTimeSlider);
            InputManager.Instance.AddForm(this);
            
            for (int ui = 0;
                ui < (int) MUICommand.CommandType.MAX_COMMAND_TYPE; ui++)
            {
                MUICommand kCommand = MFramework.Instance.Input.GetCommand(
                    (MUICommand.CommandType) ui);
                kCommand.OnCommandStateChanged += 
                    new MUICommand.__Delegate_OnCommandStateChanged(
                    this.OnUICommandChanged);
            }

            MFramework.Instance.Input.OnActiveUITypeChanged += 
                new MUIManager.__Delegate_OnActiveUITypeChanged(
                this.OnUITypeChanged);

            MFramework.Instance.Animation.OnPlaybackModeChanged +=
                new MAnimation.__Delegate_OnPlaybackModeChanged(
                this.OnPlaybackModeChanged);

            MFramework.Instance.Renderer.SetActiveModeString(MFramework
                .Instance.Animation.Mode.ToString());

            if (!HelpManager.HelpExists())
                miHelpRoot.Enabled = false;

            RefreshToolbar();
        }

        private void miSceneOptions_Click(object sender, System.EventArgs e)
        {
            SceneOptionsForm dlgSceneOptions = new SceneOptionsForm();
            dlgSceneOptions.ShowDialog(this);
        }

        private void cmRenderMenu_Popup(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            for (int ui = 0;
                ui < (int) MUICommand.CommandType.MAX_COMMAND_TYPE; ui++)
            {
                MUICommand kCommand = MFramework.Instance.Input.GetCommand(
                    (MUICommand.CommandType) ui);
                OnUICommandChanged(kCommand);
            }

            OnUITypeChanged(MFramework.Instance.Input.ActiveUIType);
        }

        private void OnPlaybackModeChanged(MAnimation.PlaybackMode eMode)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Renderer.SetActiveModeString(
                eMode.ToString());
        }

        private void OnUITypeChanged(MUIState.UIType eType)
        {
            switch(eType)
            {
                case MUIState.UIType.ORBIT:
                    miOrbitMode.Checked = true;
                    miFreeLookMode.Checked = false;
                    miAdjustSceneHeightMode.Checked = false;
                    break;
                case MUIState.UIType.FREELOOK:
                    miOrbitMode.Checked = false;
                    miFreeLookMode.Checked = true;
                    miAdjustSceneHeightMode.Checked = false;
                    break;
                case MUIState.UIType.BACKGROUNDHEIGHT:
                    miOrbitMode.Checked = false;
                    miFreeLookMode.Checked = false;
                    miAdjustSceneHeightMode.Checked = true;
                    break;
#if EE_PHYSX_BUILD
                case MUIState.UIType.PHYSICSFORCE:
                    miOrbitMode.Checked = false;
                    miFreeLookMode.Checked = false;
                    miAdjustSceneHeightMode.Checked = false;
                    break;
#endif
            }

            RefreshToolbar();
        }

        private void OnUICommandChanged(MUICommand kCommand)
        {
            if (!MFramework.InstanceIsValid())
                return;

            switch(kCommand.GetCommandType())
            {
                case MUICommand.CommandType.ZOOM_EXTENTS:
                    miZoomExtentsCamera.Enabled = kCommand.GetEnabled();
                    break;
                case MUICommand.CommandType.RESET_FRUSTUM:
                    break;
                case MUICommand.CommandType.BONELOD_INCREMENT:
                    break;
                case MUICommand.CommandType.BONELOD_DECREMENT:
                    break;
                case MUICommand.CommandType.TOGGLE_ORBIT_FOLLOW:
                    miTrackActor.Enabled = kCommand.GetEnabled();
                    miTrackActor.Checked = kCommand.GetActive();
                    break;
                case MUICommand.CommandType.TOGGLE_WIREFRAME:
                    miWireframe.Enabled = kCommand.GetEnabled();
                    miWireframe.Checked = kCommand.GetActive();
                    break;
                case MUICommand.CommandType.TOGGLE_SCENE_LIGHTS:
                    miSceneLights.Enabled = kCommand.GetEnabled();
                    miSceneLights.Checked = kCommand.GetActive();
                    break;
                case MUICommand.CommandType.TOGGLE_BACKGROUND_SCENE_VIS:
                    miBackgroundScene.Enabled = kCommand.GetEnabled();
                    miBackgroundScene.Checked = kCommand.GetActive();
                    break;
                case MUICommand.CommandType.TOGGLE_FRAME_RATE:
                    miFrameRate.Enabled = kCommand.GetEnabled();
                    miFrameRate.Checked = kCommand.GetActive();
                    break;
                case MUICommand.CommandType.RESET_BACKGROUND_HEIGHT:
                    break;
                case MUICommand.CommandType.RESET_ACTOR_POSITION:
                    miResetActorPos.Enabled = kCommand.GetEnabled();
                    break;
                case MUICommand.CommandType.TOGGLE_COLLISION_ABVS:
                    miCollisionABVs.Enabled = kCommand.GetEnabled();
                    miCollisionABVs.Checked = kCommand.GetActive();
                    break;
            }

            RefreshToolbar();
        }

        private void miZoomExtentsCamera_Click(object sender,
            System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.ZOOM_EXTENTS);
        }

        private void miOrbitMode_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            if (MFramework.Instance.Input.ActiveUIType != 
                MUIState.UIType.ORBIT)
            {
                MFramework.Instance.Input.ActiveUIType = 
                    MUIState.UIType.ORBIT;
            }
        }

        private void miFreeLookMode_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            if (MFramework.Instance.Input.ActiveUIType != 
                MUIState.UIType.FREELOOK)
            {
                MFramework.Instance.Input.ActiveUIType = 
                    MUIState.UIType.FREELOOK;
            }
        }

        private void miAdjustSceneHeightMode_Click(object sender,
            System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            if (MFramework.Instance.Input.ActiveUIType != 
                MUIState.UIType.BACKGROUNDHEIGHT)
            {
                MFramework.Instance.Input.ActiveUIType = 
                    MUIState.UIType.BACKGROUNDHEIGHT;
            }
        }

        private void miWireframe_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.TOGGLE_WIREFRAME);
        }

        private void miSceneLights_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.TOGGLE_SCENE_LIGHTS);
        }

        private void miBackgroundScene_Click(object sender,
            System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.TOGGLE_BACKGROUND_SCENE_VIS);
        }

        private void miFrameRate_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.TOGGLE_FRAME_RATE);
        }

        private void miReloadShaders_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Renderer.ReloadShaders();
        }

        private void miCollisionABVs_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.TOGGLE_COLLISION_ABVS);
        }

        private void miResetActorPos_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.RESET_ACTOR_POSITION);
        }

        private void miResetOrbitPoint_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.RESET_ORBIT_POINT);
        }

        
        private void miTrackActor_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
                return;

            MFramework.Instance.Input.SubmitCommand(
                MUICommand.CommandType.TOGGLE_ORBIT_FOLLOW);
        }

        private void tbNavigationToolbar_ButtonClick(object sender,
            System.Windows.Forms.ToolBarButtonClickEventArgs e)
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            if (e.Button == tbbOrbitButton)
            {
                tbbOrbitButton.Pushed = true;
                tbbFreeCameraButton.Pushed = false;
                tbbAdjustSceneHeightButton.Pushed = false;
                if (MFramework.Instance.Input.ActiveUIType != 
                    MUIState.UIType.ORBIT)
                {
                    MFramework.Instance.Input.ActiveUIType = 
                        MUIState.UIType.ORBIT;
                }
                
            }
            else if (e.Button == tbbFreeCameraButton)
            {
                tbbOrbitButton.Pushed = false;
                tbbFreeCameraButton.Pushed = true;
                tbbAdjustSceneHeightButton.Pushed = false;
                if (MFramework.Instance.Input.ActiveUIType != 
                    MUIState.UIType.FREELOOK)
                {
                    MFramework.Instance.Input.ActiveUIType = 
                        MUIState.UIType.FREELOOK;
                }
            }
            else if (e.Button == tbbAdjustSceneHeightButton)
            {
                tbbOrbitButton.Pushed = false;
                tbbFreeCameraButton.Pushed = false;
                tbbAdjustSceneHeightButton.Pushed = true;
                if (MFramework.Instance.Input.ActiveUIType != 
                    MUIState.UIType.BACKGROUNDHEIGHT)
                {
                    MFramework.Instance.Input.ActiveUIType = 
                        MUIState.UIType.BACKGROUNDHEIGHT;
                }
            }
            else if (e.Button == this.tbbTrackActorButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_ORBIT_FOLLOW);
            }
            else if (e.Button == tbbEnableWireframeButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_WIREFRAME);
            }
            else if (e.Button == tbbResetShadersButton)
            {
                MFramework.Instance.Renderer.ReloadShaders();
            }
            else if (e.Button == tbbIncBoneLODButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.BONELOD_INCREMENT);
                RefreshToolbar();
            }
            else if (e.Button == tbbDecBoneLODButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.BONELOD_DECREMENT);
                RefreshToolbar();
            }
            else if (e.Button == tbbEnableDefaultLightsButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_SCENE_LIGHTS);
            }
            else if (e.Button == tbbRenderBackgroundButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_BACKGROUND_SCENE_VIS);
            }
            else if (e.Button == tbbFrameRateButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_FRAME_RATE);
            }
            else if (e.Button == tbbCollisionABVButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_COLLISION_ABVS);
            }
            else if (e.Button == tbbTrajectoryButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.TOGGLE_TRAJECTORY);
            }
            else if (e.Button == tbbSceneOptionsButton)
            {
                SceneOptionsForm kSceneOptions = new SceneOptionsForm();
                kSceneOptions.ShowDialog(this);
            }
            else if (e.Button == tbbCameraZoomExtentsButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.ZOOM_EXTENTS);
            }
            else if (e.Button == tbbResetActorPosButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_ACTOR_POSITION);
            }
            else if (e.Button == tbbResetOrbitPointButton)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_ORBIT_POINT);
            }
        }

        private void RefreshToolbar()
        {
            if (!MFramework.InstanceIsValid())
                return;

            if (MFramework.Instance.Input.ActiveUIType == 
                MUIState.UIType.ORBIT)
            {
                tbbOrbitButton.Pushed = true;
                tbbFreeCameraButton.Pushed = false;
                tbbAdjustSceneHeightButton.Pushed = false;
                
            }
            else if (MFramework.Instance.Input.ActiveUIType == 
                MUIState.UIType.FREELOOK)
            {
                tbbOrbitButton.Pushed = false;
                tbbFreeCameraButton.Pushed = true;
                tbbAdjustSceneHeightButton.Pushed = false;
            }
            else if (MFramework.Instance.Input.ActiveUIType == 
                MUIState.UIType.BACKGROUNDHEIGHT)
            {
                tbbOrbitButton.Pushed = false;
                tbbFreeCameraButton.Pushed = false;
                tbbAdjustSceneHeightButton.Pushed = true;
            }
#if EE_PHYSX_BUILD
            else if (MFramework.Instance.Input.ActiveUIType ==
                MUIState.UIType.PHYSICSFORCE)
            {
                tbbOrbitButton.Pushed = false;
                tbbFreeCameraButton.Pushed = false;
                tbbAdjustSceneHeightButton.Pushed = false;
            }
#endif
            
            MUICommand kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_ORBIT_FOLLOW);
            tbbTrackActorButton.Pushed = kCommand.GetActive();
            tbbTrackActorButton.Enabled = kCommand.GetEnabled();
            
            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_WIREFRAME);
            tbbEnableWireframeButton.Pushed = kCommand.GetActive();
            tbbEnableWireframeButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_BACKGROUND_SCENE_VIS);
            tbbRenderBackgroundButton.Pushed = kCommand.GetActive();
            tbbRenderBackgroundButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.BONELOD_INCREMENT);
            tbbIncBoneLODButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.BONELOD_DECREMENT);
            tbbDecBoneLODButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_SCENE_LIGHTS);
            tbbEnableDefaultLightsButton.Pushed = kCommand.GetActive();
            tbbEnableDefaultLightsButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_FRAME_RATE);
            tbbFrameRateButton.Pushed = kCommand.GetActive();
            tbbFrameRateButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_COLLISION_ABVS);
            tbbCollisionABVButton.Pushed = kCommand.GetActive();
            tbbCollisionABVButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.TOGGLE_TRAJECTORY);
            tbbTrajectoryButton.Pushed = kCommand.GetActive();
            tbbTrajectoryButton.Enabled = kCommand.GetEnabled();

            kCommand = MFramework.Instance.Input.GetCommand(
                MUICommand.CommandType.RESET_ACTOR_POSITION);
            tbbResetActorPosButton.Enabled = kCommand.GetEnabled();
        }

        private MColor GetManagedColor(Color kColor)
        {
            float r = ((float)kColor.R)/255.0f;
            float g = ((float)kColor.G)/255.0f;
            float b = ((float)kColor.B)/255.0f;
            return new MColor(r, g, b);
        }

        private void miBackgroundColor_Click(object sender, System.EventArgs e)
        {
            ProjectData kData = ProjectData.Instance;
            MColor kOriginalColor = kData.RendererClearColor;
            Color kPanelColor = Color.FromArgb(255, 
                (int)(kOriginalColor.r*255 + 0.5f), 
                (int)(kOriginalColor.g*255 + 0.5f), 
                (int)(kOriginalColor.b*255 + 0.5f));
            
            ColorDialog_Dlg.Color = kPanelColor;
            DialogResult kRes = ColorDialog_Dlg.ShowDialog();
            if (kRes == DialogResult.OK)
            {
                if (ColorDialog_Dlg.Color != kPanelColor)
                {
                    kData.RendererClearColor = 
                        GetManagedColor(ColorDialog_Dlg.Color);
                    kData.SaveRendererRegistrySettings();
                }
            }
        }

        private void miHelpTimeSlider_Click(object sender, System.EventArgs e)
        {
            HelpManager.DoHelp(this, 
                @"User_Interface_Description/"+
                "Render_View.htm#Render_View_Time_Slider");        
        }

        private void miHelpRender_Click(object sender, System.EventArgs e)
        {
            HelpManager.DoHelp(this, 
                @"User_Interface_Description/"+
                "Render_View.htm#Render_View_Display");
        }

        private void miHelpToolbars_Click(object sender, System.EventArgs e)
        {
            HelpManager.DoHelp(this, 
                @"User_Interface_Description/"+
                "Render_View.htm#Render_View_Toolbar");
        }
    }
}
