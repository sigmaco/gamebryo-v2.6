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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors
{
    /// <summary>
    /// Summary description for EntityAddRemoveDialog.
    /// </summary>
    public class EntityCollectionEditorDialog : System.Windows.Forms.Form
    {
        private System.Windows.Forms.ListBox m_lbEntitiesInScene;
        private System.Windows.Forms.Label m_lblEntitiesInScene;
        private System.Windows.Forms.Button m_btnAdd;
        private System.Windows.Forms.Button m_btnRemove;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Label m_lblEntitiesInCollection;
        private System.Windows.Forms.ListBox m_lbEntitiesInCollection;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public EntityCollectionEditorDialog(MEntity[] amEntityCollection,
            MPropertyContainer pmSourceEntity)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            if (amEntityCollection != null)
            {
                foreach (MEntity pmEntity in amEntityCollection)
                {
                    m_pmEntityCollection.Add(pmEntity);
                }
            }
            m_pmSourcePropertyContainer = pmSourceEntity;
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(EntityCollectionEditorDialog));
            this.m_lbEntitiesInScene = new System.Windows.Forms.ListBox();
            this.m_lblEntitiesInScene = new System.Windows.Forms.Label();
            this.m_lblEntitiesInCollection = new System.Windows.Forms.Label();
            this.m_btnAdd = new System.Windows.Forms.Button();
            this.m_btnRemove = new System.Windows.Forms.Button();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_lbEntitiesInCollection = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // m_lbEntitiesInScene
            // 
            this.m_lbEntitiesInScene.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lbEntitiesInScene.IntegralHeight = false;
            this.m_lbEntitiesInScene.Location = new System.Drawing.Point(8, 32);
            this.m_lbEntitiesInScene.Name = "m_lbEntitiesInScene";
            this.m_lbEntitiesInScene.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.m_lbEntitiesInScene.Size = new System.Drawing.Size(224, 248);
            this.m_lbEntitiesInScene.Sorted = true;
            this.m_lbEntitiesInScene.TabIndex = 1;
            this.m_lbEntitiesInScene.DoubleClick += new System.EventHandler(this.m_lbEntitiesInScene_DoubleClick);
            this.m_lbEntitiesInScene.SelectedIndexChanged += new System.EventHandler(this.m_lbEntitiesInScene_SelectedIndexChanged);
            // 
            // m_lblEntitiesInScene
            // 
            this.m_lblEntitiesInScene.AutoSize = true;
            this.m_lblEntitiesInScene.Location = new System.Drawing.Point(8, 8);
            this.m_lblEntitiesInScene.Name = "m_lblEntitiesInScene";
            this.m_lblEntitiesInScene.Size = new System.Drawing.Size(92, 16);
            this.m_lblEntitiesInScene.TabIndex = 0;
            this.m_lblEntitiesInScene.Text = "Entities in Scene:";
            // 
            // m_lblEntitiesInCollection
            // 
            this.m_lblEntitiesInCollection.AutoSize = true;
            this.m_lblEntitiesInCollection.Location = new System.Drawing.Point(320, 8);
            this.m_lblEntitiesInCollection.Name = "m_lblEntitiesInCollection";
            this.m_lblEntitiesInCollection.Size = new System.Drawing.Size(109, 16);
            this.m_lblEntitiesInCollection.TabIndex = 3;
            this.m_lblEntitiesInCollection.Text = "Entities in Collection:";
            // 
            // m_btnAdd
            // 
            this.m_btnAdd.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnAdd.Location = new System.Drawing.Point(241, 128);
            this.m_btnAdd.Name = "m_btnAdd";
            this.m_btnAdd.Size = new System.Drawing.Size(72, 23);
            this.m_btnAdd.TabIndex = 2;
            this.m_btnAdd.Text = "Add -->";
            this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
            // 
            // m_btnRemove
            // 
            this.m_btnRemove.Anchor = System.Windows.Forms.AnchorStyles.Left;
            this.m_btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnRemove.Location = new System.Drawing.Point(241, 169);
            this.m_btnRemove.Name = "m_btnRemove";
            this.m_btnRemove.Size = new System.Drawing.Size(72, 23);
            this.m_btnRemove.TabIndex = 5;
            this.m_btnRemove.Text = "<-- Remove";
            this.m_btnRemove.Click += new System.EventHandler(this.m_btnRemove_Click);
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(192, 290);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 6;
            this.m_btnOK.Text = "OK";
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(288, 290);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 7;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_lbEntitiesInCollection
            // 
            this.m_lbEntitiesInCollection.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lbEntitiesInCollection.IntegralHeight = false;
            this.m_lbEntitiesInCollection.Location = new System.Drawing.Point(320, 32);
            this.m_lbEntitiesInCollection.Name = "m_lbEntitiesInCollection";
            this.m_lbEntitiesInCollection.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.m_lbEntitiesInCollection.Size = new System.Drawing.Size(224, 248);
            this.m_lbEntitiesInCollection.Sorted = true;
            this.m_lbEntitiesInCollection.TabIndex = 4;
            this.m_lbEntitiesInCollection.DoubleClick += new System.EventHandler(this.m_lbEntitiesInCollection_DoubleClick);
            this.m_lbEntitiesInCollection.SelectedIndexChanged += new System.EventHandler(this.m_lbEntitiesInCollection_SelectedIndexChanged);
            // 
            // EntityCollectionEditorDialog
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(554, 320);
            this.Controls.Add(this.m_lbEntitiesInCollection);
            this.Controls.Add(this.m_lbEntitiesInScene);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_btnRemove);
            this.Controls.Add(this.m_btnAdd);
            this.Controls.Add(this.m_lblEntitiesInCollection);
            this.Controls.Add(this.m_lblEntitiesInScene);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "EntityCollectionEditorDialog";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Entity Collection Editor";
            this.Load += new System.EventHandler(this.EntityAddRemoveDialog_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private ArrayList m_pmEntityCollection = new ArrayList();
        private MPropertyContainer m_pmSourcePropertyContainer;

        public MEntity[] GetEntityCollection()
        {
            return (MEntity[]) m_pmEntityCollection.ToArray(typeof(MEntity));
        }

        private void EntityAddRemoveDialog_Load(object sender,
            System.EventArgs e)
        {
            PopulateSceneListBox();
            PopulateCollectionListBox();
            UpdateButtonStates();
        }

        private void PopulateSceneListBox()
        {
            ListBox.SelectedObjectCollection pmSelectedItems =
                m_lbEntitiesInScene.SelectedItems;

            m_lbEntitiesInScene.Items.Clear();

            MEntity[] amSceneEntities = MFramework.Instance.Scene
                .GetEntities();
            foreach (MEntity pmEntity in amSceneEntities)
            {
                if (!m_pmEntityCollection.Contains(pmEntity) &&
                    m_pmSourcePropertyContainer != pmEntity)
                {
                    m_lbEntitiesInScene.Items.Add(pmEntity);
                }
            }

            foreach (object pmObject in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInScene.Items.IndexOf(pmObject);
                if (iIndex > 0)
                {
                    m_lbEntitiesInScene.SetSelected(iIndex, true);
                }
            }
            if (m_lbEntitiesInScene.SelectedIndex == -1 &&
                m_lbEntitiesInScene.Items.Count > 0)
            {
                m_lbEntitiesInScene.SelectedIndex = 0;
            }
        }

        private void PopulateCollectionListBox()
        {
            ListBox.SelectedObjectCollection pmSelectedItems =
                m_lbEntitiesInCollection.SelectedItems;

            m_lbEntitiesInCollection.Items.Clear();

            foreach (MEntity pmEntity in m_pmEntityCollection)
            {
                m_lbEntitiesInCollection.Items.Add(pmEntity);
            }

            foreach (object pmObject in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInCollection.Items.IndexOf(pmObject);
                if (iIndex > 0)
                {
                    m_lbEntitiesInCollection.SetSelected(iIndex, true);
                }
            }
            if (m_lbEntitiesInCollection.SelectedIndex == -1 &&
                m_lbEntitiesInCollection.Items.Count > 0)
            {
                m_lbEntitiesInCollection.SelectedIndex = 0;
            }
        }

        private void UpdateButtonStates()
        {
            m_btnAdd.Enabled = (m_lbEntitiesInScene.SelectedIndex > -1);
            m_btnRemove.Enabled = (m_lbEntitiesInCollection.SelectedIndex >
                -1);
        }

        private void Do_Add()
        {
            ArrayList pmSelectedItems = new ArrayList(
                m_lbEntitiesInScene.SelectedItems);

            foreach (object pmSelectedItem in pmSelectedItems)
            {
                m_pmEntityCollection.Add(pmSelectedItem);
                m_lbEntitiesInCollection.Items.Add(pmSelectedItem);
                m_lbEntitiesInScene.Items.Remove(pmSelectedItem);
            }

            // Select entities we just added in collection list box.
            m_lbEntitiesInCollection.SelectedItem = null;
            foreach (object pmSelectedItem in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInCollection.Items.IndexOf(
                    pmSelectedItem);
                if (iIndex > 0)
                {
                    m_lbEntitiesInCollection.SetSelected(iIndex, true);
                }
            }

            UpdateButtonStates();
        }

        private void Do_Remove()
        {
            ArrayList pmSelectedItems = new ArrayList(
                m_lbEntitiesInCollection.SelectedItems);

            foreach (object pmSelectedItem in pmSelectedItems)
            {
                m_pmEntityCollection.Remove(pmSelectedItem);
                m_lbEntitiesInCollection.Items.Remove(pmSelectedItem);
                m_lbEntitiesInScene.Items.Add(pmSelectedItem);
            }

            // Select entities we just removed in the scene list box.
            m_lbEntitiesInScene.SelectedItem = null;
            foreach (object pmSelectedItem in pmSelectedItems)
            {
                int iIndex = m_lbEntitiesInScene.Items.IndexOf(
                    pmSelectedItem);
                if (iIndex > 0)
                {
                    m_lbEntitiesInScene.SetSelected(iIndex, true);
                }
            }

            UpdateButtonStates();
        }

        private void m_btnAdd_Click(object sender, System.EventArgs e)
        {
            Do_Add();
        }

        private void m_btnRemove_Click(object sender, System.EventArgs e)
        {
            Do_Remove();
        }

        private void m_lbEntitiesInScene_DoubleClick(object sender,
            System.EventArgs e)
        {
            Do_Add();
        }

        private void m_lbEntitiesInCollection_DoubleClick(object sender,
            System.EventArgs e)
        {
            Do_Remove();
        }

        private void m_lbEntitiesInScene_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            UpdateButtonStates();
        }

        private void m_lbEntitiesInCollection_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            UpdateButtonStates();
        }
    }
}
