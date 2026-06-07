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
    partial class TagsPanel
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
            System.Windows.Forms.Label m_lblDummy;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TagsPanel));
            this.m_lbTags = new System.Windows.Forms.ListBox();
            m_lblDummy = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_lbTags
            // 
            this.m_lbTags.Dock = System.Windows.Forms.DockStyle.Fill;
            this.m_lbTags.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.m_lbTags.FormattingEnabled = true;
            this.m_lbTags.IntegralHeight = false;
            this.m_lbTags.ItemHeight = 24;
            this.m_lbTags.Location = new System.Drawing.Point(0, 0);
            this.m_lbTags.Name = "m_lbTags";
            this.m_lbTags.Size = new System.Drawing.Size(311, 346);
            this.m_lbTags.Sorted = true;
            this.m_lbTags.TabIndex = 0;
            this.m_lbTags.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.m_lbTags_DrawItem);
            this.m_lbTags.MouseClick += new System.Windows.Forms.MouseEventHandler(this.m_lbTags_MouseClick);
            this.m_lbTags.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.m_lbTags_KeyPress);
            // 
            // m_lblDummy
            // 
            m_lblDummy.Location = new System.Drawing.Point(132, 80);
            m_lblDummy.Name = "m_lblDummy";
            m_lblDummy.Size = new System.Drawing.Size(8, 8);
            m_lblDummy.TabIndex = 8;
            // 
            // TagsPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(311, 346);
            this.Controls.Add(this.m_lbTags);
            this.Controls.Add(m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "TagsPanel";
            this.Text = "Tags";
            this.Load += new System.EventHandler(this.TagsPanel_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox m_lbTags;
    }
}
//END auto generated code
