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
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    /// <summary>
    /// Summary description for AddPropertyDlg.
    /// </summary>
    public class AddPropertyDlg : System.Windows.Forms.Form
    {
        #region Private Data
        private IComponentService m_componentService;
        private IPropertyTypeService m_propertyTypeService;
        private PropertyType m_selectedPropertyType;
        private static PropertyType m_lastUsedPropertyType;
        private string[] m_bannedNames;
        private bool m_bUndoable = true;
        #endregion

        private System.Windows.Forms.Label m_lblName;
        private System.Windows.Forms.TextBox m_tbName;
        private System.Windows.Forms.Label m_lblType;
        private System.Windows.Forms.ComboBox m_cbType;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox m_tbDescription;
        private System.Windows.Forms.ErrorProvider m_errorProvider;
        private System.Windows.Forms.CheckBox m_chkMakeCollection;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;


        public AddPropertyDlg()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_bannedNames = new string[]{};

        }

        public string NewPropertyName
        {
            get 
            { return m_tbName.Text; }
            set
            { m_tbName.Text = value; }
        }

        public PropertyType NewPropertyType
        {
            get
            {
                return m_selectedPropertyType;
            }

            set
            {
                m_selectedPropertyType = value;
            }
        }
        
        public bool Collection
        {
            get
            {
                return m_chkMakeCollection.Checked;
            }
            set
            {
                m_chkMakeCollection.Checked = value;
            }
        }

        public string NewPropertyDescription
        {
            get
            {
                return m_tbDescription.Text;
            }
            set
            {
                m_tbDescription.Text = value;
            }
        }

        public string[] BannedNames
        {
            set
            {
                m_bannedNames = value;
            }
        }

        public bool Undoable
        {
            get
            {
                return m_bUndoable;
            }
            set
            {
                m_bUndoable = value;
            }
        }

        private IComponentService ComponentService
        {
            get
            {
                if (m_componentService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_componentService = 
                        sp.GetService(typeof(IComponentService))
                        as IComponentService;
                }
                return m_componentService;
            }
        }

        private IPropertyTypeService PropertyTypeService
        {
            get
            {
                if (m_propertyTypeService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_propertyTypeService = 
                        sp.GetService(typeof(IPropertyTypeService)) 
                        as IPropertyTypeService;
                }
                return m_propertyTypeService;
                
            }
        }

        private MFramework FW
        {
            get { return MFramework.Instance; }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(AddPropertyDlg));
            this.m_lblName = new System.Windows.Forms.Label();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.m_lblType = new System.Windows.Forms.Label();
            this.m_cbType = new System.Windows.Forms.ComboBox();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.m_tbDescription = new System.Windows.Forms.TextBox();
            this.m_errorProvider = new System.Windows.Forms.ErrorProvider();
            this.m_chkMakeCollection = new System.Windows.Forms.CheckBox();
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
            this.m_tbName.Size = new System.Drawing.Size(258, 20);
            this.m_tbName.TabIndex = 1;
            this.m_tbName.Text = "";
            this.m_tbName.Validating += new System.ComponentModel.CancelEventHandler(this.m_tbName_Validating);
            // 
            // m_lblType
            // 
            this.m_lblType.AutoSize = true;
            this.m_lblType.Location = new System.Drawing.Point(8, 56);
            this.m_lblType.Name = "m_lblType";
            this.m_lblType.Size = new System.Drawing.Size(33, 16);
            this.m_lblType.TabIndex = 2;
            this.m_lblType.Text = "Type:";
            // 
            // m_cbType
            // 
            this.m_cbType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbType.Location = new System.Drawing.Point(8, 72);
            this.m_cbType.Name = "m_cbType";
            this.m_cbType.Size = new System.Drawing.Size(258, 21);
            this.m_cbType.Sorted = true;
            this.m_cbType.TabIndex = 3;
            this.m_cbType.SelectedIndexChanged += new System.EventHandler(this.m_cbType_SelectedIndexChanged);
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(64, 328);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 6;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(152, 328);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 7;
            this.m_btnCancel.Text = "Cancel";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 136);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(64, 16);
            this.label1.TabIndex = 4;
            this.label1.Text = "Description:";
            // 
            // m_tbDescription
            // 
            this.m_tbDescription.AcceptsReturn = true;
            this.m_tbDescription.AcceptsTab = true;
            this.m_tbDescription.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbDescription.Location = new System.Drawing.Point(8, 160);
            this.m_tbDescription.Multiline = true;
            this.m_tbDescription.Name = "m_tbDescription";
            this.m_tbDescription.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.m_tbDescription.Size = new System.Drawing.Size(272, 160);
            this.m_tbDescription.TabIndex = 5;
            this.m_tbDescription.Text = "";
            // 
            // m_errorProvider
            // 
            this.m_errorProvider.ContainerControl = this;
            // 
            // m_chkMakeCollection
            // 
            this.m_chkMakeCollection.Location = new System.Drawing.Point(8, 104);
            this.m_chkMakeCollection.Name = "m_chkMakeCollection";
            this.m_chkMakeCollection.TabIndex = 8;
            this.m_chkMakeCollection.Text = "Make Collection";
            // 
            // AddPropertyDlg
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(290, 360);
            this.Controls.Add(this.m_chkMakeCollection);
            this.Controls.Add(this.m_tbDescription);
            this.Controls.Add(this.m_cbType);
            this.Controls.Add(this.m_tbName);
            this.Controls.Add(this.m_lblType);
            this.Controls.Add(this.m_lblName);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AddPropertyDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Property";
            this.Load += new System.EventHandler(this.AddPropertyDlg_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void AddPropertyDlg_Load(object sender, System.EventArgs e)
        {
            FillTypeDropDown();
        }

        #region Helper Methods

        private void FillTypeDropDown()
        {
            m_cbType.Items.Clear();
            PropertyType[] types = 
                PropertyTypeService.GetAllPropertyTypes();
            foreach (PropertyType type in types)
            {
                if (PropertyTypeService.IsTypeVisibleToUser(type))
                {
                    int index = m_cbType.Items.Add(type.Name);
                    if (type == m_selectedPropertyType)
                    {
                        m_cbType.SelectedIndex = index;
                    }
                }
            }
            if (m_cbType.SelectedIndex < 0)
            {
                if (m_lastUsedPropertyType != null)
                {
                    int lastIndex = 
                        m_cbType.Items.IndexOf(m_lastUsedPropertyType.Name);
                    m_cbType.SelectedIndex = lastIndex;
                }
                else
                {
                    m_cbType.SelectedIndex = 0;                    
                }
            }            
        }

        private string GetNameErrorText()
        {
            if (m_tbName.Text.Equals(string.Empty))
            {
                return "Please enter a name";
            }
            else
            {
                if (Array.IndexOf(m_bannedNames, m_tbName.Text) != -1)
                {
                    return "Name already exists";
                }
                else
                {
                    return string.Empty;
                }
            }
        }

        #endregion


        private void m_cbType_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            int index = m_cbType.SelectedIndex;
            PropertyType newSelectedType = 
                PropertyTypeService.LookupType(
                m_cbType.Items[index].ToString());
            m_selectedPropertyType = newSelectedType;        
        }
        private void m_tbName_Validating(object sender,
            System.ComponentModel.CancelEventArgs e)
        {
            m_errorProvider.SetError(sender as Control,
                GetNameErrorText());
        }

        private void m_btnOK_Click(object sender, System.EventArgs e)
        {
            if (!m_bUndoable && MessageBox.Show(
                "This operation is not undoable and will cause the current " +
                "undo stack to be cleared.\nAre you sure you want to " +
                "continue?", "Continue Non-Undoable Operation?",
                MessageBoxButtons.YesNo, MessageBoxIcon.Question) ==
                DialogResult.No)
            {
                return;
            }

            string errorText = GetNameErrorText();
            if (errorText.Equals(string.Empty))
            {
                this.DialogResult = DialogResult.OK;
                m_lastUsedPropertyType = m_selectedPropertyType;
            }
            else
            {
                MessageBox.Show(errorText, "InvalidInput", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }


    }
}
