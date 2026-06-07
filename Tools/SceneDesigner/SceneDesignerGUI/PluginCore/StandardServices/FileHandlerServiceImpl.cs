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
// http://www.emergent.netusing System;

using System;
using System.Collections;
using System.IO;
using System.Text;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    class FileHandlerServiceImpl : StandardServiceBase, IFileHandlerService
    {
        #region Private Data
        private Hashtable m_pmFileLoadHandlers;
        
        #endregion
        
        public FileHandlerServiceImpl()
        {
            m_pmFileLoadHandlers = new Hashtable();
        }

        public bool RegisterFileLoader(
            string strExtension, FileLoadHandler pmCallBack)
        {
            string adjustedExtension = strExtension.ToUpper();
            if (!adjustedExtension.StartsWith("."))
            {
                adjustedExtension = "." + adjustedExtension;
            }
            FileLoadHandler existingHandler =
                m_pmFileLoadHandlers[adjustedExtension] as FileLoadHandler;
            if (existingHandler != null)
            {
                IMessageService messageService =
                    ServiceProvider.Instance.GetService(
                        typeof(IMessageService)) as IMessageService;
                string msgText =
                    "An attempt was made to register a file load " +
                    "handler twice.";
                string msgDetails = string.Format(
                    "The file extension: \'{0}\' already has a registered" +
                    "loader at \n {1}", adjustedExtension,
                    existingHandler.GetInvocationList()[0].Method.ToString());
                
                Message msg = new Message(msgText, msgDetails, null );
                
                messageService.AddMessage(MessageChannelType.Errors, msg );

                return false;
                
            }
            m_pmFileLoadHandlers[adjustedExtension] = pmCallBack;
            return true;
        }

        public bool LoadFile(string pmFileName)
        {
            string extension = Path.GetExtension(pmFileName).ToUpper();
            FileLoadHandler handler =
                m_pmFileLoadHandlers[extension] as FileLoadHandler;
            if (handler != null)
            {
                handler(this, pmFileName);
                return true;
            }
            else
            {
                IMessageService messageService =
                    ServiceProvider.Instance.GetService(
                        typeof(IMessageService)) as IMessageService;
                string msgText =
                    "An attempt was made to load a file " +
                    "for which there is no registered handler";
                string msgDetails = string.Format(
                    "The file extension: \'{0}\' is unknown for file: \n" +
                    "{1}", extension, pmFileName);

                Message msg = new Message(msgText, msgDetails, null);
                messageService.AddMessage(MessageChannelType.Errors, msg);
                
                return false;
            }            
        }
    }
}
