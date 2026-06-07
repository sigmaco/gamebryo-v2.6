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
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Controls
{
    /// <summary>
    /// Summary description for PaletteTreeView.
    /// </summary>
    public class PaletteTreeView : System.Windows.Forms.TreeView
    {
        #region Private Data
        private bool m_bDisplayTemplates;
        private MPalette[] m_palettes;
        private MPalette m_selectedPalette;
        private string m_selectedCategory;
        #endregion
        private System.Windows.Forms.ImageList m_ilSmallIcons;
        private System.ComponentModel.IContainer components;


        public PaletteTreeView()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

        }

        public bool DisplayTemplates
        {
            get { return m_bDisplayTemplates; }
            set { m_bDisplayTemplates = value; }
        }

        public MPalette[] Palettes
        {
            get { return m_palettes; } 
            set
            {
                m_palettes = value;
                RefreshUI();
            }
        }

        public MPalette SelectedPalette
        {
            get
            { return m_selectedPalette; }
            set
            { m_selectedPalette = value; }
        }

        public string SelectedCategory
        {
            get
            { return m_selectedCategory; }
            set
            { m_selectedCategory = value; }
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

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(PaletteTreeView));
            this.m_ilSmallIcons = new System.Windows.Forms.ImageList(this.components);
            // 
            // m_ilSmallIcons
            // 
            this.m_ilSmallIcons.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
            this.m_ilSmallIcons.ImageSize = new System.Drawing.Size(16, 16);
            this.m_ilSmallIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_ilSmallIcons.ImageStream")));
            this.m_ilSmallIcons.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // PaletteTreeView
            // 
            this.ImageIndex = 0;
            this.ImageList = this.m_ilSmallIcons;
            this.SelectedImageIndex = 0;
            this.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.PaletteTreeView_AfterSelect);

        }
        #endregion


        #region Helper Methods
        private void RefreshUI()
        {
            this.BeginUpdate();
            this.Nodes.Clear();
            if (m_palettes != null)
            {
                foreach(MPalette palette in m_palettes)
                {
                    TreeNode node = Nodes.Add(palette.Name);
                    if (palette == m_selectedPalette && 
                        m_selectedCategory.Equals(string.Empty))
                    {
                        this.SelectedNode = node;
                    }
                    node.ImageIndex = 0;
                    string[] paletteRootCategories = 
                        PaletteUtilities.BuildSubcategories(palette, "");
                    BuildCategoryNodes(node, palette, paletteRootCategories);                    
                }
            }
            this.EndUpdate();
        }

        private void BuildCategoryNodes(TreeNode parentNode, MPalette palette, 
            string[] categoryNodes)
        {            
            foreach (string category in categoryNodes)
            {
                TreeNode categoryNode = parentNode.Nodes.Add(
                    PaletteUtilities.SimpleName(category));
                categoryNode.ImageIndex = 1;
                categoryNode.SelectedImageIndex = 1;
                string[] subcategories = PaletteUtilities.BuildSubcategories(
                    palette, category);
                if (subcategories.Length > 0)
                {
                    BuildCategoryNodes(categoryNode,  palette, subcategories);
                }
                if (category.Equals(m_selectedCategory))
                {
                    categoryNode.Expand();
                    this.SelectedNode = categoryNode;
                }
            }
        }
        #endregion

        private void PaletteTreeView_AfterSelect(object sender,
            System.Windows.Forms.TreeViewEventArgs e)
        {
            //Find root node and build Category List
            TreeNode topmostNode = e.Node;
            ArrayList categories = new ArrayList();
            while (topmostNode.Parent != null)
            {
                categories.Insert(0,topmostNode.Text);
                topmostNode = topmostNode.Parent;
            }
            foreach(MPalette palette in m_palettes)
            {
                if (palette.Name.Equals(topmostNode.Text))
                {
                    m_selectedPalette = palette;
                }
            }
            string fullCategory = PaletteUtilities.FullCategoryName(
                categories.ToArray(typeof(string)) as string[]);
            m_selectedCategory = fullCategory;
        }
    }
}
