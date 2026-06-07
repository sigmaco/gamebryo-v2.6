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

using WeifenLuo.WinFormsUI;
using SourceGrid2;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for TransitionsViewForm.
    /// </summary>
    public class TransitionsViewForm : DockContent
    {
        private WorkSheet WorkSheet_Trans;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public TransitionsViewForm()
        {
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
            this.WorkSheet_Trans = WorkSheet.Create();
            this.SuspendLayout();
            // 
            // WorkSheet_Trans
            // 
            this.WorkSheet_Trans.Dock = System.Windows.Forms.DockStyle.Fill;
            this.WorkSheet_Trans.Location = new System.Drawing.Point(0, 0);
            this.WorkSheet_Trans.Name = "WorkSheet_Trans";
            this.WorkSheet_Trans.Size = new System.Drawing.Size(432, 310);
            this.WorkSheet_Trans.TabIndex = 0;
            this.WorkSheet_Trans.Load += new System.EventHandler(this.TransitionsViewForm_Load);
            // 
            // TransitionsViewForm
            // 
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(TransitionsViewForm));
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(432, 310);
            this.Controls.Add(this.WorkSheet_Trans);
            this.HideOnClose = true;
            this.Name = "TransitionsViewForm";
            this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockLeft;
            this.Text = "TransitionsViewForm";
            this.Load += new System.EventHandler(this.TransitionsViewForm_Load);
            this.ResumeLayout(false);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));

        }
        #endregion

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
        }

        private void TransitionsViewForm_Load(
            object sender, System.EventArgs e)
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);

            // Register callback handlers
            AppEvents.FrameworkDataChanged +=
                new AnimationToolUI.
                AppEvents.EventHandler_FrameworkDataChanged(
                OnFrameworkDataChanged);
        }

        public void OnFrameworkDataChanged()
        {
            
        }

        private void label1_Click(object sender, System.EventArgs e)
        {    
        }
    }
}
