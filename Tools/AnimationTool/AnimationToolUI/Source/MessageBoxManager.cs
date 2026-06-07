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
using System.Windows.Forms;
using Microsoft.Win32;
using System.Reflection;
using System.IO;
using System.Collections;
using System.Diagnostics;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for Class1.
    /// </summary>
    public class MessageBoxManager
    {
        #region Data Members
        static private MessageBoxManager ms_kThis = null;

        static public MessageBoxManager Instance
        {
            get {return ms_kThis;}
        }

        private class AssemblyResource
        {
            public string strResourceName;
            public Assembly kAssembly;
        }

        
        private Hashtable m_hashKnownResources;
        private string m_strBaseRegistryKey;

        #endregion

        #region Constructors

        private MessageBoxManager(string strBaseRegistryKey)
        {
            m_hashKnownResources = new Hashtable();
            m_strBaseRegistryKey = strBaseRegistryKey;
        }

        public static MessageBoxManager Create(string strBaseRegistryKey)
        {
            if (ms_kThis != null)
                return ms_kThis; // already created.

            ms_kThis = new MessageBoxManager(strBaseRegistryKey);
            return ms_kThis;
        }
        #endregion

        #region Assembly Management
        public static void AddAssemblyResources(Assembly a, 
            string strBaseResourceDir)
        {
            string [] resNames = a.GetManifestResourceNames();
            string strNewResource;
            foreach (string resource in resNames)
            {
                if (resource.StartsWith(strBaseResourceDir))
                {
                    strNewResource = resource.Remove(0,
                        strBaseResourceDir.Length);
                    strNewResource = strNewResource.ToUpper();
                    AssemblyResource kAssemblyResource = 
                        new AssemblyResource();
                    kAssemblyResource.kAssembly = a;
                    kAssemblyResource.strResourceName = resource;
                    Instance.m_hashKnownResources.Add(
                        strNewResource, kAssemblyResource);
                }
            }
        }
        #endregion

        #region Registry Interaction
        static public string[] GetMessageBoxSubKeyNames()
        {
            try
            {
                IDictionaryEnumerator kDictEnum = 
                    Instance.m_hashKnownResources.GetEnumerator();
                string[] aStr = 
                    new string[Instance.m_hashKnownResources.Count];
                int i=0;
                while ( kDictEnum.MoveNext() )
                {
                    aStr[i++] = (string)kDictEnum.Key;
                }

                return aStr;
            }
            catch (Exception)
            {
                return null;
            }
        }

        static private RegistryKey GetRegistryKeyFromFilename(
            string strMessageBoxRTFFile)
        {
            strMessageBoxRTFFile = strMessageBoxRTFFile.ToUpper();
            string registryPath = 
                Instance.m_strBaseRegistryKey + strMessageBoxRTFFile;
            try 
            {
                RegistryKey key = 
                    Registry.CurrentUser.OpenSubKey(registryPath, true);
                return key;
            }
            catch (Exception)
            {
                return null;
            }
        }

        static private RegistryKey CreateRegistryKeyFromFilename(
            string strMessageBoxRTFFile)
        {
            strMessageBoxRTFFile = strMessageBoxRTFFile.ToUpper();
            string registryPath = 
                Instance.m_strBaseRegistryKey + strMessageBoxRTFFile;
            try 
            {
                RegistryKey key = 
                    Registry.CurrentUser.CreateSubKey(registryPath);
                return key;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return null;
            }
        }

        static public bool GetDoNotShowAgainFromName(string strName)
        {
            string registryPath = Instance.m_strBaseRegistryKey + strName;
            
            try
            {
                RegistryKey key = 
                    Registry.CurrentUser.OpenSubKey(registryPath, true);
    
                if (key == null)
                    return false;

                return GetDoNotShowAgainFromKey(key);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        }

        static private bool GetDoNotShowAgainFromKey(RegistryKey regKey)
        {
            try
            {
                return Convert.ToBoolean(regKey.GetValue(
                    "DoNotShowAgain",false));
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        }

        static public bool SetDoNotShowAgainToKey(string strName,
            bool bDoNotShowAgain)
        {
            string registryPath = Instance.m_strBaseRegistryKey + strName;
            
            try
            {
                RegistryKey key = 
                    Registry.CurrentUser.OpenSubKey(registryPath, true);

                if (key == null)
                {
                    key = CreateRegistryKeyFromFilename(strName);
                }
               
                SetDoNotShowAgainToKey(key, bDoNotShowAgain);
                return true;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        }

        static private bool SetDoNotShowAgainToKey(RegistryKey regKey,
            bool bDoNotShowAgain)
        {
            try
            {
                regKey.SetValue("DoNotShowAgain", bDoNotShowAgain);
                return true;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return false;
            }
        }

        static object StringToEnum( Type t, string Value )
        {
            foreach ( FieldInfo fi in t.GetFields() )
            {
                if ( fi.Name == Value )
                    return fi.GetValue( null );    // We use null because
            }
            // enumeration values
            // are static

            throw new Exception( 
                string.Format("Can't convert {0} to {1}", Value,
                   t.ToString()) );
        }

        static private DialogResult GetResultFromKey(RegistryKey regKey)
        {
            try
            {
                string strValue = (string)regKey.GetValue("DialogResult", 
                    DialogResult.Cancel.ToString());
                return (DialogResult) StringToEnum(
                    typeof(DialogResult), strValue);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
                return DialogResult.Cancel;
            }
        }

        static private bool SetResultToKey(
            RegistryKey regKey, DialogResult kRes)
        {
            try
            {
                regKey.SetValue("DialogResult", kRes.ToString());
                return true;
            }
            catch(Exception)
            {
                return false;
            }
        }

        static public void ResetEntry(string strMessageBoxRTFFile)
        {
            strMessageBoxRTFFile = strMessageBoxRTFFile.ToUpper();
            RegistryKey kRegKey = GetRegistryKeyFromFilename(
                strMessageBoxRTFFile);
            if (kRegKey != null)
            {
                SetDoNotShowAgainToKey(kRegKey, false);
                kRegKey.Close();
            }
        }

        static public void ResetAllEntries()
        {
            IDictionaryEnumerator kDictEnum = 
                Instance.m_hashKnownResources.GetEnumerator();
            while ( kDictEnum.MoveNext() )
            {
                ResetEntry((string)kDictEnum.Key);
            }
        }

        #endregion

        #region DoMessageBox
        static public DialogResult DoMessageBox(string strMessageBoxRTFFile)
        {
            return DoMessageBox(strMessageBoxRTFFile,  "", null);
        }

        static public DialogResult DoMessageBox(
            string strMessageBoxRTFFile,  string strTitle)
        {
            return DoMessageBox(strMessageBoxRTFFile,  strTitle, null);
        }

        static public DialogResult DoMessageBox(string strMessageBoxRTFFile,   
            string[] strVariables)
        {
            return DoMessageBox(strMessageBoxRTFFile,  "", strVariables);
        }

        static public DialogResult DoMessageBox(
            string strMessageBoxRTFFile,  string strTitle, 
            string[] strVariables)
        {
            return DoMessageBox(strMessageBoxRTFFile,  strTitle, 
              MessageBoxButtons.OKCancel, MessageBoxIcon.None, strVariables);
        }

        static public DialogResult DoMessageBox(
            string strMessageBoxRTFFile,  string strTitle, 
            MessageBoxButtons eButtons, MessageBoxIcon eIcon,
            string[] strVariables)
        {
            
            strMessageBoxRTFFile = strMessageBoxRTFFile.ToUpper();
            DialogResult kRes = DialogResult.Cancel;
            AssemblyResource kResource =  
                (AssemblyResource)Instance.m_hashKnownResources[
                                      strMessageBoxRTFFile];
            
            if (null != kResource)
            {
                bool bShowDialog = true;
                RegistryKey kRegKey = 
                    GetRegistryKeyFromFilename(strMessageBoxRTFFile);
                if (kRegKey != null)
                {
                    bShowDialog = !GetDoNotShowAgainFromKey(kRegKey);
                    kRes = GetResultFromKey(kRegKey);
                }

                if (bShowDialog)
                {
                    Stream RTFStream = 
                        kResource.kAssembly.GetManifestResourceStream(
                        kResource.strResourceName);
                    if (null != RTFStream )
                    {                 
                        StreamReader sr = new StreamReader(RTFStream);
                        string strContents = sr.ReadToEnd();
                        sr.Close();
                        
                        if (strVariables != null)
                        {
                            for (int i = strVariables.Length - 1; i != -1; i--)
                            {
                                string strFind = "%" + i.ToString();
                                string strReplace = strVariables[i];
                                strContents = strContents.Replace(
                                    strFind, strReplace);
                            }
                        }
                        MessageBoxForm kForm = new MessageBoxForm();
                        kForm.MessageText = strContents;
                        kForm.Title = strTitle;
                        kForm.MessageButtons = eButtons;
                        kForm.MessageIcon = eIcon;
                        kRes = kForm.ShowDialog();
                        
                        if (kRegKey == null)
                        {
                            kRegKey = CreateRegistryKeyFromFilename(
                                strMessageBoxRTFFile);
                        }
                        Debug.Assert(kRegKey != null,
                            "Unable to create registry key");
                        if (kRegKey != null)
                        {
                            SetDoNotShowAgainToKey(kRegKey,
                                kForm.DoNotShowAgain);
                            SetResultToKey(kRegKey, kRes);
                            kRegKey.Close();
                        }

                        return kRes;
                    }
                }
            }
            return kRes;
        }

        static public DialogResult DoErrorBox(
            string strMessageBoxRTFFile,  string strTitle, 
            string[] strVariables)
        {
            MessageBoxButtons eButtons = MessageBoxButtons.OK;
            MessageBoxIcon eIcon = MessageBoxIcon.Error;
 
            return DoMessageBox(strMessageBoxRTFFile, strTitle, eButtons, 
                eIcon, strVariables);
        }

        static public DialogResult DoWarningBox(
            string strMessageBoxRTFFile,  string strTitle, 
            string[] strVariables)
        {
            MessageBoxButtons eButtons = MessageBoxButtons.OK;
            MessageBoxIcon eIcon = MessageBoxIcon.Warning;
 
            return DoMessageBox(strMessageBoxRTFFile, strTitle, eButtons, 
                eIcon, strVariables);
        }
        #endregion

        #region Helpers
        static public string ConvertFilename(string strFilename)
        {
            strFilename = strFilename.Replace("\\", "\\\\");
            return strFilename;
        }

        static public string CreateBulletedList(ArrayList kList)
        {
            return CreateList(kList, true);
        }

        static public string CreateNonBulletedList(ArrayList kList)
        {
            return CreateList(kList, false);
        }

        static protected string CreateList(ArrayList kList, bool bUseBullets)
        {
            string[] strs = new string[kList.Count];
            for (int i = 0; i < kList.Count; i++)
            {
                strs[i] = (string) kList[i];
            }

            if (bUseBullets)
            {
                return CreateBulletedList(strs);
            }
            else
            {
                return CreateNonBulletedList(strs);
            }
        }

        static public string CreateBulletedList(string[] strs)
        {
            string strList = "";
            string str = "";
            if (strs.Length == 1)
            {
                str = strs[0];
                strList += "\\pard{\\pntext\\f1\\'B7\\tab}{\\*\\pn\\pnlvlblt"+
                    "\\pnf1\\pnindent0{\\pntxtb\\'B7}}\\fi-720\\li1440 " + 
                    str + "\\par\n\\pard\\par\n";
                return strList;
            }

            for (int i = 0; i < strs.Length -1 ; i++)
            {
                str = strs[i];
                if (strList.Length == 0)
                {
                    strList += 
                        "\\pard{\\pntext\\f1\\'B7\\tab}{\\*\\pn\\pnlvlblt"+
                        "\\pnf1\\pnindent0{\\pntxtb\\'B7}}\\fi-720\\li1440 " + 
                        str + "\\par\n";
                }
                else
                {
                    if (str.Length != 0)
                        strList += 
                        "{\\pntext\\f1\\'B7\\tab}" + str + "\\par\n";
                }
            }

            
            str = strs[strs.Length -1];
            strList += 
                "{\\pntext\\f1\\'B7\\tab}" + str + "\\b0\\par\n\\pard\\b\\par";
            return strList;
        }

        static public string CreateNonBulletedList(string[] strs)
        {
            string strList = string.Empty;
            for (int i = 0; i < strs.Length; i++)
            {
                strList += strs[i];
                if (i != strs.Length - 1)
                {
                    strList += "\\par ";
                }
            }

            return strList;
        }
        #endregion
    }
}
