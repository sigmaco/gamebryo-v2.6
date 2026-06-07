// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    /// <summary>
    /// Summary description for TemplateConflictDialog.
    /// </summary>
    public class TemplateConflictDlg : System.Windows.Forms.Form
    {

        #region Private Data
        ConflictAction m_selectedAction;
        #endregion
        private System.Windows.Forms.Button m_btnSkip;
        private System.Windows.Forms.Button m_btnClone;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.CheckBox m_chkApplyToAll;
        private System.Windows.Forms.GroupBox m_gbMessage;
        private System.Windows.Forms.Label m_lblMessage;
        private System.Windows.Forms.Button m_btnReplace;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public TemplateConflictDlg()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_selectedAction = ConflictAction.Skip;

        }

        public enum ConflictAction
        {
            Skip,
            Clone,
            Replace,
            Cancel
        }

        public ConflictAction Action
        {
            get
            {
                return m_selectedAction;
            }
        }

        public string Message
        {
            get
            { return m_lblMessage.Text; }
            set
            { m_lblMessage.Text = value; }
        }

        public bool ApplyToAll
        {
            get
            { return m_chkApplyToAll.Checked; }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(TemplateConflictDlg));
            this.m_btnSkip = new System.Windows.Forms.Button();
            this.m_btnClone = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_chkApplyToAll = new System.Windows.Forms.CheckBox();
            this.m_gbMessage = new System.Windows.Forms.GroupBox();
            this.m_lblMessage = new System.Windows.Forms.Label();
            this.m_btnReplace = new System.Windows.Forms.Button();
            this.m_gbMessage.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_btnSkip
            // 
            this.m_btnSkip.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnSkip.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnSkip.Location = new System.Drawing.Point(16, 280);
            this.m_btnSkip.Name = "m_btnSkip";
            this.m_btnSkip.Size = new System.Drawing.Size(96, 23);
            this.m_btnSkip.TabIndex = 2;
            this.m_btnSkip.Text = "Skip Template";
            this.m_btnSkip.Click += new System.EventHandler(this.m_btnSkip_Click);
            // 
            // m_btnClone
            // 
            this.m_btnClone.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnClone.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnClone.Location = new System.Drawing.Point(236, 280);
            this.m_btnClone.Name = "m_btnClone";
            this.m_btnClone.Size = new System.Drawing.Size(96, 23);
            this.m_btnClone.TabIndex = 3;
            this.m_btnClone.Text = "Import As Clone";
            this.m_btnClone.Click += new System.EventHandler(this.m_btnClone_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(342, 280);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.Size = new System.Drawing.Size(96, 23);
            this.m_btnCancel.TabIndex = 4;
            this.m_btnCancel.Text = "Cancel Import";
            this.m_btnCancel.Click += new System.EventHandler(this.m_btnCancel_Click);
            // 
            // m_chkApplyToAll
            // 
            this.m_chkApplyToAll.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_chkApplyToAll.Location = new System.Drawing.Point(156, 248);
            this.m_chkApplyToAll.Name = "m_chkApplyToAll";
            this.m_chkApplyToAll.Size = new System.Drawing.Size(144, 24);
            this.m_chkApplyToAll.TabIndex = 1;
            this.m_chkApplyToAll.Text = "Apply to All Conflicts";
            // 
            // m_gbMessage
            // 
            this.m_gbMessage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_gbMessage.Controls.Add(this.m_lblMessage);
            this.m_gbMessage.Location = new System.Drawing.Point(8, 8);
            this.m_gbMessage.Name = "m_gbMessage";
            this.m_gbMessage.Size = new System.Drawing.Size(432, 232);
            this.m_gbMessage.TabIndex = 0;
            this.m_gbMessage.TabStop = false;
            this.m_gbMessage.Text = "Message";
            // 
            // m_lblMessage
            // 
            this.m_lblMessage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.m_lblMessage.Location = new System.Drawing.Point(3, 16);
            this.m_lblMessage.Name = "m_lblMessage";
            this.m_lblMessage.Size = new System.Drawing.Size(426, 213);
            this.m_lblMessage.TabIndex = 0;
            // 
            // m_btnReplace
            // 
            this.m_btnReplace.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnReplace.Location = new System.Drawing.Point(122, 280);
            this.m_btnReplace.Name = "m_btnReplace";
            this.m_btnReplace.Size = new System.Drawing.Size(104, 23);
            this.m_btnReplace.TabIndex = 5;
            this.m_btnReplace.Text = "Replace Exisiting";
            this.m_btnReplace.Click += new System.EventHandler(this.m_btnReplace_Click);
            // 
            // TemplateConflictDlg
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(448, 310);
            this.Controls.Add(this.m_btnReplace);
            this.Controls.Add(this.m_gbMessage);
            this.Controls.Add(this.m_chkApplyToAll);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnClone);
            this.Controls.Add(this.m_btnSkip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(344, 176);
            this.Name = "TemplateConflictDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Conflicting Template Found";
            this.m_gbMessage.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void m_btnSkip_Click(object sender, System.EventArgs e)
        {
            m_selectedAction = ConflictAction.Skip;
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void m_btnReplace_Click(object sender, System.EventArgs e)
        {
            m_selectedAction = ConflictAction.Replace;
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void m_btnClone_Click(object sender, System.EventArgs e)
        {
            m_selectedAction = ConflictAction.Clone;
            DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void m_btnCancel_Click(object sender, System.EventArgs e)
        {
            m_selectedAction = ConflictAction.Cancel;
        }



    }
}
