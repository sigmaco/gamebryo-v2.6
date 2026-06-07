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
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
    /// <summary>
    /// Summary description for DynamicShortCutBuilder.
    /// </summary>
    internal class DynamicShortcutBuilder : AbstractBuilder
    {
        #region Private Data
        private ServiceProvider m_provider;
        private IUICommandService m_commandService;
//        private IToolBarService m_toolbarService;
//        private IMessageService m_messageService;
        private IShortcutService m_shortcutService;
        //Used to temporarily store the file name of the XML stream
        private string m_xmlFileName;
        #endregion
        public DynamicShortcutBuilder(ServiceProvider provider)
        {
            m_provider = provider;
            m_commandService = m_provider.GetService(typeof(IUICommandService))
                as IUICommandService;
            m_shortcutService = m_provider.GetService(typeof(IShortcutService))
                as IShortcutService;
        }

        public override void Build(Stream xmlStream)
        {
            m_xmlFileName = null;
            //Debugger.Break();
            FileStream fStream = xmlStream as FileStream;
            if (fStream != null)
            {
                m_xmlFileName = fStream.Name;
            }
            XmlDocument domObject = ReadXML(xmlStream);
            XmlNodeList items = 
                domObject.DocumentElement.SelectNodes("Shortcut");
            BuildFromNodeList(items, "");

        }

        private void BuildFromNodeList(XmlNodeList items, string s)
        {
            foreach (XmlNode shortcutNode in items)
            {
                string shortcutName = 
                    shortcutNode.SelectSingleNode("@name").Value;
                string commandName =
                    shortcutNode.SelectSingleNode("Command/@name").Value;
                XmlNodeList keyNodes = shortcutNode.SelectNodes("Key");
                Keys key = Keys.None;
                foreach (XmlNode keyNode in keyNodes)
                {
                    string keyString = 
                        keyNode.SelectSingleNode("@name").Value;
                    Keys keyValue = (Keys) Enum.Parse(typeof(Keys), keyString);
                    key = key | keyValue;
                }
                UICommand command = m_commandService.GetCommand(commandName);
                m_shortcutService.RegisterShortcut(key, command);
            }
        }

        private void BuildShortcut(string name, XmlNodeList items)
        {
        }


    }
}
