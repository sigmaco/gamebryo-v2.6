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

using System.Collections;
using System.Windows.Forms;
//using Reflector.UserInterface;
using WeifenLuo.WinFormsUI;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for WinFormsUtility.
    /// </summary>
    public class WinFormsUtility
    {

        #region Private Data
        private static Form m_mainForm;
        private static DockPanel m_dockPanel;
        #endregion
        

        public static Form MainForm
        {
            get { return m_mainForm; }
            set { m_mainForm = value; }
        }


        public static DockPanel DockPanel
        {
            get { return m_dockPanel; }
            set { m_dockPanel = value; }
        }

//        internal static EventHandler EnitityChanged
//        {
//            get { return m_entityChangedEvent; }
//            set { m_entityChangedEvent = value; }
//        }

        private WinFormsUtility() // utility (static) class
        {
        }

//        public static ArrayList Panels
//        {
//            get{ return m_widgets; }
//                
//            set { m_widgets = value; }
//        }

    }
}
