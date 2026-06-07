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
    /// Summary description for TransitionEdit.
    /// </summary>
    public class TransitionEdit : System.Windows.Forms.UserControl, IPostShow
    {
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.TextBox tbTranType;
        private System.Windows.Forms.GroupBox gbGroupName;
        private System.Windows.Forms.TextBox tbDestination;
        private System.Windows.Forms.Label lblDestination;
        private System.Windows.Forms.Label lblSource;
        private System.Windows.Forms.TextBox tbSource;
        private System.Windows.Forms.Label lblDurationInSeconds;
        private System.Windows.Forms.UserControl UserControl_Context;
        private ArrayList m_Transitions;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label1;
        private bool AllowDurationChange;
        private bool InitialDragPlayState;

        MAnimation.__Delegate_OnTransitionRemoved TransitionRemovedHandler;
        MAnimation.__Delegate_OnTransitionModified TransitionModifedHandler;
        MAnimation.__Delegate_OnDefaultTransitionSettingsChanged 
            TransitionSettingsChangedHandler;
        private System.Windows.Forms.Panel pnlTransitionType;
        private System.Windows.Forms.Button btnChangeTransitionType;
        private System.Windows.Forms.Panel pnlAdvanced;
        private System.Windows.Forms.ToolTip ttTransitionEdit;
        private AnimationToolUI.NumericSlider nsDuration;
        AppEvents.EventHandler_TransitionTableSettingsChanged 
            TransitionTableSettingsChangedHandler;
        private static uint NumInstances = 0;

        public void PostShow()
        {
            // check if the IPostShow interface exists
            if ( ( UserControl_Context is IPostShow ) )
            {
                ((IPostShow)UserControl_Context).PostShow();
            }
        }

        public TransitionEdit(ArrayList Transitions)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_Transitions = Transitions;
            AllowDurationChange = true;

            // Set label tool tips for numeric sliders. This needs to be done
            // to correct for a bug in which tool tips do not appear over
            // portions of the numeric slider.
            this.ttTransitionEdit.SetToolTip(this.lblDurationInSeconds,
                this.ttTransitionEdit.GetToolTip(this.nsDuration));

            InitTransitionEdit();

            TransitionRemovedHandler = 
                new MAnimation.__Delegate_OnTransitionRemoved(
                    OnTransitionRemoved);
            MFramework.Instance.Animation.OnTransitionRemoved += 
                TransitionRemovedHandler;
                
            TransitionModifedHandler = 
                new MAnimation.__Delegate_OnTransitionModified(
                    OnTransitionModified);

            MFramework.Instance.Animation.OnTransitionModified += 
                TransitionModifedHandler;
            
            TransitionSettingsChangedHandler = 
                new MAnimation.__Delegate_OnDefaultTransitionSettingsChanged(
                OnDefaultTransitionSettingsChanged);
            MFramework.Instance.Animation.OnDefaultTransitionSettingsChanged +=
                TransitionSettingsChangedHandler;    
            
            TransitionTableSettingsChangedHandler =
                new AppEvents.EventHandler_TransitionTableSettingsChanged(
                    TransitionTableSettingsChanged);
            AppEvents.TransitionTableSettingsChanged += 
                TransitionTableSettingsChangedHandler;

            bool bSuccess;
            if (m_Transitions.Count == 1 && GetTran(0) != null)
            {
                bSuccess = MFramework.Instance.Animation.SetPlaybackMode(
                    MAnimation.PlaybackMode.Transition, GetTran(0));
            }
            else
            {
                bSuccess = MFramework.Instance.Animation.SetPlaybackMode(
                    MAnimation.PlaybackMode.None, null);
            }

            AppEvents.RaiseTransitionEditLoaded();
            Debug.Assert(bSuccess);
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
            MFramework.Instance.Animation.OnTransitionRemoved -= 
                TransitionRemovedHandler;
            MFramework.Instance.Animation.OnTransitionModified -= 
                TransitionModifedHandler;
            MFramework.Instance.Animation.OnDefaultTransitionSettingsChanged -=
                TransitionSettingsChangedHandler;    
            AppEvents.TransitionTableSettingsChanged -= 
                TransitionTableSettingsChangedHandler;

        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tbTranType = new System.Windows.Forms.TextBox();
            this.pnlTransitionType = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.gbGroupName = new System.Windows.Forms.GroupBox();
            this.tbDestination = new System.Windows.Forms.TextBox();
            this.lblDestination = new System.Windows.Forms.Label();
            this.nsDuration = new AnimationToolUI.NumericSlider();
            this.lblSource = new System.Windows.Forms.Label();
            this.tbSource = new System.Windows.Forms.TextBox();
            this.lblDurationInSeconds = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnChangeTransitionType = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.pnlAdvanced = new System.Windows.Forms.Panel();
            this.ttTransitionEdit = new System.Windows.Forms.ToolTip(this.components);
            this.pnlTransitionType.SuspendLayout();
            this.panel2.SuspendLayout();
            this.gbGroupName.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tbTranType
            // 
            this.tbTranType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbTranType.BackColor = System.Drawing.SystemColors.Info;
            this.tbTranType.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbTranType.Location = new System.Drawing.Point(88, 6);
            this.tbTranType.Name = "tbTranType";
            this.tbTranType.ReadOnly = true;
            this.tbTranType.Size = new System.Drawing.Size(104, 20);
            this.tbTranType.TabIndex = 1;
            this.tbTranType.TabStop = false;
            this.tbTranType.Text = "Immediate";
            this.tbTranType.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttTransitionEdit.SetToolTip(this.tbTranType, "The type of this transition.");
            this.tbTranType.DoubleClick += new System.EventHandler(this.tbTranType_DoubleClick);
            // 
            // pnlTransitionType
            // 
            this.pnlTransitionType.Controls.Add(this.panel2);
            this.pnlTransitionType.Controls.Add(this.panel1);
            this.pnlTransitionType.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlTransitionType.DockPadding.All = 5;
            this.pnlTransitionType.Location = new System.Drawing.Point(5, 5);
            this.pnlTransitionType.Name = "pnlTransitionType";
            this.pnlTransitionType.Size = new System.Drawing.Size(238, 155);
            this.pnlTransitionType.TabIndex = 0;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.gbGroupName);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(5, 40);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(228, 110);
            this.panel2.TabIndex = 28;
            // 
            // gbGroupName
            // 
            this.gbGroupName.Controls.Add(this.tbDestination);
            this.gbGroupName.Controls.Add(this.lblDestination);
            this.gbGroupName.Controls.Add(this.nsDuration);
            this.gbGroupName.Controls.Add(this.lblSource);
            this.gbGroupName.Controls.Add(this.tbSource);
            this.gbGroupName.Controls.Add(this.lblDurationInSeconds);
            this.gbGroupName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gbGroupName.Location = new System.Drawing.Point(0, 0);
            this.gbGroupName.Name = "gbGroupName";
            this.gbGroupName.Size = new System.Drawing.Size(228, 110);
            this.gbGroupName.TabIndex = 0;
            this.gbGroupName.TabStop = false;
            this.gbGroupName.Text = "Common Transition Controls:";
            // 
            // tbDestination
            // 
            this.tbDestination.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbDestination.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbDestination.Location = new System.Drawing.Point(72, 47);
            this.tbDestination.Name = "tbDestination";
            this.tbDestination.ReadOnly = true;
            this.tbDestination.Size = new System.Drawing.Size(148, 20);
            this.tbDestination.TabIndex = 3;
            this.tbDestination.TabStop = false;
            this.tbDestination.Text = "Destination";
            this.ttTransitionEdit.SetToolTip(this.tbDestination, "The destination sequence for this transition.");
            // 
            // lblDestination
            // 
            this.lblDestination.Location = new System.Drawing.Point(8, 50);
            this.lblDestination.Name = "lblDestination";
            this.lblDestination.Size = new System.Drawing.Size(64, 16);
            this.lblDestination.TabIndex = 2;
            this.lblDestination.Text = "Destination:";
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
            this.nsDuration.Location = new System.Drawing.Point(72, 77);
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
            this.nsDuration.Size = new System.Drawing.Size(148, 28);
            this.nsDuration.TabIndex = 5;
            this.nsDuration.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ttTransitionEdit.SetToolTip(this.nsDuration, "The duration in seconds of this transition.");
            this.nsDuration.Value = new System.Decimal(new int[] {
                                                                     5,
                                                                     0,
                                                                     0,
                                                                     65536});
            this.nsDuration.BeginValueDrag += new AnimationToolUI.ValueChangedEventHandler(this.nsDuration_BeginValueDrag);
            this.nsDuration.EndValueDrag += new AnimationToolUI.ValueChangedEventHandler(this.nsDuration_EndValueDrag);
            this.nsDuration.ValueChanged += new AnimationToolUI.ValueChangedEventHandler(this.nsDuration_ValueChanged);
            // 
            // lblSource
            // 
            this.lblSource.Location = new System.Drawing.Point(16, 24);
            this.lblSource.Name = "lblSource";
            this.lblSource.Size = new System.Drawing.Size(48, 16);
            this.lblSource.TabIndex = 0;
            this.lblSource.Text = "Source:";
            // 
            // tbSource
            // 
            this.tbSource.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbSource.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbSource.Location = new System.Drawing.Point(72, 21);
            this.tbSource.Name = "tbSource";
            this.tbSource.ReadOnly = true;
            this.tbSource.Size = new System.Drawing.Size(148, 20);
            this.tbSource.TabIndex = 1;
            this.tbSource.TabStop = false;
            this.tbSource.Text = "Source";
            this.ttTransitionEdit.SetToolTip(this.tbSource, "The source sequence for this transition.");
            // 
            // lblDurationInSeconds
            // 
            this.lblDurationInSeconds.Location = new System.Drawing.Point(8, 74);
            this.lblDurationInSeconds.Name = "lblDurationInSeconds";
            this.lblDurationInSeconds.Size = new System.Drawing.Size(64, 32);
            this.lblDurationInSeconds.TabIndex = 4;
            this.lblDurationInSeconds.Text = "Duration in Seconds:";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.tbTranType);
            this.panel1.Controls.Add(this.btnChangeTransitionType);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(5, 5);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(228, 35);
            this.panel1.TabIndex = 0;
            // 
            // btnChangeTransitionType
            // 
            this.btnChangeTransitionType.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnChangeTransitionType.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnChangeTransitionType.Location = new System.Drawing.Point(200, 8);
            this.btnChangeTransitionType.Name = "btnChangeTransitionType";
            this.btnChangeTransitionType.Size = new System.Drawing.Size(24, 20);
            this.btnChangeTransitionType.TabIndex = 2;
            this.btnChangeTransitionType.Text = "...";
            this.ttTransitionEdit.SetToolTip(this.btnChangeTransitionType, "Click to change the type of this transition.");
            this.btnChangeTransitionType.Click += new System.EventHandler(this.btnChangeTransitionType_Click);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(0, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(88, 16);
            this.label1.TabIndex = 0;
            this.label1.Text = "Transition Type:";
            // 
            // pnlAdvanced
            // 
            this.pnlAdvanced.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlAdvanced.Location = new System.Drawing.Point(5, 160);
            this.pnlAdvanced.Name = "pnlAdvanced";
            this.pnlAdvanced.Size = new System.Drawing.Size(238, 371);
            this.pnlAdvanced.TabIndex = 1;
            // 
            // TransitionEdit
            // 
            this.AutoScroll = true;
            this.Controls.Add(this.pnlAdvanced);
            this.Controls.Add(this.pnlTransitionType);
            this.DockPadding.All = 5;
            this.Name = "TransitionEdit";
            this.Size = new System.Drawing.Size(248, 536);
            this.Load += new System.EventHandler(this.TransitionEdit_Load);
            this.pnlTransitionType.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.gbGroupName.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        public MSequence GetSource(int i)
        {
            Debug.Assert(m_Transitions != null);
            Debug.Assert(m_Transitions.Count > i);
            Debug.Assert(((MTransitionData)m_Transitions[i]) != null);
            return ((MTransitionData)m_Transitions[i]).Source;
        }
        
        public MSequence GetDest(int i)
        {
            Debug.Assert(m_Transitions != null);
            Debug.Assert(m_Transitions.Count > i);
            Debug.Assert(((MTransitionData)m_Transitions[i]) != null);
            return ((MTransitionData)m_Transitions[i]).Destination;
        }

        public MTransition GetTran(int i)
        {
            Debug.Assert(m_Transitions != null);
            Debug.Assert(m_Transitions.Count > i);
            Debug.Assert(((MTransitionData)m_Transitions[i]) != null);
            return ((MTransitionData)m_Transitions[i]).Transition;
        }

        public float GetCommonDuration()
        {
            float fDuration = 0.0f;

            ArrayList kList = m_Transitions;

            MTransition kTran = GetTran(0);
            if (kTran != null)
                fDuration = kTran.Duration;

            for (int i = 1; i < kList.Count; i++)
            {
                MTransitionData data = (MTransitionData) kList[i];
                if (data.Source == data.Destination)
                    continue;

                if (data.Transition != null && 
                    data.Transition.Duration != fDuration)
                {
                    // Not in common
                    return -1.0f;
                }
            }

            return fDuration;
        }

        public MTransition.TransitionType GetCommonStoredType()
        {
            ArrayList kList = m_Transitions;
            MTransition.TransitionType eType = MTransition.TransitionType
                .Trans_None;

            MTransition kTran = GetTran(0);
            if (kTran != null)
                eType = kTran.StoredType;

            for (int i = 1; i < kList.Count; i++)
            {
                MTransitionData data = (MTransitionData) kList[i];
                if (data.Source == data.Destination)
                    continue;

                if (eType == MTransition.TransitionType.Trans_None)
                {
                    // This is a special case where the first row
                    // or column has been chosen and hence, the first
                    // cell has a transition type of none...
                    // so we find the next to use the type.
                    if (data.Transition != null && 
                        data.Transition.StoredType != eType)
                    {
                        eType = data.Transition.StoredType;
                    }
                }
                else if (data.Transition != null && 
                    data.Transition.StoredType != eType)
                {
                    return MTransition.TransitionType.Trans_MultipleSelected;
                }
                //else if (data.Transition == null && eType != 
                //    MTransition.TransitionType.Trans_None)
                //{
                //    return MTransition.TransitionType.Trans_MultipleSelected;
                //}
            }

            return eType;
        }

        public MTransition.TransitionType GetCommonType()
        {
            ArrayList kList = m_Transitions;
            MTransition.TransitionType eType = MTransition.TransitionType
                .Trans_None;

            MTransition kTran = GetTran(0);
            if (kTran != null)
                eType = kTran.Type;

            for (int i = 1; i < kList.Count; i++)
            {
                MTransitionData data = (MTransitionData) kList[i];
                
                if (data.Source == data.Destination)
                    continue;

                if (data.Transition != null && 
                    data.Transition.Type != eType)
                {
                    return MTransition.TransitionType.Trans_MultipleSelected;
                }
                else if (data.Transition == null && eType != 
                    MTransition.TransitionType.Trans_None)
                {
                    return MTransition.TransitionType.Trans_MultipleSelected;
                }
            }

            return eType;
        }

        private void SetupTransitionTypeNames(MTransition.TransitionType eType)
        {
            System.Drawing.Color BackColor = 
                ProjectData.GetColorForTransitionType(eType);
            System.Drawing.Color TextColor = 
                ProjectData.GetTextColorForTransitionType(eType);
            tbTranType.BackColor = BackColor;
            tbTranType.ForeColor = TextColor;
            tbTranType.Text = ProjectData.GetTypeNameForTransitionType(eType);
        }

        private void InitTransitionEdit()
        {
            this.SuspendLayout();
            if (UserControl_Context != null)
                UserControl_Context.Dispose();

            if (m_Transitions.Count == 1)
            {
                Debug.Assert(GetSource(0) != null, 
                    "Source must exist!", "GetSource(0) != null");
                Debug.Assert(GetDest(0) != null, 
                    "Dest must exist!", "GetDest(0) != null");
             
                tbSource.Text = GetSource(0).Name;
                tbDestination.Text = GetDest(0).Name;

                if (GetSource(0) == GetDest(0))
                {
                    btnChangeTransitionType.Enabled = false;
                }
            }
            else
            {
                bool bMultipleSrc = false;
                bool bMultipleDes = false;
                uint uiSrcID = 
                    ((MTransitionData)m_Transitions[0]).Source.SequenceID;
                uint uiDesID = 
                    ((MTransitionData)m_Transitions[0]).Destination.SequenceID;

                foreach (MTransitionData t in m_Transitions)
                {
                    if (uiSrcID != t.Source.SequenceID)
                        bMultipleSrc = true;

                    if (uiDesID != t.Destination.SequenceID)
                        bMultipleDes = true;

                }
                if (bMultipleSrc == true)
                    tbSource.Text = "<Multiple Sources>";
                else
                    tbSource.Text = GetSource(0).Name;

                if (bMultipleDes == true)
                    tbDestination.Text = "<Multiple Destinations>";
                else
                    tbDestination.Text = GetDest(0).Name;
            }

            MTransition.TransitionType eType = MTransition.TransitionType
                .Trans_None;

            if (m_Transitions.Count == 1 && GetTran(0) != null)
            {
                MTransition Tran = GetTran(0);
                eType = Tran.StoredType;
                decimal dValue = Convert.ToDecimal(Tran.Duration);
                if (nsDuration.Value != dValue)
                {
                    AllowDurationChange = false;
                    nsDuration.Value = dValue;
                }

                if (eType == MTransition.TransitionType.Trans_DefaultSync ||
                    eType == MTransition.TransitionType.Trans_DefaultNonSync
                    || eType == MTransition.TransitionType.Trans_Chain)
                {
                    nsDuration.Enabled = false;
                }
                else
                {
                    nsDuration.Enabled = true;
                }
            }
            else if (m_Transitions.Count == 1 && GetTran(0) == null)
            {
                decimal dValue = Convert.ToDecimal(0.00f);
                if (nsDuration.Value != dValue)
                {
                    AllowDurationChange = false;
                    nsDuration.Value = dValue;
                }
                nsDuration.Enabled = false;
            }
            else
            {
                eType = GetCommonStoredType();
                nsDuration.Enabled = true;
                
                switch(eType)
                {
                    case MTransition.TransitionType.Trans_None:
                    case MTransition.TransitionType.Trans_Chain:
                    case MTransition.TransitionType.Trans_DefaultSync:
                    case MTransition.TransitionType.Trans_DefaultNonSync:
                        nsDuration.Enabled = false;
                        break;
                }
            
                decimal dValue = Convert.ToDecimal( GetCommonDuration() );
                if (nsDuration.Value != dValue)
                {
                    if ((float)dValue == -1.0f)
                    {
                        nsDuration.NumericSpinner.ForeColor = 
                            System.Drawing.Color.Red;
                    }

                    AllowDurationChange = false;
                    nsDuration.Value = dValue;
                }
            }

            SetupTransitionTypeNames(eType);

            UserControl_Context = null;

            if (m_Transitions.Count == 1)
            {
                switch(eType)
                {
                    case MTransition.TransitionType.Trans_ImmediateBlend:
                    {
                        TransitionImmediateBlendEdit kImmediateBlend =
                            new TransitionImmediateBlendEdit(GetTran(0));
                        UserControl_Context = kImmediateBlend;
                        break;
                    }
                    case MTransition.TransitionType.Trans_DelayedBlend:
                    {
                        TransitionDelayedBlendEdit kDelayedBlend =
                            new TransitionDelayedBlendEdit(GetTran(0));
                        UserControl_Context = kDelayedBlend;
                        break;
                    }
                    case MTransition.TransitionType.Trans_Chain:
                    {
                        TransitionChainEdit kChain =
                            new TransitionChainEdit(GetTran(0));
                        UserControl_Context = kChain;
                        break;
                    }
                    case MTransition.TransitionType.Trans_Morph:
                        break;
                    case MTransition.TransitionType.Trans_CrossFade:
                        break;
                    case MTransition.TransitionType.Trans_DefaultSync:
                    case MTransition.TransitionType.Trans_DefaultNonSync:
                    {
                        TransitionDefaultEdit kDefaultEdit = new
                            TransitionDefaultEdit(GetTran(0));
                        UserControl_Context = kDefaultEdit;
                        break;
                    }
                }
            }
            else
            {
                switch(eType)
                {
                    case MTransition.TransitionType.Trans_DefaultSync:
                    case MTransition.TransitionType.Trans_DefaultNonSync:
                    {
                        // To handle multiple selection Defaults, we create
                        // an ArrayList that contains only the transitions
                        // that will be affected. This at first seems a little
                        // strange because the "Default" types are global,
                        // however, the user does have the option of making
                        // his/her selection "Unique", so we must pass the
                        // selected array.
                        ArrayList al = 
                            AnimationHelpers.GetTransStoredTypeSubset(
                            m_Transitions, eType);
                        TransitionDefaultEdit kDefaultEdit = new
                            TransitionDefaultEdit(al);

                        UserControl_Context = kDefaultEdit;
                        break;
                    }

                    case MTransition.TransitionType.Trans_DelayedBlend:
                    {
            // To handle multiple selection of delayed blends, we must create
            // an ArrayList that contains only the blends that we wish
            // to edit and no other transitions.                   
                        ArrayList al = 
                            AnimationHelpers.GetTransTypeSubset(m_Transitions,
                            MTransition.TransitionType.Trans_DelayedBlend);
                      
                        TransitionDelayedBlendEdit kDelayedBlend =
                            new TransitionDelayedBlendEdit(al);
                        UserControl_Context = kDelayedBlend;

                        break;
                    }

                    case MTransition.TransitionType.Trans_Chain:
                    {
                    // To handle multiple selection of chains, we must create
                    // an ArrayList that contains only the chains that we wish
                    // to edit and no other transitions.                   
                        ArrayList al = 
                            AnimationHelpers.GetTransTypeSubset(m_Transitions,
                            MTransition.TransitionType.Trans_Chain);
                    
                        TransitionChainEdit kChain =
                            new TransitionChainEdit(al);
                        UserControl_Context = kChain;

                        break;
                    }
                }
            }
            
            pnlAdvanced.Controls.Clear(); // remove the previous control

            if (UserControl_Context != null)
            {
                pnlAdvanced.Height = UserControl_Context.Height;
                UserControl_Context.Dock = System.Windows.Forms.DockStyle.Fill;
                pnlAdvanced.Controls.Add(UserControl_Context);
                UserControl_Context.Show();
            }

            this.ResumeLayout();
        }

        private void TransitionEdit_Load(object sender, System.EventArgs e)
        {
        }

        private void OnTransitionModified(
            MTransition.PropertyType ePropChanged, MTransition kTransition)
        {
            if (ePropChanged == MTransition.PropertyType.Prop_Type)
            {
                InitTransitionEdit();
                Invalidate();
            }
        }

        private void OnTransitionRemoved(MTransition kTransition)
        {
            foreach(MTransitionData kData in m_Transitions)
            {
                if (kData.Source == kTransition.Source &&
                    kData.Destination == kTransition.Destination)
                {
                    MFramework.Instance.Animation.SetPlaybackMode(
                        MAnimation.PlaybackMode.None, null);
                    kData.Transition = null;
                    InitTransitionEdit();
                    Invalidate();
                }
            }
        }

        private void TransitionTableSettingsChanged()
        {
            InitTransitionEdit();
            Invalidate();
        }

        private void OnDefaultTransitionSettingsChanged(
            MTransition.TransitionType eType)
        {
            InitTransitionEdit();
            Invalidate();
        }

        private void DoChangeTransitionTypeDialog()
        {
            bool bChanged = WorkSheet.DoChangeTransitionTypeDialog(
                ref m_Transitions);
        }

        private void tbTranType_DoubleClick(
            object sender, System.EventArgs e)
        {
            DoChangeTransitionTypeDialog();
        }

        private void btnChangeTransitionType_Click(
            object sender, System.EventArgs e)
        {
            DoChangeTransitionTypeDialog();
        }

        private void nsDuration_ValueChanged(object sender, decimal Value)
        {
            if (AllowDurationChange)
            {
                if (Value <= 0.0M)
                {
                    nsDuration.Value = 0.00001M;
                    nsDuration.Invalidate();
                }

                foreach (MTransitionData data in m_Transitions)
                {
                    if (data.Transition != null)
                    {
                        data.Transition.Duration = (float)nsDuration.Value;
                    }
                }

                nsDuration.NumericSpinner.ForeColor = 
                    System.Drawing.SystemColors.WindowText;
            }
           
            AllowDurationChange = true;
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
    }
}
