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
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility
{
    /// <summary>
    /// Summary description for ComponentPropertyUtilities.
    /// </summary>
    public class ComponentPropertyUtilities
    {
        public ComponentPropertyUtilities()
        {
        }

        public static bool CanAddProperty(string propertyName, 
            MComponent component)
        {
            bool canAdd = true;
            MFramework fw = MFramework.Instance;
            MScene scene = fw.Scene;
            if (scene != null)
            canAdd = canAdd && CanAddProperty(propertyName, component, scene);
            MPalette[] palettes = fw.PaletteManager.GetPalettes();
            foreach (MPalette palette in palettes)
            {
                canAdd = canAdd && CanAddProperty(propertyName, component,
                    palette.Scene);
            }
            return canAdd;
        }

        public static bool CanRemoveProperty(string propertyName,
            MComponent component)
        {
            bool canRemove = true;
            MFramework fw = MFramework.Instance;
            MScene scene = fw.Scene;
            if (scene != null)
            canRemove = canRemove && CanRemoveProperty(propertyName, component, scene);
            MPalette[] palettes = fw.PaletteManager.GetPalettes();
            foreach (MPalette palette in palettes)
            {
                canRemove = canRemove && CanRemoveProperty(propertyName, component,
                    palette.Scene);
            }
            return canRemove;
        }

        private static bool CanAddProperty(string propertyName, 
            MComponent component, MScene scene)
        {
            MEntity[] conflictingEntities = 
                GetEntitiesWithConflictOnAdd(propertyName, component, scene);
            return conflictingEntities.Length == 0;
        }

        private static MEntity[] GetEntitiesWithConflictOnAdd(
            string propertyName, MComponent component, MScene scene)
        {
            ArrayList entityList = new ArrayList();
            MEntity[] entities = scene.GetEntities();
            foreach (MEntity entity in entities)
            {
                MComponent existingComponent = entity
                    .GetComponentByTemplateID(component.TemplateID);
                //if the entity does not have the component we're adding to,
                //then there is no conflict
                if (existingComponent != null)
                {
                    string[] propertyNames = entity.GetPropertyNames();
                    if (Array.IndexOf(propertyNames,  propertyName) != -1)
                    {
                        entityList.Add(entity);
                        break;
                    }
                }
            }
            return entityList.ToArray(typeof(MEntity)) as 
                MEntity[];
        }

        private static bool CanRemoveProperty(string propertyName,
            MComponent component, MScene scene)
        {
            MEntity[] conflictingEntities = 
                GetEntitiesWithConflictOnRemove(propertyName, component,
                scene);
            return conflictingEntities.Length == 0;
        }

        private static MEntity[] GetEntitiesWithConflictOnRemove(
            string propertyName, MComponent component, MScene scene)
        {
            ArrayList entityList = new ArrayList();
            MEntity[] entities = scene.GetEntities();
            foreach (MEntity entity in entities)
            {
                MComponent existingComponent =
                    entity.GetComponentByTemplateID(component.TemplateID);
                //if the entity does not have the component we're adding to,
                //then there is no conflict
                if (existingComponent != null)
                {
                    MComponent[] entityComponents = 
                        entity.GetComponents();
                    foreach (MComponent entityComponent in entityComponents)
                    {
                        string[] dependentNames = 
                            entityComponent.GetDependentPropertyNames();
                        if (Array.IndexOf(dependentNames, propertyName) != -1)
                        {
                            entityList.Add(entity);
                        }
                    }
                }
            }
            return entityList.ToArray(typeof(MEntity)) as 
                MEntity[];

        }

    }
}
