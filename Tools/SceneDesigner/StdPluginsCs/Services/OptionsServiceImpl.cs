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
using System.ComponentModel;
using System.Diagnostics;
using System.Text.RegularExpressions;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Services
{
    /// <summary>
    /// Summary description for OptionsServiceImpl.
    /// </summary>
    public class OptionsServiceImpl : IOptionsService
    {

        #region Private Data
        ArrayList m_options;
        ISettingsService m_settingsService;
        #endregion

        public OptionsServiceImpl()
        {
            m_options = new ArrayList();
        }

        private class Option
        {
            public string Name;
            public SettingsCategory SettingsStore;
            public string Setting;
//            public string DisplayCategory;
            public string HelpText;
            public TypeConverter CustomTypeConverter;
            public object CustomTypeEditor;
        }

        private ISettingsService SettingService
        {
            get
            {
                if (m_settingsService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_settingsService = sp.GetService(typeof(ISettingsService))
                        as ISettingsService;
                }
                return m_settingsService;
            }
        }


        #region IService Implementation

        public bool Initialize()
        {
            return true;
        }

        public bool Start()
        {
            return true;
        }

        public string Name
        {
            get { return this.GetType().ToString(); }
        }
        #endregion

        #region IOptionsService Implementation

        public void AddOption(string name, SettingsCategory eCategory,
            string strSetting)
        {
            Option newOption = new Option();
            newOption.Name = name;
            newOption.SettingsStore = eCategory;
            newOption.Setting = strSetting;
            Option existingOption = null;
            foreach (Option option in m_options)
            {
                if (option.SettingsStore == eCategory &&
                    option.Setting.Equals(strSetting) )
                {
                    existingOption = option;
                    break;
                }
            }
            if (existingOption == null)
            {
                m_options.Add(newOption);
            }
        }

        public void SetHelpDescription(string optionName, string helpText)
        {
            Option option = FindOption(optionName);
            Debug.Assert(option != null);
            if (option != null)
            {
                option.HelpText = helpText;
            }
        }

        public string GetHelpDescription(string optionName)
        {
            Option option = FindOption(optionName);
            Debug.Assert(option != null);
            if (option != null)
            {
                return option.HelpText;
            }
            return null;
        }
        
        public void SetTypeConverter(string strOptionName, 
            TypeConverter pmTypeConverter)
        {
            Option option = FindOption(strOptionName);
            Debug.Assert(option != null);
            if (option != null)
            {
                option.CustomTypeConverter = pmTypeConverter;
            }
        }
        
        
        
        public TypeConverter GetTypeConverter(string strOptionName)
        {            
            Option option = FindOption(strOptionName);
            Debug.Assert(option != null);
            if (option != null)
            {
                return option.CustomTypeConverter;
            }
            return null;
        }

        public void SetTypeEditor(string strOptionName, object pmEditor)
        {
            Option option = FindOption(strOptionName);
            Debug.Assert(option != null);
            if (option != null)
            {
                option.CustomTypeEditor = pmEditor;
            }
        }

        public object GetTypeEditor(string strOptionName)
        {
            Option option = FindOption(strOptionName);
            Debug.Assert(option != null);
            if (option != null)
            {
                return option.CustomTypeEditor;
            }
            return null;            
        }

        public string[] GetOptions(string strOptionCategory, 
            SettingsCategory eCategory)
        {
            ArrayList retVal = new ArrayList();
            string[] optionsInCategory = GetOptions(eCategory);
            foreach (string option in optionsInCategory)
            {
                string optionCategory = OptionUtility.StripOption(option);
                if (optionCategory.Equals(strOptionCategory))
                {
                    retVal.Add(option);
                }
            }
            return retVal.ToArray(typeof(string)) as string[];
        }

        public string[] GetOptions(SettingsCategory eCategory)
        {
            ArrayList optionsInCategory = new ArrayList();
            foreach (Option option in m_options)
            {
                if (GetSettingsCategory(option.Name) ==
                    eCategory)
                {
                    optionsInCategory.Add(option.Name);
                }
            }
            return optionsInCategory.ToArray(typeof(string)) as string[];
        }

        public object GetSettingsObject(string optionName)
        {
            Option option = FindOption(optionName);
            if (option != null)
            {
                return SettingService.GetSettingsObject(option.Setting,
                    option.SettingsStore);
            }
            return null;
        }

        public string GetSettingsName(string optionName)
        {
            Option option = FindOption(optionName);
            if (option != null)
            {
                return option.Setting;
            }
            return null;
        }

        public SettingsCategory GetSettingsCategory(string optionName)
        {
            Option option = FindOption(optionName);
            if (option != null)
            {
                return option.SettingsStore;
            }
            return SettingsCategory.None;
        }

        private Option FindOption(string name)
        {
            foreach (Option option in m_options)
            {
                if (option.Name.Equals(name))
                {
                    return option;
                }
            }
            return null;
        }

        #endregion
    }

    //NOTE this class contains code that was cut and pasted from
    //the Options dialog in the Gui project
    //The options implementation class should probably be moved to the gui
    //project, since it is very closely integrated with the gui....

    public class OptionUtility
    {
        /// <summary>
        /// Returns the name of the option without the category prefixes
        /// e.g., an input of "advanced.user.inputmode" returns 
        /// "inputmode"
        /// </summary>
        /// <param name="option"></param>
        /// <returns></returns>
        public static string StripCategories(string option)
        {
            //finds word at end of line that follows zero or more dots.
            Regex regex = new Regex("(?<=[.])?[^.]+$");
            return regex.Match(option).Value;
        }

        /// <summary>
        /// Similar to <c>StripCategories</c> 
        /// except it returns the opposite portion of the option
        /// e.g., an input of "advanced.user.inputmode" returns 
        /// "advanced.user"
        /// </summary>
        /// <param name="option"></param>
        /// <returns></returns>
        public static string StripOption(string option)
        {
            //return all charaters up until but not including the last 
            //occuring dot.
            Regex regex = new Regex(".+(?=[.])");
            return regex.Match(option).Value;            
        }
        
    }
}
