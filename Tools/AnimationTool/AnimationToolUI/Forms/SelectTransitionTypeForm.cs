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
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for SelectTransitionTypeForm.
    /// </summary>
    public class SelectTransitionTypeForm : System.Windows.Forms.Form
    {
        private ArrayList m_Transitions;
        private ArrayList m_kPossibleTransitionList;
        private System.Windows.Forms.Panel panel1;
        public MTransition.TransitionType m_eFinalType;
        public TransitionChainEditForm m_ChainEditForm;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.ListBox lbTransitions;
        private System.Windows.Forms.ToolTip ttSelectTransitionType;
        private System.Windows.Forms.Button btnHelp;
        private System.ComponentModel.IContainer components;

        public SelectTransitionTypeForm(ArrayList Transitions)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_Transitions = Transitions;
            
            m_kPossibleTransitionList = 
                AnimationHelpers.GetPossibleTransitionTypes(Transitions);

            
            m_eFinalType = AnimationHelpers.GetCommonStoredType(m_Transitions);
        
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnHelp = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.lbTransitions = new System.Windows.Forms.ListBox();
            this.ttSelectTransitionType = new System.Windows.Forms.ToolTip(this.components);
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btnHelp);
            this.panel1.Controls.Add(this.btnOK);
            this.panel1.Controls.Add(this.btnCancel);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(10, 156);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(332, 64);
            this.panel1.TabIndex = 1;
            // 
            // btnHelp
            // 
            this.btnHelp.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.btnHelp.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnHelp.Location = new System.Drawing.Point(218, 24);
            this.btnHelp.Name = "btnHelp";
            this.btnHelp.Size = new System.Drawing.Size(72, 32);
            this.btnHelp.TabIndex = 2;
            this.btnHelp.Text = "Help";
            this.btnHelp.Click += new System.EventHandler(this.btnHelp_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(42, 24);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(72, 32);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(130, 24);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(72, 32);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            // 
            // lbTransitions
            // 
            this.lbTransitions.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lbTransitions.Location = new System.Drawing.Point(10, 10);
            this.lbTransitions.Name = "lbTransitions";
            this.lbTransitions.Size = new System.Drawing.Size(332, 134);
            this.lbTransitions.TabIndex = 0;
            this.ttSelectTransitionType.SetToolTip(this.lbTransitions, "Double-click a transition type to change\nthe transition to that type.");
            this.lbTransitions.DoubleClick += new System.EventHandler(this.lbTransitions_DoubleClick);
            // 
            // SelectTransitionTypeForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(352, 230);
            this.ControlBox = false;
            this.Controls.Add(this.lbTransitions);
            this.Controls.Add(this.panel1);
            this.DockPadding.All = 10;
            this.Name = "SelectTransitionTypeForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Select Transition Type:";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.SelectTransitionTypeForm_Closing);
            this.Load += new System.EventHandler(this.SelectTransitionTypeForm_Load);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        public MSequence GetSource(int i)
        {
            return ((MTransitionData)m_Transitions[i]).Source;
        }
        
        public MSequence GetDest(int i)
        {
            return ((MTransitionData)m_Transitions[i]).Destination;
        }

        public MTransition GetTran(int i)
        {
            return ((MTransitionData)m_Transitions[i]).Transition;
        }

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            MTransition.TransitionType eType = (MTransition.TransitionType)
                m_kPossibleTransitionList[lbTransitions.SelectedIndex];
            
            if (eType == m_eFinalType)
            {
                DialogResult = DialogResult.Cancel;
            }
            else
            {
                DialogResult = DialogResult.OK;
            }
        }

        private void SelectTransitionTypeForm_Load(
            object sender, System.EventArgs e)
        {
            int iNoneIndex = 0;
            int iSelectedIndex = -1;
            int i = 0;
            foreach (
                MTransition.TransitionType eType in m_kPossibleTransitionList)
            {
                string Text = ProjectData.GetTypeNameForTransitionType(eType);
                lbTransitions.Items.Add(Text);

                if (m_eFinalType == eType)
                    iSelectedIndex = i;
                if (eType == MTransition.TransitionType.Trans_None)
                    iNoneIndex = i;
                i++;
            }
            
            if (iSelectedIndex == -1)
                iSelectedIndex = iNoneIndex;

            lbTransitions.SelectedIndex = iSelectedIndex;

            if (!HelpManager.HelpExists())
                btnHelp.Enabled = false;

        }

        private void lbTransitions_DoubleClick(
            object sender, System.EventArgs e)
        {
            btnOK_Click(this, null);
            this.Close();
        }

        public bool ConvertTran(ref ArrayList Transitions)
        {
            if (m_ChainEditForm != null)
            {
                return AnimationHelpers.ConvertTransition(
                    ref Transitions, m_eFinalType, 
                    ((TransitionChainEditForm.SeqTuple)
                     m_ChainEditForm.m_aSeqTuple[0]).m_aSequences);
            }
            else
            {
                return AnimationHelpers.ConvertTransition(
                    ref Transitions, m_eFinalType, 
                    null);
            }
        }

        private void SelectTransitionTypeForm_Closing(
            object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (DialogResult == DialogResult.OK)
            {
                MTransition.TransitionType eType = (MTransition.TransitionType)
                    m_kPossibleTransitionList[lbTransitions.SelectedIndex];
            
                m_eFinalType = eType;
                DialogResult = DialogResult.OK;
            }
        }

        private void btnHelp_Click(object sender, System.EventArgs e)
        {
            HelpManager.DoHelp(this, @"Basics/Transition_Types.htm");
        }
    }
}
