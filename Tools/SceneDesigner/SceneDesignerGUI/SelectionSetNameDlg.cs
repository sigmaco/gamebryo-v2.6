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

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    /// <summary>
    /// Summary description for SelectionSetNameDlg.
    /// </summary>
    public class SelectionSetNameDlg : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Label m_lblName;
        private System.Windows.Forms.ErrorProvider m_errErrorProvider;
        private System.Windows.Forms.TextBox m_tbName;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public SelectionSetNameDlg()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
        }
        
        public string NewName
        {
            get
            {
                return m_tbName.Text;
            }

            set
            {
                m_tbName.Text = value;
            }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(SelectionSetNameDlg));
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_lblName = new System.Windows.Forms.Label();
            this.m_errErrorProvider = new System.Windows.Forms.ErrorProvider();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnCancel.CausesValidation = false;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(135, 56);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 3;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(47, 56);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 2;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // m_lblName
            // 
            this.m_lblName.AutoSize = true;
            this.m_lblName.Location = new System.Drawing.Point(8, 8);
            this.m_lblName.Name = "m_lblName";
            this.m_lblName.Size = new System.Drawing.Size(38, 16);
            this.m_lblName.TabIndex = 0;
            this.m_lblName.Text = "Name:";
            // 
            // m_errErrorProvider
            // 
            this.m_errErrorProvider.ContainerControl = this;
            // 
            // m_tbName
            // 
            this.m_tbName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbName.Location = new System.Drawing.Point(8, 24);
            this.m_tbName.Name = "m_tbName";
            this.m_tbName.Size = new System.Drawing.Size(224, 20);
            this.m_tbName.TabIndex = 1;
            this.m_tbName.Text = "";
            this.m_tbName.Validating += new System.ComponentModel.CancelEventHandler(this.m_tbName_Validating);
            // 
            // SelectionSetNameDlg
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(256, 86);
            this.Controls.Add(this.m_tbName);
            this.Controls.Add(this.m_lblName);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(0, 120);
            this.Name = "SelectionSetNameDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "SelectionSetNameDlg";
            this.ResumeLayout(false);

        }
        #endregion

        private string GetErrorStringForTextBox()
        {
            if (this.NewName.Equals(string.Empty))
            {
                return "Name cannot be empty; please enter a name.";
            }
            else
            {
                return string.Empty;
            }
        }

        private void m_tbName_Validating(object sender, 
            System.ComponentModel.CancelEventArgs e)
        {
            m_errErrorProvider.SetError(sender as Control,
                GetErrorStringForTextBox());
        }

        private void m_btnOK_Click(object sender, System.EventArgs e)
        {
            string strErrorString = GetErrorStringForTextBox();
            if (!strErrorString.Equals(string.Empty))
            {
                MessageBox.Show(this, strErrorString, "Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.DialogResult = DialogResult.None;
            }
        }
    }
}
