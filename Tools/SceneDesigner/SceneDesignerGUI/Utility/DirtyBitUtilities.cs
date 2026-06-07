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
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for DirtyBitUtilitys.
    /// </summary>
    public class DirtyBitUtilities
    {
        #region Private Data

        #endregion

        public DirtyBitUtilities()
        {
        }

        public static void Init()
        {
            
        }

        private static void OnSettingChanged(object pmSender, 
            SettingChangedEventArgs pmEventArgs)
        {
            MFramework.Instance.Scene.Dirty = true;
        }

        public static void MakeSceneClean(MScene pmScene)
        {
            pmScene.Dirty = false;
            MEntity[] entities = pmScene.GetEntities();
            foreach(MEntity entity in entities)
            {
                entity.Dirty = false;
            }
        }

    }
}
