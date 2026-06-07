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
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for SceneManagementUtilities.
    /// </summary>
    public class SceneManagementUtilities
    {
        #region Private Data
        //private static string m_previo
        #endregion

        static SceneManagementUtilities()
        {
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;
            MFolderLocation loc = new MFolderLocation("");

            //            settingsService.RemoveSettingsObject("PaletteFolder",
            //                 SettingsCategory.PerUser);

            settingsService.RegisterSettingsObject("PaletteFolder",
                loc, SettingsCategory.PerUser);

            settingsService.SetChangedSettingHandler("PaletteFolder",
                SettingsCategory.PerUser, 
                new SettingChangedHandler(OnUserPaletteFolderChanged));
        }

        private SceneManagementUtilities()
        {
        }

        internal static bool CheckMainSceneForConflicts()
        {
            MFramework fw = MFramework.Instance;
            return ConflictManagementUtilities.CheckForConflicts(fw.Scene,
                fw.PaletteManager.GetPalettes());
        }

        public static void PutSceneItemsIntoSettings(MScene scene)
        {
            ArrayList frozenIDList = new ArrayList();

            MEntity[] entities = scene.GetEntities();
            foreach (MEntity entity in entities)
            {
                if (entity.LocalFrozen)
                {
                    frozenIDList.Add(entity.ID);
                }
            }
            
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            settingsService.SetSettingsObject("FrozenEntities", frozenIDList,
                SettingsCategory.PerScene);

            PutViewportItemsIntoSettings(settingsService);
        }

        private static void PutViewportItemsIntoSettings(
            ISettingsService settingService)
        {
            MFramework fw = MFramework.Instance;
            //ViewPorts 
            bool bExclusive = fw.ViewportManager.ExclusiveViewport != null;
            ArrayList viewPorts = new ArrayList();
            uint viewPortIndex = 0;
            for (uint i = 0; i < fw.ViewportManager.ViewportCount; i++)
            {
                MViewport viewport = fw.ViewportManager.GetViewport(i);
                string viewportPropertyPrefix = string.Format("Viewport{0}_",
                    i);
                ArrayList standardCameras = new ArrayList();
                foreach (MCameraManager.StandardCamera standardCameraType in 
                    Enum.GetValues(typeof(MCameraManager.StandardCamera)))
                {
                    MEntity camera = 
                        fw.CameraManager.GetStandardCameraEntity(viewport,
                        standardCameraType);
                    standardCameras.Add( camera);
                    //Now, save the position and orientation for each 
                    //standard camera
                    string cameraPrefix = "StandardCam:" + 
                        standardCameraType.ToString();

                    MPoint3 translation = 
                        camera.GetPropertyData("Translation") as MPoint3;
                    settingService.SetSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + ":Translation",
                        translation, SettingsCategory.PerScene);

                    MMatrix3 rotation = (MMatrix3) 
                        camera.GetPropertyData("Rotation");
                    settingService.SetSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + ":Rotation", 
                        rotation,
                        SettingsCategory.PerScene);

                    float frustumWidth = (float) camera.GetPropertyData(
                        "Orthographic Frustum Width");
                    settingService.SetSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + 
                        ":FrustumWidth", frustumWidth, 
                        SettingsCategory.PerScene);
                    
                }
                //int sceneCameras = fw.CameraManager.SceneCameraCount
                MEntity activeCamera = viewport.CameraEntity;
                bool activeCameraIsStandard = false;
                if (standardCameras.Contains(activeCamera))
                {
                    activeCameraIsStandard = true;
                }
                settingService.SetSettingsObject(viewportPropertyPrefix + 
                    ":ActiveCameraIsStandard", activeCameraIsStandard,
                    SettingsCategory.PerScene);
                settingService.SetSettingsObject(viewportPropertyPrefix + 
                    ":ActiveCamera", activeCamera.Name,
                    SettingsCategory.PerScene);
                if (viewport == fw.ViewportManager.ActiveViewport)
                {
                    viewPortIndex = i;
                }

                // Save out the name of the rendering mode for the viewport.
                string strRenderingModeName = string.Empty;
                if (viewport.RenderingMode != null)
                {
                    strRenderingModeName = viewport.RenderingMode.Name;
                }
                settingService.SetSettingsObject(viewportPropertyPrefix +
                    ":RenderingMode", strRenderingModeName,
                    SettingsCategory.PerScene);
            }
            settingService.SetSettingsObject("ExclusiveViewPort", bExclusive,
                SettingsCategory.PerScene);
    
            settingService.SetSettingsObject("ActiveViewPort", viewPortIndex,
                SettingsCategory.PerScene);
        }

        public static void PutSettingsIntoSceneItems(MScene scene)
        {
            MFramework fw = MFramework.Instance;
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            ArrayList frozenEntityIDs = settingsService.GetSettingsObject(
                "FrozenEntities", SettingsCategory.PerScene) as ArrayList;
            if (frozenEntityIDs != null)
            {
                try
                {
                    foreach (Guid id in frozenEntityIDs)
                    {
                        MEntity entity = scene.GetEntityByID(id);
                        if (entity != null)
                        {
                            entity.SetFrozen(true, false);
                        }
                    }
                }
                catch (InvalidCastException)
                {
                    // The setting used to be stored as an array of strings.
                    foreach (string entityName in frozenEntityIDs)
                    {
                        MEntity entity = scene.GetEntityByName(entityName);
                        if (entity != null)
                        {
                            entity.SetFrozen(true, false);
                        }
                    }
                }
            }

            PutSettingsIntoViewportItems(settingsService, scene);
        }

        public static void ClearSceneSettings()
        {
            ClearViewportSettings();
            ClearLocalPalettePath();
            ClearLayersSettings();
        }

        private static void ClearLayersSettings()
        {
            // Clear the layers settings
            ISettingsService settingService =
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            settingService.RemoveSettingsObject("LayerGroups",
                SettingsCategory.PerScene);
        }

        private static void ClearLocalPalettePath()
        {
            MFramework fw = MFramework.Instance;
            ISettingsService settingService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            settingService.RemoveSettingsObject("PaletteFolder",
                SettingsCategory.PerScene);
            object peruserSetting = 
                settingService.GetSettingsObject("PaletteFolder",
                SettingsCategory.PerUser);
            settingService.RegisterSettingsObject("PaletteFolder", 
                peruserSetting, SettingsCategory.PerScene);
        }

        private static void PutSettingsIntoViewportItems(
            ISettingsService settingService, MScene scene)
        {
            MFramework fw = MFramework.Instance;
            //ViewPort\camera stuff
            for (uint i = 0; i < fw.ViewportManager.ViewportCount; i++)
            {
                MViewport viewport = fw.ViewportManager.GetViewport(i);
                string viewportPropertyPrefix = string.Format("Viewport{0}_",
                    i);
                ArrayList standardCameras = new ArrayList();
                foreach (MCameraManager.StandardCamera standardCameraType in 
                    Enum.GetValues(typeof(MCameraManager.StandardCamera)))
                {
                    MEntity camera = 
                        fw.CameraManager.GetStandardCameraEntity(viewport,
                        standardCameraType);
                    standardCameras.Add( camera);
                    //Now, get the position and orientation for each 
                    //standard camera
                    string cameraPrefix = "StandardCam:" + 
                        standardCameraType.ToString();

                    MPoint3 translation = settingService.GetSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + ":Translation",
                        SettingsCategory.PerScene) as MPoint3;
                    if (translation != null)
                    {
                        camera.SetPropertyData(
                            "Translation", translation, false);
                    }

                    MMatrix3 rotation = settingService.GetSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + ":Rotation",
                        SettingsCategory.PerScene) as MMatrix3;
                    if (rotation != null)
                    {
                        camera.SetPropertyData("Rotation", rotation, false);
                    }

                    object oFrustumWidth = settingService.GetSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + 
                        ":FrustumWidth", SettingsCategory.PerScene);
                    if (oFrustumWidth != null)
                    {
                        camera.SetPropertyData("Orthographic Frustum Width",
                            (float)oFrustumWidth, false);
                    }

                }

                bool activeCameraIsStandard = true;

                object oActiveCameraIsStandard = 
                    settingService.GetSettingsObject(
                    viewportPropertyPrefix + ":ActiveCameraIsStandard", 
                    SettingsCategory.PerScene);

                if (oActiveCameraIsStandard != null)
                {
                    activeCameraIsStandard = (bool) oActiveCameraIsStandard;
                }

                string activeCameraName =
                    settingService.GetSettingsObject(
                    viewportPropertyPrefix + ":ActiveCamera", 
                    SettingsCategory.PerScene) as string;
                if (activeCameraName != null)
                {
                    if (activeCameraIsStandard)
                    {
                        MCameraManager.StandardCamera cameraType = 
                            (MCameraManager.StandardCamera) 
                            Enum.Parse(typeof(MCameraManager.StandardCamera),
                            activeCameraName);
                        MEntity activeCamera = 
                            fw.CameraManager.GetStandardCameraEntity(viewport,
                            cameraType);
                    
                        viewport.SetCamera(activeCamera, false);

                    }
                    else
                    {
                        MEntity activeCamera = scene.GetEntityByName(
                            activeCameraName);
                        if (activeCamera != null)
                        {
                            viewport.SetCamera(activeCamera, false);
                        }
                    }
                }

                // Set the rendering mode for the viewport.
                IRenderingMode pmRenderingMode = null;
                string strRenderingModeName =
                    settingService.GetSettingsObject(viewportPropertyPrefix +
                    ":RenderingMode", SettingsCategory.PerScene) as string;
                if (strRenderingModeName != null)
                {
                    IRenderingModeService pmRenderingModeService =
                        ServiceProvider.Instance.GetService(typeof(
                        IRenderingModeService)) as IRenderingModeService;
                    Debug.Assert(pmRenderingModeService != null,
                        "Rendering mode service not found!");

                    pmRenderingMode =
                        pmRenderingModeService.GetRenderingModeByName(
                        strRenderingModeName);
                    if (pmRenderingMode == null)
                    {
                        // Rendering mode not found, choose first available
                        // mode.
                        foreach (IRenderingMode pmMode in
                            pmRenderingModeService.GetRenderingModes())
                        {
                            if (pmMode.DisplayToUser)
                            {
                                pmRenderingMode = pmMode;
                                break;
                            }
                        }
                    }

                    viewport.RenderingMode = pmRenderingMode;
                }
            }
    
            object oExclusive = 
                settingService.GetSettingsObject("ExclusiveViewPort",
                SettingsCategory.PerScene);
            bool bExclusive = true;
            if (oExclusive != null)
            {
                bExclusive = (bool) oExclusive;
            }
    
            uint viewPortIndex = fw.ViewportManager.ViewportCount - 1;
            object oViewPortIndex =
                settingService.GetSettingsObject("ActiveViewPort",
                SettingsCategory.PerScene);
            if (oViewPortIndex != null)
            {
                viewPortIndex = (uint) oViewPortIndex;
            }
    
            if (bExclusive)
            {
                fw.ViewportManager.SetExclusiveViewport(viewPortIndex);
            }
            else
            {
                fw.ViewportManager.ClearExclusiveViewport();
                fw.ViewportManager.SetActiveViewport(viewPortIndex);
            }
        }


        private static void ClearViewportSettings()
        {
            MFramework fw = MFramework.Instance;
            ISettingsService settingService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            //ViewPort\camera stuff
            for (uint i = 0; i < fw.ViewportManager.ViewportCount; i++)
            {
                MViewport viewport = fw.ViewportManager.GetViewport(i);
                string viewportPropertyPrefix = string.Format("Viewport{0}_",
                    i);
                foreach (MCameraManager.StandardCamera standardCameraType in 
                    Enum.GetValues(typeof(MCameraManager.StandardCamera)))
                {
                    string cameraPrefix = "StandardCam:" + 
                        standardCameraType.ToString();

                    settingService.RemoveSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + ":Translation",
                        SettingsCategory.PerScene);

                    settingService.RemoveSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + ":Rotation",
                        SettingsCategory.PerScene);

                    settingService.RemoveSettingsObject(
                        viewportPropertyPrefix + cameraPrefix + 
                        ":FrustumWidth", SettingsCategory.PerScene);

                }


                settingService.RemoveSettingsObject(
                    viewportPropertyPrefix + ":ActiveCameraIsStandard", 
                    SettingsCategory.PerScene);

                settingService.RemoveSettingsObject(
                    viewportPropertyPrefix + ":ActiveCamera", 
                    SettingsCategory.PerScene);
                
            }
    
            settingService.RemoveSettingsObject("ExclusiveViewPort",
                SettingsCategory.PerScene);
    
            settingService.RemoveSettingsObject("ActiveViewPort",
                SettingsCategory.PerScene);
        }

        internal static void ResetPalettes()
        {
            FindPaletteFolder();
            RemoveAllPalettes();
            ArrayList paletteList = LoadPalettesFromFolder();
            RemovepertyTypes(paletteList);
            CheckAndRegisterPalettesAndPropertyTypes(paletteList);

            MFramework.Instance.PrefabManager.RemoveAllPrefabs();
            LoadPrefabsFromFolder(
                MFramework.Instance.PaletteManager.PaletteFolder);
        }

        private static void LoadPrefabsFromFolder(String directory)
        {
            DirectoryInfo di = new DirectoryInfo(directory);
            if (!di.Exists)
            {
                return;
            }

            MPrefabManager prefabManager =
                MFramework.Instance.PrefabManager;

            // Load all prefabs in this directory
            FileInfo[] files = di.GetFiles("*.*");
            foreach (FileInfo file in files)
            {
                if (file.Extension.ToLower() != ".pal")
                {
                    prefabManager.LoadPrefab(file.FullName);
                }
            }

            // Then check all subdirectories
            DirectoryInfo[] dirs = di.GetDirectories();
            foreach (DirectoryInfo dir in dirs)
            {
                if (dir.Name != "." && dir.Name != "..")
                {
                    LoadPrefabsFromFolder(dir.FullName);
                }
            }
        }

        internal static void CheckForConflicts()
        {
            //Now, recheck main scene
            bool bConflictsFound = CheckMainSceneForConflicts();
            MPalette orphans =
                EntityManagementUtilities.ResolveMasterEntities(
                MFramework.Instance.Scene,
                MFramework.Instance.PaletteManager.GetPalettes());
            ArrayList readOnlyPalettes = new ArrayList();
            bool bPalettesChanged = false;
            if (orphans.GetEntities().Length > 0)
            {
                MPalette[] palettes = 
                    MFramework.Instance.PaletteManager.GetPalettes();
                foreach (MPalette palette in palettes)
                {
                    // Put all the clean read-only palettes in this array
                    if (!palette.Writable && !palette.Dirty)
                        readOnlyPalettes.Add(palette);
                }
                                
                orphans.Name = "Unsaved Scene Templates";
                orphans.DontSave = true;
                if (SceneManagementUtilities.AddOrphansToPalettes(orphans))
                {
                    bPalettesChanged = true;
                    palettes = readOnlyPalettes.ToArray(typeof(MPalette)) 
                        as MPalette[];
                    foreach (MPalette palette in palettes)
                    {
                        if (!palette.Dirty)
                            readOnlyPalettes.Remove(palette);
                    }

                }
                orphans.Dispose();
            }

            string message = "";
            if (bConflictsFound)
            {
                message += "Conflicts were found between the scene that " +
                    "was just loaded and the\ncurrent set of palettes.  " +
                    "The scene has been modified to match\nthe palettes.\n\n";
            }
            if (bPalettesChanged)
            {
                message += "Palettes have been updated with templates in " +
                    "the loaded scene that\ndid not previously exist.  ";

                if (readOnlyPalettes.Count > 0)
                {
                    message += "Note that the following read-only palette " +
                        "files have been changed.\nThese files must be " +
                        "overwritten to save any changes to the scene\n";
                    foreach (MPalette palette in readOnlyPalettes)
                    {
                        message += "\n" + palette.Scene.SourceFilename;
                    }
                }
                message += "\n\n";
            }

            if (message.Length > 0)
            {
                message += "Please check the Messages Panel " +
                    "for more details.";
                MessageBox.Show(message, "Conflicts Resolved",
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

	    /// <summary>
        /// Determined where palettes should be loaded from
        /// </summary>
        private static void FindPaletteFolder()
        {
            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;
            string oldPaletteFolder = paletteManager.PaletteFolder;

            string pathToUse = null;
	        pathToUse = CalculatePaletteFolder();
            //Now, change path...
            MFramework.Instance.PaletteManager.PaletteFolder = 
                pathToUse;
        }

        internal static string CalculatePaletteFolder()
        {
            string pathToUse;
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                    as ISettingsService;
    
            string localFolder = 
                (settingsService.GetSettingsObject("PaletteFolder",
                SettingsCategory.PerScene) as string);
    
            if (localFolder == null || localFolder == string.Empty)
            {
                MFolderLocation perUserFolderLocation = 
                    settingsService.GetSettingsObject("PaletteFolder",
                    SettingsCategory.PerUser) as MFolderLocation;
                

                string perUserFolder = null;
                if (perUserFolderLocation != null)
                {
                    perUserFolder = perUserFolderLocation.Path;
                }
                if (perUserFolder == null || perUserFolder == string.Empty)
                {
                    pathToUse = Path.GetFullPath(
                        MFramework.Instance.AppStartupPath +
                        "..\\..\\Data\\Palettes\\");
                }
                else
                {
                    pathToUse = perUserFolder;
                }
            }
            else
            {
                if (settingsService.ScenePath == "")
                {
                    string properPath = Path.GetFullPath(localFolder) + "\\";
                    pathToUse = properPath;
                    
                }
                else
                {
                    string properPath = Path.GetFullPath(
                        settingsService.ScenePath + "\\" + localFolder) + "\\";
                    pathToUse = properPath;
                }
            }
            return pathToUse;
        }


        private static void RemoveAllPalettes()
        {
            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;
            MPalette[] palettes = paletteManager.GetPalettes();
            foreach(MPalette palette in palettes)
            {
                paletteManager.RemovePalette(palette);
            }
        }

        private static void OnUserPaletteFolderChanged(object pmSender, 
            SettingChangedEventArgs pmEventArgs)
        {
            CopyUserSettingsToSceneSettings();
            ResetPalettes();
            CheckForConflicts();
        }

        private static ArrayList LoadPalettesFromFolder()
        {        
            ArrayList retVal = new ArrayList();
            ArrayList readOnlyUpdatedPalettes = new ArrayList();
            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;

            IMessageService messageService = 
                ServiceProvider.Instance.GetService(
                typeof(IMessageService)) as IMessageService;
            try
            {
                DirectoryInfo di = new DirectoryInfo(
                    MFramework.Instance.PaletteManager.PaletteFolder);
                FileInfo[] files = di.GetFiles("*.pal");
                foreach (FileInfo file in files)
                {
                    MPalette palette = 
                        paletteManager.LoadPalette(file.FullName);
                    if (palette != null)
                    {
                        palette.ResetEntityNames();
                        retVal.Add(palette);
                        if (palette.Dirty && file.IsReadOnly)
                        {
                            readOnlyUpdatedPalettes.Add(file.FullName);
                        }
                    }
                    else
                    {
                        Message message = new Message();
                        message.m_strText = "Could not load palette file."; 
                            
                        message.m_strDetails = "The following file could " +
                            "not be loaded:\n\n" + file.FullName + 
                            "\n\nCheck that the file is valid.";
                        messageService.AddMessage(
                            MessageChannelType.Errors, message);
                    }
                }
            }
            catch (DirectoryNotFoundException e)
            {
                Message message = new Message();
                message.m_strText = "Palette folder not found.";
                message.m_strDetails = e.Message;
                messageService.AddMessage(MessageChannelType.Errors, message);
            }
            catch (IOException e)
            {
                Message message = new Message();
                message.m_strText = "IO error when accessing palette folder.";
                message.m_strDetails = "Path:\n" +
                    MFramework.Instance.PaletteManager.PaletteFolder +
                    "\nError:\n" + e.Message;
                messageService.AddMessage(MessageChannelType.Errors, message);
            }
            if (readOnlyUpdatedPalettes.Count > 0)
            {
                string message = "The palettes loaded from the following " +
                    "read-only files have been converted from a previous\n" +
                    "file version.  If these files are not saved, there " +
                    "may be errors when this scene is loaded next time.\n";
                foreach (string filename in readOnlyUpdatedPalettes)
                    message += '\n' + filename;

                MessageBox.Show(message, "Read-Only Palettes Updated!", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }

            return retVal;
        }

        public static void ConvertScenePalettePathToAbsolute(
            string basePath)
        {
            ISettingsService settingService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;
            MFolderLocation location = 
                settingService.GetSettingsObject("PaletteFolder",
                SettingsCategory.PerScene) as MFolderLocation;
            if (location != null && location.Path != string.Empty)
            {
                string combinedPath = Path.Combine(basePath, location.Path);
                string fullPath = BuildAbsolutePath(combinedPath);

                location = new MFolderLocation(fullPath);
                settingService.SetSettingsObject("PaletteFolder",
                    location, SettingsCategory.PerScene);
            }
        }


        public static void ConvertScenePalettePathToRelative(
            string basePath)
        {
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;
            MFolderLocation location = settingsService.GetSettingsObject(
                "PaletteFolder", SettingsCategory.PerScene) 
                as MFolderLocation;
            if (location != null && location.Path != string.Empty)
            {
                //Make sure both paths are absolute
                string absoluteBasePath = Path.GetFullPath(basePath);
                string absolutePaletteFolderPath = 
                    Path.GetFullPath(location.Path);
                
                string baseRoot = Path.GetPathRoot(absoluteBasePath);
                string paletteFolderRoot = 
                    Path.GetPathRoot(absolutePaletteFolderPath);
                if (baseRoot == paletteFolderRoot) //then  relative path 
                {
                    string relativePath = BuildRelativePath(absoluteBasePath,
                        absolutePaletteFolderPath);
                    location = new MFolderLocation(relativePath);
                    settingsService.SetSettingsObject("PaletteFolder",
                        location, SettingsCategory.PerScene);

                }
                else //keep absolute path
                {
                    //Do nothing
                }
                
            }
        }



        /// <summary>
        /// Builds a relative path, assuming both paths share a root.
        /// </summary>
        /// <param name="basePath">what the path shoule be relative to</param>
        /// <param name="path">path to make relative</param>
        /// <returns></returns>
        internal static string BuildRelativePath(string basePath, string path)
        {
            string retVal = path;
            //'scrub' the paths so they look like what we expect
            basePath = StripTrialingSeparator(basePath);
            path = StripTrialingSeparator(path);
            string commonRoot = StripTrialingSeparator(
                CommonRootPath(basePath, path));
            string subfolder = string.Empty;

            //basically walk from the base path to the given path
            string currentFolder = basePath;
            while (!(currentFolder == commonRoot) )
            {
                subfolder += @"\..";
                currentFolder = GetParentDirectory(currentFolder);
            }
            //now drill down from there
            subfolder += path.Substring(commonRoot.Length);
            //retVal = @"." + subfolder;
            retVal = @"." + subfolder;

            return retVal;
        }

        private static string BuildAbsolutePath(string path)
        {
            string wellFormedPath = StripTrialingSeparator(
                path.Replace(Path.AltDirectorySeparatorChar,
                Path.DirectorySeparatorChar));
            string pathWithDotsRemoved = wellFormedPath.Replace(@"\.\",
                @"\");
            string retVal = pathWithDotsRemoved;
            while (retVal.IndexOf(@"..") > -1)
            {
                int firstParentIndex = retVal.IndexOf(@"\..");
                int parentNameIndex = retVal.LastIndexOf(@"\",
                    Math.Max(firstParentIndex - 1, 0));
                if (parentNameIndex < 0)
                    parentNameIndex = 0;
                retVal = retVal.Substring(0, parentNameIndex) +
                    retVal.Substring(firstParentIndex + @"\..".Length);

            }

            retVal = retVal.Replace(@"\.", "");
            return retVal;
        }

        internal static string StripTrialingSeparator(string path)
        {
            string retVal = null;
            if (path.EndsWith(Path.DirectorySeparatorChar.ToString()) ||
                path.EndsWith(Path.AltDirectorySeparatorChar.ToString()))
            {
                retVal = path.Substring(0, path.Length - 1);
            }
            else
            {
                retVal = path;
            }
            return retVal.Replace(Path.AltDirectorySeparatorChar, 
                Path.DirectorySeparatorChar);
        }
        //Note that this function expects paths that have been 'conditioned'
        //by calling 'StripTrialingSeparator'
        internal static string CommonRootPath(string path1, string path2)
        {
            //StringBuilder retVal = new StringBuilder();
            string[] splitPath1 = path1.Split(Path.DirectorySeparatorChar);
            string[] splitPath2 = path2.Split(Path.DirectorySeparatorChar);

            int maxIndex = Math.Min(splitPath1.Length, splitPath2.Length);
            int lastMatchingIndex = -1;
            for (int index = 0; index < maxIndex; index++)
            {                
                if (splitPath1[index] == splitPath2[index])
                {
                    lastMatchingIndex = index;
                    continue;
                }
                break;
            }
            if (lastMatchingIndex > -1)
            {
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i <= lastMatchingIndex; i++)
                {
                    sb.Append(splitPath1[i]);
                    if ( i<(lastMatchingIndex))
                    {
                        sb.Append(Path.DirectorySeparatorChar);
                    }
                }
                return sb.ToString();
            }
            else
            {
                return "";
            }
        }

        internal static string GetParentDirectory(string path)
        {
            int index = path.LastIndexOf(Path.DirectorySeparatorChar);
            if (index >= 1)
            {
                return path.Substring(0, index);
            }
            return "";
                
        }

        public static void CopySceneSettingsToUserSettings()
        {
            ISettingsService settingsService =
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            object perScenePaletteFolder = 
                settingsService.GetSettingsObject("PaletteFolder",
                SettingsCategory.PerScene);
            settingsService.SetSettingsObject("PaletteFolder", 
                perScenePaletteFolder, SettingsCategory.PerUser);
        }

        private static void CopyUserSettingsToSceneSettings()
        {
            ISettingsService settingsService =
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            object perUserPaletteFolder = 
                settingsService.GetSettingsObject("PaletteFolder",
                SettingsCategory.PerUser);
            settingsService.SetSettingsObject("PaletteFolder", 
                perUserPaletteFolder, SettingsCategory.PerScene);
            
        }

        private static void RemovepertyTypes(ArrayList list)
        {
            foreach (MPalette palette in list)
            {
                ComponentManagementUtilities.
                    RemoveRegisteredcomponentsInScene(palette.Scene);
            }
        }


        private static void CheckAndRegisterPalettesAndPropertyTypes(
            ArrayList palettes)
        {
            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;
            
            foreach(MPalette palette in palettes)
            {
                ComponentManagementUtilities.BuildComponentListFromScene(
                    palette.Scene);
                paletteManager.AddPalette(palette);
            }

            // Now mark all palletes as clean
//            foreach(MPalette palette in paletteManager.GetPalettes())
//            {
//                DirtyBitUtilities.MakeSceneClean(palette.Scene);
//            }
            
        }

        public static void SetNameOnTemplates(MPalette palette)
        {
            //We only rename top level tmeplates
            MEntity[] templates = palette.GetEntities();
            foreach(MEntity entity in templates)
            {
                string simpleName;
                Regex regex = new Regex(@"(\[.+\])");
                Match match = regex.Match(entity.Name);
                if (match.Success)
                {
                    simpleName = entity.Name.Substring(match.Length);
                }
                else
                {
                    simpleName = entity.Name;
                }

                entity.Name = string.Format("[{0}]{1}",
                    palette.Name, simpleName);
            }
        }

        // Returns true if the entities in the orphans palette are added to an existing palette or
        // are added to a new palette in the palette manager.
	    public static bool AddOrphansToPalettes(MPalette orphans)
	    {
            bool bPalettesChanged = false;
            IMessageService messageService = 
                ServiceProvider.Instance.GetService(
                typeof(IMessageService)) as IMessageService;

            MessageChannelType channelForOrphans = 
                MessageChannelType.Conflicts;

            MPaletteManager paletteManager = 
                MFramework.Instance.PaletteManager;

            MEntity[] entities = orphans.GetEntities();
            Message msg;
            foreach(MEntity entity in entities)
            {
                string paletteName = ParsePaletteName(entity.Name);
                Debug.Assert(paletteName != null);
                if (string.IsNullOrEmpty(paletteName))
                {
                    orphans.RemoveEntity(entity, false);
                    continue;
                }
                MPalette homePalette = 
                    paletteManager.GetPaletteByName(paletteName);
                if (homePalette == null)
                {
                    //Add a message
                    msg = new Message();
                    msg.m_strText = string.Format(
                        "PALETTE CHANGED: Palette '{0}' was created.",
                        paletteName);
                    msg.m_strDetails = string.Format(
                        "Palette '{0}' was added in palette folder: \n{1}",
                        paletteName, paletteManager.PaletteFolder);
                    messageService.AddMessage(channelForOrphans, msg);
                    homePalette = new MPalette(paletteName,0);
                    homePalette.Scene.SourceFilename =
                        paletteManager.PaletteFolder + paletteName + ".pal";
                    paletteManager.AddPalette(homePalette);
                }
                orphans.RemoveEntity(entity, false);
                homePalette.AddEntity(entity, "", false);
                msg = new Message(); 
                msg.m_strText = string.Format(
                    "PALETTE CHANGED: Template '{0}' Added.",
                    entity.Name);
                msg.m_strDetails = string.Format(
                    "Template '{0}' was added to palette '{1}'",
                    entity.Name, homePalette.Name);
                messageService.AddMessage(channelForOrphans, msg);
                bPalettesChanged = true;

            }
            return bPalettesChanged;
	    }

	    public static string ParsePaletteName(string name)
        {
            //matches text between two square brakets
            Regex regex = new Regex(@"(?<=(\[))[^\]]+");
            Match match = regex.Match(name);
            if (match.Success)
            {
                return match.Value;
            }
            else
            {
                return string.Empty;
            }
        }


    }
}
