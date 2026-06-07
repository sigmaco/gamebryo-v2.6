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
using System.ComponentModel;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for EntityManagementUtilities.
    /// </summary>
    public class EntityManagementUtilities
    {

        public static MPalette ResolveMasterEntities(MScene sceneToResolve,
            MPalette[] palettes)
        {
            MPalette orphanPalette = new MPalette("Unnamed", 0);
            ArrayList sceneArray = new ArrayList();
            foreach (MPalette palette in palettes)
            {
                sceneArray.Add(palette.Scene);
            }
            MEntity[] orphanEntities = ResolveMasterEntities(sceneToResolve,
                sceneArray.ToArray(typeof(MScene)) as MScene[]);
            foreach (MEntity orphan in orphanEntities)
            {
                string paletteName = SceneManagementUtilities.ParsePaletteName(
                    orphan.Name);
                if (!string.IsNullOrEmpty(paletteName))
                {
                    orphanPalette.AddEntity(orphan, string.Empty, false);
                }
            }
            return orphanPalette;
        }

        public static MEntity[] ResolveMasterEntities(MScene sceneToResolve,
            MScene[] masterEntityScenes)
        {
            ArrayList orphanEntityList = new ArrayList();
            MEntity[] sceneFlatEntities = sceneToResolve.GetEntities();
            
            foreach(MEntity sceneEntity in sceneFlatEntities)
            {
                MEntity actualEntity = sceneEntity;
                if (actualEntity.MasterEntity != null)
                {
                    while (actualEntity.MasterEntity.MasterEntity != null)
                    {
                        actualEntity = actualEntity.MasterEntity;
                    }

                    string masterEntitySceneName = null;
                    MEntity master = FindByID(
                        actualEntity.MasterEntity.TemplateID, 
                        masterEntityScenes, out masterEntitySceneName);


                    if (master == null)
                    {
                        //check if not already added...
                        master = FindByID(actualEntity.MasterEntity.TemplateID,
                            orphanEntityList);
                    }
                    if (master == null)
                    {
                        //this entity is "Orphaned" add it to the return list
                        orphanEntityList.Add(actualEntity.MasterEntity);
                    }
                    else if (actualEntity.MasterEntity != master)
                    {
                        MFramework.Instance.EntityFactory.Remove(
                            actualEntity.MasterEntity);
                        actualEntity.MasterEntity = master;
                    }
                }
            }
            return orphanEntityList.ToArray(typeof(MEntity))
                as MEntity[];
        }


        internal static MEntity FindByID(Guid id, MScene[] scenes, 
            out string sceneName)
        {
            sceneName = null;
            foreach(MScene scene in scenes)
            {
                //Note that we do NOT want the flat list,
                //we're only searching the top level
                MEntity[] sceneEntities = scene.GetEntities();
                foreach(MEntity entity in sceneEntities)
                {
                    if (entity.TemplateID == id)
                    {
                        sceneName = scene.Name;
                        return entity;
                    }
                }
            }
            return null;
        }

        /// <summary>
        /// Finds an entity in by template ID
        /// </summary>
        /// <param name="id">Template ID to find the entity in the ArrayList
        /// </param>
        /// <param name="list"></param>
        /// <returns></returns>
        private static MEntity FindByID(Guid id, ArrayList list)
        {
            foreach(MEntity entity in list)
            {
                if (entity.TemplateID == id)
                {
                    return entity;
                }
            }
            return null;
        }
        
    }
}
