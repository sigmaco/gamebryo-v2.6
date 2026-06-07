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
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using PluginAPI = Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    /// <summary>
    /// Summary description for MessagesPanel.
    /// </summary>
    [DockPosition(Position=DefaultDock.DockBottom)]
    public class MessagesPanel : System.Windows.Forms.Form, ICommandPanel
    {
        #region Private Data
        private IMessageService m_messageService;
        private IUICommandService m_commandService;
        private ListViewItem.ListViewSubItem m_lastItem;
        #endregion

        private System.Windows.Forms.ListView m_lvMessages;
        private System.Windows.Forms.ColumnHeader messageHeader;
        private System.Windows.Forms.ComboBox m_cbChannel;
        private System.Windows.Forms.Button m_btnClearChannel;
        private System.Windows.Forms.Button m_btnClearAll;
        private System.Windows.Forms.ToolTip m_toolTip;
        private System.Windows.Forms.ColumnHeader msgTime;
        private System.Windows.Forms.ColumnHeader detailsHeader;
        private System.Windows.Forms.Label m_lblDummy;
        private System.ComponentModel.IContainer components;

        public MessagesPanel( IUICommandService commandService,
            IMessageService messageService)
        {
            //
            // Required for Windows Form Designer support
            //
            //Debugger.Break();
            InitializeComponent();
            m_messageService = messageService;
            m_commandService = commandService;
            InitChannelCombo();
            m_commandService.BindCommands(this);
           
        }

        private void InitChannelCombo()
        {
            string[] channelNames = 
                Enum.GetNames(typeof(MessageChannelType));
            foreach(string name in channelNames)
            {
                m_cbChannel.Items.Add(name);
            }
        }

//        private IMessageService MessageService
//        {
//            get
//            {
//                return
//                    m_messageService.GetService(typeof(IMessageService))
//                    as IMessageService;
//            }
//        }

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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MessagesPanel));
            this.m_btnClearChannel = new System.Windows.Forms.Button();
            this.m_lvMessages = new System.Windows.Forms.ListView();
            this.msgTime = new System.Windows.Forms.ColumnHeader();
            this.messageHeader = new System.Windows.Forms.ColumnHeader();
            this.detailsHeader = new System.Windows.Forms.ColumnHeader();
            this.m_cbChannel = new System.Windows.Forms.ComboBox();
            this.m_btnClearAll = new System.Windows.Forms.Button();
            this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.m_lblDummy = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_btnClearChannel
            // 
            this.m_btnClearChannel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_btnClearChannel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnClearChannel.Location = new System.Drawing.Point(8, 456);
            this.m_btnClearChannel.Name = "m_btnClearChannel";
            this.m_btnClearChannel.Size = new System.Drawing.Size(88, 23);
            this.m_btnClearChannel.TabIndex = 2;
            this.m_btnClearChannel.Text = "Clear Channel";
            this.m_btnClearChannel.Click += new System.EventHandler(this.m_btnClearChannel_Click);
            // 
            // m_lvMessages
            // 
            this.m_lvMessages.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lvMessages.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
                                                                                           this.msgTime,
                                                                                           this.messageHeader,
                                                                                           this.detailsHeader});
            this.m_lvMessages.FullRowSelect = true;
            this.m_lvMessages.GridLines = true;
            this.m_lvMessages.Location = new System.Drawing.Point(8, 32);
            this.m_lvMessages.Name = "m_lvMessages";
            this.m_lvMessages.Size = new System.Drawing.Size(664, 416);
            this.m_lvMessages.TabIndex = 1;
            this.m_lvMessages.View = System.Windows.Forms.View.Details;
            this.m_lvMessages.DoubleClick += new System.EventHandler(this.m_lvMessages_DoubleClick);
            this.m_lvMessages.MouseMove += new System.Windows.Forms.MouseEventHandler(this.m_lvMessages_MouseMove);
            // 
            // msgTime
            // 
            this.msgTime.Text = "Time";
            this.msgTime.Width = 91;
            // 
            // messageHeader
            // 
            this.messageHeader.Text = "Message";
            this.messageHeader.Width = 267;
            // 
            // detailsHeader
            // 
            this.detailsHeader.Text = "Details";
            this.detailsHeader.Width = 255;
            // 
            // m_cbChannel
            // 
            this.m_cbChannel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbChannel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbChannel.Location = new System.Drawing.Point(8, 8);
            this.m_cbChannel.Name = "m_cbChannel";
            this.m_cbChannel.Size = new System.Drawing.Size(664, 21);
            this.m_cbChannel.TabIndex = 0;
            this.m_cbChannel.SelectedIndexChanged += new System.EventHandler(this.m_cbChannel_SelectedIndexChanged);
            // 
            // m_btnClearAll
            // 
            this.m_btnClearAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_btnClearAll.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnClearAll.ForeColor = System.Drawing.SystemColors.ControlText;
            this.m_btnClearAll.Location = new System.Drawing.Point(608, 456);
            this.m_btnClearAll.Name = "m_btnClearAll";
            this.m_btnClearAll.Size = new System.Drawing.Size(64, 23);
            this.m_btnClearAll.TabIndex = 3;
            this.m_btnClearAll.Text = "Clear All";
            this.m_btnClearAll.Click += new System.EventHandler(this.m_btnClearAll_Click);
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.BackColor = System.Drawing.SystemColors.Control;
            this.m_lblDummy.Location = new System.Drawing.Point(168, 104);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(8, 8);
            this.m_lblDummy.TabIndex = 4;
            // 
            // MessagesPanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(680, 486);
            this.Controls.Add(this.m_btnClearAll);
            this.Controls.Add(this.m_cbChannel);
            this.Controls.Add(this.m_btnClearChannel);
            this.Controls.Add(this.m_lvMessages);
            this.Controls.Add(this.m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MessagesPanel";
            this.Text = "Messages";
            this.Load += new System.EventHandler(this.MessagesPanel_Load);
            this.ResumeLayout(false);

        }
        #endregion

        public void RegisterEventHandlers()
        {
            m_messageService.MessageAdded +=
                new MessageAddedHandler(MessageAdded);
            m_messageService.AllMessagesCleared +=
                new AllMessagesClearedHandler(AllMessagesCleared);
            m_messageService.ChannelMessagesCleared +=
                new ChannelMessagesClearedHandler(ChannelMessagesCleared);
                
        }

        private void ChannelMessagesCleared(MessageChannelType eChannel)
        {
            RefreshList();
        }

        private void AllMessagesCleared()
        {
            RefreshList();
        }


        private void MessageAdded(MessageChannelType eChannel, int iIndex)
        {
            try
            {
                MessageChannelType eCurrentChannel = GetChannel();
                if (eCurrentChannel == eChannel ||
                    eCurrentChannel == MessageChannelType.AllChannels)
                {
                    string channelName =
                        m_messageService.GetChannelName(eChannel);
                    PluginAPI.Message message =
                        m_messageService.GetMessage(eChannel,
                        iIndex);
                    AddItem(iIndex, eChannel, channelName, message);
                }

                if (eChannel == MessageChannelType.Errors || 
                    eChannel == MessageChannelType.Conflicts)
                {
                    if (!this.Visible && this.Created)
                    {
                        ServiceProvider sp = ServiceProvider.Instance;
                        ICommandPanelService panelService = 
                            sp.GetService(typeof(ICommandPanelService))
                            as ICommandPanelService;
                        panelService.ShowPanel(this.Text, true);
                    }
                    if (eCurrentChannel != eChannel &&
                        eCurrentChannel != MessageChannelType.AllChannels)
                    {
                        m_cbChannel.SelectedIndex = (int) eChannel;
                    }
                }
            }
            catch (InvalidOperationException)
            {
            }
        }

        private void AddItem(int iIndex, MessageChannelType eChannel, 
            string channelName, PluginAPI.Message message)
        {
            ListViewItem item = m_lvMessages.Items.Add(message.m_strTime);
            item.Tag = message;
            //Add to channel
            item.SubItems.Add(message.m_strText);
            item.SubItems.Add(message.m_strDetails);
            item.UseItemStyleForSubItems = true;
            item.ForeColor = GetChannelTextColor(eChannel);
        }

        private Color GetChannelTextColor(MessageChannelType channelType)
        {
            switch (channelType)
            {
                case MessageChannelType.Errors:
                    return Color.Red;
                case MessageChannelType.Conflicts:
                    return Color.Fuchsia;
                default:
                    return SystemColors.ControlText;
            }
        }

        private void MessagesPanel_Load(object sender, System.EventArgs e)
        {
            m_cbChannel.SelectedIndex = (int) MessageChannelType.AllChannels;
        }

        private void RefreshList()
        {
            m_lvMessages.Items.Clear();
            MessageChannelType channel = 
                GetChannel();
            if (channel == (MessageChannelType) (-1))
                return;

            if (channel != MessageChannelType.AllChannels)
            {
                string channelName = m_messageService.GetChannelName(channel);
                PluginAPI.Message[] messages = m_messageService.GetMessages(
                    channel);
                int size = messages.Length;
                for (int i = 0; i < size; i++)
                {
                    AddItem(i, channel, channelName, messages[i]);
                }
            }
            else
            {
                int iCount = 0;
                string[] channelNames = 
                    Enum.GetNames(typeof(MessageChannelType));
                foreach (string channelName in channelNames)
                {
                    channel = (MessageChannelType)Enum.Parse(
                        typeof(MessageChannelType), channelName);

                    PluginAPI.Message[] messages = 
                        m_messageService.GetMessages(channel);
                    int size = messages.Length;
                    for (int i = 0; i < size; i++)
                    {
                        AddItem(iCount, channel, channelName, messages[i]);
                        iCount++;
                    }
                }
            }
        }

        private MessageChannelType GetChannel()
        {
            int selectedIndex = m_cbChannel.SelectedIndex;
            if (selectedIndex >= 0)
            {
                string text = m_cbChannel.SelectedText;
                text = m_cbChannel.Items[m_cbChannel.SelectedIndex].ToString();
                //if the implementation of MMessageManager changes, 
                //"this may break.
                return (MessageChannelType) Enum.Parse(
                    typeof(MessageChannelType), text);
            }
            else
            {
                return (MessageChannelType) (-1);
            }
        }

        private void m_btnClearChannel_Click(object sender, System.EventArgs e)
        {
            string message = 
                "Are you sure you would like to "
                + "clear all messages for this channel?";
            if (MessageBox.Show(message, "Clear Channel?", 
                MessageBoxButtons.YesNo)
                == System.Windows.Forms.DialogResult.Yes)
            {
                m_messageService.ClearMessages(GetChannel());
            }
        }

        private void m_btnClearAll_Click(object sender, System.EventArgs e)
        {
            string message = 
                "Are you sure you would like to clear all messages?";
            if (MessageBox.Show(message, "Clear All?", 
                MessageBoxButtons.YesNo)
                == System.Windows.Forms.DialogResult.Yes)
            {
                m_messageService.ClearAllMessages();
            }
        }

        private void m_cbChannel_SelectedIndexChanged(object sender, 
            System.EventArgs e)
        {
            RefreshList();            
        }

        private void m_lvMessages_MouseMove(object sender, 
            System.Windows.Forms.MouseEventArgs e)
        {
            ListViewItem.ListViewSubItem subItem = 
                GetListViewSubItemAt(m_lvMessages, e.X, e.Y);
            if (subItem != m_lastItem)
            {
                m_lastItem = subItem;
                if (subItem != null)
                {
                    m_toolTip.SetToolTip(m_lvMessages, 
                        subItem.Text);
                }
                else
                {
                    m_toolTip.SetToolTip(m_lvMessages, "");
                }
            }

        }

        private void m_lvMessages_DoubleClick(object sender, 
            System.EventArgs e)
        {
            ListView.SelectedListViewItemCollection items = 
                m_lvMessages.SelectedItems;
            if (items != null && items.Count > 0)
            {
                ListViewItem item = items[0];
                PluginAPI.Message message = item.Tag as PluginAPI.Message;
                if (message != null)
                {
                    string popupText = message.m_strText 
                        + Environment.NewLine
                        + "---------------------------------------------" 
                        + Environment.NewLine
                        + "Details:" + Environment.NewLine
                        + message.m_strDetails;
                    MessageBox.Show(popupText, "Message Details",
                        MessageBoxButtons.OK);
                }

            }
        }

        private static ListViewItem.ListViewSubItem GetListViewSubItemAt(
            ListView listView, int x, int y)
        {
            ListViewItem item = listView.GetItemAt(x, y);
            if (item != null)
            {
                int xMin = 0;
                int xMax = 0;
                for (int i = 0; i < item.SubItems.Count; i++)
                {
                    xMin = xMax;
                    xMax = xMax + listView.Columns[i].Width;
                    if (x <= xMax && x >= xMin)
                    {
                        return item.SubItems[i];
                    }                   
                }
            }
            return null;
        }
    }
}
