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
using System.Text;
using Microsoft.Win32;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for AssociateFileTypes.
    /// </summary>
    public class AssociateFileTypes
    {
        public static bool Associate(
            string strExtension, string strApplication, string strOptions,
            string strIdentifier, string strDescription, string strIcon)
        {
            try
            {
                RegistryKey ExtKey = null;
                ExtKey = GetKey(Registry.ClassesRoot, strExtension);
                if (ExtKey == null)
                    return false;
                ExtKey.SetValue("", strIdentifier);

                RegistryKey IdKey = null;
                IdKey = GetKey(Registry.ClassesRoot, strIdentifier);
                if (IdKey == null)
                    return false;

                IdKey.SetValue("", strDescription);

                RegistryKey IconKey = GetKey(IdKey, "DefaultIcon");
                if (IconKey == null)
                    return false;
                IconKey.SetValue("", strIcon);

                RegistryKey ShellKey = GetKey(IdKey, "shell");
                if (ShellKey == null)
                    return false;

                RegistryKey OpenKey = GetKey(ShellKey, "open");
                if (OpenKey == null)
                    return false;

                RegistryKey OpenCommandKey = GetKey(OpenKey, "command");
                if (OpenCommandKey == null)
                    return false;

                StringBuilder CommandString = new StringBuilder();
                CommandString.Append("\"");
                CommandString.Append(strApplication);
                CommandString.Append("\" ");
                CommandString.Append(strOptions);
                CommandString.Append(" \"%1\"");

                OpenCommandKey.SetValue("", CommandString.ToString());

                ExtKey.Close();
                IdKey.Close();
                IconKey.Close();
                ShellKey.Close();
                OpenKey.Close();
                OpenCommandKey.Close();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        private static RegistryKey GetKey(RegistryKey Parent, string SubKey)
        {
            string [] strSubKeys = Parent.GetSubKeyNames();
            foreach (string key in strSubKeys)
            {
                if (key.Equals(SubKey))
                {
                    return Parent.OpenSubKey(SubKey, true);
                }
            }

            return CreateKey(Parent, SubKey);
        }

        private static RegistryKey CreateKey(RegistryKey Parent, string SubKey)
        {
            return Parent.CreateSubKey(SubKey);
        }
    }
}
