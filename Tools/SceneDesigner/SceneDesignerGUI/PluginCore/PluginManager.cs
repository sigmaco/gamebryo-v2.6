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
using System.Reflection;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.Utility;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore
{
    /// <summary>
    /// 
    /// Utility class for initializing framework core
    /// </summary>
    public class PluginManager
    {
        #region Private Data
        private static ArrayList m_pluginList;
        private static IMessageService m_pmMessageService;
        private static ArrayList m_delayedMessages;
        #endregion

        static PluginManager()
        {
            m_pluginList = new ArrayList();
            m_delayedMessages = new ArrayList();
        }

        class DelayedMessage
        {
            public Message m_message;
            public MessageChannelType m_eChannel;
        }

        private static IMessageService MessageService
        {
            get
            {
                if (m_pmMessageService == null)
                {
                    m_pmMessageService = ServiceProvider.Instance.GetService(
                        typeof(IMessageService)) as IMessageService;
                    Debug.Assert(m_pmMessageService != null, "Message " +
                        "service not found!");
                }
                return m_pmMessageService;
            }
        }


        public static void LoadPlugins(string pluginPath)
        {
            
            LoadInternalPlugins();
            LoadUserPlugins(pluginPath);
            InitializeServices();
            StartServices();
            StartPlugins();
            PrintMessages();
        }

        public static void Shutdown()
        {
            foreach (IPlugin plugin in m_pluginList)
            {
                IDisposable disposable = plugin as IDisposable;
                if (disposable != null)
                {
                    disposable.Dispose();
                }
            }
        }

        private static void AddDelayedMessage(MessageChannelType channel,
            string messageString)
        {
            AddDelayedMessage(channel, messageString, string.Empty);
        }

        private static void AddDelayedMessage(MessageChannelType channel,
            string messageString, string messageDetails)
        {
            DelayedMessage delayedMessage = new DelayedMessage();
            delayedMessage.m_eChannel = channel;
            Message message = new Message();
            message.m_strText = messageString;
            message.m_strDetails = messageDetails;
            delayedMessage.m_message = message;
            m_delayedMessages.Add(delayedMessage);
        }

        private static void PrintMessages()
        {
            IMessageService service = MessageService;
            if (service != null)
            {
                foreach (DelayedMessage message in m_delayedMessages)
                {
                    service.AddMessage(message.m_eChannel, message.m_message);
                }
            }
        }

        private static void StartPlugins()
        {
            foreach (IPlugin plugin in m_pluginList)
            {
                plugin.Start();
            }
        }

        private static void InitializeServices()
        {
            ServiceProviderImplementation.Instance.IntializeServices();
        }

        private static void StartServices()
        {
            ServiceProviderImplementation.Instance.StartServices();
        }

        private static void LoadInternalPlugins()
        {
            LoadPluginsFromAssembly(Assembly.GetExecutingAssembly());
        }

        private static void LoadUserPlugins(string plugInDirectoryPath)
        {
            DirectoryInfo di = new DirectoryInfo(plugInDirectoryPath);
            try
            {
                FileInfo[] files = di.GetFiles("*.dll");
                foreach(FileInfo file in files)
                {
                    Assembly assembly;
                    try
                    {
                        assembly  = Assembly.LoadFile(file.FullName);
                    }
                    catch (BadImageFormatException)
                    {
                        AddDelayedMessage(MessageChannelType.Errors,
                            "Ignoring plug-in file; DLL does not contain a " +
                            ".NET assembly.", file.Name);
                        continue;
                    }
                    bool bHasInitAndShutdown =
                        AssemblyUtilities.CheckForInitAndShutDown(assembly);
                    if (!bHasInitAndShutdown)
                    {
                        AddDelayedMessage(MessageChannelType.General,
                            "Plug-in does not contain both Init and " +
                            "Shutdown functions; consider adding such " +
                            "functions to properly handle static data in " +
                            "Managed C++ DLLs.",
                            file.Name);                        
                    }
                    AssemblyUtilities.FindAllCallInitMethod(assembly);
                    LoadPluginsFromAssembly(assembly);
                }
            }
            catch (DirectoryNotFoundException)
            {
                string message = "Plugin Directory:" + Environment.NewLine + 
                    di.FullName + Environment.NewLine + "Not Found!";
                AddDelayedMessage(MessageChannelType.Errors,
                    message);
                Debug.WriteLine(message);
            }
        }

        private static void LoadPluginsFromAssembly(Assembly assembly)
        {
            //Check for Plug-In instances
            Version toolVersion = 
                Assembly.GetExecutingAssembly().GetName().Version;
            
            Type[] types = assembly.GetTypes();
            Type plugInType = typeof(IPlugin);
            ArrayList plugInList = new ArrayList();
            foreach(Type type in types)
            {
                if (plugInType.IsAssignableFrom(type) && !type.IsAbstract)
                {
                    try
                    {
                        IPlugin plugin = 
                            (IPlugin) Activator.CreateInstance(type);
                        plugin.Load(toolVersion.Major,
                            toolVersion.Minor);
                        plugInList.Add(plugin);
                    }
                    catch (Exception e)
                    {
                        string message = "Could not load plugin class:"
                            + type + Environment.NewLine + "\t" + e;
                        Debug.WriteLine(message);
                        AddDelayedMessage(
                            MessageChannelType.Errors, message);
                    }
                }                
            }
            foreach(IPlugin plugIn in plugInList)
            {
                IService[] services = 
                    plugIn.GetProvidedServices();
                if ( services != null)
                {
                    foreach(IService service in services)
                    {
                        ServiceProviderImplementation.Instance.AddService(
                            service.GetType(), service);
                    }
                }
            }
            if (plugInList.Count > 0)
            {
                m_pluginList.AddRange(plugInList);
            }

        }

    }
}
