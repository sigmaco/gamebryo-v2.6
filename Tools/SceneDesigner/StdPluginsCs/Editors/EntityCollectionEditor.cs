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
using System.Drawing.Design;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Forms;
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors
{
    /// <summary>
    /// Summary description for MEntityCollectionEditor.
    /// </summary>
    public class EntityCollectionEditor : UITypeEditor
    {
        #region Service Accessors
        private static ICommandService ms_pmCommandService;
        private static ICommandService CommandService
        {
            get
            {
                if (ms_pmCommandService == null)
                {
                    ms_pmCommandService = ServiceProvider.Instance.GetService(
                        typeof(ICommandService)) as ICommandService;
                    Debug.Assert(ms_pmCommandService != null,
                        "Command service not found!");
                }
                return ms_pmCommandService;
            }
        }
        #endregion

        public override UITypeEditorEditStyle GetEditStyle(
            ITypeDescriptorContext pmContext)
        {
            // Return none if it is in a palette
            if (pmContext == null || pmContext.PropertyDescriptor == null ||
                pmContext.PropertyDescriptor.IsReadOnly ||
                !(pmContext.PropertyDescriptor is MEntityPropertyDescriptor))
            {
                return UITypeEditorEditStyle.None;
            }
            else
            {
                MEntityPropertyDescriptor pmDescriptor =
                    (MEntityPropertyDescriptor)pmContext.PropertyDescriptor;
                MEntity pmEntity = pmDescriptor.PropertyContainer as MEntity;
                if (pmEntity == null ||
                    !MFramework.Instance.Scene.IsEntityInScene(pmEntity) ||
                    !pmDescriptor.PropertyContainer.HasProperty(
                        pmDescriptor.PropertyName))
                {
                    return UITypeEditorEditStyle.None;
                }
                else
                {
                    return UITypeEditorEditStyle.Modal;
                }
            }
        }

        public override object EditValue(ITypeDescriptorContext pmContext,
            IServiceProvider pmProvider, object pmValue)
        {
            if (pmContext != null && pmContext.PropertyDescriptor != null &&
                pmContext.PropertyDescriptor is MEntityPropertyDescriptor)
            {
                MEntityPropertyDescriptor pmDescriptor =
                    (MEntityPropertyDescriptor) pmContext.PropertyDescriptor;

                Debug.Assert(pmDescriptor.PropertyContainer.HasProperty(
                    pmDescriptor.PropertyName), "Entity does not contain " +
                    "property!");

                uint uiCount = pmDescriptor.PropertyContainer.GetElementCount(
                    pmDescriptor.PropertyName);
                ArrayList pmEntities = new ArrayList();
                for (uint ui = 0; ui < uiCount; ui++)
                {
                    MEntity pmEntity = pmDescriptor.PropertyContainer.
                        GetPropertyData(pmDescriptor.PropertyName, ui)
                        as MEntity;
                    if (pmEntity != null)
                    {
                        pmEntities.Add(pmEntity);
                    }
                }

                MEntity[] amOldEntities = (MEntity[]) pmEntities.ToArray(
                    typeof(MEntity));
                EntityCollectionEditorDialog pmDialog = new
                    EntityCollectionEditorDialog(amOldEntities,
                    pmDescriptor.PropertyContainer);
                if (pmDialog.ShowDialog() == DialogResult.OK)
                {
                    MEntity[] amNewEntities = pmDialog.GetEntityCollection();

                    CommandService.BeginUndoFrame("Change affected " +
                        "entities for \"" +
                        pmDescriptor.PropertyContainer.Name + "\" light");

                    // Clear out all entity entries.
                    for (uint ui = 0; ui < pmDescriptor
                        .PropertyContainer.GetElementCount(pmDescriptor
                        .PropertyName); ui++)
                    {
                        pmDescriptor.PropertyContainer.SetPropertyData(
                            pmDescriptor.PropertyName, null, ui, true);
                    }

                    // Add new entity entries.
                    for (int i = 0; i < amNewEntities.Length; i++)
                    {
                        pmDescriptor.PropertyContainer.SetPropertyData(
                            pmDescriptor.PropertyName, amNewEntities[i],
                            (uint) i, true);
                    }

                    CommandService.EndUndoFrame(true);
                }
            }

            return pmValue;
        }
    }
}
