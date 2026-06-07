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
using System.Drawing;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
//using Reflector.UserInterface;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for ToolBarServiceImpl.
    /// </summary>
    public class ToolBarServiceImpl : StandardServiceBase, IToolBarService
    {

        #region PrivateData
        private static readonly string m_standardToolBarName = 
            "0StandardToolBar";
        private Hashtable m_toolBarCollection;
        private ArrayList m_controlBindings;
        //indicated the toolbar needs to be rebuilt
        private bool m_bDirty = true;
        //The winforms toolbar
        ToolBar m_toolbar;
        private ShortcutServiceImpl m_shortcutService;

        #endregion

        public ToolBarServiceImpl()
        {
            m_toolBarCollection = new Hashtable();
            m_controlBindings = new ArrayList();
        }

        private class LogicalToolBar
        {
            public string m_name;
            public ArrayList m_buttons;

            public LogicalToolBar(string name)
            {
                m_name = name;
                m_buttons = new ArrayList();
            }
        }

        private class LogicalButton
        {
            public string m_name;
            public Image m_image;
            public UICommand m_command;

        }

        private class ControlBinding
        {
            public ToolBarButton Item;
            public UICommand Command;
        }

        private ShortcutServiceImpl ShortcutService
        {
            get
            {
                if (m_shortcutService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_shortcutService = sp.GetService(typeof(IShortcutService))
                        as ShortcutServiceImpl;
                }
                return m_shortcutService;
            }
        }

        public bool AddToolBar(string name)
        {
            if (m_toolBarCollection.ContainsKey(name))
            {
                return false;
            }
            m_bDirty = true;
            LogicalToolBar toolbar = new LogicalToolBar(name);
            m_toolBarCollection.Add(name, toolbar);
            return true;
        }

        public bool RemoveToolBar(string name)
        {
            if (m_toolBarCollection.ContainsKey(name))
            {
                m_bDirty = true;
                m_toolBarCollection.Remove(name);
                return true;
            }
            return false;
        }

        public bool AddToolBarButton(string toolBarName, string buttonName, 
            Image buttonImage, UICommand command)
        {
            
            LogicalToolBar logicalToolbar = 
                m_toolBarCollection[toolBarName] as LogicalToolBar;

            if (logicalToolbar == null)
            {
                return false;
            }
            m_bDirty = true;

            LogicalButton logicalButon = new LogicalButton();

            logicalButon.m_name = buttonName;
            logicalButon.m_image = buttonImage;
            logicalButon.m_command = command;

            logicalToolbar.m_buttons.Add(logicalButon);
            return true;
        }


        public bool ReplaceToolBarButton(string strToolBarName,
            string strOldButtonName, string strNewButtonName,
            Image pmNewButtonImage, UICommand pmNewCommand)
        {
            LogicalToolBar logicalToolBar = m_toolBarCollection[strToolBarName]
                as LogicalToolBar;

            if (logicalToolBar != null)
            {
                foreach (LogicalButton button in logicalToolBar.m_buttons)
                {
                    if (button.m_name.CompareTo(strOldButtonName) == 0)
                    {
                        button.m_name = strNewButtonName;
                        button.m_image = pmNewButtonImage;
                        button.m_command = pmNewCommand;

                        RebuildToolBars();
                        return true;
                    }

                }
            }
            return false;
        }
        public bool EnableToolBarButton(string toolBarName, string buttonName,
            bool enabled)
        {
            LogicalToolBar logicalToolBar = m_toolBarCollection[toolBarName]
                as LogicalToolBar;
            if (logicalToolBar != null)
            {
                if (logicalToolBar.m_buttons.Contains(buttonName))
                {
                    ToolBarButton button = GetButton(logicalToolBar,
                        buttonName);
                    button.Enabled = enabled;
                    return true;
                }
            }

            return false;            
        }

        public bool CheckToolBarButton(string strToolBarName,
            string strButtonName, bool bChecked)
        {
            LogicalToolBar pmLogicalToolBar =
                m_toolBarCollection[strToolBarName] as LogicalToolBar;
            if (pmLogicalToolBar != null)
            {
                if (pmLogicalToolBar.m_buttons.Contains(strButtonName))
                {
                    ToolBarButton pmButton = GetButton(pmLogicalToolBar,
                        strButtonName);
                    pmButton.Pushed = bChecked;
                    return true;
                }
            }

            return false;
        }

        public bool RemoveToolBarButton(string toolBarName, string buttonName)
        {
            LogicalToolBar logicalToolBar = m_toolBarCollection[toolBarName]
                as LogicalToolBar;
            if (logicalToolBar != null)
            {
                if (logicalToolBar.m_buttons.Contains(buttonName))
                {
                    m_bDirty = true;
                    logicalToolBar.m_buttons.Remove(buttonName);
                    return true;
                }
            }
            return false;
        }

        public void UpdateToolTips()
        {
            foreach (ControlBinding binding in m_controlBindings)
            {
                LogicalButton logicalButton = (LogicalButton) binding.Item.Tag;
                string strToolTipText = logicalButton.m_name;
                Keys key = ShortcutService.GetKey(binding.Command);
                if (key != Keys.None)
                {
                    strToolTipText += string.Format(" ({0})",
                        ShortcutService.GetTextRepresentation(key));
                }
                binding.Item.ToolTipText = strToolTipText;
            }
        }

        public string StandardToolBarName
        {
            get { return m_standardToolBarName; }
        }

        public override bool Initialize()
        {
            //AddToolBar(StandardToolBarName);
            m_toolbar = new ToolBar();
            m_toolbar.AutoSize = true;
            m_toolbar.Wrappable = true;
            m_toolbar.ButtonClick += 
                new ToolBarButtonClickEventHandler(OnToolbarButtonClick);
            m_toolbar.Appearance = ToolBarAppearance.Flat;
            WinFormsUtility.MainForm.Controls.Add(m_toolbar);
            return base.Initialize();
        }

        #region Utility functions

        private ToolBarButton GetButton(LogicalToolBar toolBar, 
            string buttonName)
        {
            foreach (LogicalButton button in toolBar.m_buttons)
            {
                if (button.m_name.Equals(buttonName))
                {                    
                    foreach( ToolBarButton toolbarButton in m_toolbar.Buttons)
                    {
                        if (toolbarButton.Tag == button)
                        {
                            return toolbarButton;
                        }
                    }
                    
                }
            }
            return null;
        }

        private void RebuildToolBars()
        {
            m_bDirty = false;
            m_toolbar.SuspendLayout();
            m_controlBindings.Clear();
            m_toolbar.Buttons.Clear();
            m_toolbar.ImageList = new ImageList();
            m_toolbar.ImageList.ColorDepth = ColorDepth.Depth32Bit;
            ArrayList nameList = new ArrayList(m_toolBarCollection.Keys);
            nameList.Sort();
            int separatorCount = nameList.Count - 1;
            foreach (string name in nameList)
            {
                LogicalToolBar logicalBar = m_toolBarCollection[name] 
                    as LogicalToolBar;
                foreach (LogicalButton logicalButton in logicalBar.m_buttons)
                {
                    ToolBarButton button = new ToolBarButton();

                    Size imageListSize = m_toolbar.ImageList.ImageSize;
                    if (imageListSize.Height < logicalButton.m_image.Height)
                    {
                        imageListSize.Height = logicalButton.m_image.Height;
                    }
                    if (imageListSize.Width < logicalButton.m_image.Width)
                    {
                        imageListSize.Width = logicalButton.m_image.Width;
                    }
                    if (imageListSize != m_toolbar.ImageList.ImageSize)
                    {
                        m_toolbar.ImageList.ImageSize = imageListSize;
                    }
                    m_toolbar.ImageList.Images.Add(logicalButton.m_image);
                    int index = m_toolbar.ImageList.Images.Count - 1;

                    button.ImageIndex = index;
                    button.Tag = logicalButton;
                    button.Style = ToolBarButtonStyle.PushButton;
                    ControlBinding binding = new ControlBinding();
                    binding.Item = button;
                    binding.Command = logicalButton.m_command;
                    m_controlBindings.Add(binding);
                    m_toolbar.Buttons.Add(button);

                }
                if ((separatorCount--) > 0)
                {
                    ToolBarButton separator = new ToolBarButton();
                    separator.Style = ToolBarButtonStyle.Separator;
                    m_toolbar.Buttons.Add(separator);
                }
            }
            m_toolbar.ResumeLayout();
            UpdateToolTips();
        }
        #endregion

        private void dummy(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        [UICommandHandler("Idle")]
        private void Application_Idle(object sender, EventArgs e)
        {
            if (m_bDirty)
            {
                RebuildToolBars();
                return;
            }
            foreach(ControlBinding binding in m_controlBindings)
            {
                if (binding.Command != null)
                {
                    UIState state = new UIState();
                    binding.Command.ValidateCommand(state);
                    binding.Item.Enabled = state.Enabled;
                    binding.Item.Pushed = state.Checked;                   
                }
                else
                {
                    binding.Item.Enabled = false;
                }
            }
        }

        private void OnToolbarButtonClick(object sender, 
            ToolBarButtonClickEventArgs e)
        {
            //unusual way winforms makes you do this...
            foreach (ControlBinding binding in m_controlBindings)
            {
                if (e.Button == binding.Item)
                {
                    binding.Command.DoClick(e.Button.Tag, e);
                }
            }
        }

    }
}
