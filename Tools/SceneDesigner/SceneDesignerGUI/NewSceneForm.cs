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
using System.IO;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.Utility;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    /// <summary>
    /// Summary description for NewSceneForm.
    /// </summary>
    public class NewSceneForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.TextBox m_tbCurrentPath;
        private System.Windows.Forms.TextBox m_tbDefaultPath;
        private System.Windows.Forms.TextBox m_tbCustomPath;
        private System.Windows.Forms.Button m_btnBrowse;
        private System.Windows.Forms.RadioButton m_rbCustomFolder;
        private System.Windows.Forms.RadioButton m_rbApplicationDefault;
        private System.Windows.Forms.RadioButton m_rbCurrentFolder;
        private System.Windows.Forms.ToolTip m_ttToolTip;
        private System.Windows.Forms.GroupBox m_gbPaletteFolderLocation;
        private System.Windows.Forms.Label m_lblInstructions1;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Label m_lblInstructions2;
        private System.ComponentModel.IContainer components;

        public NewSceneForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
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
            this.m_gbPaletteFolderLocation = new System.Windows.Forms.GroupBox();
            this.m_lblInstructions1 = new System.Windows.Forms.Label();
            this.m_lblInstructions2 = new System.Windows.Forms.Label();
            this.m_btnBrowse = new System.Windows.Forms.Button();
            this.m_tbCustomPath = new System.Windows.Forms.TextBox();
            this.m_tbDefaultPath = new System.Windows.Forms.TextBox();
            this.m_tbCurrentPath = new System.Windows.Forms.TextBox();
            this.m_rbCustomFolder = new System.Windows.Forms.RadioButton();
            this.m_rbApplicationDefault = new System.Windows.Forms.RadioButton();
            this.m_rbCurrentFolder = new System.Windows.Forms.RadioButton();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_ttToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.m_gbPaletteFolderLocation.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_gbPaletteFolderLocation
            // 
            this.m_gbPaletteFolderLocation.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_lblInstructions1);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_lblInstructions2);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_btnBrowse);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_tbCustomPath);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_tbDefaultPath);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_tbCurrentPath);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_rbCustomFolder);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_rbApplicationDefault);
            this.m_gbPaletteFolderLocation.Controls.Add(this.m_rbCurrentFolder);
            this.m_gbPaletteFolderLocation.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_gbPaletteFolderLocation.Location = new System.Drawing.Point(8, 8);
            this.m_gbPaletteFolderLocation.Name = "m_gbPaletteFolderLocation";
            this.m_gbPaletteFolderLocation.Size = new System.Drawing.Size(408, 176);
            this.m_gbPaletteFolderLocation.TabIndex = 0;
            this.m_gbPaletteFolderLocation.TabStop = false;
            this.m_gbPaletteFolderLocation.Text = "Palette Folder Location";
            // 
            // m_lblInstructions1
            // 
            this.m_lblInstructions1.AutoSize = true;
            this.m_lblInstructions1.Location = new System.Drawing.Point(8, 24);
            this.m_lblInstructions1.Name = "m_lblInstructions1";
            this.m_lblInstructions1.Size = new System.Drawing.Size(373, 13);
            this.m_lblInstructions1.TabIndex = 0;
            this.m_lblInstructions1.Text = "Select the palette folder location that you would like to use for the new scene:";
            // 
            // m_lblInstructions2
            // 
            this.m_lblInstructions2.AutoSize = true;
            this.m_lblInstructions2.Location = new System.Drawing.Point(8, 40);
            this.m_lblInstructions2.Name = "m_lblInstructions2";
            this.m_lblInstructions2.Size = new System.Drawing.Size(299, 13);
            this.m_lblInstructions2.TabIndex = 8;
            this.m_lblInstructions2.Text = "(This location can be changed later in the Options dialog box.)";
            // 
            // m_btnBrowse
            // 
            this.m_btnBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_btnBrowse.Location = new System.Drawing.Point(336, 145);
            this.m_btnBrowse.Name = "m_btnBrowse";
            this.m_btnBrowse.Size = new System.Drawing.Size(64, 23);
            this.m_btnBrowse.TabIndex = 7;
            this.m_btnBrowse.Text = "Browse...";
            this.m_btnBrowse.Click += new System.EventHandler(this.m_btnBrowse_Click);
            // 
            // m_tbCustomPath
            // 
            this.m_tbCustomPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbCustomPath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_tbCustomPath.Location = new System.Drawing.Point(80, 146);
            this.m_tbCustomPath.Name = "m_tbCustomPath";
            this.m_tbCustomPath.ReadOnly = true;
            this.m_tbCustomPath.Size = new System.Drawing.Size(248, 20);
            this.m_tbCustomPath.TabIndex = 6;
            this.m_tbCustomPath.TextChanged += new System.EventHandler(this.m_tbCustomPath_TextChanged);
            // 
            // m_tbDefaultPath
            // 
            this.m_tbDefaultPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbDefaultPath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_tbDefaultPath.Location = new System.Drawing.Point(80, 106);
            this.m_tbDefaultPath.Name = "m_tbDefaultPath";
            this.m_tbDefaultPath.ReadOnly = true;
            this.m_tbDefaultPath.Size = new System.Drawing.Size(320, 20);
            this.m_tbDefaultPath.TabIndex = 5;
            this.m_tbDefaultPath.TextChanged += new System.EventHandler(this.m_tbDefaultPath_TextChanged);
            // 
            // m_tbCurrentPath
            // 
            this.m_tbCurrentPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbCurrentPath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.m_tbCurrentPath.Location = new System.Drawing.Point(80, 66);
            this.m_tbCurrentPath.Name = "m_tbCurrentPath";
            this.m_tbCurrentPath.ReadOnly = true;
            this.m_tbCurrentPath.Size = new System.Drawing.Size(320, 20);
            this.m_tbCurrentPath.TabIndex = 4;
            this.m_tbCurrentPath.TextChanged += new System.EventHandler(this.m_tbCurrentPath_TextChanged);
            // 
            // m_rbCustomFolder
            // 
            this.m_rbCustomFolder.Location = new System.Drawing.Point(8, 144);
            this.m_rbCustomFolder.Name = "m_rbCustomFolder";
            this.m_rbCustomFolder.Size = new System.Drawing.Size(64, 24);
            this.m_rbCustomFolder.TabIndex = 3;
            this.m_rbCustomFolder.Text = "Custom:";
            this.m_rbCustomFolder.CheckedChanged += new System.EventHandler(this.m_rbCustomFolder_CheckedChanged);
            // 
            // m_rbApplicationDefault
            // 
            this.m_rbApplicationDefault.Location = new System.Drawing.Point(8, 104);
            this.m_rbApplicationDefault.Name = "m_rbApplicationDefault";
            this.m_rbApplicationDefault.Size = new System.Drawing.Size(64, 24);
            this.m_rbApplicationDefault.TabIndex = 2;
            this.m_rbApplicationDefault.Text = "Default:";
            // 
            // m_rbCurrentFolder
            // 
            this.m_rbCurrentFolder.Location = new System.Drawing.Point(8, 64);
            this.m_rbCurrentFolder.Name = "m_rbCurrentFolder";
            this.m_rbCurrentFolder.Size = new System.Drawing.Size(64, 24);
            this.m_rbCurrentFolder.TabIndex = 1;
            this.m_rbCurrentFolder.Text = "Current:";
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(172, 192);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.Size = new System.Drawing.Size(80, 23);
            this.m_btnOK.TabIndex = 1;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // NewSceneForm
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(424, 222);
            this.ControlBox = false;
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_gbPaletteFolderLocation);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(1280, 256);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(432, 256);
            this.Name = "NewSceneForm";
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Create New Scene";
            this.Load += new System.EventHandler(this.NewSceneForm_Load);
            this.m_gbPaletteFolderLocation.ResumeLayout(false);
            this.m_gbPaletteFolderLocation.PerformLayout();
            this.ResumeLayout(false);

        }
        #endregion

        public string PalettePath
        {
            get
            {
                if (m_rbCurrentFolder.Checked)
                {
                    return null;
                }
                else if (m_rbCustomFolder.Checked)
                {
                    return m_tbCustomPath.Text;
                }
                else
                {
                    return string.Empty;
                }
            }
        }

        private static string ms_strCustomDefault = "<Choose Custom Folder>";

        private void NewSceneForm_Load(object sender, System.EventArgs e)
        {
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService)) 
                as ISettingsService;

            //Set text

            m_tbCurrentPath.Text = 
                SceneManagementUtilities.CalculatePaletteFolder();
            m_rbCurrentFolder.Checked = true;
            m_tbDefaultPath.Text = Path.GetFullPath(
                MFramework.Instance.AppStartupPath +
                "..\\..\\Data\\Palettes\\");
            m_tbCustomPath.Text = ms_strCustomDefault;
        }

        private void m_btnBrowse_Click(object sender, System.EventArgs e)
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.SelectedPath = m_tbCustomPath.Text;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                m_tbCustomPath.Text = dlg.SelectedPath;
                m_rbCustomFolder.Checked = true;
            }
        }

        private void m_tbCurrentPath_TextChanged(object sender, 
            System.EventArgs e)
        {
            m_ttToolTip.SetToolTip(m_tbCurrentPath, m_tbCurrentPath.Text);
        }

        private void m_tbDefaultPath_TextChanged(object sender, 
            System.EventArgs e)
        {
            m_ttToolTip.SetToolTip(m_tbDefaultPath, m_tbDefaultPath.Text);
        }

        private void m_tbCustomPath_TextChanged(object sender, 
            System.EventArgs e)
        {
            m_ttToolTip.SetToolTip(m_tbCustomPath, m_tbCustomPath.Text);
        }

        private void m_rbCustomFolder_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (m_tbCustomPath.Text == ms_strCustomDefault)
            {
                m_tbCustomPath.Text = string.Empty;
            }
        }

        private void m_btnOK_Click(object sender, EventArgs e)
        {
            if (m_rbCustomFolder.Checked &&
                m_tbCustomPath.Text == string.Empty)
            {
                MessageBox.Show("The specified custom path is empty. The " +
                    "default path will be used instead.", "Empty Custom Path",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}
