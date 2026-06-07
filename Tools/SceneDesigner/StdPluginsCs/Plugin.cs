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
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Xml.Schema;
using System.ComponentModel;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Services;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.TypeConverters;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs
{
	/// <summary>
	/// Summary description for Plugin.
	/// </summary>
	public class Plugin : IPlugin
	{
        #region Private Data
        private static readonly string m_menuFileExtension = "menu.xml";
        private static readonly string m_toolbarFileExtension = "toolbar.xml";
	    private static readonly string m_commandFileExtension = "commands.xml";
	    private static readonly string m_shortcutFileExtension 
            = "shortcuts.xml";
        private static readonly string m_optionsBindingFileExtension
            = "optbind.xml";
        private static readonly string ms_strPathSettingName =
            "External Application Path";
        private static readonly string ms_strCommandLineSettingName =
            "External Application Command Line";
        private static readonly string ms_strSaveWithoutPromptingSettingName =
            "Save Without Prompting";
	    #endregion

        public Plugin()
		{
            
		}

	    public string Name
	    {
	        get { throw new NotImplementedException(); }
	    }

	    public Version Version
	    {
	        get
	        { return Assembly.GetExecutingAssembly().GetName().Version; }
	    }

	    public Version ExpectedVersion
	    {
	        get 
            { return new Version(1, 0); }
	    }

	    /// <summary>
	    /// Called when a Plugin is loaded
	    /// </summary>
	    /// <param name="toolMajorVersion"></param>
	    /// <param name="toolMinorVersion"></param>
	    public void Load(int toolMajorVersion, int toolMinorVersion)
	    {
	        
	    }

	    /// <summary>
	    /// Provides a mechanism to allow packages to report services
	    /// </summary>
	    /// <returns>A list of service objects that this package provides</returns>
	    public IService[] GetProvidedServices()
	    {
            IService[] retVal = 
                new IService[] {new UICommandServiceImpl(),
                                new OptionsServiceImpl(),
                                new EntityPathServiceImpl()};

            UICommandUtility.Init();
	        return retVal;
	    }

	    /// <summary>
	    /// Provides a mechanism for a plugin to interact with services.
	    /// </summary>
	    public void Start()
	    {
            
            ServiceProvider serviceProvider = 
                ServiceProvider.Instance;

            LoadPanels();
            string searchFolderRoot = MFramework.Instance.AppStartupPath;
            string[] folders = BuildSubfolderList(searchFolderRoot);

            BuildItems(serviceProvider, 
                new DynamicMenuBuilder(serviceProvider), 
                BuildFileList(folders, m_menuFileExtension),
                BuildSchemaPath(m_menuFileExtension));

            BuildItems(serviceProvider, 
                new DynamicToolbarBuilder(serviceProvider),
                BuildFileList(folders, m_toolbarFileExtension),
                BuildSchemaPath(m_toolbarFileExtension));

            BuildItems(serviceProvider, 
                new DynamicShortcutBuilder(serviceProvider),
                BuildFileList(folders, m_shortcutFileExtension),
                BuildSchemaPath(m_shortcutFileExtension));

            CommandBindingBuilder bindingBuilder = 
                new CommandBindingBuilder(serviceProvider);

            BuildItems(serviceProvider, bindingBuilder,
                BuildFileList(folders, m_commandFileExtension),
                null);

            BuildItems(serviceProvider, new OptionsBindingsBuilder(),
                BuildFileList(folders, m_optionsBindingFileExtension),
                BuildSchemaPath(m_optionsBindingFileExtension));

            bindingBuilder.BuildFromLoadedAssemblies(); 

            RegisterPropertyTypes();

            RegisterExternalApplicationSettings();
	    }


	    private void LoadPanels()
	    {
            ServiceProvider serviceProvider = 
                ServiceProvider.Instance;
	        ICommandPanelService panelService = 
                serviceProvider.GetService(typeof(ICommandPanelService))
                as ICommandPanelService;

            IMessageService messageService = 
                serviceProvider.GetService(typeof(IMessageService))
                as IMessageService;

            IUICommandService commandService =
                serviceProvider.GetService(typeof(IUICommandService))
                as IUICommandService;
           
            panelService.RegisterPanel(new MessagesPanel(commandService,
                messageService));

            panelService.RegisterPanel(new UndoPanel());

            panelService.RegisterPanel(new ComponentPanel());

            panelService.RegisterPanel(new EntityTemplatePanel());

            panelService.RegisterPanel(new PalettePanel());

            panelService.RegisterPanel(new LayersPanel());

            panelService.RegisterPanel(new TagsPanel());

            panelService.RegisterPanel(new SelectionFilterPanel());
        }

        private void RegisterPropertyTypes()
        {
            IPropertyTypeService pmPropertyTypeService =
                ServiceProvider.Instance.GetService(
                typeof(IPropertyTypeService)) as IPropertyTypeService;
            Debug.Assert(pmPropertyTypeService != null, "Property type " +
                "service not found!");

            pmPropertyTypeService.RegisterType(new PropertyType(
                "Color (RGBA)", "Color (RGBA)", typeof(Color), null,
                typeof(ColorAConverter)));

            pmPropertyTypeService.RegisterType(new PropertyType(
                "Entity Pointer", "Entity Pointer", typeof(MEntity),
                typeof(EntityPointerEditor), null,
                typeof(EntityCollectionEditor)));

            pmPropertyTypeService.RegisterType(new PropertyType(
                "Entity Tags", "String", typeof(String),
                typeof(EntityTagsEditor), typeof(EntityTagsConverter), null),
                false);
        }

        private void RegisterExternalApplicationSettings()
        {
            ISettingsService pmSettingsService = ServiceProvider.Instance
                .GetService(typeof(ISettingsService)) as ISettingsService;

            pmSettingsService.RegisterSettingsObject(ms_strPathSettingName,
                new MFileLocation(string.Empty), SettingsCategory.PerUser);

            pmSettingsService.RegisterSettingsObject(
                ms_strCommandLineSettingName, string.Empty,
                SettingsCategory.PerUser);

            pmSettingsService.RegisterSettingsObject(
                ms_strSaveWithoutPromptingSettingName, false,
                SettingsCategory.PerUser);
        }

        private static string ConstructSceneAppPath()
        {
            string strSceneAppPath = Application.ExecutablePath;
            int iIndex = strSceneAppPath.LastIndexOf('\\');
            for (int iCount = 0; iIndex > -1 && iCount < 6; iCount++)
            {
                strSceneAppPath = strSceneAppPath.Substring(0, iIndex);
                iIndex = strSceneAppPath.LastIndexOf('\\');
            }
            strSceneAppPath += "\\Samples\\FullDemos\\SceneApp\\Win32\\" +
#if VC80
                "VC80"
#elif VC90
                "VC90"
#endif
                + "\\SceneApp.exe";

            return strSceneAppPath;
        }

        #region UI Command Binding Helpers
        private void BuildItems(ServiceProvider provider, 
            AbstractBuilder builder, FileInfo[] files, string schemaPath
            )
        {
            IMessageService messageService = 
                provider.GetService(typeof(IMessageService))
                as IMessageService;

            XmlSchema schema = null;
            if (schemaPath != null)
            {
                try
                {

                    using (FileStream schemaStream = new FileStream(schemaPath,
                               FileMode.Open, FileAccess.Read))
                    {
                        schema = XmlSchema.Read(schemaStream, null);
                    }
                }
                catch (FileNotFoundException)
                {
                    messageService.AddMessage(MessageChannelType.General,
                        string.Format("Schema '{0}' not found",
                        schemaPath));
                }
            }

            builder.Schema = schema;

            foreach(FileInfo file in files)
            {
                try
                {
                    using (Stream stream = 
                               new FileStream(file.FullName, 
                               FileMode.Open, FileAccess.Read))
                    {
                        builder.Build(stream);
                    }
                }
                catch (Exception e)
                {        
                    PluginAPI.Message message = new PluginAPI.Message();
                    message.m_strText = "Could not load file: " +
                        Environment.NewLine + file.FullName;
                    message.m_strDetails = e.ToString();
                    messageService.AddMessage(MessageChannelType.Errors,
                        message);
                }
            }
        }

        private string[] BuildSubfolderList(string rootPath)
        {
            ArrayList directoryNames = new ArrayList();
            DirectoryInfo rootInfo = new DirectoryInfo(rootPath);
            directoryNames.Add(rootInfo.FullName);
            DirectoryInfo[] subFolders = rootInfo.GetDirectories();
            foreach (DirectoryInfo di in subFolders)
            {
                directoryNames.AddRange(BuildSubfolderList(di.FullName));
            }           
            return (string[]) directoryNames.ToArray(typeof(string));
        }

        private FileInfo[] BuildFileList(string[] folders, string extension)
        {
            ArrayList fileInfoList = new ArrayList();
            foreach (string folder in folders)
            {
                DirectoryInfo di = new DirectoryInfo(folder);
                FileInfo[] files = di.GetFiles("*." + extension);
                fileInfoList.AddRange(files);
            }
            return (FileInfo[]) fileInfoList.ToArray(typeof(FileInfo));
        }

        private string BuildSchemaPath(string extension)
        {
            return MFramework.Instance.AppStartupPath + 
                @"data\" + extension.Replace("xml", "xsd");
        }
        #endregion

        #region UI Command Handlers
        [UICommandHandler("CreateCameraFromViewport")]
        private static void OnCreateCameraFromViewport(object sender,
            EventArgs args)
        {
            // Take the current viewport camera and clone it
            MEntity currentCamera = MFramework.Instance.ViewportManager
                .ActiveViewport.CameraEntity;

            // Create unique name for the new camera.
            string cloneName = currentCamera.Name;

            // Search for last space character in proposed name.
            int iIndex = currentCamera.Name.LastIndexOf(' ');
            if (iIndex == currentCamera.Name.Length - 1)
            {
                iIndex = currentCamera.Name.LastIndexOf(' ', iIndex - 1);
            }
            if (iIndex > -1)
            {
                string strSuffix = currentCamera.Name.Substring(iIndex + 1);
                try
                {
                    int iSuffix = Int32.Parse(strSuffix);
                    cloneName = currentCamera.Name.Substring(0, iIndex);
                }
                catch (FormatException)
                {
                    // The suffix is not an integer; set the base name to the
                    // proposed name.
                    cloneName = currentCamera.Name;
                }

            }

            cloneName = cloneName + " 01";
            cloneName = MFramework.Instance.Scene.GetUniqueEntityName(
                cloneName);
            MEntity[] amClones = currentCamera.Clone(cloneName, false);
            Debug.Assert(amClones.Length == 1);
            MEntity clonedCamera = amClones[0];

            // Add it to the scene
            MFramework.Instance.Scene.AddEntity(clonedCamera, true);

            // Select the current camera, de-selecting any current selections
            ISelectionService selectionService =
                ServiceProvider.Instance.GetService(typeof(ISelectionService))
                as ISelectionService;

            selectionService.ClearSelectedEntities();
            selectionService.AddEntityToSelection(clonedCamera);

            // Set the viewport to use the newly cloned camera
            MFramework.Instance.ViewportManager.ActiveViewport.SetCamera(
                clonedCamera, false);
        }

        [UICommandValidatorAttribute("CreateCameraFromViewport")]
        private static void OnCreateCameraFromViewport(
            object pmSender, UIState pmState)
        {
            if (MFramework.Instance.LayerManager.ActiveLayer == null)
                pmState.Enabled = false;
            else
                pmState.Enabled = true;
        }

        [UICommandHandler("LaunchExternalApplication")]
        private static void OnLanchExternalApplication(object sender,
            EventArgs args)
        {
            ISettingsService settingService =
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            bool bSaveWithoutPrompting = (bool)
                settingService.GetSettingsObject(
                ms_strSaveWithoutPromptingSettingName,
                SettingsCategory.PerUser);
            if (!bSaveWithoutPrompting)
            {
                DialogResult result = MessageBox.Show(
                    "The scene must now be saved before launching the " +
                    "external application.\n\nDo you want to automatically " +
                    "save without prompting from now on?",
                    "Save Without Prompting?", MessageBoxButtons.YesNoCancel,
                    MessageBoxIcon.Question);
                switch (result)
                {
                    case DialogResult.Yes:
                        settingService.SetSettingsObject(
                            ms_strSaveWithoutPromptingSettingName, true,
                            SettingsCategory.PerUser);
                        break;
                    case DialogResult.No:
                        // Don't do anything. The setting is already set to 
                        // prompt.
                        break;
                    case DialogResult.Cancel:
                        // Abort the external application launch.
                        return;
                }
            }

            // Save the scene file.
            IUICommandService UICommandService = ServiceProvider.Instance
                .GetService(typeof(IUICommandService)) as IUICommandService;
            UICommand saveCommand = UICommandService.GetCommand("SaveFile");
            if (saveCommand != null)
            {
                saveCommand.DoClick(null, null);
            }

            MFileLocation pmFileLocation = (MFileLocation)
                settingService.GetSettingsObject(ms_strPathSettingName,
                SettingsCategory.PerUser);
            string strSceneAppPath = ConstructSceneAppPath();
            string strAppPath = string.Empty;
            if (pmFileLocation != null)
            {
                strAppPath = pmFileLocation.Path;
            }
            if (strAppPath.Equals(string.Empty))
            {
                strAppPath = strSceneAppPath;
            }

            string strCommandLine = (string) settingService.GetSettingsObject(
               ms_strCommandLineSettingName, SettingsCategory.PerUser);
            if (strCommandLine == null)
            {
                strCommandLine = string.Empty;
            }
            string strFullCommandLine = string.Format(" \"{0}\" {1}",
                MFramework.Instance.CurrentFilename, strCommandLine);

            bool bLaunched = false;
            while (!bLaunched)
            {
                try
                {
                    Process myProcess = new Process();
                    myProcess.StartInfo.Arguments = strFullCommandLine;
                    myProcess.StartInfo.FileName = "\"" + strAppPath + "\"";
                    bLaunched = myProcess.Start();
                }
                catch (System.Exception)
                {
                }

                if (!bLaunched)
                {
                    if (strAppPath.Equals(strSceneAppPath))
                    {
                        MessageBox.Show("Cannot launch SceneApp from its " +
                            "default path at:\n\n" + strSceneAppPath +
                            "\n\nPlease check the path and try again.",
                            "External Application Launch Failure",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }

                    if (MessageBox.Show("Cannot launch external " +
                        "application from the following location:\n\n" +
                        strAppPath + "\n\nwith the following command-line " +
                        "parameters:\n\n" + strFullCommandLine + "\n\nWould " +
                        "you like to reset your external application path " +
                        "to the default and try again?",
                        "External Application Launch Failure",
                        MessageBoxButtons.YesNo, MessageBoxIcon.Question) ==
                        DialogResult.Yes)
                    {
                        // Reset app path to default to try again.
                        strAppPath = strSceneAppPath;

                        // Reset external application path setting to default.
                        settingService.SetSettingsObject(ms_strPathSettingName,
                            new MFileLocation(string.Empty),
                            SettingsCategory.PerUser);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        [UICommandHandler("AddTagToSelectedEntities")]
        private static void OnAddTagToSelectedEntities(object sender,
            EventArgs args)
        {
            AddTagDlg dlg = new AddTagDlg();
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                ISelectionService selectionService = (ISelectionService)
                ServiceProvider.Instance.GetService(typeof(ISelectionService));
                Debug.Assert(selectionService != null,
                    "Null selection service!");

                ICommandService commandService = (ICommandService)
                    ServiceProvider.Instance.GetService(
                    typeof(ICommandService));
                Debug.Assert(commandService != null, "Null command service!");

                IMessageService messageService = (IMessageService)
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService));
                Debug.Assert(messageService != null, "Null message service!");

                commandService.BeginUndoFrame(string.Format("Add the " +
                    "\"{0}\" tag to selected entities", dlg.TagText));
                MEntity[] selectedEntities =
                    selectionService.GetSelectedEntities();
                foreach (MEntity entity in selectedEntities)
                {
                    if (entity.Writable)
                    {
                        if (!entity.ContainsTag(dlg.TagText))
                        {
                            entity.AddTag(dlg.TagText);
                        }
                    }
                    else
                    {
                        messageService.AddMessage(MessageChannelType.Errors,
                            new Emergent.Gamebryo.SceneDesigner.PluginAPI
                            .Message("\"" + dlg.TagText + "\" tag could not " +
                            "be added to \"" + entity.Name + "\" entity.",
                            "The entity is read-only and cannot be modified.",
                            entity));
                    }
                }
                commandService.EndUndoFrame(true);
            }
        }

        [UICommandValidatorAttribute("AddTagToSelectedEntities")]
        private static void OnValidateAddTagToSelectedEntities(
            object pmSender, UIState pmState)
        {
            ISelectionService selectionService = (ISelectionService)
                ServiceProvider.Instance.GetService(typeof(ISelectionService));
            Debug.Assert(selectionService != null, "Null selection service!");
            pmState.Enabled = (selectionService.NumSelectedEntities > 0);
        }

        [UICommandHandler("RemoveTagsFromSelectedEntities")]
        private static void OnRemoveTagsFromSelectedEntities(object sender,
            EventArgs args)
        {
            ISelectionService selectionService = (ISelectionService)
                ServiceProvider.Instance.GetService(typeof(ISelectionService));
            Debug.Assert(selectionService != null, "Null selection service!");

            IMessageService messageService = (IMessageService)
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService));
            Debug.Assert(messageService != null, "Null message service!");

            MEntity[] selectedEntities = selectionService.GetSelectedEntities();
            Debug.Assert(selectedEntities.Length > 0);

            List<string> allTags = new List<string>();
            foreach (MEntity entity in selectedEntities)
            {
                string[] tags = entity.TagsArray;
                foreach (string tag in tags)
                {
                    if (!allTags.Contains(tag))
                    {
                        allTags.Add(tag);
                    }
                }
            }

            if (allTags.Count == 0)
            {
                MessageBox.Show("There are no tags on any of the " +
                    "selected entities.", "Cannot Remove Tag",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            RemoveTagDlg dlg = new RemoveTagDlg();
            dlg.InputTags = allTags.ToArray();
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                ICommandService commandService = (ICommandService)
                    ServiceProvider.Instance.GetService(
                    typeof(ICommandService));
                Debug.Assert(commandService != null, "Null command service!");

                string[] tagsToRemove = dlg.TagsToRemove;
                string strMessage;
                if (tagsToRemove.Length == 1)
                {
                    strMessage = string.Format("Remove the \"{0}\" tag from " +
                        "selected entities", tagsToRemove[0]);
                }
                else
                {
                    strMessage = string.Format("Remove {0} tags from " +
                        "selected entities", tagsToRemove.Length);
                }
                commandService.BeginUndoFrame(strMessage);
                foreach (MEntity entity in selectedEntities)
                {
                    if (entity.Writable)
                    {
                        foreach (string tag in tagsToRemove)
                        {
                            if (entity.ContainsTag(tag))
                            {
                                entity.RemoveTag(tag);
                            }
                        }
                    }
                    else
                    {
                        messageService.AddMessage(MessageChannelType.Errors,
                            new Emergent.Gamebryo.SceneDesigner.PluginAPI
                            .Message("Tags could not be removed from \"" +
                            entity.Name + "\" entity.",
                            "The entity is read-only and cannot be modified.",
                            entity));
                    }
                }
                commandService.EndUndoFrame(true);
            }
        }

        [UICommandValidatorAttribute("RemoveTagsFromSelectedEntities")]
        private static void OnValidateRemoveTagsFromSelectedEntities(
            object pmSender, UIState pmState)
        {
            ISelectionService selectionService = (ISelectionService)
                ServiceProvider.Instance.GetService(typeof(ISelectionService));
            Debug.Assert(selectionService != null, "Null selection service!");
            pmState.Enabled = (selectionService.NumSelectedEntities > 0);
        }

        [UICommandHandler("EditCommonTagsOnSelectedEntities")]
        private static void OnEditCommonTagsOnSelectedEntities(object sender,
            EventArgs args)
        {
            ISelectionService selectionService = (ISelectionService)
                ServiceProvider.Instance.GetService(typeof(ISelectionService));
            Debug.Assert(selectionService != null, "Null selection service!");

            IMessageService messageService = (IMessageService)
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService));
            Debug.Assert(messageService != null, "Null message service!");

            MEntity[] selectedEntities = selectionService
                .GetSelectedEntities();
            Debug.Assert(selectedEntities.Length > 0);

            List<MEntity> writableEntities = new List<MEntity>(
                selectedEntities.Length);
            foreach (MEntity entity in selectedEntities)
            {
                if (entity.Writable)
                {
                    writableEntities.Add(entity);
                }
                else
                {
                    messageService.AddMessage(MessageChannelType.Errors,
                        new Emergent.Gamebryo.SceneDesigner.PluginAPI.Message(
                        "Tags could not be edited on \"" + entity.Name +
                        "\" entity.",
                        "The entity is read-only and cannot be modified.",
                        entity));
                }
            }
            if (writableEntities.Count == 0)
            {
                return;
            }

            List<string> commonTags = new List<string>();
            string[] tags = writableEntities[0].TagsArray;
            foreach (string tag in tags)
            {
                if (!string.IsNullOrEmpty(tag))
                {
                    commonTags.Add(tag);
                }
            }
            List<string> tagsToRemove = new List<string>(commonTags.Count);
            for (int i = 1; i < writableEntities.Count; ++i)
            {
                MEntity entity = writableEntities[i];
                foreach (string tag in commonTags)
                {
                    if (!entity.ContainsTag(tag))
                    {
                        if (!tagsToRemove.Contains(tag))
                        {
                            tagsToRemove.Add(tag);
                        }
                    }
                }
            }
            foreach (string tag in tagsToRemove)
            {
                commonTags.Remove(tag);
            }

            string tagsString = null;
            if (commonTags.Count > 0)
            {
                tagsString = string.Join(MEntity.TagDelimiterString,
                    commonTags.ToArray());
                tagsString += MEntity.TagDelimiterString;
            }

            EntityTagsEditorDialog dlg = new EntityTagsEditorDialog();
            dlg.TagsString = tagsString;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                List<string> newTags = new List<string>();
                if (!string.IsNullOrEmpty(dlg.TagsString))
                {
                    string[] newTagsArray = dlg.TagsString.Split(
                        MEntity.TagDelimiterString.ToCharArray());
                    foreach (string tag in newTagsArray)
                    {
                        if (!string.IsNullOrEmpty(tag))
                        {
                            newTags.Add(tag);
                        }
                    }
                }

                ICommandService commandService = (ICommandService)
                    ServiceProvider.Instance.GetService(
                    typeof(ICommandService));
                Debug.Assert(commandService != null, "Null command service!");

                commandService.BeginUndoFrame("Modified tags on selected " +
                    "entities");
                foreach (MEntity entity in writableEntities)
                {
                    foreach (string tag in commonTags)
                    {
                        if (!newTags.Contains(tag) && entity.ContainsTag(tag))
                        {
                            entity.RemoveTag(tag);
                        }
                    }

                    foreach (string tag in newTags)
                    {
                        if (!commonTags.Contains(tag))
                        {
                            entity.AddTag(tag);
                        }
                    }
                }
                commandService.EndUndoFrame(true);
            }
        }

        [UICommandValidatorAttribute("EditCommonTagsOnSelectedEntities")]
        private static void OnValidateEditCommonTagsOnSelectedEntities(
            object pmSender, UIState pmState)
        {
            ISelectionService selectionService = (ISelectionService)
                ServiceProvider.Instance.GetService(typeof(ISelectionService));
            Debug.Assert(selectionService != null, "Null selection service!");
            pmState.Enabled = (selectionService.NumSelectedEntities > 0);
        }
        #endregion

        [DllInit]
        private static void Init()
        {
            
        }

        [DllShutdown]
        private static void Shutdown()
        {
            
        }
	}
}
