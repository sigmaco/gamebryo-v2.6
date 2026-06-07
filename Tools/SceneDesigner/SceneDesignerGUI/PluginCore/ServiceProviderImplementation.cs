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
using Emergent.Gamebryo.SceneDesigner.PluginAPI;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore
{
    /// <summary>
    /// Summary description for ServiceProvidor.
    /// </summary>
    public class ServiceProviderImplementation : ServiceProvider
    {
        #region Private Data
        private bool m_bAllowAccessToServices = false;
        private Hashtable m_services;
        #endregion

        private ServiceProviderImplementation()
        {
            m_services = new Hashtable();
        }

        internal static new ServiceProviderImplementation Instance
        {
            get
            {
                if (ms_pmThis == null)
                {
                    ms_pmThis = new ServiceProviderImplementation();
                }
                return (ServiceProviderImplementation) ms_pmThis;
            }
        }

        public override void Shutdown()
        {
            foreach (object obj in m_services.Values)
            {
                IDisposable disposable = obj as IDisposable;
                if (disposable != null)
                {
                    disposable.Dispose();
                }
            }
            m_services.Clear();
        }

        private void command_Click(object sender, EventArgs e)
        {

        }


        public override IService GetService(Type serviceType)
        {
            if (m_bAllowAccessToServices)
            {
                return GetServiceInstance(serviceType);
            }
            else
            {
                throw new NotSupportedException("A service was accessed " +
                    "before all services have been intialized.\nPlease " +
                    "move code to the service or plugin's Start method");
            }

        }

        private IService GetServiceInstance(Type serviceType)
        {
            foreach(IService service in m_services.Values)
            {
                if (serviceType.IsAssignableFrom(service.GetType()))
                {
                    return service;
                }
            }
            return null;
        }


        internal void AddService(Type serviceType, object serviceInstance)
        {
            object priorService = GetServiceInstance(serviceType);
            if (priorService == null)
            {
                m_services.Add(serviceType, serviceInstance);
            }
            else
            {
                throw new 
                    ArgumentException(
                    "An attempt was made to add a service that already existed"
                    , "serviceType");
            }
        }

        internal bool IntializeServices()
        {
            foreach (IService service in m_services.Values)
            {
                service.Initialize();
            }
            return true;
        }

        public bool StartServices()
        {
            m_bAllowAccessToServices = true;
            foreach (IService service in m_services.Values)
            {
                service.Start();
            }
            return true;
        }

    }
}
