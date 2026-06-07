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
using System.ComponentModel;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for OptionPropertyDescriptor.
    /// </summary>
    public class OptionPropertyDescriptor : PropertyDescriptor
    {
        #region Private Data
        string m_helpText;
        TypeConverter m_customConvertor;
        object m_customEditor;
        PropertyDescriptor m_originalDescriptor;
        object m_mainComponent;
        #endregion

        #region PropertyDescriptor Abstract Overrrides 
        
        public OptionPropertyDescriptor(object mainComponent,
            PropertyDescriptor descriptor, string helpText) : 
            this(mainComponent, descriptor, helpText, null, null)
        {            
        }

        public OptionPropertyDescriptor(object mainComponent, 
            PropertyDescriptor descriptor, string helpText, 
            TypeConverter convertor, object customEditor) 
            : base(descriptor)
        {
            m_mainComponent = mainComponent;
            m_originalDescriptor = descriptor;
            m_helpText = helpText;
            m_customConvertor = convertor;
            m_customEditor = customEditor;
        }

        public override bool CanResetValue(object component)
        {
            return false;
        }

        public override object GetValue(object component)
        {
            return OptionUtilities.GetProperty(component, Name);
        }

        public override void ResetValue(object component)
        {
            throw new NotImplementedException();
        }

        public override void SetValue(object component, object value)
        {
            OptionUtilities.SetProperty(m_mainComponent, Name, value);
        }

        public override bool ShouldSerializeValue(object component)
        {
            return true;
        }

        public override Type ComponentType
        {
            get { return m_originalDescriptor.ComponentType; }
        }

        public override bool IsReadOnly
        {
            get { return m_originalDescriptor.IsReadOnly; }
        }

        public override Type PropertyType
        {
            get { return m_originalDescriptor.PropertyType; }
        }

        public override TypeConverter Converter
        {
            get
            {
                if (m_customConvertor == null)
                {
                    TypeConverter baseConverter = base.Converter;

                    if (baseConverter.GetType() == typeof(TypeConverter))
                    {
                        ServiceProvider sp = ServiceProvider.Instance;

                        IPropertyTypeService typeService = 
                            sp.GetService(typeof(IPropertyTypeService)) 
                            as IPropertyTypeService;

                        TypeConverter converter = 
                            FindConverter(PropertyType);
                        if (converter != null)
                        {
                            return converter;
                        }
                    }
                
                    return baseConverter;
                }
                else
                {
                    return m_customConvertor;
                }
            }
        }
        
        public override object GetEditor(Type editorBaseType)
        {
            if (m_customEditor == null)
            {
                return base.GetEditor(editorBaseType);
            }
            else
            {
                return m_customEditor;
            }
        }

        #endregion

        TypeConverter FindConverter(Type type)
        {
            ServiceProvider sp = ServiceProvider.Instance;

            IPropertyTypeService typeService = 
                sp.GetService(typeof(IPropertyTypeService)) 
                as IPropertyTypeService;
            
            
            PropertyType[] propertyTypes = typeService.GetAllPropertyTypes();
            foreach (PropertyType propertyType in propertyTypes)
            {                
                if (propertyType.ManagedType == type)
                {
                    return Activator.CreateInstance(
                        propertyType.TypeConverter) as TypeConverter;
                }
            }
            return null;
        }

        public override string DisplayName
        {
            get
            {
                return OptionUtilities.StripCategories(Name);
            }
        }

        public override string Description
        {
            get
            {
                return m_helpText;
            }
        }

        public override string Category
        {
            get
            {
                return OptionUtilities.StripOption(Name);
            }
        }

    }
}
