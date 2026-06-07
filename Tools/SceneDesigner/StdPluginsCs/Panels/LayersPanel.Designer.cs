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
    partial class LayersPanel
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
            System.Windows.Forms.Timer m_timer;
            System.Windows.Forms.Label m_lblDummy;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LayersPanel));
            this.tree = new System.Windows.Forms.TreeView();
            this.m_mnuContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.m_mnuSetCurrent = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.m_mnuSaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.m_mnuDeleteLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.m_mnuExplicitLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.m_mnuMoveSelected = new System.Windows.Forms.ToolStripMenuItem();
            this.m_mnuSelectAll = new System.Windows.Forms.ToolStripMenuItem();
            this.m_mnuUnhideAll = new System.Windows.Forms.ToolStripMenuItem();
            this.m_mnuUnfreezeAll = new System.Windows.Forms.ToolStripMenuItem();
            this.m_mnuColor = new System.Windows.Forms.ToolStripMenuItem();
            this.StatusIcons = new System.Windows.Forms.ImageList(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.m_btnNewLayerFromSelection = new System.Windows.Forms.Button();
            this.m_btnImportLayer = new System.Windows.Forms.Button();
            this.m_btnNewLayer = new System.Windows.Forms.Button();
            this.m_btnDeleteLayer = new System.Windows.Forms.Button();
            this.m_dlgNewLayer = new System.Windows.Forms.SaveFileDialog();
            this.ttTips = new System.Windows.Forms.ToolTip(this.components);
            this.dlgColor = new System.Windows.Forms.ColorDialog();
            m_timer = new System.Windows.Forms.Timer(this.components);
            m_lblDummy = new System.Windows.Forms.Label();
            this.m_mnuContext.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_timer
            // 
            m_timer.Enabled = true;
            m_timer.Interval = 1000;
            m_timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // m_lblDummy
            // 
            m_lblDummy.Location = new System.Drawing.Point(164, 97);
            m_lblDummy.Name = "m_lblDummy";
            m_lblDummy.Size = new System.Drawing.Size(8, 8);
            m_lblDummy.TabIndex = 9;
            // 
            // tree
            // 
            this.tree.AllowDrop = true;
            this.tree.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tree.ContextMenuStrip = this.m_mnuContext;
            this.tree.DrawMode = System.Windows.Forms.TreeViewDrawMode.OwnerDrawText;
            this.tree.HideSelection = false;
            this.tree.ImageIndex = 0;
            this.tree.ImageList = this.StatusIcons;
            this.tree.ItemHeight = 16;
            this.tree.Location = new System.Drawing.Point(0, 0);
            this.tree.Name = "tree";
            this.tree.SelectedImageIndex = 0;
            this.tree.ShowNodeToolTips = true;
            this.tree.ShowRootLines = false;
            this.tree.Size = new System.Drawing.Size(329, 298);
            this.tree.TabIndex = 1;
            this.tree.QueryContinueDrag += new System.Windows.Forms.QueryContinueDragEventHandler(this.tree_QueryContinueDrag);
            this.tree.MouseClick += new System.Windows.Forms.MouseEventHandler(this.tree_MouseClick);
            this.tree.DrawNode += new System.Windows.Forms.DrawTreeNodeEventHandler(this.treeView1_DrawNode);
            this.tree.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.tree_BeforeCollapse);
            this.tree.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeView1_DragDrop);
            this.tree.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeView1_DragEnter);
            this.tree.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tree_KeyPress);
            this.tree.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeView1_ItemDrag);
            this.tree.DragOver += new System.Windows.Forms.DragEventHandler(this.treeView1_DragOver);
            // 
            // m_mnuContext
            // 
            this.m_mnuContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_mnuSetCurrent,
            this.toolStripMenuItem1,
            this.m_mnuSaveAs,
            this.m_mnuDeleteLayer,
            this.m_mnuExplicitLayer,
            this.toolStripMenuItem2,
            this.m_mnuMoveSelected,
            this.m_mnuSelectAll,
            this.m_mnuUnhideAll,
            this.m_mnuUnfreezeAll,
            this.m_mnuColor});
            this.m_mnuContext.Name = "m_mnuContext";
            this.m_mnuContext.ShowImageMargin = false;
            this.m_mnuContext.Size = new System.Drawing.Size(119, 214);
            this.m_mnuContext.Text = "&Set as current";
            this.m_mnuContext.Opening += new System.ComponentModel.CancelEventHandler(this.m_mnuContext_Opening);
            // 
            // m_mnuSetCurrent
            // 
            this.m_mnuSetCurrent.Name = "m_mnuSetCurrent";
            this.m_mnuSetCurrent.Size = new System.Drawing.Size(118, 22);
            this.m_mnuSetCurrent.Text = "Set as &current";
            this.m_mnuSetCurrent.ToolTipText = "Set the selected layer as the current working layer";
            this.m_mnuSetCurrent.Click += new System.EventHandler(this.m_mnuSetCurrent_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(115, 6);
            // 
            // m_mnuSaveAs
            // 
            this.m_mnuSaveAs.Name = "m_mnuSaveAs";
            this.m_mnuSaveAs.Size = new System.Drawing.Size(118, 22);
            this.m_mnuSaveAs.Text = "&Rename...";
            this.m_mnuSaveAs.Click += new System.EventHandler(this.m_mnuSaveAs_Click);
            // 
            // m_mnuDeleteLayer
            // 
            this.m_mnuDeleteLayer.Name = "m_mnuDeleteLayer";
            this.m_mnuDeleteLayer.Size = new System.Drawing.Size(118, 22);
            this.m_mnuDeleteLayer.Text = "&Remove layer";
            this.m_mnuDeleteLayer.ToolTipText = "Removes this layer from the scene";
            this.m_mnuDeleteLayer.Click += new System.EventHandler(this.DeleteLayer_Click);
            // 
            // m_mnuExplicitLayer
            // 
            this.m_mnuExplicitLayer.Name = "m_mnuExplicitLayer";
            this.m_mnuExplicitLayer.Size = new System.Drawing.Size(118, 22);
            this.m_mnuExplicitLayer.Text = "Make explicit";
            this.m_mnuExplicitLayer.ToolTipText = "Make implicit layers explicit or explicit layers implicit";
            this.m_mnuExplicitLayer.Click += new System.EventHandler(this.m_mnuExplicitLayer_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(115, 6);
            // 
            // m_mnuMoveSelected
            // 
            this.m_mnuMoveSelected.Name = "m_mnuMoveSelected";
            this.m_mnuMoveSelected.Size = new System.Drawing.Size(118, 22);
            this.m_mnuMoveSelected.Text = "&Move selected";
            this.m_mnuMoveSelected.ToolTipText = "Move the selected entities into this layer";
            this.m_mnuMoveSelected.Click += new System.EventHandler(this.m_mnuMoveSelected_Click);
            // 
            // m_mnuSelectAll
            // 
            this.m_mnuSelectAll.Name = "m_mnuSelectAll";
            this.m_mnuSelectAll.Size = new System.Drawing.Size(118, 22);
            this.m_mnuSelectAll.Text = "Select a&ll";
            this.m_mnuSelectAll.ToolTipText = "Select all entities in this layer";
            this.m_mnuSelectAll.Click += new System.EventHandler(this.m_mnuSelectAll_Click);
            // 
            // m_mnuUnhideAll
            // 
            this.m_mnuUnhideAll.Name = "m_mnuUnhideAll";
            this.m_mnuUnhideAll.Size = new System.Drawing.Size(118, 22);
            this.m_mnuUnhideAll.Text = "Un&hide all";
            this.m_mnuUnhideAll.ToolTipText = "Shows all entities in the selected layers";
            this.m_mnuUnhideAll.Click += new System.EventHandler(this.m_mnuUnhideAll_Click);
            // 
            // m_mnuUnfreezeAll
            // 
            this.m_mnuUnfreezeAll.Name = "m_mnuUnfreezeAll";
            this.m_mnuUnfreezeAll.Size = new System.Drawing.Size(118, 22);
            this.m_mnuUnfreezeAll.Text = "Un&freeze all";
            this.m_mnuUnfreezeAll.ToolTipText = "Unfreezes all entities in the selected layers";
            this.m_mnuUnfreezeAll.Click += new System.EventHandler(this.m_mnuUnfreezeAll_Click);
            // 
            // m_mnuColor
            // 
            this.m_mnuColor.Name = "m_mnuColor";
            this.m_mnuColor.Size = new System.Drawing.Size(118, 22);
            this.m_mnuColor.Text = "&Color";
            this.m_mnuColor.Click += new System.EventHandler(this.m_mnuColor_Click);
            // 
            // StatusIcons
            // 
            this.StatusIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("StatusIcons.ImageStream")));
            this.StatusIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.StatusIcons.Images.SetKeyName(0, "Current.png");
            this.StatusIcons.Images.SetKeyName(1, "Readonly.png");
            this.StatusIcons.Images.SetKeyName(2, "Writable.png");
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.m_btnNewLayerFromSelection);
            this.panel1.Controls.Add(this.m_btnImportLayer);
            this.panel1.Controls.Add(this.m_btnNewLayer);
            this.panel1.Controls.Add(this.m_btnDeleteLayer);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 301);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(329, 42);
            this.panel1.TabIndex = 7;
            // 
            // m_btnNewLayerFromSelection
            // 
            this.m_btnNewLayerFromSelection.FlatAppearance.BorderSize = 0;
            this.m_btnNewLayerFromSelection.Image = ((System.Drawing.Image)(resources.GetObject("m_btnNewLayerFromSelection.Image")));
            this.m_btnNewLayerFromSelection.Location = new System.Drawing.Point(49, 3);
            this.m_btnNewLayerFromSelection.Name = "m_btnNewLayerFromSelection";
            this.m_btnNewLayerFromSelection.Size = new System.Drawing.Size(36, 36);
            this.m_btnNewLayerFromSelection.TabIndex = 11;
            this.ttTips.SetToolTip(this.m_btnNewLayerFromSelection, "Create new layer from selected items");
            this.m_btnNewLayerFromSelection.UseVisualStyleBackColor = true;
            this.m_btnNewLayerFromSelection.Click += new System.EventHandler(this.m_btnNewLayerFromSelection_Click);
            // 
            // m_btnImportLayer
            // 
            this.m_btnImportLayer.FlatAppearance.BorderSize = 0;
            this.m_btnImportLayer.Image = ((System.Drawing.Image)(resources.GetObject("m_btnImportLayer.Image")));
            this.m_btnImportLayer.Location = new System.Drawing.Point(7, 3);
            this.m_btnImportLayer.Name = "m_btnImportLayer";
            this.m_btnImportLayer.Size = new System.Drawing.Size(36, 36);
            this.m_btnImportLayer.TabIndex = 10;
            this.ttTips.SetToolTip(this.m_btnImportLayer, "Import new layer");
            this.m_btnImportLayer.UseVisualStyleBackColor = true;
            this.m_btnImportLayer.Click += new System.EventHandler(this.m_btnImportLayer_Click);
            // 
            // m_btnNewLayer
            // 
            this.m_btnNewLayer.FlatAppearance.BorderSize = 0;
            this.m_btnNewLayer.Image = ((System.Drawing.Image)(resources.GetObject("m_btnNewLayer.Image")));
            this.m_btnNewLayer.Location = new System.Drawing.Point(91, 3);
            this.m_btnNewLayer.Name = "m_btnNewLayer";
            this.m_btnNewLayer.Size = new System.Drawing.Size(36, 36);
            this.m_btnNewLayer.TabIndex = 8;
            this.ttTips.SetToolTip(this.m_btnNewLayer, "Create new empty layer");
            this.m_btnNewLayer.UseVisualStyleBackColor = true;
            this.m_btnNewLayer.Click += new System.EventHandler(this.btnNewLayer_Click);
            // 
            // m_btnDeleteLayer
            // 
            this.m_btnDeleteLayer.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.m_btnDeleteLayer.FlatAppearance.BorderSize = 0;
            this.m_btnDeleteLayer.Image = ((System.Drawing.Image)(resources.GetObject("m_btnDeleteLayer.Image")));
            this.m_btnDeleteLayer.Location = new System.Drawing.Point(133, 3);
            this.m_btnDeleteLayer.Name = "m_btnDeleteLayer";
            this.m_btnDeleteLayer.Size = new System.Drawing.Size(36, 36);
            this.m_btnDeleteLayer.TabIndex = 7;
            this.ttTips.SetToolTip(this.m_btnDeleteLayer, "Remove selected layers");
            this.m_btnDeleteLayer.UseVisualStyleBackColor = true;
            this.m_btnDeleteLayer.Click += new System.EventHandler(this.DeleteLayer_Click);
            // 
            // m_dlgNewLayer
            // 
            this.m_dlgNewLayer.Filter = "(*.gsa)|*.gsa";
            this.m_dlgNewLayer.OverwritePrompt = false;
            this.m_dlgNewLayer.Title = "New layer name";
            // 
            // LayersPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(329, 343);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.tree);
            this.Controls.Add(m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "LayersPanel";
            this.Text = "Layers";
            this.Load += new System.EventHandler(this.LayersPanel_Load);
            this.m_mnuContext.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView tree;
        private System.Windows.Forms.ImageList StatusIcons;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button m_btnNewLayerFromSelection;
        private System.Windows.Forms.Button m_btnImportLayer;
        private System.Windows.Forms.Button m_btnNewLayer;
        private System.Windows.Forms.Button m_btnDeleteLayer;
        private System.Windows.Forms.SaveFileDialog m_dlgNewLayer;
        private System.Windows.Forms.ContextMenuStrip m_mnuContext;
        private System.Windows.Forms.ToolStripMenuItem m_mnuSetCurrent;
        private System.Windows.Forms.ToolStripMenuItem m_mnuMoveSelected;
        private System.Windows.Forms.ToolStripMenuItem m_mnuSelectAll;
        private System.Windows.Forms.ToolStripMenuItem m_mnuDeleteLayer;
        private System.Windows.Forms.ToolTip ttTips;
        private System.Windows.Forms.ToolStripMenuItem m_mnuSaveAs;
        private System.Windows.Forms.ToolStripMenuItem m_mnuExplicitLayer;
        private System.Windows.Forms.ToolStripMenuItem m_mnuUnhideAll;
        private System.Windows.Forms.ToolStripMenuItem m_mnuUnfreezeAll;
        private System.Windows.Forms.ToolStripMenuItem m_mnuColor;
        private System.Windows.Forms.ColorDialog dlgColor;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
    }
}

//END auto generated code