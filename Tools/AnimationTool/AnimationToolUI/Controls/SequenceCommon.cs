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
using System.Windows.Forms.Design;
using System.Diagnostics;
using System.IO;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for SequenceCommon.
    /// </summary>
    public class SequenceCommon : System.Windows.Forms.UserControl
    {
        #region Delegates

        private MAnimation.__Delegate_OnNewKFM m_OnNewKFM = null;
        private MAnimation.__Delegate_OnKFMLoaded m_OnKFMLoaded = null;
        private MAnimation.__Delegate_OnSequenceModified m_OnSequenceModified =
            null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_OnNewKFM = new MAnimation.__Delegate_OnNewKFM(KFMChanged);
            m_OnKFMLoaded = new MAnimation.__Delegate_OnKFMLoaded(KFMChanged);
            m_OnSequenceModified = 
                new MAnimation.__Delegate_OnSequenceModified(SequenceModified);

            // Register delegates.
            MFramework.Instance.Animation.OnNewKFM += m_OnNewKFM;
            MFramework.Instance.Animation.OnKFMLoaded += m_OnKFMLoaded;
            MFramework.Instance.Animation.OnSequenceModified += 
                m_OnSequenceModified;
        }

        private void UnregisterDelegates()
        {
            if (m_OnNewKFM != null)
                MFramework.Instance.Animation.OnNewKFM -= m_OnNewKFM;
            if (m_OnKFMLoaded != null)
                MFramework.Instance.Animation.OnKFMLoaded -= m_OnKFMLoaded;
            if (m_OnSequenceModified != null)
                MFramework.Instance.Animation.OnSequenceModified -= 
                    m_OnSequenceModified;
        }

        #endregion

        private const string m_strName = "Name";
        private const string m_strSequenceID = "ID";
        private const string m_strFilename = "KF Filename";
        private const string m_strAnimIndex = "KF Index";
        private MSequence m_kSequence;
        private System.Windows.Forms.ToolTip ToolTip_SequenceProperties;
        private System.Windows.Forms.Label Label_Frequency;
        private System.Windows.Forms.TextBox Edit_Frequency;
        private System.Windows.Forms.Label Label_CycleType;
        private System.Windows.Forms.TextBox Edit_CycleType;
        private System.Windows.Forms.Label Label_AnimIndex;
        private System.Windows.Forms.TextBox Edit_AnimIndex;
        private System.Windows.Forms.Label Label_Filename;
        private System.Windows.Forms.TextBox Edit_Filename;
        private System.Windows.Forms.Label Label_Name;
        private System.Windows.Forms.TextBox Edit_Name;
        private System.Windows.Forms.TextBox Edit_ID;
        private System.Windows.Forms.Label Label_ID;
        private System.Windows.Forms.Label Label_End;
        private System.Windows.Forms.TextBox Edit_End;
        private System.Windows.Forms.TextBox Edit_Begin;
        private System.Windows.Forms.Label Label_Begin;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public SequenceCommon(MSequence kSequence)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_kSequence = kSequence;

            RegisterDelegates();
            MFramework.Instance.Animation.SetPlaybackMode(
                MAnimation.PlaybackMode.Sequence, m_kSequence);
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
            this.ToolTip_SequenceProperties = new System.Windows.Forms.ToolTip(this.components);
            this.Edit_Frequency = new System.Windows.Forms.TextBox();
            this.Edit_CycleType = new System.Windows.Forms.TextBox();
            this.Edit_AnimIndex = new System.Windows.Forms.TextBox();
            this.Edit_Filename = new System.Windows.Forms.TextBox();
            this.Edit_Name = new System.Windows.Forms.TextBox();
            this.Edit_ID = new System.Windows.Forms.TextBox();
            this.Edit_End = new System.Windows.Forms.TextBox();
            this.Edit_Begin = new System.Windows.Forms.TextBox();
            this.Label_Frequency = new System.Windows.Forms.Label();
            this.Label_CycleType = new System.Windows.Forms.Label();
            this.Label_AnimIndex = new System.Windows.Forms.Label();
            this.Label_Filename = new System.Windows.Forms.Label();
            this.Label_Name = new System.Windows.Forms.Label();
            this.Label_ID = new System.Windows.Forms.Label();
            this.Label_End = new System.Windows.Forms.Label();
            this.Label_Begin = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // ToolTip_SequenceProperties
            // 
            this.ToolTip_SequenceProperties.AutoPopDelay = 50000;
            this.ToolTip_SequenceProperties.InitialDelay = 500;
            this.ToolTip_SequenceProperties.ReshowDelay = 100;
            // 
            // Edit_Frequency
            // 
            this.Edit_Frequency.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_Frequency.Location = new System.Drawing.Point(72, 176);
            this.Edit_Frequency.Name = "Edit_Frequency";
            this.Edit_Frequency.ReadOnly = true;
            this.Edit_Frequency.Size = new System.Drawing.Size(181, 20);
            this.Edit_Frequency.TabIndex = 15;
            this.Edit_Frequency.Text = "";
            this.Edit_Frequency.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_Frequency, "A multiplier that is applied to the sequence update time.");
            // 
            // Edit_CycleType
            // 
            this.Edit_CycleType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_CycleType.Location = new System.Drawing.Point(72, 152);
            this.Edit_CycleType.Name = "Edit_CycleType";
            this.Edit_CycleType.ReadOnly = true;
            this.Edit_CycleType.Size = new System.Drawing.Size(181, 20);
            this.Edit_CycleType.TabIndex = 13;
            this.Edit_CycleType.Text = "";
            this.Edit_CycleType.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_CycleType, "The cycle type of the sequence is either loop or clamp. Clamp plays once.\nLoop wi" +
                "ll play the sequence over again until it is deactivated.");
            // 
            // Edit_AnimIndex
            // 
            this.Edit_AnimIndex.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_AnimIndex.Location = new System.Drawing.Point(72, 72);
            this.Edit_AnimIndex.Name = "Edit_AnimIndex";
            this.Edit_AnimIndex.ReadOnly = true;
            this.Edit_AnimIndex.Size = new System.Drawing.Size(181, 20);
            this.Edit_AnimIndex.TabIndex = 7;
            this.Edit_AnimIndex.Text = "";
            this.Edit_AnimIndex.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_AnimIndex, "Each KF file can have more than one sequence within it. The anim index\nis the ind" +
                "ex at which the current sequence occurs in the KF file.");
            // 
            // Edit_Filename
            // 
            this.Edit_Filename.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_Filename.Location = new System.Drawing.Point(72, 48);
            this.Edit_Filename.Name = "Edit_Filename";
            this.Edit_Filename.ReadOnly = true;
            this.Edit_Filename.Size = new System.Drawing.Size(181, 20);
            this.Edit_Filename.TabIndex = 5;
            this.Edit_Filename.Text = "";
            this.Edit_Filename.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_Filename, "The KF file from which this sequence was loaded.");
            // 
            // Edit_Name
            // 
            this.Edit_Name.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_Name.Location = new System.Drawing.Point(72, 24);
            this.Edit_Name.Name = "Edit_Name";
            this.Edit_Name.ReadOnly = true;
            this.Edit_Name.Size = new System.Drawing.Size(181, 20);
            this.Edit_Name.TabIndex = 3;
            this.Edit_Name.Text = "";
            this.Edit_Name.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_Name, "The name of this sequence.");
            // 
            // Edit_ID
            // 
            this.Edit_ID.Location = new System.Drawing.Point(8, 24);
            this.Edit_ID.Name = "Edit_ID";
            this.Edit_ID.ReadOnly = true;
            this.Edit_ID.Size = new System.Drawing.Size(40, 20);
            this.Edit_ID.TabIndex = 2;
            this.Edit_ID.Text = "";
            this.Edit_ID.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_ID, "The unique identifier of this sequence in the project.");
            // 
            // Edit_End
            // 
            this.Edit_End.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_End.Location = new System.Drawing.Point(72, 128);
            this.Edit_End.Name = "Edit_End";
            this.Edit_End.ReadOnly = true;
            this.Edit_End.Size = new System.Drawing.Size(181, 20);
            this.Edit_End.TabIndex = 11;
            this.Edit_End.Text = "";
            this.Edit_End.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_End, "The time in seconds of the last animation key in this sequence.");
            // 
            // Edit_Begin
            // 
            this.Edit_Begin.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.Edit_Begin.Location = new System.Drawing.Point(72, 104);
            this.Edit_Begin.Name = "Edit_Begin";
            this.Edit_Begin.ReadOnly = true;
            this.Edit_Begin.Size = new System.Drawing.Size(181, 20);
            this.Edit_Begin.TabIndex = 9;
            this.Edit_Begin.Text = "";
            this.Edit_Begin.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_Begin, "The time in seconds of the first animation key in this sequence.");
            // 
            // Label_Frequency
            // 
            this.Label_Frequency.Location = new System.Drawing.Point(8, 176);
            this.Label_Frequency.Name = "Label_Frequency";
            this.Label_Frequency.Size = new System.Drawing.Size(64, 16);
            this.Label_Frequency.TabIndex = 14;
            this.Label_Frequency.Text = "Frequency:";
            // 
            // Label_CycleType
            // 
            this.Label_CycleType.Location = new System.Drawing.Point(8, 152);
            this.Label_CycleType.Name = "Label_CycleType";
            this.Label_CycleType.Size = new System.Drawing.Size(64, 16);
            this.Label_CycleType.TabIndex = 12;
            this.Label_CycleType.Text = "Cycle Type:";
            // 
            // Label_AnimIndex
            // 
            this.Label_AnimIndex.Location = new System.Drawing.Point(8, 72);
            this.Label_AnimIndex.Name = "Label_AnimIndex";
            this.Label_AnimIndex.Size = new System.Drawing.Size(64, 16);
            this.Label_AnimIndex.TabIndex = 6;
            this.Label_AnimIndex.Text = "Anim Index:";
            // 
            // Label_Filename
            // 
            this.Label_Filename.Location = new System.Drawing.Point(8, 48);
            this.Label_Filename.Name = "Label_Filename";
            this.Label_Filename.Size = new System.Drawing.Size(56, 16);
            this.Label_Filename.TabIndex = 4;
            this.Label_Filename.Text = "Filename:";
            // 
            // Label_Name
            // 
            this.Label_Name.Location = new System.Drawing.Point(72, 8);
            this.Label_Name.Name = "Label_Name";
            this.Label_Name.Size = new System.Drawing.Size(40, 16);
            this.Label_Name.TabIndex = 1;
            this.Label_Name.Text = "Name";
            // 
            // Label_ID
            // 
            this.Label_ID.Location = new System.Drawing.Point(8, 8);
            this.Label_ID.Name = "Label_ID";
            this.Label_ID.Size = new System.Drawing.Size(56, 16);
            this.Label_ID.TabIndex = 0;
            this.Label_ID.Text = "ID";
            // 
            // Label_End
            // 
            this.Label_End.Location = new System.Drawing.Point(8, 128);
            this.Label_End.Name = "Label_End";
            this.Label_End.Size = new System.Drawing.Size(64, 14);
            this.Label_End.TabIndex = 10;
            this.Label_End.Text = "End:";
            // 
            // Label_Begin
            // 
            this.Label_Begin.Location = new System.Drawing.Point(8, 104);
            this.Label_Begin.Name = "Label_Begin";
            this.Label_Begin.Size = new System.Drawing.Size(48, 15);
            this.Label_Begin.TabIndex = 8;
            this.Label_Begin.Text = "Begin:";
            // 
            // SequenceCommon
            // 
            this.AutoScroll = true;
            this.Controls.Add(this.Edit_Frequency);
            this.Controls.Add(this.Edit_Begin);
            this.Controls.Add(this.Edit_ID);
            this.Controls.Add(this.Label_ID);
            this.Controls.Add(this.Label_Filename);
            this.Controls.Add(this.Label_CycleType);
            this.Controls.Add(this.Edit_CycleType);
            this.Controls.Add(this.Label_Begin);
            this.Controls.Add(this.Label_Name);
            this.Controls.Add(this.Edit_Name);
            this.Controls.Add(this.Label_Frequency);
            this.Controls.Add(this.Edit_End);
            this.Controls.Add(this.Label_End);
            this.Controls.Add(this.Edit_AnimIndex);
            this.Controls.Add(this.Label_AnimIndex);
            this.Controls.Add(this.Edit_Filename);
            this.DockPadding.All = 5;
            this.Name = "SequenceCommon";
            this.Size = new System.Drawing.Size(264, 208);
            this.Load += new System.EventHandler(this.SequenceCommon_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void SequenceCommon_Load(object sender, System.EventArgs e)
        {
            // Set label tooltips.
            ToolTip_SequenceProperties.SetToolTip(Label_Frequency,
                ToolTip_SequenceProperties.GetToolTip(Edit_Frequency));
            ToolTip_SequenceProperties.SetToolTip(Label_CycleType,
                ToolTip_SequenceProperties.GetToolTip(Edit_CycleType));
            ToolTip_SequenceProperties.SetToolTip(Label_AnimIndex,
                ToolTip_SequenceProperties.GetToolTip(Edit_AnimIndex));
            ToolTip_SequenceProperties.SetToolTip(Label_Filename,
                ToolTip_SequenceProperties.GetToolTip(Edit_Filename));
            ToolTip_SequenceProperties.SetToolTip(Label_Name,
                ToolTip_SequenceProperties.GetToolTip(Edit_Name));
            ToolTip_SequenceProperties.SetToolTip(Label_ID,
                ToolTip_SequenceProperties.GetToolTip(Edit_ID));
            ToolTip_SequenceProperties.SetToolTip(Label_End,
                ToolTip_SequenceProperties.GetToolTip(Edit_End));
            ToolTip_SequenceProperties.SetToolTip(Label_Begin,
                ToolTip_SequenceProperties.GetToolTip(Edit_Begin));

            UpdateSequenceUI();
        }

        private void SequenceModified(MSequence.PropertyType ePropChanged,
            MSequence kSequence)
        {
            if (kSequence == m_kSequence)
                UpdateSequenceUI();
        }

        private void UpdateSequenceUI()
        {
            Debug.Assert(m_kSequence != null);

            string strFullKFFilename = MFramework.Instance.Animation
                .GetFullKFFilename(m_kSequence.SequenceID);

            Edit_Name.Text = m_kSequence.Name;
            Edit_ID.Text = m_kSequence.SequenceID.ToString();
            Edit_Filename.Text = Path.GetFileName(strFullKFFilename);
            Edit_AnimIndex.Text = m_kSequence.AnimIndex.ToString() ;
            if (m_kSequence.Loop)
                Edit_CycleType.Text =  "Loop";
            else
                Edit_CycleType.Text =  "Clamp";
            Edit_Frequency.Text = m_kSequence.Frequency.ToString("f3");
            Edit_Begin.Text = 0.ToString("f3");
            Edit_End.Text = m_kSequence.Duration.ToString("f3");

            // Update filename text box and label tool tip.
            string strToolTip = this.ToolTip_SequenceProperties.GetToolTip(
                this.Edit_Filename);
            char[] acSeparators = new char[] {'\n'};
            string[] astrLines = strToolTip.Split(acSeparators);
            strToolTip = astrLines[0] + "\n" + strFullKFFilename;
            this.ToolTip_SequenceProperties.SetToolTip(this.Edit_Filename,
                strToolTip);
            this.ToolTip_SequenceProperties.SetToolTip(this.Label_Filename,
                strToolTip);
        }

        
        private void KFMChanged()
        {
            try
            {
                m_kSequence = null;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }
    }
}
