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
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary> 
    /// Summary description for ComponentManagementUtilities.
    /// </summary>
    public class ComponentManagementUtilities
    {

        #region Private Data
        #endregion

        private ComponentManagementUtilities()
        {
        }

        private static MFramework FW
        {
            get
            {
                return MFramework.Instance;
            }
        }

        public static void BuildComponentListFromPalettes(MPalette[] palettes)
        {
            foreach (MPalette palette in palettes)
            {
                BuildComponentListFromScene(palette.Scene);
            }
        }

        public static void BuildComponentListFromScene(MScene scene)
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IComponentService componentService = 
                sp.GetService(typeof(IComponentService)) as IComponentService;
            MEntity[] entities = scene.GetEntities();
            foreach(MEntity entity in entities)
            {
                MComponent[] components = entity.GetComponents();
                foreach(MComponent component in components)
                {
                    MComponent existingComponent = 
                        componentService.GetComponentByID(
                        component.TemplateID);
                    if (existingComponent == null)
                    {                     
                        MComponent newComponent = 
                            component.Clone(false);
                        newComponent.MasterComponent = null;
                        componentService.RegisterComponent(
                            newComponent);
                    }
                    else
                    {
                        if (!existingComponent.Name.Equals(component.Name))
                        {
                            string message = component.Name + " and " 
                                + existingComponent.Name + "are the same...";
                            IMessageService msgService = 
                                ServiceProvider.Instance.GetService(
                                typeof(IMessageService)) as IMessageService;
                            msgService.AddMessage(MessageChannelType.General, 
                                message);
                        }
                    }
                }
            }
        }

        public static void RemoveRegisteredcomponentsInScene(MScene scene)
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IComponentService componentService = 
                sp.GetService(typeof(IComponentService)) as IComponentService;
            MEntity[] entities = scene.GetEntities();
            foreach(MEntity entity in entities)
            {
                MComponent[] components = entity.GetComponents();
                foreach(MComponent component in components)
                {
                    MComponent existingComponent = 
                        componentService.GetComponentByID(
                        component.TemplateID);
                    if (existingComponent != null)                    
                    {   
                        if (existingComponent.IsAddOrRemovePropertySupported())
                        {
                            componentService.
                                UnregisterComponent(existingComponent);
                        }
                    }
                }
            }
            
        }
    }
}
