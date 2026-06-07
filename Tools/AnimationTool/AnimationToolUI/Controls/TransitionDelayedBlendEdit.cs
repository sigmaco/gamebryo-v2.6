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
    /// Summary description for TransitionDelayedBlendEdit.
    /// </summary>
    public class TransitionDelayedBlendEdit : 
        System.Windows.Forms.UserControl, IPostShow
    {
        private ArrayList m_aTran;
        private MTransition.MBlendPair[] m_aBlendPairs;
        private MSequence.MTextKey[] m_aSrcTextKeys = null;
        private MSequence.MTextKey[] m_aDesTextKeys = null;
        private Size m_szDefaultSizeOfPanel;
        private System.Windows.Forms.ImageList ilPlusMinus;
        private System.Windows.Forms.ToolTip ttDelayedBlendEdit;
        private System.Windows.Forms.Panel pnlBottom;
        private System.Windows.Forms.Button btnRemovePair;
        private System.Windows.Forms.Button btnAddPair;
        private System.Windows.Forms.ComboBox cbSourceTags;
        private System.Windows.Forms.Label lblTimeInSource;
        private System.Windows.Forms.ComboBox cbDestTags;
        private System.Windows.Forms.TextBox tbBlendPairInfo;
        private System.Windows.Forms.Label lblTimeInDestination;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.Panel pnlTop;
        private System.Windows.Forms.TextBox tbBlendPair;
        private System.Windows.Forms.ListBox lbBlendPairs;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public TransitionDelayedBlendEdit(MTransition Tran)
        {
            ArrayList aTran = new ArrayList(1);
            aTran.Add(Tran);
            TransitionDelayedBlendEditConstructorInit(aTran);
        }

        public TransitionDelayedBlendEdit(ArrayList aTran)
        {
            TransitionDelayedBlendEditConstructorInit(aTran);
        }

        public void TransitionDelayedBlendEditConstructorInit(ArrayList aTran)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            // m_aTran is an array of MTransitions,ie the multiple
            // selection of chains.
            m_aTran = aTran;

            // For debug purposes, assert that each transition is actually
            // a delayed blend
            foreach (MTransition tran in m_aTran)
            {          
                Debug.Assert(tran.StoredType == 
                    MTransition.TransitionType.Trans_DelayedBlend);
            }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(TransitionDelayedBlendEdit));
            this.ilPlusMinus = new System.Windows.Forms.ImageList(this.components);
            this.ttDelayedBlendEdit = new System.Windows.Forms.ToolTip(this.components);
            this.btnRemovePair = new System.Windows.Forms.Button();
            this.btnAddPair = new System.Windows.Forms.Button();
            this.cbSourceTags = new System.Windows.Forms.ComboBox();
            this.cbDestTags = new System.Windows.Forms.ComboBox();
            this.lbBlendPairs = new System.Windows.Forms.ListBox();
            this.pnlBottom = new System.Windows.Forms.Panel();
            this.lblTimeInSource = new System.Windows.Forms.Label();
            this.tbBlendPairInfo = new System.Windows.Forms.TextBox();
            this.lblTimeInDestination = new System.Windows.Forms.Label();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.pnlTop = new System.Windows.Forms.Panel();
            this.tbBlendPair = new System.Windows.Forms.TextBox();
            this.pnlBottom.SuspendLayout();
            this.pnlTop.SuspendLayout();
            this.SuspendLayout();
            // 
            // ilPlusMinus
            // 
            this.ilPlusMinus.ImageSize = new System.Drawing.Size(16, 16);
            this.ilPlusMinus.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilPlusMinus.ImageStream")));
            this.ilPlusMinus.TransparentColor = System.Drawing.Color.White;
            // 
            // btnRemovePair
            // 
            this.btnRemovePair.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnRemovePair.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnRemovePair.ImageList = this.ilPlusMinus;
            this.btnRemovePair.Location = new System.Drawing.Point(95, 8);
            this.btnRemovePair.Name = "btnRemovePair";
            this.btnRemovePair.Size = new System.Drawing.Size(56, 24);
            this.btnRemovePair.TabIndex = 0;
            this.btnRemovePair.Text = "Delete";
            this.ttDelayedBlendEdit.SetToolTip(this.btnRemovePair, "Click to remove the currently selected blend pair.");
            this.btnRemovePair.Click += new System.EventHandler(this.btnRemovePair_Click);
            // 
            // btnAddPair
            // 
            this.btnAddPair.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnAddPair.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnAddPair.ImageIndex = 0;
            this.btnAddPair.Location = new System.Drawing.Point(95, 143);
            this.btnAddPair.Name = "btnAddPair";
            this.btnAddPair.Size = new System.Drawing.Size(56, 24);
            this.btnAddPair.TabIndex = 6;
            this.btnAddPair.Text = "Add";
            this.ttDelayedBlendEdit.SetToolTip(this.btnAddPair, "Click to add a new blend pair.");
            this.btnAddPair.Click += new System.EventHandler(this.btnAddPair_Click);
            // 
            // cbSourceTags
            // 
            this.cbSourceTags.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbSourceTags.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbSourceTags.Location = new System.Drawing.Point(88, 65);
            this.cbSourceTags.Name = "cbSourceTags";
            this.cbSourceTags.Size = new System.Drawing.Size(150, 21);
            this.cbSourceTags.TabIndex = 3;
            this.ttDelayedBlendEdit.SetToolTip(this.cbSourceTags, "The text key in the source sequence that indicates\nthe frame at which the delayed" +
                " blend should start.");
            this.cbSourceTags.SelectedIndexChanged += new System.EventHandler(this.cbSourceTags_SelectedIndexChanged);
            // 
            // cbDestTags
            // 
            this.cbDestTags.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbDestTags.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbDestTags.Location = new System.Drawing.Point(88, 112);
            this.cbDestTags.Name = "cbDestTags";
            this.cbDestTags.Size = new System.Drawing.Size(150, 21);
            this.cbDestTags.TabIndex = 5;
            this.ttDelayedBlendEdit.SetToolTip(this.cbDestTags, "The text key in the destination sequence that\nindicates the frame to blend to.");
            this.cbDestTags.SelectedIndexChanged += new System.EventHandler(this.cbDestTags_SelectedIndexChanged);
            // 
            // lbBlendPairs
            // 
            this.lbBlendPairs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lbBlendPairs.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbBlendPairs.IntegralHeight = false;
            this.lbBlendPairs.Location = new System.Drawing.Point(0, 24);
            this.lbBlendPairs.Name = "lbBlendPairs";
            this.lbBlendPairs.Size = new System.Drawing.Size(246, 62);
            this.lbBlendPairs.TabIndex = 1;
            this.ttDelayedBlendEdit.SetToolTip(this.lbBlendPairs, "Select a blend pair to see its information below.");
            this.lbBlendPairs.KeyUp += new System.Windows.Forms.KeyEventHandler(this.lbBlendPairs_KeyUp);
            // 
            // pnlBottom
            // 
            this.pnlBottom.Controls.Add(this.cbSourceTags);
            this.pnlBottom.Controls.Add(this.lblTimeInSource);
            this.pnlBottom.Controls.Add(this.cbDestTags);
            this.pnlBottom.Controls.Add(this.tbBlendPairInfo);
            this.pnlBottom.Controls.Add(this.lblTimeInDestination);
            this.pnlBottom.Controls.Add(this.btnRemovePair);
            this.pnlBottom.Controls.Add(this.btnAddPair);
            this.pnlBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlBottom.Location = new System.Drawing.Point(5, 99);
            this.pnlBottom.Name = "pnlBottom";
            this.pnlBottom.Size = new System.Drawing.Size(246, 184);
            this.pnlBottom.TabIndex = 1;
            // 
            // lblTimeInSource
            // 
            this.lblTimeInSource.Location = new System.Drawing.Point(0, 64);
            this.lblTimeInSource.Name = "lblTimeInSource";
            this.lblTimeInSource.Size = new System.Drawing.Size(80, 32);
            this.lblTimeInSource.TabIndex = 2;
            this.lblTimeInSource.Text = "Time in Source to Start Blend:";
            // 
            // tbBlendPairInfo
            // 
            this.tbBlendPairInfo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbBlendPairInfo.BackColor = System.Drawing.SystemColors.Info;
            this.tbBlendPairInfo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbBlendPairInfo.Location = new System.Drawing.Point(0, 40);
            this.tbBlendPairInfo.Name = "tbBlendPairInfo";
            this.tbBlendPairInfo.ReadOnly = true;
            this.tbBlendPairInfo.Size = new System.Drawing.Size(246, 20);
            this.tbBlendPairInfo.TabIndex = 1;
            this.tbBlendPairInfo.TabStop = false;
            this.tbBlendPairInfo.Text = "Create New Blend Pair";
            this.tbBlendPairInfo.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblTimeInDestination
            // 
            this.lblTimeInDestination.Location = new System.Drawing.Point(0, 104);
            this.lblTimeInDestination.Name = "lblTimeInDestination";
            this.lblTimeInDestination.Size = new System.Drawing.Size(80, 40);
            this.lblTimeInDestination.TabIndex = 4;
            this.lblTimeInDestination.Text = "Time in Destination to Blend to:";
            // 
            // splitter1
            // 
            this.splitter1.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(5, 91);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(246, 8);
            this.splitter1.TabIndex = 0;
            this.splitter1.TabStop = false;
            this.splitter1.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoved);
            this.splitter1.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoving);
            // 
            // pnlTop
            // 
            this.pnlTop.Controls.Add(this.tbBlendPair);
            this.pnlTop.Controls.Add(this.lbBlendPairs);
            this.pnlTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlTop.Location = new System.Drawing.Point(5, 5);
            this.pnlTop.Name = "pnlTop";
            this.pnlTop.Size = new System.Drawing.Size(246, 86);
            this.pnlTop.TabIndex = 0;
            // 
            // tbBlendPair
            // 
            this.tbBlendPair.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbBlendPair.BackColor = System.Drawing.SystemColors.Info;
            this.tbBlendPair.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbBlendPair.Location = new System.Drawing.Point(0, 0);
            this.tbBlendPair.Name = "tbBlendPair";
            this.tbBlendPair.ReadOnly = true;
            this.tbBlendPair.Size = new System.Drawing.Size(246, 20);
            this.tbBlendPair.TabIndex = 0;
            this.tbBlendPair.TabStop = false;
            this.tbBlendPair.Text = "Blend Pairs";
            this.tbBlendPair.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // TransitionDelayedBlendEdit
            // 
            this.Controls.Add(this.pnlTop);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.pnlBottom);
            this.DockPadding.All = 5;
            this.Name = "TransitionDelayedBlendEdit";
            this.Size = new System.Drawing.Size(256, 288);
            this.Load += new System.EventHandler(this.TransitionDelayedBlendEdit_Load);
            this.pnlBottom.ResumeLayout(false);
            this.pnlTop.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void InitDelayedBlendControls()
        {
            lbBlendPairs.Items.Clear();

            m_aBlendPairs = 
                AnimationHelpers.GetCommonBlendPairs(m_aTran);

            if (m_aBlendPairs != null)
            {
                foreach (MTransition.MBlendPair kPair in m_aBlendPairs)
                {
                    if (kPair == null)
                        continue;

                    string Text = "\"" + kPair.StartKey + "\" to \"" +
                        kPair.TargetKey + "\"";

                    lbBlendPairs.Items.Add(Text);
                }
            }

            cbSourceTags.Items.Clear();
            ArrayList aSrcTextKeys =
                AnimationHelpers.GetCommonSrcTextKeys(m_aTran);

            if (aSrcTextKeys != null)
            {
                foreach (MSequence.MTextKey key in aSrcTextKeys)
                {
                    string strMsg = key.Text + " (" + 
                        key.Time.ToString("f3") + ")";
                    cbSourceTags.Items.Add(strMsg);
                }
            }

            cbDestTags.Items.Clear();
            ArrayList aDesTextKeys =
                AnimationHelpers.GetCommonDesTextKeys(m_aTran);

            if (aDesTextKeys != null)
            {
                foreach (MSequence.MTextKey key in aDesTextKeys)
                {
                    string strMsg = key.Text + " (" + 
                        key.Time.ToString("f3") + ")";
                    cbDestTags.Items.Add(strMsg);
                }
            }

            // Default is End to Start
            int iSrcSelectedIndex = 0;
            int iDesSelectedIndex = 0;

            string strSrc = ((MSequence.MTextKey)aSrcTextKeys[0]).Text;
            string strDes = ((MSequence.MTextKey)
                             aDesTextKeys[aDesTextKeys.Count-1]).Text;

            // When blend paris are first created, they only have
            // the end, start blend pair. Here, we ensure that 
            // our combo boxes are focused on them.
            MTransition tran = (MTransition)m_aTran[0];

            m_aSrcTextKeys = new MSequence.MTextKey[aSrcTextKeys.Count];
            m_aDesTextKeys = new MSequence.MTextKey[aDesTextKeys.Count];

            for (int i=0; i< aSrcTextKeys.Count; i++)
            {
                MSequence.MTextKey tk = ((MSequence.MTextKey)aSrcTextKeys[i]);
                m_aSrcTextKeys[i] = tk;

                if (tk.Text == strSrc)
                {
                    iSrcSelectedIndex = i;
                    //break;
                }
            }

            for (int i=0; i< aDesTextKeys.Count; i++)
            {
                MSequence.MTextKey tk = ((MSequence.MTextKey)aDesTextKeys[i]);
                m_aDesTextKeys[i] = tk;

                if (tk.Text == strDes)
                {
                    iDesSelectedIndex = i;
                    //break;
                }
            }

            cbSourceTags.SelectedIndex = iSrcSelectedIndex;
            cbDestTags.SelectedIndex = iDesSelectedIndex;

            if (lbBlendPairs.Items.Count > 0)
                lbBlendPairs.SelectedIndex = 0;
        }

        private void TransitionDelayedBlendEdit_Load(
            object sender, System.EventArgs e)
        {
            m_szDefaultSizeOfPanel = pnlBottom.Size;
            InitDelayedBlendControls();
        }

        private void cbSourceTags_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            if (DoesSelectedPairExist() == true)
                btnAddPair.Enabled = false;
            else
                btnAddPair.Enabled = true;           
        }

        private void cbDestTags_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            if (DoesSelectedPairExist() == true)
                btnAddPair.Enabled = false;
            else
                btnAddPair.Enabled = true;            
        }

        private void DeleteSelectedBlendPair()
        {
            if (lbBlendPairs.SelectedIndex < 0)
                return;

            int iPrevIndex = lbBlendPairs.SelectedIndex;

            MTransition.MBlendPair kPair = 
                ((MTransition)m_aTran[0]).BlendPairs[
                lbBlendPairs.SelectedIndex];
            string strSrc = kPair.StartKey;
            string strDest = kPair.TargetKey;

            foreach(MTransition tran in m_aTran)
            {
                if (tran.BlendPairs.Length == 1)
                {
                    string [] strVariables = new string[4];
                    strVariables[0] = tran.Source.Name;
                    strVariables[1] = tran.Destination.Name;
                    strVariables[2] = strSrc;
                    strVariables[3] = strDest;

                    DialogResult kYesNoCancelResult = 
                        MessageBoxManager.DoMessageBox(
                        "DeleteLastBlendPair.rtf", 
                        "Delete Last Blend Pair Warning",  
                        MessageBoxButtons.YesNoCancel, 
                        MessageBoxIcon.Warning, strVariables);

                    if (kYesNoCancelResult == DialogResult.Yes)
                    {
                        tran.StoredType = 
                            MTransition.TransitionType.Trans_ImmediateBlend;
                    }
                }
                else
                {
                    tran.RemoveBlendPair(strSrc, strDest);
                }
            }

            InitDelayedBlendControls();

            if (lbBlendPairs.Items.Count > 0)
            {
                if (iPrevIndex == 0)
                    lbBlendPairs.SelectedIndex = 0;
                else
                    lbBlendPairs.SelectedIndex = iPrevIndex - 1;
            }
        }

        private void lbBlendPairs_KeyUp(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                DeleteSelectedBlendPair();
            }
        }

        private void SetSelectedBlendPair(ComboBox box, string strKeyValue)
        {
            int i = 0;
            foreach (string Text in box.Items)
            {
                if (Text.Equals(strKeyValue))
                    break;
                i++;
            }
            if (i != box.SelectedIndex)
                box.SelectedIndex = i;
        }

        private void lbBlendPairs_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            MTransition.MBlendPair kPair = 
                m_aBlendPairs[lbBlendPairs.SelectedIndex];
            if (kPair != null)
            {
                SetSelectedBlendPair(cbSourceTags, kPair.StartKey);
                SetSelectedBlendPair(cbDestTags, kPair.TargetKey);
            }
        }

        private void btnRemovePair_Click(object sender, System.EventArgs e)
        {
            DeleteSelectedBlendPair();
        }

        private void btnAddPair_Click(object sender, System.EventArgs e)
        {
            string start = m_aSrcTextKeys[cbSourceTags.SelectedIndex].Text;
            string target = m_aDesTextKeys[cbDestTags.SelectedIndex].Text;

            foreach(MTransition tran in m_aTran)
            {
                // First, check if the the current combo box pair already
                // exists.
                // Check to see if we already have a blendpair for
                // this srckey... we only allow one to prevent confusion.
                if (tran.DoesBlendPairExistForSrcKey(start))
                    continue;

                tran.AddBlendPair(start, target);
            }

            InitDelayedBlendControls();
            int iIndex = GetBlendPairIndex(start, target);
            this.lbBlendPairs.SelectedIndex = iIndex;
        }

        private int GetBlendPairIndex(string strStart, string strTarget)
        {
            if (m_aBlendPairs == null)
                return -1;
    
            int i = 0;
            foreach(MTransition.MBlendPair bp in m_aBlendPairs)
            {
                if (bp == null)
                {
                    i++;
                    continue;
                }

                if (bp.StartKey == strStart &&
                    bp.TargetKey == strTarget)
                {
                    return i;
                }
                i++;
            }

            return -1;
        }

        private void lbBlendPairs_DoubleClick(
            object sender, System.EventArgs e)
        {
            // Removed the double click delete...
            // DeleteSelectedBlendPair();       
        }

        private void splitter1_SplitterMoved(
            object sender, System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoved(
                sender, this, pnlBottom, m_szDefaultSizeOfPanel.Height);
        }

        private void splitter1_SplitterMoving(
            object sender, System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoving(sender, this, pnlBottom);
        }

        private bool DoesSelectedPairExist()
        {           
            // Check to see if add button needs to be greyed or ungreyed...
            if (cbSourceTags.SelectedIndex < 0 ||
                cbDestTags.SelectedIndex < 0)
                return false;

            string start = m_aSrcTextKeys[cbSourceTags.SelectedIndex].Text;
            string target = m_aDesTextKeys[cbDestTags.SelectedIndex].Text;

            // Check to see if we already have a blendpair for
            // this srckey... we only allow one to prevent confusion.
            bool bFound = false;

            foreach(MTransition tran in m_aTran)
            {
                if (tran.Type != MTransition.TransitionType.Trans_DelayedBlend)
                    return false;

                if (tran.DoesBlendPairExistForSrcKey(start))
                    bFound = true;
                else // if a single one doesn't exist, allow add button
                    return false; 
                
            }

            return (bFound);
        }

        public void PostShow()
        {
            if (lbBlendPairs.Items.Count <= 0)
                return;

            int iCellOffset = 17;
            int iInitialOffset = 32;
            int iItems = lbBlendPairs.Items.Count;

            // cap the items
            if (iItems < 3)
                iItems = 3;

            // This breaks down when the number of items
            // multiplied the offset is greater than the
            // control height - initialoffset. When this
            // happens, panel1.Height will equal zero.
            pnlBottom.Height = Size.Height - 
                iInitialOffset - (iCellOffset * iItems); 

            RollBarControl.SplitterMoved(
                splitter1, this, pnlBottom, m_szDefaultSizeOfPanel.Height);   
            Invalidate(true);
        }
    }
}
