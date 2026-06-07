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
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    /// <summary>
    /// Summary description for AddNewComponentDlg.
    /// </summary>
    public class AddNewComponentDlg : System.Windows.Forms.Form
    {
        #region PrivateData
        string[] m_blackListedNames;
        #endregion
        private System.Windows.Forms.Label m_lblName;
        private System.Windows.Forms.TextBox m_tbName;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.ErrorProvider m_errorProvider;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public AddNewComponentDlg(string[] blackListedNames)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_blackListedNames = blackListedNames;

        }

        public string NewComponentName
        {
            get
            {
                return m_tbName.Text;
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(AddNewComponentDlg));
            this.m_lblName = new System.Windows.Forms.Label();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_errorProvider = new System.Windows.Forms.ErrorProvider();
            this.SuspendLayout();
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
            // m_tbName
            // 
            this.m_tbName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbName.Location = new System.Drawing.Point(8, 24);
            this.m_tbName.Name = "m_tbName";
            this.m_tbName.Size = new System.Drawing.Size(248, 20);
            this.m_tbName.TabIndex = 1;
            this.m_tbName.Text = "";
            this.m_tbName.Validating += new System.ComponentModel.CancelEventHandler(this.m_tbName_Validating);
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(59, 56);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 2;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnCancel.CausesValidation = false;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(147, 56);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 3;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_errorProvider
            // 
            this.m_errorProvider.ContainerControl = this;
            // 
            // AddNewComponentDlg
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(280, 88);
            this.Controls.Add(this.m_tbName);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_lblName);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(184, 120);
            this.Name = "AddNewComponentDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Component";
            this.ResumeLayout(false);

        }
        #endregion

        #region Winforms Handlers

        private void m_tbName_Validating(object sender, 
            System.ComponentModel.CancelEventArgs e)
        {
            Control control = sender as Control;
            m_errorProvider.SetError(control, GetErrorText());
        }

        private void m_btnOK_Click(object sender, System.EventArgs e)
        {
            string errorText = GetErrorText();
            if (errorText.Equals(string.Empty))
            {
                DialogResult = DialogResult.OK;               
            }
            else
            {
                MessageBox.Show(errorText, "Invalid Input");
            }

        }

        #endregion

        #region Helper Methods
        private string GetErrorText()
        {
            string proposedName = NewComponentName;
            if (proposedName.Equals(string.Empty))
            {
                return "You must enter a name.";
            }
            else
            {
                if (Array.IndexOf(m_blackListedNames, proposedName) != -1)
                {
                    return "A Component with that name already exists.";
                }
                else
                {
                    return string.Empty;
                }
            }

        }
        #endregion


    }
}
