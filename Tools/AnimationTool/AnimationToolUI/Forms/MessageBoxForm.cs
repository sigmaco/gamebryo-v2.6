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
    /// Summary description for MessageBoxForm.
    /// </summary>
    public class MessageBoxForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.CheckBox CheckBox_DoNotShowAgain;
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.Panel Panel_Bottom;
        private bool m_bDoNotShowAgain;
        private System.Windows.Forms.Button Button_2A;
        private System.Windows.Forms.Button Button_2B;
        private System.Windows.Forms.Button Button_1A;
        private System.Windows.Forms.Button Button_3B;
        private System.Windows.Forms.Button Button_3A;
        private System.Windows.Forms.Button Button_3C;
        private System.Windows.Forms.RichTextBox RichTextBox_Message;
        private System.Windows.Forms.ImageList ImageList_MessageBoxIcons;
        private System.Windows.Forms.PictureBox PictureBox_Icon;

        public string MessageText
        {
            get {return RichTextBox_Message.Rtf;}
            set {RichTextBox_Message.Rtf = value;}
        }

        public bool DoNotShowAgain
        {
            get {return m_bDoNotShowAgain;}
        }

        public string Title
        {
            get {return Text;}
            set {Text = value;}
        }

        public MessageBoxButtons MessageButtons
        {
            get {return m_kButton;}
            set {SetButtons(value);}
        }

        public MessageBoxIcon MessageIcon
        {
            get {return m_kIcon;}
            set {SetIcon(value);}
        }

        private MessageBoxButtons m_kButton;
        private MessageBoxIcon m_kIcon;

        public MessageBoxForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_kButton = MessageBoxButtons.OKCancel;
            m_kIcon = MessageBoxIcon.None;

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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MessageBoxForm));
            this.Button_2A = new System.Windows.Forms.Button();
            this.Button_2B = new System.Windows.Forms.Button();
            this.Panel_Bottom = new System.Windows.Forms.Panel();
            this.Button_3C = new System.Windows.Forms.Button();
            this.Button_3A = new System.Windows.Forms.Button();
            this.Button_3B = new System.Windows.Forms.Button();
            this.Button_1A = new System.Windows.Forms.Button();
            this.CheckBox_DoNotShowAgain = new System.Windows.Forms.CheckBox();
            this.PictureBox_Icon = new System.Windows.Forms.PictureBox();
            this.RichTextBox_Message = new System.Windows.Forms.RichTextBox();
            this.ImageList_MessageBoxIcons = new System.Windows.Forms.ImageList(this.components);
            this.Panel_Bottom.SuspendLayout();
            this.SuspendLayout();
            // 
            // Button_2A
            // 
            this.Button_2A.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_2A.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_2A.Location = new System.Drawing.Point(215, 32);
            this.Button_2A.Name = "Button_2A";
            this.Button_2A.Size = new System.Drawing.Size(52, 32);
            this.Button_2A.TabIndex = 1;
            this.Button_2A.Text = "OK";
            this.Button_2A.Visible = false;
            this.Button_2A.Click += new System.EventHandler(this.Button_2A_Click);
            // 
            // Button_2B
            // 
            this.Button_2B.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_2B.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Button_2B.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_2B.Location = new System.Drawing.Point(295, 32);
            this.Button_2B.Name = "Button_2B";
            this.Button_2B.Size = new System.Drawing.Size(52, 32);
            this.Button_2B.TabIndex = 5;
            this.Button_2B.Text = "Cancel";
            this.Button_2B.Visible = false;
            this.Button_2B.Click += new System.EventHandler(this.Button_2B_Click);
            // 
            // Panel_Bottom
            // 
            this.Panel_Bottom.Controls.Add(this.Button_3C);
            this.Panel_Bottom.Controls.Add(this.Button_3A);
            this.Panel_Bottom.Controls.Add(this.Button_3B);
            this.Panel_Bottom.Controls.Add(this.Button_1A);
            this.Panel_Bottom.Controls.Add(this.CheckBox_DoNotShowAgain);
            this.Panel_Bottom.Controls.Add(this.Button_2B);
            this.Panel_Bottom.Controls.Add(this.Button_2A);
            this.Panel_Bottom.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.Panel_Bottom.Location = new System.Drawing.Point(15, 344);
            this.Panel_Bottom.Name = "Panel_Bottom";
            this.Panel_Bottom.Size = new System.Drawing.Size(562, 72);
            this.Panel_Bottom.TabIndex = 2;
            this.Panel_Bottom.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // Button_3C
            // 
            this.Button_3C.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_3C.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_3C.Location = new System.Drawing.Point(331, 32);
            this.Button_3C.Name = "Button_3C";
            this.Button_3C.Size = new System.Drawing.Size(52, 32);
            this.Button_3C.TabIndex = 6;
            this.Button_3C.Text = "OK";
            this.Button_3C.Visible = false;
            this.Button_3C.Click += new System.EventHandler(this.Button_3C_Click);
            // 
            // Button_3A
            // 
            this.Button_3A.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_3A.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_3A.Location = new System.Drawing.Point(179, 32);
            this.Button_3A.Name = "Button_3A";
            this.Button_3A.Size = new System.Drawing.Size(52, 32);
            this.Button_3A.TabIndex = 0;
            this.Button_3A.Text = "OK";
            this.Button_3A.Visible = false;
            this.Button_3A.Click += new System.EventHandler(this.Button_3A_Click);
            // 
            // Button_3B
            // 
            this.Button_3B.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_3B.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_3B.Location = new System.Drawing.Point(255, 32);
            this.Button_3B.Name = "Button_3B";
            this.Button_3B.Size = new System.Drawing.Size(52, 32);
            this.Button_3B.TabIndex = 4;
            this.Button_3B.Text = "OK";
            this.Button_3B.Visible = false;
            this.Button_3B.Click += new System.EventHandler(this.Button_3B_Click);
            // 
            // Button_1A
            // 
            this.Button_1A.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Button_1A.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.Button_1A.Location = new System.Drawing.Point(255, 32);
            this.Button_1A.Name = "Button_1A";
            this.Button_1A.Size = new System.Drawing.Size(52, 32);
            this.Button_1A.TabIndex = 3;
            this.Button_1A.Text = "OK";
            this.Button_1A.Visible = false;
            this.Button_1A.Click += new System.EventHandler(this.Button_1A_Click);
            // 
            // CheckBox_DoNotShowAgain
            // 
            this.CheckBox_DoNotShowAgain.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.CheckBox_DoNotShowAgain.Location = new System.Drawing.Point(176, 8);
            this.CheckBox_DoNotShowAgain.Name = "CheckBox_DoNotShowAgain";
            this.CheckBox_DoNotShowAgain.Size = new System.Drawing.Size(211, 16);
            this.CheckBox_DoNotShowAgain.TabIndex = 6;
            this.CheckBox_DoNotShowAgain.Text = "Do not show me this message again";
            this.CheckBox_DoNotShowAgain.CheckedChanged += new System.EventHandler(this.CheckBox_DoNotShowAgain_CheckedChanged);
            // 
            // PictureBox_Icon
            // 
            this.PictureBox_Icon.Dock = System.Windows.Forms.DockStyle.Left;
            this.PictureBox_Icon.Location = new System.Drawing.Point(15, 15);
            this.PictureBox_Icon.Name = "PictureBox_Icon";
            this.PictureBox_Icon.Size = new System.Drawing.Size(50, 207);
            this.PictureBox_Icon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PictureBox_Icon.TabIndex = 3;
            this.PictureBox_Icon.TabStop = false;
            // 
            // RichTextBox_Message
            // 
            this.RichTextBox_Message.BackColor = System.Drawing.SystemColors.Control;
            this.RichTextBox_Message.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RichTextBox_Message.Location = new System.Drawing.Point(65, 15);
            this.RichTextBox_Message.Name = "RichTextBox_Message";
            this.RichTextBox_Message.ReadOnly = true;
            this.RichTextBox_Message.Size = new System.Drawing.Size(512, 329);
            this.RichTextBox_Message.TabIndex = 4;
            this.RichTextBox_Message.Text = "RichTextBox_Message";
            // 
            // ImageList_MessageBoxIcons
            // 
            this.ImageList_MessageBoxIcons.ImageSize = new System.Drawing.Size(36, 36);
            this.ImageList_MessageBoxIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ImageList_MessageBoxIcons.ImageStream")));
            this.ImageList_MessageBoxIcons.TransparentColor = System.Drawing.Color.Fuchsia;
            // 
            // MessageBoxForm
            // 
            this.AcceptButton = this.Button_2B;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.Button_2B;
            this.ClientSize = new System.Drawing.Size(592, 416);
            this.ControlBox = false;
            this.Controls.Add(this.RichTextBox_Message);
            this.Controls.Add(this.PictureBox_Icon);
            this.Controls.Add(this.Panel_Bottom);
            this.DockPadding.Left = 15;
            this.DockPadding.Right = 15;
            this.DockPadding.Top = 15;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(275, 175);
            this.Name = "MessageBoxForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "MessageBoxForm";
            this.Load += new System.EventHandler(this.MessageBoxForm_Load);
            this.Panel_Bottom.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        void HideAllButtons()
        {
            Button_1A.Visible = false;
            Button_2A.Visible = false;
            Button_2B.Visible = false;
            Button_3A.Visible = false;
            Button_3B.Visible = false;
            Button_3C.Visible = false;
        }


        void Show1Button(string ATitle)
        {
            Button_1A.Visible = true;
            Button_1A.Text = ATitle;
        }

        void Show2Buttons(string ATitle, string BTitle)
        {
            Button_2A.Visible = true;
            Button_2B.Visible = true;
            
            Button_2A.Text = ATitle;
            Button_2B.Text = BTitle;
        }

        void Show3Buttons(string ATitle, string BTitle, string CTitle)
        {
            Button_3A.Visible = true;
            Button_3B.Visible = true;
            Button_3C.Visible = true;

            Button_3A.Text = ATitle;
            Button_3B.Text = BTitle;
            Button_3C.Text = CTitle;
        }
        

        void SetButtons(MessageBoxButtons eButton)
        {
            HideAllButtons();
            switch (eButton)
            {
                case MessageBoxButtons.AbortRetryIgnore:
                    Show3Buttons("Abort", "Retry", "Ignore");
                    AcceptButton = Button_3B;
                    CancelButton = Button_3C;
                    break;
                case MessageBoxButtons.OK:
                    Show1Button("OK");
                    AcceptButton = Button_1A;
                    break;
                case MessageBoxButtons.OKCancel: 
                    Show2Buttons("OK", "Cancel");
                    AcceptButton = Button_2A;
                    CancelButton = Button_2B;
                    break;
                case MessageBoxButtons.RetryCancel: 
                    Show2Buttons("Retry", "Cancel");
                    AcceptButton = Button_2A;
                    CancelButton = Button_2B;
                    break;
                case MessageBoxButtons.YesNo:
                    Show2Buttons("Yes", "No");
                    AcceptButton = Button_2A;
                    CancelButton = Button_2B;
                    break;
                case MessageBoxButtons.YesNoCancel:
                    Show3Buttons("Yes", "No", "Cancel");
                    AcceptButton = Button_3A;
                    CancelButton = Button_3C;
                    break;
            }
            m_kButton = eButton;
        }

        void SetIcon(MessageBoxIcon eIcon)
        {
            switch (eIcon)
            {
                case MessageBoxIcon.Asterisk:
                    PictureBox_Icon.Image = 
                        ImageList_MessageBoxIcons.Images[0];
                    break;
                case MessageBoxIcon.Error:
                    PictureBox_Icon.Image = 
                        ImageList_MessageBoxIcons.Images[1];
                    break;
                case MessageBoxIcon.Question:
                    PictureBox_Icon.Image = 
                        ImageList_MessageBoxIcons.Images[2];
                    break;
                case MessageBoxIcon.Exclamation: 
                    PictureBox_Icon.Image = 
                        ImageList_MessageBoxIcons.Images[3];
                    break;
                default:
                    eIcon = MessageBoxIcon.None;
                    break;
            }

            if (eIcon == MessageBoxIcon.None)
                PictureBox_Icon.Visible = false;
            else 
                PictureBox_Icon.Visible = true;
            m_kIcon = eIcon;
        }

        private void SetDialogResult(string strText)
        {
            switch (strText)
            {
                case "Abort":
                    DialogResult = DialogResult.Abort;
                    break;
                case "Retry":
                    DialogResult = DialogResult.Retry;
                    break;
                case "Ignore":
                    DialogResult = DialogResult.Ignore;
                    break;
                case "Yes":
                    DialogResult = DialogResult.Yes;
                    break;
                case "No":
                    DialogResult = DialogResult.No;
                    break;
                case "Cancel":
                    DialogResult = DialogResult.Cancel;
                    break;
                case "OK":
                    DialogResult = DialogResult.OK;
                    break;
                default:
                    DialogResult = DialogResult.Cancel;
                    break;
            }
        }

        private void panel1_Paint(
            object sender, System.Windows.Forms.PaintEventArgs e)
        {
        
        }

        private void CheckBox_DoNotShowAgain_CheckedChanged(
            object sender, System.EventArgs e)
        {
            m_bDoNotShowAgain = CheckBox_DoNotShowAgain.Checked;
        }

        private void RichTextBox_Message_LinkClicked(
            object sender, System.Windows.Forms.LinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start(e.LinkText);
        }


        private void Button_1A_Click(object sender, System.EventArgs e)
        {
            SetDialogResult(Button_1A.Text);
        }
        
        private void Button_2A_Click(object sender, System.EventArgs e)
        {
            SetDialogResult(Button_2A.Text);
        }

        private void Button_2B_Click(object sender, System.EventArgs e)
        {
            SetDialogResult(Button_2B.Text);
        }

        private void Button_3A_Click(object sender, System.EventArgs e)
        {
            SetDialogResult(Button_3A.Text);
        }

        private void Button_3B_Click(object sender, System.EventArgs e)
        {
            SetDialogResult(Button_3B.Text);
        }

        private void Button_3C_Click(object sender, System.EventArgs e)
        {
            SetDialogResult(Button_3C.Text);
        }

        private void MessageBoxForm_Load(object sender, System.EventArgs e)
        {
            SetButtons(m_kButton);
            SetIcon(m_kIcon);

            // approximately 90 characters per line
            int iTextLength = RichTextBox_Message.Text.Length;
            int iLines = (iTextLength / 90) + 1;

            // Add in lines for carriage returns...
            string[] strReturns = RichTextBox_Message.Text.Split('\n');

            if (strReturns != null)
                iLines += strReturns.Length;

            int iHeight = (int)(iLines * Font.Height * 3.0f);

            if (iHeight > 450)
                iHeight = 450;

            System.Drawing.Size s = 
                new System.Drawing.Size(600, iHeight);

            this.Size = s;
            CenterToParent();

            Focus();
        }
    }
}
