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
using System.Diagnostics;
using System.IO;
using System.Xml;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
    /// <summary>
    /// Summary description for DynamicMenuBuilder.
    /// </summary>
    internal class DynamicMenuBuilder : AbstractBuilder
    {
        #region Private Data
        private ServiceProvider m_provider;
        private IUICommandService m_commandService;
        private IMenuService m_menuService;

        #endregion
        public DynamicMenuBuilder(ServiceProvider provider)
        {
            m_provider = provider;
            m_commandService = m_provider.GetService(typeof(IUICommandService))
                as IUICommandService;
            m_menuService = m_provider.GetService(typeof(IMenuService))
                as IMenuService;
        }


        public override void Build(Stream xmlStream)
        {
            XmlDocument domObject = ReadXML(xmlStream);
            XmlNodeList items = 
                domObject.DocumentElement.SelectNodes("MenuItem");
            BuildFromNodeList(items, "");
        }

        private void BuildFromNodeList(XmlNodeList menuItems, string baseName)
        {
            foreach(XmlNode menuItem in menuItems)
            {
                string name = menuItem.SelectSingleNode("@name").Value;
                string fullName = baseName + name;
                XmlNode customMenuNode = 
                    menuItem.SelectSingleNode("CustomMenu");
                XmlNode commandNameNode = 
                    menuItem.SelectSingleNode("./Command/@name");
                UICommand command = null;
                if (commandNameNode != null)
                {
                    command = 
                        m_commandService.GetCommand(commandNameNode.Value);
                }
                if (commandNameNode != null || customMenuNode != null)
                {
                    if (command == null && customMenuNode != null)
                    {
                        command = 
                            m_commandService.GetCommand(
                            "CustomMenuNullCommand");
                    }
                    m_menuService.AddMenu(fullName, command);
                }
                //Find subitems off this menu
                XmlNodeList subList = menuItem.SelectNodes("MenuItem");
                if (subList != null)
                {
                    string subName = fullName + "##";
                    BuildFromNodeList(subList, subName);
                }
            }
        }

        [UICommandHandler("CustomMenuNullCommand")]
        public static void Null(object obj, EventArgs args)
        {
            //do nothing
        }

    }
}
