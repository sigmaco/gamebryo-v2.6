// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.IO;
using System.Reflection;
using System.Xml;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
    /// <summary>
    /// Summary description for CommandBindingBuilder.
    /// </summary>
    internal class CommandBindingBuilder : AbstractBuilder
    {

        #region Private Data
        private ServiceProvider m_provider;
        private IUICommandService m_commandService;
        private IMessageService m_messageService;
        #endregion

        public CommandBindingBuilder(ServiceProvider provider)
        {
            m_provider = provider;
            m_commandService = m_provider.GetService(typeof(IUICommandService))
                as IUICommandService;
            m_messageService = m_provider.GetService(typeof(IMessageService))
                as IMessageService;
        }

        //NOTE jwolfe I have not implemented this part yet because
        //I am not sure of the usefullness of binding commands using 
        //an external xml file. The Method bound to the command would have to
        //have the right signature anyway, so it would be extreamly limited in
        //what it could bind to.
        public override void Build(Stream xmlStream)
        {
            XmlDocument domObject = ReadXML(xmlStream);
            XmlNodeList items = domObject.DocumentElement.SelectNodes("MenuItem");
            BuildFromNodeList(items, "");
        }

        public void BuildFromLoadedAssemblies()
        {
            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach (Assembly assembly in assemblies)
            {
                object[] attributes = assembly.GetCustomAttributes(
                    typeof(ContainsUICommandsAttribute), false);
                if (attributes.Length > 0)
                {                    
                    BindAssemblyStaticMethods(assembly);
                }
            }
        }


        private void BindAssemblyStaticMethods(Assembly assembly)
        {
            Type[] types = assembly.GetTypes();
            BindingFlags bindingFlags = BindingFlags.NonPublic | BindingFlags.Public
                | BindingFlags.Static;
            foreach (Type type in types)
            {
                UICommandUtility.BindMethodsOnType(type, bindingFlags, null);
            }
        }




        private void BuildFromNodeList(XmlNodeList items, string s)
        {
        }
    }
}
