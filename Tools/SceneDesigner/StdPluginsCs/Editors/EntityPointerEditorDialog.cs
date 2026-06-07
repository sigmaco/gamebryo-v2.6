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
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors
{
    /// <summary>
    /// Summary description for EntityPointerEditorDialog.
    /// </summary>
    public class EntityPointerEditorDialog : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Label m_lblChooseSceneEntity;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.ListBox m_lbSceneEntities;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public EntityPointerEditorDialog()
        {
            //
            // Required for Windows Form Designer support
            //
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(EntityPointerEditorDialog));
            this.m_lblChooseSceneEntity = new System.Windows.Forms.Label();
            this.m_lbSceneEntities = new System.Windows.Forms.ListBox();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_lblChooseSceneEntity
            // 
            this.m_lblChooseSceneEntity.AutoSize = true;
            this.m_lblChooseSceneEntity.Location = new System.Drawing.Point(8, 8);
            this.m_lblChooseSceneEntity.Name = "m_lblChooseSceneEntity";
            this.m_lblChooseSceneEntity.Size = new System.Drawing.Size(113, 16);
            this.m_lblChooseSceneEntity.TabIndex = 0;
            this.m_lblChooseSceneEntity.Text = "Choose Scene Entity:";
            // 
            // m_lbSceneEntities
            // 
            this.m_lbSceneEntities.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbSceneEntities.IntegralHeight = false;
            this.m_lbSceneEntities.Location = new System.Drawing.Point(8, 32);
            this.m_lbSceneEntities.Name = "m_lbSceneEntities";
            this.m_lbSceneEntities.Size = new System.Drawing.Size(336, 255);
            this.m_lbSceneEntities.Sorted = true;
            this.m_lbSceneEntities.TabIndex = 1;
            this.m_lbSceneEntities.DoubleClick += new System.EventHandler(this.m_lbSceneEntities_DoubleClick);
            this.m_lbSceneEntities.SelectedIndexChanged += new System.EventHandler(this.m_lbSceneEntities_SelectedIndexChanged);
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(96, 296);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 2;
            this.m_btnOK.Text = "OK";
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(181, 296);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 3;
            this.m_btnCancel.Text = "Cancel";
            // 
            // EntityPointerEditorDialog
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(352, 326);
            this.Controls.Add(this.m_lbSceneEntities);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_lblChooseSceneEntity);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(192, 176);
            this.Name = "EntityPointerEditorDialog";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Scene Entity Selector";
            this.Load += new System.EventHandler(this.EntityPointerEditorDialog_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private MEntity m_pmSelectedEntity;
        public MEntity SelectedEntity
        {
            get
            {
                return m_pmSelectedEntity;
            }
            set
            {
                m_pmSelectedEntity = value;
            }
        }

        private MEntity m_pmActiveEntity;
        public MEntity ActiveEntity
        {
            get
            {
                return m_pmActiveEntity;
            }
            set
            {
                m_pmActiveEntity = value;
            }
        }

        private void EntityPointerEditorDialog_Load(object sender,
            System.EventArgs e)
        {
            m_lbSceneEntities.Items.Clear();
            m_lbSceneEntities.Items.Add("<None>");

            MEntity[] amSceneEntities = MFramework.Instance.Scene
                .GetEntities();
            if (amSceneEntities != null)
            {
                foreach (MEntity pmEntity in amSceneEntities)
                {
                    if (pmEntity != ActiveEntity)
                    {
                        m_lbSceneEntities.Items.Add(pmEntity);
                    }
                }
                if (SelectedEntity != null)
                {
                    int iIndex = m_lbSceneEntities.Items.IndexOf(
                        SelectedEntity);
                    if (iIndex > -1)
                    {
                        m_lbSceneEntities.SelectedIndex = iIndex;
                    }
                    else if (m_lbSceneEntities.Items.Count > 0)
                    {
                        m_lbSceneEntities.SelectedIndex = 0;
                    }
                    else
                    {
                        SelectedEntity = null;
                    }
                }
                else if (m_lbSceneEntities.Items.Count > 0)
                {
                    m_lbSceneEntities.SelectedIndex = 0;
                }
            }
        }

        private void m_lbSceneEntities_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            SelectedEntity = m_lbSceneEntities.SelectedItem as MEntity;
        }

        private void m_lbSceneEntities_DoubleClick(object sender,
            System.EventArgs e)
        {
            SelectedEntity = m_lbSceneEntities.SelectedItem as MEntity;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}
