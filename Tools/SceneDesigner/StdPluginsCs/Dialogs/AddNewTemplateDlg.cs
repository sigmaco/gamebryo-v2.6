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

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    /// <summary>
    /// Summary description for AddNewTemplateDlg.
    /// </summary>
    public class AddNewTemplateDlg : System.Windows.Forms.Form
    {

        #region Private Data
        MPalette m_selectedPalette;
        //The following are used to help validation
        string m_originalName;
        string m_originalCategory;
        MPalette m_originalPalette;
        #endregion
        private System.Windows.Forms.Label m_lblName;
        private System.Windows.Forms.TextBox m_tbName;
        private System.Windows.Forms.Label m_lblPalette;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Button m_btnBrowsePalettes;
        private System.Windows.Forms.ComboBox m_cbPalette;
        private System.Windows.Forms.TextBox m_tbCategory;
        private System.Windows.Forms.Label m_lblCategoryName;
        private System.Windows.Forms.ErrorProvider m_errorProvider;
        private System.Windows.Forms.CheckBox m_cbMakeClone;
        private System.Windows.Forms.Label m_lblDummy;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public AddNewTemplateDlg()
        {
            InitializeComponent();
            ShowCopyOption = false;
        }

        public string TemplateName
        {
            get
            { return m_tbName.Text; }
            set
            { m_tbName.Text = value; }
        }

        public bool TemplateNameEnabled
        {
            get
            {
                return m_tbName.Enabled;
            }
            set
            {
                m_tbName.Enabled = value;
            }
        }

        public MPalette SelectedPalette
        {
            get
            {
                return m_selectedPalette;
            }
            set
            {
                m_selectedPalette = value;
            }            
        }

        public string Category
        {
            get
            { return m_tbCategory.Text; }
            set
            { m_tbCategory.Text = value; }
        }

        public bool ShowCopyOption
        {
            get
            { return m_cbMakeClone.Enabled; }

            set
            {
                m_cbMakeClone.Enabled = value;
                m_cbMakeClone.Visible = value;
            }
        }

        public bool CopyOptionChecked
        {
            get
            { return m_cbMakeClone.Checked; }
            set
            { m_cbMakeClone.Checked = value; }
        }

        private MFramework FW
        {
            get
            { return MFramework.Instance; }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(AddNewTemplateDlg));
            this.m_lblName = new System.Windows.Forms.Label();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.m_lblPalette = new System.Windows.Forms.Label();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_btnBrowsePalettes = new System.Windows.Forms.Button();
            this.m_cbPalette = new System.Windows.Forms.ComboBox();
            this.m_tbCategory = new System.Windows.Forms.TextBox();
            this.m_lblCategoryName = new System.Windows.Forms.Label();
            this.m_errorProvider = new System.Windows.Forms.ErrorProvider();
            this.m_cbMakeClone = new System.Windows.Forms.CheckBox();
            this.m_lblDummy = new System.Windows.Forms.Label();
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
            this.m_tbName.Size = new System.Drawing.Size(398, 20);
            this.m_tbName.TabIndex = 1;
            this.m_tbName.Text = "";
            this.m_tbName.Validating += new System.ComponentModel.CancelEventHandler(this.m_tbName_Validating);
            // 
            // m_lblPalette
            // 
            this.m_lblPalette.AutoSize = true;
            this.m_lblPalette.Location = new System.Drawing.Point(8, 56);
            this.m_lblPalette.Name = "m_lblPalette";
            this.m_lblPalette.Size = new System.Drawing.Size(43, 16);
            this.m_lblPalette.TabIndex = 2;
            this.m_lblPalette.Text = "Palette:";
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(131, 112);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 8;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnCancel.CausesValidation = false;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(219, 112);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 9;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_btnBrowsePalettes
            // 
            this.m_btnBrowsePalettes.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_btnBrowsePalettes.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnBrowsePalettes.Location = new System.Drawing.Point(342, 72);
            this.m_btnBrowsePalettes.Name = "m_btnBrowsePalettes";
            this.m_btnBrowsePalettes.Size = new System.Drawing.Size(64, 24);
            this.m_btnBrowsePalettes.TabIndex = 6;
            this.m_btnBrowsePalettes.Text = "Browse...";
            this.m_btnBrowsePalettes.Click += new System.EventHandler(this.m_btnBrowsePalettes_Click);
            // 
            // m_cbPalette
            // 
            this.m_cbPalette.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbPalette.Location = new System.Drawing.Point(8, 72);
            this.m_cbPalette.Name = "m_cbPalette";
            this.m_cbPalette.Size = new System.Drawing.Size(136, 21);
            this.m_cbPalette.TabIndex = 3;
            this.m_cbPalette.SelectedIndexChanged += new System.EventHandler(this.m_cbPalette_SelectedIndexChanged);
            // 
            // m_tbCategory
            // 
            this.m_tbCategory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbCategory.Location = new System.Drawing.Point(160, 72);
            this.m_tbCategory.Name = "m_tbCategory";
            this.m_tbCategory.Size = new System.Drawing.Size(174, 20);
            this.m_tbCategory.TabIndex = 5;
            this.m_tbCategory.Text = "";
            this.m_tbCategory.Validating += new System.ComponentModel.CancelEventHandler(this.m_tbCategory_Validating);
            // 
            // m_lblCategoryName
            // 
            this.m_lblCategoryName.AutoSize = true;
            this.m_lblCategoryName.Location = new System.Drawing.Point(160, 56);
            this.m_lblCategoryName.Name = "m_lblCategoryName";
            this.m_lblCategoryName.Size = new System.Drawing.Size(87, 16);
            this.m_lblCategoryName.TabIndex = 4;
            this.m_lblCategoryName.Text = "Category Name:";
            // 
            // m_errorProvider
            // 
            this.m_errorProvider.ContainerControl = this;
            // 
            // m_cbMakeClone
            // 
            this.m_cbMakeClone.Location = new System.Drawing.Point(24, 104);
            this.m_cbMakeClone.Name = "m_cbMakeClone";
            this.m_cbMakeClone.Size = new System.Drawing.Size(88, 24);
            this.m_cbMakeClone.TabIndex = 7;
            this.m_cbMakeClone.Text = "Make Copy";
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.Location = new System.Drawing.Point(392, 120);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(8, 8);
            this.m_lblDummy.TabIndex = 10;
            // 
            // AddNewTemplateDlg
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(424, 146);
            this.Controls.Add(this.m_tbName);
            this.Controls.Add(this.m_tbCategory);
            this.Controls.Add(this.m_cbPalette);
            this.Controls.Add(this.m_lblDummy);
            this.Controls.Add(this.m_cbMakeClone);
            this.Controls.Add(this.m_lblPalette);
            this.Controls.Add(this.m_lblName);
            this.Controls.Add(this.m_btnBrowsePalettes);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_lblCategoryName);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(1024, 180);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(0, 180);
            this.Name = "AddNewTemplateDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add New Entity Template";
            this.Load += new System.EventHandler(this.AddNewTemplateDlg_Load);
            this.ResumeLayout(false);

        }
        #endregion

        #region WinForms Handlers

        private void m_btnBrowsePalettes_Click(object sender,
            System.EventArgs e)
        {
            PickPaletteDialog dlg = new PickPaletteDialog();
            dlg.SelectedPalette = m_selectedPalette;
            dlg.SelectedCategory = m_tbCategory.Text;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                m_selectedPalette = dlg.SelectedPalette;
                m_tbCategory.Text = dlg.SelectedCategory;
                SyncCombo();
                m_tbName_Validating(m_tbName, new CancelEventArgs());
            }
        }

        private void AddNewTemplateDlg_Load(object sender, System.EventArgs e)
        {
            m_originalPalette = m_selectedPalette;
            m_originalCategory = Category;
            m_originalName = TemplateName;
            foreach(MPalette palette in FW.PaletteManager.GetPalettes())
            {
                if (!palette.DontSave)
                {
                    String name = palette.Name;
                    if (!palette.Writable)
                    {
                        name += " (read-only)";
                    }                    
                    m_cbPalette.Items.Add(name);
                }
            }

            SyncCombo();

        }

        private void m_cbPalette_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            String paletteName = m_cbPalette.Items[m_cbPalette.SelectedIndex] 
                as string;
            if (paletteName.EndsWith(" (read-only)"))
            {
                paletteName = 
                    paletteName.Remove(paletteName.IndexOf(" (read-only)"));
            }
            foreach (MPalette palette in FW.PaletteManager.GetPalettes())
            {     
                if (palette.Name.Equals(paletteName))
                {
                    m_selectedPalette = palette;
                    break;
                }
            }            
        
        }

        private void m_tbName_Validating(object sender,
            System.ComponentModel.CancelEventArgs e)
        {
            Control control = sender as Control;
            m_errorProvider.SetError(control, 
                                     GetNameErrorString());
        }

        private void m_tbCategory_Validating(object sender,
            System.ComponentModel.CancelEventArgs e)
        {
            Control control = sender as Control;
            m_errorProvider.SetError(control, 
                GetCategoryErrorString());
        
        }

	    private void m_btnOK_Click(object sender, System.EventArgs e)
        {
            string errorString = GetNameErrorString();
            if (errorString.Equals(string.Empty))
            {
                errorString = GetCategoryErrorString();
                if (errorString.Equals(string.Empty))
                {
                    if (!SelectedPalette.Writable)
                    {
                        MessageBox.Show("Read-only palettes cannot be " +
                            "changed.\nPlease select another " +
                            "palette.", "Read-Only Palette");
                        return;
                    }
                    else if (!CopyOptionChecked && !m_originalPalette.Writable)
                    {
                        MessageBox.Show("Template cannot be removed from " +
                            "a read-only palette.", "Read-Only Palette");
                        return;
                    }
                    this.DialogResult = DialogResult.OK;
                }
                else
                {
                    MessageBox.Show(errorString, "Invalid Category");
                }
            }
            else
            {                
                MessageBox.Show(errorString, "Invalid Name");
            }
        }

        #endregion

        #region Helper Methods

        private void SyncCombo()
        {
            int index = 0;
            foreach(MPalette palette in FW.PaletteManager.GetPalettes())
            {                
                if (!palette.DontSave && palette == m_selectedPalette)
                {                    
                    m_cbPalette.SelectedIndex = index;
                    break;
                }
                if (!palette.DontSave)
                {
                    index++;
                }
            }            
        }

        private string GetNameErrorString()
        {
            //Control control = sender as Control;
            if (TemplateName.Equals(string.Empty))
            {
                return "You must provide a Template Name";
            } 
            else if (TemplateName.IndexOf(".") > -1)
            {
                return "Template names can not contain periods.";
            }
            else
            {
                bool perfectMatch = (
                    (m_originalName.Equals(TemplateName)) &&
                    (m_originalCategory.Equals(Category)) &&
                    (m_originalPalette == m_selectedPalette));
                
                MEntity existingEntity = 
                    m_selectedPalette.GetEntityByName(TemplateName);
                
                if ((!perfectMatch) && (existingEntity != null))
                {
                    bool categoryMatch = 
                        m_selectedPalette.GetEntityCategory(existingEntity).
                        Equals(Category);
                    bool paletteMatch =                         
                        m_selectedPalette == m_originalPalette;
                    if (categoryMatch && paletteMatch)
                    {
                        return "A Template already exists with this name";
                    }
                }
            }
            return "";           
        }

        private string GetCategoryErrorString()
        {
            if (Category.Length > 0)
            {
                string[] categories = Category.Split('.');
                foreach (string category in categories)
                {
                    if (category == string.Empty)
                    {
                        return "The category may not have consecutive \n" + 
                                " periods, and may not begin or end with \n" +
                                "a period";
                    }
                }
            }
            return string.Empty;
        }



        #endregion


    }
}
