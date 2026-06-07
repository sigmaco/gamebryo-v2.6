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
using System.IO;
using System.Reflection;
using System.Reflection.Emit;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text.RegularExpressions;
using System.Threading;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for OptionUtilities.
    /// </summary>
    internal class OptionUtilities
    {
        public OptionUtilities()
        {
        }

        /// <summary>
        /// Returns the name of the option without the category prefixes
        /// e.g., an input of "advanced.user.inputmode" returns 
        /// "inputmode"
        /// </summary>
        /// <param name="option"></param>
        /// <returns></returns>
        public static string StripCategories(string option)
        {
            //finds word at end of line that follows zero or more dots.
            Regex regex = new Regex("(?<=[.])?[^.]+$");
            return regex.Match(option).Value;
        }

        /// <summary>
        /// Similar to <c>StripCategories</c> 
        /// except it returns the opposite portion of the option
        /// e.g., an input of "advanced.user.inputmode" returns 
        /// "advanced.user"
        /// </summary>
        /// <param name="option"></param>
        /// <returns></returns>         
        public static string StripOption(string option)
        {
            //return all charaters up until but not including the last 
            //occuring dot.
            Regex regex = new Regex(".+(?=[.])");
            return regex.Match(option).Value;            
        }

        public static TypeBuilder GetTypeBuilder()
        {
            AppDomain myDomain = Thread.GetDomain();   
            AssemblyName myAsmName = new AssemblyName();    
            myAsmName.Name = "DynamicAssemblyName";
            AssemblyBuilder asmBuilder = myDomain.DefineDynamicAssembly(
                myAsmName, 
                AssemblyBuilderAccess.RunAndSave);    
            ModuleBuilder moduleBuilder = 
                asmBuilder.DefineDynamicModule("DynamicOptionsmodule");
            return moduleBuilder.DefineType("OptionsObject",
                TypeAttributes.Public, typeof(OptionObjectDescriptor));
        }

        public static void AddProperty(string propertyName, string displayName,
            Type propertyType, TypeBuilder typeBuilder)
        {
            PropertyBuilder propBuilder = typeBuilder.DefineProperty(
                propertyName, PropertyAttributes.None, propertyType, 
                new Type[] {propertyType});

            FieldBuilder fieldBuilder = typeBuilder.DefineField("custom" +
                propertyName, propertyType, FieldAttributes.Private);

            //Create Getter
            MethodBuilder getterBuilder = 
                typeBuilder.DefineMethod("get_" + propertyName, 
                MethodAttributes.Public | MethodAttributes.HideBySig |
                MethodAttributes.SpecialName, propertyType, new Type[] {});
                
            ILGenerator ilGenerator = getterBuilder.GetILGenerator();
            ilGenerator.DeclareLocal(propertyType);
            ilGenerator.Emit(OpCodes.Ldarg_0);
            ilGenerator.Emit(OpCodes.Ldfld, fieldBuilder);
            ilGenerator.Emit(OpCodes.Stloc_0);
            ilGenerator.Emit(OpCodes.Ldloc_0);
            ilGenerator.Emit(OpCodes.Ret);
            propBuilder.SetGetMethod(getterBuilder);

            //Create Setter
            MethodBuilder setterBuilder =
                typeBuilder.DefineMethod("set_" + propertyName,
                MethodAttributes.Public | MethodAttributes.HideBySig |
                MethodAttributes.SpecialName, null, new Type[]{propertyType});

            ilGenerator = setterBuilder.GetILGenerator();
            ilGenerator.Emit(OpCodes.Ldarg_0);
            ilGenerator.Emit(OpCodes.Ldarg_1);
            ilGenerator.Emit(OpCodes.Stfld, fieldBuilder);
            ilGenerator.Emit(OpCodes.Ret);

            propBuilder.SetSetMethod(setterBuilder);            
        }

        /// <summary>
        /// Guarantees full depth deep copy by using serialization
        /// </summary>
        /// <param name="oldObject"></param>
        /// <returns></returns>
        public static object DeepCopy(object oldObject)
        {
            if (oldObject == null)
                return null;
            MemoryStream ms = new MemoryStream();
            BinaryFormatter formatter = new BinaryFormatter();
            formatter.Serialize(ms, oldObject);
            ms.Seek(0, SeekOrigin.Begin);
            object retVal = formatter.Deserialize(ms);
            return retVal;
        }

        public static void SetProperty(object obj, string option, object val)
        {
            Type objectType = obj.GetType();
            PropertyInfo propInfo = objectType.GetProperty(option);
            if (propInfo != null)
            {
                propInfo.SetValue(obj, val, null);
            }
            else
            {
                IMessageService msgService = 
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService)) as IMessageService;
                msgService.AddMessage(MessageChannelType.Errors, 
                    "Could not set property for option: " + option);
            }
        }

        public static object GetProperty(object obj, string option)
        {
            Type objectType = obj.GetType();
            PropertyInfo propInfo = objectType.GetProperty(option);
            if (propInfo != null)
            {
                return propInfo.GetValue(obj, new object[] {});
            }
            else
            {
                IMessageService msgService = 
                    ServiceProvider.Instance.GetService(
                    typeof(IMessageService)) as IMessageService;
                msgService.AddMessage(MessageChannelType.Errors, 
                    "Could not get property for option: " + option);
                return null;
            }

        }
    }
}
