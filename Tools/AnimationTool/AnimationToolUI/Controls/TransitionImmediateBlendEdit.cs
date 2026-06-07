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
    /// Summary description for TransitionImmediateBlendEdit.
    /// </summary>
    public class TransitionImmediateBlendEdit : 
        System.Windows.Forms.UserControl
    {
        private System.Windows.Forms.Label lblTimeInDestination;
        private MTransition m_Tran;
        private System.Windows.Forms.GroupBox gbImmediateBlendControls;
        private System.Windows.Forms.ComboBox cbDestTags;
        private System.Windows.Forms.ToolTip ttImmediateBlendEdit;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public TransitionImmediateBlendEdit(MTransition Tran)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;
            
            m_Tran = Tran;
            Debug.Assert(m_Tran.StoredType == 
                MTransition.TransitionType.Trans_ImmediateBlend);
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
            this.gbImmediateBlendControls = new System.Windows.Forms.GroupBox();
            this.cbDestTags = new System.Windows.Forms.ComboBox();
            this.lblTimeInDestination = new System.Windows.Forms.Label();
            this.ttImmediateBlendEdit = new System.Windows.Forms.ToolTip(this.components);
            this.gbImmediateBlendControls.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbImmediateBlendControls
            // 
            this.gbImmediateBlendControls.Controls.Add(this.cbDestTags);
            this.gbImmediateBlendControls.Controls.Add(this.lblTimeInDestination);
            this.gbImmediateBlendControls.Dock = System.Windows.Forms.DockStyle.Top;
            this.gbImmediateBlendControls.Location = new System.Drawing.Point(5, 5);
            this.gbImmediateBlendControls.Name = "gbImmediateBlendControls";
            this.gbImmediateBlendControls.Size = new System.Drawing.Size(198, 67);
            this.gbImmediateBlendControls.TabIndex = 0;
            this.gbImmediateBlendControls.TabStop = false;
            this.gbImmediateBlendControls.Text = "Immediate Blend Controls:";
            // 
            // cbDestTags
            // 
            this.cbDestTags.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbDestTags.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbDestTags.Location = new System.Drawing.Point(112, 30);
            this.cbDestTags.Name = "cbDestTags";
            this.cbDestTags.Size = new System.Drawing.Size(72, 21);
            this.cbDestTags.TabIndex = 1;
            this.ttImmediateBlendEdit.SetToolTip(this.cbDestTags, "The text key in the destination sequence that\nindicates the frame to blend to.");
            this.cbDestTags.SelectedIndexChanged += new System.EventHandler(this.cbDestTags_SelectedIndexChanged);
            // 
            // lblTimeInDestination
            // 
            this.lblTimeInDestination.Location = new System.Drawing.Point(8, 24);
            this.lblTimeInDestination.Name = "lblTimeInDestination";
            this.lblTimeInDestination.Size = new System.Drawing.Size(104, 32);
            this.lblTimeInDestination.TabIndex = 0;
            this.lblTimeInDestination.Text = "Time in destination to blend to:";
            // 
            // TransitionImmediateBlendEdit
            // 
            this.Controls.Add(this.gbImmediateBlendControls);
            this.DockPadding.All = 5;
            this.Name = "TransitionImmediateBlendEdit";
            this.Size = new System.Drawing.Size(208, 88);
            this.Load += new System.EventHandler(this.TransitionImmediateBlendEdit_Load);
            this.gbImmediateBlendControls.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void InitImmediateBlendControls()
        {
            cbDestTags.Items.Clear();
            string strOffsetKey = m_Tran.ImmediateOffsetTextKey;
            if (strOffsetKey == null)
                strOffsetKey = "start";

            int iSelIndex = 0;
            int iIndex = 0;
            foreach (MSequence.MTextKey key in m_Tran.Destination.TextKeys)
            {
                if (key.Text == strOffsetKey)
                    iSelIndex = iIndex;
                cbDestTags.Items.Add(key.Text);
                iIndex++;
            }
            cbDestTags.SelectedIndex = iSelIndex;
        }

        private void TransitionImmediateBlendEdit_Load(
            object sender, System.EventArgs e)
        {
            InitImmediateBlendControls();
        }

        private void cbDestTags_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            string strOffsetKey = m_Tran.ImmediateOffsetTextKey;
            if (strOffsetKey == null)
                strOffsetKey = "start";
            string strNewOffsetKey = 
                (string)cbDestTags.Items[cbDestTags.SelectedIndex];
            
            if (!strOffsetKey.Equals(strNewOffsetKey))
            {
                m_Tran.ImmediateOffsetTextKey = strNewOffsetKey;
                InitImmediateBlendControls();
                Invalidate(true);
            }
        }
    }
}
