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
    /// Summary description for TransitionDefaultEdit.
    /// </summary>
    public class TransitionDefaultEdit : System.Windows.Forms.UserControl
    {
        private MTransition [] m_aTran;

        private System.Windows.Forms.GroupBox gbDefaultSettings;
        private System.Windows.Forms.Label lblDefaultType;
        private System.Windows.Forms.Label lblDefaultDuration;
        private System.Windows.Forms.Button btnChangeDefaults;
        private System.Windows.Forms.Button btnMakeUnique;
        private System.Windows.Forms.TextBox tbDefaultType;
        private System.Windows.Forms.TextBox tbDefaultDuration;
        private System.Windows.Forms.ToolTip ttDefaultEdit;
        private System.ComponentModel.IContainer components;

        private AppEvents.EventHandler_TransitionTableSettingsChanged 
            m_kTransitionTableSettingsEventHandler;

        public TransitionDefaultEdit(MTransition Tran)
        {
            ArrayList aTran = new ArrayList(1);
            aTran.Add(Tran);
            TransitionDefaultEditConstructorInit(aTran);
        }

        public TransitionDefaultEdit(ArrayList kTranList)
        {
            TransitionDefaultEditConstructorInit(kTranList);            
        }

        private void TransitionDefaultEditConstructorInit(ArrayList kTranList)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_aTran = new MTransition[kTranList.Count];
            int j = 0;
            for (int i = 0; i < kTranList.Count; i++)
            {
                MTransition Tran = (MTransition) kTranList[i];
                if (Tran.Source == Tran.Destination)
                {
                    continue;
                }

                m_aTran[j] = Tran;
                //MTransition Tran = m_TranData[j].Transition;

                if (Tran != null)
                    Debug.Assert(Tran.StoredType == MTransition.TransitionType
                        .Trans_DefaultSync || Tran.StoredType == MTransition
                        .TransitionType.Trans_DefaultNonSync);

                j++;
            }

            m_kTransitionTableSettingsEventHandler = null;
        }

        static uint NumInstances = 0;

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
                AppEvents.TransitionTableSettingsChanged -= 
                    m_kTransitionTableSettingsEventHandler;
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
            this.gbDefaultSettings = new System.Windows.Forms.GroupBox();
            this.tbDefaultDuration = new System.Windows.Forms.TextBox();
            this.tbDefaultType = new System.Windows.Forms.TextBox();
            this.btnMakeUnique = new System.Windows.Forms.Button();
            this.btnChangeDefaults = new System.Windows.Forms.Button();
            this.lblDefaultDuration = new System.Windows.Forms.Label();
            this.lblDefaultType = new System.Windows.Forms.Label();
            this.ttDefaultEdit = new System.Windows.Forms.ToolTip(this.components);
            this.gbDefaultSettings.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbDefaultSettings
            // 
            this.gbDefaultSettings.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.gbDefaultSettings.Controls.Add(this.tbDefaultDuration);
            this.gbDefaultSettings.Controls.Add(this.tbDefaultType);
            this.gbDefaultSettings.Controls.Add(this.btnMakeUnique);
            this.gbDefaultSettings.Controls.Add(this.btnChangeDefaults);
            this.gbDefaultSettings.Controls.Add(this.lblDefaultDuration);
            this.gbDefaultSettings.Controls.Add(this.lblDefaultType);
            this.gbDefaultSettings.Location = new System.Drawing.Point(8, 8);
            this.gbDefaultSettings.Name = "gbDefaultSettings";
            this.gbDefaultSettings.Size = new System.Drawing.Size(280, 160);
            this.gbDefaultSettings.TabIndex = 0;
            this.gbDefaultSettings.TabStop = false;
            this.gbDefaultSettings.Text = "Default Transition Settings";
            // 
            // tbDefaultDuration
            // 
            this.tbDefaultDuration.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbDefaultDuration.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbDefaultDuration.Location = new System.Drawing.Point(96, 60);
            this.tbDefaultDuration.Name = "tbDefaultDuration";
            this.tbDefaultDuration.ReadOnly = true;
            this.tbDefaultDuration.Size = new System.Drawing.Size(176, 20);
            this.tbDefaultDuration.TabIndex = 3;
            this.tbDefaultDuration.TabStop = false;
            this.tbDefaultDuration.Text = "textBox1";
            this.tbDefaultDuration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttDefaultEdit.SetToolTip(this.tbDefaultDuration, "The transition duration currently associated with this\ndefault transition type.");
            // 
            // tbDefaultType
            // 
            this.tbDefaultType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbDefaultType.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbDefaultType.Location = new System.Drawing.Point(96, 25);
            this.tbDefaultType.Name = "tbDefaultType";
            this.tbDefaultType.ReadOnly = true;
            this.tbDefaultType.Size = new System.Drawing.Size(176, 20);
            this.tbDefaultType.TabIndex = 1;
            this.tbDefaultType.TabStop = false;
            this.tbDefaultType.Text = "textBox1";
            this.tbDefaultType.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttDefaultEdit.SetToolTip(this.tbDefaultType, "The transition type currently associated with this\ndefault transition type.");
            // 
            // btnMakeUnique
            // 
            this.btnMakeUnique.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnMakeUnique.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnMakeUnique.Location = new System.Drawing.Point(68, 96);
            this.btnMakeUnique.Name = "btnMakeUnique";
            this.btnMakeUnique.Size = new System.Drawing.Size(144, 23);
            this.btnMakeUnique.TabIndex = 4;
            this.btnMakeUnique.Text = "Make Unique";
            this.ttDefaultEdit.SetToolTip(this.btnMakeUnique, "Click to change the type of this transition from default\nto the type associated w" +
                "ith that default.");
            this.btnMakeUnique.Click += new System.EventHandler(this.btnMakeUnique_Click);
            // 
            // btnChangeDefaults
            // 
            this.btnChangeDefaults.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnChangeDefaults.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnChangeDefaults.Location = new System.Drawing.Point(68, 128);
            this.btnChangeDefaults.Name = "btnChangeDefaults";
            this.btnChangeDefaults.Size = new System.Drawing.Size(144, 23);
            this.btnChangeDefaults.TabIndex = 5;
            this.btnChangeDefaults.Text = "Change Default Settings";
            this.ttDefaultEdit.SetToolTip(this.btnChangeDefaults, "Click to change the settings for this default transition type.");
            this.btnChangeDefaults.Click += new System.EventHandler(this.btnChangeDefaults_Click);
            // 
            // lblDefaultDuration
            // 
            this.lblDefaultDuration.AutoSize = true;
            this.lblDefaultDuration.Location = new System.Drawing.Point(8, 64);
            this.lblDefaultDuration.Name = "lblDefaultDuration";
            this.lblDefaultDuration.Size = new System.Drawing.Size(89, 16);
            this.lblDefaultDuration.TabIndex = 2;
            this.lblDefaultDuration.Text = "Default Duration:";
            // 
            // lblDefaultType
            // 
            this.lblDefaultType.AutoSize = true;
            this.lblDefaultType.Location = new System.Drawing.Point(8, 29);
            this.lblDefaultType.Name = "lblDefaultType";
            this.lblDefaultType.Size = new System.Drawing.Size(71, 16);
            this.lblDefaultType.TabIndex = 0;
            this.lblDefaultType.Text = "Default Type:";
            // 
            // TransitionDefaultEdit
            // 
            this.Controls.Add(this.gbDefaultSettings);
            this.Name = "TransitionDefaultEdit";
            this.Size = new System.Drawing.Size(296, 200);
            this.Load += new System.EventHandler(this.TransitionDefaultEdit_Load);
            this.gbDefaultSettings.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void TransitionDefaultEdit_Load(object sender,
            System.EventArgs e)
        {
            InitializeControls();
            m_kTransitionTableSettingsEventHandler = 
                new AppEvents.EventHandler_TransitionTableSettingsChanged(
                    TransitionTableSettingsChanged);
            AppEvents.TransitionTableSettingsChanged += 
                m_kTransitionTableSettingsEventHandler;
            
        }

        private void TransitionTableSettingsChanged()
        {
            InitializeControls();
            Invalidate();
        }

        private void InitializeControls()
        {
            tbDefaultType.BackColor = ProjectData.GetColorForTransitionType(
                m_aTran[0].Type);
            tbDefaultType.ForeColor = 
                ProjectData.GetTextColorForTransitionType(m_aTran[0].Type);
            tbDefaultType.Text = ProjectData.GetTypeNameForTransitionType(
                m_aTran[0].Type);

            tbDefaultDuration.Text = m_aTran[0].Duration.ToString("f3");
        }

        private void btnMakeUnique_Click(object sender, System.EventArgs e)
        {
            string[] aStrings = new string[2];
            aStrings[0] = ProjectData.GetTypeNameForTransition(m_aTran[0]);
            aStrings[1] = ProjectData.GetTypeNameForTransitionType(
                m_aTran[0].Type);
            DialogResult result = MessageBoxManager.DoMessageBox(
                "MakeUnique.rtf", "Make Unique", MessageBoxButtons.YesNo,
                MessageBoxIcon.Question, aStrings);
            if (result == DialogResult.No)
            {
                return;
            }

            foreach(MTransition Tran in m_aTran)
            {
                if (Tran == null)
                    continue;
                if (Tran.Source == Tran.Destination)
                    continue;
                Tran.StoredType = Tran.Type;
            }
        }

        private void btnChangeDefaults_Click(object sender,
            System.EventArgs e)
        {
            DefaultTransitionSettingsForm dlg = new
                DefaultTransitionSettingsForm(m_aTran[0].StoredType);
            dlg.ShowDialog();
        }
    }
}
