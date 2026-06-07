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
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Filters;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    public partial class SelectionFilterPanel : Form
    {
        public SelectionFilterPanel()
        {
            InitializeComponent();
        }

        #region Data Members
        private bool m_updatingFilterCheckBox = false;
        private NameFilter m_nameFilter = new NameFilter();
        private LayersFilter m_layersFilter = new LayersFilter();
        private TagsFilter m_tagsFilter = new TagsFilter();
        private bool m_populateLayersListBox = false;
        private bool m_populateTagsListBox = false;
        #endregion

        #region Helper Properties
        private MFramework FW
        {
            get
            {
                return MFramework.Instance;
            }
        }
        #endregion

        #region EventManager Event Handlers
        private void EventManager_NewSceneLoaded(MScene scene)
        {
            InitializeNameControls();
            PopulateLayersListBox();
            PopulateTagsListBox();
        }

        private void EventManager_SceneClosing(MScene scene)
        {
            m_tbName.Text = string.Empty;
            m_lbLayers.Items.Clear();
            m_layersFilter.Layers.Clear();
            m_lbTags.Items.Clear();
            m_tagsFilter.Tags.Clear();
        }

        private void EventManager_FilterEnabledChanged()
        {
            SynchronizeFilterEnabledCheckBox();
        }

        private void EventManager_LayerAdded(MLayer layer, MLayer parent)
        {
            if (FW.PerformingLongOperation)
            {
                m_populateLayersListBox = true;
            }
            else
            {
                PopulateLayersListBox();
            }
        }

        private void EventManager_LayerDeleted(MLayer layer, bool bFileDeleted)
        {
            if (FW.PerformingLongOperation)
            {
                m_populateLayersListBox = true;
            }
            else
            {
                PopulateLayersListBox();
            }
        }

        private void EventManager_TagsManagerUpdated()
        {
            if (FW.PerformingLongOperation)
            {
                m_populateTagsListBox = true;
            }
            else
            {
                PopulateTagsListBox();
            }
        }

        private void EventManager_LongOperationCompleted()
        {
            if (m_populateLayersListBox)
            {
                PopulateLayersListBox();
                m_populateLayersListBox = false;
            }
            if (m_populateTagsListBox)
            {
                PopulateTagsListBox();
                m_populateTagsListBox = false;
            }
        }
        #endregion

        #region Helper Functions
        private void SynchronizeFilterEnabledCheckBox()
        {
            if (!m_updatingFilterCheckBox)
            {
                m_chkFilterEnabled.Checked = FW.FilterManager.FilterEnabled;
            }
        }

        private void EnableDisableControls()
        {
            bool bEnabled = FW.FilterManager.FilterEnabled;

            m_gbFilterByName.Enabled = bEnabled;
            m_rbEntityName.Enabled = bEnabled;
            m_rbTemplateName.Enabled = bEnabled;
            m_rbComponentName.Enabled = bEnabled;
            m_tbName.Enabled = bEnabled;
            m_lblFilterByLayers.Enabled = bEnabled;
            m_lbLayers.Enabled = bEnabled;
            m_lblFilterByTags.Enabled = bEnabled;
            m_lbTags.Enabled = bEnabled;
        }

        private void InitializeNameControls()
        {
            m_rbEntityName.Checked = true;
            m_rbTemplateName.Checked = false;
            m_rbComponentName.Checked = false;
            m_tbName.Text = string.Empty;
        }

        private struct LayerItem
        {
            public LayerItem(MLayer layer)
            {
                Layer = layer;
            }

            public override string ToString()
            {
                return Layer.ShortName;
            }

            public MLayer Layer;
        }

        private void PopulateLayersListBox()
        {
            m_lbLayers.BeginUpdate();

            List<MLayer> checkedLayers = m_layersFilter.Layers;
            m_layersFilter.Layers.Clear();
            int selectedIndex = m_lbLayers.SelectedIndex;

            m_lbLayers.Items.Clear();
            MLayer[] layers = FW.LayerManager.GetLayers();
            foreach (MLayer layer in layers)
            {
                bool isChecked = checkedLayers.Contains(layer);
                m_lbLayers.Items.Add(new LayerItem(layer), isChecked);
                if (isChecked)
                {
                    m_layersFilter.Layers.Add(layer);
                }
            }

            if (selectedIndex > -1 && m_lbLayers.Items.Count > 0)
            {
                m_lbLayers.SelectedIndex = selectedIndex;
            }

            m_lbLayers.EndUpdate();
        }

        private void PopulateTagsListBox()
        {
            m_lbTags.BeginUpdate();

            List<string> checkedTags = m_tagsFilter.Tags;
            m_tagsFilter.Tags.Clear();
            int selectedIndex = m_lbTags.SelectedIndex;

            m_lbTags.Items.Clear();
            string[] tags = FW.TagsManager.GetTags();
            foreach (string tag in tags)
            {
                bool isChecked = checkedTags.Contains(tag);
                m_lbTags.Items.Add(tag, isChecked);
                if (isChecked)
                {
                    m_tagsFilter.Tags.Add(tag);
                }
            }

            if (selectedIndex > -1 && m_lbTags.Items.Count > 0)
            {
                m_lbTags.SelectedIndex = selectedIndex;
            }

            m_lbTags.EndUpdate();
        }
        #endregion

        #region Form Event Handlers
        private void SelectionFilterPanel_Load(object sender, EventArgs e)
        {
            EnableDisableControls();
            SynchronizeFilterEnabledCheckBox();
            InitializeNameControls();
            PopulateLayersListBox();
            PopulateTagsListBox();

            // Register event handlers.
            FW.EventManager.NewSceneLoaded +=
                new MEventManager.__Delegate_NewSceneLoaded(
                EventManager_NewSceneLoaded);
            FW.EventManager.SceneClosing +=
                new MEventManager.__Delegate_SceneClosing(
                EventManager_SceneClosing);
            FW.EventManager.FilterEnabledChanged +=
                new MEventManager.__Delegate_FilterEnabledChanged(
                EventManager_FilterEnabledChanged);
            FW.EventManager.LayerAdded +=
                new MEventManager.__Delegate_LayerAdded(
                EventManager_LayerAdded);
            FW.EventManager.LayerDeleted +=
                new MEventManager.__Delegate_LayerDeleted(
                EventManager_LayerDeleted);
            FW.EventManager.TagsManagerUpdated +=
                new MEventManager.__Delegate_TagsManagerUpdated(
                EventManager_TagsManagerUpdated);
            FW.EventManager.LongOperationCompleted +=
                new MEventManager.__Delegate_LongOperationCompleted(
                EventManager_LongOperationCompleted);
        }

        private void m_chkFilterEnabled_CheckedChanged(object sender,
            EventArgs e)
        {
            m_updatingFilterCheckBox = true;
            FW.FilterManager.FilterEnabled = m_chkFilterEnabled.Checked;
            m_updatingFilterCheckBox = false;

            EnableDisableControls();
        }

        private void m_rbEntityName_CheckedChanged(object sender, EventArgs e)
        {
            if (m_rbEntityName.Checked)
            {
                m_nameFilter.FilterType = NameFilter.Type.Entity;
                FW.FilterManager.UpdateFilter(m_nameFilter);
            }
        }

        private void m_rbTemplateName_CheckedChanged(object sender,
            EventArgs e)
        {
            if (m_rbTemplateName.Checked)
            {
                m_nameFilter.FilterType = NameFilter.Type.Template;
                FW.FilterManager.UpdateFilter(m_nameFilter);
            }
        }

        private void m_rbComponentName_CheckedChanged(object sender,
            EventArgs e)
        {
            if (m_rbComponentName.Checked)
            {
                m_nameFilter.FilterType = NameFilter.Type.Component;
                FW.FilterManager.UpdateFilter(m_nameFilter);
            }
        }

        private void m_tbName_TextChanged(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(m_tbName.Text))
            {
                m_nameFilter.MatchString = null;
                FW.FilterManager.RemoveFilter(m_nameFilter);
            }
            else
            {
                m_nameFilter.MatchString = m_tbName.Text;
                FW.FilterManager.UpdateFilter(m_nameFilter);
            }
        }

        private void m_lbLayers_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (m_lbLayers.CheckedItems.Count == 1 &&
                e.NewValue == CheckState.Unchecked)
            {
                // This is the only checked item and is about to be unchecked.
                m_layersFilter.Layers.Clear();
                FW.FilterManager.RemoveFilter(m_layersFilter);
            }
            else if (m_lbLayers.CheckedItems.Count == 0 &&
                e.NewValue == CheckState.Checked)
            {
                // This is the first item to be checked.
                Debug.Assert(e.Index > -1, "Invalid index!");
                LayerItem item = (LayerItem) m_lbLayers.Items[e.Index];
                m_layersFilter.Layers.Add(item.Layer);
                FW.FilterManager.AddFilter(m_layersFilter);
            }
            else
            {
                Debug.Assert(e.Index > -1, "Invalid index!");
                LayerItem item = (LayerItem) m_lbLayers.Items[e.Index];
                if (e.NewValue == CheckState.Checked)
                {
                    m_layersFilter.Layers.Add(item.Layer);
                }
                else
                {
                    Debug.Assert(e.NewValue == CheckState.Unchecked,
                        "Invalid CheckState!");
                    m_layersFilter.Layers.Remove(item.Layer);
                }
                FW.FilterManager.UpdateFilter(m_layersFilter);
            }
        }

        private void m_lbTags_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (m_lbTags.CheckedItems.Count == 1 &&
                e.NewValue == CheckState.Unchecked)
            {
                // This is the only checked item and is about to be unchecked.
                m_tagsFilter.Tags.Clear();
                FW.FilterManager.RemoveFilter(m_tagsFilter);
            }
            else if (m_lbTags.CheckedItems.Count == 0 &&
                e.NewValue == CheckState.Checked)
            {
                // This is the first item to be checked.
                Debug.Assert(e.Index > -1, "Invalid index!");
                m_tagsFilter.Tags.Add((string) m_lbTags.Items[e.Index]);
                FW.FilterManager.AddFilter(m_tagsFilter);
            }
            else
            {
                Debug.Assert(e.Index > -1, "Invalid index!");
                if (e.NewValue == CheckState.Checked)
                {
                    m_tagsFilter.Tags.Add((string) m_lbTags.Items[e.Index]);
                }
                else
                {
                    Debug.Assert(e.NewValue == CheckState.Unchecked,
                        "Invalid CheckState!");
                    m_tagsFilter.Tags.Remove((string) m_lbTags.Items[e.Index]);
                }
                FW.FilterManager.UpdateFilter(m_tagsFilter);
            }
        }

        private void m_miUncheckAllLayers_Click(object sender, EventArgs e)
        {
            for (int index = 0; index < m_lbLayers.Items.Count; ++index)
            {
                m_lbLayers.SetItemChecked(index, false);
            }
        }

        private void m_miUncheckAllTags_Click(object sender, EventArgs e)
        {
            for (int index = 0; index < m_lbTags.Items.Count; ++index)
            {
                m_lbTags.SetItemChecked(index, false);
            }
        }
        #endregion
    }
}
