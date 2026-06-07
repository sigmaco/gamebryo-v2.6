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
    /// Summary description for TransitionChainEdit.
    /// </summary>
    public class TransitionChainEdit : 
        System.Windows.Forms.UserControl, IPostShow
    {
        private Size m_szDefaultSizeOfPanel;
        private System.Windows.Forms.TextBox tbChain;
        private ArrayList m_aTran;
        private System.Windows.Forms.ListBox lbChainSequence;
        private System.Windows.Forms.ImageList ilPlusMinus;
        private System.Windows.Forms.ToolTip ttChainEdit;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;
        private System.Windows.Forms.Panel pnlTop;
        private System.Windows.Forms.Panel pnlBottom;
        private System.Windows.Forms.GroupBox gbDuration;
        private System.Windows.Forms.RadioButton rbEndOfSequence;
        private System.Windows.Forms.RadioButton rbInSeconds;
        private AnimationToolUI.NumericSlider nsDuration;
        private System.Windows.Forms.Button btnEditChainSequence;
        private System.Windows.Forms.TextBox tbSelectedSequence;
        private System.Windows.Forms.Splitter splitter1;
        private bool InitialDragPlayState;

        public TransitionChainEdit(MTransition Tran)
        {
            ArrayList aTran = new ArrayList(1);
            aTran.Add(Tran);
            TransitionChainEditConstructorInit(aTran);
        }

        public TransitionChainEdit(ArrayList aTran)
        {
            TransitionChainEditConstructorInit(aTran);
        }

        public void TransitionChainEditConstructorInit(ArrayList aTran)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            // m_aTran is an array of MTransitions,ie the multiple
            // selection of chains.
            m_aTran = aTran;

            // For debug purposes, assert that each transition is actually
            // a chain.
            foreach (MTransition tran in m_aTran)
            {          
                Debug.Assert(tran.Type == 
                    MTransition.TransitionType.Trans_Chain);
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(TransitionChainEdit));
            this.lbChainSequence = new System.Windows.Forms.ListBox();
            this.tbChain = new System.Windows.Forms.TextBox();
            this.ilPlusMinus = new System.Windows.Forms.ImageList(this.components);
            this.ttChainEdit = new System.Windows.Forms.ToolTip(this.components);
            this.rbEndOfSequence = new System.Windows.Forms.RadioButton();
            this.rbInSeconds = new System.Windows.Forms.RadioButton();
            this.btnEditChainSequence = new System.Windows.Forms.Button();
            this.pnlTop = new System.Windows.Forms.Panel();
            this.pnlBottom = new System.Windows.Forms.Panel();
            this.gbDuration = new System.Windows.Forms.GroupBox();
            this.nsDuration = new AnimationToolUI.NumericSlider();
            this.tbSelectedSequence = new System.Windows.Forms.TextBox();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.pnlTop.SuspendLayout();
            this.pnlBottom.SuspendLayout();
            this.gbDuration.SuspendLayout();
            this.SuspendLayout();
            // 
            // lbChainSequence
            // 
            this.lbChainSequence.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lbChainSequence.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbChainSequence.CausesValidation = false;
            this.lbChainSequence.Location = new System.Drawing.Point(0, 21);
            this.lbChainSequence.Name = "lbChainSequence";
            this.lbChainSequence.Size = new System.Drawing.Size(246, 80);
            this.lbChainSequence.TabIndex = 1;
            this.ttChainEdit.SetToolTip(this.lbChainSequence, "Select a sequence in the chain to see its properties below.");
            this.lbChainSequence.SelectedIndexChanged += new System.EventHandler(this.lbChainSequence_SelectedIndexChanged);
            // 
            // tbChain
            // 
            this.tbChain.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbChain.BackColor = System.Drawing.SystemColors.Info;
            this.tbChain.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbChain.Location = new System.Drawing.Point(0, 0);
            this.tbChain.Name = "tbChain";
            this.tbChain.ReadOnly = true;
            this.tbChain.Size = new System.Drawing.Size(246, 20);
            this.tbChain.TabIndex = 0;
            this.tbChain.TabStop = false;
            this.tbChain.Text = "Chain Sequence";
            this.tbChain.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // ilPlusMinus
            // 
            this.ilPlusMinus.ImageSize = new System.Drawing.Size(16, 16);
            this.ilPlusMinus.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilPlusMinus.ImageStream")));
            this.ilPlusMinus.TransparentColor = System.Drawing.Color.White;
            // 
            // rbEndOfSequence
            // 
            this.rbEndOfSequence.Location = new System.Drawing.Point(16, 72);
            this.rbEndOfSequence.Name = "rbEndOfSequence";
            this.rbEndOfSequence.Size = new System.Drawing.Size(112, 24);
            this.rbEndOfSequence.TabIndex = 2;
            this.rbEndOfSequence.Text = "End of Sequence";
            this.ttChainEdit.SetToolTip(this.rbEndOfSequence, "Select to wait until the end of this sequence before\ntransitioning to the next se" +
                "quence in the chain.");
            this.rbEndOfSequence.CheckedChanged += new System.EventHandler(this.rbEndOfSequence_CheckedChanged);
            // 
            // rbInSeconds
            // 
            this.rbInSeconds.Location = new System.Drawing.Point(16, 32);
            this.rbInSeconds.Name = "rbInSeconds";
            this.rbInSeconds.Size = new System.Drawing.Size(80, 24);
            this.rbInSeconds.TabIndex = 0;
            this.rbInSeconds.Text = "In Seconds";
            this.ttChainEdit.SetToolTip(this.rbInSeconds, "Select to wait a specified duration in seconds before\ntransitioning to the next s" +
                "equence in the chain.");
            this.rbInSeconds.CheckedChanged += new System.EventHandler(this.rbInSeconds_CheckedChanged);
            // 
            // btnEditChainSequence
            // 
            this.btnEditChainSequence.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnEditChainSequence.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnEditChainSequence.Location = new System.Drawing.Point(72, 15);
            this.btnEditChainSequence.Name = "btnEditChainSequence";
            this.btnEditChainSequence.Size = new System.Drawing.Size(101, 24);
            this.btnEditChainSequence.TabIndex = 5;
            this.btnEditChainSequence.Text = "Edit Chain";
            this.ttChainEdit.SetToolTip(this.btnEditChainSequence, "Click to edit this chain.");
            this.btnEditChainSequence.Click += new System.EventHandler(this.btnEditChainSequence_Click);
            // 
            // pnlTop
            // 
            this.pnlTop.Controls.Add(this.tbChain);
            this.pnlTop.Controls.Add(this.lbChainSequence);
            this.pnlTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlTop.Location = new System.Drawing.Point(5, 5);
            this.pnlTop.Name = "pnlTop";
            this.pnlTop.Size = new System.Drawing.Size(246, 94);
            this.pnlTop.TabIndex = 1;
            // 
            // pnlBottom
            // 
            this.pnlBottom.Controls.Add(this.gbDuration);
            this.pnlBottom.Controls.Add(this.btnEditChainSequence);
            this.pnlBottom.Controls.Add(this.tbSelectedSequence);
            this.pnlBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlBottom.Location = new System.Drawing.Point(5, 99);
            this.pnlBottom.Name = "pnlBottom";
            this.pnlBottom.Size = new System.Drawing.Size(246, 184);
            this.pnlBottom.TabIndex = 2;
            // 
            // gbDuration
            // 
            this.gbDuration.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.gbDuration.Controls.Add(this.rbEndOfSequence);
            this.gbDuration.Controls.Add(this.rbInSeconds);
            this.gbDuration.Controls.Add(this.nsDuration);
            this.gbDuration.Location = new System.Drawing.Point(8, 75);
            this.gbDuration.Name = "gbDuration";
            this.gbDuration.Size = new System.Drawing.Size(230, 104);
            this.gbDuration.TabIndex = 7;
            this.gbDuration.TabStop = false;
            this.gbDuration.Text = "Time until next sequence in chain is triggered";
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
            this.nsDuration.Location = new System.Drawing.Point(112, 32);
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
            this.nsDuration.Size = new System.Drawing.Size(112, 28);
            this.nsDuration.TabIndex = 1;
            this.nsDuration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nsDuration.Value = new System.Decimal(new int[] {
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0});
            this.nsDuration.Load += new System.EventHandler(this.nsDuration_Load);
            this.nsDuration.BeginValueDrag += new AnimationToolUI.ValueChangedEventHandler(this.nsDuration_BeginValueDrag);
            this.nsDuration.EndValueDrag += new AnimationToolUI.ValueChangedEventHandler(this.nsDuration_EndValueDrag);
            this.nsDuration.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsDuration_ValueChanged);
            // 
            // tbSelectedSequence
            // 
            this.tbSelectedSequence.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbSelectedSequence.BackColor = System.Drawing.SystemColors.Info;
            this.tbSelectedSequence.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbSelectedSequence.Location = new System.Drawing.Point(0, 51);
            this.tbSelectedSequence.Name = "tbSelectedSequence";
            this.tbSelectedSequence.ReadOnly = true;
            this.tbSelectedSequence.Size = new System.Drawing.Size(246, 20);
            this.tbSelectedSequence.TabIndex = 6;
            this.tbSelectedSequence.TabStop = false;
            this.tbSelectedSequence.Text = "Selected Sequence";
            this.tbSelectedSequence.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // splitter1
            // 
            this.splitter1.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(5, 91);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(246, 8);
            this.splitter1.TabIndex = 3;
            this.splitter1.TabStop = false;
            this.splitter1.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoved);
            this.splitter1.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoving);
            // 
            // TransitionChainEdit
            // 
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.pnlTop);
            this.Controls.Add(this.pnlBottom);
            this.DockPadding.All = 5;
            this.Name = "TransitionChainEdit";
            this.Size = new System.Drawing.Size(256, 288);
            this.Load += new System.EventHandler(this.TransitionChainEdit_Load);
            this.pnlTop.ResumeLayout(false);
            this.pnlBottom.ResumeLayout(false);
            this.gbDuration.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void TransitionChainEdit_Load(
            object sender, System.EventArgs e)
        {
            m_szDefaultSizeOfPanel = pnlBottom.Size;
            TransitionChainEditLoadInit();
        }

        private void TransitionChainEditLoadInit()
        {
            lbChainSequence.Items.Clear();

            if (m_aTran.Count == 1)
            {
                MTransition tran = (MTransition)m_aTran[0];

                lbChainSequence.Items.Add(tran.Source.Name);

                foreach (MTransition.MChainInfo chain in tran.ChainInfo)
                {
                    MSequence kSequence = MFramework.Instance.Animation
                        .GetSequence(chain.SequenceID);
                    Debug.Assert(kSequence != null);

                    string strSeqName = kSequence.Name;
                    float fDur = chain.Duration;

                    lbChainSequence.Items.Add(AnimationHelpers.
                        GetListBoxEntryString(strSeqName, fDur));
                    }

                lbChainSequence.Items.Add(tran.Destination.Name);
            }
            else
            {
                bool bMultipleSrc = false;
                bool bMultipleDes = false;
                string strSrc;
                string strDes;
                uint uiSrcID = ((MTransition)m_aTran[0]).SrcID;
                uint uiDesID = ((MTransition)m_aTran[0]).DesID;

                foreach (MTransition t in m_aTran)
                {
                    if (uiSrcID != t.SrcID)
                        bMultipleSrc = true;

                    if (uiDesID != t.DesID)
                        bMultipleDes = true;
                }

                if (bMultipleSrc == true)
                    strSrc = "<Multiple Sources>";
                else
                    strSrc = ((MTransition)m_aTran[0]).Source.Name;

                if (bMultipleDes == true)
                    strDes = "<Multiple Destinations>";
                else
                    strDes = ((MTransition)m_aTran[0]).Destination.Name;


                // Get common sequences from chain list.               
                ArrayList astrCommonWithDurations = 
                    AnimationHelpers.GetCommonChainSequencesWithDurations(
                    m_aTran);

                lbChainSequence.Items.Add(strSrc);

                foreach (string str in astrCommonWithDurations)
                {
                    lbChainSequence.Items.Add(str);
                }

                lbChainSequence.Items.Add(strDes);
            }

            lbChainSequence.SelectedIndex = 0;
        }

        private void lbChainSequence_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            MTransition tran = (MTransition)m_aTran[0];

            if (lbChainSequence.SelectedIndex <= 0 ||
                lbChainSequence.SelectedIndex >= 
                lbChainSequence.Items.Count -1) 
            {
                gbDuration.Enabled = false;
                return;
            }

            if (m_aTran.Count == 1)
            {
                gbDuration.Enabled = true;

                float fDuration = tran.ChainInfo[lbChainSequence
                    .SelectedIndex - 1].Duration;
                if (fDuration == MTransition.MChainInfo.MAX_DURATION)
                {
                    rbInSeconds.Checked = false;
                    rbEndOfSequence.Checked = true;
                }
                else
                {
                    nsDuration.Value = Convert.ToDecimal(fDuration);
                    rbInSeconds.Checked = true;
                    rbEndOfSequence.Checked = false;
                }
            }
            else
            {
                string strText = lbChainSequence.Text;
                bool bDiffer = strText.EndsWith("(DURATIONS DIFFER)");

                if (strText.EndsWith("<INTERMEDIATES DIFFER>"))
                    bDiffer = true;

                if (bDiffer)
                    gbDuration.Enabled = false;
                else
                {
                    gbDuration.Enabled = true;

                    int iIndex = lbChainSequence.SelectedIndex;
                    int iIndexFromEnd = 
                        lbChainSequence.Items.Count - iIndex - 1;
                    int iChainIndex = tran.ChainInfo.Length - iIndexFromEnd;
                    float fDuration = tran.ChainInfo[iChainIndex].Duration;

                    if (fDuration == MTransition.MChainInfo.MAX_DURATION)
                    {
                        rbInSeconds.Checked = false;
                        nsDuration.Enabled = false;
                        rbEndOfSequence.Checked = true;
                    }
                    else
                    {
                        nsDuration.Value = Convert.ToDecimal(fDuration);
                        rbInSeconds.Checked = true;
                        rbEndOfSequence.Checked = false;
                    }
                }
            }
        }

        private void btnEditChainSequence_Click(object sender,
            System.EventArgs e)
        {
            ArrayList aaSeqs = new ArrayList(m_aTran.Count);

            foreach (MTransition tran in m_aTran)
            {
                Debug.Assert(tran.ChainInfo.Length >= 1, 
                    "No Intermediate sequence in Chain.");

                ArrayList aSeqs = new ArrayList(tran.ChainInfo.Length + 2);
                aSeqs.Add(tran.Source);

                foreach (MTransition.MChainInfo chain in tran.ChainInfo)
                {
                    MSequence kSequence = MFramework.Instance.Animation
                        .GetSequence(chain.SequenceID);
                    aSeqs.Add(kSequence);
                }

                aSeqs.Add(tran.Destination);

                aaSeqs.Add(aSeqs);
            }
            
            TransitionChainEditForm kForm = 
                new TransitionChainEditForm(aaSeqs);

            DialogResult kRes = kForm.ShowDialog();
            if (kRes == DialogResult.OK)
            {
                int i=0;
                foreach (MTransition tran in m_aTran)
                {
                    kForm.ConvertTransition(tran, kForm.GetSeqArray(i));
                    i++;
                }

                TransitionChainEditLoadInit();
                Invalidate(true);
            }        
        }

        private void rbInSeconds_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (rbInSeconds.Checked)
            {
                nsDuration.Enabled = true;
                nsDuration_ValueChanged(nsDuration, nsDuration.Value);
            }
            else
            {
                nsDuration.Enabled = false;
            }
        }

        private void nsDuration_ValueChanged(object sender,
            decimal Value)
        {
            if (nsDuration.Enabled == false)
                return;

            MTransition kBaseTran = (MTransition)m_aTran[0];
            int iBaseSelIndex = lbChainSequence.SelectedIndex;

            if (iBaseSelIndex == 0 || 
                iBaseSelIndex >= lbChainSequence.Items.Count -1) 
                return;

            int iIndex; 
            int iIndexFromEnd;
            int iChainIndex; 
            
            foreach( MTransition kTran in m_aTran)
            {
                iIndex = iBaseSelIndex;
                iIndexFromEnd = lbChainSequence.Items.Count - iIndex - 1;
                iChainIndex = kTran.ChainInfo.Length - iIndexFromEnd;

                MTransition.MChainInfo chain = kTran.ChainInfo[iChainIndex];

                if (nsDuration.Value == Convert.ToDecimal(chain.Duration))
                    continue;

                chain.Duration = (float) nsDuration.Value;
                MSequence kSequence = MFramework.Instance.Animation
                    .GetSequence(chain.SequenceID);
                Debug.Assert(kSequence != null);

                string strSeqName = kSequence.Name;
                float fDur = chain.Duration;
            }

            iIndex = iBaseSelIndex;
            iIndexFromEnd = lbChainSequence.Items.Count - iIndex - 1;
            iChainIndex = kBaseTran.ChainInfo.Length - iIndexFromEnd;

            MTransition.MChainInfo kBaseChain = 
                kBaseTran.ChainInfo[iChainIndex];
            MSequence kBaseSequence = MFramework.Instance.Animation
                .GetSequence(kBaseChain.SequenceID);
            Debug.Assert(kBaseSequence != null);
            string strBaseSeqName = kBaseSequence.Name;

            lbChainSequence.Items[iBaseSelIndex] = AnimationHelpers.
                GetListBoxEntryString(strBaseSeqName, kBaseChain.Duration);
            lbChainSequence.SelectedIndex = iBaseSelIndex;
        }

        private void rbEndOfSequence_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (rbEndOfSequence.Checked == false)
                return;

            MTransition kBaseTran = (MTransition)m_aTran[0];
            int iBaseSelIndex = lbChainSequence.SelectedIndex;

            if (iBaseSelIndex == 0 || 
                iBaseSelIndex >= lbChainSequence.Items.Count -1)
                return;

            int iIndex;
            int iIndexFromEnd;
            int iChainIndex;            

            foreach( MTransition kTran in m_aTran)
            {
                iIndex = iBaseSelIndex;
                iIndexFromEnd = lbChainSequence.Items.Count - iIndex - 1;
                iChainIndex = kTran.ChainInfo.Length - iIndexFromEnd;

                MTransition.MChainInfo chain = kTran.ChainInfo[iChainIndex];

                if (chain.Duration == MTransition.MChainInfo.MAX_DURATION)
                    continue;

                chain.Duration = MTransition.MChainInfo.MAX_DURATION;
                MSequence kSequence = MFramework.Instance.Animation
                    .GetSequence(chain.SequenceID);
                Debug.Assert(kSequence != null);

                string strSeqName = kSequence.Name;
                float fDur = chain.Duration;
            }

            iIndex = iBaseSelIndex;
            iIndexFromEnd = lbChainSequence.Items.Count - iIndex - 1;
            iChainIndex = kBaseTran.ChainInfo.Length - iIndexFromEnd;

            MTransition.MChainInfo kBaseChain = 
                kBaseTran.ChainInfo[iChainIndex];
            MSequence kBaseSequence = MFramework.Instance.Animation
                .GetSequence(kBaseChain.SequenceID);
            Debug.Assert(kBaseSequence != null);
            string strBaseSeqName = kBaseSequence.Name;

            lbChainSequence.Items[iBaseSelIndex] = AnimationHelpers.
                GetListBoxEntryString(strBaseSeqName, kBaseChain.Duration);
            lbChainSequence.SelectedIndex = iBaseSelIndex;
        }

        private void nsDuration_BeginValueDrag(object sender, decimal Value)
        {
            this.InitialDragPlayState = MFramework.Instance.Clock.Enabled;
            MFramework.Instance.Clock.Enabled = false;
            MFramework.Instance.Clock.RunUpEnabled = false;
        }

        private void nsDuration_EndValueDrag(object sender, decimal Value)
        {
            MFramework.Instance.Clock.Enabled = InitialDragPlayState;
            MFramework.Instance.Clock.RunUpEnabled = true;
            MFramework.Instance.Clock.RunUpTime(
                MFramework.Instance.Animation.CurrentTime);
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

        private void nsDuration_Load(object sender, System.EventArgs e)
        {       
        }

        public void PostShow()
        {
            if (m_aTran.Count <= 0)
                return;

            int iCellOffset = lbChainSequence.ItemHeight;
            int iInitialOffset = 40; //lbChainSequence.Location.Y;
            int iItems = 0;

            if (m_aTran.Count == 1)
            {
                iItems = ((MTransition)m_aTran[0]).ChainInfo.Length + 2;
            }
            else
            {
                ArrayList astrCommon = 
                    AnimationHelpers.GetCommonChainSequences(
                    AnimationHelpers.Convert_aTran_To_aaSeq(m_aTran));

                iItems = astrCommon.Count + 2;
            }

            // Auto-adjust the height of the window

            // cap the items
            if (iItems < 3)
                iItems = 3;
            //else if (iItems > 11)
            //    iItems = 11;

            pnlBottom.Height = Size.Height - 
                iInitialOffset - (iCellOffset * iItems); 

            RollBarControl.SplitterMoved(
                splitter1, this, pnlBottom, m_szDefaultSizeOfPanel.Height);   
            Invalidate(true);
        }
    }
}
