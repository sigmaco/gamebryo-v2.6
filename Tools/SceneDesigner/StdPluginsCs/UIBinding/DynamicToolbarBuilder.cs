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
using System.Collections.Specialized;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Resources;
using System.Windows.Forms;
using System.Xml;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
    /// <summary>
    /// Summary description for DyanmicToolbarBuilder.
    /// </summary>
    internal class DynamicToolbarBuilder : AbstractBuilder
    {
        #region Private Data
        private ServiceProvider m_provider;
        private IUICommandService m_commandService;
        private IToolBarService m_toolbarService;
        private IMessageService m_messageService;
        //Used to temporarily store the file name of the XML stream
        private string m_xmlFileName;
        #endregion

        public DynamicToolbarBuilder(ServiceProvider provider)
        {
            m_provider = provider;
            m_commandService = m_provider.GetService(typeof(IUICommandService))
                as IUICommandService;
            m_toolbarService = m_provider.GetService(typeof(IToolBarService))
                as IToolBarService;
            m_messageService = m_provider.GetService(typeof(IMessageService))
                as IMessageService;
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
                domObject.DocumentElement.SelectNodes("Toolbar");
            BuildFromNodeList(items, "");
        }

        private void BuildFromNodeList(XmlNodeList items, string s)
        {
            foreach (XmlNode toolbarNode in items)
            {
                string name = toolbarNode.SelectSingleNode("@name").Value;
                m_toolbarService.AddToolBar(name);
                XmlNodeList toolbarItems = toolbarNode.SelectNodes("./*");
                BuildToolbar(name, toolbarItems);
            }
        }

        private void BuildToolbar(string toolbarName, XmlNodeList items)
        {
            foreach (XmlNode item in items)
            {
                string element = item.Name;
                switch (element)
                {
                    case "Button":
                    {
                        BuildButton(item, toolbarName);
                        break;
                    }
                    default:
                    {
                        string messageText = "Invalid Tag in XML: " + element;
                        m_messageService.AddMessage(MessageChannelType.Errors,
                            messageText);
                        break;
                    }
                }
            }
        }


        private void BuildButton(XmlNode item, string toolbarName)
        {
            UICommand command = GetCommand(item.SelectSingleNode("Command"));
            Image tempImage = GetImage(item.SelectSingleNode("Image"));
            if (tempImage != null)
            {
                string buttonName = item.SelectSingleNode("@name").Value;
                m_toolbarService.AddToolBarButton(toolbarName, buttonName,
                    tempImage, command);
            }
        }

        private UICommand GetCommand(XmlNode node)
        {
            UICommand command = null;
            if (node != null)
            {
                string name = node.SelectSingleNode("@name").Value;
                command = m_commandService.GetCommand(name);
            }
            return command;
        }

        private Image GetImage(XmlNode imageNode)
        {
            Image retVal = null;
            //imageNode.Attributes.GetNamedItem("@file");
            XmlNode fileNameNode = imageNode.SelectSingleNode("File");
            XmlNode resourceNode = imageNode.SelectSingleNode("Resource");
            if (fileNameNode != null)
            {
                string fullPath;
                string fileName = fileNameNode.InnerText;
                if (m_xmlFileName != null)
                {
                    FileInfo info = new FileInfo(m_xmlFileName);
                    fullPath = info.DirectoryName + @"\" + fileName;
                }
                else
                {
                    fullPath = fileName;
                }
                try
                {
                    retVal = new Bitmap(fullPath);
                }
                catch (Exception e)
                {
                    Message msg = new Message();
                    msg.m_strText = "Bitmap resource not loaded:" + 
                        fullPath;
                    msg.m_strDetails = e.ToString();
                    m_messageService.AddMessage(MessageChannelType.Errors,
                        msg);
                    
                }
            } 
            else if (resourceNode != null)
            {
                string resourceName = resourceNode.InnerText;
                using (Stream stream = GetResourceStream(resourceName))
                {
                    if (stream != null)
                    {
                        retVal = new Bitmap(stream);
                    }
                    else
                    {
                        string messageText = "Bitmap resource not found:" + 
                            resourceName;
                        m_messageService.AddMessage(MessageChannelType.Errors,
                            messageText);
                    }
                }
            }
            if (retVal != null)
            {
                retVal = ConvertToScreenBitDepth(retVal);
            }
            return retVal;
        }

        /// <summary>
        /// Converts an Image object to use the current desktop's bit-dpeth
        /// </summary>
        /// <param name="originalbitmap"></param>
        /// <returns></returns>
        /// <remarks>
        /// inspired from:
        /// http://www.dotnet247.com/247reference/msgs/40/202528.aspx
        /// </remarks>
        private Image ConvertToScreenBitDepth(Image originalbitmap)
        {
            Image retVal;
            // Get the current desktop screen graphics.
            using (Graphics g = Graphics.FromHwnd(IntPtr.Zero))
            {
                // Make a bitmap using that color depth.
                retVal = 
                    new Bitmap(originalbitmap.Width, originalbitmap.Height, g);
            }
    
            // Start drawing on our new bitmap.
            using (Graphics g = Graphics.FromImage(retVal))
            {
                //Copy the loaded bitmap to the bitmap with proper screen 
                //depth.
                g.DrawImage(originalbitmap, 0, 0, retVal.Width, retVal.Height);
            }

            return retVal;
        }

        private Stream GetResourceStream(string name)
        {
            Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach(Assembly assembly in assemblies)
            {
                StringCollection resources = new StringCollection();
                resources.AddRange(assembly.GetManifestResourceNames());
                if (resources.Contains(name))
                {
                    return assembly.GetManifestResourceStream(name);
                }
            }
            return null;
        }
    }
}
