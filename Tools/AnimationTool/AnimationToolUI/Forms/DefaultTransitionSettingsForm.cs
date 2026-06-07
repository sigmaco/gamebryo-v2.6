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

using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for DefaultTransitionSettingsForm.
    /// </summary>
    public class DefaultTransitionSettingsForm : System.Windows.Forms.Form
    {
        private class TransTypeObject
        {
            private MTransition.TransitionType m_eType;
            public MTransition.TransitionType Type
            {
                get
                {
                    return m_eType;
                }
                set
                {
                    m_eType = value;
                }
            }

            public TransTypeObject(MTransition.TransitionType eType)
            {
                m_eType = eType;
            }

            public override string ToString()
            {
                return ProjectData.GetTypeNameForTransitionType(m_eType);
            }
        }

        private MTransition.TransitionType m_eType;
        private System.Windows.Forms.TextBox tbTransType;
        private System.Windows.Forms.Label lblDefaultType;
        private System.Windows.Forms.Label lblDefaultDuration;
        private System.Windows.Forms.ComboBox cbDefaultType;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private AnimationToolUI.NumericSlider nsDuration;
        private System.Windows.Forms.ToolTip ttDefaultTransitionSettings;
        private System.ComponentModel.IContainer components;

        public DefaultTransitionSettingsForm(MTransition.TransitionType eType)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_eType = eType;
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
            this.tbTransType = new System.Windows.Forms.TextBox();
            this.lblDefaultType = new System.Windows.Forms.Label();
            this.lblDefaultDuration = new System.Windows.Forms.Label();
            this.cbDefaultType = new System.Windows.Forms.ComboBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.nsDuration = new AnimationToolUI.NumericSlider();
            this.ttDefaultTransitionSettings = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // tbTransType
            // 
            this.tbTransType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbTransType.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbTransType.Location = new System.Drawing.Point(8, 8);
            this.tbTransType.Name = "tbTransType";
            this.tbTransType.ReadOnly = true;
            this.tbTransType.Size = new System.Drawing.Size(244, 20);
            this.tbTransType.TabIndex = 0;
            this.tbTransType.TabStop = false;
            this.tbTransType.Text = "textBox1";
            this.tbTransType.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblDefaultType
            // 
            this.lblDefaultType.AutoSize = true;
            this.lblDefaultType.Location = new System.Drawing.Point(8, 45);
            this.lblDefaultType.Name = "lblDefaultType";
            this.lblDefaultType.Size = new System.Drawing.Size(71, 16);
            this.lblDefaultType.TabIndex = 1;
            this.lblDefaultType.Text = "Default &Type:";
            // 
            // lblDefaultDuration
            // 
            this.lblDefaultDuration.AutoSize = true;
            this.lblDefaultDuration.Location = new System.Drawing.Point(8, 76);
            this.lblDefaultDuration.Name = "lblDefaultDuration";
            this.lblDefaultDuration.Size = new System.Drawing.Size(89, 16);
            this.lblDefaultDuration.TabIndex = 3;
            this.lblDefaultDuration.Text = "Default &Duration:";
            // 
            // cbDefaultType
            // 
            this.cbDefaultType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbDefaultType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbDefaultType.Location = new System.Drawing.Point(96, 40);
            this.cbDefaultType.Name = "cbDefaultType";
            this.cbDefaultType.Size = new System.Drawing.Size(156, 21);
            this.cbDefaultType.TabIndex = 1;
            this.ttDefaultTransitionSettings.SetToolTip(this.cbDefaultType, "Select the transition type to be associated\nwith this default transition type.");
            // 
            // btnOK
            // 
            this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(47, 120);
            this.btnOK.Name = "btnOK";
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(143, 120);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.TabIndex = 3;
            this.btnCancel.Text = "Cancel";
            // 
            // nsDuration
            // 
            this.nsDuration.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.nsDuration.DecimalPlaces = 5;
            this.nsDuration.EnableMeter = true;
            this.nsDuration.EnablePopup = true;
            this.nsDuration.Increment = new System.Decimal(new int[] {
                                                                         5,
                                                                         0,
                                                                         0,
                                                                         196608});
            this.nsDuration.Location = new System.Drawing.Point(96, 72);
            this.nsDuration.Maximum = new System.Decimal(new int[] {
                                                                       5,
                                                                       0,
                                                                       0,
                                                                       0});
            this.nsDuration.MeterLeftColor = System.Drawing.SystemColors.ActiveCaption;
            this.nsDuration.MeterRightColor = System.Drawing.SystemColors.Control;
            this.nsDuration.Minimum = new System.Decimal(new int[] {
                                                                       0,
                                                                       0,
                                                                       0,
                                                                       0});
            this.nsDuration.Name = "nsDuration";
            this.nsDuration.PopupHeight = 16;
            this.nsDuration.ReadOnly = false;
            this.nsDuration.Size = new System.Drawing.Size(156, 28);
            this.nsDuration.TabIndex = 2;
            this.nsDuration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttDefaultTransitionSettings.SetToolTip(this.nsDuration, "Select the transition duration to be associated\nwith this default transition type" +
                ".");
            this.nsDuration.Value = new System.Decimal(new int[] {
                                                                     5,
                                                                     0,
                                                                     0,
                                                                     65536});
            // 
            // DefaultTransitionSettingsForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(264, 158);
            this.ControlBox = false;
            this.Controls.Add(this.nsDuration);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.cbDefaultType);
            this.Controls.Add(this.lblDefaultDuration);
            this.Controls.Add(this.lblDefaultType);
            this.Controls.Add(this.tbTransType);
            this.MinimizeBox = false;
            this.Name = "DefaultTransitionSettingsForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Default Transition Settings";
            this.Load += new System.EventHandler(this.DefaultTransitionSettingsForm_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void DefaultTransitionSettingsForm_Load(object sender,
            System.EventArgs e)
        {
            // Set label tool tips for numeric sliders. This needs to be done
            // to correct for a bug in which tool tips do not appear over
            // portions of the numeric slider.
            this.ttDefaultTransitionSettings.SetToolTip(
                this.lblDefaultDuration, this.ttDefaultTransitionSettings
                .GetToolTip(this.nsDuration));

            tbTransType.BackColor = ProjectData.GetColorForTransitionType(
                m_eType);
            tbTransType.ForeColor = ProjectData.GetTextColorForTransitionType(
                m_eType);
            tbTransType.Text = ProjectData.GetTypeNameForTransitionType(
                m_eType);

            switch (m_eType)
            {
                case MTransition.TransitionType.Trans_DefaultSync:
                    cbDefaultType.Items.Add(new TransTypeObject(MTransition
                        .TransitionType.Trans_Morph));
                    cbDefaultType.SelectedIndex = cbDefaultType.FindString(
                        ProjectData.GetTypeNameForTransitionType(MFramework
                        .Instance.Animation.DefaultSyncTransType));
                    nsDuration.Value = Convert.ToDecimal(MFramework
                        .Instance.Animation.DefaultSyncTransDuration);
                    break;
                case MTransition.TransitionType.Trans_DefaultNonSync:
                    cbDefaultType.Items.Add(new TransTypeObject(MTransition
                        .TransitionType.Trans_ImmediateBlend));
                    cbDefaultType.Items.Add(new TransTypeObject(MTransition
                        .TransitionType.Trans_CrossFade));
                    cbDefaultType.SelectedIndex = cbDefaultType.FindString(
                        ProjectData.GetTypeNameForTransitionType(MFramework
                        .Instance.Animation.DefaultNonSyncTransType));
                    nsDuration.Value = Convert.ToDecimal(MFramework
                        .Instance.Animation.DefaultNonSyncTransDuration);
                    break;
            }
        }

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            TransTypeObject obj = cbDefaultType.SelectedItem as
                TransTypeObject;
            switch (m_eType)
            {
                case MTransition.TransitionType.Trans_DefaultSync:
                    MFramework.Instance.Animation.DefaultSyncTransType =
                        obj.Type;
                    MFramework.Instance.Animation.DefaultSyncTransDuration =
                        (float) nsDuration.Value;
                    break;
                case MTransition.TransitionType.Trans_DefaultNonSync:
                    MFramework.Instance.Animation.DefaultNonSyncTransType =
                        obj.Type;
                    MFramework.Instance.Animation.DefaultNonSyncTransDuration
                        = (float) nsDuration.Value;
                    break;
            }

            this.DialogResult = DialogResult.OK;
        }
    }
}
