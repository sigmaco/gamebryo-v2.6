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
using System.IO;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using WeifenLuo.WinFormsUI;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for CommandPanelServiceImpl.
    /// </summary>
    public class CommandPanelServiceImpl : StandardServiceBase,  
        ICommandPanelService
    {
        #region Private Data
        private ArrayList m_aPanels;
        #endregion

        public CommandPanelServiceImpl()
        {
            m_aPanels = new ArrayList();
        }

        public override bool Start()
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IMenuService menuService = sp.GetService(typeof(IMenuService)) 
                as IMenuService;

            IUICommandService commandService = 
                sp.GetService(typeof(IUICommandService)) 
                as IUICommandService;

            commandService.BindCommands(this);

            //Application.Idle += new EventHandler(Application_Idle);

            return true;
        }

        public void RegisterPanel(Form form)
        {

            CommandPanelAdapter adapter = new CommandPanelAdapter(form);
            ICommandPanel commandPanelForm = form as ICommandPanel;
            if (commandPanelForm != null)
            {
                commandPanelForm.RegisterEventHandlers();
            }
            m_aPanels.Add(adapter);
            m_aPanels.Sort(new FormTextCompare());
            RebuildMenus();            
        }

        public void ShowAllPanels()
        {
            foreach (CommandPanelAdapter panel in m_aPanels)
            {
                panel.Show(WinFormsUtility.DockPanel);
            }            
        }

        public void ShowPanel(string name, bool show)
        {
            CommandPanelAdapter panel = FindPanel(name) as CommandPanelAdapter;
            if (panel != null)
            {
                panel.Show(WinFormsUtility.DockPanel);
            }
            else
            {
                MessageBox.Show("The panel: \'" + name + "\'" +
                    " can not be shown because it was not found.",
                    "Panel not found.");
            }
        }
        
        public Form GetPanel(string name)
        {
            CommandPanelAdapter panel = FindPanel(name) as CommandPanelAdapter;
            if (panel != null)
            {
                return panel.InternalForm;
            }
            return null;
        }

        internal void LoadConfig(Stream stream)
        {
            WinFormsUtility.DockPanel.LoadFromXml(stream, 
                new DeserializeDockContent(GetContentFromPersistString));
        }

        private void RebuildMenus()
        {
            ServiceProvider sp = ServiceProvider.Instance;
            if (sp == null)
            {
                return;
            }

            IUICommandService commandService = 
                sp.GetService(typeof(IUICommandService)) 
                as IUICommandService;

            IMenuService menuService = 
                sp.GetService(typeof(IMenuService)) 
                as IMenuService;

            //clear exisiting menu
            foreach (Form panel in m_aPanels)
            {
                string commandName = "CommandPanelService." + panel.Text;
                UICommand command = commandService.GetCommand(commandName);
                string[] menus = menuService.GetAssignedMenus(command);
                if (menus.Length > 0)
                {
                    command.Click -= new EventHandler(MenuClickHandler);
                    command.Validate -= 
                        new UICommand.ValidateCommandHandler(command_Validate);
                    menuService.RemoveMenu(
                        "View##Command Panels##" + panel.Text, command, false);
                }
            }

            foreach (Form panel in m_aPanels)
            {
                string commandName = "CommandPanelService." + panel.Text;
                UICommand command = commandService.GetCommand(commandName);
                command.Click += new EventHandler(MenuClickHandler);
                command.Validate += 
                    new UICommand.ValidateCommandHandler(command_Validate);
                menuService.AddMenu("View##Command Panels##" + panel.Text,
                    command);
            }

        }


        private IDockContent GetContentFromPersistString(string persistString)
        {
            foreach (DockContent form in m_aPanels)
            {
                if(form.Text.Equals(persistString))
                {
                    return form;
                }
            }
            return null;
        }

        internal void SaveConfig(Stream stream)
        {
            WinFormsUtility.DockPanel.SaveAsXml(stream, Encoding.UTF8);

        }

        private class FormTextCompare : IComparer
        {
            public int Compare(object x, object y)
            {
                Form xForm = (Form)x;
                Form yForm = (Form)y;
                return xForm.Text.CompareTo(yForm.Text);
            }
        }

        private class FormToStringCompare : IComparer
        {
            public int Compare(object x, object y)
            {
                Form xForm = (Form)x;
                string text = (string)y;
                return xForm.Text.CompareTo(text);
            }   

        }

        private Form FindPanel(string name)
        {
            int index = m_aPanels.BinarySearch(name, 
                new FormToStringCompare());
            if (index >= 0)
            {
                return m_aPanels[index] as Form;
            }
            else
            {
                return null;
            }
        }

        private void MenuClickHandler(object sender, EventArgs e)
        {
            UICommand command = sender as UICommand;
            if (command != null)
            {
                string panelName =
                    command.Name.Replace("CommandPanelService.", ""); 
                CommandPanelAdapter panel = FindPanel(panelName)
                    as CommandPanelAdapter;
                if (panel != null)
                {
                    bool panelVisible = GetPanelVisibleState(panel);
                    if (panelVisible && panel.Visible == false)
                    {
                        //In this case we just bring the panel up front
                        panel.Show();
                    }
                    else
                    {
                        SetPanelVisibleState(panel, !panelVisible);
                    }
                }
            }
        }

        //Lifted from Animation Tool
        private static void SetPanelVisibleState(CommandPanel panel, 
            bool visible)
        {
            if (visible)
            {
                // Would like to just set IsHidden = false, but doing that
                // can cause the app to crash if ProjectView.Show is not
                // called. So we save the active view and reshow it as a way
                // around the issue.
                // I was going to use ActiveView =
                //                  dpDockManager.ActiveDocument;
                // but this is unrelyable when doc tabs are at bottom...
                // so we search and find which are not hidden and it.
                DockContent ActiveView = panel;
                DockContentCollection contents = 
                    WinFormsUtility.DockPanel.Contents;
                foreach (DockContent d in contents)
                    if (d.IsHidden == false)
                        ActiveView = d;

                panel.Show(WinFormsUtility.DockPanel);
                //ActiveView.Show(WinFormsUtility.DockPanel);
            }
            else
            {
                try { panel.IsHidden = true; }
                catch { }
            }

        }

        private static bool GetPanelVisibleState(CommandPanel panel)
        {
            return (panel.DockState != DockState.Unknown) 
                && !panel.IsHidden;
        }

        private void command_Validate(object sender, UIState state)
        {
            UICommand command = sender as UICommand;
            if (command != null)
            {
                string panelName = 
                    command.Name.Replace("CommandPanelService.", ""); 
                CommandPanel panel = FindPanel(panelName) as CommandPanel;
                if (panel != null && GetPanelVisibleState(panel))
                {
                    state.Checked = true;
                }
                else
                {
                    state.Checked = false;
                }
            }
        }
    }
}
