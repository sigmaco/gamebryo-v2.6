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
    /// Summary description for SequenceFolderEdit.
    /// </summary>
    public class SequenceFolderEdit : System.Windows.Forms.UserControl
    {
        #region Delegates

        private MAnimation.__Delegate_OnModelPathAndRootChanged 
            m_OnModelPathAndRootChanged = null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_OnModelPathAndRootChanged = 
                new MAnimation.__Delegate_OnModelPathAndRootChanged(
                    OnModelPathAndRootChanged);

            // Register delegates.
            MFramework.Instance.Animation.OnModelPathAndRootChanged += 
                m_OnModelPathAndRootChanged;
        }

        private void UnregisterDelegates()
        {
            if (m_OnModelPathAndRootChanged != null)
                MFramework.Instance.Animation.OnModelPathAndRootChanged -= 
                    m_OnModelPathAndRootChanged;
        }

        #endregion

        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.Button Button_CreateFolder;
        private System.Windows.Forms.Button Button_LoadMultipleKFs;
        private System.Windows.Forms.ToolTip ToolTip_SequenceFolder;
        private ProjectTreeNode m_kNode;
        private static uint NumInstances = 0;

        public SequenceFolderEdit(ProjectTreeNode kNode)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_kNode = kNode;
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
            this.Button_CreateFolder = new System.Windows.Forms.Button();
            this.Button_LoadMultipleKFs = new System.Windows.Forms.Button();
            this.ToolTip_SequenceFolder = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // Button_CreateFolder
            // 
            this.Button_CreateFolder.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.Button_CreateFolder.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_CreateFolder.Location = new System.Drawing.Point(8, 16);
            this.Button_CreateFolder.Name = "Button_CreateFolder";
            this.Button_CreateFolder.Size = new System.Drawing.Size(176, 24);
            this.Button_CreateFolder.TabIndex = 0;
            this.Button_CreateFolder.Text = "Create Sequence Folder";
            this.ToolTip_SequenceFolder.SetToolTip(this.Button_CreateFolder, "Create a new sequence folder as a child of the current folder.");
            this.Button_CreateFolder.Click += new System.EventHandler(this.Button_CreateFolder_Click);
            // 
            // Button_LoadMultipleKFs
            // 
            this.Button_LoadMultipleKFs.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.Button_LoadMultipleKFs.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_LoadMultipleKFs.Location = new System.Drawing.Point(8, 56);
            this.Button_LoadMultipleKFs.Name = "Button_LoadMultipleKFs";
            this.Button_LoadMultipleKFs.Size = new System.Drawing.Size(176, 24);
            this.Button_LoadMultipleKFs.TabIndex = 1;
            this.Button_LoadMultipleKFs.Text = "Add Sequences from KF File(s)";
            this.ToolTip_SequenceFolder.SetToolTip(this.Button_LoadMultipleKFs, "Load one or more sequences and add them as children of the current folder.");
            this.Button_LoadMultipleKFs.Click += new System.EventHandler(this.Button_LoadMultipleKFs_Click);
            // 
            // ToolTip_SequenceFolder
            // 
            this.ToolTip_SequenceFolder.AutoPopDelay = 50000;
            this.ToolTip_SequenceFolder.InitialDelay = 500;
            this.ToolTip_SequenceFolder.ReshowDelay = 100;
            // 
            // SequenceFolderEdit
            // 
            this.Controls.Add(this.Button_LoadMultipleKFs);
            this.Controls.Add(this.Button_CreateFolder);
            this.Name = "SequenceFolderEdit";
            this.Size = new System.Drawing.Size(192, 96);
            this.Load += new System.EventHandler(this.SequenceFolderEdit_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void SequenceFolderEdit_Load(object sender, System.EventArgs e)
        {
            RegisterDelegates();
            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.None, null);
            UpdateContents();
        }

        private void UpdateContents()
        {
            bool bShow = false;

            if (MFramework.Instance.Animation.ModelPath.Length > 0 &&
                MFramework.Instance.Animation.ModelRoot.Length > 0)
                bShow = true;

            Button_LoadMultipleKFs.Enabled = bShow;
        }

        private void OnModelPathAndRootChanged(
            string strModelPath, string strModelRoot)
        {
            UpdateContents();
        }

        private void Button_CreateFolder_Click(
            object sender, System.EventArgs e)
        {
            AppEvents.RaiseProjectViewNewFolder();
        }

        private void Button_LoadMultipleKFs_Click(
            object sender, System.EventArgs e)
        {
            AnimationHelpers.OpenMultipleKFsDialog(this.ParentForm);
        }
    }
}
