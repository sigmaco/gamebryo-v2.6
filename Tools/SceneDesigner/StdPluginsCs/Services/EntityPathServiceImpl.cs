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
using System.Text;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Services
{
    /// <summary>
    /// Summary description for EntityPathServiceImpl.
    /// </summary>
    public class EntityPathServiceImpl : IEntityPathService
    {
        public EntityPathServiceImpl()
        {
        }

        #region IService Implementation

        public bool Initialize()
        {
            return true;
        }

        public bool Start()
        {
            return true;
        }

        public string Name
        {
            get { return this.GetType().ToString(); }
        }

        #endregion

        #region IEntityPathService Implementation

        public string GetSimpleName(string pmFullPath)
        {
            return PaletteUtilities.SimpleName(pmFullPath);
        }

        public string GetFullPath(MScene pmScene, MEntity pmEntity)
        {
            StringBuilder sb = new StringBuilder();
            if (pmScene != null)
            {
                sb.AppendFormat("[{0}]", pmScene.Name);
            }
            sb.Append(MPalette.StripPaletteName(pmEntity.Name));
            return sb.ToString();
        }

        public string FindFullPath(MEntity pmEntity)
        {
            MFramework fw = MFramework.Instance;

            //First, try the main scene
            MScene mainScene = fw.Scene;
            if (mainScene.IsEntityInScene(pmEntity))
            {
                return GetFullPath(mainScene, pmEntity);
            }

            MPalette containingPalette = FindPaletteContainingEntity(pmEntity);

            if (containingPalette != null)
            {
                return GetFullPath(containingPalette.Scene, pmEntity);
            }
            return pmEntity.Name;
        }

        public MPalette FindPaletteContainingEntity(MEntity pmEntity)
        {
            MFramework fw = MFramework.Instance;

            MPalette[] palettes = fw.PaletteManager.GetPalettes();

            foreach (MPalette palette in palettes)
            {
                if (palette.Scene.IsEntityInScene(pmEntity))
                {
                    return palette;
                }
            }
            return null;            
        }

        public MEntity FindEntity(string strFullPath)
        {
            MFramework fw = MFramework.Instance;
            //Check the main Scene first:
            MEntity sceneEntity = 
                fw.Scene.GetEntityByName(strFullPath);
            if (sceneEntity != null)
            {
                return sceneEntity;
            }
            else
            {
                MPalette[] palettes = fw.PaletteManager.GetPalettes();
                foreach(MPalette palette in palettes)
                {
                    MEntity paletteEntity =
                        palette.Scene.GetEntityByName(strFullPath);
                    if (paletteEntity != null)
                    {
                        return paletteEntity;
                    }
                }
            }
            return null;
        }


        #endregion
    }
}
