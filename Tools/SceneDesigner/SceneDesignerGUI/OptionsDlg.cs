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
using System.Diagnostics;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.IO;
using System.Reflection;
using System.Reflection.Emit;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.GUI.Utility;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    /// <summary>
    /// Summary description for OptionsDlg.
    /// </summary>
    public class OptionsDlg : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Button m_btnOk;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Panel m_pnlMainPanel;
        private System.Windows.Forms.TreeView m_tvCategories;
        private IOptionsService m_optionsService;
        private ISettingsService m_settingsService;
        private System.Windows.Forms.Panel m_pnlSettingsControlHolder;
        /// <summary>
        /// Required designer variable.
        /// </summary>        
        private System.ComponentModel.Container components = null;

        #region Private Data
        private Control m_currentControlObject;
        private SettingsCategory[] m_eCategories;

        private static string sm_strLastCategory;

        #endregion

        public OptionsDlg()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
        }

        public void SetCategories(SettingsCategory[] categories)
        {
            m_eCategories = categories;
        }

        private IOptionsService OptionsService
        {
            get
            {
                if (m_optionsService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_optionsService = sp.GetService(typeof(IOptionsService))
                        as IOptionsService;
                }
                return m_optionsService;
            }
        }

        private ISettingsService SettingsService
        {
            get
            {
                if (m_settingsService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_settingsService = sp.GetService(typeof(ISettingsService))
                        as ISettingsService;
                }
                return m_settingsService;
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(OptionsDlg));
            this.m_btnOk = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_pnlMainPanel = new System.Windows.Forms.Panel();
            this.m_pnlSettingsControlHolder = new System.Windows.Forms.Panel();
            this.m_tvCategories = new System.Windows.Forms.TreeView();
            this.m_pnlMainPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_btnOk
            // 
            this.m_btnOk.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOk.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOk.Location = new System.Drawing.Point(218, 324);
            this.m_btnOk.Name = "m_btnOk";
            this.m_btnOk.TabIndex = 1;
            this.m_btnOk.Text = "OK";
            this.m_btnOk.Click += new System.EventHandler(this.m_btnOk_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(306, 324);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 2;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_pnlMainPanel
            // 
            this.m_pnlMainPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_pnlMainPanel.BackColor = System.Drawing.SystemColors.Control;
            this.m_pnlMainPanel.Controls.Add(this.m_pnlSettingsControlHolder);
            this.m_pnlMainPanel.Controls.Add(this.m_tvCategories);
            this.m_pnlMainPanel.Location = new System.Drawing.Point(0, 0);
            this.m_pnlMainPanel.Name = "m_pnlMainPanel";
            this.m_pnlMainPanel.Size = new System.Drawing.Size(596, 324);
            this.m_pnlMainPanel.TabIndex = 0;
            // 
            // m_pnlSettingsControlHolder
            // 
            this.m_pnlSettingsControlHolder.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_pnlSettingsControlHolder.BackColor = System.Drawing.Color.Lavender;
            this.m_pnlSettingsControlHolder.Location = new System.Drawing.Point(192, 8);
            this.m_pnlSettingsControlHolder.Name = "m_pnlSettingsControlHolder";
            this.m_pnlSettingsControlHolder.Size = new System.Drawing.Size(376, 308);
            this.m_pnlSettingsControlHolder.TabIndex = 1;
            // 
            // m_tvCategories
            // 
            this.m_tvCategories.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left)));
            this.m_tvCategories.HideSelection = false;
            this.m_tvCategories.ImageIndex = -1;
            this.m_tvCategories.Location = new System.Drawing.Point(8, 8);
            this.m_tvCategories.Name = "m_tvCategories";
            this.m_tvCategories.SelectedImageIndex = -1;
            this.m_tvCategories.Size = new System.Drawing.Size(176, 308);
            this.m_tvCategories.Sorted = true;
            this.m_tvCategories.TabIndex = 0;
            this.m_tvCategories.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.m_tvCategories_AfterSelect);
            // 
            // OptionsDlg
            // 
            this.AcceptButton = this.m_btnOk;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(584, 358);
            this.Controls.Add(this.m_pnlMainPanel);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOk);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(416, 272);
            this.Name = "OptionsDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Options";
            this.Load += new System.EventHandler(this.OptionsDlg_Load);
            this.m_pnlMainPanel.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        #region Winforms handlers

        private void OptionsDlg_Load(object sender, System.EventArgs e)
        {
            Debug.Assert(m_eCategories != null);
            ArrayList options = new ArrayList();
            foreach (SettingsCategory category in m_eCategories)
            {
                options.AddRange(
                    OptionsService.GetOptions(category));
            }
            foreach(string option in options)
            {
                string categoryPart = OptionUtilities.StripOption(option);
                BuildFullNode(categoryPart);
            }

            TreeNode lastNode = FindNodeByCategory(sm_strLastCategory,
                    m_tvCategories.Nodes);
            if (lastNode == null && m_tvCategories.Nodes.Count > 0)
            {
                lastNode = m_tvCategories.Nodes[0];
                sm_strLastCategory = lastNode.Text;
            }

            m_tvCategories.SelectedNode = lastNode;
        }

        private void m_tvCategories_AfterSelect(object sender, 
            System.Windows.Forms.TreeViewEventArgs e)
        {
            if (e.Node.Tag == null)
            {
                string category = GetCategoryNameFromNode(e.Node);
                object optionObject = BuildOptionObject(category);
                InitOptionObject(optionObject, category);
                Control optionControl = CreateOptionControl();
                optionControl.Dock = DockStyle.Fill;

                PropertyGrid optionGrid = optionControl as PropertyGrid;
                optionGrid.ToolbarVisible = false;
                if ( optionGrid != null)
                {
                    optionGrid.SelectedObject = optionObject;
                }
                e.Node.Tag = optionControl;

                sm_strLastCategory = category;
            }
            this.SuspendLayout();
            if (m_currentControlObject != null)
            {
                m_pnlSettingsControlHolder.Controls.Remove(m_currentControlObject);
            }
            m_pnlSettingsControlHolder.Controls.Add(e.Node.Tag as Control);
            m_currentControlObject = e.Node.Tag as Control;
            this.ResumeLayout();
        }

        private void m_btnOk_Click(object sender, System.EventArgs e)
        {
            //Traverse tree
            TreeNodeCollection currentNode = m_tvCategories.Nodes;
            PersistNodes(currentNode);
        }
        #endregion

        #region Private Helpers

        private void InitOptionObject(object obj, string category)
        {
            ArrayList options = GetOptionsList(category);
            foreach (string option in options)
            {
                object settingsObject = 
                    OptionsService.GetSettingsObject(option);
                object newObject = OptionUtilities.DeepCopy(settingsObject);
                OptionUtilities.SetProperty(obj, option, newObject);
            }        
        }        


        private object BuildOptionObject(string category)
        {
            ArrayList options = GetOptionsList(category);

            TypeBuilder tb = OptionUtilities.GetTypeBuilder();
            
            foreach (string option in options)
            {
                object settingsObject = 
                    OptionsService.GetSettingsObject(option);
                if (settingsObject == null)
                {
                    IMessageService messageService = 
                        ServiceProvider.Instance.GetService(
                        typeof(IMessageService)) as IMessageService;
                    Message message = new Message();
                    message.m_strText = "Invalid option";
                    string settingName = 
                        OptionsService.GetSettingsName(option);
                    SettingsCategory settingStore = 
                        OptionsService.GetSettingsCategory(option);
                    message.m_strDetails = 
                        string.Format(
                        "The option {0} is mapped to non-existent setting {1}" 
                        + Environment.NewLine 
                        + "In the settings store: {2}", option, settingName,
                        settingStore);

                    messageService.AddMessage(MessageChannelType.Errors,
                        message);
                }
                else
                {
                    OptionUtilities.AddProperty(option, 
                        OptionUtilities.StripCategories(option), 
                        settingsObject.GetType(), tb);
                }
            }
//            optionsControl
            Type myType = tb.CreateType();
            object optionObject = Activator.CreateInstance(myType);
            return optionObject;

        }


        Control CreateOptionControl()
        {
            return new PropertyGrid();
        }

        private TreeNode BuildFullNode(string nodeName)
        {
            TreeNodeCollection currentNodes = m_tvCategories.Nodes;
            string[] categories = nodeName.Split('.');
            TreeNode node = null;
            foreach (string categoryName in categories)
            {
                node = FindNode(categoryName, currentNodes);
                if (node == null)
                {
                    node = currentNodes.Add(categoryName);
                }
                currentNodes = node.Nodes;
            }
            return node;
        }

        private static string GetCategoryNameFromNode(TreeNode node)
        {
            StringBuilder builder = new StringBuilder(node.Text);
            TreeNode parent = node.Parent;
            while(parent != null)
            {
                builder.Insert(0, '.');
                builder.Insert(0, parent.Text);
                parent = parent.Parent;
            }
            return builder.ToString();
        }

        private static TreeNode FindNode(string name, TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                if (node.Text.Equals(name))
                {
                    return node;
                }
            }
            return null;
        }

        private static TreeNode FindNodeByCategory(string category,
            TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                if (GetCategoryNameFromNode(node).Equals(category))
                {
                    return node;
                }
                else if (node.Nodes != null)
                {
                    TreeNode childNode = FindNodeByCategory(category,
                        node.Nodes);
                    if (childNode != null)
                    {
                        return childNode;
                    }
                }
            }
            return null;
        }

        private void PersistNodes(TreeNodeCollection nodes)
        {
            ArrayList nodeList = new ArrayList();
            foreach(TreeNode node in nodes)
            {
                nodeList.Add(node);
            }
            PersistNodes(nodeList);
        }

        private void PersistNodes(ArrayList nodes)
        {
            ArrayList subNodes = new ArrayList();
            foreach (TreeNode node in nodes)
            {

                if (node.Tag != null)
                {
                    PropertyGrid grid = node.Tag as PropertyGrid;
                    if (grid != null)
                    {
                        object optionObject = grid.SelectedObject;
                        if (optionObject != null)
                        {
                            SaveOptions(optionObject);
                        }
                    }
                }
                if (node.Nodes != null && node.Nodes.Count > 0)
                {
                    subNodes.AddRange(node.Nodes);
                }
            }
            if (subNodes.Count > 0)
            {
                PersistNodes(subNodes);
            }
        }

        private void SaveOptions(object optionObject)
        {
            PropertyDescriptorCollection properties =
                TypeDescriptor.GetProperties(optionObject);

            foreach (PropertyDescriptor property in properties)
            {
                string optionName = property.Name;
                object value = property.GetValue(optionObject);
                string settingName = 
                    OptionsService.GetSettingsName(optionName);
                SettingsCategory category = 
                    OptionsService.GetSettingsCategory(optionName);
                SettingsService.SetSettingsObject(settingName, value,
                    category);
            }
        }

        private ArrayList GetOptionsList(string category)
        {
            Debug.Assert(m_eCategories != null);
            ArrayList filteredOptions = new ArrayList();
            foreach (SettingsCategory settingsCategory in m_eCategories)
            {
                filteredOptions.AddRange(
                    OptionsService.GetOptions(category, settingsCategory));
            }
            return filteredOptions;
        }

        #endregion


    }
}
