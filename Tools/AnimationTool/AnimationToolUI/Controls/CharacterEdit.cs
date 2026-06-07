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
using System.IO;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for CharacterEdit.
    /// </summary>
    public class CharacterEdit : System.Windows.Forms.UserControl
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
            MFramework.Instance.Animation.OnModelPathAndRootChanged 
                += m_OnModelPathAndRootChanged;
        }

        private void UnregisterDelegates()
        {
            if(m_OnModelPathAndRootChanged != null)
                MFramework.Instance.Animation.OnModelPathAndRootChanged -= 
                    m_OnModelPathAndRootChanged;
        }

        #endregion
        
        private System.Windows.Forms.Button btnEditRoot;
        private System.Windows.Forms.Label lblSelectedRoot;
        private System.Windows.Forms.Button btnSelectFile;
        private System.Windows.Forms.TextBox tbRoot;
        private System.Windows.Forms.TextBox tbFile;
        private System.Windows.Forms.ToolTip ttCharacterEdit;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public CharacterEdit()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
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
            this.btnEditRoot = new System.Windows.Forms.Button();
            this.lblSelectedRoot = new System.Windows.Forms.Label();
            this.btnSelectFile = new System.Windows.Forms.Button();
            this.tbRoot = new System.Windows.Forms.TextBox();
            this.tbFile = new System.Windows.Forms.TextBox();
            this.ttCharacterEdit = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // btnEditRoot
            // 
            this.btnEditRoot.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnEditRoot.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnEditRoot.Location = new System.Drawing.Point(24, 88);
            this.btnEditRoot.Name = "btnEditRoot";
            this.btnEditRoot.Size = new System.Drawing.Size(192, 24);
            this.btnEditRoot.TabIndex = 4;
            this.btnEditRoot.Text = "Edit Selected Root...";
            this.ttCharacterEdit.SetToolTip(this.btnEditRoot, "Select the actor root from the available nodes in the scene graph.");
            this.btnEditRoot.Click += new System.EventHandler(this.btnEditRoot_Click);
            // 
            // lblSelectedRoot
            // 
            this.lblSelectedRoot.Location = new System.Drawing.Point(8, 56);
            this.lblSelectedRoot.Name = "lblSelectedRoot";
            this.lblSelectedRoot.Size = new System.Drawing.Size(80, 16);
            this.lblSelectedRoot.TabIndex = 2;
            this.lblSelectedRoot.Text = "Selected Root:";
            // 
            // btnSelectFile
            // 
            this.btnSelectFile.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnSelectFile.Location = new System.Drawing.Point(8, 16);
            this.btnSelectFile.Name = "btnSelectFile";
            this.btnSelectFile.Size = new System.Drawing.Size(80, 24);
            this.btnSelectFile.TabIndex = 0;
            this.btnSelectFile.Text = "Select File...";
            this.ttCharacterEdit.SetToolTip(this.btnSelectFile, "Select the NIF file for use with the current actor manager.");
            this.btnSelectFile.Click += new System.EventHandler(this.btnSelectFile_Click);
            // 
            // tbRoot
            // 
            this.tbRoot.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbRoot.Location = new System.Drawing.Point(104, 56);
            this.tbRoot.Name = "tbRoot";
            this.tbRoot.ReadOnly = true;
            this.tbRoot.Size = new System.Drawing.Size(128, 20);
            this.tbRoot.TabIndex = 3;
            this.tbRoot.Text = "";
            this.ttCharacterEdit.SetToolTip(this.tbRoot, "The name of the node in the scene graph to which the actor manager is attached.");
            // 
            // tbFile
            // 
            this.tbFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbFile.Location = new System.Drawing.Point(104, 18);
            this.tbFile.Name = "tbFile";
            this.tbFile.ReadOnly = true;
            this.tbFile.Size = new System.Drawing.Size(128, 20);
            this.tbFile.TabIndex = 1;
            this.tbFile.Text = "";
            this.ttCharacterEdit.SetToolTip(this.tbFile, "The path to the NIF file used to specify the model geometry for the actor manager" +
                ".");
            // 
            // CharacterEdit
            // 
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.tbFile);
            this.Controls.Add(this.tbRoot);
            this.Controls.Add(this.lblSelectedRoot);
            this.Controls.Add(this.btnSelectFile);
            this.Controls.Add(this.btnEditRoot);
            this.DockPadding.All = 5;
            this.Name = "CharacterEdit";
            this.Size = new System.Drawing.Size(240, 128);
            this.Load += new System.EventHandler(this.CharacterEdit_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void CharacterEdit_Load(object sender, System.EventArgs e)
        {
            RegisterDelegates();
            UpdateContents();

            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.None, null);
        }

        private void OnModelPathAndRootChanged(string strModelPath,
            string strModelRoot)
        {
            UpdateContents();
        }

        private void btnSelectFile_Click(object sender, System.EventArgs e)
        {
            AnimationHelpers.OpenModelDialog(this.ParentForm);
        }

        public void UpdateContents()
        {
            // Update model root text box contents.
            if (MFramework.Instance.Animation.ModelRoot == null ||
                MFramework.Instance.Animation.ModelRoot == string.Empty)
            {
                tbRoot.Text = "<UNDEFINED>";
            }
            else
            {
                tbRoot.Text = MFramework.Instance.Animation.ModelRoot;
            }

            // Update model root text box tool tip.
            string strToolTip = this.ttCharacterEdit.GetToolTip(
                this.tbRoot);
            char[] acSeparators = new char[] {'\n'};
            string[] astrLines = strToolTip.Split(acSeparators);
            this.ttCharacterEdit.SetToolTip(this.tbRoot,
                astrLines[0] + "\n" + this.tbRoot.Text);

            // Update model path text box contents.
            if (MFramework.Instance.Animation.ModelPath == null ||
                MFramework.Instance.Animation.ModelPath == string.Empty)
            {
                tbFile.Text = "<UNDEFINED>";
            }
            else
            {
                tbFile.Text = Path.GetFileName(MFramework.Instance.Animation
                    .FullModelPath);
            }

            // Update model path text box tool tip.
            strToolTip = this.ttCharacterEdit.GetToolTip(
                this.tbFile);
            astrLines = strToolTip.Split(acSeparators);
            this.ttCharacterEdit.SetToolTip(this.tbFile,
                astrLines[0] + "\n" +
                MFramework.Instance.Animation.FullModelPath);

            // Update edit root button active state.
            if (MFramework.Instance.Animation.FullModelPath != string.Empty)
            {
                btnEditRoot.Enabled = true;
            }
            else
            {
                btnEditRoot.Enabled = false;
            }
        }

        private void btnEditRoot_Click(object sender, System.EventArgs e)
        {
            AnimationHelpers.DoModelRootDialog(this.ParentForm);
            UpdateContents();
        }
    }
}
