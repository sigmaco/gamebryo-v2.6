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
using System.Diagnostics;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for TransitionChainEditForm.
    /// </summary>
    public class TransitionChainEditForm : System.Windows.Forms.Form
    {
        public class SeqTuple
        {
            public ArrayList m_aSequences;
            public ArrayList m_aOriginalSequences;
        };

        public ArrayList m_aSeqTuple;
        private ArrayList m_aPossibleSequences;
        private bool m_bTransBoth;
        private System.Windows.Forms.GroupBox gbPossibleSequences;
        private System.Windows.Forms.ListBox lbPossibleSequences;
        private System.Windows.Forms.GroupBox gbCurrentChain;
        private System.Windows.Forms.Label lblArrow2;
        private System.Windows.Forms.ImageList ilArrows;
        private System.Windows.Forms.Label lblArrow1;
        private System.Windows.Forms.TextBox tbSource;
        private System.Windows.Forms.GroupBox gbIntermediateSequences;
        private OOGroup.Windows.Forms.ImageButton btnPush;
        private OOGroup.Windows.Forms.ImageButton btnPop;
        private System.Windows.Forms.ListView lvIntermediateSequences;
        private System.Windows.Forms.TextBox tbDestination;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.RadioButton rbTransLastOnly;
        private System.Windows.Forms.RadioButton rbTransBoth;
        private System.Windows.Forms.ToolTip ttChainEdit;
        private System.ComponentModel.IContainer components;

        public SeqTuple GetSeqTuple(int iIndex)
        {
            Debug.Assert(iIndex >= 0 && iIndex < m_aSeqTuple.Count,
                "Index for SeqTuple out of range.");

            return (SeqTuple)m_aSeqTuple[iIndex];           
        }

        public ArrayList GetSeqArray(int iIndex)
        {
            SeqTuple st = GetSeqTuple(iIndex);
            return st.m_aSequences;
        }

        public ArrayList GetOrigSeqArray(int iIndex)
        {
            SeqTuple st = GetSeqTuple(iIndex);
            return st.m_aOriginalSequences;
        }

        public TransitionChainEditForm(ArrayList aaSequences)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            m_aSeqTuple = new ArrayList(aaSequences.Count);

            int i;
            for (i=0; i < aaSequences.Count; i++)
            {
                m_aSeqTuple.Add( new SeqTuple() );
                SeqTuple st = GetSeqTuple(i);
                st.m_aSequences = (ArrayList)aaSequences[i];
                st.m_aOriginalSequences = new ArrayList(GetSeqArray(i)); 
            }

            m_aPossibleSequences = new ArrayList();
            m_bTransBoth = false;
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(TransitionChainEditForm));
            this.gbPossibleSequences = new System.Windows.Forms.GroupBox();
            this.rbTransBoth = new System.Windows.Forms.RadioButton();
            this.rbTransLastOnly = new System.Windows.Forms.RadioButton();
            this.lbPossibleSequences = new System.Windows.Forms.ListBox();
            this.gbCurrentChain = new System.Windows.Forms.GroupBox();
            this.lvIntermediateSequences = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.lblArrow2 = new System.Windows.Forms.Label();
            this.ilArrows = new System.Windows.Forms.ImageList(this.components);
            this.lblArrow1 = new System.Windows.Forms.Label();
            this.tbSource = new System.Windows.Forms.TextBox();
            this.tbDestination = new System.Windows.Forms.TextBox();
            this.gbIntermediateSequences = new System.Windows.Forms.GroupBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnPush = new OOGroup.Windows.Forms.ImageButton();
            this.btnPop = new OOGroup.Windows.Forms.ImageButton();
            this.ttChainEdit = new System.Windows.Forms.ToolTip(this.components);
            this.gbPossibleSequences.SuspendLayout();
            this.gbCurrentChain.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbPossibleSequences
            // 
            this.gbPossibleSequences.Controls.Add(this.rbTransBoth);
            this.gbPossibleSequences.Controls.Add(this.rbTransLastOnly);
            this.gbPossibleSequences.Controls.Add(this.lbPossibleSequences);
            this.gbPossibleSequences.Location = new System.Drawing.Point(8, 16);
            this.gbPossibleSequences.Name = "gbPossibleSequences";
            this.gbPossibleSequences.Size = new System.Drawing.Size(216, 328);
            this.gbPossibleSequences.TabIndex = 0;
            this.gbPossibleSequences.TabStop = false;
            this.gbPossibleSequences.Text = "Possible Next Sequences:";
            // 
            // rbTransBoth
            // 
            this.rbTransBoth.Location = new System.Drawing.Point(16, 280);
            this.rbTransBoth.Name = "rbTransBoth";
            this.rbTransBoth.Size = new System.Drawing.Size(184, 40);
            this.rbTransBoth.TabIndex = 2;
            this.rbTransBoth.Text = "Show only transitions from last intermediate sequence to destination sequence";
            this.ttChainEdit.SetToolTip(this.rbTransBoth, "Select to show only sequences that are valid to transition\nto from the last inter" +
                "mediate sequence and from which it is\nvalid to transition to the destination seq" +
                "uence.");
            this.rbTransBoth.CheckedChanged += new System.EventHandler(this.rbTransBoth_CheckedChanged);
            // 
            // rbTransLastOnly
            // 
            this.rbTransLastOnly.Location = new System.Drawing.Point(16, 240);
            this.rbTransLastOnly.Name = "rbTransLastOnly";
            this.rbTransLastOnly.Size = new System.Drawing.Size(184, 32);
            this.rbTransLastOnly.TabIndex = 1;
            this.rbTransLastOnly.Text = "Show all transitions from last intermediate sequence";
            this.ttChainEdit.SetToolTip(this.rbTransLastOnly, "Select to show all sequences that are valid to transition\nto from the last interm" +
                "ediate sequence (or source sequence).");
            // 
            // lbPossibleSequences
            // 
            this.lbPossibleSequences.Location = new System.Drawing.Point(16, 24);
            this.lbPossibleSequences.Name = "lbPossibleSequences";
            this.lbPossibleSequences.Size = new System.Drawing.Size(184, 212);
            this.lbPossibleSequences.TabIndex = 0;
            this.ttChainEdit.SetToolTip(this.lbPossibleSequences, "Double-click a sequence to add it to the chain.");
            this.lbPossibleSequences.DoubleClick += new System.EventHandler(this.lbPossibleSequences_DoubleClick);
            // 
            // gbCurrentChain
            // 
            this.gbCurrentChain.Controls.Add(this.lvIntermediateSequences);
            this.gbCurrentChain.Controls.Add(this.lblArrow2);
            this.gbCurrentChain.Controls.Add(this.lblArrow1);
            this.gbCurrentChain.Controls.Add(this.tbSource);
            this.gbCurrentChain.Controls.Add(this.tbDestination);
            this.gbCurrentChain.Controls.Add(this.gbIntermediateSequences);
            this.gbCurrentChain.Location = new System.Drawing.Point(336, 16);
            this.gbCurrentChain.Name = "gbCurrentChain";
            this.gbCurrentChain.Size = new System.Drawing.Size(216, 328);
            this.gbCurrentChain.TabIndex = 3;
            this.gbCurrentChain.TabStop = false;
            this.gbCurrentChain.Text = "Current Chain:";
            // 
            // lvIntermediateSequences
            // 
            this.lvIntermediateSequences.AutoArrange = false;
            this.lvIntermediateSequences.BackColor = System.Drawing.SystemColors.Control;
            this.lvIntermediateSequences.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                                                                                                      this.columnHeader1});
            this.lvIntermediateSequences.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.lvIntermediateSequences.Location = new System.Drawing.Point(24, 104);
            this.lvIntermediateSequences.MultiSelect = false;
            this.lvIntermediateSequences.Name = "lvIntermediateSequences";
            this.lvIntermediateSequences.Size = new System.Drawing.Size(168, 136);
            this.lvIntermediateSequences.TabIndex = 3;
            this.lvIntermediateSequences.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Width = 145;
            // 
            // lblArrow2
            // 
            this.lblArrow2.ImageIndex = 0;
            this.lblArrow2.ImageList = this.ilArrows;
            this.lblArrow2.Location = new System.Drawing.Point(96, 248);
            this.lblArrow2.Name = "lblArrow2";
            this.lblArrow2.Size = new System.Drawing.Size(24, 24);
            this.lblArrow2.TabIndex = 4;
            // 
            // ilArrows
            // 
            this.ilArrows.ImageSize = new System.Drawing.Size(16, 16);
            this.ilArrows.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilArrows.ImageStream")));
            this.ilArrows.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // lblArrow1
            // 
            this.lblArrow1.ImageIndex = 0;
            this.lblArrow1.ImageList = this.ilArrows;
            this.lblArrow1.Location = new System.Drawing.Point(96, 48);
            this.lblArrow1.Name = "lblArrow1";
            this.lblArrow1.Size = new System.Drawing.Size(24, 24);
            this.lblArrow1.TabIndex = 1;
            // 
            // tbSource
            // 
            this.tbSource.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbSource.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbSource.Location = new System.Drawing.Point(16, 24);
            this.tbSource.Name = "tbSource";
            this.tbSource.ReadOnly = true;
            this.tbSource.Size = new System.Drawing.Size(184, 20);
            this.tbSource.TabIndex = 0;
            this.tbSource.TabStop = false;
            this.tbSource.Text = "Source";
            this.ttChainEdit.SetToolTip(this.tbSource, "The source sequence for the chain transition.");
            // 
            // tbDestination
            // 
            this.tbDestination.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbDestination.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbDestination.Location = new System.Drawing.Point(16, 280);
            this.tbDestination.Name = "tbDestination";
            this.tbDestination.ReadOnly = true;
            this.tbDestination.Size = new System.Drawing.Size(184, 20);
            this.tbDestination.TabIndex = 5;
            this.tbDestination.TabStop = false;
            this.tbDestination.Text = "Destination";
            this.ttChainEdit.SetToolTip(this.tbDestination, "The destination sequence for the chain transition.");
            // 
            // gbIntermediateSequences
            // 
            this.gbIntermediateSequences.Location = new System.Drawing.Point(16, 80);
            this.gbIntermediateSequences.Name = "gbIntermediateSequences";
            this.gbIntermediateSequences.Size = new System.Drawing.Size(184, 168);
            this.gbIntermediateSequences.TabIndex = 2;
            this.gbIntermediateSequences.TabStop = false;
            this.gbIntermediateSequences.Text = "Intermediate Sequences:";
            // 
            // btnOK
            // 
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(188, 360);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(80, 32);
            this.btnOK.TabIndex = 4;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(292, 360);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(80, 32);
            this.btnCancel.TabIndex = 5;
            this.btnCancel.Text = "Cancel";
            // 
            // btnPush
            // 
            this.btnPush.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnPush.ImageIndex = 1;
            this.btnPush.ImageList = this.ilArrows;
            this.btnPush.Location = new System.Drawing.Point(240, 128);
            this.btnPush.Name = "btnPush";
            this.btnPush.Size = new System.Drawing.Size(80, 32);
            this.btnPush.TabIndex = 1;
            this.ttChainEdit.SetToolTip(this.btnPush, "Click to add the currently selected sequence to the chain.");
            this.btnPush.Click += new System.EventHandler(this.btnPush_Click);
            // 
            // btnPop
            // 
            this.btnPop.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnPop.ImageIndex = 2;
            this.btnPop.ImageList = this.ilArrows;
            this.btnPop.Location = new System.Drawing.Point(240, 176);
            this.btnPop.Name = "btnPop";
            this.btnPop.Size = new System.Drawing.Size(80, 32);
            this.btnPop.TabIndex = 2;
            this.ttChainEdit.SetToolTip(this.btnPop, "Click to remove the last intermediate sequence from the chain.");
            this.btnPop.Click += new System.EventHandler(this.btnPop_Click);
            // 
            // TransitionChainEditForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(560, 400);
            this.ControlBox = false;
            this.Controls.Add(this.btnPop);
            this.Controls.Add(this.btnPush);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.gbPossibleSequences);
            this.Controls.Add(this.gbCurrentChain);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "TransitionChainEditForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Edit Chain Dialog";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.TransitionChainEditForm_Closing);
            this.Load += new System.EventHandler(this.TransitionChainEditForm_Load);
            this.gbPossibleSequences.ResumeLayout(false);
            this.gbCurrentChain.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void TransitionChainEditForm_Load(object sender,
            System.EventArgs e)
        {
            // Initialize button images.
            btnPush.SetImage((Bitmap) btnPush.ImageList.Images
                [btnPush.ImageIndex]);
            btnPop.SetImage((Bitmap) btnPop.ImageList.Images
                [btnPop.ImageIndex]);

            rbTransLastOnly.Checked = !m_bTransBoth;
            rbTransBoth.Checked = m_bTransBoth;

            if (m_aSeqTuple.Count == 1)
            {
                ArrayList aSeqs = GetSeqArray(0);

                MSequence seqSrc = (MSequence) aSeqs[0]; 
                MSequence seqDest = (MSequence) aSeqs[aSeqs.Count - 1];

                tbSource.Text = seqSrc.Name + "   (Source Sequence)";
                tbDestination.Text = seqDest.Name + 
                    "   (Destination Sequence)";
            }
            else
            {
                // We are handling multiply selected items
                ArrayList aSeqs = GetSeqArray(0);
                bool bMultipleSrc = false;
                bool bMultipleDes = false;
                uint uiSrcID = ((MSequence) aSeqs[0]).SequenceID;
                uint uiDesID = ((MSequence) aSeqs[aSeqs.Count - 1]).SequenceID;

                foreach (SeqTuple tp in m_aSeqTuple)
                {
                    if (uiSrcID != ((MSequence)tp.m_aSequences[0]).SequenceID)
                        bMultipleSrc = true;

                    if (uiDesID != ((MSequence)tp.m_aSequences[
                        tp.m_aSequences.Count - 1]).SequenceID)
                        bMultipleDes = true;

                }
                if (bMultipleSrc == true)
                    tbSource.Text = "<Multiple Sources>";
                else
                    tbSource.Text = ((MSequence) aSeqs[0]).Name;               

                if (bMultipleDes == true)
                    tbDestination.Text = "<Multiple Destinations>";
                else
                    tbDestination.Text = 
                        ((MSequence) aSeqs[aSeqs.Count - 1]).Name;
            }

            UpdateChain();
        }

        public ArrayList CollectNonexistentTransitions(ArrayList aSeqs)
        {
            // List should be of the following form:
            // SequenceFrom, SequenceTo, SequenceFrom, SequenceTo
            ArrayList list = new ArrayList();

            for (int i = 1; i < aSeqs.Count; i++)
            {
                MSequence seqFrom = (MSequence) aSeqs[i-1];
                MSequence seqTo = (MSequence) aSeqs[i];

                MTransition tran = seqFrom.GetTransition(seqTo.SequenceID);
                if (tran == null)
                {
                    list.Add(seqFrom);
                    list.Add(seqTo);
                }
                else if (tran.Type == MTransition.TransitionType.Trans_None)
                {
                    list.Add(seqFrom);
                    list.Add(seqTo);
                }
            }

            return list;
        }

        public void ConvertTransition(MTransition tran, ArrayList aSeqs)
        {
            if (DialogResult != DialogResult.OK)
            {
                return;
            }

            Debug.Assert(tran.Source == ((MSequence)aSeqs[0]));
            Debug.Assert(tran.Destination == 
                         ((MSequence)aSeqs[aSeqs.Count-1]));
            
            AnimationHelpers.ConvertChainTransition(tran, aSeqs);
        }

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            foreach(SeqTuple kSeqTuple in m_aSeqTuple)
            {
                ArrayList aOrigSeqs = kSeqTuple.m_aOriginalSequences;
                ArrayList aSeqs = kSeqTuple.m_aSequences;

                if (aOrigSeqs.Count == aSeqs.Count)
                {
                    bool bAllSame = true;
                    for (int i = 0; i < aSeqs.Count; i++)
                    {
                        MSequence kSeq = (MSequence) aSeqs[i];
                        MSequence kOrigSeq = (MSequence) aOrigSeqs[i];

                        if (kSeq.SequenceID != kOrigSeq.SequenceID)
                        {
                            bAllSame = false;
                            break;
                        }
                    }

                    if (bAllSame && aOrigSeqs.Count != 2)
                    {
                        DialogResult = DialogResult.Cancel;
                        return;
                    }
                }
            }

            DialogResult = DialogResult.OK;
        }

        private bool ValidateChain(ArrayList aSeqs)
        {
            for (int i = 0; i < aSeqs.Count - 1; i++)
            {
                MSequence seqFrom = (MSequence) aSeqs[i];
                MSequence seqTo = (MSequence) aSeqs[i+1];
                bool bAllowNull = false;
                if (i == aSeqs.Count - 2)
                {
                    if (seqFrom.SequenceID == ((MSequence)aSeqs[0]).SequenceID)
                    {
                        if (seqTo.SequenceID == 
                            ((MSequence)aSeqs[aSeqs.Count - 1]).SequenceID)
                        {
                            // In this case, our last transition is recursive.
                            // we can't allow this.
                            return false;
                        }
                    }
                    else if (seqFrom.SequenceID == 
                        ((MSequence)aSeqs[aSeqs.Count -1]).SequenceID)
                    {
                        // The last 
                        return false;
                    }

                    bAllowNull = true;
                }
                if (!ValidateLinkInChain(seqFrom, seqTo, bAllowNull))
                {
                    return false;
                }
            }

            return true;
        }

        private bool ValidateLinkInChain(MSequence seqFrom, MSequence seqTo,
            bool bAllowNull)
        {
            MTransition tran = seqFrom.GetTransition(seqTo.SequenceID);
            return IsValidTransition(tran, bAllowNull);
        }

        private bool IsValidTransition(MTransition tran, bool bAllowNull)
        {
            if (!bAllowNull && tran == null)
            {
                return false;
            }

            if (tran == null)
                return true;

            // Do not show invalid transitions
            if (tran.Type == MTransition.TransitionType.Trans_None)
            {
                return false;
            }

            // Do not show chain transitions
            if (tran.Type == MTransition.TransitionType.Trans_Chain)
            {
                return false;
            }

            return true;
        }

        private void UpdateChain()
        {
            lvIntermediateSequences.Items.Clear();

            if (m_aSeqTuple.Count == 1)
            {
                ArrayList aSeqs = GetSeqArray(0);

                for (int i = 0; i < aSeqs.Count; i++)
                {
                    if (i == 0 || i == aSeqs.Count - 1)
                    {
                        continue;
                    }
                    lvIntermediateSequences.Items.Add(
                        ((MSequence)aSeqs[i]).Name);
                }

                if (aSeqs.Count == 2)
                    btnPop.Enabled = false;
                else
                    btnPop.Enabled = true;

                MSequence seqCurrent = (MSequence) aSeqs[aSeqs.Count - 2];
                MSequence seqNext = (MSequence) aSeqs[aSeqs.Count - 1];

                ConstructPossibleSequences(
                    ref m_aPossibleSequences, seqCurrent, seqNext, aSeqs);
            }
            else
            {
                // Make an array of sequence arrays from tuple
                ArrayList aaSeq = new ArrayList(m_aSeqTuple.Count);
                foreach(SeqTuple st in m_aSeqTuple)
                {
                    aaSeq.Add(st.m_aSequences);
                }

                ArrayList aStr = 
                    AnimationHelpers.GetCommonChainSequences(aaSeq);

                foreach (string str in aStr)
                {
                    lvIntermediateSequences.Items.Add(str);
                }

                int iIndexFromEnd = 0;
                bool bCommon = AnimationHelpers.DoChainsHaveAnyCommonSequences(
                    aaSeq, ref iIndexFromEnd);

                btnPop.Enabled = bCommon;

                ArrayList aaPossibles = new ArrayList(m_aSeqTuple.Count);
                foreach(SeqTuple st in m_aSeqTuple)
                {
                    ArrayList aSeqs = st.m_aSequences;
                    MSequence seqCurrent = (MSequence) aSeqs[aSeqs.Count - 2];
                    MSequence seqNext = (MSequence) aSeqs[aSeqs.Count - 1];

                    ArrayList al = new ArrayList();
                    
                    ConstructPossibleSequences(
                        ref al, seqCurrent, seqNext, aSeqs);

                    aaPossibles.Add(al);
                }

                // aaPossibles is an array of array of sequence possibilities
                // Now we must collapse to a common set or possibilities.
                // In the following code, we loop over each sequence in our
                // base and ensure that that sequence exists in everyone elses
                // possibilities. If it doesn't, then it doesn't get added.
                // NOTE: This code could be made more efficient.

                m_aPossibleSequences.Clear();
                ArrayList aBaseSeqs = (ArrayList)aaPossibles[0];
                foreach(MSequence seqBase in aBaseSeqs)
                {
                    bool bFound = false;

                    // Must validate in all others.
                    for( int x=1; x<aaPossibles.Count; x++)
                    {
                        bFound = false;

                        foreach(MSequence seq in (ArrayList)aaPossibles[x])
                        {
                            if (seq.SequenceID == seqBase.SequenceID)
                            {
                                bFound = true;
                                break;
                            }
                        }

                        if (bFound == false)
                            break;
                    }

                    if (bFound == true)
                    {
                        // We went through every possibility list and
                        // found every time.
                        m_aPossibleSequences.Add(seqBase);
                    }
                }
            }

            UpdatePossibleSequences();
        }

        private void ConstructPossibleSequences(
            ref ArrayList aPossibleSequences,
            MSequence seqCurrent,
            MSequence seqNext,
            ArrayList aSeqs)
        {
            MSequence seqSrc = (MSequence) aSeqs[0]; 
            MSequence seqDest = (MSequence) aSeqs[aSeqs.Count - 1];

            aPossibleSequences.Clear();
            
            foreach (MSequence seqTransDestination in
                MFramework.Instance.Animation.Sequences)
            {
                if (seqCurrent == seqTransDestination)
                {
                    continue;
                }

                if (seqCurrent == seqSrc && seqTransDestination == seqDest)
                {
                    continue;
                }

                MTransition tran = seqCurrent.GetTransition(
                    seqTransDestination.SequenceID);
                if (!IsValidTransition(tran, false))
                {
                    continue;
                }

                // Do not show terminal (dead-end) transitions, unless it
                // matches the final destination.
                if (seqTransDestination != seqNext &&
                    (seqTransDestination.Transitions == null ||
                    seqTransDestination.Transitions.Length == 0))
                {
                    continue;
                }
                
                if (m_bTransBoth &&
                    !ValidateLinkInChain(seqTransDestination, seqNext, false))
                {
                    continue;
                }            

                aPossibleSequences.Add(seqTransDestination);
            }
        }

        private void UpdatePossibleSequences()
        {
            lbPossibleSequences.Items.Clear();

            foreach(MSequence seq in m_aPossibleSequences)
            {
                lbPossibleSequences.Items.Add(seq);
            }

            if (lbPossibleSequences.Items.Count > 0)
            {
                lbPossibleSequences.SelectedIndex = 0;
                btnPush.Enabled = true;
            }
            else
            {
                btnPush.Enabled = false;
            }
        }

        private void btnPush_Click(object sender, System.EventArgs e)
        {
            foreach(SeqTuple st in m_aSeqTuple)
            {
                ArrayList aSeqs = st.m_aSequences;

                if (m_aPossibleSequences.Count > 0)
                {
                    aSeqs.Insert(aSeqs.Count - 1, 
                        m_aPossibleSequences[
                            lbPossibleSequences.SelectedIndex]);
                }
            }

            UpdateChain();
        }

        private void btnPop_Click(object sender, System.EventArgs e)
        {
            foreach(SeqTuple st in m_aSeqTuple)
            {
                ArrayList aSeqs = st.m_aSequences;

                if (aSeqs.Count > 2)
                {
                    aSeqs.RemoveAt(aSeqs.Count - 2);
                    UpdateChain();
                }
            }

            // Because multiple selected chains must be in sync,
            // we can simply check the first to determine if we
            // need to disable the pop button.
            if (((SeqTuple)m_aSeqTuple[0]).m_aSequences.Count == 2)
            {
                btnPop.Enabled = false;
            }
            else
            {
                btnPop.Enabled = true;
            }
        }

        private void TransitionChainEditForm_Closing(object sender,
            System.ComponentModel.CancelEventArgs e)
        {
            foreach (SeqTuple st in m_aSeqTuple)
            {
                ArrayList aSeqs = st.m_aSequences;

                if (DialogResult == DialogResult.OK)
                {
                    int iCount = aSeqs.Count;

                    if (iCount <= 2)
                    {
                        MessageBoxManager.DoMessageBox(
                            "TooFewIntermediates.rtf",
                            "Too few intermediate sequences.", 
                            MessageBoxButtons.OK, MessageBoxIcon.Error, null);
                        e.Cancel = true;
                        DialogResult = DialogResult.Cancel;
                    }
                    else if (!ValidateChain(aSeqs))
                    {
                        string[] astrVariables =
                            {
                                ((MSequence) aSeqs[iCount - 2]).Name,
                                ((MSequence) aSeqs[iCount - 1]).Name
                            };

                        if (astrVariables[0] == astrVariables[1])
                        {
                            // Failed because the last sequences were identical
                            MessageBoxManager.DoMessageBox(
                                "InvalidTransitionError.rtf",
                                "Invalid Transition Found", 
                                MessageBoxButtons.OK, MessageBoxIcon.Error,
                                astrVariables);
                        }
                        else
                        {
                            // Failed because this was a nested chain.
                            MessageBoxManager.DoMessageBox(
                                "NestedChainError.rtf",
                                "Nested Chain Found", MessageBoxButtons.OK,
                                MessageBoxIcon.Error, astrVariables);
                        }
                        e.Cancel = true;
                        DialogResult = DialogResult.Cancel;
                    }
                    else
                    {
                        ArrayList aNonExistent = 
                            CollectNonexistentTransitions(aSeqs);
                        if (aNonExistent.Count > 0)
                        {
                            string[] strVariables = new string[1];
                            for (int i = 1; i < aNonExistent.Count; i += 2)
                            {
                                MSequence seqFrom = (MSequence)
                                    aNonExistent[i - 1];
                                MSequence seqTo = (MSequence) aNonExistent[i];

                                strVariables[0] += "\"" + seqFrom.Name +
                                    "\" to \"" + seqTo.Name + "\"\\par\n";
                            }

                            DialogResult drOKCancel = MessageBoxManager
                                .DoMessageBox(
                                "ChainCreateDefaultTransitions.rtf", 
                                "Non-Existent Transitions Found Warning",
                                MessageBoxButtons.OKCancel,
                                MessageBoxIcon.Warning, strVariables);

                            if (drOKCancel == DialogResult.OK)
                            {
                                for (int i = 1; i < aNonExistent.Count; i += 2)
                                {
                                    MSequence seqFrom = (MSequence)
                                        aNonExistent[i - 1];
                                    MSequence seqTo = 
                                        (MSequence) aNonExistent[i];

                                    if (seqFrom.CanSyncTo(seqTo))
                                    {
                                        MFramework.Instance.Animation
                                            .AddTransition(seqFrom.SequenceID,
                                            seqTo.SequenceID, MTransition
                                            .TransitionType.Trans_DefaultSync,
                                            0.0f);
                                    }
                                    else
                                    {
                                        MFramework.Instance.Animation
                                            .AddTransition(seqFrom.SequenceID,
                                            seqTo.SequenceID, MTransition
                                            .TransitionType
                                            .Trans_DefaultNonSync,
                                            0.0f);
                                    }
                                }
                            }
                            else
                            {
                                e.Cancel = true;
                                DialogResult = DialogResult.Cancel;
                            }
                        }
                    }
                }
            }
        }
        
        private void lbPossibleSequences_DoubleClick(object sender,
            System.EventArgs e)
        {
            btnPush_Click(this, null);
        }

        private void rbTransBoth_CheckedChanged(object sender,
            System.EventArgs e)
        {        
            m_bTransBoth = rbTransBoth.Checked;
            UpdateChain();
        }
    }
}
