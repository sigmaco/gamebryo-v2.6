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
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Controls;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    /// <summary>
    /// Summary description for PickPaletteDialog.
    /// </summary>
    public class PickPaletteDialog : System.Windows.Forms.Form
    {
        private PaletteTreeView m_tvPalettes;
        private System.Windows.Forms.Button m_btnOk;
        private System.Windows.Forms.Button m_btnCancel;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public PickPaletteDialog()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
        }

        public MPalette SelectedPalette
        {
            get
            { return m_tvPalettes.SelectedPalette; }
            set
            { m_tvPalettes.SelectedPalette = value; }        
        }

        public string SelectedCategory
        {
            get
            { return m_tvPalettes.SelectedCategory; }
            set
            { m_tvPalettes.SelectedCategory = value; }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(PickPaletteDialog));
            this.m_tvPalettes = new Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Controls.PaletteTreeView();
            this.m_btnOk = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_tvPalettes
            // 
            this.m_tvPalettes.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tvPalettes.DisplayTemplates = false;
            this.m_tvPalettes.Location = new System.Drawing.Point(8, 8);
            this.m_tvPalettes.Name = "m_tvPalettes";
            this.m_tvPalettes.Palettes = null;
            this.m_tvPalettes.SelectedCategory = null;
            this.m_tvPalettes.SelectedPalette = null;
            this.m_tvPalettes.Size = new System.Drawing.Size(436, 376);
            this.m_tvPalettes.TabIndex = 0;
            // 
            // m_btnOk
            // 
            this.m_btnOk.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOk.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOk.Location = new System.Drawing.Point(147, 392);
            this.m_btnOk.Name = "m_btnOk";
            this.m_btnOk.TabIndex = 1;
            this.m_btnOk.Text = "OK";
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(235, 392);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 2;
            this.m_btnCancel.Text = "Cancel";
            // 
            // PickPaletteDialog
            // 
            this.AcceptButton = this.m_btnOk;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(456, 422);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOk);
            this.Controls.Add(this.m_tvPalettes);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimizeBox = false;
            this.Name = "PickPaletteDialog";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Select Palette Location";
            this.Load += new System.EventHandler(this.PickPaletteDialog_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void PickPaletteDialog_Load(object sender, System.EventArgs e)
        {
            m_tvPalettes.Palettes = 
                MFramework.Instance.PaletteManager.GetPalettes();
//            m_tvPalettes.SelectedPalette = FW.PaletteManager.ActivePalette;
        }
    }
}
