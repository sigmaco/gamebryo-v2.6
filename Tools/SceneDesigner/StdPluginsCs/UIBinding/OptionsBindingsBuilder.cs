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
using System.Xml;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
    /// <summary>
    /// Summary description for OptionsBindingsBuilder.
    /// </summary>
    internal class OptionsBindingsBuilder : AbstractBuilder
    {
        #region Private Data
        private ServiceProvider m_provider;
        private IUICommandService m_commandService;
        private IOptionsService m_optionsService;
        #endregion

        public OptionsBindingsBuilder()
        {
            m_provider = ServiceProvider.Instance;
            m_commandService = m_provider.GetService(typeof(IUICommandService))
                as IUICommandService;
            m_optionsService = m_provider.GetService(typeof(IOptionsService))
                as IOptionsService;
        }

        public override void Build(Stream xmlStream)
        {
            XmlDocument domObject = ReadXML(xmlStream);
            XmlNodeList items = 
                domObject.DocumentElement.SelectNodes("Category");
            BuildFromNodeList(items, "");

        }

        private void BuildFromNodeList(XmlNodeList items, string rootCategory)
        {
            foreach (XmlNode node in items)
            {
                string categoryName = rootCategory 
                    + node.SelectSingleNode("@name").Value;
                XmlNodeList optionList = node.SelectNodes("Option");
                if (optionList != null)
                {
                    foreach(XmlNode option in optionList)
                    {
                        string optionName = 
                            categoryName + "." + 
                            option.SelectSingleNode(@"@name").Value;
                        XmlNode settingNode = 
                            option.SelectSingleNode(@"SettingsObject");
                        string storeName = 
                            settingNode.SelectSingleNode(@"@store").Value;
                        string settingName =
                            settingNode.SelectSingleNode(@"@name").Value;
                        SettingsCategory settingCategory = (SettingsCategory) 
                            Enum.Parse(typeof(SettingsCategory), storeName);

                        XmlNode settingHelpNode = 
                            option.SelectSingleNode(@"HelpText");                            

                        m_optionsService.AddOption(optionName, settingCategory,
                            settingName);

                        if (settingHelpNode != null)
                        {
                            m_optionsService.SetHelpDescription(optionName,
                                settingHelpNode.InnerText);
                        }

                    }
                }
                XmlNodeList subList = node.SelectNodes("Category");
                if (subList != null)
                {
                    string subName = categoryName + ".";
                    BuildFromNodeList(subList, subName);
                }
            }
        }
    }
}
