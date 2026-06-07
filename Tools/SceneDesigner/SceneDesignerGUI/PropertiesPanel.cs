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
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    [DockPosition(Position=DefaultDock.DockLeft)]
    public class PropertiesPanel : Form, ICommandPanel
    {
        private System.Windows.Forms.PropertyGrid m_gridProperties;
        private System.Windows.Forms.Label m_lblDummy;
        private System.Windows.Forms.ContextMenu m_cmContext;
        private System.Windows.Forms.MenuItem m_miReset;
        private System.Windows.Forms.MenuItem m_miMakeUnique;
        private System.ComponentModel.IContainer components = null;

        public PropertiesPanel()
        {
            // This call is required by the Windows Form Designer.
            InitializeComponent();

            this.TopLevel = false;

        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if (components != null) 
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(PropertiesPanel));
            this.m_gridProperties = new System.Windows.Forms.PropertyGrid();
            this.m_cmContext = new System.Windows.Forms.ContextMenu();
            this.m_miReset = new System.Windows.Forms.MenuItem();
            this.m_miMakeUnique = new System.Windows.Forms.MenuItem();
            this.m_lblDummy = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_gridProperties
            // 
            this.m_gridProperties.CommandsVisibleIfAvailable = true;
            this.m_gridProperties.ContextMenu = this.m_cmContext;
            this.m_gridProperties.Dock = System.Windows.Forms.DockStyle.Fill;
            this.m_gridProperties.LargeButtons = false;
            this.m_gridProperties.LineColor = System.Drawing.SystemColors.ScrollBar;
            this.m_gridProperties.Location = new System.Drawing.Point(0, 0);
            this.m_gridProperties.Name = "m_gridProperties";
            this.m_gridProperties.Size = new System.Drawing.Size(392, 382);
            this.m_gridProperties.TabIndex = 0;
            this.m_gridProperties.Text = "PropertyGrid";
            this.m_gridProperties.ViewBackColor = System.Drawing.SystemColors.Window;
            this.m_gridProperties.ViewForeColor = System.Drawing.SystemColors.WindowText;
            // 
            // m_cmContext
            // 
            this.m_cmContext.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
                                                                                        this.m_miReset,
                                                                                        this.m_miMakeUnique});
            this.m_cmContext.Popup += new System.EventHandler(this.m_cmContext_Popup);
            // 
            // m_miReset
            // 
            this.m_miReset.Index = 0;
            this.m_miReset.Text = "&Reset";
            this.m_miReset.Click += new System.EventHandler(this.m_miReset_Click);
            // 
            // m_miMakeUnique
            // 
            this.m_miMakeUnique.Index = 1;
            this.m_miMakeUnique.Text = "Make &Unique";
            this.m_miMakeUnique.Click += new System.EventHandler(this.m_miMakeUnique_Click);
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.Location = new System.Drawing.Point(168, 144);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(8, 8);
            this.m_lblDummy.TabIndex = 1;
            // 
            // PropertiesPanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(392, 382);
            this.Controls.Add(this.m_gridProperties);
            this.Controls.Add(this.m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "PropertiesPanel";
            this.Text = "Entity Properties";
            this.Load += new System.EventHandler(this.PropertiesPanel_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void PropertiesPanel_Load(object sender, System.EventArgs e)
        {
        }

        public void RegisterEventHandlers()
        {
            MFramework.Instance.EventManager.SceneClosing += new
                MEventManager.__Delegate_SceneClosing(
                this.OnSceneClosing);
            MFramework.Instance.EventManager.EntityPropertyChanged += new
                MEventManager.__Delegate_EntityPropertyChanged(
                this.OnEntityPropertyChanged);
            MFramework.Instance.EventManager.SelectedEntitiesChanged += new
                MEventManager.__Delegate_SelectedEntitiesChanged(
                this.OnSelectedEntitiesChanged);
            MFramework.Instance.EventManager.EntityNameChanged += new
                MEventManager.__Delegate_EntityNameChanged(
                this.OnEntityNameChanged);
            MFramework.Instance.EventManager.EntityFrozenStateChanged += new
                MEventManager.__Delegate_EntityFrozenStateChanged(
                this.OnEntityFrozenStateChanged);
            MFramework.Instance.EventManager.EntityComponentAdded += new
                MEventManager.__Delegate_EntityComponentAdded(
                this.OnEntityComponentAddedRemoved);
            MFramework.Instance.EventManager.EntityComponentRemoved += new
                MEventManager.__Delegate_EntityComponentRemoved(
                this.OnEntityComponentAddedRemoved);
            MFramework.Instance.EventManager.ComponentPropertyAdded += new 
                MEventManager.__Delegate_ComponentPropertyAdded(
                this.OnComponentPropertyAddedOrRemoved);
            MFramework.Instance.EventManager.ComponentPropertyRemoved += new 
                MEventManager.__Delegate_ComponentPropertyRemoved(
                this.OnComponentPropertyAddedOrRemoved);
            MFramework.Instance.EventManager.ComponentPropertyChanged += new
                MEventManager.__Delegate_ComponentPropertyChanged(
                this.OnComponentPropertyChanged);
            MFramework.Instance.EventManager.LongOperationCompleted += new
                MEventManager.__Delegate_LongOperationCompleted(
                this.OnLongOperationCompleted);
            MFramework.Instance.EventManager.LayerStatusChanged += new
                MEventManager.__Delegate_LayerStatusChanged(
                this.OnLayerStatusChanged);
            MFramework.Instance.EventManager.LayerDeleted +=
                new MEventManager.__Delegate_LayerDeleted(
                OnLayerDeleted);
            MFramework.Instance.EventManager.LayerFilenameRemoved +=
                new MEventManager.__Delegate_LayerFilenameRemoved(
                OnLayerFilenameRemoved);
            MFramework.Instance.EventManager.EntityTagsChanged += new
                MEventManager.__Delegate_EntityTagsChanged(
                this.OnEntityTagsChanged);
        }

        private ISelectionService ms_pmSelectionService = null;
        private ISelectionService SelectionService
        {
            get
            {
                if (ms_pmSelectionService == null)
                {
                    ms_pmSelectionService = ServiceProvider.Instance
                        .GetService(typeof(ISelectionService)) as
                        ISelectionService;
                    Debug.Assert(ms_pmSelectionService != null,
                        "Selection service not found!");
                }
                return ms_pmSelectionService;
            }
        }

        #region Event Handlers
        private bool m_bRefreshGrid = false;
        private bool m_bUpdateSelectedObjects = false;

        private void OnSceneClosing(MScene pmScene)
        {
            // PropertyGrid keeps around a cache of its last selected object
            // in order to restore its previous tree expansion, if possible.
            // Because entities in the old scene will be deleted, we need to
            // set and then clear some innocuous selected object here to avoid
            // accessing a disposed object.
            m_gridProperties.SelectedObject = string.Empty;
            m_gridProperties.SelectedObject = null;
        }

        private void OnEntityPropertyChanged(MEntity pmEntity,
            string strPropertyName, uint uiPropertyIndex, bool bInBatch)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            foreach(MEntity selectedEntity in
                m_gridProperties.SelectedObjects)
            {
                bool bShouldUpdate = false;
                if (selectedEntity == pmEntity)
                {
                    bShouldUpdate = true;
                }
                MEntity curEntity = selectedEntity.MasterEntity;
                while (!bShouldUpdate && curEntity != null)
                {
                    if (curEntity == pmEntity)
                    {
                        bShouldUpdate = true;
                    }
                    curEntity = curEntity.MasterEntity;
                }
                if (bShouldUpdate)
                {
                    m_gridProperties.Refresh();
                    break;
                }
            }
        }

        private void OnComponentPropertyAddedOrRemoved(MComponent pmComponent, 
            string strPropertyName, bool bInBatch)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            foreach (MEntity pmEntity in m_gridProperties.SelectedObjects)
            {
                MComponent pmEntityComponent = pmEntity
                    .GetComponentByTemplateID(pmComponent.TemplateID);
                if (pmEntityComponent != null)
                {
                    m_gridProperties.Refresh();
                    break;
                }
            }
        }

        private void OnComponentPropertyChanged(MComponent pmComponent, 
            string strPropertyName, uint uiPropertyIndex, bool bInBatch)
        {
            OnComponentPropertyAddedOrRemoved(pmComponent, strPropertyName,
                bInBatch);
        }

        private void OnSelectedEntitiesChanged()
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bUpdateSelectedObjects = true;
                return;
            }

            m_gridProperties.SelectedObjects =
                SelectionService.GetSelectedEntities();
        }

        private void OnEntityNameChanged(MEntity pmEntity, string strOldName,
            bool bInBatch)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            if (!bInBatch && m_gridProperties.SelectedObject == pmEntity)
            {
                m_gridProperties.Refresh();
            }
        }

        private void OnEntityFrozenStateChanged(MEntity pmEntity,
            bool bFrozen)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            foreach (object pmSelectedObject in
                m_gridProperties.SelectedObjects)
            {
                if (pmSelectedObject == pmEntity)
                {
                    m_gridProperties.Refresh();
                    break;
                }
            }
        }

        private void OnEntityComponentAddedRemoved(MEntity pmEntity,
            MComponent pmComponent)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            if (m_gridProperties.SelectedObject == pmEntity)
            {
                m_gridProperties.Refresh();
            }
        }

        private void OnLayerStatusChanged(MLayer pmLayer)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            m_gridProperties.Refresh();
        }

        private void OnLayerDeleted(MLayer pmLayer, bool bFileDeleted)
        {
            // PropertyGrid keeps around a cache of its last selected object
            // in order to restore its previous tree expansion, if possible.
            // Because entities in the old scene will be deleted, we need to
            // set and then clear some innocuous selected object here to avoid
            // accessing a disposed object.
            m_gridProperties.SelectedObject = string.Empty;
            m_gridProperties.SelectedObjects =
                SelectionService.GetSelectedEntities();
        }

        private void OnLayerFilenameRemoved(MLayer pmLayer, String strFilename)
        {
            OnLayerDeleted(pmLayer, false);
        }

        private void OnEntityTagsChanged(MEntity pmEntity, string strOldTags)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bRefreshGrid = true;
                return;
            }

            foreach (MEntity selectedEntity in
                m_gridProperties.SelectedObjects)
            {
                bool bShouldUpdate = false;
                if (selectedEntity == pmEntity)
                {
                    bShouldUpdate = true;
                }
                MEntity curEntity = selectedEntity.MasterEntity;
                while (!bShouldUpdate && curEntity != null)
                {
                    if (curEntity == pmEntity)
                    {
                        bShouldUpdate = true;
                    }
                    curEntity = curEntity.MasterEntity;
                }
                if (bShouldUpdate)
                {
                    m_gridProperties.Refresh();
                    break;
                }
            }
        }

        private void OnLongOperationCompleted()
        {
            if (m_bUpdateSelectedObjects)
            {
                m_gridProperties.SelectedObjects =
                    SelectionService.GetSelectedEntities();
            }

            if (m_bRefreshGrid)
            {
                m_gridProperties.Refresh();
            }

            m_bRefreshGrid = false;
            m_bUpdateSelectedObjects = false;
        }
        #endregion

        private void m_cmContext_Popup(object sender, System.EventArgs e)
        {
            bool bCanResetValue = false;
            bool bCanMakeUnique = false;

            if (m_gridProperties.SelectedGridItem != null)
            {
                MEntityPropertyDescriptor pmPropertyDescriptor =
                    m_gridProperties.SelectedGridItem.PropertyDescriptor as
                    MEntityPropertyDescriptor;
                if (pmPropertyDescriptor != null)
                {
                    bCanResetValue = (
                        pmPropertyDescriptor.PropertyContainer
                            .CanResetProperty(pmPropertyDescriptor
                            .PropertyName) &&
                        pmPropertyDescriptor.PropertyContainer.Writable);
                    bCanMakeUnique = (
                        !pmPropertyDescriptor.PropertyContainer
                            .IsPropertyUnique(pmPropertyDescriptor
                            .PropertyName) &&
                        pmPropertyDescriptor.PropertyContainer.Writable);
                }
            }

            m_miReset.Enabled = bCanResetValue;
            m_miMakeUnique.Enabled = bCanMakeUnique;
        }

        private void m_miReset_Click(object sender, System.EventArgs e)
        {
            m_gridProperties.ResetSelectedProperty();
        }

        private void m_miMakeUnique_Click(object sender, System.EventArgs e)
        {
            MEntityPropertyDescriptor pmDescriptor =
                m_gridProperties.SelectedGridItem.PropertyDescriptor as
                MEntityPropertyDescriptor;
            if (pmDescriptor != null)
            {
                pmDescriptor.PropertyContainer.MakePropertyUnique(
                    pmDescriptor.PropertyName);
            }
        }

    }
}
