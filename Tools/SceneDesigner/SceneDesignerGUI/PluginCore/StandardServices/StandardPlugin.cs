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
using System.Drawing.Imaging;
using System.Reflection;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for StandardPlugin.
    /// </summary>
    public sealed class StandardPlugin : IPlugin
    {
        public StandardPlugin()
        {
        }

        public string Name
        {
            get { return this.GetType().ToString(); }
        }

        public Version Version
        {
            get {  return Assembly.GetExecutingAssembly().GetName().Version; }
        }

        public Version ExpectedVersion
        {
            get { return  Assembly.GetExecutingAssembly().GetName().Version; }
        }

        public void Load(int toolMajorVersion, int toolMinorVersion)
        {
        }

        public IService[] GetProvidedServices()
        {
            return new IService[]
                {
                    new MenuServiceImpl(WinFormsUtility.MainForm.Menu),
                    new ToolBarServiceImpl(),
                    new CommandPanelServiceImpl(),
                    new SettingsServiceImpl(),
                    new ShortcutServiceImpl(),
                    new FileHandlerServiceImpl()
                };
        }

        public void Start()
        {
            ServiceProvider serviceProvider = ServiceProvider.Instance;

            //Start standard panels
            CreateStandardPanels();
        }

        private void CreateStandardPanels()
        {
            ServiceProvider provider = ServiceProvider.Instance;

            ICommandPanelService panelService = 
                provider.GetService(typeof(ICommandPanelService))
                as ICommandPanelService;

            IUICommandService commandService = 
                provider.GetService(typeof(IUICommandService))
                as IUICommandService;

            // Create command panels.
            panelService.RegisterPanel(new PropertiesPanel());
            panelService.RegisterPanel(new SelectionPanel(commandService));
            panelService.RegisterPanel(new SelectionSetPanel());
            panelService.RegisterPanel(new ViewportSettingsPanel());
            panelService.RegisterPanel(new RenderForm());

            //panelService.ShowAllPanels();

        }

    }
}
