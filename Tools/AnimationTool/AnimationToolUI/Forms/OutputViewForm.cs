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

// For printing
using System.IO;
using System.Diagnostics;

using System.Runtime.InteropServices;

using WeifenLuo.WinFormsUI;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for OutputViewForm.
    /// </summary>
    public class OutputViewForm : DockContent
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        private System.Windows.Forms.RichTextBox RichEditBox_Output;

        private System.Windows.Forms.ComboBox ComboBox_Output;
        private System.Windows.Forms.GroupBox GroupBox_Output;
        private System.Windows.Forms.ContextMenu ContextMenu_Output;
        private System.Windows.Forms.MenuItem MenuItem_ClearAll;

        private const int SB_VERT = 1;
        private const int EM_SETSCROLLPOS = 0x0400 + 222;
        private System.Windows.Forms.MenuItem miSave;
        private System.Windows.Forms.MenuItem menuItem1;

        private const int m_iMaxLogs = 4096;

        private class SavedMessage
        {
            public SavedMessage()
            {
                m_kColor = System.Drawing.Color.Black;
            }

            public int m_iChannel;
            public string m_strMessage;
            public string m_strTime;
            public System.Drawing.Color m_kColor;
        };

        private ArrayList m_alSavedMessage = new ArrayList();

        public OutputViewForm()
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(OutputViewForm));
            this.RichEditBox_Output = new System.Windows.Forms.RichTextBox();
            this.ContextMenu_Output = new System.Windows.Forms.ContextMenu();
            this.MenuItem_ClearAll = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.miSave = new System.Windows.Forms.MenuItem();
            this.ComboBox_Output = new System.Windows.Forms.ComboBox();
            this.GroupBox_Output = new System.Windows.Forms.GroupBox();
            this.GroupBox_Output.SuspendLayout();
            this.SuspendLayout();
            // 
            // RichEditBox_Output
            // 
            this.RichEditBox_Output.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.RichEditBox_Output.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(255)), ((System.Byte)(224)), ((System.Byte)(192)));
            this.RichEditBox_Output.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.RichEditBox_Output.ContextMenu = this.ContextMenu_Output;
            this.RichEditBox_Output.Location = new System.Drawing.Point(3, 10);
            this.RichEditBox_Output.Name = "RichEditBox_Output";
            this.RichEditBox_Output.ReadOnly = true;
            this.RichEditBox_Output.Size = new System.Drawing.Size(376, 280);
            this.RichEditBox_Output.TabIndex = 0;
            this.RichEditBox_Output.Text = "";
            this.RichEditBox_Output.WordWrap = false;
            this.RichEditBox_Output.TextChanged += new System.EventHandler(this.RichEditBox_Output_TextChanged);
            this.RichEditBox_Output.KeyUp += new System.Windows.Forms.KeyEventHandler(this.RichEditBox_Output_KeyUp);
            // 
            // ContextMenu_Output
            // 
            this.ContextMenu_Output.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                               this.MenuItem_ClearAll,
                                                                                               this.menuItem1,
                                                                                               this.miSave});
            // 
            // MenuItem_ClearAll
            // 
            this.MenuItem_ClearAll.Index = 0;
            this.MenuItem_ClearAll.Text = "Clear All";
            this.MenuItem_ClearAll.Click += new System.EventHandler(this.MenuItem_ClearAll_Click);
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 1;
            this.menuItem1.Text = "-";
            // 
            // miSave
            // 
            this.miSave.Index = 2;
            this.miSave.Text = "Save All to File";
            this.miSave.Click += new System.EventHandler(this.miSave_Click);
            // 
            // ComboBox_Output
            // 
            this.ComboBox_Output.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.ComboBox_Output.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ComboBox_Output.Items.AddRange(new object[] {
                                                                 "Test1",
                                                                 "Test2",
                                                                 "Test3"});
            this.ComboBox_Output.Location = new System.Drawing.Point(8, 8);
            this.ComboBox_Output.Name = "ComboBox_Output";
            this.ComboBox_Output.Size = new System.Drawing.Size(384, 21);
            this.ComboBox_Output.TabIndex = 1;
            this.ComboBox_Output.SelectedIndexChanged += new System.EventHandler(this.ComboBox_Output_SelectedIndexChanged);
            // 
            // GroupBox_Output
            // 
            this.GroupBox_Output.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.GroupBox_Output.Controls.Add(this.RichEditBox_Output);
            this.GroupBox_Output.ForeColor = System.Drawing.SystemColors.Control;
            this.GroupBox_Output.Location = new System.Drawing.Point(8, 24);
            this.GroupBox_Output.Name = "GroupBox_Output";
            this.GroupBox_Output.Size = new System.Drawing.Size(384, 296);
            this.GroupBox_Output.TabIndex = 0;
            this.GroupBox_Output.TabStop = false;
            // 
            // OutputViewForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(400, 326);
            this.Controls.Add(this.ComboBox_Output);
            this.Controls.Add(this.GroupBox_Output);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "OutputViewForm";
            this.Text = "OutputViewForm";
            this.Load += new System.EventHandler(this.OutputViewForm_Load);
            this.GroupBox_Output.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        public void ListBox_Clear()
        {
            RichEditBox_Output.Clear();
        }

        static public System.Drawing.Color ChooseChannelColor(int iChannelID)
        {
            switch(iChannelID)
            {
                case 0: return System.Drawing.Color.Black; 
                case 1: return System.Drawing.Color.Magenta; 
                case 2: return System.Drawing.Color.Blue; 
                case 3: return System.Drawing.Color.BlueViolet; 
                case 4: return System.Drawing.Color.Red; 
                default: return System.Drawing.Color.Black;
            }
        }

        private void SaveMessage(
            int iChannelID, string strMessage, string strTime)
        {
            if (m_alSavedMessage.Count > m_iMaxLogs)
            {
                ClearAll();
            }

            SavedMessage kSavedMessage = new SavedMessage();
            kSavedMessage.m_iChannel = iChannelID;
            kSavedMessage.m_strMessage = strMessage;
            kSavedMessage.m_strTime = strTime;
            kSavedMessage.m_kColor = ChooseChannelColor(iChannelID);
            m_alSavedMessage.Add(kSavedMessage);
        }

        private void InitializeSavedMessages(ArrayList ChannelList)
        {
            m_alSavedMessage.Clear();

            for (int i=0; i < ChannelList.Count; i++)
            {            
                MMessageChannel kChannel = (MMessageChannel)ChannelList[i];

                ArrayList alMessageList = kChannel.MessageList;

                for (int j=0; j < alMessageList.Count; j++)
                {
                    SaveMessage( 
                        kChannel.ChannelID, 
                        kChannel.get_Message(j),
                        kChannel.get_Time(j));
                }
            }
        }

        public void RefreshData()
        {
            InitializeListBoxWithSavedMessages(ComboBox_Output.SelectedIndex);
        }

        private void InitializeListBoxWithSavedMessages(int iChannel)
        {
            ListBox_Clear();

            ArrayList ChannelList = MFramework.Instance.Output.ChannelList;
            int iMaxChannel = ChannelList.Count;

            bool bFilter = true;
            if (iChannel >= iMaxChannel)
                bFilter = false;

            for (int i=0; i< m_alSavedMessage.Count; i++)
            {
                SavedMessage kSavedMessage = (SavedMessage)m_alSavedMessage[i];

                if (bFilter)
                {
                    if (kSavedMessage.m_iChannel != iChannel)
                        continue;
                }
                
                string strMessage =
                    kSavedMessage.m_strMessage;
                
                System.Drawing.Color kColor = ChooseChannelColor(
                    kSavedMessage.m_iChannel);

                if (bFilter)
                    RichEditBox_Output.ForeColor = kColor;
                else
                    RichEditBox_Output.SelectionColor = kColor;

                RichEditBox_Output.AppendText(strMessage);

                if (kSavedMessage.m_iChannel == 0)
                    AppendTime(kSavedMessage.m_strTime, 
                        System.Drawing.Color.RoyalBlue);
                else
                    RichEditBox_Output.AppendText("\n");
            }   
       
            ScrollToEnd();            
        }

        private void OutputViewForm_Load(object sender, System.EventArgs e)
        {
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);

            // Fill the combo box
            ArrayList ChannelList = MFramework.Instance.Output.ChannelList;

            object[] kObjects = new object[ChannelList.Count + 1];
            int i;
            for (i=0; i < ChannelList.Count; i++)
            {            
                MMessageChannel kChannel = (MMessageChannel)ChannelList[i];
                kObjects[i] = kChannel.Name;            
            }

            kObjects[ ChannelList.Count ] = "All Channels"; 

            ComboBox_Output.Items.Clear();
            ComboBox_Output.Items.AddRange( kObjects );

            // listen to all messages 
            for (i=0; i < ChannelList.Count; i++)
            {            
                MMessageChannel kChannel = (MMessageChannel)ChannelList[i];
                kChannel.OnNewMessageEvent += 
                    new NewMessageEvent( OnNewMessage);            
            }

            // Fill our Saved structure with current messages that may have
            // occured before our form loaded.
            InitializeSavedMessages(ChannelList);         

            // Clear current listbox and fill with our saved messages.
            int iSelected = ComboBox_Output.Items.Count - 1;
            ComboBox_Output.SelectedIndex = iSelected;

            MLogger.OnUpdateScrollEvent +=
                new UpdateScrollEvent(OnUpdateScrollEvent);
        }

        public void OnUpdateScrollEvent()
        {
            ScrollToEnd();
        }

        public void OnNewMessage(MMessageChannel kChannel)
        {
            if (!IsDisposed)
            {
                // on our given channel
                string strMessage = kChannel.LatestMessage;
                string strTime = kChannel.LatestTime;

                string strComb = strMessage;

                // Save for purposes of recreating and filtering
                SaveMessage( kChannel.ChannelID, strMessage, strTime );

                ArrayList ChannelList = MFramework.Instance.Output.ChannelList;
                int iMaxChannel = ChannelList.Count;
                int iSelectedChannel = ComboBox_Output.SelectedIndex; 
                bool bFilter = true;
                if (iSelectedChannel >= iMaxChannel)
                    bFilter = false;

                if (bFilter)
                {
                    if (kChannel.ChannelID != iSelectedChannel)
                        return;
                }
               
                System.Drawing.Color kColor = 
                    ChooseChannelColor(kChannel.ChannelID); 

                if (bFilter)
                    RichEditBox_Output.ForeColor = kColor;
                else
                    RichEditBox_Output.SelectionColor = kColor;

                RichEditBox_Output.AppendText(strComb);

                if (kChannel.ChannelID  == 0)
                    AppendTime(strTime, System.Drawing.Color.RoyalBlue);
                else
                    RichEditBox_Output.AppendText("\n");

                if (kChannel.UpdateScroll)
                    ScrollToEnd();
            }
        }

        private void AppendTime(string strTime, System.Drawing.Color kColor)
        {
            RichEditBox_Output.SelectionColor = kColor;
            string strMessage = " Time=" + strTime + "\n";
            RichEditBox_Output.AppendText(strMessage);
        }

        private void ComboBox_Output_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            int iSelected = ComboBox_Output.SelectedIndex; 
            InitializeListBoxWithSavedMessages(iSelected);
        }

        private void RichEditBox_Output_TextChanged(
            object sender, System.EventArgs e)
        {
        }

        private void ScrollToEnd()
        {
            int min, max;

            // For some reason, the scrollbars have not yet been updated at 
            // this point, so, to ensure that our call to GetScrollRange is 
            // accurate, we force the scrollbars to be recaluclated...
            RichEditBox_Output.ScrollBars = 0;
            RichEditBox_Output.ScrollBars = 
                System.Windows.Forms.RichTextBoxScrollBars.Both;
            GetScrollRange(RichEditBox_Output.Handle,
                SB_VERT, out min, out max);
            int iEditBoxHeight = RichEditBox_Output.Height;
            
            // We use iPreferredHeight to add an extra line that may be needed
            // when the horizontal scrollbar is out.
            int iPreferredHeight = RichEditBox_Output.PreferredHeight;
            iEditBoxHeight -= iPreferredHeight;
            if (iEditBoxHeight < 0)
                iEditBoxHeight = 0;

            if (max > iEditBoxHeight)
                SendMessage(RichEditBox_Output.Handle, EM_SETSCROLLPOS, 0,
                    new POINT(0, max - (iEditBoxHeight))); 
            else
               SendMessage(RichEditBox_Output.Handle, EM_SETSCROLLPOS, 0,
                    new POINT(0, 0)); 
        }

        [DllImport("user32", CharSet=CharSet.Auto)]
        public static extern bool GetScrollRange(
            IntPtr hWnd, int nBar, out int lpMinPos, out int lpMaxPos);

        [DllImport("user32", CharSet=CharSet.Auto)]
        public static extern IntPtr SendMessage(
            IntPtr hWnd, int msg, int wParam, POINT lParam);

        private void ClearAll()
        {
            m_alSavedMessage.Clear();
            ListBox_Clear();
            ScrollToEnd();
        }

        private void MenuItem_ClearAll_Click(object sender, System.EventArgs e)
        {
            ClearAll();
        }

        private void RichEditBox_Output_KeyUp(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F1 && HelpManager.HelpExists())
            {
                HelpManager.DoHelp(
                    this, @"User_Interface_Description/Output_View.htm");
            }
        }

        private void miSave_Click(object sender, System.EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.DefaultExt = "*.rtf";
            sfd.Filter = "RTF Files|*.rtf";

            if(sfd.ShowDialog() == DialogResult.OK)
            {
                RichEditBox_Output.SaveFile(sfd.FileName,
                    RichTextBoxStreamType.RichText); 
                // RichTextBoxStreamType.PlainText);
            }           
        }

        [StructLayout(LayoutKind.Sequential)]
            public class POINT
        {
            public int x;
            public int y;

            public POINT(){}

            public POINT(int x, int y)
            {
                this.x = x;
                this.y = y;
            }
        }
    }
}
