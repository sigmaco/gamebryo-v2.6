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
    /// Summary description for SequenceGroupCommon.
    /// </summary>
    public class SequenceGroupCommon : System.Windows.Forms.UserControl
    {
        #region Delegates

        private MAnimation.__Delegate_OnSequenceGroupModified 
            m_OnSequenceGroupModified = null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_OnSequenceGroupModified = 
                new MAnimation.__Delegate_OnSequenceGroupModified(
                OnSequenceGroupModified);

            // Register delegates.
            MFramework.Instance.Animation.OnSequenceGroupModified += 
                m_OnSequenceGroupModified;
        }

        private void UnregisterDelegates()
        {
            if(m_OnSequenceGroupModified != null)
                MFramework.Instance.Animation.OnSequenceGroupModified -= 
                    m_OnSequenceGroupModified;
        }

        #endregion

        private MSequenceGroup m_SeqGrp;
        private System.Windows.Forms.Label lbName;
        private System.Windows.Forms.TextBox tbName;
        private System.Windows.Forms.TextBox tbGrpID;
        private System.Windows.Forms.Label lbGrpID;
        private static uint NumInstances = 0;

        private ArrayList m_kKnownSequences;
        private System.Windows.Forms.ToolTip ToolTip_SequenceGroup;
        private System.ComponentModel.IContainer components;

        public SequenceGroupCommon(MSequenceGroup SeqGrp)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_SeqGrp = SeqGrp;

            RegisterDelegates();
            InitAllStates();
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
            this.lbName = new System.Windows.Forms.Label();
            this.tbName = new System.Windows.Forms.TextBox();
            this.tbGrpID = new System.Windows.Forms.TextBox();
            this.lbGrpID = new System.Windows.Forms.Label();
            this.ToolTip_SequenceGroup = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // lbName
            // 
            this.lbName.Location = new System.Drawing.Point(72, 8);
            this.lbName.Name = "lbName";
            this.lbName.Size = new System.Drawing.Size(40, 16);
            this.lbName.TabIndex = 1;
            this.lbName.Text = "Name";
            // 
            // tbName
            // 
            this.tbName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbName.Location = new System.Drawing.Point(72, 24);
            this.tbName.Name = "tbName";
            this.tbName.ReadOnly = true;
            this.tbName.Size = new System.Drawing.Size(160, 20);
            this.tbName.TabIndex = 3;
            this.tbName.Text = "";
            this.ToolTip_SequenceGroup.SetToolTip(this.tbName, "The name of this sequence group.");
            // 
            // tbGrpID
            // 
            this.tbGrpID.Location = new System.Drawing.Point(8, 24);
            this.tbGrpID.Name = "tbGrpID";
            this.tbGrpID.ReadOnly = true;
            this.tbGrpID.Size = new System.Drawing.Size(48, 20);
            this.tbGrpID.TabIndex = 2;
            this.tbGrpID.Text = "";
            this.tbGrpID.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceGroup.SetToolTip(this.tbGrpID, "The unique identifier for this sequence group in the project.");
            // 
            // lbGrpID
            // 
            this.lbGrpID.Location = new System.Drawing.Point(8, 8);
            this.lbGrpID.Name = "lbGrpID";
            this.lbGrpID.Size = new System.Drawing.Size(56, 16);
            this.lbGrpID.TabIndex = 0;
            this.lbGrpID.Text = "Group ID";
            // 
            // ToolTip_SequenceGroup
            // 
            this.ToolTip_SequenceGroup.AutoPopDelay = 50000;
            this.ToolTip_SequenceGroup.InitialDelay = 500;
            this.ToolTip_SequenceGroup.ReshowDelay = 100;
            // 
            // SequenceGroupCommon
            // 
            this.Controls.Add(this.lbGrpID);
            this.Controls.Add(this.tbGrpID);
            this.Controls.Add(this.tbName);
            this.Controls.Add(this.lbName);
            this.Name = "SequenceGroupCommon";
            this.Size = new System.Drawing.Size(240, 56);
            this.Load += new System.EventHandler(this.SequenceGroupCommon_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void SequenceGroupCommon_Load(
            object sender, System.EventArgs e)
        {
        }

        public void InitAllStates()
        {
            tbName.Text = m_SeqGrp.Name;
            tbGrpID.Text = m_SeqGrp.GroupID.ToString();

            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.SequenceGroup, m_SeqGrp);
            MFramework.Instance.Clock.ScaleFactor = 1.0f;
            MFramework.Instance.Clock.Enabled = true;
            
            MSequenceGroup.MSequenceInfo[] aSeqInfo = m_SeqGrp.SequenceInfo;

            m_kKnownSequences = new ArrayList();
            m_kKnownSequences.Clear();
            if (aSeqInfo != null)
            {
                foreach (MSequenceGroup.MSequenceInfo seqInfo in aSeqInfo)
                {
                    // Fill our tree with sequence names...
                    if (seqInfo.DefaultActiveValue == true)
                        seqInfo.Activate();

                    m_kKnownSequences.Add(seqInfo.Sequence);
                }
            }
        }

        public void SetSequenceGroup(MSequenceGroup SeqGrp)
        {
            m_SeqGrp = SeqGrp;
            InitAllStates();
        }

        private void OnSequenceGroupModified(
            MSequenceGroupEventArgs Args, MSequenceGroup kGroup)
        {
            try 
            {
                if (kGroup == m_SeqGrp)
                {
                    switch (Args.Type)
                    {
                        case MSequenceGroup.PropertyType.Prop_GroupID:
                            tbGrpID.Text = m_SeqGrp.GroupID.ToString();
                            break;
                        case MSequenceGroup.PropertyType.Prop_Name:
                            tbName.Text = m_SeqGrp.Name;
                            break;
                        case MSequenceGroup.PropertyType.Prop_SequenceInfo:
                            if (kGroup.SequenceInfo != null)
                            {
                                MSequenceGroupChangedSequenceInfoEventArgs 
                                    kAddRemoveArgs = (
                                    MSequenceGroupChangedSequenceInfoEventArgs)
                                    Args;
                                if (kAddRemoveArgs.Added)
                                {
                                    int iIndex = 
                                        m_SeqGrp.GetSequenceInfoIndex(
                                            kAddRemoveArgs.SequenceID);
                                    MSequenceGroup.MSequenceInfo kInfo =
                                        m_SeqGrp.GetAt(iIndex);
                                    Debug.Assert(kInfo != null, 
                                        "Sequence Info is not valid");
                                    kInfo.Activate();
                                    m_kKnownSequences.Add(kInfo.Sequence);
                                
                                }
                                else 
                                {
                                    MSequence kSeq = 
                                        MFramework.Instance.Animation.
                                            GetSequence(
                                            kAddRemoveArgs.SequenceID);

                                    Debug.Assert(kSeq != null, 
                                        "Sequence Exists", 
                                        "Sequence does not exist.");
                                    if (kSeq.GetAnimState() != 
                                        MSequence.AnimState.INACTIVE)
                                    {
                                        MFramework.Instance.Animation.
                                            DeactivateSequence(
                                            kSeq.SequenceID, 
                                            kAddRemoveArgs.EaseOut);
                                    }
                                    m_kKnownSequences.Remove(kSeq);
                                }
                            }
                            break;
                    }
                }
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }
    }
}
