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
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    public partial class TagsPanel : Form
    {
        #region ListItem class
        private class ListItem
        {
            public ListItem(string tag)
            {
                Tag = tag;
            }

            public override string ToString()
            {
                return Tag;
            }

            private ICommandService m_commandService;
            private ICommandService CommandService
            {
                get
                {
                    if (m_commandService == null)
                    {
                        m_commandService = (ICommandService) ServiceProvider
                            .Instance.GetService(typeof(ICommandService));
                        Debug.Assert(m_commandService != null,
                            "Command service not found!");
                    }
                    return m_commandService;
                }
            }

            private IMessageService m_messageService;
            private IMessageService MessageService
            {
                get
                {
                    if (m_messageService == null)
                    {
                        m_messageService = (IMessageService)ServiceProvider
                            .Instance.GetService(typeof(IMessageService));
                        Debug.Assert(m_messageService != null,
                            "Message service not found!");
                    }
                    return m_messageService;
                }
            }

            private string m_tag;
            public string Tag
            {
                get
                {
                    return m_tag;
                }
                set
                {
                    Debug.Assert(!string.IsNullOrEmpty(value),
                        "Null or empty tag!");
                    if (!string.IsNullOrEmpty(value))
                    {
                        m_tag = value;
                    }
                }
            }

            private Rectangle m_visibleBounds;
            public Rectangle VisibleBounds
            {
                get
                {
                    return m_visibleBounds;
                }
                set
                {
                    m_visibleBounds = value;
                }
            }

            private Rectangle m_frozenBounds;
            public Rectangle FrozenBounds
            {
                get
                {
                    return m_frozenBounds;
                }
                set
                {
                    m_frozenBounds = value;
                }
            }

            public enum State
            {
                False,
                True,
                Indeterminate
            };

            public State VisibleState
            {
                get
                {
                    MEntity[] entities = MFramework.Instance.TagsManager
                        .GetEntitiesForTag(Tag);
                    uint numVisible = 0;
                    foreach (MEntity entity in entities)
                    {
                        if (!entity.Hidden)
                        {
                            ++numVisible;
                        }
                    }

                    State visibleState;
                    if (numVisible == entities.Length)
                    {
                        visibleState = State.True;
                    }
                    else if (numVisible > 0)
                    {
                        visibleState = State.Indeterminate;
                    }
                    else
                    {
                        visibleState = State.False;
                    }

                    return visibleState;
                }
                set
                {
                    Debug.Assert(value != State.Indeterminate,
                        "VisibleState set to be Indeterminate!");
                    bool bHide = (value == State.False);

                    CommandService.BeginUndoFrame(
                        (bHide ? "Hide" : "Show") +
                        " all entities with the \"" + Tag + "\" tag.");

                    MEntity[] entities = MFramework.Instance.TagsManager
                        .GetEntitiesForTag(Tag);
                    Debug.Assert(entities.Length > 0, "Empty entities array!");
                    foreach (MEntity entity in entities)
                    {
                        if (entity.Writable)
                        {
                            entity.Hidden = bHide;
                        }
                        else
                        {
                            MessageService.AddMessage(
                                MessageChannelType.Errors, new Emergent
                                .Gamebryo.SceneDesigner.PluginAPI.Message(
                                "Entity is read-only and cannot be modified.",
                                "The " + (bHide ? "hide" : "show") + " tag " +
                                "operation will skip the \"" + entity.Name +
                                "\" entity.", entity));
                        }
                    }

                    CommandService.EndUndoFrame(true);
                }
            }

            public State FrozenState
            {
                get
                {
                    MEntity[] entities = MFramework.Instance.TagsManager
                        .GetEntitiesForTag(Tag);
                    uint numFrozen = 0;
                    foreach (MEntity entity in entities)
                    {
                        if (entity.Frozen)
                        {
                            ++numFrozen;
                        }
                    }

                    State frozenState;
                    if (numFrozen == entities.Length)
                    {
                        frozenState = State.True;
                    }
                    else if (numFrozen > 0)
                    {
                        frozenState = State.Indeterminate;
                    }
                    else
                    {
                        frozenState = State.False;
                    }

                    return frozenState;
                }
                set
                {
                    Debug.Assert(value != State.Indeterminate,
                        "FrozenState set to be Indeterminate!");
                    bool bFreeze = (value == State.True);

                    CommandService.BeginUndoFrame(
                        (bFreeze ? "Freeze" : "Unfreeze") +
                        " all entities with the \"" + Tag + "\" tag.");

                    MEntity[] entities = MFramework.Instance.TagsManager
                        .GetEntitiesForTag(Tag);
                    Debug.Assert(entities.Length > 0, "Empty entities array!");
                    foreach (MEntity entity in entities)
                    {
                        if (entity.Writable)
                        {
                            entity.Frozen = bFreeze;
                        }
                        else
                        {
                            MessageService.AddMessage(
                                MessageChannelType.Errors, new Emergent
                                .Gamebryo.SceneDesigner.PluginAPI.Message(
                                "Entity is read-only and cannot be modified.",
                                "The " + (bFreeze ? "freeze" : "unfreeze") +
                                " tag operation will skip the \"" +
                                entity.Name + "\" entity.", entity));
                        }
                    }

                    CommandService.EndUndoFrame(true);
                }
            }

            public void SwapVisibleState()
            {
                switch (VisibleState)
                {
                    case State.False:
                        VisibleState = State.True;
                        break;
                    case State.True:
                    case State.Indeterminate:
                        VisibleState = State.False;
                        break;
                }
            }

            public void SwapFrozenState()
            {
                switch (FrozenState)
                {
                    case State.False:
                    case State.Indeterminate:
                        FrozenState = State.True;
                        break;
                    case State.True:
                        FrozenState = State.False;
                        break;
                }
            }

        }
        #endregion

        #region EventManager Event Handlers
        private bool m_bBuildList = false;

        private void OnTagsManagerUpdated()
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bBuildList = true;
            }
            else
            {
                BuildList();
            }
        }

        public void OnEntityHiddenStateChanged(MEntity entity, bool hidden)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bBuildList = true;
            }
            else
            {
                BuildList();
            }
        }

        public void OnEntityFrozenStateChanged(MEntity entity, bool frozen)
        {
            if (MFramework.Instance.PerformingLongOperation)
            {
                m_bBuildList = true;
            }
            else
            {
                BuildList();
            }
        }
        
        private void OnLongOperationCompleted()
        {
            if (m_bBuildList)
            {
                BuildList();
            }
            m_bBuildList = false;
        }
        #endregion

        private int m_imageWidth = 24;

        public TagsPanel()
        {
            InitializeComponent();
        }

        public void BuildList()
        {
            m_lbTags.BeginUpdate();

            int selectedIndex = m_lbTags.SelectedIndex;
            m_lbTags.Items.Clear();

            string[] tags = MFramework.Instance.TagsManager.GetTags();
            foreach (string tag in tags)
            {
                m_lbTags.Items.Add(new ListItem(tag));
            }

            if (selectedIndex > -1 && selectedIndex < m_lbTags.Items.Count)
            {
                m_lbTags.SelectedIndex = selectedIndex;
            }

            m_lbTags.EndUpdate();
        }

        private void TagsPanel_Load(object sender, EventArgs e)
        {
            MFramework.Instance.EventManager.TagsManagerUpdated += new
                MEventManager.__Delegate_TagsManagerUpdated(
                OnTagsManagerUpdated);
            MFramework.Instance.EventManager.EntityHiddenStateChanged += new
                MEventManager.__Delegate_EntityHiddenStateChanged(
                OnEntityHiddenStateChanged);
            MFramework.Instance.EventManager.EntityFrozenStateChanged += new
                MEventManager.__Delegate_EntityFrozenStateChanged(
                OnEntityFrozenStateChanged);
            MFramework.Instance.EventManager.LongOperationCompleted += new
                MEventManager.__Delegate_LongOperationCompleted(
                OnLongOperationCompleted);

            BuildList();
        }

        private void m_lbTags_DrawItem(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();

            if (e.Index > -1)
            {
                Brush brush = ((e.State & DrawItemState.Selected) ==
                    DrawItemState.Selected ? SystemBrushes.HighlightText :
                    new SolidBrush(e.ForeColor));

                Debug.Assert(m_lbTags.Items[e.Index] is ListItem,
                    "Invalid item in list!");
                ListItem item = (ListItem) m_lbTags.Items[e.Index];

                // Draw text.
                Rectangle textBounds = e.Bounds;
                textBounds.Width -= m_imageWidth * 2;
                SizeF textSize = e.Graphics.MeasureString(item.ToString(),
                    e.Font);
                textBounds.Inflate(0, (int) (-(textBounds.Height -
                    textSize.Height) / 2));
                e.Graphics.DrawString(item.ToString(), e.Font, brush,
                    textBounds);

                // Draw visible image.
                Rectangle visibleBounds = e.Bounds;
                visibleBounds.X = e.Bounds.Right - m_imageWidth * 2;
                visibleBounds.Width = m_imageWidth;
                item.VisibleBounds = visibleBounds;
                e.Graphics.DrawImage(Properties.Resources.Visible,
                    item.VisibleBounds, (int) item.VisibleState * m_imageWidth,
                    0, m_imageWidth, m_imageWidth, GraphicsUnit.Pixel);
                
                // Draw frozen image.
                Rectangle frozenBounds = e.Bounds;
                frozenBounds.X = e.Bounds.Right - m_imageWidth;
                frozenBounds.Width = m_imageWidth;
                item.FrozenBounds = frozenBounds;
                e.Graphics.DrawImage(Properties.Resources.Locked,
                    item.FrozenBounds, (int) item.FrozenState * m_imageWidth,
                    0, m_imageWidth, m_imageWidth, GraphicsUnit.Pixel);
            }

            e.DrawFocusRectangle();
        }

        private void m_lbTags_MouseClick(object sender, MouseEventArgs e)
        {
            foreach (ListItem item in m_lbTags.Items)
            {
                if (item.VisibleBounds.Contains(e.X, e.Y))
                {
                    item.SwapVisibleState();
                    break;
                }
                else if (item.FrozenBounds.Contains(e.X, e.Y))
                {
                    item.SwapFrozenState();
                    break;
                }
            }
        }

        private void m_lbTags_KeyPress(object sender, KeyPressEventArgs e)
        {
            ListItem item = m_lbTags.SelectedItem as ListItem;
            if (item == null)
            {
                return;
            }

            switch (e.KeyChar)
            {
                case 'v':
                case 'V':
                case 'h':
                case 'H':
                    item.SwapVisibleState();
                    e.Handled = true;
                    break;
                case 'l':
                case 'L':
                case 'f':
                case 'F':
                    item.SwapFrozenState();
                    e.Handled = true;
                    break;
            }
        }
    }
}