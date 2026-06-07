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
using System.Text.RegularExpressions;
using System.Diagnostics;
using System.IO;
using WeifenLuo.WinFormsUI;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for ProjectViewForm.
    /// </summary>
    public class ProjectViewForm : DockContent
    {
        private System.ComponentModel.IContainer components;

        private int m_iOldNodeIndex = -1; 
        private ProjectTreeNode LastDragOverTarget;
        private float m_fLastDragOverBeginTime;
        private System.Windows.Forms.TreeView tvProjectView;
        private System.Windows.Forms.ContextMenu cmProjectView;
        private System.Windows.Forms.MenuItem miCreateNewFolder;
        private System.Windows.Forms.MenuItem miDeleteFolder;
        private System.Windows.Forms.MenuItem miRenameFolder;
        private System.Windows.Forms.MenuItem miAddSequences;
        private System.Windows.Forms.MenuItem miCreateSequenceGroup;
        private System.Windows.Forms.MenuItem miDeleteSequence;
        private System.Windows.Forms.MenuItem miRenameSequenceGroup;
        private System.Windows.Forms.MenuItem miDeleteSequenceGroup;
        private System.Windows.Forms.MenuItem miSeparator;
        private System.Windows.Forms.ImageList ilIcons;
        private System.Windows.Forms.ErrorProvider epProjectView;
        private System.Windows.Forms.ToolTip ttProjectView;
        private System.Windows.Forms.OpenFileDialog ofdKFFile;
        private System.Windows.Forms.MenuItem miSelectModel;
        private System.Windows.Forms.MenuItem miSelectModelRoot;
        private System.Windows.Forms.MenuItem miHelpRoot;
        private System.Windows.Forms.MenuItem miSeparator2;
        private System.Windows.Forms.DragDropEffects m_eLastDragDropEffect;

        public ProjectViewForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            LastDragOverTarget = null;
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ProjectViewForm));
            this.tvProjectView = new System.Windows.Forms.TreeView();
            this.cmProjectView = new System.Windows.Forms.ContextMenu();
            this.miCreateNewFolder = new System.Windows.Forms.MenuItem();
            this.miDeleteFolder = new System.Windows.Forms.MenuItem();
            this.miRenameFolder = new System.Windows.Forms.MenuItem();
            this.miAddSequences = new System.Windows.Forms.MenuItem();
            this.miCreateSequenceGroup = new System.Windows.Forms.MenuItem();
            this.miDeleteSequence = new System.Windows.Forms.MenuItem();
            this.miRenameSequenceGroup = new System.Windows.Forms.MenuItem();
            this.miDeleteSequenceGroup = new System.Windows.Forms.MenuItem();
            this.miSelectModel = new System.Windows.Forms.MenuItem();
            this.miSelectModelRoot = new System.Windows.Forms.MenuItem();
            this.miSeparator = new System.Windows.Forms.MenuItem();
            this.miHelpRoot = new System.Windows.Forms.MenuItem();
            this.ilIcons = new System.Windows.Forms.ImageList(this.components);
            this.epProjectView = new System.Windows.Forms.ErrorProvider();
            this.ttProjectView = new System.Windows.Forms.ToolTip(this.components);
            this.ofdKFFile = new System.Windows.Forms.OpenFileDialog();
            this.miSeparator2 = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();
            // 
            // tvProjectView
            // 
            this.tvProjectView.AllowDrop = true;
            this.tvProjectView.ContextMenu = this.cmProjectView;
            this.tvProjectView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tvProjectView.HideSelection = false;
            this.tvProjectView.ImageList = this.ilIcons;
            this.tvProjectView.LabelEdit = true;
            this.tvProjectView.Location = new System.Drawing.Point(0, 0);
            this.tvProjectView.Name = "tvProjectView";
            this.tvProjectView.Size = new System.Drawing.Size(240, 550);
            this.tvProjectView.TabIndex = 0;
            this.tvProjectView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tvProjectView_KeyDown);
            this.tvProjectView.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tvProjectView_MouseDown);
            this.tvProjectView.DragOver += new System.Windows.Forms.DragEventHandler(this.tvProjectView_DragOver);
            this.tvProjectView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvProjectView_AfterSelect);
            this.tvProjectView.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.tvProjectView_AfterLabelEdit);
            this.tvProjectView.DragEnter += new System.Windows.Forms.DragEventHandler(this.tvProjectView_DragEnter);
            this.tvProjectView.MouseMove += new System.Windows.Forms.MouseEventHandler(this.tvProjectView_MouseMove);
            this.tvProjectView.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.tvProjectView_ItemDrag);
            this.tvProjectView.BeforeLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.tvProjectView_BeforeLabelEdit);
            this.tvProjectView.DragDrop += new System.Windows.Forms.DragEventHandler(this.tvProjectView_DragDrop);
            // 
            // cmProjectView
            // 
            this.cmProjectView.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                          this.miCreateNewFolder,
                                                                                          this.miDeleteFolder,
                                                                                          this.miRenameFolder,
                                                                                          this.miAddSequences,
                                                                                          this.miCreateSequenceGroup,
                                                                                          this.miDeleteSequence,
                                                                                          this.miRenameSequenceGroup,
                                                                                          this.miDeleteSequenceGroup,
                                                                                          this.miSelectModel,
                                                                                          this.miSelectModelRoot,
                                                                                          this.miSeparator,
                                                                                          this.miHelpRoot,
                                                                                          this.miSeparator2});
            this.cmProjectView.Popup += new System.EventHandler(this.cmProjectView_Popup);
            // 
            // miCreateNewFolder
            // 
            this.miCreateNewFolder.Index = 0;
            this.miCreateNewFolder.Text = "Create &New Folder";
            this.miCreateNewFolder.Click += new System.EventHandler(this.miCreateNewFolder_Click);
            // 
            // miDeleteFolder
            // 
            this.miDeleteFolder.Index = 1;
            this.miDeleteFolder.Text = "&Delete Folder";
            this.miDeleteFolder.Click += new System.EventHandler(this.miDeleteFolder_Click);
            // 
            // miRenameFolder
            // 
            this.miRenameFolder.Index = 2;
            this.miRenameFolder.Text = "&Rename Folder";
            this.miRenameFolder.Click += new System.EventHandler(this.miRenameFolder_Click);
            // 
            // miAddSequences
            // 
            this.miAddSequences.Index = 3;
            this.miAddSequences.Text = "&Add Sequences from KF File(s)...";
            this.miAddSequences.Click += new System.EventHandler(this.miAddSequences_Click);
            // 
            // miCreateSequenceGroup
            // 
            this.miCreateSequenceGroup.Index = 4;
            this.miCreateSequenceGroup.Text = "Create New Sequence &Group";
            this.miCreateSequenceGroup.Click += new System.EventHandler(this.miCreateSequenceGroup_Click);
            // 
            // miDeleteSequence
            // 
            this.miDeleteSequence.Index = 5;
            this.miDeleteSequence.Text = "&Delete Sequence";
            this.miDeleteSequence.Click += new System.EventHandler(this.miDeleteSequence_Click);
            // 
            // miRenameSequenceGroup
            // 
            this.miRenameSequenceGroup.Index = 6;
            this.miRenameSequenceGroup.Text = "&Rename Sequence Group";
            this.miRenameSequenceGroup.Click += new System.EventHandler(this.miRenameSequenceGroup_Click);
            // 
            // miDeleteSequenceGroup
            // 
            this.miDeleteSequenceGroup.Index = 7;
            this.miDeleteSequenceGroup.Text = "&Delete Sequence Group";
            this.miDeleteSequenceGroup.Click += new System.EventHandler(this.miDeleteSequenceGroup_Click);
            // 
            // miSelectModel
            // 
            this.miSelectModel.Index = 8;
            this.miSelectModel.Text = "Select &Model...";
            this.miSelectModel.Click += new System.EventHandler(this.miSelectModel_Click);
            // 
            // miSelectModelRoot
            // 
            this.miSelectModelRoot.Index = 9;
            this.miSelectModelRoot.Text = "Select Model &Root...";
            this.miSelectModelRoot.Click += new System.EventHandler(this.miSelectModelRoot_Click);
            // 
            // miSeparator
            // 
            this.miSeparator.Index = 10;
            this.miSeparator.Text = "-";
            // 
            // miHelpRoot
            // 
            this.miHelpRoot.Index = 11;
            this.miHelpRoot.Text = "Help...";
            this.miHelpRoot.Click += new System.EventHandler(this.miHelpRoot_Click);
            // 
            // ilIcons
            // 
            this.ilIcons.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
            this.ilIcons.ImageSize = new System.Drawing.Size(16, 16);
            this.ilIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilIcons.ImageStream")));
            this.ilIcons.TransparentColor = System.Drawing.Color.Fuchsia;
            // 
            // epProjectView
            // 
            this.epProjectView.ContainerControl = this;
            // 
            // ttProjectView
            // 
            this.ttProjectView.AutoPopDelay = 110000;
            this.ttProjectView.InitialDelay = 500;
            this.ttProjectView.ReshowDelay = 100;
            // 
            // ofdKFFile
            // 
            this.ofdKFFile.DefaultExt = "kf";
            this.ofdKFFile.Filter = "KF files|*.kf";
            this.ofdKFFile.Title = "Select KF Files";
            // 
            // miSeparator2
            // 
            this.miSeparator2.Index = 12;
            this.miSeparator2.Text = "-";
            // 
            // ProjectViewForm
            // 
            this.AllowDrop = true;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(240, 550);
            this.Controls.Add(this.tvProjectView);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ProjectViewForm";
            this.Text = "Project View";
            this.Load += new System.EventHandler(this.ProjectViewForm_Load);
            this.TextChanged += new System.EventHandler(this.ProjectViewForm_TextChanged);
            this.ResumeLayout(false);

        }
        #endregion

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
        }

        private void ProjectViewForm_Load(object sender, System.EventArgs e)
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);

            AppEvents.FrameworkDataChanged +=
                new AppEvents.EventHandler_FrameworkDataChanged(
                OnFrameworkDataChangedHandler);
            ProjectData.Create();

            AppEvents.ProjectViewSelectionChanged +=
                new AnimationToolUI.AppEvents.
                EventHandler_ProjectViewSelectionChanged(
                AppEvents_ProjectViewSelectionChanged);

            AppEvents.ProjectViewNewFolder +=
                new AppEvents.EventHandler_ProjectViewNewFolder(OnNewFolder);

            AppEvents.TransitionViewSelectionChanged +=
                new AppEvents.EventHandler_TransitionViewSelectionChanged(
                this.OnTransitionViewSelectionChanged);

            if (!HelpManager.HelpExists())
                miHelpRoot.Enabled = false;

            MFramework.Instance.Animation.OnModelPathAndRootChanged +=
                new NiManagedToolInterface.
                MAnimation.__Delegate_OnModelPathAndRootChanged(
                Animation_OnModelPathAndRootChanged);
        }

        private void OnTransitionViewSelectionChanged(ArrayList sel)
        {
            if (tvProjectView.SelectedNode != ProjectData.Instance
                .GetTransitionsPreviewNode())
            {
                tvProjectView.SelectedNode = ProjectData.Instance
                    .GetTransitionsPreviewNode();
            }
        }

        
        private void Animation_OnModelPathAndRootChanged(
            string strNewModelPath, string strNewModelRoot)
        {
            ProjectTreeNode kCharNode = 
                ProjectData.Instance.GetRootCharacterNode();
            if (tvProjectView.SelectedNode != kCharNode)
                tvProjectView.SelectedNode = kCharNode;
        }

        private void tvProjectView_KeyDown(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Delete:
                    OnDelete();
                    break;
                case Keys.F1:
                    miHelpRoot_Click(this, null);
                    break;
                case Keys.F2:
                    if (tvProjectView.SelectedNode != null)
                    {
                        tvProjectView.SelectedNode.BeginEdit();
                    }
                    break;
            }
        }

        public void OnNewFolder()
        {
            ProjectTreeNode ptn = (ProjectTreeNode) tvProjectView.SelectedNode;
            int iFolderCnt = 0;
            foreach (ProjectTreeNode ptnChild in ptn.Nodes)
            {
                if (ptnChild.IsFolder())
                {
                    iFolderCnt++;
                }
            }
            string strFolderName = "NewFolder";
            if (iFolderCnt > 0)
            {
                strFolderName += iFolderCnt.ToString();
            }
            ProjectTreeNode kNewFolder = ProjectData.CreateNewFolder(
                strFolderName, ptn);

            if (kNewFolder == null)
                return;

            ptn.Expand();
            tvProjectView.SelectedNode = kNewFolder;
            kNewFolder.BeginEdit();
        }

        public void OnDelete()
        {
            ProjectTreeNode node = 
                (ProjectTreeNode) tvProjectView.SelectedNode;
            if (node.CanDelete() == false)
            {
                MLogger.LogWarning("Cannot delete \"" + node.Text + "\".");
                return;
            }
            TreeNode kNext = node.NextNode;
            if (kNext == null)
            {
                kNext = node.Parent;
            }
            if (node.Delete())
            {
                // Deletion Successful
                tvProjectView.SelectedNode = kNext;
                //AppEvents.RaiseFrameworkDataChanged();
            }
        }

        private void OnFrameworkDataChangedHandler()
        {
            try
            {
                PopulateTreeView();
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void PopulateTreeView()
        {
            Debug.Assert(tvProjectView != null);
            ProjectData.PopulateTreeView(tvProjectView);
        }

        private void tvProjectView_AfterSelect(object sender,
            System.Windows.Forms.TreeViewEventArgs e)
        {
            ProjectTreeNode node = (ProjectTreeNode) e.Node;
            NodeType nodeType = node.NodeType;
            
            AppEvents.RaiseProjectViewSelectionChanged(node);
        }

        private void AppEvents_ProjectViewSelectionChanged(
            ProjectTreeNode kNode)
        {
            if (tvProjectView.SelectedNode != kNode)
                tvProjectView.SelectedNode = kNode;
        }

        #region ContextMenu Creation

        private void CreateContextMenu_Model()
        {
            if (MFramework.Instance.Animation.FullModelPath != string.Empty)
            {
                cmProjectView.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[]
                    {
                        miSelectModel,
                        miSelectModelRoot,
                        miSeparator,
                        miHelpRoot
                    });
            }
            else
            {
                cmProjectView.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[]
                    {
                        miSelectModel,
                        miSeparator,
                        miHelpRoot
                    });
            }


        }

        private void CreateContextMenu_Sequence()
        {
            cmProjectView.MenuItems.AddRange(
                new System.Windows.Forms.MenuItem[]
                        {
                            miDeleteSequence,
                            miAddSequences,
                            miSeparator,
                            miHelpRoot
                        });
        }

        private void CreateContextMenu_SequenceGroup()
        {
            cmProjectView.MenuItems.AddRange(
                new System.Windows.Forms.MenuItem[]
                        {
                            miRenameSequenceGroup,
                            miDeleteSequenceGroup,
                            miCreateSequenceGroup,
                            miSeparator,
                            miHelpRoot
                        });
        }

        private void CreateContextMenu_SequenceFolderRoot()
        {
            if (MFramework.Instance.Animation.ModelPath == string.Empty ||
                MFramework.Instance.Animation.ModelRoot == string.Empty)
            {
                cmProjectView.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[]
                    {
                        miCreateNewFolder,
                        miSeparator,
                        miHelpRoot
                    });
            }
            else
            {
                cmProjectView.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[]
                    {
                        miAddSequences,
                        miSeparator,
                        miCreateNewFolder,
                        miSeparator2,
                        miHelpRoot
                    });
            }
        }

        private void CreateContextMenu_SequenceFolder()
        {
            if (MFramework.Instance.Animation.ModelPath == string.Empty ||
                MFramework.Instance.Animation.ModelRoot == string.Empty)
            {
                cmProjectView.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[] 
                    {
                        miCreateNewFolder,
                        miDeleteFolder,
                        miRenameFolder,
                        miSeparator,
                        miHelpRoot
                    });
            }
            else
            {
                cmProjectView.MenuItems.AddRange(
                    new System.Windows.Forms.MenuItem[] 
                    {
                        miAddSequences,
                        miSeparator,
                        miCreateNewFolder,
                        miDeleteFolder,
                        miRenameFolder,
                        miSeparator2,
                        miHelpRoot
                    });
            }
        }

        private void CreateContextMenu_SequenceGroupFolder()
        {
            cmProjectView.MenuItems.AddRange(
                new System.Windows.Forms.MenuItem[]
                {
                    miCreateSequenceGroup,
                    miSeparator,
                    miCreateNewFolder,
                    miDeleteFolder,
                    miRenameFolder,
                    miSeparator2,
                    miHelpRoot
                });
        }

        private void CreateContextMenu_SequenceGroupFolderRoot()
        {
            cmProjectView.MenuItems.AddRange(
                new System.Windows.Forms.MenuItem[]
                {
                    miCreateSequenceGroup,
                    miSeparator,
                    miCreateNewFolder,
                    miSeparator2,
                    miHelpRoot
                });
        }

        /// <summary>
        /// ContextMenu Creation dispatcher
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cmProjectView_Popup(object sender, System.EventArgs e)
        {
            if (tvProjectView.SelectedNode == null)
            {
                return;
            }

            ProjectTreeNode kPTN = 
                (ProjectTreeNode) tvProjectView.SelectedNode;
            NodeType nodeType = kPTN.NodeType; 

            cmProjectView.MenuItems.Clear();

            switch(nodeType)
            {
                case NodeType.CharacterModelRoot:
                    CreateContextMenu_Model();
                    break;               
                case NodeType.SequenceFolderRoot: 
                    CreateContextMenu_SequenceFolderRoot(); 
                    break;
                case NodeType.SequenceFolder: 
                    CreateContextMenu_SequenceFolder(); 
                    break;
                case NodeType.Sequence: 
                    CreateContextMenu_Sequence();
                    break;
                case NodeType.SequenceGroupFolderRoot:
                    CreateContextMenu_SequenceGroupFolderRoot();
                    break;
                case NodeType.SequenceGroupFolder:
                    CreateContextMenu_SequenceGroupFolder(); 
                    break;
                case NodeType.SequenceGroup: 
                    CreateContextMenu_SequenceGroup(); 
                    break;
                case NodeType.Undefined:
                    break;
                case NodeType.SequenceIDOverviewRoot:
                    break;
                case NodeType.InteractivePreviewRoot:
                    break;
            }
        }

        #endregion

        #region Label Editing
        private void tvProjectView_AfterLabelEdit(
            object sender, System.Windows.Forms.NodeLabelEditEventArgs e)
        {
            if (e.Label != null)
            {
                if(e.Label.Length > 0)
                {
                    bool bValid = false;
                    ProjectTreeNode kNode = (ProjectTreeNode) e.Node;
                    if (kNode.IsFolder() || kNode.Tag is MSequenceGroup)
                    {
                        bValid = ValidateName(e.Label, kNode, (Control)
                            sender);
                    }

                    if (bValid)
                    {
                        kNode.Text = e.Label;
                        if (kNode.Tag is MSequenceGroup)
                        {
                            ((MSequenceGroup) kNode.Tag).Name = e.Label;
                        }
                        return;
                    }
                }

                e.CancelEdit = true;
                e.Node.BeginEdit();
            }
        }

        private bool ValidateName(string strLabel, ProjectTreeNode kPTN,
            Control ctrl)
        {
            if (strLabel == null)
            {
                // name hasn't changed... for now, true
                return false;
            }

            Regex kAlphaNumeric = new Regex(@"^[a-zA-Z0-9]");

            if (kAlphaNumeric.IsMatch(strLabel) == false)
            {
                epProjectView.SetError(ctrl,"Name must be alphanumeric.");
                return false;
            }
            else
            {
                if (kPTN.IsFolder() &&
                    ((ProjectTreeNode) kPTN.Parent).DoesFolderExist(strLabel))
                {
                    epProjectView.SetError(ctrl, "Folder Name " +
                        "must be unique. The folder \"" + strLabel + "\" " +
                        "already exists.");
                    return false;
                }
                else if (kPTN.Tag is MSequenceGroup &&
                    MFramework.Instance.Animation.FindUnusedSequenceGroupName
                    (strLabel) != strLabel)
                {
                    epProjectView.SetError(ctrl, "Sequence " +
                        "group name must be unique. The sequence group \"" +
                        strLabel + "\" already exists.");
                    return false;
                }
            }

            epProjectView.SetError(ctrl,"");
            return true;
        }

        private void tvProjectView_BeforeLabelEdit(object sender,
            System.Windows.Forms.NodeLabelEditEventArgs e)
        {
            ProjectTreeNode ptn = (ProjectTreeNode) e.Node;

            if (ptn.IsRoot() || ptn.IsUndefined())
            {
                // Nope..
                e.CancelEdit = true;
                return;
            }

            if (ptn.Tag is MSequenceGroup)
            {
                e.CancelEdit = false;
            }
            else if (ptn.IsFolder() == false)
            {
                e.CancelEdit = true;
            }
        }

        private void ProjectViewForm_TextChanged(
            object sender, System.EventArgs e)
        {
            Control ctrl = (Control)sender;
        }
        #endregion

        #region Mouse Support

        private void tvProjectView_MouseMove(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            // Support for Per node ToolTip
            ProjectTreeNode tn = (ProjectTreeNode)tvProjectView.GetNodeAt(e.X,
                e.Y);
            if(tn == null)
            {
                if (m_iOldNodeIndex != -1)
                {
                    m_iOldNodeIndex = -1;
                    ttProjectView.SetToolTip(tvProjectView, "");
                }
                return;
            }

            int iCurrentNodeIndex = tn.Index;
            if(iCurrentNodeIndex != m_iOldNodeIndex)
            {
                m_iOldNodeIndex = iCurrentNodeIndex;
                if(ttProjectView != null && ttProjectView.Active)
                    ttProjectView.Active = false; //turn it off

                string strNodeType = tn.NodeType.ToString();
                string strToolTip = "Unknown item!";
                switch (tn.NodeType)
                {
                    case NodeType.CharacterModelRoot:
                        strToolTip = "Select this node to see the " +
                            "properties for the current character model.";
                        strToolTip += "\r\nBasic Properties:";
                        strToolTip += "\r\n    NIF Filename: " +
                            Path.GetFileName(MFramework.Instance.Animation
                            .FullModelPath);
                        strToolTip += "\r\n    Selected Root: " + MFramework
                            .Instance.Animation.ModelRoot;
                        break;
                    case NodeType.SequenceFolderRoot:
                        strToolTip = "Expand this node to see the " +
                            "sequences in this project.";
                        break;
                    case NodeType.SequenceFolder:
                        strToolTip = "Expand this node to see the " +
                            "sequences in this folder.";
                        strToolTip += "\r\nDrag and drop sequences onto " +
                            "this node to add sequences to this folder.";
                        break;
                    case NodeType.Sequence:
                    {
                        MSequence seq = (MSequence) tn.Tag;
                        strToolTip = "Select this node to see the " +
                            "properties of this sequence.";
                        strToolTip += "\r\nBasic Properties:";
                        strToolTip += "\r\n    Name: " + seq.Name;
                        strToolTip += "\r\n    Sequence ID: " +
                            seq.SequenceID.ToString();
                        strToolTip += "\r\n    KF Filename: " +
                            Path.GetFileName(MFramework.Instance.Animation
                            .GetFullKFFilename(seq.SequenceID));
                        strToolTip += "\r\n    Index in KF File: " +
                            seq.AnimIndex;
                        break;
                    }
                    case NodeType.SequenceGroupFolderRoot:
                        strToolTip = "Expand this node to see the sequence " +
                            "groups in this project.";
                        break;
                    case NodeType.SequenceGroupFolder:
                        strToolTip = "Expand this node to see the sequence " +
                            "groups in this folder.";
                        strToolTip += "\r\nDrag and drop sequence groups " +
                            "onto this node to add sequence groups to this " +
                            "folder.";
                        break;
                    case NodeType.SequenceGroup:
                    {
                        MSequenceGroup group = (MSequenceGroup) tn.Tag;
                        strToolTip = "Select this node to see the " +
                            "properties of this sequence group.";
                        strToolTip += "\r\nDrag and drop Sequences onto " +
                            "this node to add sequences to this sequence " +
                            "group.";
                        strToolTip += "\r\nBasic Properties:";
                        strToolTip += "\r\n    Group ID: " +
                            group.GroupID.ToString();
                        strToolTip += "\r\n    Name: " + group.Name;
                        strToolTip += "\r\n    Sequences:";
                        if (group.SequenceInfo != null &&
                            group.SequenceInfo.Length != 0)
                        {
                            foreach (MSequenceGroup.MSequenceInfo info in
                                group.SequenceInfo)
                            {
                                strToolTip += "\r\n            " + info.Name;
                            }
                        }
                        else
                        {
                            strToolTip += " None";
                        }
                        break;
                    }
                    case NodeType.SequenceIDOverviewRoot:
                        strToolTip = "Select this node to see the sequence " +
                            "IDs for all sequences in the project.";
                        break;
                    case NodeType.GroupIDOverviewRoot:
                        strToolTip = "Select this node to see the group " +
                            "ID's for all sequence groups in the project.";
                        break;
                    case NodeType.InteractivePreviewRoot:
                        strToolTip = "Select this node to enter into " +
                            "Interactive Preview mode.";
                        strToolTip += "\r\nThis mode allows the user to " +
                            "trigger transitions from one sequence to " +
                            "another interactively.";
                        break;
                    case NodeType.TransitionsPreviewRoot:
                        strToolTip = "Select this node to enter into " +
                            "Transitions Preview mode.";
                        strToolTip += "\r\nThis mode allows the user to " +
                            "preview individual transitions and change " +
                            "their settings.";
                        break;
                }

                ttProjectView.SetToolTip(tvProjectView, strToolTip);
                ttProjectView.Active = true; //make it active so it can show
            }
        }

        private void tvProjectView_MouseDown(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Right)
            {
                tvProjectView.SelectedNode = 
                    (ProjectTreeNode) tvProjectView.GetNodeAt(e.X, e.Y);
            } 
        }

        #endregion
        
        #region Drag and Drop Support

        private void tvProjectView_ItemDrag(
            object sender, System.Windows.Forms.ItemDragEventArgs e)
        {
            // Depending on the item, we may want to move, copy, or nothing
            DoDragDrop(e.Item, DragDropEffects.Move);
        }

        private void tvProjectView_DragEnter(
            object sender, System.Windows.Forms.DragEventArgs e)
        {
            // Depending on the drop zone... we may want to
            // copy, move, or have no effect...
            e.Effect = DragDropEffects.Move;
        }

        private void tvProjectView_DragDrop(
            object sender, System.Windows.Forms.DragEventArgs e)
        {
            TreeView tree = (TreeView)sender;

            Point pt = new Point(e.X, e.Y);
            pt = tree.PointToClient(pt);

            ProjectTreeNode TargetNode = 
                (ProjectTreeNode)tree.GetNodeAt(pt);
            ProjectTreeNode MovingNode = 
                (ProjectTreeNode)e.Data.GetData(typeof(ProjectTreeNode));
            
            if (MovingNode == TargetNode)
                return;

            if (!ProjectData.MoveToFolder(TargetNode, MovingNode))
            {
                MessageBox.Show(
                    "You cannot drop the currently selected node "+
                    "into this location in the hierarchy!", 
                    "Drag and Drop Error");
                return;
            }

            TargetNode.Expand();
            tree.SelectedNode = LastDragOverTarget;
            LastDragOverTarget = null;

            return;
        }

        private void tvProjectView_DragOver(
            object sender, System.Windows.Forms.DragEventArgs e)
        {
            TreeView tree = (TreeView)sender;

            e.Effect = DragDropEffects.None;

            // Is acceptable for drop?
            if (e.Data.GetData(typeof(ProjectTreeNode)) == null)
                return;

            Point pt = new Point(e.X, e.Y);
            pt = tree.PointToClient(pt);

            ProjectTreeNode TargetNode = (ProjectTreeNode)tree.GetNodeAt(pt);

            if (TargetNode == null)
            {
                return;
            }

            if (LastDragOverTarget == TargetNode)
            {
                float fCurrentDragOverTime = MUtility.GetCurrentTimeInSec();
                if (fCurrentDragOverTime - m_fLastDragOverBeginTime > 0.50f)
                    TargetNode.Expand();
                e.Effect = m_eLastDragDropEffect;
                return;
            }

            // Is the node we are over acceptable 
            // for the type we are trying to drag?
            ProjectTreeNode MovingNode = 
                (ProjectTreeNode)e.Data.GetData(typeof(ProjectTreeNode));
            if (MovingNode.CanDropInto(TargetNode) == false)
            {
                return;
            }

            // Is the node we are moving over an child-ancestor?
            if ((TargetNode.NodeType == NodeType.SequenceFolder ||
                TargetNode.NodeType == NodeType.SequenceGroupFolder) &&
                MovingNode.ContainsInHierarchy(TargetNode) == true)
            {
                return;
            }

            e.Effect = DragDropEffects.Move;
            LastDragOverTarget = TargetNode;
            m_fLastDragOverBeginTime = MUtility.GetCurrentTimeInSec();
            m_eLastDragDropEffect = e.Effect;

            return;
        }
        #endregion

        #region Menu Item Handlers

        private void miCreateNewFolder_Click(object sender, System.EventArgs e)
        {
            AppEvents.RaiseProjectViewNewFolder();
        }
        
        private void miDeleteFolder_Click(object sender, System.EventArgs e)
        {
            OnDelete();
        }

        private void miRenameFolder_Click(object sender, System.EventArgs e)
        {
            if (tvProjectView.SelectedNode != null)
            {
                tvProjectView.SelectedNode.BeginEdit();
            }
        }

        private void miAddSequences_Click(object sender, System.EventArgs e)
        {
            ProjectTreeNode ptnSelected = (ProjectTreeNode)
                tvProjectView.SelectedNode;
            if (ptnSelected.NodeType == NodeType.SequenceFolder)
            {
                ProjectData.Instance.CurrentFolder = ptnSelected;
            }
            else
            {
                ProjectData.Instance.CurrentFolder = null;
            }

            AnimationHelpers.OpenMultipleKFsDialog(this);

            ProjectData.Instance.CurrentFolder = null;
        }

        private void miCreateSequenceGroup_Click(
            object sender, System.EventArgs e)
        {
            ProjectTreeNode kPTN = 
                (ProjectTreeNode) tvProjectView.SelectedNode;
            
            string strName = 
                MFramework.Instance.Animation.FindUnusedSequenceGroupName(
                "New Sequence Group");
            uint uiGroupID = 
                MFramework.Instance.Animation.FindUnusedGroupID();
            MSequenceGroup kNewGroup = 
                MFramework.Instance.Animation.AddSequenceGroup(
                    uiGroupID, strName);
            Debug.Assert(kNewGroup != null,
                "Failed attempt to create new sequence group");

            switch (kPTN.NodeType)
            {
                case NodeType.SequenceGroupFolder:
                    break;
                case NodeType.SequenceGroupFolderRoot:
                    break;
                case NodeType.SequenceGroup:
                    kPTN = (ProjectTreeNode) kPTN.Parent;
                    break;
                default:
                    kPTN = null;
                    break;
            }

            if (kPTN != null)
            {
                ProjectTreeNode kNewNode = 
                    ProjectData.GetSequenceGroupNode(kNewGroup.GroupID);
                ProjectData.MoveToFolder(kPTN, kNewNode);
                kPTN.Expand();
                tvProjectView.SelectedNode = kNewNode;
                kNewNode.BeginEdit();
            }
        }

        private void miDeleteSequence_Click(object sender, System.EventArgs e)
        {
            if (tvProjectView.SelectedNode != null)
            {
                OnDelete();
            }
        }

        private void miRenameSequenceGroup_Click(
            object sender, System.EventArgs e)
        {
            if (tvProjectView.SelectedNode != null)
            {
                tvProjectView.SelectedNode.BeginEdit();
            }
        }

        private void miDeleteSequenceGroup_Click(
            object sender, System.EventArgs e)
        {
            if (tvProjectView.SelectedNode != null)
            {
                OnDelete();
            }
        }

        private void miSelectModel_Click(object sender, System.EventArgs e)
        {
            AnimationHelpers.OpenModelDialog(this);
        }

        private void miSelectModelRoot_Click(object sender, System.EventArgs e)
        {
            AnimationHelpers.DoModelRootDialog(this);
        }
        
        private void miHelpRoot_Click(object sender, System.EventArgs e)
        {
            if (HelpManager.HelpExists())
                HelpManager.DoHelp(this,
                    @"User_Interface_Description/Project_View.htm");       
        }
        #endregion

    }
}
