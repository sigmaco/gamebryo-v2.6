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

// This file has been auto generated

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    partial class DeleteLayerDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DeleteLayerDialog));
            System.Windows.Forms.Label label1;
            this.m_pbIcon = new System.Windows.Forms.PictureBox();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnContinue = new System.Windows.Forms.Button();
            this.cbDeleteFile = new System.Windows.Forms.CheckBox();
            this.lbDirtyText = new System.Windows.Forms.Label();
            label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.m_pbIcon)).BeginInit();
            this.SuspendLayout();
            // 
            // m_pbIcon
            // 
            this.m_pbIcon.Image = ((System.Drawing.Image)(resources.GetObject("m_pbIcon.Image")));
            this.m_pbIcon.Location = new System.Drawing.Point(12, 12);
            this.m_pbIcon.Name = "m_pbIcon";
            this.m_pbIcon.Size = new System.Drawing.Size(32, 32);
            this.m_pbIcon.TabIndex = 6;
            this.m_pbIcon.TabStop = false;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(50, 12);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(304, 13);
            label1.TabIndex = 16;
            label1.Text = "Deleting a layer is NOT undoable and will clear the undo stack.";
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(192, 84);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 14;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // btnContinue
            // 
            this.btnContinue.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btnContinue.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnContinue.Location = new System.Drawing.Point(111, 84);
            this.btnContinue.Name = "btnContinue";
            this.btnContinue.Size = new System.Drawing.Size(75, 23);
            this.btnContinue.TabIndex = 13;
            this.btnContinue.Text = "&Continue";
            this.btnContinue.UseVisualStyleBackColor = true;
            // 
            // cbDeleteFile
            // 
            this.cbDeleteFile.AutoSize = true;
            this.cbDeleteFile.Location = new System.Drawing.Point(134, 34);
            this.cbDeleteFile.Name = "cbDeleteFile";
            this.cbDeleteFile.Size = new System.Drawing.Size(118, 17);
            this.cbDeleteFile.TabIndex = 15;
            this.cbDeleteFile.Text = "Delete file from disk";
            this.cbDeleteFile.UseVisualStyleBackColor = true;
            this.cbDeleteFile.CheckedChanged += new System.EventHandler(this.cbDeleteFile_CheckedChanged);
            // 
            // lbDirtyText
            // 
            this.lbDirtyText.AutoSize = true;
            this.lbDirtyText.Location = new System.Drawing.Point(50, 61);
            this.lbDirtyText.Name = "lbDirtyText";
            this.lbDirtyText.Size = new System.Drawing.Size(175, 13);
            this.lbDirtyText.TabIndex = 17;
            this.lbDirtyText.Text = "<This space intentionally left blank>";
            // 
            // DeleteLayerDialog
            // 
            this.AcceptButton = this.btnContinue;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(379, 119);
            this.Controls.Add(this.lbDirtyText);
            this.Controls.Add(label1);
            this.Controls.Add(this.cbDeleteFile);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnContinue);
            this.Controls.Add(this.m_pbIcon);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DeleteLayerDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Delete Layer?";
            this.Load += new System.EventHandler(this.DeleteLayerDialog_Load);
            ((System.ComponentModel.ISupportInitialize)(this.m_pbIcon)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnContinue;
        private System.Windows.Forms.CheckBox cbDeleteFile;
        private System.Windows.Forms.Label lbDirtyText;
        private System.Windows.Forms.PictureBox m_pbIcon;

    }
}
//END auto generated code
