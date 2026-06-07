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
using System.Windows.Forms;
using Microsoft.Win32;
using System.Reflection;
using System.IO;
using System.Collections;
using System.Diagnostics;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for Class1.
    /// </summary>
    public class HelpManager
    {
        #region Data Members
        static private HelpManager ms_kThis = null;

        static public HelpManager Instance
        {
            get {return ms_kThis;}
        }

        private string m_strHelpPath;
        private string m_strBaseAnimationToolHelpPath;
        #endregion

        #region Constructors

        private HelpManager(string strURL, string strBasePage)
        {
            if (!File.Exists(strURL))
                strURL = null;

            m_strHelpPath = strURL;
            m_strBaseAnimationToolHelpPath = strBasePage;
        }

        public static HelpManager Create(string strURL, string strBasePage)
        {
            if (ms_kThis != null)
                return ms_kThis; // already created.

            ms_kThis = new HelpManager(strURL, strBasePage);
            return ms_kThis;
        }
        #endregion

        static public bool HelpExists()
        {
            return Instance.m_strHelpPath != null;
        }

        #region DoHelp

        public static void DoHelp(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(kControl, Instance.m_strHelpPath,
                    HelpNavigator.Topic, 
                    Instance.m_strBaseAnimationToolHelpPath + strSubPage);
            }
            catch (Exception e)
            {
                MLogger.LogDebug("Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        public static void DoHelpAndTOC(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath,
                    HelpNavigator.TableOfContents);
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                   Instance.m_strBaseAnimationToolHelpPath + strSubPage);
            }
            catch (Exception e)
            {
                MLogger.LogDebug("Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        public static void DoHelpAndIndex(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                    Instance.m_strBaseAnimationToolHelpPath + strSubPage);
                Help.ShowHelpIndex(kControl, Instance.m_strHelpPath);
            }
            catch (Exception e)
            {
                MLogger.LogDebug("Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        public static void DoHelpAndSearch(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                    Instance.m_strBaseAnimationToolHelpPath + strSubPage);
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Find, "");
            }
            catch (Exception e)
            {
                MLogger.LogDebug("Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        #endregion
    }
}
