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
using Emergent.Gamebryo.SceneDesigner.PluginAPI;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility
{
    /// <summary>
    /// Summary description for AnimationControlUtility.
    /// </summary>
    public class AnimationControlUtility
    {
        public AnimationControlUtility()
        {
        }
        
        [UICommandHandler("PlayAnimations")]
        private static void OnPlayAnimations(object sender, EventArgs args)
        {
            MFramework.Instance.TimeManager.Enabled =
                !MFramework.Instance.TimeManager.Enabled;
        }
        
        [UICommandValidator("PlayAnimations")]
        private static void OnValidatePlayAnimations(object sender, 
            UIState state)
        {
            state.Checked = MFramework.Instance.TimeManager.Enabled;
        }
        
        [UICommandHandler("ResetAnimations")]
        private static void OnResetAnimations(object sender, EventArgs args)
        {
            MFramework.Instance.RestartAnimation();
        }
        
        [UICommandValidator("ResetAnimations")]
        private static void OnValidateResetAnimations(object sender, 
            UIState state)
        {
            
        }
    }
}
