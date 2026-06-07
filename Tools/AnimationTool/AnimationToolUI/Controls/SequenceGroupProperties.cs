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
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for SequenceGroupProperties.
    /// </summary>
    public class SequenceGroupProperties : 
        System.Windows.Forms.UserControl, IPostShow
    {
        #region Delegates

        private MAnimation.__Delegate_OnSequenceGroupModified 
            m_OnSequenceGroupModified = null;
        private AppEvents.EventHandler_KeystrokeBindingsChanged 
            m_KeystrokeBindingsChanged = null;
        private AnimationToolUI.NumericSlider nsEaseOut;
        private AnimationToolUI.NumericSlider nsEaseIn;
        private System.Windows.Forms.Label lblEaseOut;
        private System.Windows.Forms.Label lblEaseIn;
        private AppEvents.EventHandler_ToggleSequence m_ToggleSequence = null;

        public void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_OnSequenceGroupModified = 
                new MAnimation.__Delegate_OnSequenceGroupModified(
                OnSequenceGroupModified);
            m_KeystrokeBindingsChanged = 
                new AppEvents.EventHandler_KeystrokeBindingsChanged(
                OnKeystrokeBindingsChanged);
            m_ToggleSequence = 
                new AppEvents.EventHandler_ToggleSequence(
                OnToggleSequence);

            // Register delegates.
            MFramework.Instance.Animation.OnSequenceGroupModified += 
                m_OnSequenceGroupModified;
            AppEvents.KeystrokeBindingsChanged += m_KeystrokeBindingsChanged;
            AppEvents.ToggleSequence += m_ToggleSequence;
        }

        public void UnregisterDelegates()
        {
            if (m_OnSequenceGroupModified != null)
                MFramework.Instance.Animation.OnSequenceGroupModified -= 
                    m_OnSequenceGroupModified;
            if (m_KeystrokeBindingsChanged != null)
                AppEvents.KeystrokeBindingsChanged -= 
                    m_KeystrokeBindingsChanged;
            if (m_ToggleSequence != null)
                AppEvents.ToggleSequence -= m_ToggleSequence;
        }

        #endregion

        private MSequenceGroup m_SeqGrp;
        private Size m_szDefaultSizeOfPanel;
        private System.Windows.Forms.Label lblKeyAssignment;
        private System.Windows.Forms.Panel pnlActive;
        private AnimationToolUI.SequenceGroupMixer gridActive;
        private System.Windows.Forms.TextBox tbActiveSeq;
        private OOGroup.Windows.Forms.ImageButton btnRemoveSequence;
        private System.Windows.Forms.ImageList ilPlusMinus;
        private OOGroup.Windows.Forms.ImageButton btnAddSequence;
        private System.Windows.Forms.Panel pnlProps;
        private AnimationToolUI.NumericSlider nsWeight;
        private AnimationToolUI.NumericSlider nsPriority;
        private System.Windows.Forms.Label lblSyncToSequence;
        private System.Windows.Forms.ComboBox cbSyncToSequence;
        private System.Windows.Forms.Label lblWeight;
        private System.Windows.Forms.Label lblPriority;
        private System.Windows.Forms.TextBox tbKeyAssignment;
        private System.Windows.Forms.TextBox tbProperties;
        private System.Windows.Forms.Splitter splitActive;
        private System.Windows.Forms.ToolTip ttProperties;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public SequenceGroupProperties(MSequenceGroup SeqGrp)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_SeqGrp = SeqGrp;
        }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                NumInstances--;
                Debug.Assert(NumInstances == 0, 
                    "There should only ever be one un-disposed" + 
                    " version of this class in memory at a time.");
                UnregisterDelegates();
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(SequenceGroupProperties));
            this.pnlActive = new System.Windows.Forms.Panel();
            this.gridActive = new AnimationToolUI.SequenceGroupMixer();
            this.tbActiveSeq = new System.Windows.Forms.TextBox();
            this.btnRemoveSequence = new OOGroup.Windows.Forms.ImageButton();
            this.ilPlusMinus = new System.Windows.Forms.ImageList(this.components);
            this.btnAddSequence = new OOGroup.Windows.Forms.ImageButton();
            this.pnlProps = new System.Windows.Forms.Panel();
            this.nsEaseOut = new AnimationToolUI.NumericSlider();
            this.nsEaseIn = new AnimationToolUI.NumericSlider();
            this.nsWeight = new AnimationToolUI.NumericSlider();
            this.nsPriority = new AnimationToolUI.NumericSlider();
            this.lblSyncToSequence = new System.Windows.Forms.Label();
            this.cbSyncToSequence = new System.Windows.Forms.ComboBox();
            this.lblEaseOut = new System.Windows.Forms.Label();
            this.lblEaseIn = new System.Windows.Forms.Label();
            this.lblWeight = new System.Windows.Forms.Label();
            this.lblPriority = new System.Windows.Forms.Label();
            this.tbKeyAssignment = new System.Windows.Forms.TextBox();
            this.lblKeyAssignment = new System.Windows.Forms.Label();
            this.tbProperties = new System.Windows.Forms.TextBox();
            this.splitActive = new System.Windows.Forms.Splitter();
            this.ttProperties = new System.Windows.Forms.ToolTip(this.components);
            this.pnlActive.SuspendLayout();
            this.pnlProps.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlActive
            // 
            this.pnlActive.Controls.Add(this.gridActive);
            this.pnlActive.Controls.Add(this.tbActiveSeq);
            this.pnlActive.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlActive.Location = new System.Drawing.Point(0, 0);
            this.pnlActive.Name = "pnlActive";
            this.pnlActive.Size = new System.Drawing.Size(208, 475);
            this.pnlActive.TabIndex = 0;
            // 
            // gridActive
            // 
            this.gridActive.AllowDrop = true;
            this.gridActive.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.gridActive.AutoScroll = true;
            this.gridActive.DockPadding.All = 2;
            this.gridActive.Location = new System.Drawing.Point(0, 24);
            this.gridActive.Name = "gridActive";
            this.gridActive.Size = new System.Drawing.Size(208, 144);
            this.gridActive.TabIndex = 1;
            this.ttProperties.SetToolTip(this.gridActive, "Click on a Sequence to activate or deactivate it.");
            this.gridActive.Load += new System.EventHandler(this.gridActive_Load);
            this.gridActive.KeyDown += new System.Windows.Forms.KeyEventHandler(this.gridActive_KeyDown);
            // 
            // tbActiveSeq
            // 
            this.tbActiveSeq.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbActiveSeq.BackColor = System.Drawing.SystemColors.Info;
            this.tbActiveSeq.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbActiveSeq.Location = new System.Drawing.Point(0, 0);
            this.tbActiveSeq.Name = "tbActiveSeq";
            this.tbActiveSeq.ReadOnly = true;
            this.tbActiveSeq.Size = new System.Drawing.Size(208, 20);
            this.tbActiveSeq.TabIndex = 0;
            this.tbActiveSeq.TabStop = false;
            this.tbActiveSeq.Text = "Active Status of Sequences";
            this.tbActiveSeq.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // btnRemoveSequence
            // 
            this.btnRemoveSequence.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnRemoveSequence.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnRemoveSequence.ImageIndex = 1;
            this.btnRemoveSequence.ImageList = this.ilPlusMinus;
            this.btnRemoveSequence.Location = new System.Drawing.Point(108, 16);
            this.btnRemoveSequence.Name = "btnRemoveSequence";
            this.btnRemoveSequence.Size = new System.Drawing.Size(48, 32);
            this.btnRemoveSequence.TabIndex = 1;
            this.ttProperties.SetToolTip(this.btnRemoveSequence, "Remove the currently selected sequence from this sequence group.");
            this.btnRemoveSequence.Click += new System.EventHandler(this.btnRemoveSequence_Click);
            // 
            // ilPlusMinus
            // 
            this.ilPlusMinus.ImageSize = new System.Drawing.Size(16, 16);
            this.ilPlusMinus.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilPlusMinus.ImageStream")));
            this.ilPlusMinus.TransparentColor = System.Drawing.Color.White;
            // 
            // btnAddSequence
            // 
            this.btnAddSequence.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnAddSequence.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnAddSequence.ImageIndex = 0;
            this.btnAddSequence.ImageList = this.ilPlusMinus;
            this.btnAddSequence.Location = new System.Drawing.Point(52, 16);
            this.btnAddSequence.Name = "btnAddSequence";
            this.btnAddSequence.Size = new System.Drawing.Size(48, 32);
            this.btnAddSequence.TabIndex = 0;
            this.ttProperties.SetToolTip(this.btnAddSequence, "Add sequences to this sequence group.");
            this.btnAddSequence.Click += new System.EventHandler(this.btnAddSequence_Click);
            // 
            // pnlProps
            // 
            this.pnlProps.Controls.Add(this.nsEaseOut);
            this.pnlProps.Controls.Add(this.nsEaseIn);
            this.pnlProps.Controls.Add(this.nsWeight);
            this.pnlProps.Controls.Add(this.nsPriority);
            this.pnlProps.Controls.Add(this.lblSyncToSequence);
            this.pnlProps.Controls.Add(this.cbSyncToSequence);
            this.pnlProps.Controls.Add(this.lblEaseOut);
            this.pnlProps.Controls.Add(this.lblEaseIn);
            this.pnlProps.Controls.Add(this.lblWeight);
            this.pnlProps.Controls.Add(this.lblPriority);
            this.pnlProps.Controls.Add(this.tbKeyAssignment);
            this.pnlProps.Controls.Add(this.lblKeyAssignment);
            this.pnlProps.Controls.Add(this.tbProperties);
            this.pnlProps.Controls.Add(this.btnRemoveSequence);
            this.pnlProps.Controls.Add(this.btnAddSequence);
            this.pnlProps.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlProps.Location = new System.Drawing.Point(0, 171);
            this.pnlProps.Name = "pnlProps";
            this.pnlProps.Size = new System.Drawing.Size(208, 304);
            this.pnlProps.TabIndex = 2;
            this.pnlProps.Paint += new System.Windows.Forms.PaintEventHandler(this.pnlProps_Paint);
            // 
            // nsEaseOut
            // 
            this.nsEaseOut.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.nsEaseOut.DecimalPlaces = 5;
            this.nsEaseOut.EnableMeter = true;
            this.nsEaseOut.EnablePopup = true;
            this.nsEaseOut.Increment = new System.Decimal(new int[] {
                                                                        5,
                                                                        0,
                                                                        0,
                                                                        196608});
            this.nsEaseOut.Location = new System.Drawing.Point(96, 228);
            this.nsEaseOut.Maximum = new System.Decimal(new int[] {
                                                                      5,
                                                                      0,
                                                                      0,
                                                                      0});
            this.nsEaseOut.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsEaseOut.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsEaseOut.Minimum = new System.Decimal(new int[] {
                                                                      0,
                                                                      0,
                                                                      0,
                                                                      0});
            this.nsEaseOut.Name = "nsEaseOut";
            this.nsEaseOut.PopupHeight = 16;
            this.nsEaseOut.ReadOnly = false;
            this.nsEaseOut.Size = new System.Drawing.Size(104, 28);
            this.nsEaseOut.TabIndex = 12;
            this.nsEaseOut.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttProperties.SetToolTip(this.nsEaseOut, "The ease-out time determines how long a sequence will take\nto fully reach a weigh" +
                "t of zero once it has been deactivated.");
            this.nsEaseOut.Value = new System.Decimal(new int[] {
                                                                    0,
                                                                    0,
                                                                    0,
                                                                    0});
            this.nsEaseOut.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsEaseOut_ValueChanged);
            // 
            // nsEaseIn
            // 
            this.nsEaseIn.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.nsEaseIn.DecimalPlaces = 5;
            this.nsEaseIn.EnableMeter = true;
            this.nsEaseIn.EnablePopup = true;
            this.nsEaseIn.Increment = new System.Decimal(new int[] {
                                                                       5,
                                                                       0,
                                                                       0,
                                                                       196608});
            this.nsEaseIn.Location = new System.Drawing.Point(96, 192);
            this.nsEaseIn.Maximum = new System.Decimal(new int[] {
                                                                     5,
                                                                     0,
                                                                     0,
                                                                     0});
            this.nsEaseIn.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsEaseIn.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsEaseIn.Minimum = new System.Decimal(new int[] {
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0});
            this.nsEaseIn.Name = "nsEaseIn";
            this.nsEaseIn.PopupHeight = 16;
            this.nsEaseIn.ReadOnly = false;
            this.nsEaseIn.Size = new System.Drawing.Size(104, 28);
            this.nsEaseIn.TabIndex = 10;
            this.nsEaseIn.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttProperties.SetToolTip(this.nsEaseIn, "The ease-in time determines how long a sequence will take\nto fully reach its targ" +
                "et weight once it has been activated.");
            this.nsEaseIn.Value = new System.Decimal(new int[] {
                                                                   0,
                                                                   0,
                                                                   0,
                                                                   0});
            this.nsEaseIn.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsEaseIn_ValueChanged);
            // 
            // nsWeight
            // 
            this.nsWeight.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.nsWeight.DecimalPlaces = 3;
            this.nsWeight.EnableMeter = true;
            this.nsWeight.EnablePopup = true;
            this.nsWeight.Increment = new System.Decimal(new int[] {
                                                                       1,
                                                                       0,
                                                                       0,
                                                                       131072});
            this.nsWeight.Location = new System.Drawing.Point(96, 156);
            this.nsWeight.Maximum = new System.Decimal(new int[] {
                                                                     1,
                                                                     0,
                                                                     0,
                                                                     0});
            this.nsWeight.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsWeight.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsWeight.Minimum = new System.Decimal(new int[] {
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0});
            this.nsWeight.Name = "nsWeight";
            this.nsWeight.PopupHeight = 16;
            this.nsWeight.ReadOnly = false;
            this.nsWeight.Size = new System.Drawing.Size(104, 28);
            this.nsWeight.TabIndex = 8;
            this.nsWeight.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttProperties.SetToolTip(this.nsWeight, "The weight modifies the effect of a sequence on the final blended\nanimation. The " +
                "valid range for a weight is 0.0 to 1.0.");
            this.nsWeight.Value = new System.Decimal(new int[] {
                                                                   5,
                                                                   0,
                                                                   0,
                                                                   65536});
            this.nsWeight.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsWeight_ValueChanged);
            // 
            // nsPriority
            // 
            this.nsPriority.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.nsPriority.DecimalPlaces = 0;
            this.nsPriority.EnableMeter = true;
            this.nsPriority.EnablePopup = true;
            this.nsPriority.Increment = new System.Decimal(new int[] {
                                                                         1,
                                                                         0,
                                                                         0,
                                                                         0});
            this.nsPriority.Location = new System.Drawing.Point(96, 120);
            this.nsPriority.Maximum = new System.Decimal(new int[] {
                                                                       10,
                                                                       0,
                                                                       0,
                                                                       0});
            this.nsPriority.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsPriority.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsPriority.Minimum = new System.Decimal(new int[] {
                                                                       0,
                                                                       0,
                                                                       0,
                                                                       0});
            this.nsPriority.Name = "nsPriority";
            this.nsPriority.PopupHeight = 16;
            this.nsPriority.ReadOnly = false;
            this.nsPriority.Size = new System.Drawing.Size(104, 28);
            this.nsPriority.TabIndex = 6;
            this.nsPriority.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttProperties.SetToolTip(this.nsPriority, "Priority determines which set of sequences are active in the current\nsequence gro" +
                "up. Only sequences of the highest priority are active.");
            this.nsPriority.Value = new System.Decimal(new int[] {
                                                                     1,
                                                                     0,
                                                                     0,
                                                                     0});
            this.nsPriority.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsPriority_ValueChanged);
            // 
            // lblSyncToSequence
            // 
            this.lblSyncToSequence.Location = new System.Drawing.Point(8, 91);
            this.lblSyncToSequence.Name = "lblSyncToSequence";
            this.lblSyncToSequence.Size = new System.Drawing.Size(88, 24);
            this.lblSyncToSequence.TabIndex = 3;
            this.lblSyncToSequence.Text = "Synchronize To:";
            this.lblSyncToSequence.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // cbSyncToSequence
            // 
            this.cbSyncToSequence.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbSyncToSequence.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbSyncToSequence.Location = new System.Drawing.Point(96, 91);
            this.cbSyncToSequence.Name = "cbSyncToSequence";
            this.cbSyncToSequence.Size = new System.Drawing.Size(104, 21);
            this.cbSyncToSequence.TabIndex = 4;
            this.ttProperties.SetToolTip(this.cbSyncToSequence, "Specifies the master sequence for the current sequence.");
            this.cbSyncToSequence.SelectedIndexChanged += new System.EventHandler(this.cbSyncToSequence_SelectedIndexChanged);
            // 
            // lblEaseOut
            // 
            this.lblEaseOut.Location = new System.Drawing.Point(8, 224);
            this.lblEaseOut.Name = "lblEaseOut";
            this.lblEaseOut.Size = new System.Drawing.Size(88, 24);
            this.lblEaseOut.TabIndex = 11;
            this.lblEaseOut.Text = "Ease-Out (sec):";
            this.lblEaseOut.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblEaseIn
            // 
            this.lblEaseIn.Location = new System.Drawing.Point(8, 191);
            this.lblEaseIn.Name = "lblEaseIn";
            this.lblEaseIn.Size = new System.Drawing.Size(88, 24);
            this.lblEaseIn.TabIndex = 9;
            this.lblEaseIn.Text = "Ease-In (sec):";
            this.lblEaseIn.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblWeight
            // 
            this.lblWeight.Location = new System.Drawing.Point(8, 156);
            this.lblWeight.Name = "lblWeight";
            this.lblWeight.Size = new System.Drawing.Size(88, 24);
            this.lblWeight.TabIndex = 7;
            this.lblWeight.Text = "Weight:";
            this.lblWeight.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblPriority
            // 
            this.lblPriority.Location = new System.Drawing.Point(8, 121);
            this.lblPriority.Name = "lblPriority";
            this.lblPriority.Size = new System.Drawing.Size(88, 24);
            this.lblPriority.TabIndex = 5;
            this.lblPriority.Text = "Priority:";
            this.lblPriority.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // tbKeyAssignment
            // 
            this.tbKeyAssignment.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbKeyAssignment.Location = new System.Drawing.Point(96, 264);
            this.tbKeyAssignment.Name = "tbKeyAssignment";
            this.tbKeyAssignment.ReadOnly = true;
            this.tbKeyAssignment.Size = new System.Drawing.Size(104, 20);
            this.tbKeyAssignment.TabIndex = 14;
            this.tbKeyAssignment.Text = "Up + Shift";
            this.tbKeyAssignment.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttProperties.SetToolTip(this.tbKeyAssignment, "The shortcut key that will activate/deactive the current sequence.");
            // 
            // lblKeyAssignment
            // 
            this.lblKeyAssignment.Location = new System.Drawing.Point(8, 262);
            this.lblKeyAssignment.Name = "lblKeyAssignment";
            this.lblKeyAssignment.Size = new System.Drawing.Size(88, 24);
            this.lblKeyAssignment.TabIndex = 13;
            this.lblKeyAssignment.Text = "Key Assignment:";
            this.lblKeyAssignment.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // tbProperties
            // 
            this.tbProperties.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbProperties.BackColor = System.Drawing.SystemColors.Info;
            this.tbProperties.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbProperties.Location = new System.Drawing.Point(0, 61);
            this.tbProperties.Name = "tbProperties";
            this.tbProperties.ReadOnly = true;
            this.tbProperties.Size = new System.Drawing.Size(208, 20);
            this.tbProperties.TabIndex = 2;
            this.tbProperties.TabStop = false;
            this.tbProperties.Text = "Properties";
            this.tbProperties.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // splitActive
            // 
            this.splitActive.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitActive.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitActive.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitActive.Location = new System.Drawing.Point(0, 163);
            this.splitActive.MinExtra = 75;
            this.splitActive.Name = "splitActive";
            this.splitActive.Size = new System.Drawing.Size(208, 8);
            this.splitActive.TabIndex = 1;
            this.splitActive.TabStop = false;
            this.splitActive.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitActive_SplitterMoved);
            this.splitActive.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitActive_SplitterMoving);
            // 
            // ttProperties
            // 
            this.ttProperties.AutoPopDelay = 50000;
            this.ttProperties.InitialDelay = 500;
            this.ttProperties.ReshowDelay = 100;
            // 
            // SequenceGroupProperties
            // 
            this.Controls.Add(this.splitActive);
            this.Controls.Add(this.pnlProps);
            this.Controls.Add(this.pnlActive);
            this.Name = "SequenceGroupProperties";
            this.Size = new System.Drawing.Size(208, 475);
            this.Load += new System.EventHandler(this.SequenceGroupProperties_Load);
            this.pnlActive.ResumeLayout(false);
            this.pnlProps.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void SequenceGroupProperties_Load(
            object sender, System.EventArgs e)
        {
            RegisterDelegates();

            // Initialize button images.
            btnAddSequence.SetImage((Bitmap) btnAddSequence.ImageList.Images
                [btnAddSequence.ImageIndex]);
            btnRemoveSequence.SetImage((Bitmap)
                btnRemoveSequence.ImageList.Images
                [btnRemoveSequence.ImageIndex]);

            // Set label tool tips for numeric sliders. This needs to be done
            // to correct for a bug in which tool tips do not appear over
            // portions of the numeric slider.
            this.ttProperties.SetToolTip(this.lblPriority,
                this.ttProperties.GetToolTip(this.nsPriority));
            this.ttProperties.SetToolTip(this.lblWeight,
                this.ttProperties.GetToolTip(this.nsWeight));
            this.ttProperties.SetToolTip(this.lblEaseIn,
                this.ttProperties.GetToolTip(this.nsEaseIn));
            this.ttProperties.SetToolTip(this.lblEaseOut,
                this.ttProperties.GetToolTip(this.nsEaseOut));

            m_szDefaultSizeOfPanel = pnlProps.Size;
            
            gridActive.SetSequenceGroup(m_SeqGrp);
            if (m_SeqGrp.SequenceInfo == null)
                EnablePropertySection(false);
            else
                CreatePropertySection();

            gridActive.CheckedChanged += new IntEventHandler(
                this.gridActive_CheckedChanged);
            gridActive.WeightChanged += new IntEventHandler(
                this.gridActive_WeightChanged);
            gridActive.SelectionChanged += new EventHandler(
                this.gridActive_SelectionChanged);
        }

        public void PostShow()
        {
            if (m_SeqGrp.SequenceInfo != null)
            {
                // Auto-adjust the height of the window
                int iCellOffset = 
                    ((SourceGrid2.GridVirtual)
                     (gridActive.Grid_SequenceGroup)).AutoSizeMinHeight;
                int iInitialOffset = iCellOffset + 12;
                int iItems = m_SeqGrp.SequenceInfo.Length;

                // cap the items
                if (iItems < 3)
                    iItems = 3;
                //else if (iItems > 11)
                //    iItems = 11;

                pnlProps.Height = Size.Height - 
                    (gridActive.Location.Y + iInitialOffset) -
                    (iCellOffset * iItems); 

                RollBarControl.SplitterMoved(
                    splitActive, this,
                    pnlProps, m_szDefaultSizeOfPanel.Height);   
                Invalidate(true);
            }
        }

        private void OnToggleSequence(uint uiSequenceID)
        {
            int i = m_SeqGrp.GetSequenceInfoIndex(uiSequenceID);
            bool bCheck = gridActive.GetCheck(i);
            gridActive.SetCheck(i, !bCheck);
        }

        private void InitAllStates()
        {
            ResetTree();
        }

        public void SetSequenceGroup(MSequenceGroup SeqGrp)
        {
            m_SeqGrp = SeqGrp;
            InitAllStates();
        }

        private void ResetTree()
        {
            gridActive.SetSequenceGroup(m_SeqGrp);
            CreatePropertySection();
        }

        private void OnSequenceGroupModified(MSequenceGroupEventArgs Args,
            MSequenceGroup kGroup)
        {
            if (kGroup == m_SeqGrp)
            {
                ResetTree();
                
                if (gridActive.SelectedSequenceInfo != null)
                {
                    ResetDropDown(gridActive.SelectedSequenceInfo);
                }
            }
        }

        private void EnablePropertySection(bool bEnable)
        {
            nsEaseIn.Enabled = bEnable;
            nsEaseOut.Enabled = bEnable;
            nsPriority.Enabled = bEnable;
            nsWeight.Enabled = bEnable;
            tbKeyAssignment.Enabled = bEnable;
            cbSyncToSequence.Enabled = bEnable;
        }

        private void CreatePropertySection()
        {
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;

            if (seqInfo != null)
            {
                EnablePropertySection(true);
                nsEaseIn.Value = Convert.ToDecimal(seqInfo.EaseInTime);
                nsEaseOut.Value = Convert.ToDecimal(seqInfo.EaseOutTime);
                nsPriority.Value = seqInfo.Priority;
                nsWeight.Value = Convert.ToDecimal(seqInfo.Weight);
                ResetDropDown(seqInfo);
            }
            else
            {
                EnablePropertySection(false);
                ResetDropDown(seqInfo);
            }

            OnKeystrokeBindingsChanged();
        }

        private void ResetDropDown(MSequenceGroup.MSequenceInfo kActiveEdit)
        {
            MSequenceGroup.MSequenceInfo[] aSeqInfo =
                m_SeqGrp.SequenceInfo;

            cbSyncToSequence.Items.Clear();
            cbSyncToSequence.Items.Add("None");
            if (aSeqInfo != null)
            {
                foreach (MSequenceGroup.MSequenceInfo SeqInfo in aSeqInfo)
                {
                    uint uiCurrSeqID = SeqInfo.SequenceID;
                    MSequence seq = 
                        MFramework.Instance.Animation.GetSequence(uiCurrSeqID);
                    if (kActiveEdit != SeqInfo)
                    {
                        if (kActiveEdit != null && kActiveEdit.CanSyncTo(
                            SeqInfo.SequenceID))
                        {
                            int iIndex = cbSyncToSequence.Items.Add(SeqInfo);
                        }
                    }
                }
            }

            if (kActiveEdit != null)
            {
                SetDropDown(kActiveEdit);
            }
        }

        private int GetDropDownIndex(uint uiSeqID)
        {
            for (int i = 1; i < cbSyncToSequence.Items.Count; i++)
            {
                MSequenceGroup.MSequenceInfo info = 
                    (MSequenceGroup.MSequenceInfo)
                    cbSyncToSequence.Items[i];

                if (info.SequenceID == uiSeqID)
                {
                    return i;
                }
            }
            return -1;
        }

        private void SetDropDown(MSequenceGroup.MSequenceInfo kActiveEdit)
        {
            int iIndex = m_SeqGrp.GetSequenceInfoIndex(kActiveEdit.SequenceID);
            if (iIndex != -1)
            {
                if (kActiveEdit.Synchronized)
                {
                    iIndex = GetDropDownIndex(
                        kActiveEdit.SynchronizeToSequenceID);
                    Debug.Assert(iIndex != -1, 
                        "Drop down is being set to invalid index");
                    Debug.Assert(iIndex < cbSyncToSequence.Items.Count, 
                        "Drop down is being set to invalid index");
                    if (iIndex == -1)
                        cbSyncToSequence.SelectedIndex = 0;
                    else
                        cbSyncToSequence.SelectedIndex = iIndex;
                }
                else
                {
                    cbSyncToSequence.SelectedIndex = 0;
                }
            }
        }

        private MSequenceGroup.MSequenceInfo GetSequenceInfoFromDropDown()
        {
            if (cbSyncToSequence.SelectedIndex == 0)
                return null;

            object obj = 
                cbSyncToSequence.Items[cbSyncToSequence.SelectedIndex];
            
            return (MSequenceGroup.MSequenceInfo) obj;
        }

        private void OnKeystrokeBindingsChanged()
        {
            
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;
                
            if (seqInfo == null)
                return;

            Hashtable localInput = 
                ProjectData.GetSeqInputInfoForGroup(m_SeqGrp.GroupID);
            if (localInput == null || localInput.Count == 0)
                return;

            ProjectData.SeqInputInfo inputinfo = 
                (ProjectData.SeqInputInfo)(localInput[seqInfo.SequenceID]);
            
            if (inputinfo.m_strKeyStroke == null ||
                inputinfo.m_strKeyStroke == "")
            {
                tbKeyAssignment.Text = "";
                return;
            }

            tbKeyAssignment.Text = inputinfo.m_strKeyStroke;
            
            if (inputinfo.m_strModifier == null ||
                inputinfo.m_strModifier == "")
                return;

            tbKeyAssignment.Text = inputinfo.m_strModifier + ", " + 
                tbKeyAssignment.Text;
              
        }

        private void splitActive_SplitterMoved(
            object sender, System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoved(
                sender, this, pnlProps, m_szDefaultSizeOfPanel.Height);        
        }

        private void splitActive_SplitterMoving(
            object sender, System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoving(sender, this, pnlProps);
        }

        private void pnlProps_Paint(
            object sender, System.Windows.Forms.PaintEventArgs e)
        {
        
        }

        private void DeleteActive()
        {
            if (m_SeqGrp.SequenceInfo == null)
                return;

            
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;

            ArrayList kDependentSequences = new ArrayList();
            foreach (MSequenceGroup.MSequenceInfo kOtherInfo 
                in m_SeqGrp.SequenceInfo)
            {
                if (kOtherInfo == seqInfo)
                    continue;
                if (kOtherInfo.Synchronized && 
                    kOtherInfo.SynchronizeToSequenceID == seqInfo.SequenceID)
                    kDependentSequences.Add(kOtherInfo.Name);

            }

            if (kDependentSequences.Count != 0)
            {
                string [] strVariables = new string[2];
                strVariables[0] = seqInfo.Name;
                strVariables[1] = 
                    MessageBoxManager.CreateBulletedList(kDependentSequences);
                if (MessageBoxManager.DoMessageBox(
                    "DependentSequenceInSequenceGroupFoundOnDeletion.rtf",
                    "Dependent Sequences Found", MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning, strVariables) == DialogResult.No)
                {
                    return;
                }
                else
                {
                    foreach (MSequenceGroup.MSequenceInfo kOtherInfo 
                        in m_SeqGrp.SequenceInfo)
                    {
                        if (kOtherInfo == seqInfo)
                            continue;
                        if (kOtherInfo.Synchronized && 
                            kOtherInfo.SynchronizeToSequenceID == 
                            seqInfo.SequenceID)
                        {
                            kOtherInfo.ResetSyncSequence();
                            if (kOtherInfo.Activated)
                            {
                                kOtherInfo.DeactivateImmediate();
                                kOtherInfo.ActivateImmediate();
                            }
                        }

                    }
                }
            }

            uint uiSequenceID = seqInfo.SequenceID;
            int iIndex = m_SeqGrp.GetSequenceInfoIndex(uiSequenceID);
            m_SeqGrp.GetAt(iIndex).DeactivateImmediate();           

            m_SeqGrp.RemoveSequence(uiSequenceID);
            ResetTree();
        }

        private void gridActive_KeyDown(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
               DeleteActive();
            }
        }

        private void cbSyncToSequence_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            
            MSequenceGroup.MSequenceInfo CurrentSeqInfo = 
                gridActive.SelectedSequenceInfo;

            if (cbSyncToSequence.SelectedIndex != 0)
            {
                MSequenceGroup.MSequenceInfo TargetSeqInfo =
                    GetSequenceInfoFromDropDown();
                Debug.Assert(TargetSeqInfo != null);
                if (TargetSeqInfo != null)
                {
                    if (CurrentSeqInfo.SynchronizeToSequenceID != 
                        TargetSeqInfo.SequenceID)
                    {
                        if (CurrentSeqInfo != null && CurrentSeqInfo.CanSyncTo(
                            TargetSeqInfo.SequenceID))
                        {
                            CurrentSeqInfo.SynchronizeToSequenceID = 
                                TargetSeqInfo.SequenceID;

                            if (CurrentSeqInfo.Activated)
                            {
                                CurrentSeqInfo.DeactivateImmediate();
                                CurrentSeqInfo.Activate();
                            }
                        }
                    }
                    
                    return;
                }
            }
            else if (cbSyncToSequence.SelectedIndex == 0)
            {
                // Could not find a sequence to sync to
                if (CurrentSeqInfo.Synchronized != false)
                {
                    CurrentSeqInfo.ResetSyncSequence();
                    if (CurrentSeqInfo.Activated)
                    {
                        CurrentSeqInfo.DeactivateImmediate();
                        CurrentSeqInfo.Activate();
                    }
                }
                return;
            }

            Debug.Assert(false, "Should not ever get here!");
        }

        private void btnAddSequence_Click(object sender, System.EventArgs e)
        {
            AddRemoveSeqToGrpForm kForm = new AddRemoveSeqToGrpForm(m_SeqGrp);
            DialogResult eResult = kForm.ShowDialog();
            if (eResult == DialogResult.OK)
                kForm.DoIt();
        }

        private void btnRemoveSequence_Click(object sender, System.EventArgs e)
        {
            DeleteActive();
        }

        private void nsPriority_ValueChanged(object sender, decimal Value)
        {
            
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;

            try
            {
                int iPriority = Convert.ToInt32(nsPriority.Value);
                if (iPriority != seqInfo.Priority)
                {
                    seqInfo.Priority = iPriority;    
                    seqInfo.DeactivateImmediate();
                    seqInfo.Activate();
                }
            }
            catch(Exception)
            {
            }
        }

        private void nsWeight_ValueChanged(object sender, decimal Value)
        {
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;
            if (seqInfo == null)
                return;

            try
            {
                float fWeight = Convert.ToSingle(nsWeight.Value);
                if (fWeight != seqInfo.Weight)
                {
                    seqInfo.Weight = fWeight; 
                    MFramework.Instance.Animation.SetSequenceWeight(
                        seqInfo.SequenceID, seqInfo.Weight);
                    this.gridActive.SetWeight(
                        gridActive.SelectedSequenceInfoIndex, 
                        nsWeight.Value);
                }
            }
            catch(Exception)
            {
            }
            
        }

        private void nsEaseIn_ValueChanged(object sender, decimal Value)
        {
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;

            try 
            {
                float fEaseIn = Convert.ToSingle(nsEaseIn.Value);
                if (fEaseIn != seqInfo.EaseInTime)
                {
                    seqInfo.EaseInTime = fEaseIn;
                }
            }
            catch (Exception)
            {
            }
            
        }

        private void nsEaseOut_ValueChanged(object sender, decimal Value)
        {
            MSequenceGroup.MSequenceInfo seqInfo = 
                gridActive.SelectedSequenceInfo;

            try
            {
                float fEaseOut = Convert.ToSingle(nsEaseOut.Value);
                if (fEaseOut != seqInfo.EaseOutTime)
                {
                    seqInfo.EaseOutTime = fEaseOut;
                }
            }
            catch(Exception)
            {
            }
        }

        private void gridActive_SelectionChanged(object sender, EventArgs e)
        {
            CreatePropertySection();
        }

        private void gridActive_WeightChanged(object sender, int Value)
        {
            MSequenceGroup.MSequenceInfo seqInfo =
                gridActive.SelectedSequenceInfo;

            decimal dWeight = gridActive.GetWeight(Value);
            if (dWeight == this.nsWeight.Value)
                return;

            nsWeight.Value = dWeight;
        }

        private void gridActive_CheckedChanged(object sender, int Value)
        {
        }

        private void gridActive_Load(object sender, System.EventArgs e)
        {
        
        }
    }
}
