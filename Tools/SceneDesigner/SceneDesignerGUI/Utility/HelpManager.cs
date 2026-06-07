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
using System.IO;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for Class1.
    /// </summary>
    public class HelpManager
    {
        #region Data Members
        private IMessageService m_msgService;

        private string m_strHelpPath;
        private string m_strBaseAnimationToolHelpPath;
        private static HelpManager m_instance;

        #endregion

        static public HelpManager Instance
        {
            get
            {
                if (m_instance == null)
                {
                    m_instance = new HelpManager();
                }
                return m_instance;
            }
        }

        #region Constructors
        private HelpManager()
        {
            
        }

        #endregion

        #region Properties

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

        #endregion

        public void Init(string strURL, string strBasePage)
        {
            m_strHelpPath = strURL;
            m_strBaseAnimationToolHelpPath = strBasePage;
            ServiceProvider sp = ServiceProvider.Instance;

            IUICommandService commandService =  sp.GetService(
                typeof(IUICommandService)) as IUICommandService;

            commandService.BindCommands(this);

        }

        public bool HelpExists()
        {
            return Instance.m_strHelpPath != null;
        }

        #region DoHelp

        public void DoHelp(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                    m_strBaseAnimationToolHelpPath + strSubPage);
            }
            catch (Exception e)
            {
                MessageService.AddMessage(MessageChannelType.Errors,
                    "Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        public void DoHelpAndTOC(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath,
                    HelpNavigator.TableOfContents);
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                   m_strBaseAnimationToolHelpPath + strSubPage);
            }
            catch (Exception e)
            {
                MessageService.AddMessage(MessageChannelType.Errors,
                    "Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        public void DoHelpAndIndex(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                    m_strBaseAnimationToolHelpPath + strSubPage);
                Help.ShowHelpIndex(kControl, Instance.m_strHelpPath);
            }
            catch (Exception e)
            {
                MessageService.AddMessage(MessageChannelType.Errors,
                    "Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        public void DoHelpAndSearch(Control kControl, string strSubPage)
        {
            try 
            {
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Topic, 
                    m_strBaseAnimationToolHelpPath + strSubPage);
                Help.ShowHelp(
                    kControl, Instance.m_strHelpPath, HelpNavigator.Find, "");
            }
            catch (Exception e)
            {
                MessageService.AddMessage(MessageChannelType.Errors,
                    "Exception occurred in HelpManager.DoHelp: " +
                    e.Message);
            }
        }

        #endregion

        #region UICommandHandlers

        [UICommandHandler("HelpContents")]
        private void OnHelpContents(object sender, EventArgs args)
        {
            DoHelpAndTOC(Form.ActiveForm,
                "Introduction_to_Scene_Designer.htm");
        }

        [UICommandHandler("HelpIndex")]
        private void OnHelpIndex(object sender, EventArgs args)
        {
            DoHelpAndIndex(Form.ActiveForm,
                "Introduction_to_Scene_Designer.htm");
            
        }

        [UICommandHandler("HelpSearch")]
        private void OnHelpSearch(object sender, EventArgs args)
        {
            DoHelpAndSearch(Form.ActiveForm,
                "Introduction_to_Scene_Designer.htm");
            
        }

        [UICommandHandler("HelpAbout")]
        private void OnHelpAbout(object sender, EventArgs args)
        {
            AboutBoxForm frm = new AboutBoxForm();
            frm.ShowDialog();
        }


        #endregion
    }
}
