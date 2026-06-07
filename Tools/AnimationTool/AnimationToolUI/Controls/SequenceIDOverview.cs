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
    /// Summary description for SequenceIDOverview.
    /// </summary>
    public class SequenceIDOverview : System.Windows.Forms.UserControl
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

        public enum ModeType
        {
            SequenceID,
            GroupID
        }
        private ModeType m_eMode;
        private ColumnSort m_kColumnSorter = new ColumnSort();
        private bool m_bEditing = false;
        private System.Windows.Forms.ListView lvSeqID;
        private System.Windows.Forms.ColumnHeader chSequenceID;
        private System.Windows.Forms.ColumnHeader chSequenceName;
        private System.Windows.Forms.ToolTip ttHelp;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;
        private int m_iIndexBeingEdited = 0;

        public ModeType Mode
        {
            get {return m_eMode;}
        }

        public SequenceIDOverview(ModeType eMode)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_eMode = eMode;
            lvSeqID.ListViewItemSorter = m_kColumnSorter;
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
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem(new string[] {
                                                                                                                     "33",
                                                                                                                     "Run Sequence"}, -1);
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem(new string[] {
                                                                                                                     "48",
                                                                                                                     "Walk Sequence"}, -1);
            this.lvSeqID = new System.Windows.Forms.ListView();
            this.chSequenceID = new System.Windows.Forms.ColumnHeader();
            this.chSequenceName = new System.Windows.Forms.ColumnHeader();
            this.ttHelp = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // lvSeqID
            // 
            this.lvSeqID.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lvSeqID.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                                                                                      this.chSequenceID,
                                                                                      this.chSequenceName});
            this.lvSeqID.FullRowSelect = true;
            this.lvSeqID.GridLines = true;
            this.lvSeqID.HideSelection = false;
            this.lvSeqID.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
                                                                                    listViewItem1,
                                                                                    listViewItem2});
            this.lvSeqID.LabelEdit = true;
            this.lvSeqID.Location = new System.Drawing.Point(0, 0);
            this.lvSeqID.MultiSelect = false;
            this.lvSeqID.Name = "lvSeqID";
            this.lvSeqID.Size = new System.Drawing.Size(512, 160);
            this.lvSeqID.TabIndex = 0;
            this.ttHelp.SetToolTip(this.lvSeqID, "Double click on an \'ID\' field to modify Sequence IDs. \r\nPress the \'Tab\' key to sk" +
                "ip an entry. Press \'Esc\' or select off the \r\ncurrent entry to leave edit mode.");
            this.lvSeqID.View = System.Windows.Forms.View.Details;
            this.lvSeqID.DoubleClick += new System.EventHandler(this.lvSeqID_DoubleClick);
            this.lvSeqID.AfterLabelEdit += new System.Windows.Forms.LabelEditEventHandler(this.lvSeqID_AfterLabelEdit);
            this.lvSeqID.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.lvSeqID_ColumnClick);
            this.lvSeqID.BeforeLabelEdit += new System.Windows.Forms.LabelEditEventHandler(this.lvSeqID_BeforeLabelEdit);
            // 
            // chSequenceID
            // 
            this.chSequenceID.Text = "ID";
            this.chSequenceID.Width = 42;
            // 
            // chSequenceName
            // 
            this.chSequenceName.Text = "Sequence Name";
            this.chSequenceName.Width = 293;
            // 
            // ttHelp
            // 
            this.ttHelp.AutoPopDelay = 50000;
            this.ttHelp.InitialDelay = 500;
            this.ttHelp.ReshowDelay = 100;
            this.ttHelp.ShowAlways = true;
            // 
            // SequenceIDOverview
            // 
            this.Controls.Add(this.lvSeqID);
            this.DockPadding.Left = 4;
            this.DockPadding.Right = 4;
            this.Name = "SequenceIDOverview";
            this.Size = new System.Drawing.Size(512, 160);
            this.Load += new System.EventHandler(this.SequenceIDOverview_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void SequenceIDOverview_Load(object sender, System.EventArgs e)
        {
            RegisterDelegates();
            UpdateContents();

            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.None, null);

            ColumnSort.SortOnColumn(lvSeqID,1,m_kColumnSorter);
        }

        public void UpdateContents()
        {
            switch (m_eMode)
            {
                case ModeType.SequenceID:
                    chSequenceName.Text = "Sequence Name";
                    ListViewUtil.RefreshListWith_SequenceID_Name(ref lvSeqID);
                    break;
                case ModeType.GroupID:
                    chSequenceName.Text = "Sequence Group Name";
                    ListViewUtil.RefreshListWith_GroupID_Name(ref lvSeqID);
                    break;
            }
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            Keys keyCode = keyData & Keys.KeyCode;
            if (m_bEditing)
            {
                switch (keyCode)
                {
                    case Keys.Escape:
                        m_bEditing = false;
                        break;
                    case Keys.Tab:
                        if ((keyData & Keys.Shift) != 0)
                        {
                            ListViewUtil.SelectPrevious(ref lvSeqID);
                        }
                        else
                        {
                            ListViewUtil.SelectNext(ref lvSeqID);
                        }
                        break;
                }
            }
            else
            {
                switch (keyCode)
                {
                    case Keys.F2:
                        lvSeqID.SelectedItems[0].BeginEdit();
                        break;
                }
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }
        
        private void lvSeqID_ColumnClick(object sender,
            System.Windows.Forms.ColumnClickEventArgs e)
        {
            ColumnSort.ListView_ColumnClick(sender, e, m_kColumnSorter);
        }

        private void lvSeqID_DoubleClick(object sender, System.EventArgs e)
        {
            if (lvSeqID.SelectedItems.Count > 0)
            {
                lvSeqID.SelectedItems[0].BeginEdit();
            }
        }

        private void lvSeqID_BeforeLabelEdit(object sender,
            System.Windows.Forms.LabelEditEventArgs e)
        {
            m_bEditing = true;
            m_iIndexBeingEdited = lvSeqID.SelectedIndices[0];
        }

        private void lvSeqID_AfterLabelEdit(object sender,
            System.Windows.Forms.LabelEditEventArgs e)
        {
            m_bEditing = false;

            ListViewItem kItem = lvSeqID.Items[m_iIndexBeingEdited];
            string strOldID = kItem.Text;
            string strNewID = e.Label;

            if (strNewID == null || strOldID == strNewID)
            {
                e.CancelEdit = true;
                return;
            }

            for (int i = 0; i < strNewID.Length; i++)
            {
                bool bResult = Char.IsDigit(strNewID, i);

                if (!bResult)
                {
                    // Failed!
                    e.CancelEdit = true;
                    return;
                }
            }

            uint uiNewID;
            
            try
            {
                uiNewID = Convert.ToUInt32(strNewID);
            }
            catch
            {
                e.CancelEdit = true;
                return;
            }

            switch (m_eMode)
            {
                case ModeType.SequenceID:
                {
                    if (ProjectData.DoesSeqIDExist(uiNewID))
                    {
                        MSequence kCurrentIDOwner = 
                            MFramework.Instance.Animation.GetSequence(uiNewID);
                        string [] aStrings = new string[2];
                        aStrings[0] = uiNewID.ToString();
                        aStrings[1] = kCurrentIDOwner.Name;
                        MessageBoxManager.DoMessageBox(
                            "SequenceIDAlreadyInUse.rtf",
                            "Sequence ID Warning", MessageBoxButtons.OK,
                            MessageBoxIcon.Warning, aStrings);
                        e.CancelEdit = true;
                        return;
                    }
                    MSequence seq = (MSequence) kItem.Tag;
                    seq.SequenceID = uiNewID;
                    if (seq.SequenceID != uiNewID)
                    {
                        e.CancelEdit = true;
                        return;
                    }
                    break;
                }
                case ModeType.GroupID:
                {
                    if (ProjectData.DoesGroupIDExist(uiNewID))
                    {
                        MSequenceGroup kCurrentIDOwner = 
                            MFramework.Instance.Animation.GetSequenceGroup(
                                uiNewID);
                        string [] aStrings = new string[2];
                        aStrings[0] = uiNewID.ToString();
                        aStrings[1] = kCurrentIDOwner.Name;
                        MessageBoxManager.DoMessageBox(
                            "GroupIDAlreadyInUse.rtf",
                            "Group ID Warning", MessageBoxButtons.OK,
                            MessageBoxIcon.Warning, aStrings);
                        e.CancelEdit = true;
                        return;
                    }
                    MSequenceGroup group = (MSequenceGroup) kItem.Tag;
                    group.GroupID = uiNewID;
                    if (group.GroupID != uiNewID)
                    {
                        e.CancelEdit = true;
                        return;
                    }
                    break;
                }
            }

            ListViewUtil.SelectNext(ref lvSeqID);
        }
    }
}
