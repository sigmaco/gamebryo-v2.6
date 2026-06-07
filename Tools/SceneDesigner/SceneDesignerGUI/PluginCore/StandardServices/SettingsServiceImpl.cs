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
using System.Reflection;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Serialization.Formatters.Soap;
using System.Text;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for SettingsServiceImpl.
    /// </summary>
    public class SettingsServiceImpl : StandardServiceBase, ISettingsService
    {
        #region Private Data        
        private Hashtable m_globalSettings;
        private Hashtable m_userSettings;
        private Hashtable m_sceneSettings;
        private Hashtable m_handlers;
        private readonly string m_sGamebryoFolder;
        private string m_strScenePath;
        private string m_strFileName;

        #endregion

        public SettingsServiceImpl()
        {
            m_globalSettings = new Hashtable();
            m_userSettings = new Hashtable();
            m_sceneSettings = new Hashtable();
            m_handlers = new Hashtable();
            m_handlers.Add(SettingsCategory.Global, new Hashtable());
            m_handlers.Add(SettingsCategory.PerUser, new Hashtable());
            m_handlers.Add(SettingsCategory.PerScene, new Hashtable());
            m_handlers.Add(SettingsCategory.Temp, new Hashtable());
            if (MFramework.InstanceIsValid())
            {
                m_sGamebryoFolder = 
                    @"\Emergent Game Technologies\" + 
                    MFramework.Instance.ApplicationName + @"\";
            }
            m_strScenePath = @"\NoScene.Scene.Settings";
        }

        public string ScenePath
        {
            get
            { return m_strScenePath; }
            set
            { m_strScenePath = value; }
        }

        public string SceneFileName
        {
            get
            { return m_strFileName; }
            set
            { m_strFileName = value; }
        }

        #region Experimental Code

        public class DelegateProxyClass
        {
            event SettingChangedHandler InternalEvent;

            public SettingChangedHandler DelegateObject
            {
                get
                {
                    return InternalEvent;
                }
            }

            public DelegateProxyClass(SettingChangedHandler handler)
            {
                InternalEvent = handler;
            }

            public static DelegateProxyClass operator+(DelegateProxyClass lhs,
                SettingChangedHandler handler)
            {
                lhs.InternalEvent += handler;
                return lhs;
            }

            public static DelegateProxyClass operator-(DelegateProxyClass lhs,
                SettingChangedHandler handler)
            {
                lhs.InternalEvent -= handler;
                return lhs;
            }
        }


        public DelegateProxyClass this[string name, SettingsCategory category]
        {
            get
            {
                Hashtable handlerStore = m_handlers[category]
                    as Hashtable;
                return new DelegateProxyClass(handlerStore[name] 
                    as SettingChangedHandler);
            }

            set
            {
                Hashtable handlerStore = m_handlers[category]
                    as Hashtable;
                if (handlerStore.ContainsKey(name))
                {
                    handlerStore[name] = value.DelegateObject;
                }
                else
                {
                    handlerStore.Add(name, value.DelegateObject);
                }
            }
        }

        #endregion

        class CustomBinder : SerializationBinder
        {
            public override Type BindToType(string assemblyName, 
                string typeName)
            {
                Type retVal = null;
                try
                {
                    retVal = Type.GetType(
                        string.Format("{0},{1}", typeName, assemblyName)
                        ,true, true);
                    return retVal;
                }
                catch (FileNotFoundException)
                {
                    if (retVal == null)
                    {
                        retVal = FindTypeInLoadedAssemblies(assemblyName,
                            typeName);
                    }
                    return retVal;
                    
                }
            }

            private Type FindTypeInLoadedAssemblies(string assemblyName,
                string typeName)
            {
                Assembly[] assemblies = 
                    AppDomain.CurrentDomain.GetAssemblies();
                foreach (Assembly assembly in assemblies)
                {
                    if (assembly.FullName.Equals(assemblyName))
                    {
                        Type type = assembly.GetType(typeName);
                        if (type != null)
                        {
                            return type;
                        }
                    }
                }
                return null;
            }
        }

        public override bool Start()
        {
            bool bSuccess = base.Start();
            if (bSuccess)
            {
                LoadSettings(SettingsCategory.Global);
                LoadSettings(SettingsCategory.PerUser);
            }
            return bSuccess;
        }

        public void RegisterSettingsObject(string name, object obj,
            SettingsCategory category)
        {
            object existingValue = GetSettingsObject(name, category);
            Hashtable settingsHash = GetSettingsHash(category);
            if (existingValue == null)
            {
                settingsHash.Add(name, obj);
            }
        }

        public object GetSettingsObject(string name, SettingsCategory category)
        {
            Hashtable settingsHash = GetSettingsHash(category);
            object returnVal = settingsHash[name];
            return returnVal;
        }

        public void SetSettingsObject(string name, object obj, 
            SettingsCategory category)
        {
            bool changeOccured = false;
            Hashtable settingsHash = GetSettingsHash(category);
            object existingValue = GetSettingsObject(name, category);
            if (existingValue == null)
            {
                settingsHash.Add(name, obj);                
                changeOccured = true;
            }
            else
            {
                if (!existingValue.Equals(obj))
                {
                    changeOccured = true;
                }
                settingsHash[name] = obj;
            }
            //Determine if an event needs to be raised
            Hashtable handlerStore = ((Hashtable)m_handlers[category]);   
            if (changeOccured && handlerStore.ContainsKey(name))
            {
                Delegate del = handlerStore[name]
                    as Delegate;
                SettingChangedHandler handler = del as SettingChangedHandler;
                SettingChangedEventArgs args = 
                    new SettingChangedEventArgs(name, category);
                //Shouldn't be null at this point unless someone passed a null 
                //delegate object as the handler
                if (handler != null)
                {
                    handler(this, args);
                }
            }
            if (changeOccured && category == SettingsCategory.PerScene)
            {
                MFramework.Instance.Scene.Dirty = true;
            }

        }

        public void RemoveSettingsObject(string strName, 
            SettingsCategory eCategory)
        {
            object existingValue = GetSettingsObject(strName, eCategory);
            if (existingValue != null)
            {
                Hashtable settingsHash = GetSettingsHash(eCategory);
                settingsHash.Remove(strName);
            }
        }

        public void SaveSettings(SettingsCategory category)
        {
            Hashtable settingsHash = GetSettingsHash(category);

            using (Stream stream = GetStream(category, FileMode.Create,
                       FileAccess.Write))
            {
                if (stream == null)
                {
                    return;
                }

                IFormatter formatter = GetFormatter();
                Hashtable stringHashtable = new Hashtable();
                foreach(DictionaryEntry entry in settingsHash)
                {
                    using (MemoryStream memoryStream = new MemoryStream() )
                    {
                        formatter.Serialize(memoryStream, entry.Value);
                        string settingString = 
                            Encoding.UTF8.GetString(memoryStream.GetBuffer());
                        stringHashtable.Add(entry.Key, settingString);
                    }
                }
                formatter.Serialize(stream, stringHashtable);
            }
        }      

        public void LoadSettings(SettingsCategory category)
        {
            bool bLoadFailed = true;
            try
            {
                using (Stream stream  = GetStream(category, FileMode.Open,
                           FileAccess.Read))
                {
                    if (stream == null)
                    {
                        return;
                    }

                    Hashtable oldHash = GetSettingsHash(category);
                    IFormatter formatter = GetFormatter();
                    Hashtable stringHashtable = 
                        formatter.Deserialize(stream) as Hashtable;
                    Hashtable settings = 
                        GetObjectHashFromFromStringHash(stringHashtable, 
                        formatter);
                    if (settings != null)
                    {
                        SetSettingsHash(settings, category);
                    }
                    Hashtable handlerStore = 
                        ((Hashtable)m_handlers[category]);
                    NotifyAllListeners(settings, handlerStore, category);
                    MergeMissingKeys(settings, oldHash);
                }
                bLoadFailed = false;
            }
            catch (FileNotFoundException e)
            {
                AddMessage(MessageChannelType.Errors, 
                    "Settings file not loaded",
                    e.FileName + Environment.NewLine + "Could not be found");
            }
            catch (DirectoryNotFoundException e)
            {
                AddMessage(MessageChannelType.Errors, 
                    "Settings file not loaded", 
                    e.Message + Environment.NewLine + "Could not be found");
            }
            catch (SerializationException e)
            {
                AddMessage(MessageChannelType.Errors,
                    "Settings file not loaded",
                    e.Message + Environment.NewLine + "Could not be found");
            }
            if (bLoadFailed)
            {
                ISettingsService settingService =
                    ServiceProvider.Instance.GetService(
                    typeof(ISettingsService)) as ISettingsService;
                settingService.SetSettingsObject("PaletteFolder",
                    new MFolderLocation(
                    MFramework.Instance.PaletteManager.PaletteFolder),
                    SettingsCategory.PerScene);
            }
        }

        public void ClearSettings(SettingsCategory category)
        {
            Hashtable settingsStore = GetSettingsHash(category);
            settingsStore.Clear();
        }

        public void SetChangedSettingHandler(string name, 
            SettingsCategory category, SettingChangedHandler handler)
        {
            Hashtable handlerStore = ((Hashtable)m_handlers[category]);
            SettingChangedHandler oldHandler = handlerStore[name] 
                as SettingChangedHandler;
            if (oldHandler != null)
            {
                SettingChangedHandler newHandler = oldHandler + handler;
                handlerStore[name] = newHandler;
            }
            else
            {
                handlerStore.Add(name, handler);
            }
        }

        internal string[] GetSettingsNames(SettingsCategory category)
        {
            Hashtable settingsStore = GetSettingsHash(category);
            string[] retVal = new string[settingsStore.Count];
            int index = 0;
            foreach (DictionaryEntry entry in settingsStore)
            {
                retVal[index++] = entry.Key as string;
            }
            return retVal;
        }

        private void AddMessage(MessageChannelType category, string text,
            string details)
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IMessageService msgSvc = 
                sp.GetService(typeof(IMessageService))
                as IMessageService;
            Message msg = new Message();
            msg.m_strText = text;
            msg.m_strDetails = details;
            msgSvc.AddMessage(category, msg);
            
        }

        private static void MergeMissingKeys(Hashtable settings, 
            Hashtable oldHash)
        {
            //now, if there are any settings that were not loaded 
            //in, readd those
            foreach (DictionaryEntry oldSettingEntry in oldHash)
            {
                string key = oldSettingEntry.Key as string;
                if (!settings.ContainsKey(key))
                {
                    //Then this old setting is gone, re-add it
                    settings.Add(key, oldSettingEntry.Value);
                }
            }
        }

        private void NotifyAllListeners(Hashtable settings, 
            Hashtable handlerStore, SettingsCategory category)
        {
            foreach (DictionaryEntry settingEntry in settings)
            {
                string key = settingEntry.Key as string;
                if (handlerStore.ContainsKey(key))
                {
                    Delegate del = handlerStore[key]
                        as Delegate;
                    SettingChangedHandler handler = 
                        del as SettingChangedHandler;
                    SettingChangedEventArgs args = 
                        new SettingChangedEventArgs(key,
                        category);
                    //Shouldn't be null at this point unless 
                    //someone passed a null delegate object as the
                    //handler
                    if (handler != null)
                    {
                        handler(this, args);
                    }                                
                }
            }
        }

        private Hashtable GetObjectHashFromFromStringHash(
            Hashtable stringHashtable, IFormatter formatter)
        {
            Hashtable settings = new Hashtable();
            foreach (DictionaryEntry entry in stringHashtable)
            {
                try
                {
                    using (Stream memoryStream = 
                               new MemoryStream(
                               ASCIIEncoding.UTF8.GetBytes(
                               entry.Value as string)))
                    {
                        object setting = 
                            formatter.Deserialize(memoryStream);
                        settings.Add(entry.Key, setting);
                    }
                }
                catch (Exception e)
                {
                    AddMessage(MessageChannelType.Errors,
                        string.Format("Setting: {0} not loaded", 
                        entry.Key as string), e.ToString());
                }
            }
            return settings;
        }

        private void SetSettingsHash(Hashtable settings, 
            SettingsCategory category)
        {
            switch (category)
            {
                case SettingsCategory.Global:
                {
                    m_globalSettings = settings;
                    break;
                }
                case SettingsCategory.PerUser:
                {
                    m_userSettings = settings;
                    break;
                }
                case SettingsCategory.PerScene:
                {
                    m_sceneSettings = settings;
                    break;
                }
                default:
                {
                    throw new ArgumentOutOfRangeException(
                        "category", category,"Invalid category");
                }                    
            }
        }

        private Hashtable GetSettingsHash(SettingsCategory category)
        {
            switch (category)
            {
                case SettingsCategory.Global:
                {
                    return m_globalSettings;
                }
                case SettingsCategory.PerUser:
                {
                    return m_userSettings;
                }
                case SettingsCategory.PerScene:
                {
                    return m_sceneSettings;
                }
                default:
                {
                    return null;
                }
            }                        
        }

        private Stream GetStream(SettingsCategory category, 
            FileMode mode, FileAccess access)
        {
            Stream retVal = null;
            string path;
            string fileName;
            switch (category)
            {
                case SettingsCategory.Global:
                {
   
                    path = Environment.GetFolderPath(
                        Environment.SpecialFolder.CommonApplicationData)
                        + m_sGamebryoFolder;
                    fileName = "Tool.Settings";
                    break;
                }
                case SettingsCategory.PerUser:
                {
                    path = Environment.GetFolderPath(
                        Environment.SpecialFolder.LocalApplicationData)
                        + m_sGamebryoFolder;
                    fileName = "Tool.Settings";
                    break;
                }
                case SettingsCategory.PerScene:
                {
                    path = m_strScenePath;
                    fileName = m_strFileName;
                    break;
                }
                default:
                case SettingsCategory.Temp:
                {
                    return null;
                }
            }
            CheckDirectory(path,  access);

            string fullFileName = path + @"\" + fileName;
            try
            {
                if ((access & FileAccess.Write) == FileAccess.Write)
                {
                    FileInfo fi = new FileInfo(fullFileName);
                    if (fi.Exists && (fi.Attributes & FileAttributes.ReadOnly)
                        == FileAttributes.ReadOnly)
                    {
                        fi.Attributes = fi.Attributes &
                            (~FileAttributes.ReadOnly);
                    }
                }
                retVal = new FileStream(fullFileName, mode, access);
            }
            catch (UnauthorizedAccessException e)
            {
                AddMessage(MessageChannelType.Errors, "Unable to access " +
                    "settings file at " + fullFileName + ".", e.ToString());
                retVal = null;
            }
            
            return retVal;
        }

        private bool CheckDirectory(string path, FileAccess access)
        {
            DirectoryInfo di = new DirectoryInfo(path);
            if (di.Exists)
            {
                return true;
            }
            if ((access & FileAccess.Write) == FileAccess.Write)
            {
                Directory.CreateDirectory(path);
                return true;
            }
            return false;
        }


        private IFormatter GetFormatter()
        {
            SoapFormatter retVal = new SoapFormatter();
            retVal.TypeFormat = FormatterTypeStyle.TypesAlways;
            retVal.Binder = new CustomBinder();
            
            return retVal;
        }

    }
}
