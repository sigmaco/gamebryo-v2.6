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
using System.Diagnostics;
using System.Collections;
using Microsoft.Win32;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    public delegate void MRUFileSelectedCB(object o, System.EventArgs e);

    public class MRUManager
    {
        static private string ms_strMRURegPath = null;
        static private int ms_iMaxFiles = 10;

        static public void SetMRURegPath(string strPath)
        {
            ms_strMRURegPath = strPath;
        }

        static public void DeleteMRUReg()
        {
            if (ms_strMRURegPath == null)
                return;
            
            RegistryKey MRUKey = 
                Registry.CurrentUser.CreateSubKey(ms_strMRURegPath);

            if (MRUKey == null)
                return;

            // remove existing subkeys...
            string[] strValueNames = MRUKey.GetValueNames();
            foreach(string strSubKey in strValueNames)
            {
                MRUKey.DeleteValue(strSubKey, false);
            }

            MRUKey.Close();
        }

        static public bool SetMRUList(ArrayList alMRU)
        {
            if (alMRU == null || ms_strMRURegPath == null)
                return false;

            DeleteMRUReg();
           
            RegistryKey MRUKey = 
                Registry.CurrentUser.CreateSubKey(ms_strMRURegPath);

            if (MRUKey == null)
                return false;

            // Add the new keys
            int iIndex = 0;
            foreach(string strSubKey in alMRU)
            {
                // This will ensure that if there are additional files in
                // the registry (from a previous version), they will get
                // wiped out.
                if (iIndex >= ms_iMaxFiles)
                    break;

                string strVal = "MRU:" + iIndex.ToString();
                MRUKey.SetValue(strVal, strSubKey);

                iIndex++;
            }

            MRUKey.Close();

            return true;
        }

        static public ArrayList GetMRUList()
        {
            ArrayList alMRU = new ArrayList();

            if (ms_strMRURegPath == null)
                return null;

            RegistryKey MRUKey = 
                Registry.CurrentUser.OpenSubKey(ms_strMRURegPath);

            if (MRUKey == null)
                return null;

            foreach(string strSubKey in MRUKey.GetValueNames())
            {
                string strFileName = (string)MRUKey.GetValue(strSubKey,"");

                if (strFileName == null || strFileName.Length == 0)
                    break;

                alMRU.Add(strFileName);
            }

            MRUKey.Close();
            return alMRU;
        }

        static public void PopulateRecentFiles(
            MenuItem miFileMRU, ArrayList alMRU,
            MRUFileSelectedCB cbMRU)
        {
            miFileMRU.MenuItems.Clear();

            foreach(string strFile in alMRU)
            {
                MenuItem miFile = new MenuItem(strFile);
                miFile.Click += new EventHandler(cbMRU);
                miFileMRU.MenuItems.Add(miFile);
            }
        }

        static public void AddFile(string strFileName)
        {
            RemoveFile(strFileName);

            ArrayList alMRU = GetMRUList();

            if (alMRU == null)
            {
                alMRU = new ArrayList();
            }

            if ( alMRU.Count == ms_iMaxFiles )
                alMRU.RemoveAt(ms_iMaxFiles - 1);

            alMRU.Insert(0, strFileName);

            bool bResult = SetMRUList(alMRU);
            Debug.Assert(bResult == true);
        }

        static public void RemoveFile(string strFileName)
        {
            ArrayList alMRU = GetMRUList();

            if (alMRU == null)
                return;

            int i = 0;
            foreach(string strFileInReg in alMRU)
            {
                if (strFileInReg == null)
                    continue;

                if (strFileInReg == strFileName)
                {
                    alMRU.RemoveAt(i);
                    break;
                }
                i++;
            }

            bool bResult = SetMRUList(alMRU);
            Debug.Assert(bResult == true);
        }
    }
}