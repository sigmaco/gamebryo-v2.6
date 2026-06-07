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
    /// Summary description for InteractivePreview.
    /// </summary>
    public class InteractivePreview : 
        System.Windows.Forms.UserControl, IPostShow
    {
        #region Delegates

        private AppEvents.EventHandler_InteractiveSequence 
            m_InteractiveSequence = null;
        private System.Windows.Forms.Label lblCurrentSequence;
        private MAnimation.__Delegate_OnResetAnimations 
            m_OnResetAnimations = null;
        private MAnimation.__Delegate_OnTransitionAdded 
            m_OnTransitionAdded = null;
        MAnimation.__Delegate_OnTransitionRemoved m_OnTransitionRemoved = null;
        private AnimationToolUI.AppEvents.EventHandler_FrameworkDataChanged  
            m_OnFrameworkDataChanged = null;
        
        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_InteractiveSequence = 
                new AppEvents.EventHandler_InteractiveSequence(
                OnInteractiveSequence);
            m_OnResetAnimations = 
                new MAnimation.__Delegate_OnResetAnimations(
                    OnResetAnimations);
            m_OnFrameworkDataChanged =
                new AnimationToolUI.AppEvents.
                EventHandler_FrameworkDataChanged(
                AppEvents_FrameworkDataChanged);
            m_OnTransitionAdded = 
                new MAnimation.__Delegate_OnTransitionAdded(
                Animation_OnTransitionAdded);
            m_OnTransitionRemoved = 
                new MAnimation.__Delegate_OnTransitionRemoved(
                Animation_OnTransitionRemoved);
            
            // Register delegates.
            AppEvents.InteractiveSequence += m_InteractiveSequence;
            MFramework.Instance.Animation.OnResetAnimations += 
                m_OnResetAnimations;
            AppEvents.FrameworkDataChanged += m_OnFrameworkDataChanged;
            MFramework.Instance.Animation.OnTransitionAdded += 
                m_OnTransitionAdded;
            MFramework.Instance.Animation.OnTransitionRemoved += 
                m_OnTransitionRemoved;
        }

        private void UnregisterDelegates()
        {
            if (m_InteractiveSequence != null)
                AppEvents.InteractiveSequence -= m_InteractiveSequence;
            if (m_OnResetAnimations != null)
                MFramework.Instance.Animation.OnResetAnimations -= 
                    m_OnResetAnimations;
            if (m_OnFrameworkDataChanged != null)
                AppEvents.FrameworkDataChanged -= m_OnFrameworkDataChanged;
            if (m_OnTransitionAdded != null)
                MFramework.Instance.Animation.OnTransitionAdded -= 
                    m_OnTransitionAdded;
            if (m_OnTransitionRemoved != null)
                MFramework.Instance.Animation.OnTransitionRemoved -= 
                    m_OnTransitionRemoved;

        }

        #endregion

        private int m_iDefaultPanelHeight;
        private bool m_bHandlingCheckMessage = false;
        private System.Windows.Forms.CheckedListBox clbSequences;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label lblIdleSequence;
        private System.Windows.Forms.ComboBox cbIdleSequence;
        private System.Windows.Forms.ToolTip ttInteractivePreview;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public InteractivePreview()
        {
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            AppEvents.InteractivePreviewSelected +=
                new AppEvents.EventHandler_InteractivePreviewSelected(
                this.OnInteractivePreviewSelected);

            RegisterDelegates();
            m_iDefaultPanelHeight = panel1.Size.Height;

            InitializeCheckedListBox();

            bool bSuccess = MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.Interactive, null);
            Debug.Assert(bSuccess);
            
            MFramework.Instance.Clock.ScaleFactor = 1.0f;
            MFramework.Instance.Clock.Enabled = true;
            
            
            cbIdleSequence.SelectedIndex = FindIdleSequenceIndex();
            
            if (cbIdleSequence.SelectedIndex >= 0)
            {
                SelectAndCheckSequence(
                    ((MSequence) cbIdleSequence.SelectedItem).SequenceID);
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
            this.clbSequences = new System.Windows.Forms.CheckedListBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.panel2 = new System.Windows.Forms.Panel();
            this.lblCurrentSequence = new System.Windows.Forms.Label();
            this.cbIdleSequence = new System.Windows.Forms.ComboBox();
            this.lblIdleSequence = new System.Windows.Forms.Label();
            this.ttInteractivePreview = new System.Windows.Forms.ToolTip(this.components);
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // clbSequences
            // 
            this.clbSequences.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.clbSequences.CheckOnClick = true;
            this.clbSequences.IntegralHeight = false;
            this.clbSequences.Location = new System.Drawing.Point(0, 56);
            this.clbSequences.Name = "clbSequences";
            this.clbSequences.Size = new System.Drawing.Size(206, 174);
            this.clbSequences.Sorted = true;
            this.clbSequences.TabIndex = 2;
            this.ttInteractivePreview.SetToolTip(this.clbSequences, "Click on a sequence in this list to transition to that sequence. Sequences\nthat a" +
                "re grayed out are not reachable from the currently active sequence.");
            this.clbSequences.KeyDown += new System.Windows.Forms.KeyEventHandler(this.clbSequences_KeyDown);
            this.clbSequences.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.clbSequences_KeyPress);
            this.clbSequences.KeyUp += new System.Windows.Forms.KeyEventHandler(this.clbSequences_KeyUp);
            this.clbSequences.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.clbSequences_ItemCheck);
            // 
            // panel1
            // 
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(5, 243);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(206, 16);
            this.panel1.TabIndex = 2;
            // 
            // splitter1
            // 
            this.splitter1.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(5, 235);
            this.splitter1.MinExtra = 75;
            this.splitter1.MinSize = 15;
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(206, 8);
            this.splitter1.TabIndex = 1;
            this.splitter1.TabStop = false;
            this.splitter1.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoved);
            this.splitter1.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoving);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.lblCurrentSequence);
            this.panel2.Controls.Add(this.cbIdleSequence);
            this.panel2.Controls.Add(this.lblIdleSequence);
            this.panel2.Controls.Add(this.clbSequences);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(5, 5);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(206, 230);
            this.panel2.TabIndex = 0;
            // 
            // lblCurrentSequence
            // 
            this.lblCurrentSequence.Location = new System.Drawing.Point(0, 40);
            this.lblCurrentSequence.Name = "lblCurrentSequence";
            this.lblCurrentSequence.Size = new System.Drawing.Size(104, 16);
            this.lblCurrentSequence.TabIndex = 3;
            this.lblCurrentSequence.Text = "Current Sequence:";
            // 
            // cbIdleSequence
            // 
            this.cbIdleSequence.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbIdleSequence.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbIdleSequence.Location = new System.Drawing.Point(80, 8);
            this.cbIdleSequence.Name = "cbIdleSequence";
            this.cbIdleSequence.Size = new System.Drawing.Size(126, 21);
            this.cbIdleSequence.TabIndex = 1;
            this.ttInteractivePreview.SetToolTip(this.cbIdleSequence, "Select the default sequence to activate when no other sequences are active.");
            this.cbIdleSequence.SelectedValueChanged += new System.EventHandler(this.cbIdleSequence_SelectedValueChanged);
            // 
            // lblIdleSequence
            // 
            this.lblIdleSequence.AutoSize = true;
            this.lblIdleSequence.Location = new System.Drawing.Point(0, 13);
            this.lblIdleSequence.Name = "lblIdleSequence";
            this.lblIdleSequence.Size = new System.Drawing.Size(80, 16);
            this.lblIdleSequence.TabIndex = 0;
            this.lblIdleSequence.Text = "Idle Sequence:";
            // 
            // InteractivePreview
            // 
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.panel1);
            this.DockPadding.All = 5;
            this.Name = "InteractivePreview";
            this.Size = new System.Drawing.Size(216, 264);
            this.Load += new System.EventHandler(this.InteractivePreview_Load);
            this.panel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void InteractivePreview_Load(object sender,
            System.EventArgs e)
        {           
        }

        private void InitializeCheckedListBox()
        {
            clbSequences.Items.Clear();           

            if (MFramework.Instance.Animation.Sequences != null)
            {
                foreach (MSequence seq in MFramework.Instance.Animation
                    .Sequences)
                {
                    int iIndex = clbSequences.Items.Add(seq);
                    clbSequences.SetItemCheckState(iIndex,
                        CheckState.Unchecked);
                    if (!cbIdleSequence.Items.Contains(seq))
                    {
                        cbIdleSequence.Items.Add(seq);
                    }
                }
            }
        }

        private void OnInteractiveSequence(bool bIdle, uint uiSequenceID)
        {
            MSequence idleseq = (MSequence) cbIdleSequence.SelectedItem;
            if (uiSequenceID == idleseq.SequenceID)
            {
                return;
            }
            else if (bIdle)
            {
                uiSequenceID = idleseq.SequenceID;
            }

            SelectAndCheckSequence(uiSequenceID);
        }

        private void OnInteractivePreviewSelected()
        {
            
        }

        private void OnResetAnimations()
        {
            InitializeCheckedListBox();
        }

        private void SelectAndCheckSequence(uint uiSequenceID)
        {
            for (int i = 0; i < clbSequences.Items.Count; i++)
            {
                if (((MSequence) clbSequences.Items[i]).SequenceID ==
                    uiSequenceID)
                {
                    clbSequences.SetItemCheckState(i, CheckState.Checked);
                    clbSequences.SelectedIndex = i;
                    break;
                }
            }
        }

        private int FindIdleSequenceIndex()
        {
            
            Hashtable hash = ProjectData.GetGlobalInputHash();

            int iIndex = -1;
            for (int i = 0; i < cbIdleSequence.Items.Count; i++)
            {
                uint uiSequenceID = ((MSequence) cbIdleSequence.Items[i])
                    .SequenceID;

                if (ProjectData.Instance.IdleSequenceID != uint.MaxValue)
                {
                    if (uiSequenceID == ProjectData.Instance.IdleSequenceID)
                        return i;
                }
                else 
                {
                    if (hash.Contains(uiSequenceID))
                    {
                        ProjectData.SeqInputInfo info = 
                            (ProjectData.SeqInputInfo)
                            hash[uiSequenceID];
                        if (info.m_eKeyStroke == Keys.None)
                        {
                            iIndex = i;
                            break;
                        }
                    }
                }
            }

            return iIndex;
        }

        private void clbSequences_ItemCheck(object sender,
            System.Windows.Forms.ItemCheckEventArgs e)
        {
            if (m_bHandlingCheckMessage)
            {
                return;
            }

            if (e.CurrentValue == CheckState.Indeterminate)
            {
                e.NewValue = CheckState.Indeterminate;
                return;
            }

            MSequence curseq = MFramework.Instance.Animation.TargetAnimation;
            MSequence targetseq = (MSequence) clbSequences.Items[e.Index];

            if (curseq == targetseq)
            {
                e.NewValue = CheckState.Checked;
                return;
            }

            Debug.Assert(e.NewValue == CheckState.Checked, "e.NewValue == " +
                "CheckState.Checked");

            m_bHandlingCheckMessage = true;
            for (int i = 0; i < clbSequences.Items.Count; i++)
            {
                if (i != e.Index)
                {
                    if (targetseq != null && targetseq.GetTransition(
                        ((MSequence) clbSequences.Items[i]).SequenceID) ==
                        null)
                    {
                        clbSequences.SetItemCheckState(i,
                            CheckState.Indeterminate);
                    }
                    else
                    {
                        clbSequences.SetItemCheckState(i,
                            CheckState.Unchecked);
                    }
                }
            }

            MFramework.Instance.Animation.SetTargetAnimation(targetseq
                .SequenceID);
            m_bHandlingCheckMessage = false;
        }

        private void ResetPossibleSequences()
        {
            if (m_bHandlingCheckMessage)
            {
                return;
            }

            Debug.Assert(clbSequences.SelectedIndex != -1);

            MSequence curseq = 
                (MSequence) clbSequences.Items[clbSequences.SelectedIndex];
            
            m_bHandlingCheckMessage = true;
            for (int i = 0; i < clbSequences.Items.Count; i++)
            {
                MSequence kTempSeq = ((MSequence) clbSequences.Items[i]);
                if (curseq != null && 
                    kTempSeq != curseq && curseq.GetTransition(
                    kTempSeq.SequenceID) == null)
                {
                    clbSequences.SetItemCheckState(i,
                        CheckState.Indeterminate);
                }
                else if (kTempSeq != curseq)
                {
                    clbSequences.SetItemCheckState(i,
                        CheckState.Unchecked);
                }
            }

            m_bHandlingCheckMessage = false;
        }

        private void splitter1_SplitterMoved(object sender,
            System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoved(sender, this, panel1,
                m_iDefaultPanelHeight);
        }

        private void splitter1_SplitterMoving(object sender,
            System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoving(sender, this, panel1);
        }

        private void cbIdleSequence_SelectedValueChanged(object sender,
            System.EventArgs e)
        {
            uint uiSelSequencID = ((MSequence) cbIdleSequence.SelectedItem)
                .SequenceID;

            if (ProjectData.Instance.IdleSequenceID != uiSelSequencID)
                ProjectData.Instance.IdleSequenceID = uiSelSequencID;

            MFramework.Instance.Animation.ResetAnimations();

            SelectAndCheckSequence(uiSelSequencID);
            
            if (Parent != null)
            {
                bool bChangedFocus = Parent.Focus();
                Debug.Assert(bChangedFocus, "Focus should have changed",
                    "Focus should have changed");
            }
        }

        private void AppEvents_FrameworkDataChanged()
        {
            InitializeCheckedListBox();
            if (clbSequences.Items.Count != 0 && 
                clbSequences.SelectedIndex != -1)
                ResetPossibleSequences();
        }

        private void Animation_OnTransitionAdded(MTransition pkNewTransition)
        {
            if (clbSequences.Items.Count != 0 && 
                clbSequences.SelectedIndex != -1)
                ResetPossibleSequences();
        }

        private void Animation_OnTransitionRemoved(
            MTransition pkTransitionToBeRemoved)
        {
            if (clbSequences.Items.Count != 0 && 
                clbSequences.SelectedIndex != -1)
                ResetPossibleSequences();
        }

        private void clbSequences_KeyDown(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            e.Handled = true;
        }

        private void clbSequences_KeyPress(
            object sender, System.Windows.Forms.KeyPressEventArgs e)
        {
            e.Handled = true;
        }

        private void clbSequences_KeyUp(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            e.Handled = true;
        }

        public void PostShow()
        {
            if (clbSequences.Items.Count <= 0)
                return;

            int iCellOffset = 17;
            int iInitialOffset = 74;
            int iItems = clbSequences.Items.Count;

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
                splitter1, this, panel1, m_iDefaultPanelHeight);   
            Invalidate(true);
        }
    }
}
