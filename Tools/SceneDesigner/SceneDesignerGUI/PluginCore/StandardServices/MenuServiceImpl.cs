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
using System.Collections.Specialized;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for MenuServiceImpl.
    /// </summary>
    public class MenuServiceImpl : StandardServiceBase , IMenuService
    {
        #region Private Data
        private Menu m_mainMenu;
        private ArrayList m_bindings;
        private IMessageService m_msgService;
        private ShortcutServiceImpl m_shortcutService;
        #endregion

        public MenuServiceImpl(Menu mainMenu)
        {
            m_mainMenu = mainMenu;
            //Set up the menu validator
            //Application.Idle += new EventHandler(Application_Idle);
            m_bindings = new ArrayList();
        }

        private class MenuBinding
        {
            public MenuItem Item;
            public UICommand Command;
            public string Name;
            //public bool IsCustom;
        }

        private class LongestToShortestString : IComparer
        {
            #region IComparer Members

            public int Compare(object x, object y)
            {
                string xStr = x.ToString();
                string yStr = y.ToString();
                return yStr.Length - xStr.Length;
            }

            #endregion

        }


        private IMessageService MessageService
        {
            get
            {
                if (m_msgService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_msgService = sp.GetService(typeof(IMessageService))
                        as IMessageService;
                }
                return m_msgService;
            }
        }

        private ShortcutServiceImpl ShortcutService
        {
            get
            {
                if (m_shortcutService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_shortcutService = sp.GetService(typeof(IShortcutService))
                        as ShortcutServiceImpl;
                }
                return m_shortcutService;
            }
        }


        public void AddMenu(string menu, UICommand command)
        {
            string[] menus = SplitMenus(menu);
            
            Menu.MenuItemCollection currentItems = m_mainMenu.MenuItems;
            for (int i = 0; i < menus.Length; i++)
            {
                string menuName = StripMenuOrder(menus[i]);
                int sortOrder = GetMenuOrder(menus[i]);

                MenuItem existingItem = null;
                if (!menuName.Equals("-"))
                {
                    existingItem = FindMenu(currentItems, menuName);
                }
                MenuItem item;
                if ( existingItem != null)
                {
                    item = existingItem;
                    if (sortOrder != -1)
                    {
                        item.MergeOrder = sortOrder;
                    }
                }
                else
                {
                    item = currentItems.Add(menuName);   
                    item.MergeOrder = sortOrder;
                    if (sortOrder != -1)
                    {
                        SortMenuItems(currentItems);
                    }
                    //currentItems.Add()
                }
                if (i == (menus.Length - 1))
                {
                    if (command != null)
                    {
                        item.Click += new EventHandler(command.DoClick);
                    }
                    MenuBinding binding = new MenuBinding();
                    binding.Item = item;
                    binding.Command = command;
                    binding.Name = menuName;
                    m_bindings.Add(binding);
                }
                currentItems = item.MenuItems;
                
            }
        }

        /// <summary>
        /// Gets the plain menu name, striping the merge order indicator '[x]'
        /// the may prefix a menu's name
        /// </summary>
        /// <param name="menuName"></param>
        /// <returns></returns>        
        /// <remarks>
        /// For an input string "[4]File"
        /// this would return "File"
        /// </remarks>
        private static string StripMenuOrder(string menuName)
        {
            Regex regex = new Regex(@"(?<=\[(\-)?[0-9]+\])+.+");
            Match match = regex.Match(menuName);
            if (match.Success)
            {
                return match.Value;
            }
            else
            {
                return menuName;
            }
        }

        private static int GetMenuOrder(string menuName)
        {
            Regex regex = new Regex(@"(?<=[\[])[\-]?[0-9]+(?=[\]])");
            Match match = regex.Match(menuName);
            if (match.Success)
            {
                return int.Parse(match.Value);
            }
            else
            {
                return -1;
            }
            
        }

        private void SortMenuItems(Menu.MenuItemCollection items)
        {
            ArrayList tempList = new ArrayList(items);
            tempList.Sort(new ItemSorter());
            items.Clear();
            foreach (MenuItem item in tempList)
            {
                items.Add(item);
            }
        }

        private class ItemSorter : IComparer
        {
            private const int First = -1;
            private const int Same = 0;
            private const int Last = 1;
            public int Compare(object x, object y)
            {
                MenuItem menuItemX = x as MenuItem;
                MenuItem menuItemY = y as MenuItem;
                if (menuItemX.MergeOrder == menuItemY.MergeOrder)
                {
                    return Same;
                }
                else if (menuItemX.MergeOrder == -1)
                {
                    return Last;
                }
                else if (menuItemY.MergeOrder == -1)
                {
                    return First;
                }
                else if (menuItemX.MergeOrder < menuItemY.MergeOrder)
                {
                    return First;
                }
                else
                {
                    return Last;
                }
            }
        }

        public void RemoveMenu(string menu, UICommand command, bool pruneEmtpy)
        {
            //RemoveIndividualMenuItem will actually do the work of 
            //removing the menus. This method simply builds the list of menus
            //to remove
            string[] menus = SplitMenus(menu);
            string[] fullNames = new string[menus.Length];
            for (int i = 0; i < menus.Length; i++)
            {
                StringBuilder sb = new StringBuilder();
                for ( int j = 0; j <= i; j++)
                {
                    sb.Append(menus[j]);
                    if ( (i > 0) && (j != i) )
                    {
                        sb.Append("##");
                    }
                }
                fullNames[i] = sb.ToString();
            }
            ArrayList removalList = new ArrayList(fullNames);

            //sort the names from longest to shortest
            removalList.Sort(new LongestToShortestString());

            for ( int i = 0; i < removalList.Count; i++)
            {
                string name = removalList[i] as string;
                if (i == 0)
                {
                    RemoveIndividualMenuItem(name, command);
                }
                else
                {
                    if (pruneEmtpy)
                    {
                        if (IsMenuEmpty(name))
                        {
                            RemoveIndividualMenuItem(name, null);
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            
        }


        private void RemoveIndividualMenuItem(string fullName, 
            UICommand command)
        {
            string[] menus = SplitMenus(fullName);

            Menu.MenuItemCollection currentItems = m_mainMenu.MenuItems;
            for (int i = 0; i < menus.Length; i++)
            {
                string menuName = menus[i];
                MenuItem existingItem = FindMenu(currentItems, menuName);
                MenuItem item;
                if ( existingItem != null)
                {
                    item = existingItem;
                }
                else
                {
                    Message msg = new Message( "Could not remove menu " 
                        + fullName,  
                        menuName + " could not be found", null);
                    MessageService.AddMessage(MessageChannelType.Errors, msg);
                    return;
                }
                if (i == (menus.Length - 1))
                {
                    if (command != null)
                    {
                        item.Click -= new EventHandler(command.DoClick);
                    }
                    RemoveItemFromBindings(item);
                    currentItems.Remove(item);
                    return;
                }
                currentItems = item.MenuItems;
            }

        }

        private void RemoveItemFromBindings(MenuItem item)
        {
            for ( int i = 0; i < m_bindings.Count; i++)
            {
                MenuBinding binding = m_bindings[i] as MenuBinding;
                if (binding.Item == item)
                {
                    m_bindings.Remove(binding);
                    return;
                }
            }
        }



        public bool IsMenuEnabled(string menu)
        {
            MenuItem item = FindMenu(m_mainMenu.MenuItems, menu);
            if (item != null)
            {
                return item.Enabled;
            }
            return false;
        }

        public void EnableMenu(string menu, bool enable)
        {
            MenuItem item = FindMenu(m_mainMenu.MenuItems, menu);
            if (item != null)
            {
                item.Enabled = enable;
            }
        }

        public string[] GetAssignedMenus(UICommand pmCommand)
        {
            ArrayList menus = new ArrayList();
            for ( int i = 0; i < m_bindings.Count; i++)
            {
                MenuBinding binding = m_bindings[i] as MenuBinding;
                if (binding.Command.Equals(pmCommand))
                {
                    menus.Add(binding.Name);
                }
            }
            return menus.ToArray(typeof(string)) as string[];
        }

        internal void AnnotateMenus()
        {
            ArrayList menus = new ArrayList();

            foreach (MenuBinding binding in m_bindings)
            {
                Keys key  = ShortcutService.GetKey(binding.Command);
                if (key != Keys.None)
                {
                    string shortcutText = 
                        ShortcutService.GetTextRepresentation(key);
                    string originalText = 
                        binding.Item.Text;
                    string newText = string.Format("{0} \t ({1})",
                        originalText, shortcutText);
                    binding.Item.Text = newText;
                }
            }
        }

        private static MenuItem FindMenu(Menu.MenuItemCollection collection, 
            string name)
        {
            foreach (MenuItem menuItem in collection)
            {
                if (menuItem.Text == name)
                {
                    return menuItem;
                }
            }
            return null;
        }

        private static MenuItem FullMenuSearch(
            Menu.MenuItemCollection collection, string fullName)
        {
            MenuItem retVal = null;
            string[] menus = SplitMenus(fullName);
            MenuItem.MenuItemCollection currentCollection = collection;
            foreach(string name in menus)
            {
                retVal = FindMenu(currentCollection, name);
                if (retVal != null)
                {
                    currentCollection = retVal.MenuItems;
                }
            }
            return retVal;
        }

        private bool IsMenuEmpty(string fullName)
        {
            MenuItem item = FullMenuSearch(m_mainMenu.MenuItems, fullName);
            if (item != null)
            {
                return ((item.MenuItems == null) ||
                    (item.MenuItems.Count == 0));
            }
            return false;
        }


        private static string[] SplitMenus(string menuName)
        {
            Regex regex = new Regex("##");//finds isolated double pounds
            return regex.Split(menuName);
        }

        [UICommandHandler("Idle")]
        private void Application_Idle(object sender, EventArgs e)
        {
            foreach (MenuBinding binding in m_bindings)
            {
                UIState state = new UIState();
                if (binding.Command != null)
                {
                    binding.Command.ValidateCommand(state);
                    binding.Item.Enabled = state.Enabled;
                    binding.Item.Checked = state.Checked;
                }
                else
                {
                    binding.Item.Enabled = false;
                }
            }
        }
    }
}
