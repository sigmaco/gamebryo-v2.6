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
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Globalization;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.TypeConverters
{
    class EntityTagsConverter : TypeConverter
    {
        public override bool CanConvertFrom(
            ITypeDescriptorContext context,
            Type sourceType)
        {
            return false;
        }

        public override object ConvertTo(
            ITypeDescriptorContext context,
            CultureInfo culture,
            object value,
            Type destinationType)
        {
            // Handle error conditions.
            if (destinationType == null)
            {
                throw new ArgumentNullException("destinationType");
            }
            if (destinationType != typeof(string))
            {
                throw this.GetConvertToException(value, destinationType);
            }
            if (context == null)
            {
                throw new ArgumentNullException("context");
            }
            if (context.PropertyDescriptor == null)
            {
                throw new ArgumentNullException("context.PropertyDescriptor");
            }
            if (value == null)
            {
                return string.Empty;
            }

            // Get initial string value.
            string output = null;
            if ((culture != null) && (culture != CultureInfo.CurrentCulture))
            {
                IFormattable formattable = value as IFormattable;
                if (formattable != null)
                {
                    output = formattable.ToString(null, culture);
                }
            }
            if (output == null)
            {
                output = value.ToString();
            }
            
            // Append inherited tags to string.
            MEntity.EntityDescriptor entityDesc = null;
            if (context != null)
            {
                entityDesc = context.PropertyDescriptor as
                    MEntity.EntityDescriptor;
            }
            string appendString = string.Empty;
            if (entityDesc != null)
            {
                MEntity entity = entityDesc.Entity.MasterEntity;
                while (entity != null)
                {
                    appendString += entity.Tags;
                    entity = entity.MasterEntity;
                }
            }
            else
            {
                appendString = "Multiple Selected";
            }
            if (!string.IsNullOrEmpty(appendString))
            {
                output = string.Format("{0} [{1}]", output, appendString);
            }

            return output;
        }
    }
}
