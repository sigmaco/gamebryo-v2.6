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
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
//using Microsoft.Win32;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    public delegate void MRUFileSelectedCB(object o, System.EventArgs e);

    public class MRUManager
    {
        private static  int ms_iMaxFiles = 10;

        public static void Init()
        {
            ArrayList mruList = GetMRUList();
            ArrayList validatedMRU = new ArrayList();
            foreach(string file in mruList)
            {
                FileInfo fi = new FileInfo(file);
                if (fi.Exists)
                {
                    validatedMRU.Add(file);
                }
            }
            SetMRUList(validatedMRU);
        }

        public static void Clear()
        {
            ArrayList emptyList = new ArrayList();
            SetMRUList(emptyList);
        }


        private static void DeleteMRUReg()
        {
            SettingsServiceImpl settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService)) 
                as SettingsServiceImpl;

            string[] names = 
                settingsService.GetSettingsNames(SettingsCategory.PerUser);

            foreach (string name in names)
            {
                if (name.IndexOf("MRU:") != -1)
                {
                    settingsService.RemoveSettingsObject(name,
                        SettingsCategory.PerUser);
                }
            }
        }

        private static bool SetMRUList(ArrayList alMRU)
        {
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService)) 
                as ISettingsService;

            if (alMRU == null )
                return false;

            DeleteMenus(GetMRUList());

            DeleteMRUReg();
           
            // Add the new keys
            int iIndex = 0;
            foreach(string strSubKey in alMRU)
            {
                // This will ensure that if there are additional files in
                // the registry (from a previous version), they will get
                // wiped out.
                if (iIndex >= ms_iMaxFiles)
                    break;

                string strVal = "MRU:" + iIndex.ToString();
                settingsService.SetSettingsObject(strVal, 
                    strSubKey, SettingsCategory.PerUser);
                iIndex++;
            }
            PopulateRecentFiles(alMRU);

            return true;
        }

        private static ArrayList GetMRUList()
        {
            ISettingsService settingsService = 
                ServiceProvider.Instance.GetService(typeof(ISettingsService)) 
                as ISettingsService;
            ArrayList alMRU = new ArrayList();

            for (int index = 0; index < ms_iMaxFiles; index++)
            {
                string strVal = "MRU:" + index.ToString();
                object setting = settingsService.GetSettingsObject(strVal, 
                     SettingsCategory.PerUser);
                if (setting != null)
                {
                    string settingString = setting as string;
                    if (settingString != null && 
                        !settingString.Equals(string.Empty))
                    {
                        alMRU.Add(settingString);
                    }
                }

            }
            return alMRU;
        }

        public static void PopulateRecentFiles(ArrayList alMRU)
        {
            if (alMRU == null)
            {
                alMRU = GetMRUList();
            }
            ServiceProvider sp = ServiceProvider.Instance;

            IUICommandService commandService = 
                sp.GetService(typeof(IUICommandService)) 
                as IUICommandService;


            IMenuService menuService = 
                sp.GetService(typeof(IMenuService)) 
                as IMenuService;

            foreach (string menu in alMRU)
            {
                string commandName = "MRUList." + menu;
                UICommand command = commandService.GetCommand(commandName);
                command.Click += new EventHandler(OnMRUClick);
                string menuString = "File##Recent##" + menu;
                menuService.AddMenu(menuString, command);
            }

        }

        private static void DeleteMenus(ArrayList alMRU)
        {
            if (alMRU == null)
            {
                return;
            }

            ServiceProvider sp = ServiceProvider.Instance;

            IUICommandService commandService = 
                sp.GetService(typeof(IUICommandService)) 
                as IUICommandService;


            IMenuService menuService = 
                sp.GetService(typeof(IMenuService)) 
                as IMenuService;

            foreach (string menu in alMRU)
            {
                string commandName = "MRUList." + menu;
                UICommand command = commandService.GetCommand(commandName);
                string[] oldMenus = menuService.GetAssignedMenus(command);
                foreach (string oldMenuName in oldMenus)
                {
                    command.Click -= new EventHandler(OnMRUClick);
                    menuService.RemoveMenu("File##Recent##" + 
                        oldMenuName, command, false);
                    
                }
            }

        }


        private static void OnMRUClick(object sender, EventArgs e)
        {
            UICommand senderCommand = sender as UICommand;
            //Emit a MRULoad UICommand
            ServiceProvider sp = ServiceProvider.Instance;

            IUICommandService commandService = 
                sp.GetService(typeof(IUICommandService)) 
                as IUICommandService;

            UICommand command = commandService.GetCommand("MRULoad");
            UICommandEventArgs commandArgs = new UICommandEventArgs();
            string filename = senderCommand.Name.Replace("MRUList.", "");
            commandArgs.SetValue("Filename", filename);
            command.DoClick(command, commandArgs);

        }

        public static void AddFile(string strFileName)
        {
            RemoveFile(strFileName);

            ArrayList alMRU = GetMRUList();

            if (alMRU == null)
            {
                alMRU = new ArrayList();
            }

            if ( alMRU.Count == ms_iMaxFiles )
                alMRU.RemoveAt(ms_iMaxFiles - 1);

            alMRU.Insert(0, strFileName);

            bool bResult = SetMRUList(alMRU);
            Debug.Assert(bResult == true);
        }

        private static void RemoveFile(string strFileName)
        {
            ArrayList alMRU = GetMRUList();

            if (alMRU == null)
                return;

            int i = 0;
            foreach(string strFileInReg in alMRU)
            {
                if (strFileInReg == null)
                    continue;

                if (strFileInReg == strFileName)
                {
                    alMRU.RemoveAt(i);
                    break;
                }
                i++;
            }

            bool bResult = SetMRUList(alMRU);
            Debug.Assert(bResult == true);
        }

    }
}