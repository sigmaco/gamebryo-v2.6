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
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for OptionObjectDescriptor.
    /// </summary>
    public class OptionObjectDescriptor : ICustomTypeDescriptor
    {
        #region Private Data
        IOptionsService m_optionsService;
        #endregion

        private IOptionsService OptionsService
        {
            get
            {
                if (m_optionsService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_optionsService = 
                        sp.GetService(typeof(IOptionsService))
                        as IOptionsService;
                }
                return m_optionsService;
            }
        }

        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        public string GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        public string GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        public TypeConverter GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        public EventDescriptor GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        public PropertyDescriptor GetDefaultProperty()
        {
            return TypeDescriptor.GetDefaultProperty(this, true);
        }

        public object GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public EventDescriptorCollection GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        public PropertyDescriptorCollection GetProperties()
        {   
            return GetProperties(new Attribute[0]);
        }

        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            ArrayList properties = new ArrayList();
                
            PropertyDescriptorCollection originalProperties = 
                TypeDescriptor.GetProperties(this, attributes, true);                
            foreach(PropertyDescriptor originalDescriptor in 
                originalProperties)
            {
                Attribute[] attributeArray = 
                    new Attribute[originalDescriptor.Attributes.Count];
                originalDescriptor.Attributes.CopyTo(attributeArray, 0);
                string helpText = 
                    OptionsService.GetHelpDescription(
                    originalDescriptor.Name);
                TypeConverter converter = OptionsService.GetTypeConverter(
                    originalDescriptor.Name);
                object editor =
                    OptionsService.GetTypeEditor(originalDescriptor.Name);
                PropertyDescriptor newDescriptor = 
                    new OptionPropertyDescriptor(this, originalDescriptor,
                    helpText, converter, editor);
                
                properties.Add(newDescriptor);

            }
            PropertyDescriptor[] propertyArray = 
                properties.ToArray(typeof(PropertyDescriptor)) 
                as PropertyDescriptor[];
            PropertyDescriptorCollection retVal = 
                new PropertyDescriptorCollection(propertyArray);
            return retVal;               
        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }
    }

}
