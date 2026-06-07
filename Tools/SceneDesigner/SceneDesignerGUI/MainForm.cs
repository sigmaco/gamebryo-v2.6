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
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices;
using Emergent.Gamebryo.SceneDesigner.GUI.Utility;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    /// <summary>
    /// Summary description for MainForm.
    /// </summary>
    public class MainForm : System.Windows.Forms.Form
    {
        private ShortcutServiceImpl m_shortcutService;
        private System.ComponentModel.IContainer components;
        private TimedInvoker m_timer;
        IUICommandService m_uiCommandService;
        UICommand m_idleCommand;
        private static readonly string dockConfigSettingName = 
            "DockConfig";
        private System.Windows.Forms.Timer m_timerComponent;
        private bool m_lastDirtyStatus;
        private DateTime m_timeOfLastAutoSave;
        private bool m_bClearWindowLayout = false;
        private bool m_bProcessDelayedLoading;
        static private Mutex m_restartLock = null;
        private bool m_bWindowPositionSet = false;
        /// <summary>
        /// Only one instace of Scene Designer is allowed.
        /// </summary>
        static private Mutex m_instanceLock;

        private class PluginLoadException : ApplicationException
        {
            public PluginLoadException(string msg) : base (msg) {}
        }



        public MainForm()
        {


            //
            // Required for Windows Form Designer support
            //
            m_bProcessDelayedLoading = true;
            InitializeComponent();

            WinFormsUtility.MainForm = this;
            WinFormsUtility.DockPanel = this.m_ctlDockPanel;
            LoadPlugins();

            InitializeGUI();

            ServiceProvider sp = ServiceProvider.Instance;
            m_uiCommandService = sp.GetService(typeof(IUICommandService))
                as IUICommandService;

            MFramework.Instance.Startup();
        }

        private ShortcutServiceImpl ShortcutService
        {
            get
            {
                if (m_shortcutService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_shortcutService = 
                        sp.GetService(typeof(IShortcutService)) 
                        as ShortcutServiceImpl;                    
                }
                return m_shortcutService;
            }
        }

        private void TimerCallBack(object sender, EventArgs e)
        {
            try
            {
                //It may be possible to get this callback in the middle of
                //Shutting things down, so guard against that
                if (!this.Disposing && !this.IsDisposed)
                {
                    m_idleCommand.DoClick(this, null);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Exception thrown in timer code:");
                Debug.WriteLine(ex);
            }
        }

        private void LoadPlugins()
        {
            try 
            {
                string pluginPath = 
                    MFramework.Instance.AppStartupPath + @"plugins";

                PluginManager.LoadPlugins(pluginPath);
            }
            catch (Exception e)
            {
                //This amounts to a catastrophic error. The application
                //cannot recover from this, so we will print this dialog
                //and throw a PlinLoadException, which will cause the app 
                //to exit
                Console.WriteLine("Uncaught exception loading plugins:");
                Console.WriteLine(e);
                MessageBox.Show("Application could not load plugins" + 
                    Environment.NewLine +
                    e.ToString(),
                                "Critical Error");
                throw new PluginLoadException(
                       "Plugins could not load properly");
            }
        }

        private static string ms_strDisplayNewSceneFormSettingName =
            "DisplayNewSceneForm";
        private static string ms_strDisplayNewSceneFormOptionName =
            "Palettes.Prompt to Select Palette Folder for New Scenes";

        private void InitializeGUI()
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IUICommandService commandService =
                sp.GetService(typeof(IUICommandService))
                as IUICommandService;
    
            commandService.BindCommands(this);
    
    
            ISettingsService settingsService = 
                sp.GetService(typeof(ISettingsService))
                as ISettingsService;

            settingsService.RegisterSettingsObject(
                "AutoSaveIntervalInMinutes", 1, SettingsCategory.PerUser);

            settingsService.RegisterSettingsObject(
                ms_strDisplayNewSceneFormSettingName, true,
                SettingsCategory.PerUser);

            IOptionsService pmOptionsService = sp.GetService(
                typeof(IOptionsService)) as IOptionsService;
            Debug.Assert(pmOptionsService != null,
                "Options service not found!");

            pmOptionsService.AddOption(ms_strDisplayNewSceneFormOptionName,
                SettingsCategory.PerUser,
                ms_strDisplayNewSceneFormSettingName);
            pmOptionsService.SetHelpDescription(
                ms_strDisplayNewSceneFormOptionName, "Indicates whether or " +
                "not a dialog box is displayed when a new scene is created " +
                "that prompts the user to select a palette folder " +
                "location. If this option is set to false, the most " +
                "recently used palette folder location will be used for " +
                "the new scene.");
    
            MenuServiceImpl menuService =
                sp.GetService(typeof(IMenuService)) as MenuServiceImpl;

            // This function must be first called here to load the saved size
            // for the Scene Designer window. It will be called again later to
            // ensure that the initialization of the third party docking code
            // does not resize the window from its stored size. The Boolean
            // passed in here allows the function to detect where it is being
            // called from to process special logic.
            LoadMainWindowPosition(false);

            menuService.AnnotateMenus();
            ComponentManagementUtilities.BuildComponentListFromPalettes(
                MFramework.Instance.PaletteManager.GetPalettes());
            MRUManager.Init();

            FileHandlerServiceImpl fileHandlerService =
                sp.GetService(typeof (IFileHandlerService)) 
                as FileHandlerServiceImpl;

            fileHandlerService.RegisterFileLoader(".gsa",
              new FileLoadHandler(SceneFileLoaderCallBack));
        }

        private void LoadMainWindowPosition(bool bFromOnIdle)
        {
            // This function is called twice: once during window creation and
            // once on the first OnIdle call. This is necessary to ensure that
            // the final window size actually matches the size stored in the
            // settings. bFromOnIdle indicates whether or not this function
            // is being called from the OnIdle function. A value of true
            // indicates that this is the second call to this function.

            // Get settings service.
            ServiceProvider sp = ServiceProvider.Instance;
            ISettingsService settingsService = 
                sp.GetService(typeof(ISettingsService))
                as ISettingsService;

            // Load window settings.
            object objWindowState = settingsService.GetSettingsObject(
                "MainWindowState", SettingsCategory.PerUser);
            object objWindowSize = settingsService.GetSettingsObject(
                "MainWindowSize", SettingsCategory.PerUser);
            object objWindowLocation = settingsService.GetSettingsObject(
                "MainWindowLocation", SettingsCategory.PerUser);

            if (objWindowState != null)
            {
                if (objWindowSize != null && objWindowLocation != null)
                {
                    this.ClientSize = (Size)objWindowSize;
                    this.Location = (Point)objWindowLocation;
                }

                FormWindowState ws = (FormWindowState)objWindowState;
                if (ws != FormWindowState.Minimized)
                {
                    this.WindowState = ws;
                }
            }
            else
            {
                // If we are in this block, it indicates that no window
                // settings exist yet. This happens the first time Scene
                // Designer is run for a user. In this case, the window is
                // intially set to be maximized. If this is the second call
                // to this function, then the window state is set to Normal,
                // but the size of the window is set to be slightly smaller
                // than its maximized state. This is to avoid a graphical
                // problem that can exhibit itself when the Scene Designer
                // window is maximized with particular configurations.

                this.WindowState = FormWindowState.Maximized;
                if (bFromOnIdle)
                {
                    Size clientSize = this.ClientSize;
                    clientSize.Height -= 10;
                    clientSize.Width -= 10;
                    this.WindowState = FormWindowState.Normal;
                    this.ClientSize = clientSize;
                }
            }

            // This block of code ensures that the launched Scene Designer
            // window is initially displayed on top of all other windows on the
            // desktop. If this is removed, the initialization of the thrid-
            // party docking code will cause the Scene Designer window to
            // appear behind other open windows.
            if (bFromOnIdle)
            {
                this.TopMost = true;
                this.TopMost = false;
            }
        }

        private void SaveMainWindowPosition()
        {
            if (this.WindowState != FormWindowState.Minimized)
            {
                ServiceProvider sp = ServiceProvider.Instance;
                ISettingsService settingsService =
                    sp.GetService(typeof(ISettingsService))
                    as ISettingsService;

                settingsService.SetSettingsObject("MainWindowState",
                    this.WindowState, SettingsCategory.PerUser);

                if (this.WindowState != FormWindowState.Maximized)
                {
                    settingsService.SetSettingsObject("MainWindowSize",
                        this.ClientSize, SettingsCategory.PerUser);

                    settingsService.SetSettingsObject("MainWindowLocation",
                        this.Location, SettingsCategory.PerUser);
                }
            }
        }

        [UICommandHandler("ReCheckSceneAfterPaletteImport")]
        private void OnReCheckScene(object sender, EventArgs args)
        {
            MPalette[] palettes = 
                MFramework.Instance.PaletteManager.GetPalettes();
            MScene scene =MFramework.Instance.Scene;

            ConflictManagementUtilities.CheckForConflicts(scene,
                palettes);
            MPalette orphans = 
                EntityManagementUtilities.ResolveMasterEntities(
                scene, palettes);
            //We should never get orphans as the result of importing a palette
            Debug.Assert(orphans.Scene.GetEntities().Length == 0);
            orphans.Dispose();
        }

        [UICommandHandler("SavePalettes")]
        private void OnSavePalettes(object sender, EventArgs args)
        {
            SavePalettes(true, false);
        }

        private void SavePalettes(bool promptIfReadonly,
            bool overwriteIfNoPrompt)
        {
            MPalette[] dirtyPalettes = GetDirtyPalettes();
            MPrefab[] dirtyPrefabs = GetDirtyPrefabs();

            if (dirtyPalettes.Length == 0 && dirtyPrefabs.Length == 0)
            {
                return;
            }

            if (!CheckPaletteFolderExists())
            {
                return;
            }

            if (!CheckForReadOnlyPaletteFiles(dirtyPalettes, dirtyPrefabs,
                promptIfReadonly, overwriteIfNoPrompt))
            {
                return;
            }

            MPrefab[] prefabs = MFramework.Instance.PrefabManager.GetPrefabs();
            foreach (MPrefab prefab in prefabs)
            {
                if (prefab.Dirty)
                {
                    if (!prefab.Writable)
                    {
                        FileInfo info = new FileInfo(prefab.SourceFilename);
                        info.Attributes = info.Attributes & 
                            ~FileAttributes.ReadOnly;
                    }
                    prefab.Save();
                }
            }

            string[] streamingExtensions = 
                MUtility.GetStreamingFormatExtensions();

            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;

            string paletteFolder = paletteManager.PaletteFolder;

            foreach (MPalette palette in paletteManager.GetPalettes())
            {
                if (palette.DontSave)
                {
                    if (palette.Dirty)
                    {
                        DirtyBitUtilities.MakeSceneClean(palette.Scene);
                    }
                    continue;
                }

                if (!palette.Dirty)
                    continue;

                string paletteFileName = paletteFolder + palette.Name 
                    + "." + paletteManager.PaletteFileExtension;

                FileInfo fi = new FileInfo(paletteFileName);
                bool bReadonly = (fi.Attributes & FileAttributes.ReadOnly)
                    == FileAttributes.ReadOnly;

                if (fi.Exists && bReadonly)
                {
                    fi.Attributes = fi.Attributes & 
                        ~FileAttributes.ReadOnly;
                }
                Application.DoEvents();
                SceneManagementUtilities.SetNameOnTemplates(palette);
                
                paletteManager.SavePalette(palette, 
                    paletteFolder + palette.Name + "." + 
                    paletteManager.PaletteFileExtension,
                    streamingExtensions[0]);

                DirtyBitUtilities.MakeSceneClean(palette.Scene);
            }
            
        }

        /// <summary>
        /// Checks to make sure the palette files are writeable
        /// </summary>
        /// <param name="palettes"></param>
        /// <returns>true</returns>
        private bool CheckForReadOnlyPaletteFiles(MPalette[] palettes, 
            MPrefab[] prefabs, bool displayPrompt, bool overwriteIfNoPrompt)
	    {
            MPaletteManager paletteManager =
                MFramework.Instance.PaletteManager;
            string folder = paletteManager.PaletteFolder;

            ArrayList readonlyPaletteFiles = new ArrayList();

	        foreach (MPalette palette in palettes)
	        {
                if (!palette.Writable && palette.Dirty)
                    readonlyPaletteFiles.Add(palette.Scene.SourceFilename);
	        }

            // Prefabs are part of palettes too.  Check those files.
            foreach (MPrefab prefab in prefabs)
            {
                if (!prefab.Writable && prefab.Dirty && 
                    prefab.Scene != MFramework.Instance.Scene)
                    readonlyPaletteFiles.Add(prefab.Scene.SourceFilename);
            }

            if (readonlyPaletteFiles.Count > 0)
            {
                bool overwrite = overwriteIfNoPrompt;
                if (displayPrompt)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.Append("The following palette file(s) are " +
                        "read-only.\n\n");
                    foreach (string paletteFile in readonlyPaletteFiles)
                    {
                        sb.AppendFormat("{0}\n", paletteFile);
                    }
                    sb.Append("\nWould you like to over write them?\n\n");
                    sb.Append(
                        "Note: If you click 'No', none " +
                        "of your palettes will be saved");
                    string messageText = sb.ToString();
                    DialogResult result = MessageBox.Show(messageText,
                        "Palette Files Are Read Only.",
                        MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                    overwrite = (result == DialogResult.Yes);
                }
                return overwrite;
            }

            return true;
        }


        private MPalette[] GetDirtyPalettes()
        {
            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;
            ArrayList dirtyPalettes = new ArrayList();

            foreach (MPalette palette in paletteManager.GetPalettes())
            {
                if (palette.Dirty)
                {
                    dirtyPalettes.Add(palette);
                }
            }
            return dirtyPalettes.ToArray(typeof(MPalette)) as MPalette[];
	    }

        private MPrefab[] GetDirtyPrefabs()
        {
            MPrefabManager prefabManager = MFramework.Instance.PrefabManager;
            ArrayList dirtyPrefabs = new ArrayList();
            foreach (MPrefab prefab in prefabManager.GetPrefabs())
            {
                if (prefab.Dirty &&
                    prefab.SourceFilename != 
                    MFramework.Instance.CurrentFilename)
                {
                    dirtyPrefabs.Add(prefab);
                }
            }
            return dirtyPrefabs.ToArray(typeof(MPrefab)) as MPrefab[];
        }

        private bool CheckPaletteFolderExists()
        {
            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;
            DirectoryInfo di = new DirectoryInfo(paletteManager.PaletteFolder);
            if (!di.Exists)
            {
                DialogResult result = 
                    MessageBox.Show("The Palette Folder:\n" + 
                    paletteManager.PaletteFolder + "\n"+
                    "does not exist. Would you like to create it?\n"
                    +"Note: if you chose not to create the folder, \n" +
                    "your palettes will not be saved.",
                    "Palette Folder Not Found", MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question);
                if (result == System.Windows.Forms.DialogResult.Yes)
                {
                    try
                    {
                        di.Create();
                    }
                    catch (IOException e)
                    {
                        MessageBox.Show("Unable to create palette folder:\n"
                            + paletteManager.PaletteFolder + "\n\nDetails:\n" +
                            e.Message + "\nPalettes will not be saved.",
                            "Palette Folder Creation Error",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return false;
                    }

                    return true;
                }
                else
                {
                    return false;
                }
            }
            return true;
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

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MainForm));
            this.m_ctlDockPanel = new WeifenLuo.WinFormsUI.DockPanel();
            this.m_menuMain = new System.Windows.Forms.MainMenu();
            this.m_timerComponent = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // m_ctlDockPanel
            // 
            this.m_ctlDockPanel.ActiveAutoHideContent = null;
            this.m_ctlDockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.m_ctlDockPanel.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
            this.m_ctlDockPanel.Location = new System.Drawing.Point(0, 0);
            this.m_ctlDockPanel.Name = "m_ctlDockPanel";
            this.m_ctlDockPanel.Size = new System.Drawing.Size(1272, 990);
            this.m_ctlDockPanel.TabIndex = 0;
            // 
            // m_timerComponent
            // 
            this.m_timerComponent.Enabled = true;
            this.m_timerComponent.Interval = 1000;
            this.m_timerComponent.Tick += new System.EventHandler(this.m_timerComponent_Tick);
            // 
            // MainForm
            // 
            this.AllowDrop = true;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(1272, 990);
            this.Controls.Add(this.m_ctlDockPanel);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.MaximumSize = new System.Drawing.Size(2048, 2048);
            this.Menu = this.m_menuMain;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Gamebryo Scene Designer";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.MainForm_Closing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnDragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnDragEnter);
            this.ResumeLayout(false);

        }
        #endregion

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args) 
        {
            m_restartLock = new Mutex(false, "SceneDesignerRestart");
            DateTime before = DateTime.Now;
            m_restartLock.WaitOne();
            DateTime after = DateTime.Now;
            Debug.WriteLine(
            "Waited " + (after - before).TotalMilliseconds.ToString()
                + "ms for startup mutext");
            m_restartLock.ReleaseMutex();           
            m_restartLock = null;

            //Now, allow only one instance.
            string mutexGuid = "{A8DE0E56-4130-4f92-A3D4-391D7A799F96}";
            // {A8DE0E56-4130-4f92-A3D4-391D7A799F96}

            m_instanceLock = new Mutex(false, mutexGuid);

            bool aquiredLock = m_instanceLock.WaitOne(new TimeSpan(0, 0, 1),
                true);

            if (!aquiredLock)
            {
                string message = string.Format(
                    "{0} is already running.\n" 
                    + "Only one instance is supported.",
                    Application.ProductName);
                    MessageBox.Show(message, 
                        string.Format("Cannot Launch {0}",
                        Application.ProductName), 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            MFramework.Init();

            Invalidator.Init();

            // Handle D3D10 command-line parameter.
            foreach (string strArg in args)
            {
                string strUpperArg = strArg.ToUpper();
                if (strUpperArg.IndexOf("-D3D10") > -1)
                {
                    MFramework.Instance.Renderer.UseD3D10();
                    break;
                }
            }

            try
            {
                using (new EnableThemingInScope(true))
                {
                    MainForm mainForm = new MainForm();

                    // Register idle handler.
                    Application.Idle += new EventHandler(mainForm.OnIdle);

                    Application.Run(mainForm);
                }
            }
            catch (PluginLoadException)
            {}
            finally
            {
                ServiceProvider.Instance.Shutdown();
                PluginManager.Shutdown();
                Invalidator.Shutdown();
                MFramework.Shutdown();
                ShutdownAssemblies();
            }

            m_instanceLock.ReleaseMutex();

            if (m_restartLock != null)
            {
                m_restartLock.ReleaseMutex();
            }
        }

        private static void ShutdownAssemblies()
        {
            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach (Assembly assembly in assemblies)
            {
                AssemblyUtilities.FindAndCallShutdownMethod(assembly);
            }
        }

        private void OnIdle(object sender, EventArgs e)
        {
            if (!m_bWindowPositionSet)
            {
                // This function must be called again here to ensure that the
                // window size matches the size stored in the settings. The
                // initialization of the third party docking code resizes the
                // window from the size set by the initial call to
                // LoadMainWindowPosition. The Boolean passed in here allows
                // the function to detect where it is being called from to
                // process special logic.
                LoadMainWindowPosition(true);
                m_bWindowPositionSet = true;
            }

            if (m_bProcessDelayedLoading)
            {
                m_bProcessDelayedLoading = false;
                DoDelayedStartupLoading();
            }
            if (this.Visible &&
                this.WindowState !=  FormWindowState.Minimized &&
                Form.ActiveForm == this)
            {
                Invalidator.Instance.Update();
            }
        }

        private string BuildTitleBarText(string fileName)
        {
            string title = string.Format("{0} - {1}", fileName, 
                MFramework.Instance.ApplicationName);
            if (MFramework.Instance.LayerManager.AreAnyLayersDirty())
            {
                title = title + "*";
            }
            return title;
        }

        private WeifenLuo.WinFormsUI.DockPanel m_ctlDockPanel;
        private System.Windows.Forms.MainMenu m_menuMain;

        private void MainForm_Load(object sender, System.EventArgs e)
        {
            ServiceProvider sp = ServiceProvider.Instance;

            IMessageService messageService = 
                sp.GetService(typeof(IMessageService)) as IMessageService;
            
            ISettingsService settingsService = 
                sp.GetService(typeof(ISettingsService))
                as ISettingsService;

            CommandPanelServiceImpl panelService =
                sp.GetService(typeof(ICommandPanelService))
                as CommandPanelServiceImpl;


            string dockConfig = settingsService.GetSettingsObject(
                dockConfigSettingName, SettingsCategory.PerUser) as string;

            bool panelLayoutLoaded = false;
            try
            {
                if (dockConfig != null)
                {
                    using (Stream s = new MemoryStream(
                               ASCIIEncoding.UTF8.GetBytes(dockConfig)))
                    {                        
                        panelService.LoadConfig(s);
                        panelLayoutLoaded = true;
                    }
                }
            }
            catch(Exception)
            {
                messageService.AddMessage(MessageChannelType.General, 
                    "Panel Layout not found or couldn't load.\n" +
                    "Default layout will be loaded");
                
            }
            if (!panelLayoutLoaded)
            {
                try
                {
                    using (Stream resourceStream = 
                               Assembly.GetExecutingAssembly()
                               .GetManifestResourceStream(
                               "Emergent.Gamebryo.SceneDesigner.GUI" +
                               ".DefaultDocking.config"))
                    {
                        panelService.LoadConfig(resourceStream);
                    }
                }    
                catch (InvalidOperationException)
                {
                    //Something was not right about the format...
                    messageService.AddMessage(MessageChannelType.General, 
                        "Panel Layout Failed to load.");

                }
            }

            // Associate .GSA file with SceneDesigner in Windows shell
            // If user started SceneDesigner with D3D10 option, include
            // that option in the shell association.
            string strOptions = MFramework.Instance.Renderer.D3D10 ?
                "-D3D10" : ""; 

            AssociateFileTypes.Associate(".gsa", 
                Application.ExecutablePath, strOptions, "GSA.Document",
                "Gamebryo Scene Ascii File", Application.StartupPath +
                @"\..\..\Data\GSADoc.ico");


            RenderForm renderForm = 
                panelService.GetPanel("Viewports") as RenderForm;

            Invalidator.Instance.RegisterControl(renderForm);
            Show();
            MakeNewFile(false);
            m_idleCommand = m_uiCommandService.GetCommand("Idle");
            m_timer = new TimedInvoker();
            m_timer.Interval = new TimeSpan(0, 0, 0, 0, 100);
            m_timer.SychronizedObject = this;
            m_timer.CallBack = new EventHandler(TimerCallBack);
            m_timer.Start();

            string strURL = Environment.GetEnvironmentVariable("EGB_PATH");
            if (strURL != null)
                strURL = strURL + @"\Documentation\HTML\Gamebryo.chm";

            string strBaseSceneDesignerHelpPath = 
                @"/Tool_Manuals/Scene_Designer/";
            HelpManager.Instance.Init(strURL, strBaseSceneDesignerHelpPath);

            MFramework.Instance.EventManager.LongOperationStarted +=
                new MEventManager.__Delegate_LongOperationStarted(
                this.OnLongOperationStarted);
            MFramework.Instance.EventManager.LongOperationCompleted +=
                new MEventManager.__Delegate_LongOperationCompleted(
                this.OnLongOperationCompleted);
        }

        private void DoDelayedStartupLoading()
	    {
            string[] args = Environment.GetCommandLineArgs();
            for (int i = 1; i < args.Length; i++)
            {
                string strArg = args[i];
                if (!strArg.StartsWith("-"))
                {
                    FileHandlerServiceImpl fileService = ServiceProvider
                        .Instance.GetService(typeof(IFileHandlerService))
                        as FileHandlerServiceImpl;
                    fileService.LoadFile(strArg);
                    break;
                }
            }
	    }
        
	    private void SceneFileLoaderCallBack(object sender, string strFilename)
	    {
            StringBuilder longPath = new StringBuilder(
                NativeWin32.MAX_PATH);
            NativeWin32.GetLongPathName(strFilename, longPath,
                longPath.Capacity);
            string longFileName = longPath.ToString();
            if (longFileName == null ||
                longFileName.Equals(string.Empty))
            {
                return;
            }

            FileInfo fi = new FileInfo(longFileName);
            if (fi.Extension != null &&
                !fi.Extension.Equals(string.Empty))
            {
                string extension = fi.Extension.Substring(1).ToUpper();
                LoadMainScene(longFileName, extension);
            }
	    }

        private void OnLongOperationStarted()
        {
            this.Cursor = Cursors.WaitCursor;
        }

        private void OnLongOperationCompleted()
        {
            this.Cursor = Cursors.Default;
        }

        private void OnDragEnter(object sender, 
            System.Windows.Forms.DragEventArgs e)
        {
            Debug.WriteLine(new StackTrace().GetFrame(0).ToString());
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false))
            {
                e.Effect = DragDropEffects.All;
            }
        }

        private void OnDragDrop(object sender, 
            System.Windows.Forms.DragEventArgs e)
        {
            string[] filenames = e.Data.GetData(DataFormats.FileDrop) 
                as string[];
            if (filenames.Length > 1)
            {
                MessageBox.Show("Only one Scene file may be opened at a time."
                    ,"Can not open multiple files", MessageBoxButtons.OK, 
                    MessageBoxIcon.Error);
            }
            else
            {
                FileInfo fi = new FileInfo(filenames[0]);
                string extension = fi.Extension.Substring(1).ToUpper();
                if (extension.ToUpper().CompareTo("GSA") == 0 &&
                    !CheckBeforeLosingChanges())
                {                
                    return;
                }

                FileHandlerServiceImpl fileService =
                    ServiceProvider.Instance.GetService(
                    typeof(IFileHandlerService))
                    as FileHandlerServiceImpl;

                fileService.LoadFile(filenames[0]);

                LoadMainScene(filenames[0], extension);
            }
        }


        [UICommandHandler("Exit")]
        private void m_miExit_Click(object sender, System.EventArgs e)
        {
            this.Close();
        }

        private void MainForm_Closing(object sender, 
            System.ComponentModel.CancelEventArgs e)
        {
            if (!CheckBeforeLosingChanges())
            {
                e.Cancel = true;
                return;
            }

            MFramework.Instance.EventManager.RaiseApplicationClosing();

            m_timer.Stop();

            MFramework.Instance.LayerManager.RemoveAllLayers();

            ServiceProvider sp = ServiceProvider.Instance;
            ISettingsService settingsService = 
                sp.GetService(typeof(ISettingsService))
                as ISettingsService;
            CommandPanelServiceImpl panelService =
                sp.GetService(typeof(ICommandPanelService))
                as CommandPanelServiceImpl;

            if (!m_bClearWindowLayout)
            {
                using (MemoryStream ms = new MemoryStream())
                {
                    panelService.SaveConfig(ms);
                    Encoding.UTF8.GetString(ms.GetBuffer());
                    string dockConfig = 
                        Encoding.UTF8.GetString(ms.GetBuffer());
                    settingsService.SetSettingsObject(dockConfigSettingName, 
                        dockConfig, SettingsCategory.PerUser);
                }
            }
            else
            {
                settingsService.RemoveSettingsObject(dockConfigSettingName,
                    SettingsCategory.PerUser);
            }

            SaveMainWindowPosition();

            settingsService.SaveSettings(SettingsCategory.Global);
            settingsService.SaveSettings(SettingsCategory.PerUser);
        }


        /// <summary>
        /// Makes sure the user has saved the scene if it has changed
        /// </summary>
        /// <returns>return value of false means the caller should cancle
        /// whatever it was about to do.</returns>
        private bool CheckBeforeLosingChanges()
        {
            if (!MFramework.Instance.LayerManager.AreAnyLayersDirty())
            {
                SavePalettes(true, false);
                return true;
            }
            DialogResult result = MessageBox.Show(
                "The current file, or one of its layers, has been modified." +
                "\nWould you like to save before continuing?",
                "Save Changed File?",
                MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
            switch (result)
            {
                case System.Windows.Forms.DialogResult.Yes:
                {
                    SaveFile(true);
                    break;
                }
                case System.Windows.Forms.DialogResult.No:
                {
                    // removing the layers will cleanup the autosave files
                    SavePalettes(true, false);
                    return true;
                }
                case System.Windows.Forms.DialogResult.Cancel:
                {
                    return false;
                }
            }
            return true;
        }

        #region UICommand Hanlders
        [UICommandHandler("SaveFile")]
        private void OnSave(object sender, EventArgs args)
        {
            SaveFile(false);
        }

        [UICommandHandler("SaveFileAs")]
        private void OnSaveFileAs(object sender, EventArgs args)
        {
            SaveFileAs();
        }

        private void SaveFile(bool promptForPalettes)
        {
            bool savedFile;
            if (MFramework.Instance.CurrentFilename.Equals(string.Empty))
            {
                savedFile = SaveFileAs();
            }
            else
            {
                FileInfo fi = new FileInfo(
                    MFramework.Instance.CurrentFilename);
                savedFile = SaveMainScene(MFramework.Instance.CurrentFilename,
                    fi.Extension.Replace(".", "").ToUpper(), false);
            }
            if (!savedFile && promptForPalettes)
            {
                SavePalettes(true, false);
            }
        }

        private bool SaveFileAs()
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.OverwritePrompt = false;

            string[] streamingDescriptions = 
                MUtility.GetStreamingFormatDescriptions();
            string[] streamingExtensions =
                MUtility.GetStreamingFormatExtensions();

            int iFormats = streamingDescriptions.Length;
            Debug.Assert(iFormats > 0);
            dlg.Filter = streamingDescriptions[0];
            for (int i=1; i<iFormats; i++)
            {
                dlg.Filter += "|" + streamingDescriptions[i];
            }
            dlg.FileName = MFramework.Instance.CurrentFilename;
            while (true)
            {
                if (dlg.ShowDialog() ==
                    System.Windows.Forms.DialogResult.Cancel)
                {
                    return false;
                }

                // Ensure that the chosen filename has the appropriate
                // extension.
                string strExtension = string.Empty;
                int iExtensionIndex = dlg.FileName.LastIndexOf('.');
                if (iExtensionIndex > -1)
                {
                    strExtension = dlg.FileName.Substring(iExtensionIndex + 1)
                        .ToUpper();
                }
                string strChosenExtension = streamingExtensions[
                    dlg.FilterIndex - 1];
                if (!strExtension.Equals(strChosenExtension))
                {
                    strExtension = strChosenExtension;
                    dlg.FileName += "." + strExtension.ToLower();
                }

                FileInfo info = new FileInfo(dlg.FileName);
                string extension = info.Extension;
                if (info.Exists &&
                    dlg.FileName != MFramework.Instance.CurrentFilename)
                {
                    string strMessage = null;
                    if (MFramework.Instance.PrefabManager.
                        GetPrefabByFilename(dlg.FileName) != null)
                    {

                        strMessage = "The following filename already " +
                            "identifies a prefab.";
                    }
                    else if (MFramework.Instance.LayerManager.
                        FindLayerByName(dlg.FileName) != null)
                    {
                        strMessage = "The scene you are saving " +
                            "contains a layer with the specified " +
                            "filename.";
                    }

                    if (strMessage != null)
                    {
                        strMessage += "\nPlease choose a different name.\n\n" +
                            dlg.FileName;
                        MessageBox.Show(strMessage,
                            "Save Failed",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Error);
                        continue;
                    }

                    strMessage = "File " + dlg.FileName + " already exists." +
                        "\nDo you want to replace it?";

                    if (MessageBox.Show(strMessage,
                        "Save As",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Exclamation) == DialogResult.No)
                    {
                        continue;
                    }
                }
                if (info.Exists)
                {
                    info.IsReadOnly = false;
                    info.Delete();
                }
                info = new FileInfo(dlg.FileName + ".scene.settings");
                if (info.Exists)
                {
                    info.IsReadOnly = false;
                    info.Delete();
                }

                return SaveMainScene(dlg.FileName, strExtension, false);
            }
        }

        private bool CheckForReadOnlyFiles(string mainFilename)
        {
            List<String> readOnlyFilenames = new List<String>();

            MLayer defaultLayer = MFramework.Instance.LayerManager
                .DefaultLayer;
            if (mainFilename.Equals(defaultLayer.Name))
            {
                if (defaultLayer.Dirty && !defaultLayer.Writable)
                {
                    readOnlyFilenames.Add(mainFilename);
                }
            }
            else
            {
                FileInfo fi = new FileInfo(mainFilename);
                if (fi.Exists && fi.IsReadOnly)
                {
                    readOnlyFilenames.Add(mainFilename);
                }
            }

            MLayer[] layers = MFramework.Instance.LayerManager.GetLayers();
            foreach (MLayer layer in layers)
            {
                if (layer != defaultLayer && layer.Dirty && !layer.Writable)
                {
                    readOnlyFilenames.Add(layer.Name);
                }
            }

            MPalette[] palettes = MFramework.Instance.PaletteManager
                .GetPalettes();
            foreach (MPalette palette in palettes)
            {
                if (palette.Dirty && !palette.Writable)
                {
                    readOnlyFilenames.Add(palette.Scene.SourceFilename);
                }
            }

            MPrefab[] prefabs = MFramework.Instance.PrefabManager.
                GetPrefabs();
            foreach (MPrefab prefab in prefabs)
            {
                if (prefab.Dirty && !prefab.Writable &&
                    defaultLayer.Name != prefab.SourceFilename)
                {
                    readOnlyFilenames.Add(prefab.SourceFilename);
                }
            }

            if (readOnlyFilenames.Count > 0)
            {
                string message = "The following files are read-only and " +
                    "cannot be saved:\n\n";
                foreach (string filename in readOnlyFilenames)
                {
                    message += string.Format("{0}\n", filename);
                }
                message += "\nDo you want to make them writable? Selecting " +
                    "'No' will cancel the save operation.";

                DialogResult result = MessageBox.Show(message,
                    "Make Files Writable?", MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    foreach (string filename in readOnlyFilenames)
                    {
                        FileInfo fi = new FileInfo(filename);
                        if (fi.Exists)
                        {
                            fi.IsReadOnly = false;
                        }
                    }
                    Application.DoEvents();

                    return true;
                }
                return false;
            }

            return true;
        }

	    private bool SaveMainScene(string fileName, string extension,
            bool bAutosave)
	    {
	        MFramework fw = MFramework.Instance;
            if (!bAutosave && !CheckForReadOnlyFiles(fileName))
            {
                return false;
            }

            bool fileSaved = fw.SaveScene(fileName, extension, bAutosave);
	        if (!fileSaved)
	        {
	            MessageBox.Show("Failed to save scene.", "File Save Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
	        }
	        else
	        {
                bool bSaveSettings = true;
                if (bAutosave)
                {
                    // Don't autosave settings if the main GSA file was not
                    // autosaved.
                    FileInfo fileAutoSave = new FileInfo(fileName +
                        ".autosave");
                    if (!fileAutoSave.Exists)
                    {
                        bSaveSettings = false;
                    }
                }

                if (bSaveSettings)
                {
                    FileInfo fi = new FileInfo(fileName);
                    SceneManagementUtilities.PutSceneItemsIntoSettings(
                        fw.Scene);

                    ServiceProvider sp = ServiceProvider.Instance;
                    ISettingsService settingsService =
                        sp.GetService(typeof(ISettingsService))
                            as ISettingsService;
                    settingsService.ScenePath = fi.DirectoryName;
                    settingsService.SceneFileName = fi.Name +
                        ".scene.settings";
                    string strOriginalSettingsFilename = settingsService
                        .SceneFileName;
                    if (bAutosave)
                    {
                        settingsService.SceneFileName += ".autosave";
                    }
                    SceneManagementUtilities.ConvertScenePalettePathToRelative(
                        fi.DirectoryName);
                    settingsService.SaveSettings(SettingsCategory.PerScene);
                    SceneManagementUtilities.ConvertScenePalettePathToAbsolute(
                        fi.DirectoryName);
                    if (bAutosave)
                    {
                        settingsService.SceneFileName =
                            strOriginalSettingsFilename;
                    }
                    else
                    {
                        // Upon successful save, delete any existing autosave
                        // file.
                        FileInfo file = new FileInfo(fi.FullName +
                            ".scene.settings.autosave");
                        if (file.Exists)
                        {
                            file.IsReadOnly = false;
                            file.Delete();
                        }

                        // Also delete any existing backup file.
                        file = new FileInfo(fi.FullName +
                            ".scene.settings.backup");
                        if (file.Exists)
                        {
                            file.IsReadOnly = false;
                            file.Delete();
                        }

                        SavePalettes(false, true);
                        this.Text = BuildTitleBarText(fi.Name);
                        MRUManager.AddFile(fileName);
                    }
                }
                fileSaved = bSaveSettings;
            }

            return fileSaved;
	    }

        [UICommandHandler("MRULoad")]
        [RequiresParameter("Filename")]
        private void OnMRULoad(object sender, EventArgs args)
        {
            UICommandEventArgs commandArgs = args as UICommandEventArgs;
            if (!CheckBeforeLosingChanges())
            {                
                return;
            }
            UICommand mruSender = sender as UICommand;
            string fileName = commandArgs.GetValue("Filename");
            FileInfo fi = new FileInfo(fileName);
            string extension = fi.Extension.Substring(1).ToUpper();
            LoadMainScene(fileName, extension);
        }

        [UICommandHandler("OpenFile")]
        private void OnOpenFile(object sender, EventArgs args)
        {
            if (!CheckBeforeLosingChanges())
            {                
                return;
            }
            OpenFileDialog dlg = new OpenFileDialog();

            string[] streamingDescriptions = 
                MUtility.GetStreamingFormatDescriptions();
            string[] streamingExtensions =
                MUtility.GetStreamingFormatExtensions();

            int iFormats = streamingDescriptions.Length;
            Debug.Assert(iFormats > 0);
            dlg.Filter = streamingDescriptions[0];
            for (int i=1; i<iFormats; i++)
            {
                dlg.Filter = dlg.Filter + "|" + streamingDescriptions[i];
            }
            dlg.InitialDirectory = MFramework.Instance.CurrentFilename;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                IMessageService messageService = 
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService)) as IMessageService;
                messageService.ClearMessages(MessageChannelType.Conflicts);
                string sceneFilename = dlg.FileName;
                string extension = streamingExtensions[dlg.FilterIndex-1];
                LoadMainScene(sceneFilename, extension);
            }
        }

        [UICommandHandler("ImportLayer")]
        private void OnImportLayer(object sender, EventArgs args)
        {
            OpenFileDialog dlg = new OpenFileDialog();

            string[] streamingDescriptions =
                MUtility.GetStreamingFormatDescriptions();
            string[] streamingExtensions =
                MUtility.GetStreamingFormatExtensions();

            int iFormats = streamingDescriptions.Length;
            Debug.Assert(iFormats > 0);
            dlg.Filter = streamingDescriptions[0];
            for (int i = 1; i < iFormats; i++)
            {
                dlg.Filter = dlg.Filter + "|" + streamingDescriptions[i];
            }
            dlg.InitialDirectory = MFramework.Instance.CurrentFilename;
            dlg.Multiselect = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                MPrefabManager pm = MFramework.Instance.PrefabManager;
                String conflicts = "";
                foreach (string sceneFilename in dlg.FileNames)
                {
                    if (pm.GetPrefabByFilename(sceneFilename) == null)
                    {
                        string extension =
                            streamingExtensions[dlg.FilterIndex - 1];
                        MFramework.Instance.LayerManager.
                            ImportLayer(sceneFilename, extension);

                        using (new MLayerWritableOverrideLock())
                        {
                            ResolvePaletteConflicts();
                        }
                    }
                    else
                    {
                        conflicts = String.Concat(conflicts, sceneFilename);
                    }
                }
                if (conflicts.Length > 0)
                {
                    String msg = "The following files were already in use " +
                        "as prefabs and could not be loaded as layers:\n\n" +
                        conflicts;
                    MessageBox.Show(msg, "Error Loading Scene Designer File",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

	    private void LoadMainScene(string sceneFilename, string extension)
        {
            m_timerComponent.Enabled = false;

            List<MScene> pmScenes = MFramework.Instance.LoadScene(
                sceneFilename, extension);
            if (pmScenes == null)
	        {
	            MessageBox.Show("Failed to load the following file:\n\n" +
                    sceneFilename + "\n\nCheck the Messages Panel for " +
                    "more information.", "Error Loading Scene Designer File",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
	        }
	        else
	        {
                LoadSettingsFile(sceneFilename);

                Debug.Assert(pmScenes.Count > 0);
                Application.DoEvents();
                MFramework.Instance.InitNewScene(pmScenes);

                // Backup dirty flags for all layers.
                MLayer[] layers = MFramework.Instance.LayerManager.GetLayers();
                bool[] dirtyFlags = new bool[layers.Length];
                for (int i = 0; i < layers.Length; ++i)
                {
                    dirtyFlags[i] = layers[i].Dirty;
                }

                // Backup active layer.
                MLayer activeLayer = MFramework.Instance.LayerManager
                    .ActiveLayer;

                // Use an MLayerWritableOverrideLock object here to allow
                // edits to read-only files.
                FileInfo fi = new FileInfo(sceneFilename);
                using (new MLayerWritableOverrideLock())
                {
                    SceneManagementUtilities.PutSettingsIntoSceneItems(
                        MFramework.Instance.Scene);

                    SceneManagementUtilities.ConvertScenePalettePathToAbsolute(
                        fi.DirectoryName);
                    SceneManagementUtilities.CopySceneSettingsToUserSettings();

                    bool bConflictsFound = ResolvePaletteConflicts();
                    for (int i = 0; i < layers.Length; ++i)
                    {
                        layers[i].Dirty = bConflictsFound || dirtyFlags[i];
                    }
                }

                // Update title bar text.
                this.Text = BuildTitleBarText(fi.Name);
                MRUManager.AddFile(
                    MFramework.Instance.CurrentFilename);

                // Restore active layer.
                if (MFramework.Instance.LayerManager.ActiveLayer !=
                    activeLayer)
                {
                    MFramework.Instance.LayerManager.SetActiveLayer(
                        activeLayer, false);
                }
	        }

            m_timerComponent.Enabled = true;
	    }

        private bool ResolvePaletteConflicts()
        {
            SceneManagementUtilities.CheckForConflicts();

            MPalette oldOrphan = MFramework.Instance.PaletteManager
                .GetPaletteByName("Unsaved Scene Templates");
            if (oldOrphan != null)
            {
                MFramework.Instance.PaletteManager.RemovePalette(oldOrphan);
            }
            bool bConflictsFound =
                SceneManagementUtilities.CheckMainSceneForConflicts();

            ComponentManagementUtilities.BuildComponentListFromScene(
                MFramework.Instance.Scene);
            MPalette orphans =
                EntityManagementUtilities.ResolveMasterEntities(
                    MFramework.Instance.Scene,
                    MFramework.Instance.PaletteManager.GetPalettes());
            orphans.Name = "Unsaved Scene Templates";
            orphans.DontSave = true;
            bool bAddedToManager =
                SceneManagementUtilities.AddOrphansToPalettes(orphans);
            DirtyBitUtilities.MakeSceneClean(orphans.Scene);
            orphans.Dispose();

            return bConflictsFound;
        }

        private void LoadSettingsFile(string sceneFilename)
        {
            // Check for a backup file for the main scene. If one exists
            // load the autosave settings file instead of the main settings
            // file.
            FileInfo fi = new FileInfo(sceneFilename + ".backup");
            if (fi.Exists)
            {
                fi = new FileInfo(sceneFilename +
                    ".scene.settings.autosave");
                if (fi.Exists)
                {
                    FileInfo fiBackup = new FileInfo(sceneFilename +
                        ".scene.settings");
                    if (fiBackup.Exists)
                    {
                        fiBackup.IsReadOnly = false;
                        fiBackup.CopyTo(sceneFilename +
                            ".scene.settings.backup", true);
                    }

                    fi.IsReadOnly = false;
                    fi.CopyTo(sceneFilename + ".scene.settings", true);
                }
            }

            ISettingsService settingsService = ServiceProvider.Instance
                .GetService(typeof(ISettingsService)) as ISettingsService;
            fi = new FileInfo(sceneFilename);
            settingsService.ScenePath = fi.DirectoryName;
            settingsService.SceneFileName = fi.Name + ".scene.settings";
            SceneManagementUtilities.ClearSceneSettings();
            settingsService.LoadSettings(SettingsCategory.PerScene);
        }

        [UICommandHandler("NewFile")]
        private void OnNewFile(object sender, EventArgs args)
        {
            if (!CheckBeforeLosingChanges())
            {                
                return;
            }
            IMessageService messageService = 
                ServiceProvider.Instance.GetService(
                typeof(IMessageService)) as IMessageService;
            messageService.ClearMessages(MessageChannelType.Conflicts);
            MakeNewFile(true);
        }

        private void MakeNewFile(bool bAskForPalettePath)
        {
            m_timerComponent.Enabled = false;

            List<MScene> pmScenes = MFramework.Instance.NewScene(0);
            SceneManagementUtilities.ClearSceneSettings();
            Application.DoEvents();
            MFramework.Instance.InitNewScene(pmScenes);
            string newFilePalettePath = null;
            if (bAskForPalettePath)
            {
                newFilePalettePath = AskUserForPalettePath();
            }
            ISettingsService settingService =
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;
            settingService.ScenePath = string.Empty;
            if (newFilePalettePath != null)
            {
                settingService.SetSettingsObject("PaletteFolder", 
                    new MFolderLocation(newFilePalettePath),
                    SettingsCategory.PerUser);
            }
            else
            {
                SceneManagementUtilities.ResetPalettes();
            }
            SceneManagementUtilities.CheckForConflicts();
            ISelectionService pmSelectionService =
                ServiceProvider.Instance.GetService(
                typeof(ISelectionService)) as ISelectionService;
            pmSelectionService.ClearSelectedEntities();

            DirtyBitUtilities.MakeSceneClean(MFramework.Instance.Scene);
            this.Text = BuildTitleBarText("Untitled");

            m_timerComponent.Enabled = true;
        }

        private string AskUserForPalettePath()
        {
            // Retrieve setting for whether or not to prompt the user for
            // a palette path.
            ISettingsService pmSettingsService = ServiceProvider.Instance
                .GetService(typeof(ISettingsService)) as ISettingsService;
            Debug.Assert(pmSettingsService != null,
                "Settings service not found!");
            object pmDisplayNewSceneFormObject = pmSettingsService
                .GetSettingsObject(ms_strDisplayNewSceneFormSettingName,
                SettingsCategory.PerUser);
            Debug.Assert(pmDisplayNewSceneFormObject != null, "\"" +
                ms_strDisplayNewSceneFormSettingName + "\" setting not " +
                "found!");
            bool bDisplayNewSceneForm = (bool) pmDisplayNewSceneFormObject;

            string strPalettePath = null;
            if (bDisplayNewSceneForm)
            {
                NewSceneForm pmNewSceneForm = new NewSceneForm();
                pmNewSceneForm.ShowDialog();
                strPalettePath = pmNewSceneForm.PalettePath;
            }

            return strPalettePath;
        }

        [UICommandHandler("SaveDefaultConfig")]
        private void SaveConfigToFile(object sender, EventArgs args)
        {
            using (FileStream fs = new FileStream("DefaultDocking.config",
                       FileMode.Create))
            {
                CommandPanelServiceImpl panelService =
                    ServiceProvider.Instance.
                    GetService(typeof(ICommandPanelService))
                    as CommandPanelServiceImpl;
                panelService.SaveConfig(fs);
            }
        }

        [UICommandHandler("ResetWindowLayout")]
        private void OnResetWindowLayout(object sender, EventArgs args)
        {
            DialogResult result = MessageBox.Show(
                "Would you like to reset the window layout?\n" +
                string.Format(
                "The default layout will be restored next time {0} "+
                "is launched.", MFramework.Instance.ApplicationName),
                "Reset Window Layout?", MessageBoxButtons.YesNo,
                MessageBoxIcon.Question);
            m_bClearWindowLayout = (result == DialogResult.Yes);
        }

        [UICommandHandler("Options")]
        private void OnOptions(object sender, EventArgs args)
        {
            
            OptionsDlg dlg = new OptionsDlg();
            dlg.SetCategories( new SettingsCategory[]
                {
                        SettingsCategory.Global, SettingsCategory.PerUser, 
                    SettingsCategory.PerScene, SettingsCategory.Temp
                });
            dlg.Text = "Options";
            dlg.ShowDialog();
        }

        [UICommandHandler("ClearMRUList")]
        private void OnClearMRUList(object sender, EventArgs args)
        {
            MRUManager.Clear();
        }

        [UICommandHandler("Restart")]
        private void OnRestart(object sender, EventArgs args)
        {
            string path = Assembly.GetEntryAssembly().Location;
            string command = path;
            Mutex mutex = new Mutex(true, "SceneDesignerRestart");
            mutex.WaitOne();
            ProcessStartInfo psi = new ProcessStartInfo(command);
            psi.WorkingDirectory = Path.GetDirectoryName(command);
            System.Diagnostics.Process.Start(psi);
            m_restartLock = mutex;
            this.Close();
        }

        [UICommandValidator("ChangePaletteFolder")]
        private void OnValidateChangePaletteFolder(object sender, UIState state)
        {
            MLayer[] layerArray = MFramework.Instance.LayerManager.GetLayers();
            foreach(MLayer layer in layerArray)
            {
                if (!layer.Writable)
                {
                    state.Enabled = false;
                    return;
                }
            }

            state.Enabled = true;
        }

        [UICommandHandler("ChangePaletteFolder")]
        private void OnChanagePaletteFolder(object sender, EventArgs args)
        {
            FolderBrowserDialog browseDlg = new FolderBrowserDialog();
            browseDlg.SelectedPath = SceneManagementUtilities.CalculatePaletteFolder();
            browseDlg.Description = "Select New Palette Folder";
            if (browseDlg.ShowDialog() == DialogResult.OK)
            {
                ISettingsService settingService =
                    ServiceProvider.Instance.
                    GetService(typeof(ISettingsService))
                    as ISettingsService;
                object oldSetting = settingService.GetSettingsObject(
                    "PaletteFolder",
                    SettingsCategory.PerUser);
                if (!MFramework.Instance.LayerManager.AreAnyLayersDirty() &&
                    MFramework.Instance.CurrentFilename.Equals(string.Empty))
                {
                    settingService.SetSettingsObject("PaletteFolder",
                        new MFolderLocation(browseDlg.SelectedPath),
                        SettingsCategory.PerUser);
                    MakeNewFile(false);
                    return;
                }
                string msg = "Changing the palette folder will " +
                    "automatically save the scene.  Continue?";
                if (MessageBox.Show(msg,
                    "Continue?",
                    MessageBoxButtons.OKCancel,
                    MessageBoxIcon.Question) == DialogResult.OK)
                {
                    settingService.SetSettingsObject("PaletteFolder",
                        new MFolderLocation(browseDlg.SelectedPath),
                        SettingsCategory.PerUser);
                    bool saved;
                    if (MFramework.Instance.
                        CurrentFilename.Equals(string.Empty))
                    {
                        saved = SaveFileAs();
                    }
                    else
                    {
                        FileInfo fi = new FileInfo(
                            MFramework.Instance.CurrentFilename);
                        saved = SaveMainScene(
                            MFramework.Instance.CurrentFilename,
                            fi.Extension.Replace(".", "").ToUpper(), false);
                    }

                    if (saved)
                    {
                        // Reload scene
                        FileInfo fi = new FileInfo(
                            MFramework.Instance.CurrentFilename);
                        LoadMainScene(MFramework.Instance.CurrentFilename, 
                            fi.Extension.Replace(".", "").ToUpper());
                    }
                    else
                    {
                        settingService.SetSettingsObject("PaletteFolder",
                            oldSetting,
                            SettingsCategory.PerUser);
                        string message = "The scene was not saved." +
                            "  Palette folder location is unchanged.";
                        MessageBox.Show(message,
                            "Palette Folder Location Unchanged",
                            MessageBoxButtons.OK,
                            MessageBoxIcon.Exclamation);
                    }
                }
            }
        }
        #endregion


    
        protected override bool ProcessCmdKey(
            ref System.Windows.Forms.Message msg, Keys keyData)
        {
            if (!base.ProcessCmdKey (ref msg, keyData))
            {
                return ShortcutService.HandleKey(keyData);
            }
            else
            {
                return true;
            }
        }

        private void m_timerComponent_Tick(object sender, System.EventArgs e)
        {
            //Very lazy execution, since we dont really need this to update
            //we lightning speed, a 1000 ms timer should suffice
            bool bMainSceneDirty = MFramework.Instance.LayerManager
                .AreAnyLayersDirty();
            CheckAutosaveInterval();
            if (bMainSceneDirty != m_lastDirtyStatus)
            {                                
                if (bMainSceneDirty)
                {
                    if (this.Text.IndexOf("*") == -1)
                    {
                        this.Text = this.Text + "*";
                    }
                }
                else
                {
                    this.Text = this.Text.Replace("*", "");
                }
                m_lastDirtyStatus = bMainSceneDirty;
            }
        }

        #region Autosaving Methods

	    private void CheckAutosaveInterval()
	    {
            ISettingsService settingsSvc = 
                ServiceProvider.Instance.GetService(
                typeof(ISettingsService)) as ISettingsService;
            int intervalMinutes = (int) 
                settingsSvc.GetSettingsObject("AutoSaveIntervalInMinutes",
                SettingsCategory.PerUser);
            if (intervalMinutes <= 0)
                return;
            TimeSpan interval = new TimeSpan(0, intervalMinutes, 0);

            if ((DateTime.Now - m_timeOfLastAutoSave) > interval)
            {
                m_timerComponent.Enabled = false;

                DoAutosave();
                m_timeOfLastAutoSave = DateTime.Now;

                m_timerComponent.Enabled = true;
            }
	    }

        private void DoAutosave()
        {
            string currentFilename = MFramework.Instance.CurrentFilename;
            if (!string.IsNullOrEmpty(currentFilename))
            {
                FileInfo fi = new FileInfo(currentFilename);
                SaveMainScene(currentFilename,
                    fi.Extension.Replace(".", "").ToUpper(), true);
            }
            SavePalettes(false, false);
        }
        #endregion


    }
}
