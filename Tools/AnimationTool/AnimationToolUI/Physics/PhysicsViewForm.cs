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
using System.Diagnostics;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for PhysicsViewForm.
    /// </summary>
    public class PhysicsViewForm : DockContent
    {
#if EE_PHYSX_BUILD

        private System.Windows.Forms.GroupBox m_gbPhysicalSequences;
        private System.Windows.Forms.GroupBox m_gbGravity;
        private System.Windows.Forms.GroupBox m_gbDebugRendering;
        private System.Windows.Forms.GroupBox m_gbApplyForces;
        private System.Windows.Forms.ListBox m_lbSequences;
        private System.Windows.Forms.Label m_lblGravityX;
        private System.Windows.Forms.Label m_lblGravityY;
        private System.Windows.Forms.Label m_lblGravityZ;
        private System.Windows.Forms.CheckBox m_cbDebugEnabled;
        private System.Windows.Forms.Label m_lblDebugScale;
        private System.Windows.Forms.Button m_btnClearForces;
        private System.Windows.Forms.Label m_lblForceStrength;
        private System.Windows.Forms.GroupBox m_gbSettings;
        private System.Windows.Forms.Label m_lblTimestep;
        private System.Windows.Forms.NumericUpDown m_nudDebugScale;
        private System.Windows.Forms.NumericUpDown m_nudGravityX;
        private System.Windows.Forms.NumericUpDown m_nudGravityY;
        private System.Windows.Forms.NumericUpDown m_nudGravityZ;
        private System.Windows.Forms.NumericUpDown m_nudTimestep;
        private System.Windows.Forms.NumericUpDown m_nudForceStrength;
        private System.Windows.Forms.ImageList ilPhysics;
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.NumericUpDown m_nudGroundFriction;
        private System.Windows.Forms.Label m_lblGroundFriction;
        private System.Windows.Forms.NumericUpDown m_nudGroundBounce;
        private System.Windows.Forms.Label m_lblGroundBounce;
        private System.Windows.Forms.GroupBox m_gbGroundPlane;
        private System.Windows.Forms.Label m_lblDummy;
        private System.Windows.Forms.Button m_btnAdd;
        private System.Windows.Forms.ToolBar m_tbForceApply;
        private System.Windows.Forms.Button m_btnRestoreDefaults;
        private System.Windows.Forms.Button m_btnSaveDefaults;
        private System.Windows.Forms.ToolBarButton m_tbbForceApply;

        public PhysicsViewForm()
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PhysicsViewForm));
            this.m_gbPhysicalSequences = new System.Windows.Forms.GroupBox();
            this.m_btnAdd = new System.Windows.Forms.Button();
            this.m_lbSequences = new System.Windows.Forms.ListBox();
            this.m_gbGravity = new System.Windows.Forms.GroupBox();
            this.m_nudGravityZ = new System.Windows.Forms.NumericUpDown();
            this.m_nudGravityY = new System.Windows.Forms.NumericUpDown();
            this.m_nudGravityX = new System.Windows.Forms.NumericUpDown();
            this.m_lblGravityX = new System.Windows.Forms.Label();
            this.m_lblGravityY = new System.Windows.Forms.Label();
            this.m_lblGravityZ = new System.Windows.Forms.Label();
            this.m_gbDebugRendering = new System.Windows.Forms.GroupBox();
            this.m_nudDebugScale = new System.Windows.Forms.NumericUpDown();
            this.m_lblDebugScale = new System.Windows.Forms.Label();
            this.m_cbDebugEnabled = new System.Windows.Forms.CheckBox();
            this.m_gbApplyForces = new System.Windows.Forms.GroupBox();
            this.m_nudForceStrength = new System.Windows.Forms.NumericUpDown();
            this.m_lblForceStrength = new System.Windows.Forms.Label();
            this.m_btnClearForces = new System.Windows.Forms.Button();
            this.m_tbForceApply = new System.Windows.Forms.ToolBar();
            this.m_tbbForceApply = new System.Windows.Forms.ToolBarButton();
            this.ilPhysics = new System.Windows.Forms.ImageList(this.components);
            this.m_gbSettings = new System.Windows.Forms.GroupBox();
            this.m_nudTimestep = new System.Windows.Forms.NumericUpDown();
            this.m_lblTimestep = new System.Windows.Forms.Label();
            this.m_gbGroundPlane = new System.Windows.Forms.GroupBox();
            this.m_nudGroundBounce = new System.Windows.Forms.NumericUpDown();
            this.m_lblGroundBounce = new System.Windows.Forms.Label();
            this.m_nudGroundFriction = new System.Windows.Forms.NumericUpDown();
            this.m_lblGroundFriction = new System.Windows.Forms.Label();
            this.m_lblDummy = new System.Windows.Forms.Label();
            this.m_btnRestoreDefaults = new System.Windows.Forms.Button();
            this.m_btnSaveDefaults = new System.Windows.Forms.Button();
            this.m_gbPhysicalSequences.SuspendLayout();
            this.m_gbGravity.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGravityZ)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGravityY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGravityX)).BeginInit();
            this.m_gbDebugRendering.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudDebugScale)).BeginInit();
            this.m_gbApplyForces.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudForceStrength)).BeginInit();
            this.m_gbSettings.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudTimestep)).BeginInit();
            this.m_gbGroundPlane.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGroundBounce)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGroundFriction)).BeginInit();
            this.SuspendLayout();
            // 
            // m_gbPhysicalSequences
            // 
            this.m_gbPhysicalSequences.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_gbPhysicalSequences.Controls.Add(this.m_btnAdd);
            this.m_gbPhysicalSequences.Controls.Add(this.m_lbSequences);
            this.m_gbPhysicalSequences.Location = new System.Drawing.Point(10, 14);
            this.m_gbPhysicalSequences.Name = "m_gbPhysicalSequences";
            this.m_gbPhysicalSequences.Size = new System.Drawing.Size(297, 165);
            this.m_gbPhysicalSequences.TabIndex = 0;
            this.m_gbPhysicalSequences.TabStop = false;
            this.m_gbPhysicalSequences.Text = "Physical Sequences";
            // 
            // m_btnAdd
            // 
            this.m_btnAdd.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnAdd.Location = new System.Drawing.Point(91, 126);
            this.m_btnAdd.Name = "m_btnAdd";
            this.m_btnAdd.Size = new System.Drawing.Size(115, 27);
            this.m_btnAdd.TabIndex = 1;
            this.m_btnAdd.Text = "Add/Remove";
            this.m_btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // m_lbSequences
            // 
            this.m_lbSequences.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbSequences.IntegralHeight = false;
            this.m_lbSequences.ItemHeight = 16;
            this.m_lbSequences.Location = new System.Drawing.Point(10, 28);
            this.m_lbSequences.Name = "m_lbSequences";
            this.m_lbSequences.Size = new System.Drawing.Size(278, 89);
            this.m_lbSequences.TabIndex = 0;
            // 
            // m_gbGravity
            // 
            this.m_gbGravity.Controls.Add(this.m_nudGravityZ);
            this.m_gbGravity.Controls.Add(this.m_nudGravityY);
            this.m_gbGravity.Controls.Add(this.m_nudGravityX);
            this.m_gbGravity.Controls.Add(this.m_lblGravityX);
            this.m_gbGravity.Controls.Add(this.m_lblGravityY);
            this.m_gbGravity.Controls.Add(this.m_lblGravityZ);
            this.m_gbGravity.Location = new System.Drawing.Point(173, 18);
            this.m_gbGravity.Name = "m_gbGravity";
            this.m_gbGravity.Size = new System.Drawing.Size(125, 130);
            this.m_gbGravity.TabIndex = 2;
            this.m_gbGravity.TabStop = false;
            this.m_gbGravity.Text = "Gravity";
            // 
            // m_nudGravityZ
            // 
            this.m_nudGravityZ.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_nudGravityZ.DecimalPlaces = 2;
            this.m_nudGravityZ.Location = new System.Drawing.Point(29, 92);
            this.m_nudGravityZ.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.m_nudGravityZ.Minimum = new decimal(new int[] {
            100000,
            0,
            0,
            -2147483648});
            this.m_nudGravityZ.Name = "m_nudGravityZ";
            this.m_nudGravityZ.Size = new System.Drawing.Size(86, 22);
            this.m_nudGravityZ.TabIndex = 2;
            this.m_nudGravityZ.ValueChanged += new System.EventHandler(this.nudGravityZ_Changed);
            this.m_nudGravityZ.Leave += new System.EventHandler(this.nudGravityZ_Changed);
            // 
            // m_nudGravityY
            // 
            this.m_nudGravityY.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_nudGravityY.DecimalPlaces = 2;
            this.m_nudGravityY.Location = new System.Drawing.Point(29, 55);
            this.m_nudGravityY.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.m_nudGravityY.Minimum = new decimal(new int[] {
            100000,
            0,
            0,
            -2147483648});
            this.m_nudGravityY.Name = "m_nudGravityY";
            this.m_nudGravityY.Size = new System.Drawing.Size(86, 22);
            this.m_nudGravityY.TabIndex = 1;
            this.m_nudGravityY.ValueChanged += new System.EventHandler(this.nudGravityY_Changed);
            this.m_nudGravityY.Leave += new System.EventHandler(this.nudGravityY_Changed);
            // 
            // m_nudGravityX
            // 
            this.m_nudGravityX.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_nudGravityX.DecimalPlaces = 2;
            this.m_nudGravityX.Location = new System.Drawing.Point(29, 18);
            this.m_nudGravityX.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.m_nudGravityX.Minimum = new decimal(new int[] {
            100000,
            0,
            0,
            -2147483648});
            this.m_nudGravityX.Name = "m_nudGravityX";
            this.m_nudGravityX.Size = new System.Drawing.Size(86, 22);
            this.m_nudGravityX.TabIndex = 0;
            this.m_nudGravityX.ValueChanged += new System.EventHandler(this.nudGravityX_Changed);
            this.m_nudGravityX.Leave += new System.EventHandler(this.nudGravityX_Changed);
            // 
            // m_lblGravityX
            // 
            this.m_lblGravityX.AutoSize = true;
            this.m_lblGravityX.Location = new System.Drawing.Point(10, 28);
            this.m_lblGravityX.Name = "m_lblGravityX";
            this.m_lblGravityX.Size = new System.Drawing.Size(17, 17);
            this.m_lblGravityX.TabIndex = 0;
            this.m_lblGravityX.Text = "X";
            // 
            // m_lblGravityY
            // 
            this.m_lblGravityY.AutoSize = true;
            this.m_lblGravityY.Location = new System.Drawing.Point(10, 65);
            this.m_lblGravityY.Name = "m_lblGravityY";
            this.m_lblGravityY.Size = new System.Drawing.Size(17, 17);
            this.m_lblGravityY.TabIndex = 1;
            this.m_lblGravityY.Text = "Y";
            // 
            // m_lblGravityZ
            // 
            this.m_lblGravityZ.AutoSize = true;
            this.m_lblGravityZ.Location = new System.Drawing.Point(10, 102);
            this.m_lblGravityZ.Name = "m_lblGravityZ";
            this.m_lblGravityZ.Size = new System.Drawing.Size(17, 17);
            this.m_lblGravityZ.TabIndex = 2;
            this.m_lblGravityZ.Text = "Z";
            // 
            // m_gbDebugRendering
            // 
            this.m_gbDebugRendering.Controls.Add(this.m_nudDebugScale);
            this.m_gbDebugRendering.Controls.Add(this.m_lblDebugScale);
            this.m_gbDebugRendering.Controls.Add(this.m_cbDebugEnabled);
            this.m_gbDebugRendering.Location = new System.Drawing.Point(10, 18);
            this.m_gbDebugRendering.Name = "m_gbDebugRendering";
            this.m_gbDebugRendering.Size = new System.Drawing.Size(153, 93);
            this.m_gbDebugRendering.TabIndex = 1;
            this.m_gbDebugRendering.TabStop = false;
            this.m_gbDebugRendering.Text = "Debug Rendering";
            // 
            // m_nudDebugScale
            // 
            this.m_nudDebugScale.DecimalPlaces = 2;
            this.m_nudDebugScale.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.m_nudDebugScale.Location = new System.Drawing.Point(58, 55);
            this.m_nudDebugScale.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.m_nudDebugScale.Name = "m_nudDebugScale";
            this.m_nudDebugScale.Size = new System.Drawing.Size(86, 22);
            this.m_nudDebugScale.TabIndex = 1;
            this.m_nudDebugScale.ValueChanged += new System.EventHandler(this.nudDebugScale_Changed);
            this.m_nudDebugScale.Leave += new System.EventHandler(this.nudDebugScale_Changed);
            // 
            // m_lblDebugScale
            // 
            this.m_lblDebugScale.AutoSize = true;
            this.m_lblDebugScale.Location = new System.Drawing.Point(10, 65);
            this.m_lblDebugScale.Name = "m_lblDebugScale";
            this.m_lblDebugScale.Size = new System.Drawing.Size(47, 17);
            this.m_lblDebugScale.TabIndex = 1;
            this.m_lblDebugScale.Text = "Scale:";
            // 
            // m_cbDebugEnabled
            // 
            this.m_cbDebugEnabled.Location = new System.Drawing.Point(38, 18);
            this.m_cbDebugEnabled.Name = "m_cbDebugEnabled";
            this.m_cbDebugEnabled.Size = new System.Drawing.Size(87, 28);
            this.m_cbDebugEnabled.TabIndex = 0;
            this.m_cbDebugEnabled.Text = "Enabled";
            this.m_cbDebugEnabled.Click += new System.EventHandler(this.cbDebugEnabled_Click);
            // 
            // m_gbApplyForces
            // 
            this.m_gbApplyForces.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_gbApplyForces.Controls.Add(this.m_nudForceStrength);
            this.m_gbApplyForces.Controls.Add(this.m_lblForceStrength);
            this.m_gbApplyForces.Controls.Add(this.m_btnClearForces);
            this.m_gbApplyForces.Controls.Add(this.m_tbForceApply);
            this.m_gbApplyForces.Location = new System.Drawing.Point(10, 198);
            this.m_gbApplyForces.Name = "m_gbApplyForces";
            this.m_gbApplyForces.Size = new System.Drawing.Size(163, 101);
            this.m_gbApplyForces.TabIndex = 1;
            this.m_gbApplyForces.TabStop = false;
            this.m_gbApplyForces.Text = "Apply Forces";
            // 
            // m_nudForceStrength
            // 
            this.m_nudForceStrength.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_nudForceStrength.DecimalPlaces = 1;
            this.m_nudForceStrength.Location = new System.Drawing.Point(67, 65);
            this.m_nudForceStrength.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.m_nudForceStrength.Name = "m_nudForceStrength";
            this.m_nudForceStrength.Size = new System.Drawing.Size(87, 22);
            this.m_nudForceStrength.TabIndex = 4;
            this.m_nudForceStrength.ValueChanged += new System.EventHandler(this.nudForceStrength_Changed);
            // 
            // m_lblForceStrength
            // 
            this.m_lblForceStrength.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lblForceStrength.AutoSize = true;
            this.m_lblForceStrength.Location = new System.Drawing.Point(10, 74);
            this.m_lblForceStrength.Name = "m_lblForceStrength";
            this.m_lblForceStrength.Size = new System.Drawing.Size(66, 17);
            this.m_lblForceStrength.TabIndex = 3;
            this.m_lblForceStrength.Text = "Strength:";
            // 
            // m_btnClearForces
            // 
            this.m_btnClearForces.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnClearForces.Location = new System.Drawing.Point(96, 28);
            this.m_btnClearForces.Name = "m_btnClearForces";
            this.m_btnClearForces.Size = new System.Drawing.Size(58, 26);
            this.m_btnClearForces.TabIndex = 2;
            this.m_btnClearForces.Text = "Clear";
            this.m_btnClearForces.Click += new System.EventHandler(this.btnClearForces_Click);
            // 
            // m_tbForceApply
            // 
            this.m_tbForceApply.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.m_tbbForceApply});
            this.m_tbForceApply.Divider = false;
            this.m_tbForceApply.DropDownArrows = true;
            this.m_tbForceApply.ImageList = this.ilPhysics;
            this.m_tbForceApply.Location = new System.Drawing.Point(3, 18);
            this.m_tbForceApply.Name = "m_tbForceApply";
            this.m_tbForceApply.ShowToolTips = true;
            this.m_tbForceApply.Size = new System.Drawing.Size(157, 34);
            this.m_tbForceApply.TabIndex = 1;
            this.m_tbForceApply.Wrappable = false;
            this.m_tbForceApply.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.tbForceApply_ButtonClick);
            // 
            // m_tbbForceApply
            // 
            this.m_tbbForceApply.ImageIndex = 0;
            this.m_tbbForceApply.Name = "m_tbbForceApply";
            // 
            // ilPhysics
            // 
            this.ilPhysics.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilPhysics.ImageStream")));
            this.ilPhysics.TransparentColor = System.Drawing.Color.Fuchsia;
            this.ilPhysics.Images.SetKeyName(0, "");
            // 
            // m_gbSettings
            // 
            this.m_gbSettings.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_gbSettings.Controls.Add(this.m_nudTimestep);
            this.m_gbSettings.Controls.Add(this.m_gbGravity);
            this.m_gbSettings.Controls.Add(this.m_gbDebugRendering);
            this.m_gbSettings.Controls.Add(this.m_lblTimestep);
            this.m_gbSettings.Location = new System.Drawing.Point(10, 309);
            this.m_gbSettings.Name = "m_gbSettings";
            this.m_gbSettings.Size = new System.Drawing.Size(307, 151);
            this.m_gbSettings.TabIndex = 3;
            this.m_gbSettings.TabStop = false;
            this.m_gbSettings.Text = "Settings";
            // 
            // m_nudTimestep
            // 
            this.m_nudTimestep.DecimalPlaces = 4;
            this.m_nudTimestep.Increment = new decimal(new int[] {
            5,
            0,
            0,
            196608});
            this.m_nudTimestep.Location = new System.Drawing.Point(77, 120);
            this.m_nudTimestep.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.m_nudTimestep.Name = "m_nudTimestep";
            this.m_nudTimestep.Size = new System.Drawing.Size(86, 22);
            this.m_nudTimestep.TabIndex = 3;
            this.m_nudTimestep.ValueChanged += new System.EventHandler(this.nudTimestep_Changed);
            this.m_nudTimestep.Leave += new System.EventHandler(this.nudTimestep_Changed);
            // 
            // m_lblTimestep
            // 
            this.m_lblTimestep.AutoSize = true;
            this.m_lblTimestep.Location = new System.Drawing.Point(10, 129);
            this.m_lblTimestep.Name = "m_lblTimestep";
            this.m_lblTimestep.Size = new System.Drawing.Size(70, 17);
            this.m_lblTimestep.TabIndex = 2;
            this.m_lblTimestep.Text = "Timestep:";
            // 
            // m_gbGroundPlane
            // 
            this.m_gbGroundPlane.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_gbGroundPlane.Controls.Add(this.m_nudGroundBounce);
            this.m_gbGroundPlane.Controls.Add(this.m_lblGroundBounce);
            this.m_gbGroundPlane.Controls.Add(this.m_nudGroundFriction);
            this.m_gbGroundPlane.Controls.Add(this.m_lblGroundFriction);
            this.m_gbGroundPlane.Location = new System.Drawing.Point(182, 198);
            this.m_gbGroundPlane.Name = "m_gbGroundPlane";
            this.m_gbGroundPlane.Size = new System.Drawing.Size(135, 101);
            this.m_gbGroundPlane.TabIndex = 2;
            this.m_gbGroundPlane.TabStop = false;
            this.m_gbGroundPlane.Text = "Ground Plane";
            // 
            // m_nudGroundBounce
            // 
            this.m_nudGroundBounce.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_nudGroundBounce.DecimalPlaces = 2;
            this.m_nudGroundBounce.Increment = new decimal(new int[] {
            5,
            0,
            0,
            131072});
            this.m_nudGroundBounce.Location = new System.Drawing.Point(67, 65);
            this.m_nudGroundBounce.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.m_nudGroundBounce.Name = "m_nudGroundBounce";
            this.m_nudGroundBounce.Size = new System.Drawing.Size(58, 22);
            this.m_nudGroundBounce.TabIndex = 7;
            this.m_nudGroundBounce.Value = new decimal(new int[] {
            3,
            0,
            0,
            65536});
            this.m_nudGroundBounce.ValueChanged += new System.EventHandler(this.nudGroundBounce_Changed);
            // 
            // m_lblGroundBounce
            // 
            this.m_lblGroundBounce.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lblGroundBounce.AutoSize = true;
            this.m_lblGroundBounce.Location = new System.Drawing.Point(10, 74);
            this.m_lblGroundBounce.Name = "m_lblGroundBounce";
            this.m_lblGroundBounce.Size = new System.Drawing.Size(60, 17);
            this.m_lblGroundBounce.TabIndex = 6;
            this.m_lblGroundBounce.Text = "Bounce:";
            // 
            // m_nudGroundFriction
            // 
            this.m_nudGroundFriction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_nudGroundFriction.DecimalPlaces = 2;
            this.m_nudGroundFriction.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.m_nudGroundFriction.Location = new System.Drawing.Point(67, 28);
            this.m_nudGroundFriction.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.m_nudGroundFriction.Name = "m_nudGroundFriction";
            this.m_nudGroundFriction.Size = new System.Drawing.Size(58, 22);
            this.m_nudGroundFriction.TabIndex = 5;
            this.m_nudGroundFriction.Value = new decimal(new int[] {
            3,
            0,
            0,
            65536});
            this.m_nudGroundFriction.ValueChanged += new System.EventHandler(this.nudGroundFriction_Changed);
            // 
            // m_lblGroundFriction
            // 
            this.m_lblGroundFriction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lblGroundFriction.AutoSize = true;
            this.m_lblGroundFriction.Location = new System.Drawing.Point(10, 37);
            this.m_lblGroundFriction.Name = "m_lblGroundFriction";
            this.m_lblGroundFriction.Size = new System.Drawing.Size(58, 17);
            this.m_lblGroundFriction.TabIndex = 4;
            this.m_lblGroundFriction.Text = "Friction:";
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.Enabled = false;
            this.m_lblDummy.Location = new System.Drawing.Point(307, 498);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(10, 10);
            this.m_lblDummy.TabIndex = 4;
            // 
            // m_btnRestoreDefaults
            // 
            this.m_btnRestoreDefaults.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_btnRestoreDefaults.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnRestoreDefaults.Location = new System.Drawing.Point(178, 469);
            this.m_btnRestoreDefaults.Name = "m_btnRestoreDefaults";
            this.m_btnRestoreDefaults.Size = new System.Drawing.Size(115, 28);
            this.m_btnRestoreDefaults.TabIndex = 5;
            this.m_btnRestoreDefaults.Text = "Restore Defaults";
            this.m_btnRestoreDefaults.Click += new System.EventHandler(this.btnRestoreDefaults_Click);
            // 
            // m_btnSaveDefaults
            // 
            this.m_btnSaveDefaults.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_btnSaveDefaults.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnSaveDefaults.Location = new System.Drawing.Point(24, 469);
            this.m_btnSaveDefaults.Name = "m_btnSaveDefaults";
            this.m_btnSaveDefaults.Size = new System.Drawing.Size(125, 28);
            this.m_btnSaveDefaults.TabIndex = 4;
            this.m_btnSaveDefaults.Text = "Save As Defaults";
            this.m_btnSaveDefaults.Click += new System.EventHandler(this.btnSaveDefaults_Click);
            // 
            // PhysicsViewForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 15);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(301, 471);
            this.Controls.Add(this.m_btnRestoreDefaults);
            this.Controls.Add(this.m_btnSaveDefaults);
            this.Controls.Add(this.m_gbGroundPlane);
            this.Controls.Add(this.m_gbSettings);
            this.Controls.Add(this.m_gbApplyForces);
            this.Controls.Add(this.m_gbPhysicalSequences);
            this.Controls.Add(this.m_lblDummy);
            this.HideOnClose = true;
            this.Name = "PhysicsViewForm";
            this.TabText = "Physics View";
            this.Text = "Physics View";
            this.Load += new System.EventHandler(this.PhysicsViewForm_Load);
            this.m_gbPhysicalSequences.ResumeLayout(false);
            this.m_gbGravity.ResumeLayout(false);
            this.m_gbGravity.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGravityZ)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGravityY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGravityX)).EndInit();
            this.m_gbDebugRendering.ResumeLayout(false);
            this.m_gbDebugRendering.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudDebugScale)).EndInit();
            this.m_gbApplyForces.ResumeLayout(false);
            this.m_gbApplyForces.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudForceStrength)).EndInit();
            this.m_gbSettings.ResumeLayout(false);
            this.m_gbSettings.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudTimestep)).EndInit();
            this.m_gbGroundPlane.ResumeLayout(false);
            this.m_gbGroundPlane.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGroundBounce)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_nudGroundFriction)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        private void PhysicsViewForm_Load(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }
            
            m_tbbForceApply.Pushed = false;

            AppEvents.PhysicsSettingsChanged +=
                new AppEvents.EventHandler_PhysicsSettingsChanged(
                this.OnPhysicsSettingsChanged);
            MFramework.Instance.Animation.OnNewKFM +=
                new MAnimation.__Delegate_OnNewKFM(this.OnNewKFM);
            MFramework.Instance.Physics.OnAddPhysicsSequence +=
                new MPhysics.__Delegate_OnAddPhysicsSequence(
                this.OnPhysicsSequencesChanged);
            MFramework.Instance.Physics.OnRemovePhysicsSequence +=
                new MPhysics.__Delegate_OnRemovePhysicsSequence(
                this.OnPhysicsSequencesChanged);
            MFramework.Instance.Input.OnActiveUITypeChanged += 
                new MUIManager.__Delegate_OnActiveUITypeChanged(
                this.OnUITypeChanged);

            LoadFromPhysics();
            LoadFromProjectData();            
        }

        private void LoadFromProjectData()
        {
            m_nudGravityX.Value =
                (decimal)ProjectData.Instance.PhysicsGravityX;
            m_nudGravityY.Value =
                (decimal)ProjectData.Instance.PhysicsGravityY;
            m_nudGravityZ.Value =
                (decimal)ProjectData.Instance.PhysicsGravityZ;
            m_cbDebugEnabled.Checked =
                ProjectData.Instance.PhysicsDebugEnabled;
            m_nudDebugScale.Value =
                (decimal)ProjectData.Instance.PhysicsDebugScale;
            m_nudTimestep.Value =
                (decimal)ProjectData.Instance.PhysicsTimestep;            
            m_nudGroundFriction.Value =
                (decimal)ProjectData.Instance.PhysicsGroundFriction;
            m_nudGroundBounce.Value =
                (decimal)ProjectData.Instance.PhysicsGroundBounce;
            m_nudForceStrength.Value =
                (decimal)ProjectData.Instance.PhysicsForcePower;
        }

        private void LoadFromPhysics()
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }
            m_lbSequences.Items.Clear();
            ArrayList kSequences =
                MFramework.Instance.Physics.PhysicalSequences;
            foreach (MSequence seq in kSequences)
            {
                m_lbSequences.Items.Add(seq);
            }
            
            m_nudForceStrength.Value =
                (decimal)MFramework.Instance.Physics.ForceExponent;
        }
        
        private void btnAdd_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.Instance.Physics.ScenePresent)
            {
                return;
            }
            
            AddRemovePhysicsSequenceForm kForm =
                new AddRemovePhysicsSequenceForm();
            DialogResult eResult = kForm.ShowDialog();
            if (eResult == DialogResult.OK)
                kForm.DoIt();
        }
        
        private void tbForceApply_ButtonClick(object sender,
            System.Windows.Forms.ToolBarButtonClickEventArgs e)
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            if (e.Button == m_tbbForceApply)
            {
                m_tbbForceApply.Pushed = true;
                if (MFramework.Instance.Input.ActiveUIType != 
                    MUIState.UIType.PHYSICSFORCE)
                {
                    MFramework.Instance.Input.ActiveUIType = 
                        MUIState.UIType.PHYSICSFORCE;
                }
            }
        }
        
        private void btnClearForces_Click(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }
            MFramework.Instance.Physics.ClearForces();
        }
        
        private void cbDebugEnabled_Click(object sender, System.EventArgs e)
        {
            ProjectData.Instance.PhysicsDebugEnabled =
                m_cbDebugEnabled.Checked;
        }

        private void nudGroundFriction_Changed(
            object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudGroundFriction.Value;
            if (fNewValue != ProjectData.Instance.PhysicsGroundFriction)
                ProjectData.Instance.PhysicsGroundFriction = fNewValue;
        }

        private void nudGroundBounce_Changed(
            object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudGroundBounce.Value;
            if (fNewValue != ProjectData.Instance.PhysicsGroundBounce)
                ProjectData.Instance.PhysicsGroundBounce = fNewValue;
        }

        private void nudDebugScale_Changed(object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudDebugScale.Value;
            if (fNewValue != ProjectData.Instance.PhysicsDebugScale)
                ProjectData.Instance.PhysicsDebugScale = fNewValue;
        }

        private void nudTimestep_Changed(object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudTimestep.Value;
            if (fNewValue != ProjectData.Instance.PhysicsTimestep)
                ProjectData.Instance.PhysicsTimestep = fNewValue;
        }

        private void nudGravityX_Changed(object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudGravityX.Value;
            if (fNewValue != ProjectData.Instance.PhysicsGravityX)
                ProjectData.Instance.PhysicsGravityX = fNewValue;
        }

        private void nudGravityY_Changed(object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudGravityY.Value;
            if (fNewValue != ProjectData.Instance.PhysicsGravityY)
                ProjectData.Instance.PhysicsGravityY = fNewValue;
        }

        private void nudGravityZ_Changed(object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudGravityZ.Value;
            if (fNewValue != ProjectData.Instance.PhysicsGravityZ)
                ProjectData.Instance.PhysicsGravityZ = fNewValue;
        }

        private void nudForceStrength_Changed(
            object sender, System.EventArgs e)
        {
            float fNewValue = (float)m_nudForceStrength.Value;
            if (fNewValue != ProjectData.Instance.PhysicsForcePower)
                ProjectData.Instance.PhysicsForcePower = fNewValue;
        }

        private void btnSaveDefaults_Click(object sender, System.EventArgs e)
        {
            ProjectData.Instance.SavePhysicsRegistrySettings();
        }

        private void btnRestoreDefaults_Click(
            object sender, System.EventArgs e)
        {
            ProjectData.Instance.PhysicsGravityX =
                ProjectData.Instance.DefaultPhysicsGravityX;
            ProjectData.Instance.PhysicsGravityY =
                ProjectData.Instance.DefaultPhysicsGravityY;
            ProjectData.Instance.PhysicsGravityZ =
                ProjectData.Instance.DefaultPhysicsGravityZ;
            ProjectData.Instance.PhysicsDebugEnabled =
                ProjectData.Instance.DefaultPhysicsDebugEnabled;
            ProjectData.Instance.PhysicsDebugScale =
                ProjectData.Instance.DefaultPhysicsDebugScale;
            ProjectData.Instance.PhysicsTimestep =
                ProjectData.Instance.DefaultPhysicsTimestep;
            ProjectData.Instance.PhysicsGroundBounce =
                ProjectData.Instance.DefaultPhysicsGroundBounce;
            ProjectData.Instance.PhysicsGroundFriction =
                ProjectData.Instance.DefaultPhysicsGroundFriction;
                
            LoadFromProjectData();
        }
        
        private void OnPhysicsSettingsChanged()
        {
            LoadFromProjectData();            
        }

        private void OnNewKFM()
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }
            m_lbSequences.Items.Clear();
        }

        private void OnPhysicsSequencesChanged(MSequence changedSeq)
        {
            try 
            {
                // To support design view of this control in Visual Studio.
                if (!MFramework.InstanceIsValid())
                {
                    return;
                }

                LoadFromPhysics();
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void OnUITypeChanged(MUIState.UIType eType)
        {
            switch(eType)
            {
                case MUIState.UIType.ORBIT:
                case MUIState.UIType.FREELOOK:
                case MUIState.UIType.BACKGROUNDHEIGHT:
                    m_tbbForceApply.Pushed = false;
                    break;
            }
        }

#endif  // #if EE_PHYSX_BUILD
    }
}
