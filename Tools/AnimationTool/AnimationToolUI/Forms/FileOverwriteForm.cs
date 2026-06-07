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
using System.Reflection;
using System.IO;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for FileOverwriteForm.
    /// </summary>
    public class FileOverwriteForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.RichTextBox RichTextBox_Message;
        private System.Windows.Forms.Panel Panel_Bottom;
        private System.Windows.Forms.PictureBox PictureBox_Icon;
        private System.Windows.Forms.Button Button_SaveAs;
        private System.Windows.Forms.Button Button_Overwrite;
        private System.Windows.Forms.Button Button_Cancel;
        private System.Windows.Forms.ImageList ImageList_MessageBoxIcons;
        private System.ComponentModel.IContainer components;
        private string Message;

        public FileOverwriteForm(string strFile, string strAppTitle)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            Assembly kAssembly = Assembly.GetExecutingAssembly();
            string strResourceName = 
                "AnimationToolUI.Resources.MessageBoxes." +
                "FileOverwrite.rtf";

            Stream RTFStream = kAssembly.GetManifestResourceStream(
                strResourceName);
            Message = "";
            if (null != RTFStream )
            {                 
                StreamReader sr = new StreamReader(RTFStream);
                string strContents = sr.ReadToEnd();
                sr.Close();
                
                string strFind = "%0";
                string strReplace = strFile;
                strContents = strContents.Replace(strFind, strReplace);

                strFind = "%1";
                strReplace = strAppTitle;
                strContents = strContents.Replace(strFind, strReplace);

                Message = strContents;
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
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(FileOverwriteForm));
            this.RichTextBox_Message = new System.Windows.Forms.RichTextBox();
            this.Panel_Bottom = new System.Windows.Forms.Panel();
            this.Button_Cancel = new System.Windows.Forms.Button();
            this.Button_SaveAs = new System.Windows.Forms.Button();
            this.Button_Overwrite = new System.Windows.Forms.Button();
            this.PictureBox_Icon = new System.Windows.Forms.PictureBox();
            this.ImageList_MessageBoxIcons = new System.Windows.Forms.ImageList(this.components);
            this.Panel_Bottom.SuspendLayout();
            this.SuspendLayout();
            // 
            // RichTextBox_Message
            // 
            this.RichTextBox_Message.BackColor = System.Drawing.SystemColors.Control;
            this.RichTextBox_Message.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RichTextBox_Message.Location = new System.Drawing.Point(51, 15);
            this.RichTextBox_Message.Name = "RichTextBox_Message";
            this.RichTextBox_Message.ReadOnly = true;
            this.RichTextBox_Message.Size = new System.Drawing.Size(518, 183);
            this.RichTextBox_Message.TabIndex = 7;
            this.RichTextBox_Message.TabStop = false;
            this.RichTextBox_Message.Text = "RichTextBox_Message";
            // 
            // Panel_Bottom
            // 
            this.Panel_Bottom.Controls.Add(this.Button_Cancel);
            this.Panel_Bottom.Controls.Add(this.Button_SaveAs);
            this.Panel_Bottom.Controls.Add(this.Button_Overwrite);
            this.Panel_Bottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.Panel_Bottom.Location = new System.Drawing.Point(51, 198);
            this.Panel_Bottom.Name = "Panel_Bottom";
            this.Panel_Bottom.Size = new System.Drawing.Size(518, 48);
            this.Panel_Bottom.TabIndex = 5;
            // 
            // Button_Cancel
            // 
            this.Button_Cancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Button_Cancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_Cancel.Location = new System.Drawing.Point(363, 8);
            this.Button_Cancel.Name = "Button_Cancel";
            this.Button_Cancel.Size = new System.Drawing.Size(72, 24);
            this.Button_Cancel.TabIndex = 2;
            this.Button_Cancel.Text = "Cancel";
            this.Button_Cancel.Click += new System.EventHandler(this.Button_Cancel_Click);
            // 
            // Button_SaveAs
            // 
            this.Button_SaveAs.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_SaveAs.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_SaveAs.Location = new System.Drawing.Point(83, 8);
            this.Button_SaveAs.Name = "Button_SaveAs";
            this.Button_SaveAs.Size = new System.Drawing.Size(72, 24);
            this.Button_SaveAs.TabIndex = 0;
            this.Button_SaveAs.Text = "Save As...";
            this.Button_SaveAs.Click += new System.EventHandler(this.Button_SaveAs_Click);
            // 
            // Button_Overwrite
            // 
            this.Button_Overwrite.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_Overwrite.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_Overwrite.Location = new System.Drawing.Point(219, 8);
            this.Button_Overwrite.Name = "Button_Overwrite";
            this.Button_Overwrite.Size = new System.Drawing.Size(72, 24);
            this.Button_Overwrite.TabIndex = 1;
            this.Button_Overwrite.Text = "Overwrite";
            this.Button_Overwrite.Click += new System.EventHandler(this.Button_Overwrite_Click);
            // 
            // PictureBox_Icon
            // 
            this.PictureBox_Icon.Dock = System.Windows.Forms.DockStyle.Left;
            this.PictureBox_Icon.Location = new System.Drawing.Point(15, 15);
            this.PictureBox_Icon.Name = "PictureBox_Icon";
            this.PictureBox_Icon.Size = new System.Drawing.Size(36, 36);
            this.PictureBox_Icon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PictureBox_Icon.TabIndex = 6;
            this.PictureBox_Icon.TabStop = false;
            // 
            // ImageList_MessageBoxIcons
            // 
            this.ImageList_MessageBoxIcons.ImageSize = new System.Drawing.Size(36, 36);
            this.ImageList_MessageBoxIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ImageList_MessageBoxIcons.ImageStream")));
            this.ImageList_MessageBoxIcons.TransparentColor = System.Drawing.Color.Fuchsia;
            // 
            // FileOverwriteForm
            // 
            this.AcceptButton = this.Button_SaveAs;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(584, 246);
            this.ControlBox = false;
            this.Controls.Add(this.RichTextBox_Message);
            this.Controls.Add(this.Panel_Bottom);
            this.Controls.Add(this.PictureBox_Icon);
            this.DockPadding.Left = 15;
            this.DockPadding.Right = 15;
            this.DockPadding.Top = 15;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FileOverwriteForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Save of Read-Only File";
            this.Load += new System.EventHandler(this.FileOverwriteForm_Load);
            this.Panel_Bottom.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void FileOverwriteForm_Load(object sender, System.EventArgs e)
        {
            PictureBox_Icon.Image = ImageList_MessageBoxIcons.Images[3];
            RichTextBox_Message.Rtf = Message;
        }

        private void Button_SaveAs_Click(object sender, System.EventArgs e)
        {
            DialogResult = DialogResult.Yes;
        }

        private void Button_Overwrite_Click(object sender, System.EventArgs e)
        {
            DialogResult = DialogResult.No;
        }

        private void Button_Cancel_Click(object sender, System.EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
    }
}
