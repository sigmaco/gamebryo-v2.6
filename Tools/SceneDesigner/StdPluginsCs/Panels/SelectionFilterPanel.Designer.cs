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

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    partial class SelectionFilterPanel
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.Label m_lblInstructions;
            System.Windows.Forms.Label m_lblDummy;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SelectionFilterPanel));
            this.m_chkFilterEnabled = new System.Windows.Forms.CheckBox();
            this.m_lbTags = new System.Windows.Forms.CheckedListBox();
            this.m_lblFilterByTags = new System.Windows.Forms.Label();
            this.m_lblFilterByLayers = new System.Windows.Forms.Label();
            this.m_lbLayers = new System.Windows.Forms.CheckedListBox();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.m_gbFilterByName = new System.Windows.Forms.GroupBox();
            this.m_rbComponentName = new System.Windows.Forms.RadioButton();
            this.m_rbTemplateName = new System.Windows.Forms.RadioButton();
            this.m_rbEntityName = new System.Windows.Forms.RadioButton();
            this.m_lblName = new System.Windows.Forms.Label();
            this.m_menuLayersContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.m_miUncheckAllLayers = new System.Windows.Forms.ToolStripMenuItem();
            this.m_menuTagsContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.m_miUncheckAllTags = new System.Windows.Forms.ToolStripMenuItem();
            m_lblInstructions = new System.Windows.Forms.Label();
            m_lblDummy = new System.Windows.Forms.Label();
            this.m_gbFilterByName.SuspendLayout();
            this.m_menuLayersContext.SuspendLayout();
            this.m_menuTagsContext.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_lblInstructions
            // 
            m_lblInstructions.AutoSize = true;
            m_lblInstructions.Location = new System.Drawing.Point(9, 9);
            m_lblInstructions.Name = "m_lblInstructions";
            m_lblInstructions.Size = new System.Drawing.Size(170, 13);
            m_lblInstructions.TabIndex = 0;
            m_lblInstructions.Text = "Advanced Selection Filter Options:";
            // 
            // m_lblDummy
            // 
            m_lblDummy.Location = new System.Drawing.Point(240, 334);
            m_lblDummy.Name = "m_lblDummy";
            m_lblDummy.Size = new System.Drawing.Size(8, 8);
            m_lblDummy.TabIndex = 7;
            // 
            // m_chkFilterEnabled
            // 
            this.m_chkFilterEnabled.AutoSize = true;
            this.m_chkFilterEnabled.Location = new System.Drawing.Point(9, 30);
            this.m_chkFilterEnabled.Name = "m_chkFilterEnabled";
            this.m_chkFilterEnabled.Size = new System.Drawing.Size(137, 17);
            this.m_chkFilterEnabled.TabIndex = 1;
            this.m_chkFilterEnabled.Text = "Selection Filter Enabled";
            this.m_chkFilterEnabled.UseVisualStyleBackColor = true;
            this.m_chkFilterEnabled.CheckedChanged += new System.EventHandler(this.m_chkFilterEnabled_CheckedChanged);
            // 
            // m_lbTags
            // 
            this.m_lbTags.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbTags.CheckOnClick = true;
            this.m_lbTags.ContextMenuStrip = this.m_menuTagsContext;
            this.m_lbTags.FormattingEnabled = true;
            this.m_lbTags.IntegralHeight = false;
            this.m_lbTags.Location = new System.Drawing.Point(9, 262);
            this.m_lbTags.Name = "m_lbTags";
            this.m_lbTags.Size = new System.Drawing.Size(359, 198);
            this.m_lbTags.Sorted = true;
            this.m_lbTags.TabIndex = 6;
            this.m_lbTags.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.m_lbTags_ItemCheck);
            // 
            // m_lblFilterByTags
            // 
            this.m_lblFilterByTags.AutoSize = true;
            this.m_lblFilterByTags.Location = new System.Drawing.Point(6, 246);
            this.m_lblFilterByTags.Name = "m_lblFilterByTags";
            this.m_lblFilterByTags.Size = new System.Drawing.Size(73, 13);
            this.m_lblFilterByTags.TabIndex = 5;
            this.m_lblFilterByTags.Text = "Filter by Tags:";
            // 
            // m_lblFilterByLayers
            // 
            this.m_lblFilterByLayers.AutoSize = true;
            this.m_lblFilterByLayers.Location = new System.Drawing.Point(9, 139);
            this.m_lblFilterByLayers.Name = "m_lblFilterByLayers";
            this.m_lblFilterByLayers.Size = new System.Drawing.Size(80, 13);
            this.m_lblFilterByLayers.TabIndex = 3;
            this.m_lblFilterByLayers.Text = "Filter by Layers:";
            // 
            // m_lbLayers
            // 
            this.m_lbLayers.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbLayers.CheckOnClick = true;
            this.m_lbLayers.ContextMenuStrip = this.m_menuLayersContext;
            this.m_lbLayers.FormattingEnabled = true;
            this.m_lbLayers.IntegralHeight = false;
            this.m_lbLayers.Location = new System.Drawing.Point(9, 155);
            this.m_lbLayers.Name = "m_lbLayers";
            this.m_lbLayers.Size = new System.Drawing.Size(359, 79);
            this.m_lbLayers.Sorted = true;
            this.m_lbLayers.TabIndex = 4;
            this.m_lbLayers.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.m_lbLayers_ItemCheck);
            // 
            // m_tbName
            // 
            this.m_tbName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbName.Location = new System.Drawing.Point(50, 43);
            this.m_tbName.Name = "m_tbName";
            this.m_tbName.Size = new System.Drawing.Size(303, 20);
            this.m_tbName.TabIndex = 4;
            this.m_tbName.TextChanged += new System.EventHandler(this.m_tbName_TextChanged);
            // 
            // m_gbFilterByName
            // 
            this.m_gbFilterByName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_gbFilterByName.Controls.Add(this.m_rbComponentName);
            this.m_gbFilterByName.Controls.Add(this.m_rbTemplateName);
            this.m_gbFilterByName.Controls.Add(this.m_rbEntityName);
            this.m_gbFilterByName.Controls.Add(this.m_lblName);
            this.m_gbFilterByName.Controls.Add(this.m_tbName);
            this.m_gbFilterByName.Location = new System.Drawing.Point(9, 53);
            this.m_gbFilterByName.Name = "m_gbFilterByName";
            this.m_gbFilterByName.Size = new System.Drawing.Size(359, 73);
            this.m_gbFilterByName.TabIndex = 2;
            this.m_gbFilterByName.TabStop = false;
            this.m_gbFilterByName.Text = "Filter by Name";
            // 
            // m_rbComponentName
            // 
            this.m_rbComponentName.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_rbComponentName.AutoSize = true;
            this.m_rbComponentName.Location = new System.Drawing.Point(206, 20);
            this.m_rbComponentName.Name = "m_rbComponentName";
            this.m_rbComponentName.Size = new System.Drawing.Size(79, 17);
            this.m_rbComponentName.TabIndex = 2;
            this.m_rbComponentName.TabStop = true;
            this.m_rbComponentName.Text = "Component";
            this.m_rbComponentName.UseVisualStyleBackColor = true;
            this.m_rbComponentName.CheckedChanged += new System.EventHandler(this.m_rbComponentName_CheckedChanged);
            // 
            // m_rbTemplateName
            // 
            this.m_rbTemplateName.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_rbTemplateName.AutoSize = true;
            this.m_rbTemplateName.Location = new System.Drawing.Point(131, 19);
            this.m_rbTemplateName.Name = "m_rbTemplateName";
            this.m_rbTemplateName.Size = new System.Drawing.Size(69, 17);
            this.m_rbTemplateName.TabIndex = 1;
            this.m_rbTemplateName.TabStop = true;
            this.m_rbTemplateName.Text = "Template";
            this.m_rbTemplateName.UseVisualStyleBackColor = true;
            this.m_rbTemplateName.CheckedChanged += new System.EventHandler(this.m_rbTemplateName_CheckedChanged);
            // 
            // m_rbEntityName
            // 
            this.m_rbEntityName.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.m_rbEntityName.AutoSize = true;
            this.m_rbEntityName.Location = new System.Drawing.Point(74, 20);
            this.m_rbEntityName.Name = "m_rbEntityName";
            this.m_rbEntityName.Size = new System.Drawing.Size(51, 17);
            this.m_rbEntityName.TabIndex = 0;
            this.m_rbEntityName.TabStop = true;
            this.m_rbEntityName.Text = "Entity";
            this.m_rbEntityName.UseVisualStyleBackColor = true;
            this.m_rbEntityName.CheckedChanged += new System.EventHandler(this.m_rbEntityName_CheckedChanged);
            // 
            // m_lblName
            // 
            this.m_lblName.AutoSize = true;
            this.m_lblName.Location = new System.Drawing.Point(6, 46);
            this.m_lblName.Name = "m_lblName";
            this.m_lblName.Size = new System.Drawing.Size(38, 13);
            this.m_lblName.TabIndex = 3;
            this.m_lblName.Text = "Name:";
            // 
            // m_menuLayersContext
            // 
            this.m_menuLayersContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_miUncheckAllLayers});
            this.m_menuLayersContext.Name = "m_menuLayersContext";
            this.m_menuLayersContext.ShowImageMargin = false;
            this.m_menuLayersContext.Size = new System.Drawing.Size(108, 26);
            // 
            // m_miUncheckAllLayers
            // 
            this.m_miUncheckAllLayers.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.m_miUncheckAllLayers.Name = "m_miUncheckAllLayers";
            this.m_miUncheckAllLayers.Size = new System.Drawing.Size(107, 22);
            this.m_miUncheckAllLayers.Text = "&Uncheck All";
            this.m_miUncheckAllLayers.Click += new System.EventHandler(this.m_miUncheckAllLayers_Click);
            // 
            // m_menuTagsContext
            // 
            this.m_menuTagsContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_miUncheckAllTags});
            this.m_menuTagsContext.Name = "m_menuLayersContext";
            this.m_menuTagsContext.ShowImageMargin = false;
            this.m_menuTagsContext.Size = new System.Drawing.Size(108, 26);
            // 
            // m_miUncheckAllTags
            // 
            this.m_miUncheckAllTags.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.m_miUncheckAllTags.Name = "m_miUncheckAllTags";
            this.m_miUncheckAllTags.Size = new System.Drawing.Size(107, 22);
            this.m_miUncheckAllTags.Text = "&Uncheck All";
            this.m_miUncheckAllTags.Click += new System.EventHandler(this.m_miUncheckAllTags_Click);
            // 
            // SelectionFilterPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(380, 471);
            this.Controls.Add(this.m_gbFilterByName);
            this.Controls.Add(this.m_lbLayers);
            this.Controls.Add(this.m_lblFilterByLayers);
            this.Controls.Add(this.m_lblFilterByTags);
            this.Controls.Add(this.m_lbTags);
            this.Controls.Add(m_lblInstructions);
            this.Controls.Add(this.m_chkFilterEnabled);
            this.Controls.Add(m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "SelectionFilterPanel";
            this.Text = "Selection Filter";
            this.Load += new System.EventHandler(this.SelectionFilterPanel_Load);
            this.m_gbFilterByName.ResumeLayout(false);
            this.m_gbFilterByName.PerformLayout();
            this.m_menuLayersContext.ResumeLayout(false);
            this.m_menuTagsContext.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox m_chkFilterEnabled;
        private System.Windows.Forms.CheckedListBox m_lbTags;
        private System.Windows.Forms.CheckedListBox m_lbLayers;
        private System.Windows.Forms.TextBox m_tbName;
        private System.Windows.Forms.GroupBox m_gbFilterByName;
        private System.Windows.Forms.RadioButton m_rbComponentName;
        private System.Windows.Forms.RadioButton m_rbTemplateName;
        private System.Windows.Forms.RadioButton m_rbEntityName;
        private System.Windows.Forms.Label m_lblFilterByTags;
        private System.Windows.Forms.Label m_lblFilterByLayers;
        private System.Windows.Forms.Label m_lblName;
        private System.Windows.Forms.ContextMenuStrip m_menuLayersContext;
        private System.Windows.Forms.ToolStripMenuItem m_miUncheckAllLayers;
        private System.Windows.Forms.ContextMenuStrip m_menuTagsContext;
        private System.Windows.Forms.ToolStripMenuItem m_miUncheckAllTags;

    }
}

//END auto generated code