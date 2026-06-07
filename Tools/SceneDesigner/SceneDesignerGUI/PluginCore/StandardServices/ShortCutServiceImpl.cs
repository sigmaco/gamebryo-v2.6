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
using System.Collections;
using System.Diagnostics;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for ShortCutServiceImpl.
    /// </summary>
    public class ShortcutServiceImpl : StandardServiceBase ,IShortcutService
    {

        #region Private Data
        Hashtable m_bindings;
        #endregion

        public ShortcutServiceImpl()
        {
            m_bindings = new Hashtable();
        }

        public void RegisterShortcut(Keys key, UICommand pmCommand)
        {
            if (pmCommand != null && key != Keys.None)
            {
                m_bindings[key] = pmCommand;
            }
        }

        public void UnregisterShortcut(Keys key)
        {
            m_bindings.Remove(key);
        }

        public string GetTextRepresentation(Keys key)
        {
            StringBuilder sb = new StringBuilder();
            if ((key & Keys.Control) == Keys.Control)
            {
                sb.Append("Ctrl+");
            }
            if ((key & Keys.Alt) == Keys.Alt)
            {
                sb.Append("Alt+");
            }
            if ((key & Keys.Shift) == Keys.Shift)
            {
                sb.Append("Shift+");
            }

            Keys basicKey = key & (Keys.KeyCode);
            sb.Append(basicKey.ToString());
            return sb.ToString();
        }

        public Keys GetKey(UICommand pmCommand)
        {
            foreach (DictionaryEntry binding in m_bindings)
            {
                if (binding.Value == pmCommand)
                {
                    return (Keys) binding.Key;
                }
            }
            return Keys.None;
        }

        internal bool HandleKey(Keys key)
        {
            if (m_bindings.Contains(key))
            {
                UICommand command = (UICommand) m_bindings[key];
                Debug.Assert(command != null, "Null command!");

                UIState state = new UIState();
                command.ValidateCommand(state);
                if (state.Enabled)
                {
                    command.DoClick(this, null);
                    return true;
                }
            }

            return false;
        }
    }
}
