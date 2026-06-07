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
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors
{
    /// <summary>
    /// Summary description for EntityPointerEditor.
    /// </summary>
    public class EntityPointerEditor : UITypeEditor
    {
        public override UITypeEditorEditStyle GetEditStyle(
            ITypeDescriptorContext pmContext)
        {
            if (pmContext == null || pmContext.PropertyDescriptor == null ||
                pmContext.PropertyDescriptor.IsReadOnly ||
                !(pmContext.PropertyDescriptor is MEntityPropertyDescriptor))
            {
                return UITypeEditorEditStyle.None;
            }
            else
            {
                MEntityPropertyDescriptor pmDescriptor =
                    (MEntityPropertyDescriptor) pmContext.PropertyDescriptor;
                MEntity pmEntity = pmDescriptor.PropertyContainer as MEntity;
                if (pmEntity == null ||
                    !MFramework.Instance.Scene.IsEntityInScene(pmEntity) ||
                    !pmDescriptor.PropertyContainer.HasProperty(
                        pmDescriptor.PropertyName) ||
                    pmDescriptor.PropertyContainer.GetElementCount(
                        pmDescriptor.PropertyName) != 1)
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
                Debug.Assert(pmDescriptor.PropertyContainer.GetElementCount(
                    pmDescriptor.PropertyName) == 1, "Element count is " +
                    "greater than 1!");

                MEntity pmOldEntity = pmDescriptor.PropertyContainer
                    .GetPropertyData(pmDescriptor.PropertyName) as MEntity;
                EntityPointerEditorDialog pmDialog = new
                    EntityPointerEditorDialog();
                pmDialog.SelectedEntity = pmOldEntity;
                pmDialog.ActiveEntity = pmDescriptor.PropertyContainer as
                    MEntity;
                if (pmDialog.ShowDialog() == DialogResult.OK)
                {
                    pmDescriptor.PropertyContainer.SetPropertyData(
                        pmDescriptor.PropertyName, pmDialog.SelectedEntity,
                        true);
                }
            }

            return pmValue;
        }
    }
}
