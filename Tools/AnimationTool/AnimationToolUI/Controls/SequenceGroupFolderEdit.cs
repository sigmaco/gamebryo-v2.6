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
    /// Summary description for SequenceGroupFolderEdit.
    /// </summary>
    public class SequenceGroupFolderEdit : System.Windows.Forms.UserControl
    {
        #region Delegates

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.

            // Register delegates.
        }

        private void UnregisterDelegates()
        {
        }

        #endregion

        private ProjectTreeNode m_kNode;
        private System.Windows.Forms.Button Button_CreateFolder;
        private System.Windows.Forms.Button Button_CreateSequenceGroup;
        private System.Windows.Forms.ToolTip ToolTip_SequenceGroupFolder;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public SequenceGroupFolderEdit(ProjectTreeNode kNode)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            m_kNode = kNode;

            Debug.Assert(NumInstances == 0);
            NumInstances++;
            RegisterDelegates();
            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.None, null);
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
            this.Button_CreateSequenceGroup = new System.Windows.Forms.Button();
            this.ToolTip_SequenceGroupFolder = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // Button_CreateFolder
            // 
            this.Button_CreateFolder.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.Button_CreateFolder.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_CreateFolder.Location = new System.Drawing.Point(16, 16);
            this.Button_CreateFolder.Name = "Button_CreateFolder";
            this.Button_CreateFolder.Size = new System.Drawing.Size(176, 24);
            this.Button_CreateFolder.TabIndex = 0;
            this.Button_CreateFolder.Text = "Create Sequence Group Folder";
            this.ToolTip_SequenceGroupFolder.SetToolTip(this.Button_CreateFolder, "Create a new sequence group folder as a child of the current folder.");
            this.Button_CreateFolder.Click += new System.EventHandler(this.Button_CreateFolder_Click);
            // 
            // Button_CreateSequenceGroup
            // 
            this.Button_CreateSequenceGroup.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.Button_CreateSequenceGroup.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_CreateSequenceGroup.Location = new System.Drawing.Point(16, 56);
            this.Button_CreateSequenceGroup.Name = "Button_CreateSequenceGroup";
            this.Button_CreateSequenceGroup.Size = new System.Drawing.Size(176, 24);
            this.Button_CreateSequenceGroup.TabIndex = 1;
            this.Button_CreateSequenceGroup.Text = "Create Sequence Group ";
            this.ToolTip_SequenceGroupFolder.SetToolTip(this.Button_CreateSequenceGroup, "Create a new empty sequence group as a child of the current folder.");
            this.Button_CreateSequenceGroup.Click += new System.EventHandler(this.Button_CreateSequenceGroup_Click);
            // 
            // ToolTip_SequenceGroupFolder
            // 
            this.ToolTip_SequenceGroupFolder.AutoPopDelay = 50000;
            this.ToolTip_SequenceGroupFolder.InitialDelay = 500;
            this.ToolTip_SequenceGroupFolder.ReshowDelay = 100;
            // 
            // SequenceGroupFolderEdit
            // 
            this.Controls.Add(this.Button_CreateSequenceGroup);
            this.Controls.Add(this.Button_CreateFolder);
            this.Name = "SequenceGroupFolderEdit";
            this.Size = new System.Drawing.Size(208, 96);
            this.Load += new System.EventHandler(this.SequenceGroupFolderEdit_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void SequenceGroupFolderEdit_Load(object sender,
            System.EventArgs e)
        {
        }

        private void Button_CreateFolder_Click(object sender,
            System.EventArgs e)
        {
            //NewFolderForm kNewFolderForm = new NewFolderForm(m_kNode);
            //kNewFolderForm.ShowDialog(this);  
            AppEvents.RaiseProjectViewNewFolder();
        }

        private void Button_CreateSequenceGroup_Click(object sender,
            System.EventArgs e)
        {
            string strName = MFramework.Instance.Animation
                .FindUnusedSequenceGroupName("New Sequence Group");
            uint uiGroupID = MFramework.Instance.Animation
                .FindUnusedGroupID();
            MSequenceGroup kNewGroup = MFramework.Instance.Animation
                .AddSequenceGroup(uiGroupID, strName);
            Debug.Assert(kNewGroup != null,
                "Failed to create new Sequence Group");
            ProjectTreeNode kNewGroupNode = ProjectData.GetSequenceGroupNode(
                uiGroupID);
            ProjectData.MoveToFolder(m_kNode, kNewGroupNode);
            m_kNode.Expand();
            kNewGroupNode.BeginEdit();
        }
    }
}
