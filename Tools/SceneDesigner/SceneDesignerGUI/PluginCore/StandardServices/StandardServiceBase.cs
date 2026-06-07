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

using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for StandardServiceBase.
    /// </summary>
    public class StandardServiceBase : IService
    {
        public virtual string Name
        {
            get { return this.GetType().ToString(); }
        }

        public virtual bool Initialize()
        {
            return true;
        }

        public virtual bool Start()
        {
            ServiceProvider sp = ServiceProvider.Instance;
            IUICommandService commandSrvc = 
                sp.GetService(typeof(IUICommandService))
                as IUICommandService;
            commandSrvc.BindCommands(this);
            return true;
        }

    }
}
