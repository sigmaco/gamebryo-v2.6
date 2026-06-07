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
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    public class ViewportSettingsPanel : Form
    {
        private System.Windows.Forms.RadioButton m_rbPerspective;
        private System.Windows.Forms.RadioButton m_rbSceneCamera;
        private System.Windows.Forms.ComboBox m_cbSceneCamera;
        private System.Windows.Forms.Label m_lblDummy;
        private System.Windows.Forms.Label m_lblRenderingMode;
        private System.Windows.Forms.ComboBox m_cbRenderingMode;
        private System.Windows.Forms.GroupBox m_gbCurrentCamera;
        private System.Windows.Forms.RadioButton m_rbUser;
        private System.Windows.Forms.RadioButton m_rbNegZ;
        private System.Windows.Forms.RadioButton m_rbPosZ;
        private System.Windows.Forms.RadioButton m_rbNegY;
        private System.Windows.Forms.RadioButton m_rbPosY;
        private System.Windows.Forms.RadioButton m_rbNegX;
        private System.Windows.Forms.RadioButton m_rbPosX;
        private System.ComponentModel.IContainer components = null;

        public ViewportSettingsPanel()
        {
            // This call is required by the Windows Form Designer.
            InitializeComponent();

            MFramework.Instance.EventManager.NewSceneLoaded += new
                MEventManager.__Delegate_NewSceneLoaded(
                this.OnNewSceneLoaded);
            MFramework.Instance.EventManager.LayerAdded += new
                MEventManager.__Delegate_LayerAdded(
                this.OnLayerAdded);
            MFramework.Instance.EventManager.ActiveViewportChanged +=
                new MEventManager.__Delegate_ActiveViewportChanged(
                this.OnActiveViewportChanged);
            MFramework.Instance.EventManager.ViewportCameraChanged +=
                new MEventManager.__Delegate_ViewportCameraChanged(
                this.OnViewportCameraChanged);
            MFramework.Instance.EventManager.ViewportRenderingModeChanged +=
                new MEventManager.__Delegate_ViewportRenderingModeChanged(
                this.OnViewportRenderingModeChanged);
            MFramework.Instance.EventManager.CameraAddedToManager +=
                new MEventManager.__Delegate_CameraAddedToManager(
                this.OnCameraAddedToManager);
            MFramework.Instance.EventManager.CameraRemovedFromManager +=
                new MEventManager.__Delegate_CameraRemovedFromManager(
                this.OnCameraRemovedFromManager);

            MFramework.Instance.EventManager.EntityNameChanged +=
                new MEventManager.__Delegate_EntityNameChanged(
                this.OnEntityNameChanged);

            // Populate rendering modes.
            IRenderingMode[] amRenderingModes = RenderingModeService
                .GetRenderingModes();
            foreach (IRenderingMode pmRenderingMode in amRenderingModes)
            {
                if (pmRenderingMode.DisplayToUser)
                {
                    m_cbRenderingMode.Items.Add(pmRenderingMode.Name);
                }
            }
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

        #region Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ViewportSettingsPanel));
            this.m_lblRenderingMode = new System.Windows.Forms.Label();
            this.m_cbRenderingMode = new System.Windows.Forms.ComboBox();
            this.m_gbCurrentCamera = new System.Windows.Forms.GroupBox();
            this.m_cbSceneCamera = new System.Windows.Forms.ComboBox();
            this.m_rbSceneCamera = new System.Windows.Forms.RadioButton();
            this.m_rbNegZ = new System.Windows.Forms.RadioButton();
            this.m_rbPosZ = new System.Windows.Forms.RadioButton();
            this.m_rbNegY = new System.Windows.Forms.RadioButton();
            this.m_rbPosY = new System.Windows.Forms.RadioButton();
            this.m_rbNegX = new System.Windows.Forms.RadioButton();
            this.m_rbPosX = new System.Windows.Forms.RadioButton();
            this.m_rbUser = new System.Windows.Forms.RadioButton();
            this.m_rbPerspective = new System.Windows.Forms.RadioButton();
            this.m_lblDummy = new System.Windows.Forms.Label();
            this.m_gbCurrentCamera.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_lblRenderingMode
            // 
            this.m_lblRenderingMode.AutoSize = true;
            this.m_lblRenderingMode.Location = new System.Drawing.Point(8, 8);
            this.m_lblRenderingMode.Name = "m_lblRenderingMode";
            this.m_lblRenderingMode.Size = new System.Drawing.Size(91, 16);
            this.m_lblRenderingMode.TabIndex = 0;
            this.m_lblRenderingMode.Text = "Rendering Mode:";
            // 
            // m_cbRenderingMode
            // 
            this.m_cbRenderingMode.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbRenderingMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbRenderingMode.Location = new System.Drawing.Point(8, 24);
            this.m_cbRenderingMode.Name = "m_cbRenderingMode";
            this.m_cbRenderingMode.Size = new System.Drawing.Size(312, 21);
            this.m_cbRenderingMode.Sorted = true;
            this.m_cbRenderingMode.TabIndex = 1;
            this.m_cbRenderingMode.SelectedIndexChanged += new System.EventHandler(this.m_cbRenderingMode_SelectedIndexChanged);
            // 
            // m_gbCurrentCamera
            // 
            this.m_gbCurrentCamera.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_gbCurrentCamera.Controls.Add(this.m_cbSceneCamera);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbSceneCamera);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbNegZ);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbPosZ);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbNegY);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbPosY);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbNegX);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbPosX);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbUser);
            this.m_gbCurrentCamera.Controls.Add(this.m_rbPerspective);
            this.m_gbCurrentCamera.Location = new System.Drawing.Point(8, 56);
            this.m_gbCurrentCamera.Name = "m_gbCurrentCamera";
            this.m_gbCurrentCamera.Size = new System.Drawing.Size(312, 272);
            this.m_gbCurrentCamera.TabIndex = 2;
            this.m_gbCurrentCamera.TabStop = false;
            this.m_gbCurrentCamera.Text = "Current Viewport Camera";
            // 
            // m_cbSceneCamera
            // 
            this.m_cbSceneCamera.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbSceneCamera.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbSceneCamera.Enabled = false;
            this.m_cbSceneCamera.Location = new System.Drawing.Point(32, 240);
            this.m_cbSceneCamera.Name = "m_cbSceneCamera";
            this.m_cbSceneCamera.Size = new System.Drawing.Size(273, 21);
            this.m_cbSceneCamera.Sorted = true;
            this.m_cbSceneCamera.TabIndex = 9;
            this.m_cbSceneCamera.SelectedIndexChanged += new System.EventHandler(this.m_cbSceneCamera_SelectedIndexChanged);
            // 
            // m_rbSceneCamera
            // 
            this.m_rbSceneCamera.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbSceneCamera.Enabled = false;
            this.m_rbSceneCamera.Location = new System.Drawing.Point(16, 216);
            this.m_rbSceneCamera.Name = "m_rbSceneCamera";
            this.m_rbSceneCamera.Size = new System.Drawing.Size(288, 24);
            this.m_rbSceneCamera.TabIndex = 8;
            this.m_rbSceneCamera.Text = "Scene Camera";
            this.m_rbSceneCamera.CheckedChanged += new System.EventHandler(this.m_rbSceneCamera_CheckedChanged);
            // 
            // m_rbNegZ
            // 
            this.m_rbNegZ.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbNegZ.Location = new System.Drawing.Point(16, 192);
            this.m_rbNegZ.Name = "m_rbNegZ";
            this.m_rbNegZ.Size = new System.Drawing.Size(288, 24);
            this.m_rbNegZ.TabIndex = 7;
            this.m_rbNegZ.Text = "Negative Z";
            this.m_rbNegZ.CheckedChanged += new System.EventHandler(this.m_rbNegZ_CheckedChanged);
            // 
            // m_rbPosZ
            // 
            this.m_rbPosZ.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbPosZ.Location = new System.Drawing.Point(16, 168);
            this.m_rbPosZ.Name = "m_rbPosZ";
            this.m_rbPosZ.Size = new System.Drawing.Size(288, 24);
            this.m_rbPosZ.TabIndex = 6;
            this.m_rbPosZ.Text = "Positive Z";
            this.m_rbPosZ.CheckedChanged += new System.EventHandler(this.m_rbPosZ_CheckedChanged);
            // 
            // m_rbNegY
            // 
            this.m_rbNegY.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbNegY.Location = new System.Drawing.Point(16, 144);
            this.m_rbNegY.Name = "m_rbNegY";
            this.m_rbNegY.Size = new System.Drawing.Size(288, 24);
            this.m_rbNegY.TabIndex = 5;
            this.m_rbNegY.Text = "Negative Y";
            this.m_rbNegY.CheckedChanged += new System.EventHandler(this.m_rbNegY_CheckedChanged);
            // 
            // m_rbPosY
            // 
            this.m_rbPosY.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbPosY.Location = new System.Drawing.Point(16, 120);
            this.m_rbPosY.Name = "m_rbPosY";
            this.m_rbPosY.Size = new System.Drawing.Size(288, 24);
            this.m_rbPosY.TabIndex = 4;
            this.m_rbPosY.Text = "Positive Y";
            this.m_rbPosY.CheckedChanged += new System.EventHandler(this.m_rbPosY_CheckedChanged);
            // 
            // m_rbNegX
            // 
            this.m_rbNegX.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbNegX.Location = new System.Drawing.Point(16, 96);
            this.m_rbNegX.Name = "m_rbNegX";
            this.m_rbNegX.Size = new System.Drawing.Size(288, 24);
            this.m_rbNegX.TabIndex = 3;
            this.m_rbNegX.Text = "Negative X";
            this.m_rbNegX.CheckedChanged += new System.EventHandler(this.m_rbNegX_CheckedChanged);
            // 
            // m_rbPosX
            // 
            this.m_rbPosX.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbPosX.Location = new System.Drawing.Point(16, 72);
            this.m_rbPosX.Name = "m_rbPosX";
            this.m_rbPosX.Size = new System.Drawing.Size(288, 24);
            this.m_rbPosX.TabIndex = 2;
            this.m_rbPosX.Text = "Positive X";
            this.m_rbPosX.CheckedChanged += new System.EventHandler(this.m_rbPosX_CheckedChanged);
            // 
            // m_rbUser
            // 
            this.m_rbUser.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbUser.Location = new System.Drawing.Point(16, 48);
            this.m_rbUser.Name = "m_rbUser";
            this.m_rbUser.Size = new System.Drawing.Size(288, 24);
            this.m_rbUser.TabIndex = 1;
            this.m_rbUser.Text = "User";
            this.m_rbUser.CheckedChanged += new System.EventHandler(this.m_rbUser_CheckedChanged);
            // 
            // m_rbPerspective
            // 
            this.m_rbPerspective.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_rbPerspective.Location = new System.Drawing.Point(16, 24);
            this.m_rbPerspective.Name = "m_rbPerspective";
            this.m_rbPerspective.Size = new System.Drawing.Size(288, 24);
            this.m_rbPerspective.TabIndex = 0;
            this.m_rbPerspective.Text = "Perspective";
            this.m_rbPerspective.CheckedChanged += new System.EventHandler(this.m_rbPerspective_CheckedChanged);
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.Location = new System.Drawing.Point(136, 320);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(8, 8);
            this.m_lblDummy.TabIndex = 3;
            // 
            // ViewportSettingsPanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(328, 334);
            this.Controls.Add(this.m_gbCurrentCamera);
            this.Controls.Add(this.m_cbRenderingMode);
            this.Controls.Add(this.m_lblRenderingMode);
            this.Controls.Add(this.m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ViewportSettingsPanel";
            this.Text = "Viewport Settings";
            this.m_gbCurrentCamera.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        #region Service Accessors
        private static IRenderingModeService ms_pmRenderingModeService;
        private static IRenderingModeService RenderingModeService
        {
            get
            {
                if (ms_pmRenderingModeService == null)
                {
                    ms_pmRenderingModeService = ServiceProvider.Instance
                        .GetService(typeof(IRenderingModeService)) as
                        IRenderingModeService;
                    Debug.Assert(ms_pmRenderingModeService != null,
                        "Rendering mode service not found!");
                }
                return ms_pmRenderingModeService;
            }
        }
        #endregion

        private bool m_bIgnoreRenderingModeIndexChange = false;
        private bool m_bIgnoreRadioButtonClick = false;
        private bool m_bIgnoreSceneCameraIndexChange = false;

        #region Event Handlers
        private void OnNewSceneLoaded(MScene pmScene)
        {
            PopulateSceneCameras();
            SyncSceneCamera();
        }

        private void OnLayerAdded(MLayer pmLayer, MLayer pmParent)
        {
            PopulateSceneCameras();
            SyncSceneCamera();
        }

        private void OnActiveViewportChanged(MViewport pmOldViewport,
            MViewport pmNewViewport)
        {
            SyncRenderingMode();
            SyncCameraRadioButtons();
        }

        private void OnViewportCameraChanged(MViewport pmViewport, 
            MEntity pmCamera)
        {
            if (pmViewport == MFramework.Instance.ViewportManager
                .ActiveViewport)
            {
                SyncCameraRadioButtons();
            }
        }

        private void OnViewportRenderingModeChanged(MViewport pmViewport,
            IRenderingMode pmRenderingMode)
        {
            if (pmViewport == MFramework.Instance.ViewportManager
                .ActiveViewport)
            {
                SyncRenderingMode();
            }
        }

        private void OnCameraAddedToManager(MEntity pmCamera)
        {
            PopulateSceneCameras();
            SyncSceneCamera();
        }

        private void OnCameraRemovedFromManager(MEntity pmCamera)
        {
            PopulateSceneCameras();
            SyncSceneCamera();
        }

        public void OnEntityNameChanged(MEntity pmEntity, 
            string strOldName, bool bInBatch)
        {
            bool bIsCamera = false;
            uint cameraCount = 
                MFramework.Instance.CameraManager.SceneCameraCount;

            for(uint i = 0; i < cameraCount; i++)
            {
                MEntity sceneCamera = 
                    MFramework.Instance.CameraManager.GetSceneCameraEntity(i);
                if (pmEntity == sceneCamera)
                {
                    bIsCamera = true;
                    break;
                }
            }

            if (bIsCamera)
            {
                PopulateSceneCameras();
                SyncSceneCamera();
                uint iViewportCount = 
                    MFramework.Instance.ViewportManager.ViewportCount;
                for (uint i = 0; i < iViewportCount; i++)
                {
                    MViewport pmViewport = 
                        MFramework.Instance.ViewportManager.GetViewport(i);
                    if (pmViewport.CameraEntity == pmEntity)
                    {
                        pmViewport.UpdateScreenConsole();
                    }
              
                }
            }
        }

        #endregion

        private void PopulateSceneCameras()
        {
            m_bIgnoreSceneCameraIndexChange = true;

            m_cbSceneCamera.Items.Clear();

            MCameraManager pmCameraManager = MFramework.Instance
                .CameraManager;

            for (uint ui = 0; ui < pmCameraManager.SceneCameraCount; ui++)
            {
                m_cbSceneCamera.Items.Add(pmCameraManager
                    .GetSceneCameraEntity(ui));
            }

            if (m_cbSceneCamera.Items.Count > 0)
            {
                m_cbSceneCamera.Enabled = true;
                m_rbSceneCamera.Enabled = true;
            }
            else
            {
                m_cbSceneCamera.Enabled = false;
                m_rbSceneCamera.Enabled = false;
            }

            m_bIgnoreSceneCameraIndexChange = false;
        }

        private bool SyncSceneCamera()
        {
            bool bCameraFound = false;

            m_bIgnoreSceneCameraIndexChange = true;

            if (m_cbSceneCamera.Items.Count > 0)
            {
                int iIndex = m_cbSceneCamera.Items.IndexOf(MFramework.Instance
                    .ViewportManager.ActiveViewport.CameraEntity);
                if (iIndex > -1)
                {
                    m_cbSceneCamera.SelectedIndex = iIndex;
                    bCameraFound = true;
                }

                if (m_cbSceneCamera.SelectedIndex == -1)
                {
                    m_cbSceneCamera.SelectedIndex = 0;
                }
            }

            m_bIgnoreSceneCameraIndexChange = false;

            return bCameraFound;
        }

        private void SyncRenderingMode()
        {
            m_bIgnoreRenderingModeIndexChange = true;

            for (int i = 0; i < m_cbRenderingMode.Items.Count; i++)
            {
                string strRenderingMode = m_cbRenderingMode.Items[i] as
                    string;
                IRenderingMode pmRenderingMode = MFramework.Instance
                    .ViewportManager.ActiveViewport.RenderingMode;
                if (pmRenderingMode != null &&
                    strRenderingMode.Equals(pmRenderingMode.Name))
                {
                    m_cbRenderingMode.SelectedIndex = i;
                }
            }

            m_bIgnoreRenderingModeIndexChange = false;
        }

        private void SyncCameraRadioButtons()
        {
            m_bIgnoreRadioButtonClick = true;

            MCameraManager pmManager = MFramework.Instance.CameraManager;

            MViewport pmActiveViewport = MFramework.Instance.ViewportManager
                .ActiveViewport;
            MEntity pmActiveCamera = pmActiveViewport.CameraEntity;
            
            if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.Perspective))
            {
                m_rbPerspective.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.User))
            {
                m_rbUser.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.PositiveX))
            {
                m_rbPosX.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.NegativeX))
            {
                m_rbNegX.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.PositiveY))
            {
                m_rbPosY.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.NegativeY))
            {
                m_rbNegY.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.PositiveZ))
            {
                m_rbPosZ.Checked = true;
            }
            else if (pmActiveCamera == pmManager.GetStandardCameraEntity(
                pmActiveViewport, MCameraManager.StandardCamera.NegativeZ))
            {
                m_rbNegZ.Checked = true;
            }
            else
            {
                if (SyncSceneCamera())
                {
                    m_rbSceneCamera.Checked = true;
                }
            }

            m_bIgnoreRadioButtonClick = false;
        }

        private void m_cbRenderingMode_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRenderingModeIndexChange)
            {
                Debug.Assert(m_cbRenderingMode.SelectedItem != null,
                    "No item selected in combo box!");

                string strRenderingMode = m_cbRenderingMode.SelectedItem as
                    string;
                Debug.Assert(strRenderingMode != null, "Combo box has a " +
                    "non-string item!");

                IRenderingMode pmRenderingMode = RenderingModeService
                    .GetRenderingModeByName(strRenderingMode);
                Debug.Assert(pmRenderingMode != null, "Rendering mode not" +
                    "found!");

                MFramework.Instance.ViewportManager.ActiveViewport
                    .RenderingMode = pmRenderingMode;
            }
        }

        private void SetStandardCamera(
            MCameraManager.StandardCamera eType)
        {
            MFramework.Instance.CameraManager.TransitionViewportToCamera(
                MFramework.Instance.ViewportManager.ActiveViewport,
                MFramework.Instance.CameraManager.GetStandardCameraEntity(
                MFramework.Instance.ViewportManager.ActiveViewport, eType));
        }

        private void SetSceneCamera(MEntity pmCamera)
        {
            MFramework.Instance.CameraManager.TransitionViewportToCamera(
                MFramework.Instance.ViewportManager.ActiveViewport, pmCamera);
        }

        private void m_rbPerspective_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbPerspective.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.Perspective);
            }
        }

        private void m_rbUser_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbUser.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.User);
            }
        }

        private void m_rbPosX_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbPosX.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.PositiveX);
            }
        }

        private void m_rbNegX_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbNegX.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.NegativeX);
            }
        }

        private void m_rbPosY_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbPosY.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.PositiveY);
            }
        }

        private void m_rbNegY_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbNegY.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.NegativeY);
            }
        }

        private void m_rbPosZ_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbPosZ.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.PositiveZ);
            }
        }

        private void m_rbNegZ_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbNegZ.Checked)
            {
                SetStandardCamera(MCameraManager.StandardCamera.NegativeZ);
            }
        }

        private void m_rbSceneCamera_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreRadioButtonClick && m_rbSceneCamera.Checked)
            {
                if (m_cbSceneCamera.SelectedItem != null)
                {
                    SetSceneCamera((MEntity) m_cbSceneCamera.SelectedItem);
                }
            }
        }

        private void m_cbSceneCamera_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bIgnoreSceneCameraIndexChange)
            {
                if (m_cbSceneCamera.SelectedItem != null)
                {
                    SetSceneCamera((MEntity) m_cbSceneCamera.SelectedItem);
                }
            }
        }

    }
}

