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
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Windows.Forms;
using System.Collections.Generic;

using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    public partial class LayersPanel : Form
    {
        const int ImgWidth = 24;
        const int BtnWidth = ImgWidth + 2;

        const string TreeNodeClassName = "Emergent.Gamebryo.SceneDesigner." +
            "StdPluginsCs.Panels.LayersPanel+LayerNode";

        public enum Status { Current, ReadOnly, Writable };

        [SerializableAttribute]
        public class LayerData
        {
            public String name;
            public bool expanded; // layer node has been expanded
            public bool visible;  // layer is visible
            public bool locked;
            public bool useColor;
            public Color color;
        };

        public class LayerNode : TreeNode
        {
            public LayerNode(MLayer layer)
            {
                Debug.Assert(layer != null);

                this.layer = layer;
                UpdateTooltips();
                update();
            }

            public LayerNode(String name)
            {
                this.layer = null;
                Name = name;
                ImageIndex = 3;     // group icon
                SelectedImageIndex = 3;
            }

            public MLayer layer;
            private bool m_enabled = true;
            private Color m_color = Color.Black;
            private Brush m_brush = SystemBrushes.WindowText;


            public bool Enabled
            {
                get { return m_enabled; }
                set { m_enabled = value; }
            }

            public bool Writable
            {
                get
                {
                    Debug.Assert(layer != null);
                    return layer.Writable;
                }
                set
                {
                    Debug.Assert(layer != null);
                    layer.Writable = value;
                }
            }

            public MLayer.TriState VisibleState
            {
                get
                {
                    Debug.Assert(layer != null);
                    return layer.VisibleState;
                }
            }

            public MLayer.TriState LockedState
            {
                get
                {
                    Debug.Assert(layer != null);
                    return layer.LockedState;
                }
            }

            public void update()
            {
                Status status = Status.ReadOnly;

                if (Writable)
                {
                    MLayerManager layerManager =
                        MFramework.Instance.LayerManager;
                    if (layerManager.ActiveLayer == layer)
                    {
                        status = Status.Current;
                    }
                    else
                    {
                        status = Status.Writable;
                    }
                }
                ImageIndex = (int)status;
                SelectedImageIndex = (int)status;
            }

            private void UpdateTooltips()
            {
                if (layer != null)
                {
                    ToolTipText = layer.Name == "" ?
                        "<untitled>" : layer.Name;
                }
                else
                {
                    ToolTipText = Name;
                }
            }

            public String Filename
            {
                get
                {
                    return layer != null ? layer.Name : Name;
                }
                set
                {
                    if (layer != null)
                    {
                        layer.Rename(value, true);
                    }
                    UpdateTooltips();
                }
            }
            public String DisplayText
            {
                get
                {
                    String name = layer != null ? 
                        layer.ShortName : "<Implicit>";
                    if (layer != null && layer.Dirty)
                    {
                        name += "*";
                    }
                    return name;
                }
            }

            public Brush Brush
            {
                get
                {
                    return m_brush;
                }
            }
            public Color Color
            {
                get
                {
                    return m_color;
                }
                set
                {
                    m_color = value;
                    m_brush = new SolidBrush(value);
                }
            }
        };

        private Dictionary<MLayer, LayerData> expanded;

        private MFramework FW
        {
            get
            { return MFramework.Instance; }
        }

        private Font SelectedFont;

        public LayersPanel()
        {
            // This call is required by the Windows Form Designer.
            InitializeComponent();

            expanded = new Dictionary<MLayer, LayerData>();

            Debug.Assert(Properties.Resources.Visible.Width == ImgWidth * 3);
            Debug.Assert(Properties.Resources.Visible.Height == ImgWidth);

            Debug.Assert(Properties.Resources.Locked.Width == ImgWidth * 3);
            Debug.Assert(Properties.Resources.Locked.Height == ImgWidth);

            Debug.Assert(StatusIcons.Images.Count == 3);
            Debug.Assert(StatusIcons.Images[0].Width == ImgWidth);
            Debug.Assert(StatusIcons.Images[0].Height == ImgWidth);

            FW.EventManager.LayerAdded +=
                new MEventManager.__Delegate_LayerAdded(
                EventManager_LayerAdded);

            FW.EventManager.LayerFilenameAdded +=
                new MEventManager.__Delegate_LayerFilenameAdded(
                EventManager_LayerFilenameAdded);

            FW.EventManager.LayerFilenameRemoved +=
                new MEventManager.__Delegate_LayerFilenameRemoved(
                EventManager_LayerFilenameRemoved);

            FW.EventManager.LayerDeleted +=
                new MEventManager.__Delegate_LayerDeleted(
                EventManager_LayerDeleted);

            FW.EventManager.NewSceneLoaded +=
                new MEventManager.__Delegate_NewSceneLoaded(
                EventManager_NewSceneLoaded);

            FW.EventManager.SceneClosing +=
                new MEventManager.__Delegate_SceneClosing(
                EventManager_SceneClosing);

            FW.EventManager.MainSceneSaving +=
                new MEventManager.__Delegate_MainSceneSaving(
                EventManager_MainSceneSaving);

            FW.EventManager.LayerStatusChanged +=
                new MEventManager.__Delegate_LayerStatusChanged(
                EventManager_LayerStatusChanged);

            FW.EventManager.ActiveLayerChanged +=
                new MEventManager.__Delegate_ActiveLayerChanged(
                EventManager_ActiveLayerChanged);

            FW.EventManager.ApplicationClosing +=
                new MEventManager.__Delegate_ApplicationClosing(
                EventManager_ApplicationClosing);

            FW.EventManager.EntityHiddenStateChanged +=
                new MEventManager.__Delegate_EntityHiddenStateChanged(
                EventManager_EntityHiddenStateChanged);

            FW.EventManager.EntityFrozenStateChanged +=
                new MEventManager.__Delegate_EntityFrozenStateChanged(
                EventManager_EntityFrozenStateChanged);

            FW.EventManager.LongOperationCompleted +=
                new MEventManager.__Delegate_LongOperationCompleted(
                EventManager_LongOperationCompleted);
        }

        private void LayersPanel_Load(object sender, EventArgs e)
        {
            tree.ItemHeight = ImgWidth + 3;

            StatusIcons.ImageSize = Properties.Resources.Current.Size;
            StatusIcons.ColorDepth = ColorDepth.Depth32Bit;

            StatusIcons.Images.Clear();
            StatusIcons.Images.Add(Properties.Resources.Current);
            StatusIcons.Images.Add(Properties.Resources.Readonly);
            StatusIcons.Images.Add(Properties.Resources.Writable);
            StatusIcons.Images.Add(Properties.Resources.Group);

            SelectedFont = new Font(tree.Font, FontStyle.Bold);
            tree.BeginUpdate();
            foreach (LayerNode node in tree.Nodes)
            {
                node.Expand();
            }
            tree.EndUpdate();
        }

        struct Row
        {
            public Rectangle all;      // bounds for entire node area
            public Rectangle visible;  // bounds for visible buttons
            public Rectangle locked;   // bounds for locked buttons
            public Rectangle buttons;  // bounds for all buttons
            public Rectangle text;     // bounds for text area
        };

        private LayerData GetData(MLayer pmLayer)
        {
            LayerData data;
            if (pmLayer == null || !expanded.TryGetValue(pmLayer, out data))
            {
                data = new LayerData();
                data.useColor = false;
                data.visible = true;
                data.locked = false;
                data.expanded = false;
                data.color = Color.Black;
                if (pmLayer != null)
                {
                    expanded[pmLayer] = data;
                }
            }
            return data;
        }

        private void treeView1_DrawNode(object sender, DrawTreeNodeEventArgs e)
        {
            if (m_bRebuildingList)
            {
                return;
            }
            MLayerManager layerManager = FW.LayerManager;
            e.DrawDefault = false;

            LayerNode node = (LayerNode)e.Node;

            Row row = GetButtonArea(node);
            //e.Graphics.FillRectangle(SystemBrushes.Window, row.all);

            if (row.all.Height == 0 || row.all.Width == 0)
            {
                return;
            }

            if (node.layer != null)
            {
                Debug.Assert(row.visible.Width == BtnWidth);

                e.Graphics.FillRectangle(SystemBrushes.Window, row.visible);
                e.Graphics.FillRectangle(SystemBrushes.Window, row.locked);

                Image img = Properties.Resources.Visible;
                e.Graphics.DrawImage(img, row.visible,
                    (int)node.VisibleState * ImgWidth, 0, ImgWidth, ImgWidth,
                    GraphicsUnit.Pixel);
                // Get the second button area
                img = Properties.Resources.Locked;
                e.Graphics.DrawImage(img, row.locked,
                    (int)node.LockedState * ImgWidth, 0, ImgWidth, ImgWidth,
                    GraphicsUnit.Pixel);
            }

            LayerData data = GetData(node.layer);
            Brush brush = node.Brush;
            Font font = tree.Font;
            LayerNode selected = tree.SelectedNode as LayerNode;
            if (selected != null && selected.layer == node.layer)
            {
                bool isSelected = (e.State & TreeNodeStates.Selected) == 
                    TreeNodeStates.Selected;
                if (isSelected)
                {
                    e.Graphics.FillRectangle(SystemBrushes.Highlight,
                        row.text);
                    brush = SystemBrushes.HighlightText;
                }
                font = SelectedFont;
            }
            if (!node.Enabled)
            {
                brush = SystemBrushes.GrayText;
            }

            // Draw the node text.
            String txt = node.DisplayText;
            SizeF size = e.Graphics.MeasureString(txt, font);
            row.text.Inflate(0, (int)(-(row.text.Height - size.Height) / 2));
            e.Graphics.DrawString(txt, font, brush, row.text);
        }

        private Row GetButtonArea(TreeNode node)
        {
            Row row = new Row();
            row.all = node.Bounds;
            row.text = node.Bounds;

            row.buttons = node.Bounds;
            //row.buttons.Inflate(0, -1);
            row.buttons.X = tree.Width - BtnWidth * 2;
            row.buttons.Width = BtnWidth * 2;
            row.visible = row.buttons;
            row.visible.Width = BtnWidth;
            row.locked = row.visible;

            row.locked.Offset(BtnWidth, 0);
            row.text.Width = row.buttons.X - row.text.X - 2;
            return row;
        }

        struct DragArgs
        {
            public LayerNode dest;
            public bool valid;
        };

        DragArgs CalcDropTarget(LayerNode dragee, Point pt)
        {
            DragArgs args = new DragArgs();

            if (dragee == null)
            {
                args.valid = false;
                args.dest = null;
                return args;
            }

            args.dest = tree.GetNodeAt(pt) as LayerNode;
            args.valid = args.dest.layer == null || args.dest.layer.Writable;
            return args;
        }

        private void treeView1_ItemDrag(object sender, ItemDragEventArgs e)
        {
            LayerNode node = e.Item as LayerNode;
            LayerNode parent = node.Parent as LayerNode;
            // Don't let the default or implicit nodes be dragged
            if (parent != null && (parent.layer == null || parent.Writable))
            {
                DoDragDrop(e.Item, DragDropEffects.Move);
            }
        }

        private void treeView1_DragEnter(object sender, DragEventArgs e)
        {
            LayerNode dragee = (LayerNode)e.Data.GetData(TreeNodeClassName);
            Point pt = tree.PointToClient(new Point(e.X, e.Y));
            DragArgs args = CalcDropTarget(dragee, pt);
            e.Effect = args.valid ? DragDropEffects.Move :
                                    DragDropEffects.None;
        }

        private void treeView1_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(TreeNodeClassName, true))
            {
                Point pt = tree.PointToClient(new Point(e.X, e.Y));
                LayerNode dragee = (LayerNode) e.Data.GetData(
                    TreeNodeClassName);
                DragArgs args = CalcDropTarget(dragee, pt);

                Debug.Assert(args.valid, "Should never be invalid here");

                Debug.Assert(dragee != null);
                Debug.Assert(dragee.layer != null);
                Debug.Assert(dragee.Parent != null);

                LayerNode pmOldParent = dragee.Parent as LayerNode;

                MLayer dest = args.dest.layer;
                if (dest == null)
                {
                    // drag to the implicit layer
                    // if the old parent's layer is null then they drug from
                    // the implicit section to the implicit section so do
                    // nothing
                    if (pmOldParent.layer != null)
                    {
                        pmOldParent.layer.RemoveExplicitLayer(dragee.layer);
                    }
                }
                else
                {
                    CommandService.BeginUndoFrame(String.Format(
                        "Move layer \"{0}\" to \"{1}\"", 
                        dragee.layer.ShortName, dest.ShortName));
                    dest.AddExplicitLayer(dragee.layer);
                    // if the old parent's layer is null then it
                    // was implicit and now is explicit
                    if (pmOldParent.layer != null)
                    {
                        pmOldParent.layer.RemoveExplicitLayer(dragee.layer);
                    }
                    CommandService.EndUndoFrame(true);
                }
            }
        }

        private void treeView1_DragOver(object sender, DragEventArgs e)
        {
            Point pt = tree.PointToClient(new Point(e.X, e.Y));
            DragArgs args = CalcDropTarget((LayerNode)e.Data.GetData(
                TreeNodeClassName), pt);

            e.Effect = args.valid ? DragDropEffects.Move :
                                    DragDropEffects.None;
        }

        private void tree_QueryContinueDrag(object sender,
            QueryContinueDragEventArgs e)
        {
            if (e.EscapePressed)
            {
                e.Action = DragAction.Cancel;
            }
        }

        private bool HasAncestorLayer(LayerNode node, MLayer pmAncestor)
        {
            while (node != null)
            {
                if (node.layer == pmAncestor)
                {
                    return true;
                }
                node = node.Parent as LayerNode;
            }
            return false;
        }

        private LayerNode FindLayer(TreeNodeCollection parentNodes,
            MLayer pmLayer)
        {
            foreach (LayerNode node in parentNodes)
            {
                if (node.layer == pmLayer)
                {
                    return node;
                }
            }
            return null;
        }

        private LayerNode InsertNode(TreeNodeCollection parentNodes, 
            MLayer pmLayer)
        {
            LayerNode node = FindLayer(parentNodes, pmLayer);
            if (node != null)
            {
                // this node was already added
                return node;
            }
            node = new LayerNode(pmLayer);

            // insert it in sorted order
            int i = 0;
            for (; i != parentNodes.Count; ++i)
            {
                LayerNode sibling = parentNodes[i] as LayerNode;
                if (sibling.layer.ShortName.CompareTo(pmLayer.ShortName) > 0)
                {
                    break;
                }
            }
            parentNodes.Insert(i, node);
            if (HasAncestorLayer(node.Parent as LayerNode, pmLayer) ||
                pmLayer.IsDefaultLayer)
            {
                node.Enabled = false;
            }
            else
            {
                // Recurse to get our children
                BuildListForLayer(pmLayer.GetSubLayers(), node.Nodes);
            }
            return node;
        }

        private void UpdateLayerStatus(LayerNode node)
        {
            MLayer pmLayer = node.layer;
            if (pmLayer == null)
            {
                // this is the implicit node
                node.Expand();
            }
            else
            {
                Debug.Assert(pmLayer != null);
                LayerData data = GetData(pmLayer);
                if (data.expanded)
                {
                    node.Expand();
                }
                if (pmLayer.Visible != data.visible)
                {
                    pmLayer.ForceVisible(data.visible, false);
                }
                if (pmLayer.Locked != data.locked)
                {
                    pmLayer.ForceLocked(data.locked, false);
                }
            }
        }

        private void BuildListForLayer(MLayer[] amLayers, 
            TreeNodeCollection parentNodes)
        {
            Debug.Assert(parentNodes != null);

            IEnumerator en = amLayers.GetEnumerator();
            while (en.MoveNext())
            {
                MLayer pmChild = en.Current as MLayer;
                LayerNode node = InsertNode(parentNodes, pmChild);

                UpdateLayerStatus(node);
            }
        }

        private bool m_bRebuildingList = false;

        private void RebuildList()
        {
            m_bRebuildingList = true;
            tree.BeginUpdate();
            tree.Nodes.Clear();

            MLayerManager layerManager = FW.LayerManager;
            MLayer defLayer = layerManager.DefaultLayer;
            if (defLayer != null)
            {
                LayerNode node = new LayerNode(defLayer);
                BuildListForLayer(defLayer.GetSubLayers(), node.Nodes);
                node.Expand();
                UpdateLayerStatus(node);
                tree.Nodes.Add(node);
                MLayer[] amImplicitLayers = layerManager.GetImplicitLayers();
                if (amImplicitLayers.Length > 0)
                {
                    node = FindImplicitNode(true);
                    Debug.Assert(node != null, 
                        "Couldn't create implicit node");
                    node.Remove();
                    BuildListForLayer(amImplicitLayers, node.Nodes);
                    tree.Nodes.Add(node);
                    node.Expand();
                    UpdateLayerStatus(node);
                }
            }
            tree.EndUpdate();
            m_bRebuildingList = false;
        }

        private void FindAllLayerInstances(MLayer pmLayer,
            TreeNodeCollection nodes, List<LayerNode> layers)
        {
            foreach (LayerNode node in nodes)
            {
                if (pmLayer == node.layer)
                {
                    layers.Add(node);
                }
                FindAllLayerInstances(pmLayer, node.Nodes, layers);
            }
        }

        private LayerNode FindImplicitNode(bool bCreateIfNecessary)
        {
            TreeNode[] nodes = tree.Nodes.Find("<Implicit>", false);
            LayerNode node = nodes.Length == 0 ? null : nodes[0] as LayerNode;
            if (node == null)
            {
                if (!bCreateIfNecessary)
                {
                    return null;
                }
                node = new LayerNode("<Implicit>");
                tree.Nodes.Add(node);
            }
            return node;
        }

        private void AddImplicitLayer(MLayer pmLayer)
        {
            LayerNode impLayerNode = FindImplicitNode(true);
            Debug.Assert(impLayerNode != null);
            InsertNode(impLayerNode.Nodes, pmLayer);
            if (!impLayerNode.IsExpanded)
            {
                impLayerNode.Expand();
            }
        }

        private void RemoveImplicitLayer(MLayer pmLayer)
        {
            LayerNode impLayerNode = FindImplicitNode(false);
            if (impLayerNode != null)
            {
                LayerNode node = FindLayer(impLayerNode.Nodes, pmLayer);
                if (node != null)
                {
                    node.Remove();
                    if (impLayerNode.Nodes.Count == 0)
                    {
                        impLayerNode.Remove();
                    }
                }
            }
        }

        private void EventManager_LayerAdded(MLayer pmLayer, MLayer pmParent)
        {
            if (!FW.PerformingLongOperation)
            {
                if (pmParent.IsExplicitSublayer(pmLayer))
                {
                    List<LayerNode> layers = new List<LayerNode>();
                    FindAllLayerInstances(pmParent, tree.Nodes, layers);
                    foreach (LayerNode node in layers)
                    {
                        if (node.Enabled)
                        {
                            InsertNode(node.Nodes, pmLayer);
                            if (pmParent.IsDefaultLayer)
                            {
                                node.Expand();
                            }
                        }
                    }
                }
                else
                {
                    AddImplicitLayer(pmLayer);
                }
            }
        }

        private void EventManager_LongOperationCompleted()
        {
            if (m_invalidateTree)
            {
                tree.Invalidate();
                m_invalidateTree = false;
            }
        }

        private void EventManager_LayerFilenameAdded(MLayer pmLayer,
            String strFilename)
        {
            tree.BeginUpdate();
            List<LayerNode> nodes = new List<LayerNode>();
            FindAllLayerInstances(pmLayer, tree.Nodes, nodes);
            MLayer newLayer = FW.LayerManager.FindLayerByName(strFilename);
            foreach (LayerNode parent in nodes)
            {
                if (parent.Enabled)
                {
                    InsertNode(parent.Nodes, newLayer);
                }
            }
            // the layer may have been an existing implicit layer
            RemoveImplicitLayer(newLayer);
            if (tree.Nodes.Count > 0 && !tree.Nodes[0].IsExpanded)
            {
                tree.Nodes[0].Expand();
            }
            tree.EndUpdate();
        }

        private void EventManager_LayerFilenameRemoved(MLayer pmLayer,
            String strFilename)
        {
            tree.BeginUpdate();
            List<LayerNode> nodes = new List<LayerNode>();
            FindAllLayerInstances(pmLayer, tree.Nodes, nodes);
            MLayer condemned = FW.LayerManager.FindLayerByName(strFilename);
            foreach (LayerNode parent in nodes)
            {
                LayerNode node = FindLayer(parent.Nodes, condemned);
                Debug.WriteLineIf(node == null, 
                    String.Format("{0} not found in node {1}", 
                    condemned.ShortName, pmLayer.ShortName), "WARN");
                if (node != null)
                {
                    Debug.WriteLineIf(node.Filename != condemned.Name,
                        String.Format("{0} not equal to found node {1}",
                        node.Filename, condemned.Name), "WARN");
                    node.Remove();
                }
            }
            // the layer that was removed is now probably an implicit layer
            if (FW.LayerManager.IsImplicit(condemned))
            {
                AddImplicitLayer(condemned);
            }
            tree.EndUpdate();
        }

        private void EventManager_LayerDeleted(MLayer pmLayer, 
            bool bFileDeleted)
        {
            // This can get called when we are shutting down so we ignore it.
            if (tree.IsDisposed)
            {
                return;
            }
            tree.BeginUpdate();
            if (FW.LayerManager.IsImplicit(pmLayer))
            {
                RemoveImplicitLayer(pmLayer);
            }
            else
            {
                List<LayerNode> nodes = new List<LayerNode>();
                FindAllLayerInstances(pmLayer, tree.Nodes, nodes);
                foreach (LayerNode node in nodes)
                {
                    node.Remove();
                }
                expanded.Remove(pmLayer);
            }
            tree.EndUpdate();
        }

        private void EventManager_NewSceneLoaded(MScene pmScene)
        {
            MLayerManager layerManager = FW.LayerManager;

            ServiceProvider sp = ServiceProvider.Instance;

            ISettingsService settingsService =
                            sp.GetService(typeof(ISettingsService))
                            as ISettingsService;
            ArrayList layersForLoading = settingsService.
                GetSettingsObject("LayerGroups", SettingsCategory.PerScene)
                as ArrayList;

            expanded.Clear();

            if (layersForLoading != null)
            {
                foreach (LayerData data in layersForLoading)
                {
                    MLayer layer = (data.name == "<default>") ?
                        layerManager.DefaultLayer :
                        layerManager.FindLayerByName(data.name);
                    if (layer != null)
                    {
                        expanded[layer] = data;
                    }
                }
            }
            RebuildList();
        }

        private void GetLayersForSaving(TreeNodeCollection nodes, 
                                        ArrayList layersForSaving)
        {
            foreach (LayerNode node in nodes)
            {
                if (node.layer != null)
                {
                    LayerData data = GetData(node.layer);
                    data.name = node.layer.IsDefaultLayer ?
                        "<default>" : node.Filename;
                    data.expanded = node.IsExpanded;
                    data.visible = node.layer.Visible;
                    data.locked = node.layer.Locked;
                    layersForSaving.Add(data);
                    if (node.Nodes.Count > 0)
                    {
                        GetLayersForSaving(node.Nodes, layersForSaving);
                    }
                }
            }
        }

        private void EventManager_SceneClosing(MScene pmScene)
        {
            tree.Nodes.Clear();
            expanded.Clear();
        }

        private void EventManager_MainSceneSaving(string strFilename,
            bool bAutoSave)
        {
            ArrayList layersForSaving = new ArrayList();
            GetLayersForSaving(tree.Nodes, layersForSaving);

            ServiceProvider sp = ServiceProvider.Instance;

            ISettingsService settingsService =
                            sp.GetService(typeof(ISettingsService))
                            as ISettingsService;
            settingsService.SetSettingsObject("LayerGroups", layersForSaving,
                SettingsCategory.PerScene);
        }

        private void UpdateLayer(MLayer pmLayer)
        {
            if (pmLayer != null)
            {
                List<LayerNode> layers = new List<LayerNode>();
                FindAllLayerInstances(pmLayer, tree.Nodes, layers);
                foreach (LayerNode node in layers)
                {
                    if (node.layer != null)
                        node.update();
                }
            }
        }

        private void EventManager_LayerStatusChanged(MLayer pmLayer)
        {
            //InvalidateLayer(pmLayer, tree.Nodes);
            if (!m_bRebuildingList)
            {
                UpdateLayer(pmLayer);
                tree.Invalidate();
            }
        }

        private void EventManager_ActiveLayerChanged(MLayer pmActiveLayer,
            MLayer pmOldActiveLayer)
        {
            if (!IsDisposed)
            {
                UpdateLayer(pmActiveLayer);
                UpdateLayer(pmOldActiveLayer);
                tree.Invalidate();
            }
        }

        private void EventManager_ApplicationClosing()
        {
            tree.Nodes.Clear();
            expanded.Clear();
        }

        private bool m_invalidateTree = false;

        private void EventManager_EntityHiddenStateChanged(MEntity entity,
            bool hidden)
        {
            if (FW.PerformingLongOperation)
            {
                m_invalidateTree = true;
            }
            else
            {
                tree.Invalidate();
            }
        }

        private void EventManager_EntityFrozenStateChanged(MEntity entity,
            bool frozen)
        {
            if (FW.PerformingLongOperation)
            {
                m_invalidateTree = true;
            }
            else
            {
                tree.Invalidate();
            }
        }
        // prompt for a new layer name, return null if the user cancels
        private String GetNewLayerName(string strDefFilename)
        {
            // Set up SaveFileDialog filter.
            string[] streamingDescriptions =
                MUtility.GetStreamingFormatDescriptions();
            string[] streamingExtensions =
                MUtility.GetStreamingFormatExtensions();
            int iFormats = streamingDescriptions.Length;
            Debug.Assert(iFormats > 0);
            m_dlgNewLayer.Filter = streamingDescriptions[0];
            m_dlgNewLayer.FileName = strDefFilename;
            for (int i = 1; i < iFormats; i++)
            {
                m_dlgNewLayer.Filter += "|" + streamingDescriptions[i];
            }

            while (true)
            {
                if (m_dlgNewLayer.ShowDialog(this) == DialogResult.Cancel)
                {
                    return null;
                }

                // Verify that the file path is not inside the current
                // palette directory. This is to prevent prefabs from being
                // loaded as layers.
                if (m_dlgNewLayer.FileName.Contains(
                    FW.PaletteManager.PaletteFolder))
                {
                    MessageBox.Show("The chosen path is inside the current " +
                        "palette directory. Layer files\ncannot reside in " +
                        "the palette directory. Please choose again.",
                        "Invalid Layer Path", MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                    continue;
                }

                // Ensure that the chosen filename has the appropriate
                // extension.
                string strExtension = string.Empty;
                int iExtensionIndex = m_dlgNewLayer.FileName.LastIndexOf('.');
                if (iExtensionIndex > -1)
                {
                    strExtension = m_dlgNewLayer.FileName.Substring(
                        iExtensionIndex + 1).ToUpper();
                }
                string strChosenExtension = streamingExtensions[
                    m_dlgNewLayer.FilterIndex - 1];
                if (!strExtension.Equals(strChosenExtension))
                {
                    strExtension = strChosenExtension;
                    m_dlgNewLayer.FileName += "." + strExtension.ToLower();
                }

                System.IO.FileInfo info = new System.IO.FileInfo(
                    m_dlgNewLayer.FileName);
                if (info.Exists)
                {
                    string strMessage = null;
                    if (MFramework.Instance.PrefabManager.
                        GetPrefabByFilename(m_dlgNewLayer.FileName) != null)
                    {

                        strMessage = "The following filename already " +
                            "identifies a prefab."; 
                    }
                    else if (MFramework.Instance.LayerManager.
                        FindLayerByName(m_dlgNewLayer.FileName) != null)
                    {
                        strMessage = "A layer with the specified " +
                            "filename already exists."; 
                    }

                    if (strMessage != null)
                    {
                        strMessage += "  Please choose a different name\n\n" +
                            m_dlgNewLayer.FileName;
                        MessageBox.Show(strMessage,
                            "Rename Failed",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Error);
                        continue;
                    }

                    strMessage = "The selected file already exists on disk";
                    if (info.IsReadOnly)
                    {
                        strMessage += " and is read-only";
                    }
                    strMessage += ". Do you want to overwrite it?";

                    if (MessageBox.Show(strMessage,
                        "Overwrite Existing File?",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Question) == DialogResult.Yes)
                    {
                        info.IsReadOnly = false;
                        info.Delete();
                    }
                    else
                    {
                        continue;
                    }
                }
                return m_dlgNewLayer.FileName;
            }
        }

        private void btnNewLayer_Click(object sender, EventArgs e)
        {
            String filename = GetNewLayerName("");
            if (filename != null)
            {
                MLayerManager layerManager = FW.LayerManager;
                layerManager.AddEmptyLayer(filename, true);
            }
        }

        // Returns true if the default layer or the implict layer is selected
        public bool isDefaultLayerSelected
        {
            get
            {
                LayerNode selected = tree.SelectedNode as LayerNode;
                return selected != null && selected.layer != null &&
                    selected.layer.IsDefaultLayer;
            }
        }

        private void m_mnuContext_Opening(object sender, 
            System.ComponentModel.CancelEventArgs e)
        {
            bool isDefaultLayerWritable = 
                FW.LayerManager.DefaultLayer.Writable;
            MLayer current = FW.LayerManager.ActiveLayer;
            LayerNode selected = tree.SelectedNode as LayerNode;
            bool haveSelection = selected != null;

            m_mnuSaveAs.Enabled        = !isDefaultLayerSelected;
            m_mnuSetCurrent.Enabled    = haveSelection &&
                                         selected.Writable && 
                                         selected.layer != current;
            m_mnuMoveSelected.Enabled  = haveSelection &&
                                         selected.Writable;
            m_mnuDeleteLayer.Enabled   = haveSelection &&
                                         !isDefaultLayerSelected &&
                                         isDefaultLayerWritable;
            m_mnuSelectAll.Enabled     = haveSelection;
            m_mnuUnhideAll.Enabled     = haveSelection &&
                                         selected.Writable;
            m_mnuUnfreezeAll.Enabled   = haveSelection &&
                                         selected.Writable;
            m_mnuExplicitLayer.Enabled = haveSelection &&
                                         current != null &&
                                         !isDefaultLayerSelected;
            if (haveSelection)
            {
                Debug.Assert(selected.layer != null);
                if (FW.LayerManager.IsImplicit(selected.layer))
                {
                    m_mnuExplicitLayer.Text = "Make explicit";
                    m_mnuExplicitLayer.ToolTipText =
                        "Make an implicit layer explicit to the current layer";
                }
                else
                {
                    m_mnuExplicitLayer.Text = "Make implicit";
                    m_mnuExplicitLayer.ToolTipText =
                        "Make an explicit layer implicit";
                }
            }
        }

        private bool CheckForReadOnlyFiles(string filenameToSave,
            MLayer layerToSave)
        {
            List<string> filenames = new List<string>();

            if (filenameToSave.Equals(layerToSave.Name))
            {
                if (layerToSave.Dirty && !layerToSave.Writable)
                {
                    filenames.Add(layerToSave.Name);
                }
            }
            else
            {
                FileInfo fi = new FileInfo(filenameToSave);
                if (fi.Exists && fi.IsReadOnly)
                {
                    filenames.Add(filenameToSave);
                }
            }

            // Look at all layers 
            MLayerManager layerManager = MFramework.Instance.LayerManager;
            MLayer[] layers = layerManager.GetLayers();
            foreach (MLayer layer in layers)
            {
                if (!layer.Writable && layer != layerToSave &&
                    (layer.IsExplicitSublayer(layerToSave) || layer.Dirty))
                {
                    filenames.Add(layer.Name);
                }
            }

            MPalette[] palettes = MFramework.Instance.PaletteManager
                .GetPalettes();
            foreach (MPalette palette in palettes)
            {
                if (palette.Dirty && !palette.Scene.Writable)
                {
                    filenames.Add(palette.Scene.SourceFilename);
                }
            }

            MPrefab[] prefabs = MFramework.Instance.PrefabManager.
                GetPrefabs();
            foreach (MPrefab prefab in prefabs)
            {
                if (prefab.Dirty && !prefab.Writable &&
                    layerManager.DefaultLayer.Name != 
                    prefab.SourceFilename)
                {
                    filenames.Add(prefab.SourceFilename);
                }
            }

            if (!layerToSave.Writable || filenames.Count > 0)
            {
                string message = "";

                if (!layerToSave.Writable)
                {
                    message = "Are you sure you want to rename the " +
                        "read-only layer '" + layerToSave.ShortName + "'?";
                }

                if (filenames.Count > 0)
                {
                    if (message.Length > 0)
                    {
                        message += "\n\nIn addition the ";
                    }
                    else
                    {
                        message = "The ";
                    }
                    message += "following files are read-only and " +
                         "must be made writable to be saved:\n\n";
                    foreach (string filename in filenames)
                    {
                        message += string.Format("{0}\n", filename);
                    }
                    message += "\nIf you do not wish to make them writable " +
                        "please select 'No' to cancel the save operation.";
                }
                DialogResult result = MessageBox.Show(message,
                    "Make Files Writable?", MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    // go through the layer to make sure the readonly 
                    // flag gets updated.
                    foreach (string filename in filenames)
                    {
                        MLayer layer = layerManager.GetLayerByName(filename);
                        layer.Writable = true;
                    }

                    return true;
                }

                return false;
            }

            return true;
        }

        private void m_mnuSaveAs_Click(object sender, EventArgs e)
        {
            LayerNode selected = tree.SelectedNode as LayerNode;

            Debug.Assert(selected != null);
            Debug.Assert(selected.layer != null);

            String filename = GetNewLayerName(selected.Filename);
            if (filename != null && 
                CheckForReadOnlyFiles(filename, selected.layer))
            {
                selected.Filename = filename;
                // Force a save
                IUICommandService uiCommandService = ServiceProvider.Instance
                    .GetService(typeof(IUICommandService)) 
                    as IUICommandService;
                Debug.Assert(uiCommandService != null,
                    "UICommandService not found!");
                UICommand command = uiCommandService.GetCommand("SaveFile");
                if (command != null)
                {
                    command.DoClick(null, null);
                }

                tree.Invalidate();
            }
        }

        private void m_mnuSetCurrent_Click(object sender, EventArgs e)
        {
            Debug.Assert(tree.SelectedNode != null);
            LayerNode selected = tree.SelectedNode as LayerNode;
            Debug.Assert(selected != null);
            FW.LayerManager.ActiveLayer = selected.layer;
        }

        private bool OkToMoveEntities()
        {
            ISelectionService pmSelectionService =
                ServiceProvider.Instance.GetService(
                typeof(ISelectionService)) as ISelectionService;

            // check selected entities to see if any are in a read-only
            // layer
            MLayerManager layerManager = FW.LayerManager;
            MEntity[] amEntities = pmSelectionService.GetSelectedEntities();
            List<MLayer> allDependents = new List<MLayer>();
            uint uiIndex = 0;
            for (int i = 0; i < amEntities.Length; i++)
            {
                MLayer[] dependents = layerManager.
                    SearchReadOnlyLayersForEntityReference(
                    amEntities[uiIndex]);
                if (dependents != null)
                {
                    foreach (MLayer layer in dependents)
                    {
                        if (!allDependents.Contains(layer))
                        {
                            allDependents.Add(layer);
                        }
                    }
                }
            }
            if (allDependents.Count > 0)
            {
                string message = "The following read-only " +
                    "layers are dependent on the selected entities:\n\n";
                foreach (MLayer layer in allDependents)
                {
                    message += string.Format("{0}\n", layer.Name);
                }
                message += "\nYou must make these layers writable " +
                    "before you can move these entities.";

                MessageBox.Show(message, "Cannot Move Entities",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }
            return true;
        }

        private void m_mnuMoveSelected_Click(object sender, EventArgs e)
        {
            Debug.Assert(tree.SelectedNode != null);
            if (OkToMoveEntities())
            {
                LayerNode selected = tree.SelectedNode as LayerNode;
                selected.layer.MoveSelectedEntities();
            }
        }

        private void m_mnuSelectAll_Click(object sender, EventArgs e)
        {
            LayerNode selected = tree.SelectedNode as LayerNode;
            if (selected != null)
            {
                selected.layer.SelectAllEntities();
            }
        }

        private void DeleteLayer_Click(object sender, EventArgs e)
        {
            MLayerManager layerManager = FW.LayerManager;

            LayerNode selected = tree.SelectedNode as LayerNode;
            Debug.Assert(selected != null);
            Debug.Assert(selected.layer != null);

            DeleteLayerDialog dlg = new DeleteLayerDialog();
            dlg.Layer = selected.layer;
            DialogResult res = dlg.ShowDialog(this);
            if (res == DialogResult.Cancel)
            {
                return;
            }

            MLayer[] dependents = layerManager.
                FindDependentReadOnlyLayers(selected.layer);
            if (dependents != null && dependents.Length > 0)
            {
                string message = string.Format("The following read-only " +
                    "layers are dependent on the \"{0}\" layer:\n\n",
                    selected.layer.Name);
                foreach (MLayer dep in dependents)
                {
                    message += string.Format("{0}\n", dep.Name);
                }
                message += "\nYou must make these layers writable " +
                    "before you can delete this layer.";

                MessageBox.Show(message, "Cannot Delete Layer",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                layerManager.RemoveLayer(selected.layer, dlg.DeleteFile);
            }
            tree.SelectedNode = null;
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (tree.Nodes.Count > 0)
            {
                SetButtonState();
            }
        }

        void SetButtonState()
        {
            MLayerManager layerManager = FW.LayerManager;
            bool AllowAdd = layerManager.DefaultLayer.Writable &&
                FW.PrefabManager.GetPrefabByFilename(
                layerManager.DefaultLayer.Name) == null;
            if (AllowAdd != m_btnNewLayer.Enabled)
            {
                // other buttons are always the same
                m_btnNewLayerFromSelection.Enabled = AllowAdd;
                m_btnImportLayer.Enabled = AllowAdd;
                m_btnNewLayer.Enabled = AllowAdd;
            }
            bool AllowDelete = AllowAdd &&
                    tree.SelectedNode != null && !isDefaultLayerSelected;
            if (AllowDelete != m_btnDeleteLayer.Enabled)
            {
                m_btnDeleteLayer.Enabled = AllowDelete;
            }
            ISelectionService pmSelectionService =
                ServiceProvider.Instance.GetService(
                typeof(ISelectionService)) as ISelectionService;

            bool AllowLayerFromSelection = AllowAdd &&
                pmSelectionService.NumSelectedEntities > 0;
            if (AllowLayerFromSelection != m_btnNewLayerFromSelection.Enabled)
            {
                m_btnNewLayerFromSelection.Enabled = AllowLayerFromSelection;
            }
        }

        private void m_btnImportLayer_Click(object sender, EventArgs e)
        {
            IUICommandService uiCommandService = ServiceProvider.Instance
                .GetService(typeof(IUICommandService)) as IUICommandService;
            Debug.Assert(uiCommandService != null,
                "UICommandService not found!");
            UICommand command = uiCommandService.GetCommand("ImportLayer");
            if (command != null)
            {
                command.DoClick(null, null);
            }
        }

        private void m_btnNewLayerFromSelection_Click(object sender,
            EventArgs e)
        {
            if (OkToMoveEntities())
            {
                String filename = GetNewLayerName("");
                if (filename != null)
                {
                    MLayerManager layerManager = FW.LayerManager;
                    layerManager.AddLayerFromSelectedEntities(filename);
                }
            }
        }

        private void m_mnuExplicitLayer_Click(object sender, EventArgs e)
        {
            LayerNode selected = tree.SelectedNode as LayerNode;
            Debug.Assert(selected != null);
            Debug.Assert(selected.layer != null);
            
            if (FW.LayerManager.IsImplicit(selected.layer))
            {
                FW.LayerManager.ActiveLayer.AddExplicitLayer(selected.layer);
            }
            else
            {
                Debug.Assert(selected.Parent != null);
                LayerNode pmParent = selected.Parent as LayerNode;
                Debug.Assert(pmParent.layer != null);
                pmParent.layer.RemoveExplicitLayer(selected.layer);
            }
        }

        private ICommandService CommandService
        {
            get
            {
                ServiceProvider sp = ServiceProvider.Instance;

                ICommandService pmCommandService = 
                    sp.GetService(typeof(ICommandService)) as ICommandService;
                Debug.Assert(pmCommandService != null);
                return pmCommandService;
            }
        }

        private void UnhideAll(MLayer layer)
        {
            if (layer.Writable)
            {
                MEntity[] amEntities = layer.GetEntities();
                foreach (MEntity pmEntity in amEntities)
                {
                    pmEntity.Hidden = false;
                }
            }
        }

        private void UnfreezeAll(MLayer layer)
        {
            if (layer.Writable)
            {
                MEntity[] amEntities = layer.GetEntities();
                foreach (MEntity pmEntity in amEntities)
                {
                    pmEntity.Frozen = false;
                }
            }
        }

        private void m_mnuUnhideAll_Click(object sender, EventArgs e)
        {
            CommandService.BeginUndoFrame(
                "Unhide all entites in selected layer");

            LayerNode selected = tree.SelectedNode as LayerNode;
            Debug.Assert(selected != null);
            Debug.Assert(selected.layer != null);
            UnhideAll(selected.layer);
            CommandService.EndUndoFrame(true);
        }

        private void m_mnuUnfreezeAll_Click(object sender, EventArgs e)
        {
            CommandService.BeginUndoFrame(
                "Unfreeze all entites in selected layer");

            LayerNode selected = tree.SelectedNode as LayerNode;
            Debug.Assert(selected != null);
            Debug.Assert(selected.layer != null);
            UnfreezeAll(selected.layer);

            CommandService.EndUndoFrame(true);
        }

        private void m_mnuColor_Click(object sender, EventArgs e)
        {
            LayerNode selected = tree.SelectedNode as LayerNode;
            Debug.Assert(selected != null);
            Debug.Assert(selected.layer != null);

            if (dlgColor.ShowDialog(this) == DialogResult.OK)
            {
                LayerData data = GetData(selected.layer);
                data.color = dlgColor.Color;
                data.useColor = true;
                selected.Color = data.color;
                tree.Invalidate();
            }
        }

        private void tree_BeforeCollapse(object sender,
            TreeViewCancelEventArgs e)
        {
            // don't let the root nodes collapse
            if (e.Node.Parent == null)
            {
                e.Cancel = true;
            }
        }

        private void SetLayersVisible(LayerNode pmParent, bool visible)
        {
            Debug.Assert(pmParent.layer != null);
            pmParent.layer.Visible = visible;
            foreach (LayerNode layer in pmParent.Nodes)
            {
                SetLayersVisible(layer, visible);
            }
        }

        private void SetLayersLocked(LayerNode pmParent, bool locked)
        {
            Debug.Assert(pmParent.layer != null);
            pmParent.layer.Locked = locked;
            foreach (LayerNode layer in pmParent.Nodes)
            {
                SetLayersLocked(layer, locked);
            }
        }

        private void tree_MouseClick(object sender, MouseEventArgs e)
        {
            LayerNode clickedNode = (LayerNode)tree.GetNodeAt(e.X, e.Y);
            if (clickedNode.layer == null)
            {
                // this is the special implicit layer and cannot be selected
                return;
            }
            Row row = GetButtonArea(clickedNode);
            
            if (row.text.Contains(e.X, e.Y))
            {
                tree.SelectedNode = clickedNode;
            } else if (row.visible.Contains(e.X, e.Y))
            {
                FW.BeginLongOperation();
                Debug.Print("Mouse click " + clickedNode.DisplayText);
                SetLayersVisible(clickedNode, !clickedNode.layer.Visible);
                FW.EndLongOperation();
            }
            else if (row.locked.Contains(e.X, e.Y))
            {
                FW.BeginLongOperation();
                SetLayersLocked(clickedNode, !clickedNode.layer.Locked);
                FW.EndLongOperation();
            }
        }

        private void tree_KeyPress(object sender, KeyPressEventArgs e)
        {
            LayerNode item = tree.SelectedNode as LayerNode;
            if (item == null && item.layer == null)
            {
                return;
            }

            switch (e.KeyChar)
            {
                case 'v':
                case 'V':
                case 'h':
                case 'H':
                    SetLayersVisible(item, !item.layer.Visible);
                    e.Handled = true;
                    break;
                case 'l':
                case 'L':
                case 'f':
                case 'F':
                    SetLayersLocked(item, !item.layer.Locked);
                    e.Handled = true;
                    break;
            }

        }

    }
}
