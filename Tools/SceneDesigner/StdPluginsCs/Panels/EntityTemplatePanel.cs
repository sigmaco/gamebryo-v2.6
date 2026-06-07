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
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    public class EntityTemplatePanel : Form
    {

        #region Private Data
        private IComponentService m_componentService;
        private IEntityPathService m_entityPathService;
        private bool m_bDoRefreshUI = false;
        #endregion
        private System.Windows.Forms.Label m_lblSelectedTemplateHeading;
        private System.Windows.Forms.Label m_lblSelectedTemplate;
        private System.Windows.Forms.Button m_btnEditTemplate;
        private System.Windows.Forms.Button m_btnCreateNewTemplate;
        private System.Windows.Forms.Label m_lblDummy;
        private System.Windows.Forms.Label m_lblPropertySets;
        private System.Windows.Forms.PropertyGrid m_gridProperties;
        private System.Windows.Forms.Panel panel1;
        private System.ComponentModel.IContainer components = null;
        private Font m_fontNormal;
        private Font m_fontBold;

        public EntityTemplatePanel()
        {
            // This call is required by the Windows Form Designer.
            InitializeComponent();

            IUICommandService uiCommandService = 
                ServiceProvider.Instance.GetService(
                    typeof(IUICommandService)) as IUICommandService;

            uiCommandService.BindCommands(this);
        }

        private void OnSceneClosing(MScene scene)
        {
            // PropertyGrid keeps around a cache of its last selected object
            // in order to restore its previous tree expansion, if possible.
            // Because entities in the old scene will be deleted, we need to
            // set and then clear some innocuous selected object here to avoid
            // accessing a disposed object.
            m_gridProperties.SelectedObject = string.Empty;
            m_gridProperties.SelectedObject = null;
        }

        private void OnLongOperationCompleted()
        {
            if (m_bDoRefreshUI)
            {
                RefreshUI();
                m_bDoRefreshUI = false;
            }
        }

        private MFramework FW
        {
            get { return MFramework.Instance; }
        }

        private IComponentService ComponentService
        {
            get
            {
                if (m_componentService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_componentService = 
                        sp.GetService(typeof(IComponentService))
                        as IComponentService;
                }
                return m_componentService;
            }
        }

        private IEntityPathService EntityPathService
        {
            get
            {
                if (m_entityPathService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_entityPathService = 
                        sp.GetService(typeof(IEntityPathService))
                        as IEntityPathService;
                }
                return m_entityPathService;
            }
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(EntityTemplatePanel));
            this.m_lblSelectedTemplateHeading = new System.Windows.Forms.Label();
            this.m_lblSelectedTemplate = new System.Windows.Forms.Label();
            this.m_lblPropertySets = new System.Windows.Forms.Label();
            this.m_btnEditTemplate = new System.Windows.Forms.Button();
            this.m_btnCreateNewTemplate = new System.Windows.Forms.Button();
            this.m_lblDummy = new System.Windows.Forms.Label();
            this.m_gridProperties = new System.Windows.Forms.PropertyGrid();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_lblSelectedTemplateHeading
            // 
            this.m_lblSelectedTemplateHeading.AutoSize = true;
            this.m_lblSelectedTemplateHeading.Location = new System.Drawing.Point(8, 8);
            this.m_lblSelectedTemplateHeading.Name = "m_lblSelectedTemplateHeading";
            this.m_lblSelectedTemplateHeading.Size = new System.Drawing.Size(102, 16);
            this.m_lblSelectedTemplateHeading.TabIndex = 0;
            this.m_lblSelectedTemplateHeading.Text = "Selected Template:";
            // 
            // m_lblSelectedTemplate
            // 
            this.m_lblSelectedTemplate.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lblSelectedTemplate.Location = new System.Drawing.Point(8, 24);
            this.m_lblSelectedTemplate.Name = "m_lblSelectedTemplate";
            this.m_lblSelectedTemplate.Size = new System.Drawing.Size(408, 24);
            this.m_lblSelectedTemplate.TabIndex = 1;
            // 
            // m_lblPropertySets
            // 
            this.m_lblPropertySets.AutoSize = true;
            this.m_lblPropertySets.Location = new System.Drawing.Point(8, 48);
            this.m_lblPropertySets.Name = "m_lblPropertySets";
            this.m_lblPropertySets.Size = new System.Drawing.Size(109, 16);
            this.m_lblPropertySets.TabIndex = 2;
            this.m_lblPropertySets.Text = "Template Properties:";
            // 
            // m_btnEditTemplate
            // 
            this.m_btnEditTemplate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_btnEditTemplate.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnEditTemplate.Location = new System.Drawing.Point(8, 416);
            this.m_btnEditTemplate.Name = "m_btnEditTemplate";
            this.m_btnEditTemplate.Size = new System.Drawing.Size(48, 23);
            this.m_btnEditTemplate.TabIndex = 3;
            this.m_btnEditTemplate.Text = "Edit";
            this.m_btnEditTemplate.Click += new System.EventHandler(this.m_btnEditTemplate_Click);
            // 
            // m_btnCreateNewTemplate
            // 
            this.m_btnCreateNewTemplate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_btnCreateNewTemplate.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCreateNewTemplate.Location = new System.Drawing.Point(368, 416);
            this.m_btnCreateNewTemplate.Name = "m_btnCreateNewTemplate";
            this.m_btnCreateNewTemplate.Size = new System.Drawing.Size(48, 23);
            this.m_btnCreateNewTemplate.TabIndex = 4;
            this.m_btnCreateNewTemplate.Text = "New";
            this.m_btnCreateNewTemplate.Click += new System.EventHandler(this.m_btnCreateNewTemplate_Click);
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.BackColor = System.Drawing.SystemColors.Control;
            this.m_lblDummy.Location = new System.Drawing.Point(112, 216);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(8, 8);
            this.m_lblDummy.TabIndex = 5;
            // 
            // m_gridProperties
            // 
            this.m_gridProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_gridProperties.CommandsVisibleIfAvailable = true;
            this.m_gridProperties.LargeButtons = false;
            this.m_gridProperties.LineColor = System.Drawing.SystemColors.ScrollBar;
            this.m_gridProperties.Location = new System.Drawing.Point(0, 0);
            this.m_gridProperties.Name = "m_gridProperties";
            this.m_gridProperties.Size = new System.Drawing.Size(408, 344);
            this.m_gridProperties.TabIndex = 0;
            this.m_gridProperties.Text = "m_gridProperties";
            this.m_gridProperties.ViewBackColor = System.Drawing.SystemColors.Window;
            this.m_gridProperties.ViewForeColor = System.Drawing.SystemColors.WindowText;
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.m_gridProperties);
            this.panel1.Location = new System.Drawing.Point(8, 64);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(408, 344);
            this.panel1.TabIndex = 8;
            // 
            // EntityTemplatePanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(424, 446);
            this.Controls.Add(this.m_btnCreateNewTemplate);
            this.Controls.Add(this.m_btnEditTemplate);
            this.Controls.Add(this.m_lblPropertySets);
            this.Controls.Add(this.m_lblSelectedTemplateHeading);
            this.Controls.Add(this.m_lblSelectedTemplate);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "EntityTemplatePanel";
            this.Text = "Template Properties";
            this.Load += new System.EventHandler(this.EntityTemplatePanel_Load);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        #region Winforms Handlers

        private void m_btnEditTemplate_Click(object sender, System.EventArgs e)
        {
            MEntity[] amDependentEntities = MFramework.Instance.Scene
                .GetDependentEntities(GetActiveTemplate());
            bool bReadOnlyFound = false;
            foreach (MEntity pmEntity in amDependentEntities)
            {
                if (!pmEntity.Writable)
                {
                    bReadOnlyFound = true;
                    break;
                }
            }
            if (bReadOnlyFound)
            {
                MessageBox.Show("Read-only entities in the scene depend " +
                    "on the selected template.\nIt cannot be edited.",
                    "Cannot Edit Template", MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
            else
            {
                using (EditTemplateDlg dlg =
                    new EditTemplateDlg(GetActiveTemplate()))
                {
                    if (dlg.ShowDialog() == DialogResult.OK)
                    {
                        dlg.ApplyOperations(GetActiveTemplate());
                    }
                }
            }
        }

        private void m_btnCreateNewTemplate_Click(object sender,
            System.EventArgs e)
        {
            AddNewTemplateDlg dlg = new AddNewTemplateDlg();
            dlg.SelectedPalette = FW.PaletteManager.ActivePalette;
            MEntity currentTemplate = GetActiveTemplate();
            
            if (currentTemplate != null)
            {
                dlg.Category = FW.PaletteManager.ActivePalette.
                    GetEntityCategory(currentTemplate);
            }
            else
            {

            }

            if (FW.PaletteManager.ActivePalette == null)
            {
                MPalette[] palettes = FW.PaletteManager.GetPalettes();
                foreach(MPalette palette in palettes)
                {
                    if (!palette.DontSave)
                    {
                        dlg.SelectedPalette = palette;
                        break;
                    }
                }
            }

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MEntity newEntity = 
                    MEntity.CreateGeneralEntity(dlg.TemplateName);
                newEntity.TemplateID = Guid.NewGuid();
                dlg.SelectedPalette.AddEntity(newEntity, dlg.Category,
                    true);      
                FW.PaletteManager.ActivePalette = dlg.SelectedPalette;
                FW.PaletteManager.ActivePalette.ActiveEntity = newEntity;
            }
        }

        private void EntityTemplatePanel_Load(object sender,
            System.EventArgs e)
        {
            m_fontNormal = m_lblSelectedTemplate.Font;
            m_fontBold = new Font(m_fontNormal, FontStyle.Bold);
            RefreshUI();
            FW.EventManager.SceneClosing +=
                new MEventManager.__Delegate_SceneClosing(
                    OnSceneClosing);
            FW.EventManager.LongOperationCompleted +=
                new MEventManager.__Delegate_LongOperationCompleted(
                    OnLongOperationCompleted);
            FW.EventManager.ActivePaletteChanged +=
                new MEventManager.__Delegate_ActivePaletteChanged(
                    OnPaletteChanged);
            FW.EventManager.PaletteActiveEntityChanged += 
                new MEventManager.__Delegate_PaletteActiveEntityChanged(
                    OnPaletteActiveEntityChanged);  
            FW.EventManager.EntityComponentAdded += 
                new MEventManager.__Delegate_EntityComponentAdded(
                    OnEntityComponentAdded);
            FW.EventManager.EntityComponentRemoved +=
                new MEventManager.__Delegate_EntityComponentRemoved(
                    OnEntityComponentRemoved);
            FW.EventManager.ComponentPropertyAdded += 
                new MEventManager.__Delegate_ComponentPropertyAdded(
                    OnComponentPropertyAdded);
            FW.EventManager.ComponentPropertyRemoved +=
                new MEventManager.__Delegate_ComponentPropertyRemoved(
                    OnComponentPropertyRemoved);
            FW.EventManager.EntityPropertyChanged +=
                new MEventManager.__Delegate_EntityPropertyChanged(
                    OnEntityPropertyChanged);
            FW.EventManager.ComponentPropertyChanged +=
                new MEventManager.__Delegate_ComponentPropertyChanged(
                    OnComponentPropertyChanged);
            FW.EventManager.EntityTagsChanged +=
                new MEventManager.__Delegate_EntityTagsChanged(
                    OnEntityTagsChanged);
            FW.EventManager.PaletteWritableStatusChanged +=
                new MEventManager.__Delegate_PaletteWritableStatusChanged(
                    OnPaletteWritableStatusChanged);
        }


        #endregion

        #region UI Refresh Methods

        private void OnPaletteChanged(
            MPalette pmActivePalette, MPalette pmOldActivePalette)
        {
            RefreshUI();
            // PropertyGrid keeps around a cache of its last selected object
            // in order to restore its previous tree expansion, if possible.
            // Because entities might be deleted when we change scenes, we 
            // need to set and then clear some innocuous selected object here
            // to avoid accessing a disposed object.
            if (m_gridProperties.SelectedObject == null)
            {
                m_gridProperties.SelectedObject = string.Empty;
                m_gridProperties.SelectedObject = null;
            }
        }

        private void OnEntityComponentAdded(MEntity pmEntity,
            MComponent pmComponent)
        {
            if(m_gridProperties.SelectedObject == pmEntity)
            {
                RefreshUI();
            }
        }

        private void OnEntityComponentRemoved(MEntity pmEntity,
            MComponent pmComponent)
        {
            if(m_gridProperties.SelectedObject == pmEntity)
            {
                RefreshUI();
            }
        }

        private void OnPaletteActiveEntityChanged(
            MPalette pmPalette, MEntity pmOldActiveEntity)
        {
            RefreshUI();
        }

        private void OnComponentPropertyAdded(
            MComponent pmComponent, string strPropertyName, bool bInBatch)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bDoRefreshUI = true;
            }
            else
            {
                RefreshUI();
            }
        }

        private void OnComponentPropertyRemoved(
            MComponent pmComponent, string strPropertyName, bool bInBatch)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bDoRefreshUI = true;
            }
            else
            {
                RefreshUI();
            }
        }

        private void OnEntityPropertyChanged(MEntity pmEntity, 
            string strPropertyName, uint uiPropertyIndex, bool bInBatch)
        {
            if(m_gridProperties.SelectedObject == pmEntity)
            {
                if (MFramework.Instance.PerformingLongOperation)
                {
                    m_bDoRefreshUI = true;
                }
                else
                {
                    RefreshUI();
                }
            }
        }

        private void OnComponentPropertyChanged(MComponent pmComponent,
            string strPropertyName, uint uiPropertyIndex, bool bInBatch)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bDoRefreshUI = true;
            }
            else
            {
                RefreshUI();
            }
        }

        private void OnEntityTagsChanged(MEntity pmEntity, string strOldTags)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bDoRefreshUI = true;
                return;
            }

            foreach (MEntity selectedEntity in
                m_gridProperties.SelectedObjects)
            {
                if (selectedEntity == pmEntity)
                {
                    RefreshUI();
                    break;
                }
            }
        }

        private void OnPaletteWritableStatusChanged(MPalette pmPalette)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bDoRefreshUI = true;
            }
            else
            {
                RefreshUI();
            }
        }

        private void RefreshUI()
        {
            RefreshComponentList();
            RefreshTemplateLabel();
            
        }

        private void RefreshComponentList()
        {
            MEntity template = GetActiveTemplate();
            m_gridProperties.SelectedObject = template;
        }

        private void RefreshTemplateLabel()
        {
            m_lblSelectedTemplate.Font = m_fontNormal;
            string text = "None";
            MEntity template = GetActiveTemplate();
            m_lblSelectedTemplate.ForeColor = SystemColors.WindowText;
            if (template != null)
            {
                MPalette activePalette = FW.PaletteManager.ActivePalette;

                text = EntityPathService.GetFullPath(activePalette.Scene,
                    template);
                if (activePalette.DontSave)
                {
                    m_lblSelectedTemplate.ForeColor = 
                        PalettePanel.m_cOrphanColor;
                }
                if (activePalette.Dirty)
                {
                    m_lblSelectedTemplate.Font = m_fontBold;
                }
            }
            m_lblSelectedTemplate.Text = text;

        }

        #endregion

        #region Utility Methods

        private MEntity GetActiveTemplate()
        {
            MPalette activePalette = FW.PaletteManager.ActivePalette;
            if (activePalette != null)
            {
                return activePalette.ActiveEntity;
            }
            return null;
        }

        [UICommandHandler("Idle")]
        private void OnIdle(object sender, EventArgs args)
        {
            MEntity template = GetActiveTemplate();
            m_btnEditTemplate.Enabled = 
                ((template != null) && template.Writable);
            int paletteCount = 0;
            foreach(MPalette palette in FW.PaletteManager.GetPalettes())
            {
                if (!palette.DontSave)
                {
                    paletteCount++;
                }
            }

            m_btnCreateNewTemplate.Enabled = 
                paletteCount > 0;
        }

        #endregion

    }
}

