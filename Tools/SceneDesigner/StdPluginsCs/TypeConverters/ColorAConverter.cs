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
using System.ComponentModel;
using System.Drawing;
using System.Globalization;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.TypeConverters
{
    // Converts a System.Drawing.Color object to and from a string, always
    // displaying the alpha value after the RGB values.
    class ColorAConverter : ColorConverter
    {
        public override object ConvertFrom(ITypeDescriptorContext context,
            CultureInfo culture, object value)
        {
            if (!(value is string))
            {
                return base.ConvertFrom(context, culture, value);
            }
            object obj1 = null;
            string text1 = ((string)value).Trim();
            if (text1.Length == 0)
            {
                return Color.Empty;
            }
            if (culture == null)
            {
                culture = CultureInfo.CurrentCulture;
            }
            char ch1 = culture.TextInfo.ListSeparator[0];
            TypeConverter converter1 = TypeDescriptor.GetConverter(
                typeof(int));
            string[] textArray1 = text1.Split(new char[] { ch1 });
            int[] numArray1 = new int[textArray1.Length];
            for (int num1 = 0; num1 < numArray1.Length; num1++)
            {
                numArray1[num1] = (int)converter1.ConvertFromString(
                    context, culture, textArray1[num1]);
            }
            switch (numArray1.Length)
            {
                case 3:
                    obj1 = Color.FromArgb(numArray1[0], numArray1[1],
                        numArray1[2]);
                    break;

                case 4:
                    obj1 = Color.FromArgb(numArray1[3], numArray1[0],
                        numArray1[1], numArray1[2]);
                    break;
            }
            if (obj1 == null)
            {
                throw new ArgumentException("Invalid RGBA Color; should " +
                    "have form \"R, G, B, A\".");
            }
            return obj1;
        }

        public override object ConvertTo(ITypeDescriptorContext context,
            CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == null)
            {
                throw new ArgumentNullException("destinationType");
            }
            if ((destinationType == typeof(string)) && (value is Color))
            {
                string[] textArray1;
                Color color1 = (Color)value;
                if (culture == null)
                {
                    culture = CultureInfo.CurrentCulture;
                }
                string text1 = culture.TextInfo.ListSeparator + " ";
                TypeConverter converter1 = TypeDescriptor.GetConverter(
                    typeof(int));
                int num1 = 0;
                textArray1 = new string[4];
                textArray1[num1++] = converter1.ConvertToString(context,
                    culture, color1.R);
                textArray1[num1++] = converter1.ConvertToString(context,
                    culture, color1.G);
                textArray1[num1++] = converter1.ConvertToString(context,
                    culture, color1.B);
                textArray1[num1++] = converter1.ConvertToString(context,
                    culture, color1.A);
                return string.Join(text1, textArray1);
            }
            else
            {
                return base.ConvertTo(context, culture, value,
                    destinationType);
            }
        }
    }
}
