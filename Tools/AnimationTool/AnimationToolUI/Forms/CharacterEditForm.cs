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
    /// Summary description for CharacterEditForm.
    /// </summary>
    public class CharacterEditForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Panel panel1;

        public string ModelRoot;
        public MSceneObject Scene;
        public string FullModelPath;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Panel pnlCharacterEdit;
        private System.Windows.Forms.Label lblFilename;
        private System.Windows.Forms.TreeView tvRootSel;
        private System.Windows.Forms.Label lblSelectedRoot;
        private System.Windows.Forms.TextBox tbRoot;
        private System.Windows.Forms.TextBox tbFile;
        private System.Windows.Forms.ToolTip ttCharacterEdit;
        private System.ComponentModel.IContainer components;

        public CharacterEditForm(string strFullModelPath, 
            MSceneObject kScene, string strCurrentModelRootName)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            Scene = kScene;
            ModelRoot = strCurrentModelRootName;
            FullModelPath = strFullModelPath;
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.pnlCharacterEdit = new System.Windows.Forms.Panel();
            this.lblFilename = new System.Windows.Forms.Label();
            this.tvRootSel = new System.Windows.Forms.TreeView();
            this.lblSelectedRoot = new System.Windows.Forms.Label();
            this.tbRoot = new System.Windows.Forms.TextBox();
            this.tbFile = new System.Windows.Forms.TextBox();
            this.ttCharacterEdit = new System.Windows.Forms.ToolTip(this.components);
            this.panel1.SuspendLayout();
            this.pnlCharacterEdit.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btnCancel);
            this.panel1.Controls.Add(this.btnOK);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 334);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(416, 48);
            this.panel1.TabIndex = 8;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(224, 8);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(64, 32);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            // 
            // btnOK
            // 
            this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(128, 8);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(64, 32);
            this.btnOK.TabIndex = 3;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // pnlCharacterEdit
            // 
            this.pnlCharacterEdit.BackColor = System.Drawing.SystemColors.Control;
            this.pnlCharacterEdit.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlCharacterEdit.Controls.Add(this.lblFilename);
            this.pnlCharacterEdit.Controls.Add(this.tvRootSel);
            this.pnlCharacterEdit.Controls.Add(this.lblSelectedRoot);
            this.pnlCharacterEdit.Controls.Add(this.tbRoot);
            this.pnlCharacterEdit.Controls.Add(this.tbFile);
            this.pnlCharacterEdit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlCharacterEdit.Location = new System.Drawing.Point(0, 0);
            this.pnlCharacterEdit.Name = "pnlCharacterEdit";
            this.pnlCharacterEdit.Size = new System.Drawing.Size(416, 334);
            this.pnlCharacterEdit.TabIndex = 9;
            // 
            // lblFilename
            // 
            this.lblFilename.Location = new System.Drawing.Point(15, 18);
            this.lblFilename.Name = "lblFilename";
            this.lblFilename.Size = new System.Drawing.Size(81, 16);
            this.lblFilename.TabIndex = 11;
            this.lblFilename.Text = "Filename :";
            // 
            // tvRootSel
            // 
            this.tvRootSel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tvRootSel.HideSelection = false;
            this.tvRootSel.ImageIndex = -1;
            this.tvRootSel.Location = new System.Drawing.Point(16, 80);
            this.tvRootSel.Name = "tvRootSel";
            this.tvRootSel.SelectedImageIndex = -1;
            this.tvRootSel.Size = new System.Drawing.Size(384, 238);
            this.tvRootSel.TabIndex = 10;
            this.ttCharacterEdit.SetToolTip(this.tvRootSel, "Select the desired actor root node here\nfrom the scene graph in the NIF file.");
            this.tvRootSel.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvRootSel_AfterSelect);
            // 
            // lblSelectedRoot
            // 
            this.lblSelectedRoot.Location = new System.Drawing.Point(15, 56);
            this.lblSelectedRoot.Name = "lblSelectedRoot";
            this.lblSelectedRoot.Size = new System.Drawing.Size(80, 16);
            this.lblSelectedRoot.TabIndex = 9;
            this.lblSelectedRoot.Text = "Selected Root:";
            // 
            // tbRoot
            // 
            this.tbRoot.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbRoot.Location = new System.Drawing.Point(98, 52);
            this.tbRoot.Name = "tbRoot";
            this.tbRoot.ReadOnly = true;
            this.tbRoot.Size = new System.Drawing.Size(302, 20);
            this.tbRoot.TabIndex = 7;
            this.tbRoot.Text = "";
            this.ttCharacterEdit.SetToolTip(this.tbRoot, "The name of the node in the scene graph to which the actor manager is attached.");
            // 
            // tbFile
            // 
            this.tbFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbFile.Location = new System.Drawing.Point(97, 16);
            this.tbFile.Name = "tbFile";
            this.tbFile.ReadOnly = true;
            this.tbFile.Size = new System.Drawing.Size(302, 20);
            this.tbFile.TabIndex = 6;
            this.tbFile.Text = "";
            this.ttCharacterEdit.SetToolTip(this.tbFile, "The path to the NIF file used to specify the model geometry for the actor manager" +
                ".");
            // 
            // CharacterEditForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(416, 382);
            this.ControlBox = false;
            this.Controls.Add(this.pnlCharacterEdit);
            this.Controls.Add(this.panel1);
            this.MinimizeBox = false;
            this.Name = "CharacterEditForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Select Character Root";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.CharacterEditForm_Closing);
            this.Load += new System.EventHandler(this.CharacterEditForm_Load);
            this.panel1.ResumeLayout(false);
            this.pnlCharacterEdit.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void tvRootSel_AfterSelect(
            object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
        
        }

        private void CharacterEditForm_Closing(
            object sender, System.ComponentModel.CancelEventArgs e)
        {
            ModelRoot = tvRootSel.SelectedNode.Text;
        }

        private void CharacterEditForm_Load(object sender, System.EventArgs e)
        {
            CharacterEdit_UpdateContents();
        }

        public void CharacterEdit_UpdateContents()
        {
            // Update model root text box contents.
            if (ModelRoot == null ||
                ModelRoot == string.Empty)
            {
                tbRoot.Text = "<UNDEFINED>";
            }
            else
            {
                tbRoot.Text = ModelRoot;
            }

            // Update model root text box tool tip.
            string strToolTip = this.ttCharacterEdit.GetToolTip(this.tbRoot);
            char[] acSeparators = new char[] {'\n'};
            string[] astrLines = strToolTip.Split(acSeparators);
            this.ttCharacterEdit.SetToolTip(this.tbRoot, astrLines[0] + "\n" +
                this.tbRoot.Text);

            tbFile.Text = FullModelPath;

            // Update model path text box tool tip.
            strToolTip = this.ttCharacterEdit.GetToolTip(this.tbFile);
            astrLines = strToolTip.Split(acSeparators);
            this.ttCharacterEdit.SetToolTip(this.tbFile, astrLines[0] + "\n" +
                this.tbFile.Text);
            
            if (Scene == null)
            {
                return;
            }

            TreeNode tnSelected = new TreeNode();
            tnSelected.Text = "NOT FOUND";
            TreeNode kTreeNode = CreateTreeFromMNode(Scene, ref tnSelected);
            kTreeNode.Expand();
            if (tnSelected.Text == "NOT FOUND")
            {
                tnSelected = kTreeNode;
            }

            // Now we populate tree from kScene
            tvRootSel.Nodes.Clear();
            tvRootSel.Nodes.Add(kTreeNode);
            tnSelected.EnsureVisible();
            
            tvRootSel.SelectedNode = tnSelected;
    
            // Look for MFramework.Instance.Animation.ModelRoot
            // in treeview and expand, select, and highlight

        }

        public TreeNode CreateTreeFromMNode(MSceneObject ThisNode,
            ref TreeNode tnSelected)
        {
            bool bSelectThisNode = false;
            if (ThisNode.Name == ModelRoot)
                bSelectThisNode = true;

            MSceneObject[] ChildrenNodes = ThisNode.GetChildren();

            if (ChildrenNodes == null || ChildrenNodes.Length == 0)
            {
                TreeNode tnNew = new TreeNode(ThisNode.Name);

                if (bSelectThisNode)
                    tnSelected = tnNew;

                return tnNew;
            }
 
            TreeNode[] akChildren =    new TreeNode[ChildrenNodes.Length];

            int i=0;
            foreach (MSceneObject Child in ChildrenNodes)
            {
                akChildren[i] = CreateTreeFromMNode(Child, ref tnSelected);
                i++;
            }

            TreeNode akRoot = new TreeNode(ThisNode.Name, akChildren);

            if (bSelectThisNode)
                tnSelected = akRoot;

            return akRoot;
        }
    }
}
