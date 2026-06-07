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

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for TransitionTableSettingsForm.
    /// </summary>
    public class TransitionTableSettingsForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel1;
        private System.Drawing.Color [] BackColors;
        private System.Drawing.Color [] TextColors;
        private System.Windows.Forms.ColorDialog cdCurrent;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.GroupBox gbSettings;
        private System.Windows.Forms.Panel pnlTextColor;
        private System.Windows.Forms.Panel pnlBackColor;
        private System.Windows.Forms.TextBox tbCurrentType;
        private System.Windows.Forms.Label lblBackColor;
        private System.Windows.Forms.Label lblTextColor;
        private System.Windows.Forms.Label lblCurrentType;
        private System.Windows.Forms.ListBox lbTransitionTypes;
        private System.Windows.Forms.ToolTip ttTransitionTableSettings;
        private System.Windows.Forms.Button btnResetDefaults;
        private System.ComponentModel.IContainer components;

        public TransitionTableSettingsForm()
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
            this.cdCurrent = new System.Windows.Forms.ColorDialog();
            this.panel2 = new System.Windows.Forms.Panel();
            this.btnResetDefaults = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.gbSettings = new System.Windows.Forms.GroupBox();
            this.pnlTextColor = new System.Windows.Forms.Panel();
            this.pnlBackColor = new System.Windows.Forms.Panel();
            this.tbCurrentType = new System.Windows.Forms.TextBox();
            this.lblBackColor = new System.Windows.Forms.Label();
            this.lblTextColor = new System.Windows.Forms.Label();
            this.lblCurrentType = new System.Windows.Forms.Label();
            this.lbTransitionTypes = new System.Windows.Forms.ListBox();
            this.ttTransitionTableSettings = new System.Windows.Forms.ToolTip(this.components);
            this.panel2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.gbSettings.SuspendLayout();
            this.SuspendLayout();
            // 
            // cdCurrent
            // 
            this.cdCurrent.AnyColor = true;
            this.cdCurrent.FullOpen = true;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.btnResetDefaults);
            this.panel2.Controls.Add(this.btnOK);
            this.panel2.Controls.Add(this.btnCancel);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel2.DockPadding.All = 5;
            this.panel2.Location = new System.Drawing.Point(0, 278);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(464, 48);
            this.panel2.TabIndex = 1;
            // 
            // btnResetDefaults
            // 
            this.btnResetDefaults.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnResetDefaults.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnResetDefaults.Location = new System.Drawing.Point(286, 8);
            this.btnResetDefaults.Name = "btnResetDefaults";
            this.btnResetDefaults.Size = new System.Drawing.Size(72, 32);
            this.btnResetDefaults.TabIndex = 2;
            this.btnResetDefaults.Text = "Reset";
            this.ttTransitionTableSettings.SetToolTip(this.btnResetDefaults, "Reset the Transition Table settings to the application defaults.");
            this.btnResetDefaults.Click += new System.EventHandler(this.btnResetDefaults_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(106, 8);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(72, 32);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(196, 8);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(72, 32);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.gbSettings);
            this.panel1.Controls.Add(this.lbTransitionTypes);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.DockPadding.All = 5;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(464, 278);
            this.panel1.TabIndex = 0;
            // 
            // gbSettings
            // 
            this.gbSettings.Controls.Add(this.pnlTextColor);
            this.gbSettings.Controls.Add(this.pnlBackColor);
            this.gbSettings.Controls.Add(this.tbCurrentType);
            this.gbSettings.Controls.Add(this.lblBackColor);
            this.gbSettings.Controls.Add(this.lblTextColor);
            this.gbSettings.Controls.Add(this.lblCurrentType);
            this.gbSettings.Location = new System.Drawing.Point(192, 16);
            this.gbSettings.Name = "gbSettings";
            this.gbSettings.Size = new System.Drawing.Size(264, 240);
            this.gbSettings.TabIndex = 1;
            this.gbSettings.TabStop = false;
            this.gbSettings.Text = "Selected Transition Settings:";
            // 
            // pnlTextColor
            // 
            this.pnlTextColor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlTextColor.BackColor = System.Drawing.SystemColors.Info;
            this.pnlTextColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlTextColor.Location = new System.Drawing.Point(136, 144);
            this.pnlTextColor.Name = "pnlTextColor";
            this.pnlTextColor.Size = new System.Drawing.Size(80, 20);
            this.pnlTextColor.TabIndex = 5;
            this.ttTransitionTableSettings.SetToolTip(this.pnlTextColor, "Click to select the cell text color.");
            this.pnlTextColor.DoubleClick += new System.EventHandler(this.pnlTextColor_DoubleClick);
            this.pnlTextColor.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pnlTextColor_MouseDown);
            // 
            // pnlBackColor
            // 
            this.pnlBackColor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlBackColor.BackColor = System.Drawing.SystemColors.Info;
            this.pnlBackColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlBackColor.Location = new System.Drawing.Point(136, 96);
            this.pnlBackColor.Name = "pnlBackColor";
            this.pnlBackColor.Size = new System.Drawing.Size(80, 20);
            this.pnlBackColor.TabIndex = 3;
            this.ttTransitionTableSettings.SetToolTip(this.pnlBackColor, "Click to select the cell background color.");
            this.pnlBackColor.DoubleClick += new System.EventHandler(this.pnlBackColor_DoubleClick);
            this.pnlBackColor.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pnlBackColor_MouseDown);
            // 
            // tbCurrentType
            // 
            this.tbCurrentType.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tbCurrentType.BackColor = System.Drawing.SystemColors.Control;
            this.tbCurrentType.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbCurrentType.Location = new System.Drawing.Point(136, 48);
            this.tbCurrentType.Name = "tbCurrentType";
            this.tbCurrentType.ReadOnly = true;
            this.tbCurrentType.Size = new System.Drawing.Size(80, 20);
            this.tbCurrentType.TabIndex = 1;
            this.tbCurrentType.TabStop = false;
            this.tbCurrentType.Text = "Immediate";
            this.tbCurrentType.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblBackColor
            // 
            this.lblBackColor.Location = new System.Drawing.Point(40, 96);
            this.lblBackColor.Name = "lblBackColor";
            this.lblBackColor.Size = new System.Drawing.Size(88, 24);
            this.lblBackColor.TabIndex = 2;
            this.lblBackColor.Text = "Cell Background Color:";
            // 
            // lblTextColor
            // 
            this.lblTextColor.Location = new System.Drawing.Point(40, 146);
            this.lblTextColor.Name = "lblTextColor";
            this.lblTextColor.Size = new System.Drawing.Size(88, 16);
            this.lblTextColor.TabIndex = 4;
            this.lblTextColor.Text = "Cell Text Color:";
            // 
            // lblCurrentType
            // 
            this.lblCurrentType.Location = new System.Drawing.Point(40, 50);
            this.lblCurrentType.Name = "lblCurrentType";
            this.lblCurrentType.Size = new System.Drawing.Size(56, 16);
            this.lblCurrentType.TabIndex = 0;
            this.lblCurrentType.Text = "Example:";
            // 
            // lbTransitionTypes
            // 
            this.lbTransitionTypes.Location = new System.Drawing.Point(24, 16);
            this.lbTransitionTypes.Name = "lbTransitionTypes";
            this.lbTransitionTypes.Size = new System.Drawing.Size(152, 238);
            this.lbTransitionTypes.TabIndex = 0;
            this.ttTransitionTableSettings.SetToolTip(this.lbTransitionTypes, "Select a transition type to display its color settings.");
            this.lbTransitionTypes.SelectedIndexChanged += new System.EventHandler(this.lbTransitionTypes_SelectedIndexChanged);
            // 
            // TransitionTableSettingsForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(464, 326);
            this.ControlBox = false;
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.panel2);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "TransitionTableSettingsForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Edit Transition Table Settings";
            this.Load += new System.EventHandler(this.TransitionTableSettingsForm_Load);
            this.panel2.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.gbSettings.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void SetExample()
        {
            int i = lbTransitionTypes.SelectedIndex;
            
            tbCurrentType.Text = 
                ProjectData.GetTypeNameForTransitionType(i);
            tbCurrentType.BackColor =
                BackColors[i];
            tbCurrentType.ForeColor =
                TextColors[i];
            
        }

        private void ChangeBackColor()
        {
            int i = lbTransitionTypes.SelectedIndex;
            cdCurrent.Color = pnlBackColor.BackColor;
            if (cdCurrent.ShowDialog(this) == DialogResult.OK)
            {
                pnlBackColor.BackColor = cdCurrent.Color;
                BackColors[i] = cdCurrent.Color;
            }
            SetExample();
        }

        private void pnlBackColor_DoubleClick(
            object sender, System.EventArgs e)
        {
            ChangeBackColor();
        }

        private void ChangeTextColor()
        {
            int i = lbTransitionTypes.SelectedIndex;
            cdCurrent.Color = pnlTextColor.BackColor;
            if (cdCurrent.ShowDialog(this) == DialogResult.OK)
            {
                pnlTextColor.BackColor = cdCurrent.Color;
                TextColors[i] = cdCurrent.Color;
            }
            SetExample();
        }

        private void pnlTextColor_DoubleClick(
            object sender, System.EventArgs e)
        {
            ChangeTextColor();
        }

        
        private void pnlBackColor_MouseDown(
            object sender, System.Windows.Forms.MouseEventArgs e)
        {
            ChangeBackColor();
        }

        private void pnlTextColor_MouseDown(
            object sender, System.Windows.Forms.MouseEventArgs e)
        {
            ChangeTextColor();
        }

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            int i = 0;
            foreach (System.Drawing.Color kColor in TextColors)
            {
                System.Drawing.Color kCurrentColor = 
                    ProjectData.GetTextColorForTransitionType(i);
                if (kColor != kCurrentColor)
                    ProjectData.SetTextColorForTransitionType(i, kColor);
                i++;
            }

            i = 0;
            foreach (System.Drawing.Color kColor in BackColors)
            {
                System.Drawing.Color kCurrentColor = 
                    ProjectData.GetColorForTransitionType(i);
                if (kColor != kCurrentColor)
                    ProjectData.SetColorForTransitionType(i, kColor);
                i++;
            }

            DialogResult = DialogResult.OK;
            ProjectData.Instance.SaveTransitionTableRegistrySettings();
        }

        private void TransitionTableSettingsForm_Load(
            object sender, System.EventArgs e)
        {
            lbTransitionTypes.Items.Clear();
            BackColors = 
                new System.Drawing.Color[ProjectData.GetNumTransitionTypes()];
            TextColors = 
                new System.Drawing.Color[ProjectData.GetNumTransitionTypes()];
            for (int i = 0; i < ProjectData.GetNumTransitionTypes(); i++)
            {
                lbTransitionTypes.Items.Add(
                    ProjectData.GetTypeNameForTransitionType(i));
                BackColors[i] = ProjectData.GetColorForTransitionType(i);
                TextColors[i] = ProjectData.GetTextColorForTransitionType(i);
            }

            lbTransitionTypes.SelectedIndex = 0;
        }

        private void lbTransitionTypes_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            int i = lbTransitionTypes.SelectedIndex;
            SetExample();
            pnlBackColor.BackColor =
                BackColors[i];
            pnlTextColor.BackColor =
                TextColors[i];
        }

        private void btnResetDefaults_Click(object sender, System.EventArgs e)
        {
            for (int i = 0; i < ProjectData.GetNumTransitionTypes(); i++)
            {
                BackColors[i] = 
                    ProjectData.GetAppDefaultColorForTransitionType(i);
                TextColors[i] = 
                    ProjectData.GetAppDefaultTextColorForTransitionType(i);
            }

            lbTransitionTypes_SelectedIndexChanged(this, null);
        }
    }
}
