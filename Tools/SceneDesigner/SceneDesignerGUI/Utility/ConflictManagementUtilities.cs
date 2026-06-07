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
using System.ComponentModel;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for ConflictManagementUtilities.
    /// </summary>
    public class ConflictManagementUtilities
    {
        private static IMessageService m_messageService;

        private static readonly string m_sceneChangedText = 
            "SCENE CHANGED: ";

        public ConflictManagementUtilities()
        {
        }

        #region Private Nested Types

        private enum ComparisonResults
        {
            Equal,
            ComponentsAdded,
            ComponentsRemoved,
            ComponentPropertiesAdded,
            ComponentPropertiesRemoved,
            PropertiesChanged,
            InheritedPropertiesChanged,
            PropertyTypeMismatch
        }


        private struct EntityDiscrepancy
        {
            public ComparisonResults Results;
            public string OriginalScene;
            public MEntity OriginalEntity;
            public MComponent OriginalComponent;
            public MEntityPropertyDescriptor OriginalProperty;
            public object[] OriginalValue;
            public uint OriginalValueCount;
            public string NewScene;
            public MEntity NewEntity;
            public MComponent NewComponent;
            public MEntityPropertyDescriptor NewProperty;
            public object[] NewValue;
            public uint NewValueCount;

            public override string ToString()
            {
                StringBuilder sb = new StringBuilder();
                sb.AppendFormat("\nDiscrepancy Type: {0}\n", 
                    Results.ToString());

                //Original Entity
                sb.AppendFormat("Original Entity: ");
                if (OriginalEntity != null)
                {
                    sb.AppendFormat("{0}", OriginalEntity.Name);
                }
                sb.Append("\n----------------------------------------\n");
                if (OriginalComponent != null)
                {
                    sb.AppendFormat("  Original Component: {0}\n",
                        OriginalComponent.Name);
                }
                if (OriginalProperty != null)
                {
                    sb.AppendFormat("  Original Property: {0}\n",
                        OriginalProperty.PropertyName);
                
                    sb.AppendFormat("  Original Property Type: {0}\n",
                        OriginalProperty.PropertyType.Name);
                    if (OriginalValue != null)
                    {
                        sb.Append("  Original Property Value(s):\n");
                        for (uint ui = 0; ui < OriginalValue.Length; ui++)
                        {
                            sb.AppendFormat("    {0}\n",
                                OriginalProperty.Converter.ConvertToString(
                                OriginalValue[ui]));
                            
                        }
                    }
                }
                //sb.Append("----------------------------------------\n\n");
                sb.Append("\n\n");

                //New Entity
                sb.AppendFormat("New Entity: ");
                if (NewEntity != null)
                {
                    sb.AppendFormat("{0}", NewEntity.Name);
                }
                sb.Append("\n----------------------------------------\n");
                if (NewComponent != null)
                {
                    sb.AppendFormat("  New Component: {0}\n", 
                        NewComponent.Name);
                }
                if (NewProperty != null)
                {
                    sb.AppendFormat("  New Property: {0}\n",
                        NewProperty.PropertyName);
                    sb.AppendFormat("  New Property Type: {0}\n",
                        NewProperty.PropertyType.Name);
                    if (NewValue != null)
                    {
                        sb.Append("  New Property Value(s):\n");
                        for (uint ui = 0; ui < NewValue.Length; ui++)
                        {
                            sb.AppendFormat("    {0}\n",
                                NewProperty.Converter.ConvertToString(
                                    NewValue[ui]));
                            
                        }
                    }
                }
                //sb.Append("----------------------------------------");
                return sb.ToString();
            }
        }

        #endregion

        private static IMessageService MessageService
        {
            get
            {
                if (m_messageService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_messageService = sp.GetService(typeof(IMessageService))
                        as IMessageService;
                }
                return m_messageService;
            }
        }


        /// <summary>
        /// Checks a loaded scene for conflicts against the
        ///  set of loaded palettes
        /// </summary>
        /// <param name="scene">The Scene to Check</param>
        /// <param name="palettes">The set of palettes to check againsts</param>
        /// <returns>true, if the scene contained conflicts,
        /// otherwise false</returns>
        public static bool CheckForConflicts(MScene scene, MPalette[] palettes)
        {
            bool bConflictsDetected = false;
            ArrayList conflicts = new ArrayList();
            MScene[] paletteScenes = GetSceneArray(scene, palettes);
            //First, check for conflicts on the master entities.
            MEntity[] rootEntities = GetRootEntities(scene);
            foreach(MEntity rootEntity in rootEntities)
            {
                string paletteName;
                MEntity template = EntityManagementUtilities.FindByID(
                    rootEntity.TemplateID, paletteScenes, 
                    out paletteName);
                if (template != null)
                {
                    if (rootEntity != template)
                    {
                        ArrayList templateDiscrepancies =
                            CompareEntities(scene.Name,
                            rootEntity, paletteName, template);
                        // Add the problems for each template to the master
                        // list, filtering out property differences for
                        // properties that do not inherit, since they will not
                        // affect the entities in the scene.
                        foreach (EntityDiscrepancy discrepancy in
                            templateDiscrepancies)
                        {
                            if (discrepancy.Results !=
                                ComparisonResults.PropertiesChanged)
                            {
                                conflicts.Add(discrepancy);
                            }
                        }
                    }
                }
            }
            //Check root entities against the component service
            if (conflicts.Count > 0)
            {
                HandleTemplateMistmatch(conflicts);
                bConflictsDetected = true;
            }

            //Clear List
            conflicts = new ArrayList();
            foreach (MEntity rootEntity in rootEntities)
            {
                conflicts.AddRange(
                    CheckEntityComponents(rootEntity, scene.Name));
            }
            if (conflicts.Count > 0)
            {
                HandleComponentConflicts(conflicts);
                bConflictsDetected = true;
            }
            MEntity[] sceneEntities = scene.GetEntities();
            //Now check the Scene Entities Themselves
            ArrayList sceneEntityConflicts = new ArrayList();
            foreach (MEntity sceneEntity in sceneEntities)
            {
                sceneEntityConflicts.AddRange(
                    CheckEntityComponents(sceneEntity, scene.Name));
            }
            if (sceneEntityConflicts.Count > 0)
            {
                HandleSceneEntityComponentConflicts(sceneEntityConflicts);
                bConflictsDetected = true;
            }
            return bConflictsDetected;

        }

        private static ArrayList CheckEntityComponents(MEntity entity, string
            sceneName)
        {
            ArrayList discrepancies = new ArrayList();
            ServiceProvider sp = ServiceProvider.Instance;

            IComponentService componentService =
                sp.GetService(typeof(IComponentService)) as IComponentService;
            
            MComponent[] entityComponents = entity.GetComponents();
            foreach (MComponent entityComponent in entityComponents)
            {
                PropertyDescriptorCollection entityComponentProperties = 
                    entityComponent.GetProperties();

                MComponent serviceComponent =
                    componentService.GetComponentByID(
                    entityComponent.TemplateID);
                if (serviceComponent != null)
                {

                    PropertyDescriptorCollection serviceComponentProperties = 
                        serviceComponent.GetProperties();
                    //Check for properties being added 
                    foreach (MEntityPropertyDescriptor serviceComponentProperty
                        in serviceComponentProperties)
                    {
                        MEntityPropertyDescriptor entityProperty = 
                            FindPropertyByName(
                            serviceComponentProperty.PropertyName,
                            entityComponentProperties);
                        if (entityProperty == null)
                        {
                            EntityDiscrepancy discrepancy = 
                                new EntityDiscrepancy();
                            discrepancy.OriginalScene = sceneName;
                            discrepancy.OriginalEntity = entity;
                            discrepancy.OriginalComponent = entityComponent;
                            //discrepancy.OriginalProperty = entityProperty;
                            //discrepancy.OriginalValue = 
                            //    entityProperty.GetValue(entity);
                            discrepancy.NewComponent = serviceComponent;
                            discrepancy.NewProperty = serviceComponentProperty;
                            discrepancy.Results = 
                                ComparisonResults.ComponentPropertiesAdded;
                            discrepancies.Add(discrepancy);
                        }
                        else
                        {
                            if (!entityProperty.PropertyType.Equals(
                                serviceComponentProperty.PropertyType))
                            {
                                EntityDiscrepancy discrepancy = 
                                    new EntityDiscrepancy();
//                                discrepancy.NewScene = newSceneName;
//                                discrepancy.OriginalScene = originalSceneName;
//                                discrepancy.NewEntity = newEntity;
                                discrepancy.OriginalScene = sceneName;
                                discrepancy.OriginalEntity = entity;
                                discrepancy.NewComponent = serviceComponent;
                                discrepancy.OriginalComponent = 
                                    entityComponent;
                                discrepancy.NewProperty = 
                                    serviceComponentProperty;
                                discrepancy.OriginalProperty = 
                                    entityProperty;

                                discrepancy.Results = 
                                    ComparisonResults.PropertyTypeMismatch;
                                discrepancies.Add(discrepancy);

                            }
                        }

                    }

                    foreach (MEntityPropertyDescriptor entityProperty in
                        entityComponentProperties)
                    {
                        MEntityPropertyDescriptor serviceComponentProperty =
                            FindPropertyByName(entityProperty.PropertyName,
                            serviceComponentProperties);
                        if (serviceComponentProperty == null)
                        {
                            EntityDiscrepancy discrepancy = 
                                new EntityDiscrepancy();
//                            discrepancy.NewScene = newSceneName;
//                            discrepancy.OriginalScene = originalSceneName;
//                            discrepancy.NewEntity = newEntity;
                            discrepancy.OriginalScene = sceneName;
                            discrepancy.OriginalEntity = entity;
                            discrepancy.NewComponent = serviceComponent;
                            discrepancy.OriginalComponent = entityComponent;
                            //discrepancy.NewProperty = newPropertyDescriptor;
                            discrepancy.OriginalProperty = 
                                entityProperty;
                            discrepancy.Results = 
                                ComparisonResults.ComponentPropertiesRemoved;
                            discrepancies.Add(discrepancy);
                            
                        }

                    }

                }
            }

            return discrepancies;
        }


        private static ArrayList CompareEntities(string originalSceneName,
            MEntity originalEntity, string newSceneName, MEntity newEntity)
        {
            ArrayList discrepancies = new ArrayList();

            MComponent[] originalComponents = originalEntity.GetComponents();
            MComponent[] newComponents = newEntity.GetComponents();
            //Check if the new entity had added components
            foreach (MComponent newComponent in newComponents)
            {
                if (FindComponentByID(newComponent.TemplateID, 
                    originalComponents) == null)
                {
                    EntityDiscrepancy discrepancy = new EntityDiscrepancy();
                    discrepancy.NewScene = newSceneName;
                    discrepancy.OriginalScene = originalSceneName;
                    discrepancy.NewEntity = newEntity;
                    discrepancy.OriginalEntity = originalEntity;
                    discrepancy.NewComponent = newComponent;
                    discrepancy.Results = ComparisonResults.ComponentsAdded;
                    discrepancies.Add(discrepancy);
                }
            }
            //Check if the new entity had removed components
            foreach (MComponent originalComponent in originalComponents)
            {
                if (FindComponentByID(originalComponent.TemplateID, 
                    newComponents) == null)
                {
                    EntityDiscrepancy discrepancy = new EntityDiscrepancy();
                    discrepancy.NewScene = newSceneName;
                    discrepancy.OriginalScene = originalSceneName;
                    discrepancy.NewEntity = newEntity;
                    discrepancy.OriginalEntity = originalEntity;
                    discrepancy.OriginalComponent = originalComponent;
                    discrepancy.Results = ComparisonResults.ComponentsRemoved;
                    discrepancies.Add(discrepancy);
                }                
            }
            //Check if the new entity added properties to 
            //any of it's components
            foreach (MComponent newComponent in newComponents)
            {
                MComponent originalComponent = 
                    FindComponentByID(newComponent.TemplateID, 
                    originalComponents);
                if (originalComponent != null)
                {                    
                    PropertyDescriptorCollection newProperties 
                        = newComponent.GetProperties();
                    PropertyDescriptorCollection originalProperties 
                        = originalComponent.GetProperties();

                    foreach (MEntityPropertyDescriptor newPropertyDescriptor in 
                        newProperties)
                    {
                        MEntityPropertyDescriptor originalPropertyDescriptor =
                            FindPropertyByName(
                            newPropertyDescriptor.PropertyName,
                            originalProperties);
                        if ( originalPropertyDescriptor == null)
                        {
                            EntityDiscrepancy discrepancy = 
                                new EntityDiscrepancy();
                            discrepancy.NewScene = newSceneName;
                            discrepancy.OriginalScene = originalSceneName;
                            discrepancy.NewEntity = newEntity;
                            discrepancy.OriginalEntity = originalEntity;
                            discrepancy.NewComponent = newComponent;
                            discrepancy.OriginalComponent = originalComponent;
                            discrepancy.NewProperty = newPropertyDescriptor;
                            discrepancy.Results = 
                                ComparisonResults.ComponentPropertiesAdded;
                            discrepancies.Add(discrepancy);
                        }
                        else
                        {
                            if (!newPropertyDescriptor.PropertyType.Equals(
                                originalPropertyDescriptor.PropertyType))
                            {

                                EntityDiscrepancy discrepancy = 
                                    new EntityDiscrepancy();
                                discrepancy.NewScene = newSceneName;
                                discrepancy.OriginalScene = originalSceneName;
                                discrepancy.NewEntity = newEntity;
                                discrepancy.OriginalEntity = originalEntity;
                                discrepancy.NewComponent = newComponent;
                                discrepancy.OriginalComponent = 
                                    originalComponent;
                                discrepancy.NewProperty = 
                                    newPropertyDescriptor;
                                discrepancy.OriginalProperty = 
                                    originalPropertyDescriptor;

                                discrepancy.Results = 
                                    ComparisonResults.PropertyTypeMismatch;
                                discrepancies.Add(discrepancy);
                            } 
                                //A type mistmatch implies a value mismatch,
                                //so the two are mutually exclusive
                            else 
                            {
                                uint originalCount =
                                    originalEntity.GetElementCount(
                                    originalPropertyDescriptor.PropertyName);
                                object[] originalValues = 
                                    new object[originalCount];
                                for (uint ui = 0; ui < originalCount; ui++)
                                {
                                    originalValues[ui] =
                                       originalEntity.GetPropertyData(
                                       originalPropertyDescriptor.PropertyName,
                                       ui);
                                }
                                uint newCount = newEntity.GetElementCount(
                                    newPropertyDescriptor.PropertyName);
                                object[] newValues = new object[newCount];
                                for (uint ui = 0; ui < newCount; ui++)
                                {
                                    newValues[ui] =
                                        newEntity.GetPropertyData(
                                        newPropertyDescriptor.PropertyName,
                                        ui);
                                }
                                bool entitiesMatch = true;
                                if ( originalCount == newCount)
                                {
                                    for (uint uiIndex = 0; uiIndex < newCount;
                                        uiIndex++)
                                    {
                                        if (!object.Equals(
                                            originalValues[uiIndex],
                                                 newValues[uiIndex]))
                                        {
                                            entitiesMatch = false;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    entitiesMatch = false;
                                }
                                if (entitiesMatch)
                                {
                                    continue;
                                }
                                EntityDiscrepancy discrepancy = 
                                    new EntityDiscrepancy();
                                discrepancy.NewScene = newSceneName;
                                discrepancy.OriginalScene = originalSceneName;
                                discrepancy.NewEntity = newEntity;
                                discrepancy.OriginalEntity = originalEntity;
                                discrepancy.NewComponent = newComponent;
                                discrepancy.OriginalComponent = 
                                    originalComponent;
                                discrepancy.NewProperty = 
                                    newPropertyDescriptor;
                                discrepancy.OriginalProperty = 
                                    originalPropertyDescriptor;
                                discrepancy.NewValue = newValues;
                                discrepancy.NewValueCount = newCount;
                                discrepancy.OriginalValue = originalValues;
                                discrepancy.OriginalValueCount = originalCount;

                                bool inheritable = 
                                    discrepancy.OriginalComponent.
                                    IsPropertyInheritable(
                                    discrepancy.OriginalProperty.PropertyName);

                                if (!inheritable)
                                {
                                    discrepancy.Results = 
                                        ComparisonResults.PropertiesChanged;
                                }
                                else
                                {
                                    discrepancy.Results = 
                                        ComparisonResults.
                                        InheritedPropertiesChanged;
                                }
                                discrepancies.Add(discrepancy);

                            }
                            
                        }
                    }
                       
                    foreach (
                        MEntityPropertyDescriptor originalPropertyDescriptor
                        in originalProperties)
                    {
                        MEntityPropertyDescriptor newPropertyDescriptor = 
                            FindPropertyByName(
                            originalPropertyDescriptor.PropertyName,
                            newProperties);
                        if (newPropertyDescriptor == null)
                        {
                            EntityDiscrepancy discrepancy = 
                                new EntityDiscrepancy();
                            discrepancy.NewScene = newSceneName;
                            discrepancy.OriginalScene = originalSceneName;
                            discrepancy.NewEntity = newEntity;
                            discrepancy.OriginalEntity = originalEntity;
                            discrepancy.NewComponent = newComponent;
                            discrepancy.OriginalComponent = originalComponent;
                            //discrepancy.NewProperty = newPropertyDescriptor;
                            discrepancy.OriginalProperty = 
                                originalPropertyDescriptor;
                            discrepancy.Results = 
                                ComparisonResults.ComponentPropertiesRemoved;
                            discrepancies.Add(discrepancy);
                        }
                    }
                }
            }
            return discrepancies;
        }

        private static void HandleTemplateMistmatch(
            ArrayList comparison)
        {

            foreach(EntityDiscrepancy discrepancy in comparison)
            {
                switch (discrepancy.Results)
                {
                    case ComparisonResults.ComponentsAdded:
                    {
                        HandleComponentsAdded(discrepancy);
                        break;
                    }
                    case ComparisonResults.ComponentsRemoved:
                    {
                        HandleComponentsRemoved(discrepancy);
                        break;
                    }
                    case ComparisonResults.PropertiesChanged:
                    {
                        HandlePropertiesChanged(discrepancy);
                        break;
                    }
                    case ComparisonResults.InheritedPropertiesChanged:
                    {
                        HandleInheritedPropertiesChanged(discrepancy);
                        break;
                    }
                    case ComparisonResults.ComponentPropertiesAdded:
                    {
                        HandleComponentPropertiesAdded(discrepancy);
                        break;
                    }
                    case ComparisonResults.ComponentPropertiesRemoved:
                    {
                        HandleComponentPropertiesRemoved(discrepancy);
                        break;
                    }
                    case ComparisonResults.PropertyTypeMismatch:
                    {
                        HandlePropertyTypeMismatch(discrepancy);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }


        private static void HandleComponentsAdded(
            EntityDiscrepancy discrepancy)
        {
            MComponent newComponent = discrepancy.NewComponent.Clone(false);
            AddMessage(
                string.Format("Component '{0}' was added to Entity '{1}'",
                newComponent.Name, discrepancy.OriginalEntity.Name),
                discrepancy);
            discrepancy.OriginalEntity.AddComponent(newComponent, false, false);
        }


        private static void HandleComponentsRemoved(
            EntityDiscrepancy discrepancy)
        {
            AddMessage(
                string.Format("Component '{0}' was removed from Entity '{1}'",
                discrepancy.OriginalComponent.Name, 
                discrepancy.OriginalEntity.Name),
                discrepancy);

            discrepancy.OriginalEntity.RemoveComponent(
                discrepancy.OriginalComponent, false, false);

        }

        private static void HandlePropertiesChanged(
            EntityDiscrepancy discrepancy)
        {

            AddMessage(
                string.Format("Property '{0}' has changed on Entity '{1}'",
                discrepancy.OriginalProperty.PropertyName, 
                discrepancy.OriginalEntity.Name),
                discrepancy);
        }

        private static void HandleInheritedPropertiesChanged(
            EntityDiscrepancy discrepancy)
        {
            AddMessage(
                string.Format(
                    "Inherited Property '{0}' has changed on Entity '{1}'",
                discrepancy.OriginalProperty.PropertyName, 
                discrepancy.OriginalEntity.Name),
                discrepancy);
                        
            bool bIsCollection = discrepancy.NewComponent.IsCollection(
                discrepancy.NewProperty.PropertyName);
            if (bIsCollection)
            {
                //Note: to handle the case where we are reducing the number of 
                //elements for this property, we currently must remove the 
                // property and re-add it. The NiEntityPropertyInterface class 
                // needs to have a mechnism for setting the element count 
                // before we can change properly
                PropertyType originalPropertyType =
                    discrepancy.OriginalComponent.GetPropertyType(
                    discrepancy.OriginalProperty.PropertyName);

                string originalDisplayName =
                    discrepancy.OriginalComponent.GetPropertyDisplayName(
                    discrepancy.OriginalProperty.PropertyName);

                discrepancy.OriginalComponent.RemoveProperty(
                    discrepancy.OriginalProperty.PropertyName, false);

                discrepancy.OriginalComponent.AddProperty(
                    discrepancy.OriginalProperty.PropertyName,
                    originalDisplayName,
                    originalPropertyType.PrimitiveType,
                    originalPropertyType.Name,
                    bIsCollection,
                    discrepancy.OriginalProperty.Description, false);
                for (uint ui = 0; ui < discrepancy.OriginalValueCount; ui++)
                {
                    discrepancy.OriginalEntity.SetPropertyData(
                        discrepancy.OriginalProperty.PropertyName,
                        null, ui, false);
                }
                for (uint ui = 0; ui < discrepancy.NewValueCount; ui++)
                {
                    discrepancy.OriginalEntity.SetPropertyData(
                        discrepancy.OriginalProperty.PropertyName,
                        discrepancy.NewValue[ui], ui, false);
                }
            }
            else
            {
                //now, add the new data
                if (discrepancy.NewValueCount > 0)
                {
                    // This should never happen.  If the property is not a 
                    // collection, then there shouldn't be more than one new
                    // value.  However, this code was here previously, and
                    // I don't want to change the behavior if it somehow does
                    // end up happening
                    if (discrepancy.NewValueCount > 1)
                    {
                        for (uint ui = 0; ui < discrepancy.NewValueCount; ui++)
                        {
                            discrepancy.OriginalEntity.SetPropertyData(
                              discrepancy.OriginalProperty.PropertyName,
                                discrepancy.NewValue[ui], false);
                        }
                    }
                    else
                    {
                        discrepancy.OriginalEntity.SetPropertyData(
                          discrepancy.OriginalProperty.PropertyName,
                            discrepancy.NewValue[0], false);
                    }
                }                
                else
                {
                    // There is no data in the new value, so null out the 
                    // original value
                    discrepancy.OriginalEntity.SetPropertyData(
                      discrepancy.OriginalProperty.PropertyName,
                        null, false);
                }
            }                        
        }

        private static void HandleComponentPropertiesAdded(
            EntityDiscrepancy discrepancy)
        {
            AddMessage(
                string.Format(
                    "Property '{0}' has been added to Component '{1}'\n" +
                    "On Entity '{2}'",
                discrepancy.NewProperty.PropertyName, 
                discrepancy.NewComponent.Name,
                discrepancy.OriginalEntity.Name),
                discrepancy);
            PropertyType newPropertyType = 
                discrepancy.NewComponent.GetPropertyType(
                discrepancy.NewProperty.PropertyName);
            
            bool bIsColleciton = discrepancy.NewComponent.IsCollection(
                discrepancy.NewProperty.PropertyName);

            
            discrepancy.OriginalComponent.AddProperty(
                discrepancy.NewProperty.PropertyName,
                discrepancy.NewComponent.GetPropertyDisplayName(
                discrepancy.NewProperty.PropertyName),
                newPropertyType.PrimitiveType,
                newPropertyType.Name,
                bIsColleciton,
                discrepancy.NewProperty.Description,
                false);
            
        }

        private static void HandleComponentPropertiesRemoved(
            EntityDiscrepancy discrepancy)
        {
            AddMessage(
                string.Format(
                    "Property '{0}' has been removed from Component '{1}'\n" +
                    "On Entity '{2}'",
                discrepancy.OriginalProperty.PropertyName, 
                discrepancy.OriginalComponent.Name,
                discrepancy.OriginalEntity.Name),
                discrepancy);

            discrepancy.OriginalComponent.RemoveProperty(
                discrepancy.OriginalProperty.PropertyName, false);
        }

        private static void HandlePropertyTypeMismatch(
            EntityDiscrepancy discrepancy)
        {
            AddMessage(
                string.Format(
                    "Property '{0}' has changed type on Component '{1}'\n" +
                    "in Entity '{2}'",
                discrepancy.OriginalProperty.PropertyName, 
                discrepancy.OriginalComponent.Name,
                discrepancy.OriginalEntity.Name),
                discrepancy);

            if (discrepancy.OriginalEntity.MasterEntity == null)
            {

                discrepancy.OriginalComponent.RemoveProperty(
                    discrepancy.OriginalProperty.PropertyName, false);

                PropertyType newPropertyType = 
                    discrepancy.NewComponent.GetPropertyType(
                    discrepancy.NewProperty.PropertyName);

                discrepancy.OriginalComponent.AddProperty(
                    discrepancy.NewProperty.PropertyName,
                    discrepancy.NewComponent.GetPropertyDisplayName(
                    discrepancy.NewProperty.PropertyName),
                    newPropertyType.PrimitiveType,
                    newPropertyType.Name,
                    discrepancy.NewProperty.Description,
                    false);
            }
            else
            {
                string propertyName =
                    discrepancy.OriginalProperty.PropertyName;
                MEntity entity = discrepancy.OriginalEntity;

                if (entity.IsPropertyUnique(propertyName))
                {
                    if (entity.CanResetProperty(propertyName))
                    {
                        entity.ResetProperty(propertyName);
                    }
                    else
                    {
                        discrepancy.OriginalComponent.RemoveProperty(
                            discrepancy.OriginalProperty.PropertyName, false);

                        PropertyType newPropertyType = 
                            discrepancy.NewComponent.GetPropertyType(
                            discrepancy.NewProperty.PropertyName);

                        discrepancy.OriginalComponent.AddProperty(
                            discrepancy.NewProperty.PropertyName,
                            discrepancy.NewComponent.GetPropertyDisplayName(
                            discrepancy.NewProperty.PropertyName),
                            newPropertyType.PrimitiveType,
                            newPropertyType.Name,
                            discrepancy.NewProperty.Description,
                            false);
                    }
                }
            }

            
        }

        private static void HandleComponentConflicts(ArrayList dicrepancies)
        {
            foreach(EntityDiscrepancy discrepancy in dicrepancies)
            {
                switch (discrepancy.Results)
                {
                    case ComparisonResults.ComponentPropertiesAdded:
                    {
                        HandleComponentPropertiesAdded(discrepancy);
                        break;
                    }
                    case ComparisonResults.ComponentPropertiesRemoved:
                    {
                        HandleComponentPropertiesRemoved(discrepancy);
                        break;
                    }
                    case ComparisonResults.PropertyTypeMismatch:
                    {
                        HandlePropertyTypeMismatch(discrepancy);
                        break;
                    }
                    default: //should never get here...
                    {
                        throw new NotSupportedException(
                            "An invalid conflict type has been encountered: "
                            + discrepancy.Results.ToString());
                    }
                }
            }
        }

        private static void HandleSceneEntityComponentConflicts(
            ArrayList dicrepancies)
        {
            foreach(EntityDiscrepancy discrepancy in dicrepancies)
            {
                switch (discrepancy.Results)
                {
                    case ComparisonResults.ComponentPropertiesAdded:
                    {
                        //I believe in this case, nothing should be done,
                        //The master entity for this should handle this 
                        //situation
                        break;
                    }
                    case ComparisonResults.ComponentPropertiesRemoved:
                    {
                        if (discrepancy.OriginalEntity.IsPropertyUnique(
                            discrepancy.OriginalProperty.PropertyName))
                        {
                            
                            HandleComponentPropertiesRemoved(discrepancy);
                        }
                        break;
                    }
                    case ComparisonResults.PropertyTypeMismatch:
                    {
                        HandlePropertyTypeMismatch(discrepancy);
                        break;
                    }
                    default: //should never get here...
                    {
                        throw new NotSupportedException(
                            "An invalid conflict type has been encountered: "
                            + discrepancy.Results.ToString());
                    }
                }
            }
        }


        private static MEntity[] GetRootEntities(MScene scene)
        {
            ArrayList retVal = new ArrayList();
            MEntity[] allEntities = scene.GetEntities();
            foreach (MEntity entity in allEntities)
            {
                MEntity root = GetRootEntity(entity);
                if (!retVal.Contains(root))
                {
                    retVal.Add(root);
                }
            }
            return retVal.ToArray(typeof(MEntity)) as MEntity[];
        }

        private static MEntity GetRootEntity(MEntity entity)
        {
            MEntity root = entity;
            while(root.MasterEntity != null)
            {
                root = root.MasterEntity;
            }
            return root;
        }

        private static MComponent FindComponentByID(Guid id, 
            MComponent[] components)
        {
            foreach (MComponent component in components)
            {
                if (component.TemplateID == id)

                {
                    return component;
                }
            }
            return null;
        }

        private static MEntityPropertyDescriptor FindPropertyByName(
            string name, PropertyDescriptorCollection properties)
        {
            foreach (MEntityPropertyDescriptor property in properties)
            {
                if (property.PropertyName.Equals(name))
                {
                    return property;
                }
            }
            return null;
        }

        private static MScene[] GetSceneArray(MScene sceneToFilter,
            MPalette[] palettes)
        {
            ArrayList sceneList = new ArrayList();
            //MScene[] sceneArray = new MScene[palettes.Length];
            //int index = 0;
            foreach (MPalette palette in palettes)
            {
                if (palette.Scene != sceneToFilter)
                {
                    sceneList.Add(palette.Scene);
                }
            }
            return sceneList.ToArray(typeof(MScene)) as MScene[];
        }

        private static void AddMessage(string s, EntityDiscrepancy discrepancy)
        {
            Message msg = new Message();
            msg.m_strText = m_sceneChangedText + s;
            msg.m_strDetails = discrepancy.ToString();

            MessageService.AddMessage(MessageChannelType.Conflicts, msg);
        }


    }
}
