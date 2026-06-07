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
    /// Summary description for KeystrokeBindings.
    /// </summary>
    public class KeystrokeBindings : 
        System.Windows.Forms.UserControl, IPostShow
    {
        #region Delegates

        private AppEvents.EventHandler_KeystrokeBindingsChanged 
            m_KeystrokeBindingsChanged = null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_KeystrokeBindingsChanged = 
                new AppEvents.EventHandler_KeystrokeBindingsChanged(
                OnKeystrokeBindingsChanged);
            
            // Register delegates.
            AppEvents.KeystrokeBindingsChanged += m_KeystrokeBindingsChanged;
        }

        private void UnregisterDelegates()
        {
            if (m_KeystrokeBindingsChanged != null)
                AppEvents.KeystrokeBindingsChanged -= 
                    m_KeystrokeBindingsChanged;
        }

        #endregion

        private System.ComponentModel.IContainer m_kComponents =
            new System.ComponentModel.Container();

        private ColumnSort m_kColumnSorter = new ColumnSort();
        private bool m_bEditing = false;
        private System.ComponentModel.IContainer components;
        private string m_strCapturedKey = "";
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Splitter splitter1;

        private Hashtable m_HashTable;
        private System.Windows.Forms.MenuItem miRemoveKeystrokeBindings;
        private System.Windows.Forms.ListView lvKeystroke;
        private System.Windows.Forms.ColumnHeader chKeystroke;
        private System.Windows.Forms.ColumnHeader chModifiers;
        private System.Windows.Forms.ColumnHeader chSequences;
        private System.Windows.Forms.ContextMenu cmKeystroke;
        private System.Windows.Forms.ToolTip ttKeystroke;
        private Size m_szDefaultSizeOfPanel;
        private static uint NumInstances = 0;

        public KeystrokeBindings(uint uiGroupID)
        {
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            InitializeComponent();
            
            m_HashTable = ProjectData.GetSeqInputInfoForGroup(uiGroupID);
        }

        public KeystrokeBindings(Hashtable HashTable)
        {
            Debug.Assert(NumInstances == 0);
            NumInstances++;

            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            m_HashTable = HashTable;
        }

        public void SetHashTableBySequenceGroup(uint uiGroupID)
        {
            Hashtable ht = 
                ProjectData.GetSeqInputInfoForGroup(uiGroupID);

            m_HashTable = ht;
            
            KeyStroke_UpdateContents();
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
                if(m_kComponents != null)
                {
                    m_kComponents.Dispose();
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
                                                                                                                     "KEY_A",
                                                                                                                     "CTRL + ALT",
                                                                                                                     "Run Sequence"}, -1);
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem(new string[] {
                                                                                                                     "KEY_UP",
                                                                                                                     "CTRL",
                                                                                                                     "Walk Sequence"}, -1);
            this.lvKeystroke = new System.Windows.Forms.ListView();
            this.chKeystroke = new System.Windows.Forms.ColumnHeader();
            this.chModifiers = new System.Windows.Forms.ColumnHeader();
            this.chSequences = new System.Windows.Forms.ColumnHeader();
            this.cmKeystroke = new System.Windows.Forms.ContextMenu();
            this.miRemoveKeystrokeBindings = new System.Windows.Forms.MenuItem();
            this.ttKeystroke = new System.Windows.Forms.ToolTip(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.SuspendLayout();
            // 
            // lvKeystroke
            // 
            this.lvKeystroke.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lvKeystroke.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                                                                                          this.chKeystroke,
                                                                                          this.chModifiers,
                                                                                          this.chSequences});
            this.lvKeystroke.ContextMenu = this.cmKeystroke;
            this.lvKeystroke.FullRowSelect = true;
            this.lvKeystroke.GridLines = true;
            this.lvKeystroke.HideSelection = false;
            this.lvKeystroke.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
                                                                                        listViewItem1,
                                                                                        listViewItem2});
            this.lvKeystroke.LabelEdit = true;
            this.lvKeystroke.Location = new System.Drawing.Point(0, 0);
            this.lvKeystroke.MultiSelect = false;
            this.lvKeystroke.Name = "lvKeystroke";
            this.lvKeystroke.Size = new System.Drawing.Size(288, 245);
            this.lvKeystroke.TabIndex = 0;
            this.ttKeystroke.SetToolTip(this.lvKeystroke, "Double click on a \'Keystroke\' field to modify keystroke bindings.\nPress the \'Tab\'" +
                " key to skip an entry. Press \'Esc\' or select off the\ncurrent entry to leave edit" +
                " mode.");
            this.lvKeystroke.View = System.Windows.Forms.View.Details;
            // 
            // chKeystroke
            // 
            this.chKeystroke.Text = "Keystroke";
            this.chKeystroke.Width = 71;
            // 
            // chModifiers
            // 
            this.chModifiers.Text = "Modifiers";
            this.chModifiers.Width = 62;
            // 
            // chSequences
            // 
            this.chSequences.Text = "Sequences";
            this.chSequences.Width = 98;
            // 
            // cmKeystroke
            // 
            this.cmKeystroke.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                        this.miRemoveKeystrokeBindings});
            // 
            // miRemoveKeystrokeBindings
            // 
            this.miRemoveKeystrokeBindings.Index = 0;
            this.miRemoveKeystrokeBindings.Text = "&Remove Keystroke Bindings";
            this.miRemoveKeystrokeBindings.Click += new System.EventHandler(this.miRemoveKeystrokeBindings_Click);
            // 
            // ttKeystroke
            // 
            this.ttKeystroke.AutoPopDelay = 555000;
            this.ttKeystroke.InitialDelay = 500;
            this.ttKeystroke.ReshowDelay = 100;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.Control;
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 253);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(288, 16);
            this.panel1.TabIndex = 2;
            // 
            // splitter1
            // 
            this.splitter1.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(0, 245);
            this.splitter1.MinExtra = 70;
            this.splitter1.MinSize = 15;
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(288, 8);
            this.splitter1.TabIndex = 1;
            this.splitter1.TabStop = false;
            this.splitter1.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoved);
            this.splitter1.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoving);
            // 
            // KeystrokeBindings
            // 
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.lvKeystroke);
            this.Name = "KeystrokeBindings";
            this.Size = new System.Drawing.Size(288, 269);
            this.Load += new System.EventHandler(this.KeystrokeBindings_Load);
            this.ResumeLayout(false);

        }
        #endregion

        
        private void ParseCapturedKey(string strCapturedKey, 
            ref string strKeyBinding, ref string strKeyModifier)
        {
            string [] split = strCapturedKey.Split(new char [] {','});

            int iCount = 0;
            foreach (string str in split)
            {
                //str.Trim();
                string strTrim = str.Trim();
                split[iCount] = strTrim;

                if (strTrim == "ShiftKey" || strTrim == "ControlKey" ||
                    strTrim == "AltKey" || strTrim == "Menu")
                {
                    // Has no effect.
                    split[iCount] = null;
                }
                iCount++;
            }

            iCount = 0;
            foreach (string str in split)
            {
                if (str == null)
                    continue;
        
                if (str == "Shift" || str == "Control" ||
                    str == "Alt")
                {
                    if (strKeyModifier == null)
                        strKeyModifier = str;
                    else
                        strKeyModifier += " + " + str;
                }
                else
                {
                    if (strKeyBinding == null)
                        strKeyBinding = str;
                    else
                        strKeyBinding += " + " + str;
                }
                iCount++;
            }            
        }

        protected override bool ProcessCmdKey(ref Message msg,
            Keys keyData)
        {
            int iCurrentIndex;

            if (m_bEditing)
            {
                m_bEditing = false;
                
                m_strCapturedKey = keyData.ToString();        
    
                iCurrentIndex = lvKeystroke.SelectedIndices[0];

                if (m_strCapturedKey == "Escape" ||
                    m_strCapturedKey == "Enter")
                {
                    return base.ProcessCmdKey(ref msg,keyData);
                }

                bool bRaiseEvent = false;
                if (m_strCapturedKey != "Tab")
                {
                    // Modify our AppData... and change akKeyStrokeMap
                    string strSequence = lvKeystroke.Items[
                        iCurrentIndex].SubItems[2].Text;

                    string strKeyBinding = null;
                    string strKeyModifier = null;
                    ParseCapturedKey(m_strCapturedKey, 
                        ref strKeyBinding, ref strKeyModifier);

                    if (strKeyBinding != null)
                    {
                        ProjectData.SeqInputInfo oldinfo =
                            FindExistingKeyDataEntry(keyData);
                        if (oldinfo != null &&
                            oldinfo.m_Seq.Name != strSequence)
                        {
                            string strOldSequence = oldinfo.m_Seq.Name;
                            string[] aStrings = new string[]
                            {
                                keyData.ToString(),
                                strOldSequence,
                                strSequence                            
                            };
                            DialogResult dr = MessageBoxManager.DoMessageBox(
                                "KeystrokeMappingAlreadyInUse.rtf",
                                "Keystroke Mapping Already In Use",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question, aStrings);
                            if (dr == DialogResult.No)
                            {
                                // This line will cause the listview to do a
                                // callback!!!
                                lvKeystroke.Items[iCurrentIndex]
                                    .Selected = true;
                                lvKeystroke.Select();
                                lvKeystroke.SelectedItems[0]
                                    .BeginEdit();

                                return true;
                            }

                            oldinfo.m_eKeyStroke = Keys.None;
                            oldinfo.m_strKeyStroke = null;
                            oldinfo.m_strModifier = null;
                            for (int i = 0;
                                i < lvKeystroke.Items.Count; i++)
                            {
                                if (lvKeystroke.Items[i].SubItems[2]
                                    .Text == strOldSequence)
                                {
                                    lvKeystroke.Items[i].SubItems[0]
                                        .Text = null;
                                    lvKeystroke.Items[i].SubItems[1]
                                        .Text = null;
                                    break;
                                }
                            }
                        }

                        lvKeystroke.Items[iCurrentIndex].Text = 
                            strKeyBinding;

                        lvKeystroke.Items[iCurrentIndex].SubItems[1]
                            .Text = strKeyModifier;

                        // Set the hash table...
                        //uint =
                        //Hashtable kHT = ProjectData.GetSeqInputInfoHash();
                        MSequence kSeq = 
                            (MSequence)(lvKeystroke.Items[iCurrentIndex].Tag);
                        uint uiID = kSeq.SequenceID;

                        ProjectData.SeqInputInfo kEntry;
                        
                        if (m_HashTable.ContainsKey(uiID) == false)
                        {
                            kEntry = new ProjectData.SeqInputInfo(kSeq);
                            m_HashTable.Add(uiID, kEntry);
                        }
                        else
                        {
                            kEntry = 
                                (ProjectData.SeqInputInfo)m_HashTable[uiID];
                        }

                        kEntry.m_strKeyStroke = strKeyBinding;
                        kEntry.m_strModifier = strKeyModifier;
                        kEntry.m_eKeyStroke = keyData;

                        bRaiseEvent = true;
                    }
                    else
                    {
                        //This line will cause the listview to do a callback!!!
                        lvKeystroke.Items[iCurrentIndex].Selected = true;
                        lvKeystroke.Select();
                        lvKeystroke.SelectedItems[0].BeginEdit();

                        // no selection made, so we just return.
                        return true; 
                    }
                    
                }

                // Goto to the next line in the list
                ListViewUtil.SelectNext(ref lvKeystroke);

                if (bRaiseEvent)
                {
                    AppEvents.RaiseKeystrokeBindingsChanged();
                }
                
                return true;
            }

            string strCapturedKey = keyData.ToString();        

            if (lvKeystroke.SelectedIndices != null &&
                lvKeystroke.SelectedIndices.Count > 0)
            {
                iCurrentIndex = lvKeystroke.SelectedIndices[0];
                if (strCapturedKey == "Enter")
                {   
                    // This allows the user to user enter to begin input mode.

                    if (lvKeystroke.SelectedItems.Count > 0)
                    {
                        //This line will cause the listview to do a callback!!!
                        lvKeystroke.Items[iCurrentIndex].Selected = true;
                        lvKeystroke.Select();
                        lvKeystroke.SelectedItems[0].BeginEdit();

                        return true;
                    }
                }
            }

            return base.ProcessCmdKey(ref msg,keyData);
        }

        private ProjectData.SeqInputInfo FindExistingKeyDataEntry(
            Keys keyData)
        {
            foreach (DictionaryEntry e in m_HashTable)
            {
                ProjectData.SeqInputInfo info = (ProjectData.SeqInputInfo)
                    e.Value;
                if (info.m_eKeyStroke == keyData)
                {
                    return info;
                }
            }

            return null;
        }

        public void KeyStroke_UpdateContents()
        {
            ListViewUtil.RefreshListWith_Key_Mod_Name(
                ref lvKeystroke, m_HashTable);
        }

        private void lvKeystroke_ColumnClick(object sender, 
            System.Windows.Forms.ColumnClickEventArgs e)
        {
            ColumnSort.ListView_ColumnClick(sender, e, m_kColumnSorter);
        }

        private void lvKeystroke_AfterLabelEdit(
            object sender, System.Windows.Forms.LabelEditEventArgs e)
        {
            m_bEditing = false;
            e.CancelEdit = true;
        }
        private void lvKeystroke_BeforeLabelEdit(
            object sender, System.Windows.Forms.LabelEditEventArgs e)
        {
            m_bEditing = true;
        }
        public void lvKeystroke_DoubleClick(object sender, System.EventArgs e)
        {
            if (lvKeystroke.SelectedItems.Count > 0)
            {
                lvKeystroke.SelectedItems[0].BeginEdit();
            }
        }

        private void KeystrokeBindings_Load(object sender, System.EventArgs e)
        {
            RegisterDelegates();
            m_szDefaultSizeOfPanel = panel1.Size;

            this.lvKeystroke.ListViewItemSorter = m_kColumnSorter;
            this.lvKeystroke.ColumnClick += 
                new System.Windows.Forms.ColumnClickEventHandler(
                this.lvKeystroke_ColumnClick);
            this.lvKeystroke.BeforeLabelEdit += 
                new System.Windows.Forms.LabelEditEventHandler(
                this.lvKeystroke_BeforeLabelEdit);
            this.lvKeystroke.AfterLabelEdit += 
                new System.Windows.Forms.LabelEditEventHandler(
                this.lvKeystroke_AfterLabelEdit);
            this.lvKeystroke.DoubleClick += 
                new System.EventHandler(this.lvKeystroke_DoubleClick);

            KeyStroke_UpdateContents();

            ColumnSort.SortOnColumn(lvKeystroke,2,this.m_kColumnSorter);
        }

        private void splitter1_SplitterMoved(
            object sender, System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoved(
                sender, this, panel1, m_szDefaultSizeOfPanel.Height);        
        }

        private void splitter1_SplitterMoving(
            object sender, System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoving(sender, this, panel1);
        }

        private void miRemoveKeystrokeBindings_Click(object sender,
            System.EventArgs e)
        {
            MSequence seq = (MSequence) lvKeystroke.SelectedItems[0]
                .Tag;
            Debug.Assert(m_HashTable.Contains(seq.SequenceID));
            ProjectData.SeqInputInfo info = (ProjectData.SeqInputInfo)
                m_HashTable[seq.SequenceID];
            info.m_eKeyStroke = Keys.None;
            info.m_strKeyStroke = null;
            info.m_strModifier = null;
            lvKeystroke.SelectedItems[0].SubItems[0].Text = null;
            lvKeystroke.SelectedItems[0].SubItems[1].Text = null;
            AppEvents.RaiseKeystrokeBindingsChanged();
        }

        private void OnKeystrokeBindingsChanged()
        {
            if (!m_bEditing)
            {
                KeyStroke_UpdateContents();
            }
        }

        public void PostShow()
        {
            if (lvKeystroke.Items.Count <= 0)
                return;

            // This PostShow is used both in sequence groups
            // and in the global keystroke settings
            int iCellOffset = 17;
            int iInitialOffset = 32;
            int iItems = lvKeystroke.Items.Count;

            // cap the items
            if (iItems < 3)
                iItems = 3;

            // This breaks down when the number of items
            // multiplied the offset is greater than the
            // control height - initialoffset. When this
            // happens, panel1.Height will equal zero.
            panel1.Height = Size.Height - 
                iInitialOffset - (iCellOffset * iItems); 

            RollBarControl.SplitterMoved(
                splitter1, this, panel1, m_szDefaultSizeOfPanel.Height);   
            Invalidate(true);
        }
    }
}
