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
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using PluginAPI = Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    public class SelectionPanel : Form
    {
        #region Private Data
        //the following two variables are to prevent the two selection 
        //handlers for the GUI and the manager from handling events
        //that were triggered by the other function
        //the will prevent a stack overflow 
        //private bool m_isInManagerHandler = false;
        //private bool m_isInGuiHandler = false;
        private bool m_bSynchingSelectionService = false;
        private bool m_bSynchingListBox = false;
        private bool m_bEnableListBoxSelectedIndexChanged = true;
        private bool m_bBuildingList = false;
        private EntityComparer m_pmEntityComparer = new EntityComparer();
        private Dictionary<MEntity, TreeNode> m_pmEntityMap = null;
        private Dictionary<MEntity, bool> m_pmExpandedMap =
            new Dictionary<MEntity, bool>();

        private enum FilterState
        {
            Visible,
            Hidden,
            Frozen
        };
        private FilterState m_eFilterState = FilterState.Visible;

        private static string ms_strHiddenFilterSettingName =
            "DisplayHiddenFilterWarning";
        private static string ms_strFrozenFilterSettingName =
            "DisplayFrozenFilterWarning";
        private static string ms_strAddTagUICommandName =
            "AddTagToSelectedEntities";
        private static string ms_strRemoveTagsUICommandName =
            "RemoveTagsFromSelectedEntities";
        private static string ms_strEditCommonTagsUICommandName =
            "EditCommonTagsOnSelectedEntities";
        #endregion

        private System.Windows.Forms.TextBox m_tbEntityName;
        private System.Windows.Forms.Label m_lblSelectionSet;
        private System.Windows.Forms.ComboBox m_cbSelectionSets;
        private System.Windows.Forms.ToolBar m_tbToolbar;
        private System.Windows.Forms.ToolBarButton m_tbbHideUnhide;
        private System.Windows.Forms.ToolBarButton m_tbbFreezeUnfreeze;
        private System.Windows.Forms.ToolBarButton m_tbbSelectAll;
        private System.Windows.Forms.ToolBarButton m_tbbSelectNone;
        private System.Windows.Forms.ToolBarButton m_tbbInvertSelection;
        private System.Windows.Forms.ToolBarButton m_tbbFilter;
        private System.Windows.Forms.ImageList m_ilToobarImages;
        private TreeView m_tvEntityList;
        private Label m_lblTag;
        private ComboBox m_cbTags;
        private ToolBarButton m_tbbAddTagToSelectedEntities;
        private ToolBarButton m_tbbRemoveTagsFromSelectedEntities;
        private ToolBarButton m_tbbEditCommonTagsOnSelectedEntities;
        private ToolBarButton m_tbbEnableSelectionFilter;
        private ToolBarButton m_tbbDisableSelectionFilter;
        private Panel m_pnlEntityListBorder;
        private System.ComponentModel.IContainer components = null;

        public SelectionPanel(IUICommandService commandService)
        {
            // This call is required by the Windows Form Designer.
            InitializeComponent();
            commandService.BindCommands(this);
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.ToolBarButton m_tbbSeparator1;
            System.Windows.Forms.ToolBarButton m_tbbSeparator2;
            System.Windows.Forms.ToolBarButton m_tbbSeparator3;
            System.Windows.Forms.ToolBarButton m_tbbSeparator4;
            System.Windows.Forms.Label m_lblDummy;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SelectionPanel));
            this.m_tbEntityName = new System.Windows.Forms.TextBox();
            this.m_lblSelectionSet = new System.Windows.Forms.Label();
            this.m_cbSelectionSets = new System.Windows.Forms.ComboBox();
            this.m_tbToolbar = new System.Windows.Forms.ToolBar();
            this.m_tbbSelectAll = new System.Windows.Forms.ToolBarButton();
            this.m_tbbSelectNone = new System.Windows.Forms.ToolBarButton();
            this.m_tbbInvertSelection = new System.Windows.Forms.ToolBarButton();
            this.m_tbbHideUnhide = new System.Windows.Forms.ToolBarButton();
            this.m_tbbFreezeUnfreeze = new System.Windows.Forms.ToolBarButton();
            this.m_tbbFilter = new System.Windows.Forms.ToolBarButton();
            this.m_tbbAddTagToSelectedEntities = new System.Windows.Forms.ToolBarButton();
            this.m_tbbRemoveTagsFromSelectedEntities = new System.Windows.Forms.ToolBarButton();
            this.m_tbbEditCommonTagsOnSelectedEntities = new System.Windows.Forms.ToolBarButton();
            this.m_tbbEnableSelectionFilter = new System.Windows.Forms.ToolBarButton();
            this.m_tbbDisableSelectionFilter = new System.Windows.Forms.ToolBarButton();
            this.m_ilToobarImages = new System.Windows.Forms.ImageList(this.components);
            this.m_tvEntityList = new System.Windows.Forms.TreeView();
            this.m_lblTag = new System.Windows.Forms.Label();
            this.m_cbTags = new System.Windows.Forms.ComboBox();
            this.m_pnlEntityListBorder = new System.Windows.Forms.Panel();
            m_tbbSeparator1 = new System.Windows.Forms.ToolBarButton();
            m_tbbSeparator2 = new System.Windows.Forms.ToolBarButton();
            m_tbbSeparator3 = new System.Windows.Forms.ToolBarButton();
            m_tbbSeparator4 = new System.Windows.Forms.ToolBarButton();
            m_lblDummy = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_tbbSeparator1
            // 
            m_tbbSeparator1.Name = "m_tbbSeparator1";
            m_tbbSeparator1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // m_tbbSeparator2
            // 
            m_tbbSeparator2.Name = "m_tbbSeparator2";
            m_tbbSeparator2.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // m_tbbSeparator3
            // 
            m_tbbSeparator3.Name = "m_tbbSeparator3";
            m_tbbSeparator3.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // m_tbbSeparator4
            // 
            m_tbbSeparator4.Name = "m_tbbSeparator4";
            m_tbbSeparator4.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
            // 
            // m_lblDummy
            // 
            m_lblDummy.Location = new System.Drawing.Point(297, 226);
            m_lblDummy.Name = "m_lblDummy";
            m_lblDummy.Size = new System.Drawing.Size(8, 8);
            m_lblDummy.TabIndex = 9;
            // 
            // m_tbEntityName
            // 
            this.m_tbEntityName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tbEntityName.Location = new System.Drawing.Point(8, 32);
            this.m_tbEntityName.Name = "m_tbEntityName";
            this.m_tbEntityName.Size = new System.Drawing.Size(356, 20);
            this.m_tbEntityName.TabIndex = 1;
            this.m_tbEntityName.TextChanged += new System.EventHandler(this.m_tbEntityName_TextChanged);
            // 
            // m_lblSelectionSet
            // 
            this.m_lblSelectionSet.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lblSelectionSet.AutoSize = true;
            this.m_lblSelectionSet.Location = new System.Drawing.Point(8, 380);
            this.m_lblSelectionSet.Name = "m_lblSelectionSet";
            this.m_lblSelectionSet.Size = new System.Drawing.Size(120, 13);
            this.m_lblSelectionSet.TabIndex = 3;
            this.m_lblSelectionSet.Text = "Select by Selection Set:";
            // 
            // m_cbSelectionSets
            // 
            this.m_cbSelectionSets.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbSelectionSets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbSelectionSets.Location = new System.Drawing.Point(8, 396);
            this.m_cbSelectionSets.Name = "m_cbSelectionSets";
            this.m_cbSelectionSets.Size = new System.Drawing.Size(356, 21);
            this.m_cbSelectionSets.Sorted = true;
            this.m_cbSelectionSets.TabIndex = 4;
            this.m_cbSelectionSets.SelectedIndexChanged += new System.EventHandler(this.m_cbSelectionSets_SelectedIndexChanged);
            // 
            // m_tbToolbar
            // 
            this.m_tbToolbar.Appearance = System.Windows.Forms.ToolBarAppearance.Flat;
            this.m_tbToolbar.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
            this.m_tbbSelectAll,
            this.m_tbbSelectNone,
            this.m_tbbInvertSelection,
            m_tbbSeparator1,
            this.m_tbbHideUnhide,
            this.m_tbbFreezeUnfreeze,
            m_tbbSeparator2,
            this.m_tbbFilter,
            m_tbbSeparator3,
            this.m_tbbAddTagToSelectedEntities,
            this.m_tbbRemoveTagsFromSelectedEntities,
            this.m_tbbEditCommonTagsOnSelectedEntities,
            m_tbbSeparator4,
            this.m_tbbEnableSelectionFilter,
            this.m_tbbDisableSelectionFilter});
            this.m_tbToolbar.Divider = false;
            this.m_tbToolbar.DropDownArrows = true;
            this.m_tbToolbar.ImageList = this.m_ilToobarImages;
            this.m_tbToolbar.Location = new System.Drawing.Point(0, 0);
            this.m_tbToolbar.Name = "m_tbToolbar";
            this.m_tbToolbar.ShowToolTips = true;
            this.m_tbToolbar.Size = new System.Drawing.Size(372, 26);
            this.m_tbToolbar.TabIndex = 0;
            this.m_tbToolbar.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.m_tbToolbar_ButtonClick);
            this.m_tbToolbar.ButtonDropDown += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.m_tbToolbar_ButtonDropDown);
            // 
            // m_tbbSelectAll
            // 
            this.m_tbbSelectAll.ImageIndex = 0;
            this.m_tbbSelectAll.Name = "m_tbbSelectAll";
            this.m_tbbSelectAll.ToolTipText = "Select All";
            // 
            // m_tbbSelectNone
            // 
            this.m_tbbSelectNone.ImageIndex = 1;
            this.m_tbbSelectNone.Name = "m_tbbSelectNone";
            this.m_tbbSelectNone.ToolTipText = "Select None";
            // 
            // m_tbbInvertSelection
            // 
            this.m_tbbInvertSelection.ImageIndex = 2;
            this.m_tbbInvertSelection.Name = "m_tbbInvertSelection";
            this.m_tbbInvertSelection.ToolTipText = "Invert Selection";
            // 
            // m_tbbHideUnhide
            // 
            this.m_tbbHideUnhide.ImageIndex = 3;
            this.m_tbbHideUnhide.Name = "m_tbbHideUnhide";
            // 
            // m_tbbFreezeUnfreeze
            // 
            this.m_tbbFreezeUnfreeze.ImageIndex = 4;
            this.m_tbbFreezeUnfreeze.Name = "m_tbbFreezeUnfreeze";
            // 
            // m_tbbFilter
            // 
            this.m_tbbFilter.Name = "m_tbbFilter";
            this.m_tbbFilter.Style = System.Windows.Forms.ToolBarButtonStyle.DropDownButton;
            // 
            // m_tbbAddTagToSelectedEntities
            // 
            this.m_tbbAddTagToSelectedEntities.ImageIndex = 8;
            this.m_tbbAddTagToSelectedEntities.Name = "m_tbbAddTagToSelectedEntities";
            this.m_tbbAddTagToSelectedEntities.ToolTipText = "Add Tag to Selected Entities";
            // 
            // m_tbbRemoveTagsFromSelectedEntities
            // 
            this.m_tbbRemoveTagsFromSelectedEntities.ImageIndex = 9;
            this.m_tbbRemoveTagsFromSelectedEntities.Name = "m_tbbRemoveTagsFromSelectedEntities";
            this.m_tbbRemoveTagsFromSelectedEntities.ToolTipText = "Remove Tags from Selected Entities";
            // 
            // m_tbbEditCommonTagsOnSelectedEntities
            // 
            this.m_tbbEditCommonTagsOnSelectedEntities.ImageIndex = 10;
            this.m_tbbEditCommonTagsOnSelectedEntities.Name = "m_tbbEditCommonTagsOnSelectedEntities";
            this.m_tbbEditCommonTagsOnSelectedEntities.ToolTipText = "Edit Common Tags on Selected Entities";
            // 
            // m_tbbEnableSelectionFilter
            // 
            this.m_tbbEnableSelectionFilter.ImageIndex = 11;
            this.m_tbbEnableSelectionFilter.Name = "m_tbbEnableSelectionFilter";
            this.m_tbbEnableSelectionFilter.ToolTipText = "Enable Selection Filter";
            // 
            // m_tbbDisableSelectionFilter
            // 
            this.m_tbbDisableSelectionFilter.ImageIndex = 12;
            this.m_tbbDisableSelectionFilter.Name = "m_tbbDisableSelectionFilter";
            this.m_tbbDisableSelectionFilter.ToolTipText = "Disable Selection Filter";
            // 
            // m_ilToobarImages
            // 
            this.m_ilToobarImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_ilToobarImages.ImageStream")));
            this.m_ilToobarImages.TransparentColor = System.Drawing.Color.Fuchsia;
            this.m_ilToobarImages.Images.SetKeyName(0, "");
            this.m_ilToobarImages.Images.SetKeyName(1, "");
            this.m_ilToobarImages.Images.SetKeyName(2, "");
            this.m_ilToobarImages.Images.SetKeyName(3, "");
            this.m_ilToobarImages.Images.SetKeyName(4, "");
            this.m_ilToobarImages.Images.SetKeyName(5, "");
            this.m_ilToobarImages.Images.SetKeyName(6, "");
            this.m_ilToobarImages.Images.SetKeyName(7, "");
            this.m_ilToobarImages.Images.SetKeyName(8, "08_AddTag.png");
            this.m_ilToobarImages.Images.SetKeyName(9, "09_RemoveTags.png");
            this.m_ilToobarImages.Images.SetKeyName(10, "10_EditCommonTags.png");
            this.m_ilToobarImages.Images.SetKeyName(11, "11_EnableSelectionFilter.png");
            this.m_ilToobarImages.Images.SetKeyName(12, "12_DisableSelectionFilter.png");
            // 
            // m_tvEntityList
            // 
            this.m_tvEntityList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_tvEntityList.DrawMode = System.Windows.Forms.TreeViewDrawMode.OwnerDrawText;
            this.m_tvEntityList.FullRowSelect = true;
            this.m_tvEntityList.HideSelection = false;
            this.m_tvEntityList.Location = new System.Drawing.Point(8, 60);
            this.m_tvEntityList.Name = "m_tvEntityList";
            this.m_tvEntityList.ShowPlusMinus = false;
            this.m_tvEntityList.ShowRootLines = false;
            this.m_tvEntityList.Size = new System.Drawing.Size(356, 311);
            this.m_tvEntityList.TabIndex = 5;
            this.m_tvEntityList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.m_tvEntityList_MouseDoubleClick);
            this.m_tvEntityList.MouseClick += new System.Windows.Forms.MouseEventHandler(this.m_tvEntityList_MouseClick);
            this.m_tvEntityList.DrawNode += new System.Windows.Forms.DrawTreeNodeEventHandler(this.m_tvEntityList_DrawNode);
            this.m_tvEntityList.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.m_tvEntityList_BeforeExpand);
            this.m_tvEntityList.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.m_tvEntityList_BeforeCollapse);
            this.m_tvEntityList.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_tvEntityList_MouseUp);
            this.m_tvEntityList.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.m_tvEntityList_AfterSelect);
            this.m_tvEntityList.MouseMove += new System.Windows.Forms.MouseEventHandler(this.m_tvEntityList_MouseMove);
            this.m_tvEntityList.MouseDown += new System.Windows.Forms.MouseEventHandler(this.m_tvEntityList_MouseDown);
            this.m_tvEntityList.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.m_tvEntityList_BeforeSelect);
            this.m_tvEntityList.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnKeyDown);
            // 
            // m_lblTag
            // 
            this.m_lblTag.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_lblTag.AutoSize = true;
            this.m_lblTag.Location = new System.Drawing.Point(8, 420);
            this.m_lblTag.Name = "m_lblTag";
            this.m_lblTag.Size = new System.Drawing.Size(76, 13);
            this.m_lblTag.TabIndex = 3;
            this.m_lblTag.Text = "Select by Tag:";
            // 
            // m_cbTags
            // 
            this.m_cbTags.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cbTags.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbTags.Location = new System.Drawing.Point(8, 436);
            this.m_cbTags.Name = "m_cbTags";
            this.m_cbTags.Size = new System.Drawing.Size(356, 21);
            this.m_cbTags.Sorted = true;
            this.m_cbTags.TabIndex = 4;
            this.m_cbTags.SelectedIndexChanged += new System.EventHandler(this.m_cbTags_SelectedIndexChanged);
            // 
            // m_pnlEntityListBorder
            // 
            this.m_pnlEntityListBorder.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_pnlEntityListBorder.BackColor = System.Drawing.Color.Red;
            this.m_pnlEntityListBorder.Location = new System.Drawing.Point(5, 57);
            this.m_pnlEntityListBorder.Name = "m_pnlEntityListBorder";
            this.m_pnlEntityListBorder.Size = new System.Drawing.Size(362, 317);
            this.m_pnlEntityListBorder.TabIndex = 6;
            // 
            // SelectionPanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(372, 474);
            this.Controls.Add(this.m_tvEntityList);
            this.Controls.Add(this.m_tbToolbar);
            this.Controls.Add(this.m_cbTags);
            this.Controls.Add(this.m_cbSelectionSets);
            this.Controls.Add(this.m_lblTag);
            this.Controls.Add(this.m_lblSelectionSet);
            this.Controls.Add(this.m_tbEntityName);
            this.Controls.Add(this.m_pnlEntityListBorder);
            this.Controls.Add(m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.Name = "SelectionPanel";
            this.Text = "Selection";
            this.Load += new System.EventHandler(this.SelectionPanel_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.OnKeyDown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        #endregion

        #region EventManager Handlers
        private bool m_bBuildAndSync = false;
        private bool m_bSyncListBoxToSelectionService = false;
        private bool m_bSyncToolbarButtons = false;
        private bool m_bSyncSelectionSetComboBox = false;
        private bool m_bSyncTagsComboBox = false;

        private void EventManager_NewSceneLoaded(MScene pmScene)
        {
            m_pmExpandedMap = new Dictionary<MEntity, bool>();

            if (pmScene == MFramework.Instance.Scene)
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    BuildAndSync();
                    SyncSelectionSetComboBox();
                }
                else
                {
                    m_bBuildAndSync = true;
                    m_bSyncSelectionSetComboBox = true;
                }
            }
        }

        private void EventManager_NewLayerAdded(MLayer pmLayer, 
            MLayer pmParent)
        {
            EventManager_NewSceneLoaded(MFramework.Instance.Scene);
        }

        private void EventManager_LayerDeleted(MLayer pmLayer,
            bool bFileDeleted)
        {
            EventManager_NewSceneLoaded(MFramework.Instance.Scene);
        }

        private void EventManager_LayerFilenameRemoved(MLayer pmLayer,
            String strFilename)
        {
            EventManager_NewSceneLoaded(MFramework.Instance.Scene);
        }

        private void EventManager_SelectedEntitiesChanged()
        {
            if (!MFramework.Instance.PerformingLongOperation)
            {
                SyncListBoxToSelectionService();
                SyncToolbarButtons();
            }
            else
            {
                m_bSyncListBoxToSelectionService = true;
                m_bSyncToolbarButtons = true;
            }
        }

        private void EventManager_EntityAddedToScene(MScene pmScene,
            MEntity pmEntity)
        {
            if (pmScene == MFramework.Instance.Scene)
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    BuildAndSync();
                }
                else
                {
                    m_bBuildAndSync = true;
                }
            }
        }

        private void EventManager_EntityRemovedFromScene(MScene pmScene,
            MEntity pmEntity)
        {
            if (pmScene == MFramework.Instance.Scene)
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    BuildAndSync();
                }
                else
                {
                    m_bBuildAndSync = true;
                }
            }
        }

        private void EventManager_EntityNameChanged(MEntity pmEntity,
            string strOldName, bool bInBatch)
        {
            if (MFramework.Instance.Scene.IsEntityInScene(pmEntity))
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    BuildAndSync();
                }
                else
                {
                    m_bBuildAndSync = true;
                }
            }
        }

        private void EventManager_EntityHiddenStateChanged(MEntity pmEntity,
            bool bHidden)
        {
            if (MFramework.Instance.Scene.IsEntityInScene(pmEntity))
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    BuildAndSync();
                }
                else
                {
                    m_bBuildAndSync = true;
                }
            }
        }

        private void EventManager_EntityFrozenStateChanged(MEntity pmEntity,
            bool bFrozen)
        {
            if (MFramework.Instance.Scene.IsEntityInScene(pmEntity))
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    BuildAndSync();
                }
                else
                {
                    m_bBuildAndSync = true;
                }
            }
        }

        private void EventManager_SelectionSetAddedToScene(MScene pmScene,
            MSelectionSet pmSelectionSet)
        {
            if (MFramework.Instance.Scene == pmScene)
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    SyncSelectionSetComboBox();
                }
                else
                {
                    m_bSyncSelectionSetComboBox = true;
                }
            }
        }

        private void EventManager_SelectionSetRemovedFromScene(MScene pmScene,
            MSelectionSet pmSelectionSet)
        {
            if (MFramework.Instance.Scene == pmScene)
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    SyncSelectionSetComboBox();
                }
                else
                {
                    m_bSyncSelectionSetComboBox = true;
                }
            }
        }

        private void EventManager_SelectionSetNameChanged(
            MSelectionSet pmSelectionSet, string strOldName, bool bInBatch)
        {
            if (MFramework.Instance.Scene.IsSelectionSetInScene(
                pmSelectionSet))
            {
                if (!MFramework.Instance.PerformingLongOperation)
                {
                    SyncSelectionSetComboBox();
                }
                else
                {
                    m_bSyncSelectionSetComboBox = true;
                }
            }
        }

        private void EventManager_LongOperationCompleted()
        {
            if (m_bBuildAndSync)
            {
                BuildAndSync();
            }
            else if (m_bSyncListBoxToSelectionService)
            {
                SyncListBoxToSelectionService();
            }

            if (m_bSyncToolbarButtons)
            {
                SyncToolbarButtons();
            }

            if (m_bSyncSelectionSetComboBox)
            {
                SyncSelectionSetComboBox();
            }

            if (m_bSyncTagsComboBox)
            {
                SyncTagsComboBox();
            }

            m_bBuildAndSync = false;
            m_bSyncListBoxToSelectionService = false;
            m_bSyncToolbarButtons = false;
            m_bSyncSelectionSetComboBox = false;
            m_bSyncTagsComboBox = false;
        }

        private void EventManager_SceneClosing(MScene pmScene)
        {
            if (MFramework.Instance.Scene == pmScene)
            {
                m_tvEntityList.Nodes.Clear();
            }
        }

        private void EventManager_TagsManagerUpdated()
        {
            if (!MFramework.Instance.PerformingLongOperation)
            {
                SyncTagsComboBox();
            }
            else
            {
                m_bSyncTagsComboBox = true;
            }
        }

        private void EventManager_FilterEnabledChanged()
        {
            SetEntityListColors();
            CheckFilterButtons();
            EventManager_FilterOptionsChanged();
        }

        private void EventManager_FilterOptionsChanged()
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bBuildAndSync = true;
            }
            else
            {
                BuildAndSync();
            }
        }

        #endregion

        #region Service Accessors
        private static ISelectionService ms_pmSelectionService = null;
        private static ISelectionService SelectionService
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

        private static ICommandService ms_pmCommandService = null;
        private static ICommandService CommandService
        {
            get
            {
                if (ms_pmCommandService == null)
                {
                    ms_pmCommandService = ServiceProvider.Instance
                        .GetService(typeof(ICommandService)) as
                        ICommandService;
                    Debug.Assert(ms_pmCommandService != null,
                        "Command service not found!");
                }
                return ms_pmCommandService;
            }
        }

        private static IMessageService ms_pmMessageService = null;
        private static IMessageService MessageService
        {
            get
            {
                if (ms_pmMessageService == null)
                {
                    ms_pmMessageService = ServiceProvider.Instance
                        .GetService(typeof(IMessageService)) as
                        IMessageService;
                    Debug.Assert(ms_pmMessageService != null,
                        "Message service not found!");
                }
                return ms_pmMessageService;
            }
        }

        private static ISelectionSetService ms_pmSelectionSetService = null;
        private static ISelectionSetService SelectionSetService
        {
            get
            {
                if (ms_pmSelectionSetService == null)
                {
                    ms_pmSelectionSetService = ServiceProvider.Instance
                        .GetService(typeof(ISelectionSetService)) as
                        ISelectionSetService;
                    Debug.Assert(ms_pmSelectionSetService != null,
                        "Selection set service not found!");
                }
                return ms_pmSelectionSetService;
            }
        }

        private static ISettingsService ms_pmSettingsService = null;
        private static ISettingsService SettingsService
        {
            get
            {
                if (ms_pmSettingsService == null)
                {
                    ms_pmSettingsService = ServiceProvider.Instance
                        .GetService(typeof(ISettingsService)) as
                        ISettingsService;
                    Debug.Assert(ms_pmSettingsService != null,
                        "Settings service not found!");
                }
                return ms_pmSettingsService;
            }
        }
        #endregion

        #region Private Helpers
        private void AttachToEventManager()
        {
            MEventManager pmEventMgr = MFramework.Instance.EventManager;
            pmEventMgr.SelectedEntitiesChanged += 
                new MEventManager.__Delegate_SelectedEntitiesChanged(
                EventManager_SelectedEntitiesChanged);
            pmEventMgr.EntityAddedToScene += 
                new MEventManager.__Delegate_EntityAddedToScene(
                EventManager_EntityAddedToScene);
            pmEventMgr.EntityRemovedFromScene += 
                new MEventManager.__Delegate_EntityRemovedFromScene(
                EventManager_EntityRemovedFromScene);
            pmEventMgr.EntityNameChanged +=
                new MEventManager.__Delegate_EntityNameChanged(
                EventManager_EntityNameChanged);
            pmEventMgr.EntityHiddenStateChanged += new MEventManager
                .__Delegate_EntityHiddenStateChanged(
                EventManager_EntityHiddenStateChanged);
            pmEventMgr.EntityFrozenStateChanged += new MEventManager
                .__Delegate_EntityFrozenStateChanged(
                EventManager_EntityFrozenStateChanged);
            pmEventMgr.SelectionSetAddedToScene += new MEventManager
                .__Delegate_SelectionSetAddedToScene(
                EventManager_SelectionSetAddedToScene);
            pmEventMgr.SelectionSetRemovedFromScene += new MEventManager
                .__Delegate_SelectionSetRemovedFromScene(
                EventManager_SelectionSetRemovedFromScene);
            pmEventMgr.SelectionSetNameChanged += new MEventManager
                .__Delegate_SelectionSetNameChanged(
                EventManager_SelectionSetNameChanged);
            pmEventMgr.NewSceneLoaded += new MEventManager
                .__Delegate_NewSceneLoaded(EventManager_NewSceneLoaded);
            pmEventMgr.LayerAdded += new MEventManager
                .__Delegate_LayerAdded(EventManager_NewLayerAdded);
            pmEventMgr.LayerDeleted += new MEventManager
                .__Delegate_LayerDeleted(EventManager_LayerDeleted);
            pmEventMgr.LayerFilenameRemoved += new MEventManager
                .__Delegate_LayerFilenameRemoved(
                EventManager_LayerFilenameRemoved);
            pmEventMgr.LongOperationCompleted += new MEventManager
                .__Delegate_LongOperationCompleted(
                EventManager_LongOperationCompleted);
            pmEventMgr.SceneClosing += new MEventManager
                .__Delegate_SceneClosing(EventManager_SceneClosing);
            pmEventMgr.TagsManagerUpdated += new MEventManager
                .__Delegate_TagsManagerUpdated(
                EventManager_TagsManagerUpdated);
            pmEventMgr.FilterEnabledChanged += new MEventManager
                .__Delegate_FilterEnabledChanged(
                EventManager_FilterEnabledChanged);
            pmEventMgr.FilterOptionsChanged +=
                new MEventManager.__Delegate_FilterOptionsChanged(
                EventManager_FilterOptionsChanged);
        }

        private void RegisterSettings()
        {
            SettingsService.RegisterSettingsObject(
                ms_strHiddenFilterSettingName, true,
                SettingsCategory.PerUser);
            SettingsService.RegisterSettingsObject(
                ms_strFrozenFilterSettingName, true,
                SettingsCategory.PerUser);
        }

        private void BuildAndSync()
        {
            BuildList();
            SyncListBoxToSelectionService();
        }

        private void BuildList()
        {
            m_bBuildingList = true;
            MEntity top = m_tvEntityList.TopNode != null ?
                m_tvEntityList.TopNode.Tag as MEntity :
                null;

            m_tvEntityList.BeginUpdate();
            m_tvEntityList.Nodes.Clear();

            m_pmEntityMap = new Dictionary<MEntity, TreeNode>();

            BuildList(null);

            m_tvEntityList.EndUpdate();

            if (top != null && m_pmEntityMap.ContainsKey(top))
            {
                m_tvEntityList.TopNode = m_pmEntityMap[top];
            }
            m_bBuildingList = false;
        }

        private TreeNodeCollection FindParent(MEntity child)
        {
            // we need to add the prefab root for this node
            MEntity prefabRoot = child.PrefabRoot;
            if (prefabRoot == null)
            {
                return m_tvEntityList.Nodes;
            }

            // see if our parent has already been added
            TreeNode parent;
            if (m_pmEntityMap.TryGetValue(prefabRoot, out parent))
            {
                return parent.Nodes;
            }
            // we need to add a parent node, find our grandparent
            TreeNodeCollection nodes = FindParent(prefabRoot);
            TreeNode node = nodes.Add(prefabRoot.Name);
            node.Tag = prefabRoot;
            m_pmEntityMap[prefabRoot] = node;
            return node.Nodes;
        }

        private bool MatchesFilter(MEntity pmEntity)
        {
            bool bMatchesFilter = false;
            if (m_eFilterState == FilterState.Hidden)
            {
                bMatchesFilter = pmEntity.Hidden;
            }
            else if (m_eFilterState == FilterState.Frozen)
            {
                bMatchesFilter = pmEntity.Frozen;
            }
            else
            {
                Debug.Assert(m_eFilterState == FilterState.Visible);
                bMatchesFilter = (!pmEntity.Hidden && !pmEntity.Frozen);
            }

            if (bMatchesFilter)
            {
                Debug.Assert(pmEntity.Layer != null,
                    "Entity with null layer!");
                if (!pmEntity.Layer.Visible || pmEntity.Layer.Locked)
                {
                    bMatchesFilter = false;
                }
            }

            return (bMatchesFilter && MFramework.Instance.FilterManager
                .EntityMatchesFilter(pmEntity));
        }

        private void BuildList(MEntity parent)
        {
            ArrayList pmEntityList = new ArrayList();

            MEntity[] amEntities = (parent == null ?
                MFramework.Instance.Scene.GetEntities() :
                parent.GetPrefabEntities());

            foreach (MEntity pmEntity in amEntities)
            {
                if (parent == pmEntity.PrefabRoot)
                {
                    pmEntityList.Add(pmEntity);
                }
            }
            pmEntityList.Sort(m_pmEntityComparer);

            TreeNodeCollection nodes = null;
            foreach (MEntity pmEntity in pmEntityList)
            {
                if (MatchesFilter(pmEntity))
                {
                    if (nodes == null)
                    {
                        nodes = FindParent(pmEntity);
                    }
                    TreeNode node = nodes.Add(pmEntity.Name);
                    node.Tag = pmEntity;
                    m_pmEntityMap[pmEntity] = node;
                }
                if (pmEntity.IsPrefabRoot)
                {
                    BuildList(pmEntity);
                }
            }
            if (parent != null && m_pmEntityMap.ContainsKey(parent))
            {
                bool shouldExpand = m_pmExpandedMap.ContainsKey(parent);
                bool matches = MatchesFilter(parent);
                if (shouldExpand && !matches)
                {
                    m_pmEntityMap[parent].Expand();
                }
                else
                {
                    m_pmEntityMap[parent].Collapse();
                }
            }
        }

        private void EnableSync()
        {
            m_bSynchingListBox = false;
            m_bSynchingSelectionService = false;
        }

        private void DisableSync()
        {
            m_bSynchingListBox = true;
            m_bSynchingSelectionService = true;
        }

        private bool ClearAllCheckmarks(TreeNodeCollection nodes)
        {
            bool bChecked = false;
            foreach (TreeNode node in nodes)
            {
                bChecked = bChecked || node.Checked;
                node.Checked = false;
                if (node.Nodes.Count > 0)
                {
                    bChecked = bChecked || ClearAllCheckmarks(node.Nodes);
                }
            }
            return bChecked;
        }

        private void SyncListBoxToSelectionService()
        {
            if (!m_bSynchingSelectionService &&
                m_eFilterState == FilterState.Visible)
            {
                m_bSynchingListBox = true;
                m_bEnableListBoxSelectedIndexChanged = false;

                ClearAllCheckmarks(m_tvEntityList.Nodes);

                MEntity[] amSelectedEntities = SelectionService
                    .GetSelectedEntities();

                for (int i = 0; i < amSelectedEntities.Length; i++)
                {
                    MEntity pmEntity = amSelectedEntities[i];
                    Debug.Assert(pmEntity != null, "Invalid entity in list " +
                        "box!");
                    if (m_pmEntityMap.ContainsKey(pmEntity))
                    {
                        TreeNode node = m_pmEntityMap[pmEntity];
                        node.Checked = true;
                        node.EnsureVisible();
                    }
                }
                m_bEnableListBoxSelectedIndexChanged = true;
                m_bSynchingListBox = false;
            }
        }

        private void SyncSelectionServiceToListBox()
        {
            if (!m_bSynchingListBox &&
                m_eFilterState == FilterState.Visible)
            {
                MEntity[] amEntitiesSelectedAtLastSync = SelectionService
                    .GetSelectedEntities();

                MEntity[] amEntitiesToAdd = 
                    BuildSelectionAdditionList(amEntitiesSelectedAtLastSync);
                MEntity[] amEntitiesToRemove = 
                    BuildSelectionRemovalList(amEntitiesSelectedAtLastSync);

                if (amEntitiesToRemove.Length > 0 ||
                    amEntitiesToAdd.Length > 0)
                {
                    int iNewNumSelectedEntities = SelectionService
                        .NumSelectedEntities - amEntitiesToRemove.Length +
                        amEntitiesToAdd.Length;
                    CommandService.BeginUndoFrame("Replace main selection " +
                        "with " + iNewNumSelectedEntities +
                        (iNewNumSelectedEntities == 1 ? " entity" :
                        " entities"));

                    m_bSynchingSelectionService = true;

                    SelectionService.RemoveEntitiesFromSelection(
                        amEntitiesToRemove);
                    SelectionService.AddEntitiesToSelection(amEntitiesToAdd);

                    m_bSynchingSelectionService = false;

                    CommandService.EndUndoFrame(SelectionService
                        .CommandsAreUndoable);
                }
            }
        }

        private MEntity[] BuildSelectionRemovalList(
            MEntity[] amEntitiesSelectedAtLastSync)
        {
            // Identify all items in the treeview that are not checked
            // but are still in the selection list so we can unselect them
            ArrayList pmReturnValue = new ArrayList();
            if (amEntitiesSelectedAtLastSync != null)
            {
                foreach (MEntity pmEntity in amEntitiesSelectedAtLastSync)
                {
                    TreeNode node = null;
                    if (m_pmEntityMap.TryGetValue(pmEntity, out node) &&
                        !node.Checked)
                    {
                        pmReturnValue.Add(pmEntity);
                    }
                }
            }
            return (MEntity[]) pmReturnValue.ToArray(typeof(MEntity));
        }

        private List<MEntity> FindAllCheckedItems(TreeNodeCollection nodes, 
            List<MEntity> selected)
        {
            foreach (TreeNode node in nodes)
            {
                if (node.Checked)
                {
                    selected.Add(node.Tag as MEntity);
                }
                if (node.Nodes.Count > 0)
                {
                    FindAllCheckedItems(node.Nodes, selected);
                }
            }
            return selected;
        }

        private MEntity[] BuildSelectionAdditionList(
            MEntity[] amEntitiesSelectedAtLastSync)
        {
            // identify all items in the treeview that are checked
            // that are not yet in the selection list so we can add them
            ArrayList pmReturnValue = new ArrayList();

            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());

            foreach (MEntity pmEntity in selected)
            {
                Debug.Assert(pmEntity != null, "Invalid entity in list box.");
                if (!MFramework.Instance.Scene.IsEntityInScene(pmEntity))
                {
                    SendNullEntityMessage(pmEntity.Name);
                    break;
                }
                bool bSelectedLastTime = false;
                if (amEntitiesSelectedAtLastSync != null)
                {
                    int iIndexInLastSelection = Array.IndexOf(
                        amEntitiesSelectedAtLastSync, pmEntity);
                    bSelectedLastTime = iIndexInLastSelection > 
                        (amEntitiesSelectedAtLastSync.GetLowerBound(0) 
                        - 1);
                }
                if (!bSelectedLastTime)
                {
                    pmReturnValue.Add(pmEntity);
                }
            }
            return (MEntity[]) pmReturnValue.ToArray(typeof(MEntity));
        }

        private static void SendNullEntityMessage(string strName)
        {
            PluginAPI.Message pmMessage = new PluginAPI.Message();

            pmMessage.m_strText = "Could not find entity";
            if (strName != null)
            {
                pmMessage.m_strText += ": " + strName;
            }
            pmMessage.m_strDetails = "The list box and the selection " +
                "manager appear to be out of sync.";
            MessageService.AddMessage(MessageChannelType.Errors, pmMessage);
        }

        private void SyncToolbarButtons()
        {
            // Sync hide/freeze buttons.

            bool bHideUnhideEnabled = false;
            string strHideUnhideToolTip = "Hide Entities";

            bool bFreezeUnfreezeEnabled = false;
            string strFreezeUnfreezeToolTip = "Freeze Entities";

            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());

            if (selected.Count > 0)
            {
                switch (m_eFilterState)
                {
                    case FilterState.Visible:
                        bHideUnhideEnabled = true;
                        bFreezeUnfreezeEnabled = true;
                        break;
                    case FilterState.Hidden:
                        bHideUnhideEnabled = true;
                        strHideUnhideToolTip = "Unhide Entities";
                        break;
                    case FilterState.Frozen:
                        bFreezeUnfreezeEnabled = true;
                        strFreezeUnfreezeToolTip = "Unfreeze Entities";
                        break;
                }
            }

            m_tbbHideUnhide.Enabled = bHideUnhideEnabled;
            m_tbbHideUnhide.ToolTipText = strHideUnhideToolTip;

            m_tbbFreezeUnfreeze.Enabled = bFreezeUnfreezeEnabled;
            m_tbbFreezeUnfreeze.ToolTipText = strFreezeUnfreezeToolTip;

            // Sync tags buttons.

            IUICommandService uiCommandService = (IUICommandService)
                ServiceProvider.Instance.GetService(typeof(IUICommandService));
            Debug.Assert(uiCommandService != null, "Null UI Command service!");

            UIState state = new UIState();
            UICommand command = uiCommandService.GetCommand(
                ms_strAddTagUICommandName);
            Debug.Assert(command != null, "Command not found!");
            command.ValidateCommand(state);
            m_tbbAddTagToSelectedEntities.Enabled = state.Enabled;

            state = new UIState();
            command = uiCommandService.GetCommand(
                ms_strRemoveTagsUICommandName);
            Debug.Assert(command != null, "Command not found!");
            command.ValidateCommand(state);
            m_tbbRemoveTagsFromSelectedEntities.Enabled = state.Enabled;

            state = new UIState();
            command = uiCommandService.GetCommand(
                ms_strEditCommonTagsUICommandName);
            Debug.Assert(command != null, "Command not found!");
            command.ValidateCommand(state);
            m_tbbEditCommonTagsOnSelectedEntities.Enabled = state.Enabled;
        }

        private void SetHiddenStateOnEntitiesInList(bool bHidden)
        {
            ArrayList pmEntitiesToChange = new ArrayList();
            ArrayList pmReadOnlyEntities = new ArrayList();
            ArrayList pmPrefabEntities   = new ArrayList();

            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());

            foreach (MEntity pmEntity in selected)
            {
                if (pmEntity.Hidden != bHidden)
                {
                    if (!bHidden && pmEntity.PrefabRoot != null &&
                        pmEntity.PrefabRoot.Hidden)
                    {
                        // we cannot unhide entites that belong 
                        // to hidden prefabs
                        pmPrefabEntities.Add(pmEntity);
                    } else if (pmEntity.Writable)
                    {
                        pmEntitiesToChange.Add(pmEntity);
                    }
                    else
                    {
                        pmReadOnlyEntities.Add(pmEntity);
                    }
                }
            }

            if (pmEntitiesToChange.Count > 0 || pmReadOnlyEntities.Count > 0
                || pmPrefabEntities.Count > 0)
            {
                string strCommand = bHidden ? "Hide " : "Unhide ";
                CommandService.BeginUndoFrame(strCommand +
                    pmEntitiesToChange.Count + " entities");
                foreach (MEntity pmEntity in pmEntitiesToChange)
                {
                    pmEntity.Hidden = bHidden;
                }
                CommandService.EndUndoFrame(true);

                if (pmReadOnlyEntities.Count > 0 || pmPrefabEntities.Count > 0)
                {
                    string strMessage = "";
                    if (pmReadOnlyEntities.Count > 0)
                    {
                        strMessage = "The following entities are " +
                            "read-only and were not modified:\n\n";
                        foreach (MEntity pmEntity in pmReadOnlyEntities)
                        {
                            strMessage += string.Format("\t{0}\n",
                                pmEntity.Name);
                        }
                    }
                    if (pmPrefabEntities.Count > 0)
                    {
                        strMessage += "The following entities have " +
                            "hidden prefab roots and were not modified:\n\n";
                        foreach (MEntity pmEntity in pmPrefabEntities)
                        {
                            strMessage += string.Format("\t{0}\n",
                                pmEntity.Name);
                        }
                    }

                    MessageService.AddMessage(MessageChannelType.General,
                        new Emergent.Gamebryo.SceneDesigner.PluginAPI.Message(
                        string.Format("Some entities were not {0}.", bHidden ?
                        "hidden" : "unhidden"), strMessage, null));
                }
            }
        }

        private void SetFrozenStateOnEntitiesInList(bool bFrozen)
        {
            ArrayList pmEntitiesToChange = new ArrayList();
            ArrayList pmReadOnlyEntities = new ArrayList();
            ArrayList pmPrefabEntities = new ArrayList();

            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());

            foreach (MEntity pmEntity in selected)
            {
                if (pmEntity.Frozen != bFrozen)
                {
                    if (!bFrozen && pmEntity.PrefabRoot != null &&
                        pmEntity.PrefabRoot.Frozen)
                    {
                        // we cannot unfreeze entites that belong 
                        // to frozen prefabs
                        pmPrefabEntities.Add(pmEntity);
                    }
                    else if (pmEntity.Writable)
                    {
                        pmEntitiesToChange.Add(pmEntity);
                    }
                    else
                    {
                        pmReadOnlyEntities.Add(pmEntity);
                    }
                }
            }

            if (pmEntitiesToChange.Count > 0 || pmReadOnlyEntities.Count > 0 || 
                pmPrefabEntities.Count > 0)
            {
                string strCommand = bFrozen ? "Freeze " : "Unfreeze ";
                CommandService.BeginUndoFrame(strCommand +
                    pmEntitiesToChange.Count + " entities");
                foreach (MEntity pmEntity in pmEntitiesToChange)
                {
                    pmEntity.Frozen = bFrozen;
                }
                CommandService.EndUndoFrame(true);

                if (pmReadOnlyEntities.Count > 0 || pmPrefabEntities.Count > 0)
                {
                    string strMessage = "";
                    if (pmReadOnlyEntities.Count > 0)
                    {
                        strMessage = "The following entities are " +
                            "read-only and were not modified:\n\n";
                        foreach (MEntity pmEntity in pmReadOnlyEntities)
                        {
                            strMessage += string.Format("\t{0}\n", pmEntity.Name);
                        }
                    }
                    if (pmPrefabEntities.Count > 0)
                    {
                        strMessage += "The following entities have " +
                            "frozen prefab roots and were not modified:\n\n";
                        foreach (MEntity pmEntity in pmPrefabEntities)
                        {
                            strMessage += string.Format("\t{0}\n",
                                pmEntity.Name);
                        }
                    }
                    MessageService.AddMessage(MessageChannelType.General,
                        new Emergent.Gamebryo.SceneDesigner.PluginAPI.Message(
                        string.Format("Some entities were not {0}.", bFrozen ?
                        "frozen" : "unfrozen"), strMessage, null));
                }
            }
        }

        private void SyncSelectionSetComboBox()
        {
            m_cbSelectionSets.Items.Clear();

            MSelectionSet[] amSelectionSets = SelectionSetService
                .GetSelectionSets();
            if (amSelectionSets != null)
            {
                foreach (MSelectionSet pmSelectionSet in amSelectionSets)
                {
                    m_cbSelectionSets.Items.Add(pmSelectionSet);
                }
            }
            m_cbSelectionSets.SelectedItem = null;
        }

        private void SyncTagsComboBox()
        {
            m_cbTags.Items.Clear();

            string[] tags = MFramework.Instance.TagsManager.GetTags();
            Debug.Assert(tags != null, "Null tags array!");
            foreach (string tag in tags)
            {
                m_cbTags.Items.Add(tag);
            }
            m_cbTags.SelectedItem = null;
        }

        private void ChangeFilterState(FilterState eFilterState)
        {
            bool bDisplayWarning;
            switch (eFilterState)
            {
                case FilterState.Hidden:
                    bDisplayWarning = (bool) SettingsService
                        .GetSettingsObject(ms_strHiddenFilterSettingName,
                        SettingsCategory.PerUser);
                    if (bDisplayWarning)
                    {
                        MessageBox.Show("The \"Hidden\" filter has just " +
                            "been activated. Only hidden entities will\n" +
                            "be displayed in the Selection Panel while " +
                            "this filter is active.\nEntities selected in " +
                            "the list box will not be selected in the " +
                            "scene.\nRe-enable the \"Visible\" filter to " +
                            "select entities in the scene.\n\nThis message " +
                            "will only be displayed once.",
                            "Hidden Filter Selected", MessageBoxButtons.OK,
                            MessageBoxIcon.Information);

                        SettingsService.SetSettingsObject(
                            ms_strHiddenFilterSettingName, false,
                            SettingsCategory.PerUser);
                    }
                    break;
                case FilterState.Frozen:
                    bDisplayWarning = (bool) SettingsService
                        .GetSettingsObject(ms_strFrozenFilterSettingName,
                        SettingsCategory.PerUser);
                    if (bDisplayWarning)
                    {
                        MessageBox.Show("The \"Frozen\" filter has just " +
                            "been activated. Only frozen entities will\n" +
                            "be displayed in the Selection Panel while " +
                            "this filter is active.\nEntities selected in " +
                            "the list box will not be selected in the " +
                            "scene.\nRe-enable the \"Visible\" filter to " +
                            "select entities in the scene.\n\nThis message " +
                            "will only be displayed once.",
                            "Frozen Filter Selected", MessageBoxButtons.OK,
                            MessageBoxIcon.Information);

                        SettingsService.SetSettingsObject(
                            ms_strFrozenFilterSettingName, false,
                            SettingsCategory.PerUser);
                    }
                    break;
            }

            m_eFilterState = eFilterState;
            BuildAndSync();
            SyncToolbarButtons();
            SyncFilterButton();
        }

        private void SyncFilterButton()
        {
            string strToolTip = "Change Filter:\n";
            switch (m_eFilterState)
            {
                case FilterState.Visible:
                    m_tbbFilter.ToolTipText = strToolTip +
                        "Visible Entities Shown";
                    m_tbbFilter.ImageIndex = 5;
                    break;
                case FilterState.Hidden:
                    m_tbbFilter.ToolTipText = strToolTip +
                        "Hidden Entities Shown";
                    m_tbbFilter.ImageIndex = 6;
                    break;
                case FilterState.Frozen:
                    m_tbbFilter.ToolTipText = strToolTip +
                        "Frozen Entities Shown";
                    m_tbbFilter.ImageIndex = 7;
                    break;
            }
        }

        private ContextMenu BuildContextMenu()
        {
            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());

            if (selected.Count > 0)
            {
                ContextMenu menu = new ContextMenu();

                switch (m_eFilterState)
                {
                    case FilterState.Visible:
                    {
                        if (selected.Count == 1 && selected[0].IsPrefabRoot)
                        {
                            if (m_pmEntityMap[selected[0]].IsExpanded)
                            {
                                menu.MenuItems.Add(
                                    "Collapse Prefab Entities",
                                    new EventHandler(OnContextCollapse));
                            }
                            else
                            {
                                menu.MenuItems.Add(
                                    "Expand Prefab Entities",
                                    new EventHandler(OnContextExpand));
                            }
                        }

                        menu.MenuItems.Add("Expand All",
                            new EventHandler(OnContextExpandAll));
                        menu.MenuItems.Add("Collapse All",
                            new EventHandler(OnContextCollapseAll));

                        menu.MenuItems.Add("-");

                        menu.MenuItems.Add("Hide",
                            new EventHandler(OnContextHide));
                        menu.MenuItems.Add("Freeze",
                            new EventHandler(OnContextFreeze));
                        menu.MenuItems.Add("Delete",
                            new EventHandler(OnContextDelete));

                        if (selected.Count == 1)
                        {
                            menu.MenuItems.Add("-");
                            menu.MenuItems.Add("Look At",
                                new EventHandler(OnContextLookAt));
                            menu.MenuItems.Add("Zoom Extents",
                                new EventHandler(OnContextZoomExtents));
                            menu.MenuItems.Add("Show Template",
                                new EventHandler(OnContextShowTemplate));
                            menu.MenuItems.Add("Properties",
                                new EventHandler(OnContextProperties));
                        }
                        else if (selected.Count > 1)
                        {
                            menu.MenuItems.Add("-");

                            menu.MenuItems.Add(
                                "Create Prefab from Selected Entities",
                                new EventHandler(OnContextCreatePrefab));
                        }

                        break;
                    }
                    case FilterState.Frozen:
                    {
                        MenuItem unfreezeItem = menu.MenuItems.Add("Unfreeze",
                            new EventHandler(OnContextUnfreeze));
                        break;
                    }
                    case FilterState.Hidden:
                    {
                        MenuItem unhideItem = menu.MenuItems.Add("Unhide",
                            new EventHandler(OnContextUnhide));
                        break;
                    }
                }

                return menu;
            }
            return null;
        }


        private void OnContextZoomExtents(object sender, EventArgs e)
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IUICommandService uiCommandService = 
                sp.GetService(typeof(IUICommandService)) as IUICommandService;

            UICommand zoomCommand =                
                uiCommandService.GetCommand("MoveToSelection");
            UIState state = new UIState();
            zoomCommand.ValidateCommand(state);
            if (state.Enabled)
            {
                zoomCommand.DoClick(this, null);
            }
        }

        private void OnContextLookAt(object sender, EventArgs e)
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IUICommandService uiCommandService = 
                sp.GetService(typeof(IUICommandService)) as IUICommandService;

            UICommand lookAtCommand =                
                uiCommandService.GetCommand("LookAtSelection");
            UIState state = new UIState();
            lookAtCommand.ValidateCommand(state);
            if (state.Enabled)
            {
                lookAtCommand.DoClick(this, null);
            }
        }

        private void OnContextUnhide(object sender, EventArgs e)
        {
            SetHiddenStateOnEntitiesInList(false);
            SyncToolbarButtons();
        }

        private void OnContextUnfreeze(object sender, EventArgs e)
        {
            SetFrozenStateOnEntitiesInList(false);
            SyncToolbarButtons();
        }

        private void OnContextShowTemplate(object sender, EventArgs e)
        {
            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());

            MEntity entity = selected[0] as MEntity;
            if (entity != null)
            {
                MEntity rootMasterEntity = entity.RootMasterEntity;
                MPalette palette = null;
                if (rootMasterEntity != null)
                {
                    IEntityPathService pathService = ServiceProvider.Instance
                        .GetService(typeof(IEntityPathService)) as
                        IEntityPathService;
                    palette = pathService.FindPaletteContainingEntity(
                        rootMasterEntity);
                }

                if (palette != null)
                {
                    ICommandPanelService panelService = ServiceProvider
                        .Instance.GetService(typeof(ICommandPanelService)) as
                        ICommandPanelService;
                    panelService.ShowPanel("Template Properties", true);
                    panelService.ShowPanel("Palettes", true);
                    MFramework.Instance.PaletteManager.ActivePalette =
                        palette;
                    palette.ActiveEntity = rootMasterEntity;
                }
                else
                {
                    MessageBox.Show(
                        "Selected entity does not inherit from a template.", 
                        "Cannot Show Template", MessageBoxButtons.OK,
                        MessageBoxIcon.Information);
                }
            }
        }

        private void OnContextFreeze(object sender, EventArgs e)
        {
            SetFrozenStateOnEntitiesInList(true);
            SyncToolbarButtons();
        }

        private void OnContextHide(object sender, EventArgs e)
        {
            SetHiddenStateOnEntitiesInList(true);	        
            SyncToolbarButtons();
        }

        private void OnContextDelete(object sender, EventArgs e)
        {	        
            IUICommandService uiCommandService = 
                ServiceProvider.Instance.GetService(
                typeof(IUICommandService)) as IUICommandService;
            UICommand command = 
                uiCommandService.GetCommand("DeleteSelectedEntities");
            UIState state = new UIState();
            command.ValidateCommand(state);
            if (state.Enabled)
            {
                command.DoClick(command, null);
            }

            SyncToolbarButtons();
        }

        private void OnContextProperties(object sender, EventArgs e)
        {
            ServiceProvider sp = ServiceProvider.Instance;
            ICommandPanelService panelService = sp.GetService(
                typeof(ICommandPanelService)) as ICommandPanelService;
            panelService.ShowPanel("Entity Properties", true);
        }

        private void OnContextExpand(object sender, EventArgs e)
        {
            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());
            if (selected.Count == 1)
            {
                m_pmEntityMap[selected[0]].Expand();
            }
        }

        private void OnContextCollapse(object sender, EventArgs e)
        {
            List<MEntity> selected = FindAllCheckedItems(
                m_tvEntityList.Nodes, new List<MEntity>());
            if (selected.Count == 1)
            {
                m_pmEntityMap[selected[0]].Collapse();
            }
        }

        private void OnContextExpandAll(object sender, EventArgs e)
        {
            m_tvEntityList.ExpandAll();
        }

        private void OnContextCollapseAll(object sender, EventArgs e)
        {
            m_tvEntityList.CollapseAll();
        }

        private void OnContextCreatePrefab(object sender, EventArgs e)
        {
            IUICommandService service = (IUICommandService)
                ServiceProvider.Instance.GetService(typeof(IUICommandService));
            Debug.Assert(service != null, "Null UICommandService!");

            UICommand command = service.GetCommand("CreatePrefabFromSelected");
            if (command != null)
            {
                command.DoClick(null, null);
            }
        }

        private void SetEntityListColors()
        {
            if (MFramework.Instance.FilterManager.FilterEnabled)
            {
                m_pnlEntityListBorder.Visible = true;
            }
            else
            {
                m_pnlEntityListBorder.Visible = false;
            }
        }

        private void CheckFilterButtons()
        {
            bool bEnabled = MFramework.Instance.FilterManager.FilterEnabled;
            m_tbbEnableSelectionFilter.Pushed = bEnabled;
            m_tbbEnableSelectionFilter.Enabled = !bEnabled;
            m_tbbDisableSelectionFilter.Pushed = !bEnabled;
            m_tbbDisableSelectionFilter.Enabled = bEnabled;
        }

        #endregion

        #region WinForms handlers
        private void SelectionPanel_Load(object sender, System.EventArgs e)
        {
            AttachToEventManager();
            RegisterSettings();
            BuildList();
            SyncToolbarButtons();
            SyncFilterButton();
            SyncSelectionSetComboBox();
            SetEntityListColors();
            CheckFilterButtons();
        }

        private void m_tbToolbar_ButtonClick(object sender,
            System.Windows.Forms.ToolBarButtonClickEventArgs e)
        {
            if (e.Button == m_tbbSelectAll)
            {
                m_tbbSelectAll_Click(sender, e);
            }
            else if (e.Button == m_tbbSelectNone)
            {
                m_tbbSelectNone_Click(sender, e);
            }
            else if (e.Button == m_tbbInvertSelection)
            {
                m_tbbInvertSelection_Click(sender, e);
            }
            else if (e.Button == m_tbbHideUnhide)
            {
                m_tbbHideUnhide_Click(sender, e);
            }
            else if (e.Button == m_tbbFreezeUnfreeze)
            {
                m_tbbFreezeUnfreeze_Click(sender, e);
            }
            else if (e.Button == m_tbbFilter)
            {
                bool bSetNext = false;
                foreach (FilterState eFilterState in Enum.GetValues(typeof(
                    FilterState)))
                {
                    if (eFilterState == m_eFilterState)
                    {
                        bSetNext = true;
                    }
                    else if (bSetNext)
                    {
                        ChangeFilterState(eFilterState);
                        bSetNext = false;
                        break;
                    }
                }
                if (bSetNext)
                {
                    // The current filter state is the last one in the enum.
                    // Change it to the first filter state in the enum.
                    Debug.Assert(Enum.GetValues(typeof(FilterState)).Length >
                        0, "Invalid enumeration value array!");
                    ChangeFilterState((FilterState) Enum.GetValues(typeof(
                        FilterState)).GetValue(0));
                }
            }
            else if (e.Button == m_tbbAddTagToSelectedEntities)
            {
                m_tbbAddTagToSelectedEntities_Click(sender, e);
            }
            else if (e.Button == m_tbbRemoveTagsFromSelectedEntities)
            {
                m_tbbRemoveTagsFromSelectedEntities_Click(sender, e);
            }
            else if (e.Button == m_tbbEditCommonTagsOnSelectedEntities)
            {
                m_tbbEditCommonTagsOnSelectedEntities_Click(sender, e);
            }
            else if (e.Button == m_tbbEnableSelectionFilter)
            {
                m_tbbEnableSelectionFilter_Click(sender, e);
            }
            else if (e.Button == m_tbbDisableSelectionFilter)
            {
                m_tbbDisableSelectionFilter_Click(sender, e);
            }
        }

        private void m_tbToolbar_ButtonDropDown(object sender,
            System.Windows.Forms.ToolBarButtonClickEventArgs e)
        {
            if (e.Button == m_tbbFilter)
            {
                m_tbbFilter.DropDownMenu = new ContextMenu();
                foreach (string strFilterStateName in Enum.GetNames(typeof(
                    FilterState)))
                {
                    MenuItem pmMenuItem = new MenuItem(strFilterStateName,
                        new EventHandler(m_tbbFilter_MenuItemClick));
                    if (object.Equals(m_eFilterState.ToString(),
                        pmMenuItem.Text))
                    {
                        pmMenuItem.Checked = true;
                    }
                    m_tbbFilter.DropDownMenu.MenuItems.Add(pmMenuItem);
                }
            }
        }

        private void SelectTopLevelNodes(TreeNodeCollection nodes)
        {
            // select all top level entities
            // (this will not select sub entities of prefabs because
            // they are not in the top level)
            foreach (TreeNode node in nodes)
            {
                if (MatchesFilter(node.Tag as MEntity))
                {
                    node.Checked = true;
                    // make sure any prefab entities are not checked
                    if (node.Nodes.Count > 0)
                    {
                        ClearAllCheckmarks(node.Nodes);
                    }
                }
                else
                {
                    node.Checked = false;
                    SelectTopLevelNodes(node.Nodes);
                }
            }
        }

        private void m_tbbSelectAll_Click(object sender, System.EventArgs e)
        {   
            DisableSync();
            SelectionService.ClearSelectedEntities();
            m_tvEntityList.BeginUpdate();
            m_bEnableListBoxSelectedIndexChanged = false;
            SelectTopLevelNodes(m_tvEntityList.Nodes);
            m_bEnableListBoxSelectedIndexChanged = true;
            m_tvEntityList.EndUpdate();
            EnableSync();
            SyncSelectionServiceToListBox();
            SyncToolbarButtons();
        }

        private void m_tbbSelectNone_Click(object sender, System.EventArgs e)
        {
            DisableSync();
            SelectionService.ClearSelectedEntities();
            m_tvEntityList.BeginUpdate();
            m_bEnableListBoxSelectedIndexChanged = false;

            ClearAllCheckmarks(m_tvEntityList.Nodes);
            m_LastNode = null;

            m_bEnableListBoxSelectedIndexChanged = true;
            m_tvEntityList.EndUpdate();        
            EnableSync();
            SyncSelectionServiceToListBox();
            SyncToolbarButtons();
        }

        private void m_tbbInvertSelection_Click(object sender,
            System.EventArgs e)
        {
            DisableSync();
            SelectionService.ClearSelectedEntities();
            m_tvEntityList.BeginUpdate();
            m_bEnableListBoxSelectedIndexChanged = false;

            // invert all top level entities selection state
            // (this will not select sub entities of prefabs because
            // there are not in the top level)
            foreach (TreeNode node in m_tvEntityList.Nodes)
            {
                bool bChecked = node.Checked;
                // make sure any prefab entities are not checked
                if (node.Nodes.Count > 0)
                {
                    bChecked = bChecked || ClearAllCheckmarks(node.Nodes);
                }
                node.Checked = !bChecked;
            }
            m_bEnableListBoxSelectedIndexChanged = true;
            m_tvEntityList.EndUpdate();    
            EnableSync();
            SyncSelectionServiceToListBox();
            SyncToolbarButtons();
        }

        TreeNode FindNodeStartingWithText(String txt, TreeNodeCollection nodes)
        {
            foreach (TreeNode node in nodes)
            {
                String name = node.Text.ToLower();
                if (name.StartsWith(txt))
                {
                    return node;
                }
                if (node.IsExpanded)
                {
                    TreeNode n = FindNodeStartingWithText(txt, node.Nodes);
                    if (n != null)
                    {
                        return n;
                    }
                }
            }
            return null;
        }

        private void m_tbEntityName_TextChanged(object sender,
            System.EventArgs e)
        {
            String search = m_tbEntityName.Text.ToLower();
            TreeNode node = FindNodeStartingWithText(search, 
                m_tvEntityList.Nodes);
            if (node != null)
            {
                m_tvEntityList.TopNode = node;
            }
        }

        private void m_tbbHideUnhide_Click(object sender, System.EventArgs e)
        {
            SetHiddenStateOnEntitiesInList(m_eFilterState !=
                FilterState.Hidden);
            SyncToolbarButtons();
        }

        private void m_tbbFreezeUnfreeze_Click(object sender,
            System.EventArgs e)
        {
            SetFrozenStateOnEntitiesInList(m_eFilterState !=
                FilterState.Frozen);
            SyncToolbarButtons();
        }

        private void m_tbbFilter_MenuItemClick(object sender,
            System.EventArgs e)
        {
            MenuItem pmMenuItem = (MenuItem) sender;
            ChangeFilterState((FilterState) Enum.Parse(typeof(FilterState),
                pmMenuItem.Text));
        }

        private void m_tbbAddTagToSelectedEntities_Click(object sender,
            System.EventArgs e)
        {
            IUICommandService uiCommandService = (IUICommandService)
                ServiceProvider.Instance.GetService(typeof(IUICommandService));
            Debug.Assert(uiCommandService != null, "Null UI Command service!");

            UICommand command = uiCommandService.GetCommand(
                ms_strAddTagUICommandName);
            Debug.Assert(command != null, "Command not found!");
            command.DoClick(sender, e);
        }

        private void m_tbbRemoveTagsFromSelectedEntities_Click(object sender,
            System.EventArgs e)
        {
            IUICommandService uiCommandService = (IUICommandService)
                ServiceProvider.Instance.GetService(typeof(IUICommandService));
            Debug.Assert(uiCommandService != null, "Null UI Command service!");

            UICommand command = uiCommandService.GetCommand(
                ms_strRemoveTagsUICommandName);
            Debug.Assert(command != null, "Command not found!");
            command.DoClick(sender, e);
        }

        private void m_tbbEditCommonTagsOnSelectedEntities_Click(object sender,
            System.EventArgs e)
        {
            IUICommandService uiCommandService = (IUICommandService)
                ServiceProvider.Instance.GetService(typeof(IUICommandService));
            Debug.Assert(uiCommandService != null, "Null UI Command service!");

            UICommand command = uiCommandService.GetCommand(
                ms_strEditCommonTagsUICommandName);
            Debug.Assert(command != null, "Command not found!");
            command.DoClick(sender, e);
        }

        private void m_tbbEnableSelectionFilter_Click(object sender,
            System.EventArgs e)
        {
            MFramework.Instance.FilterManager.FilterEnabled = true;

            // Show Selection Filter Panel.
            ICommandPanelService panelService = (ICommandPanelService)
                ServiceProvider.Instance.GetService(typeof(
                ICommandPanelService));
            Form panel = panelService.GetPanel("Selection Filter");
            if (panel != null)
            {
                panelService.ShowPanel("Selection Filter", true);
            }
        }

        private void m_tbbDisableSelectionFilter_Click(object sender,
            System.EventArgs e)
        {
            MFramework.Instance.FilterManager.FilterEnabled = false;
        }

        private void m_tvEntityList_DrawNode(object sender, 
            DrawTreeNodeEventArgs e)
        {
            if (e.Bounds.Height == 0 || e.Bounds.Width == 0)
            {
                return;
            }

            MEntity pmEntity = e.Node.Tag as MEntity;
            Debug.Assert(pmEntity != null, "Invalid entity in list!");

            Font pmFont = m_tvEntityList.Font;
            if (pmEntity.Hidden)
            {
                pmFont = new Font(pmFont, FontStyle.Italic);
            }

            Brush pmBrush;
            if (pmEntity.Frozen)
            {
                pmBrush = e.Node.Checked ? SystemBrushes.HighlightText :
                    Brushes.Blue;
            }
            else if (!MatchesFilter(pmEntity))
            {
                pmBrush = SystemBrushes.GrayText;
            }
            else
            {
                pmBrush = e.Node.Checked ? SystemBrushes.HighlightText : 
                    new SolidBrush(m_tvEntityList.ForeColor);
            }


            Brush background = e.Node.Checked ? SystemBrushes.Highlight :
                new SolidBrush(m_tvEntityList.BackColor);

            string strDrawString = pmEntity.Name;
            if (pmEntity.IsPrefabRoot)
            {
                strDrawString = String.Format("[{0}]", strDrawString);
            }

            Rectangle bounds = e.Bounds;
            SizeF size = e.Graphics.MeasureString(strDrawString, pmFont);
            // add one so we don't lose the fractional part
            bounds.Width = (int)(size.Width + 1); 

            e.Graphics.FillRectangle(background, bounds);
            e.Graphics.DrawString(strDrawString, pmFont, pmBrush, bounds);
        }

        private bool m_DoubleClick = false;
        private long m_LastClickTime = 0;
        private TreeNode m_LastNode = null;

        private void m_tvEntityList_BeforeExpand(object sender, 
            TreeViewCancelEventArgs e)
        {
            if (m_DoubleClick)
            {
                m_DoubleClick = false;
                if (e.Action == TreeViewAction.Expand)
                {
                    e.Cancel = true;
                    return;
                }
            }
            if (m_eFilterState == FilterState.Visible)
            {
                m_pmExpandedMap[e.Node.Tag as MEntity] = true;
            }
        }

        private void m_tvEntityList_BeforeCollapse(object sender, 
            TreeViewCancelEventArgs e)
        {
            if (m_DoubleClick)
            {
                m_DoubleClick = false;
                if (e.Action == TreeViewAction.Collapse)
                {
                    e.Cancel = true;
                    return;
                }
            }
            if (m_eFilterState == FilterState.Visible)
            {
                m_pmExpandedMap.Remove(e.Node.Tag as MEntity);
            }
        }

        private void m_tvEntityList_BeforeSelect(object sender, 
            TreeViewCancelEventArgs e)
        {
            if (!m_bBuildingList)
            {
                MEntity pmEntity = e.Node.Tag as MEntity;
                if (pmEntity.IsPrefabRoot)
                {
                    if (!MatchesFilter(pmEntity))
                    {
                        e.Cancel = true;
                    }
                }
            }
        }

        private void m_tvEntityList_AfterSelect(object sender,
            TreeViewEventArgs e)
        {
            if (e.Action == TreeViewAction.ByKeyboard)
            {
                if ((ModifierKeys & Keys.Shift) != Keys.Shift ||
                    (m_LastNode != null && m_LastNode.Parent != e.Node.Parent))
                {
                    ClearAllCheckmarks(m_tvEntityList.Nodes);
                    m_LastNode = e.Node;
                }
                SelectAllBetween(m_LastNode, e.Node);
            }
            if (m_bEnableListBoxSelectedIndexChanged)
            {
                SyncSelectionServiceToListBox();
                SyncToolbarButtons();
            }
        }

        private void SelectAllBetween(TreeNode begin, TreeNode end)
        {
            Debug.Assert(begin.Parent == end.Parent);

            m_tvEntityList.BeginUpdate();

            TreeNodeCollection nodes = begin.Parent == null ? 
                m_tvEntityList.Nodes : begin.Parent.Nodes; 

            //ClearAllCheckmarks(nodes);

            if (begin.Index > end.Index)
            {
                TreeNode tmp = begin;
                begin = end;
                end = tmp;
            }
            bool state = false;
            foreach (TreeNode node in nodes)
            {
                if (node == begin)
                {
                    state = true;
                }
                node.Checked = state;
                if (node == end)
                {
                    state = false;
                }
            }

            m_tvEntityList.EndUpdate();
        }

        private void m_tvEntityList_MouseDown(object sender, MouseEventArgs e)
        {
            Capture = true;
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                long dblClickTime = SystemInformation.DoubleClickTime;
                long now = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                Debug.Print(String.Format("elapsed: {0}", now - m_LastClickTime));
                if (now - m_LastClickTime <= dblClickTime)
                {
                    m_DoubleClick = true;
                }
                else
                {
                    m_DoubleClick = false;

                    TreeNode clicked = m_tvEntityList.GetNodeAt(e.X, e.Y);
                    if (clicked != null)
                    {
                        if ((ModifierKeys & Keys.Shift) == Keys.Shift)
                        {
                            ClearAllCheckmarks(m_tvEntityList.Nodes);
                            // if we have a "last node" then we can extend
                            // the selection.
                            if (m_LastNode != null)
                            {
                                if (clicked.Parent == m_LastNode.Parent)
                                {
                                    // select all items between last node and
                                    // this node
                                    SelectAllBetween(clicked, m_LastNode);
                                }
                            }
                            else
                            {
                                // just select the one they clicked on
                                clicked.Checked = true;
                                m_LastNode = clicked;
                            }
                        }
                        else
                        {
                            // Don't extend the selection, but
                            // we may be adding to it
                            if ((ModifierKeys & Keys.Control) != Keys.Control)
                            {
                                SelectionService.ClearSelectedEntities();
                                ClearAllCheckmarks(m_tvEntityList.Nodes);
                            }
                            if (clicked.Checked)
                            {
                                clicked.Checked = false;
                            }
                            else
                            {
                                MEntity pmEntity = clicked.Tag as MEntity;
                                if (pmEntity.PrefabRoot != null)
                                {
                                    if (m_pmEntityMap[pmEntity.PrefabRoot].Checked)
                                    {
                                        m_pmEntityMap[pmEntity.PrefabRoot].
                                            Checked = false;
                                    }
                                }
                                else if (clicked.Nodes.Count > 0)
                                {
                                    // this is a prefab root, make sure it's children
                                    // are not selected.
                                    ClearAllCheckmarks(clicked.Nodes);
                                }

                                clicked.Checked = true;
                            }
                            m_LastNode = clicked;
                        }
                    }
                }
                m_LastClickTime = now;
            }
        }

        private void m_tvEntityList_MouseUp(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            Capture = false;
            if ((e.Button & MouseButtons.Right) == MouseButtons.Right)
            {
                ContextMenu menu = BuildContextMenu();
                if (menu != null)
                {
                    menu.Show(m_tvEntityList, new Point(e.X, e.Y));
                }
            }

            if (m_bEnableListBoxSelectedIndexChanged)
            {
                SyncSelectionServiceToListBox();
                SyncToolbarButtons();
            }
        }

        private void m_tvEntityList_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                TreeNode clickedNode = m_tvEntityList.GetNodeAt(e.X, e.Y);
                // try to find the same level as the last selected node
                while (clickedNode != null && 
                       clickedNode.Parent != m_LastNode.Parent)
                {
                    clickedNode = clickedNode.Parent;
                }
                if (clickedNode != null)
                {
                    SelectAllBetween(clickedNode, m_LastNode);
                }
            }
        }

        private void m_tvEntityList_MouseDoubleClick(object sender, 
            MouseEventArgs e)
        {
            switch (m_eFilterState)
            {
                case FilterState.Hidden:
                    SetHiddenStateOnEntitiesInList(false);
                    break;
                case FilterState.Frozen:
                    SetFrozenStateOnEntitiesInList(false);
                    break;
                case FilterState.Visible:
                    OnContextZoomExtents(sender, e);
                    //OnContextLookAt(sender, e);
                    break;
            }
        }

        private void m_tvEntityList_MouseClick(object sender, 
            MouseEventArgs e)
        {
            TreeNode clickedNode = m_tvEntityList.GetNodeAt(e.X, e.Y);

            MEntity pmEntity = clickedNode.Tag as MEntity;
            if (!MatchesFilter(pmEntity))
            {
                return;
            }

            if ((e.Button & MouseButtons.Right) == MouseButtons.Right)
            {
                if (!clickedNode.Checked)
                {
                    ClearAllCheckmarks(m_tvEntityList.Nodes);
                    clickedNode.Checked = true;
                    m_tvEntityList.SelectedNode = clickedNode;
                    SyncSelectionServiceToListBox();
                }
            }
        }

        private void m_cbSelectionSets_SelectedIndexChanged(object sender,
            System.EventArgs e)
        {
            if (m_cbSelectionSets.SelectedItem != null)
            {
                MSelectionSet pmSelectionSet = m_cbSelectionSets.SelectedItem
                    as MSelectionSet;
                Debug.Assert(pmSelectionSet != null, "Invalid selection " +
                    "set in combo box!");
                SelectionService.ReplaceSelection(pmSelectionSet);

                m_cbSelectionSets.SelectedItem = null;
            }
        }

        private void m_cbTags_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (m_cbTags.SelectedItem != null)
            {
                string tag = m_cbTags.SelectedItem as string;
                Debug.Assert(tag != null, "Invalid tag in combo box!");

                MEntity[] tagEntities = MFramework.Instance.TagsManager
                    .GetEntitiesForTag(tag);
                List<MEntity> entitiesToSelect = new List<MEntity>(
                    tagEntities.Length);
                List<MEntity> entitiesToRemove = new List<MEntity>(
                    tagEntities.Length);
                foreach (MEntity entity in tagEntities)
                {
                    MEntity prefabRoot = entity.PrefabRoot;
                    if (prefabRoot != null)
                    {
                        if (MFramework.Instance.PrefabManager
                            .PickPrefabEntities)
                        {
                            entitiesToSelect.Add(entity);
                            if (!entitiesToRemove.Contains(prefabRoot))
                            {
                                entitiesToRemove.Add(prefabRoot);
                            }
                        }
                        else
                        {
                            if (!entitiesToSelect.Contains(prefabRoot))
                            {
                                entitiesToSelect.Add(prefabRoot);
                            }
                        }
                    }
                    else
                    {
                        entitiesToSelect.Add(entity);
                    }
                }
                foreach (MEntity entity in entitiesToRemove)
                {
                    entitiesToSelect.Remove(entity);
                }

                SelectionService.ReplaceSelection(entitiesToSelect.ToArray());

                m_cbTags.SelectedItem = null;
            }
        }

        private void OnKeyDown(object sender, 
            System.Windows.Forms.KeyEventArgs e)
        {
            if (m_tbEntityName.Focused)
            {
                return;
            }
            switch (e.KeyData)
            {
                case Keys.Delete:
                {
                    e.Handled = true;
                    IUICommandService uiCommandService = 
                        ServiceProvider.Instance.GetService(
                        typeof(IUICommandService)) as IUICommandService;
                    UICommand command = 
                        uiCommandService.GetCommand("DeleteSelectedEntities");
                    UIState state = new UIState();
                    command.ValidateCommand(state);
                    if (state.Enabled)
                    {
                        command.DoClick(command, null);
                    }
                    break;
                }
            }
        
        }

        #endregion

        #region UICommand Handlers
        [UICommandHandler("SelectAllEntities")]
        private void OnSelectAllEntities(object sender, 
            EventArgs args)
        {
            MEntity[] sceneEntities = MFramework.Instance.Scene.GetEntities();
            //Note that we dont not have to filter out the frozen and hidden
            //Entities, as the selections service now does that for us.
            SelectionService.ReplaceSelection(sceneEntities);          
        }

        [UICommandHandler("DeselectAllEntities")]
        private void OnDeselectAllEntities(object sender, 
            EventArgs args)
        {
            SelectionService.ClearSelectedEntities();
        }


        [UICommandHandler("CloneSelection")]
        private void OnCloneSelected(object sender, EventArgs args)
        {
            List<MEntity> entitiesToClone = new List<MEntity>(
                SelectionService.NumSelectedEntities);
            foreach(MEntity entity in SelectionService.GetSelectedEntities())
            {
                bool doNotAdd = false;
                MFramework.Instance.EventManager.RaiseRequestAddCloneToScene(
                    MFramework.Instance.Scene, entity, ref doNotAdd);
                if (!doNotAdd)
                {
                    entitiesToClone.Add(entity);
                }
            }

            if (entitiesToClone.Count == 0)
            {
                return;
            }

            CommandService.BeginUndoFrame(string.Format(
                "Cloned {0} Entities", entitiesToClone.Count));
            List<MEntity> entitiesToSelect = new List<MEntity>(
                entitiesToClone.Count);
            foreach (MEntity entity in entitiesToClone)
            {
                string newName = 
                    MFramework.Instance.Scene.GetUniqueEntityName(entity.Name);
                MEntity[] newEntities = entity.Clone(newName, false);
                for (int i = 0; i != newEntities.Length; ++i)
                {
                    MFramework.Instance.Scene.AddEntity(newEntities[i], true);
                }
                entitiesToSelect.Add(newEntities[0]);
            }
            SelectionService.ReplaceSelection(
                entitiesToSelect.ToArray());
            CommandService.EndUndoFrame(true);
        }

        [UICommandValidator("CloneSelection")]
        private void OnValidateCloneSelection(object sender, UIState state)
        {
            MLayerManager pmLayerManager = MFramework.Instance.LayerManager;
                state.Enabled = SelectionService.GetSelectedEntities().Length > 0 
                && pmLayerManager.ActiveLayer != null;
        }



        #endregion



        #region EntityComparer class
        private class EntityComparer : IComparer
        {
            #region IComparer Members
            public int Compare(object pmObjectA, object pmObjectB)
            {
                MEntity pmEntityA = pmObjectA as MEntity;
                MEntity pmEntityB = pmObjectB as MEntity;
                if (pmEntityA == null || pmEntityB == null)
                {
                    throw new ArgumentException("EntityComparer only " +
                        "supports comparing MEntity instances.");
                }

                Debug.Assert(pmEntityA.PrefabRoot == pmEntityB.PrefabRoot,
                    "Entities can only be compared if they have the " +
                    "same prefab parent");

                return pmEntityA.Name.CompareTo(pmEntityB.Name);
            }
            #endregion
        }
        #endregion

    }
}
