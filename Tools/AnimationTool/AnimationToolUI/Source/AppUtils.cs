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

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for AppUtils.
    /// </summary>
    public class AppUtils
    {
        static public string ExtractDirFromPath(string strAbsolutePath)
        {
            string delimStr = "\\/";
            char [] delimiter = delimStr.ToCharArray();
            string [] splitAbsolute = strAbsolutePath.Split(delimiter);
            string strDir = "";

            for (int i = 0; i < splitAbsolute.Length - 1; i++)
            {
                strDir += splitAbsolute[i] + '\\';
            }
            return strDir;
        }

        static public bool EqualsNoCase(string strA, string strB)
        {
            strA = strA.ToUpper();
            strB = strB.ToUpper();
            return (strA.CompareTo(strB) == 0);
        }

        static public string ExtractFileFromPath(string strAbsolutePath)
        {
            string delimStr = "\\/";
            char [] delimiter = delimStr.ToCharArray();
            string [] splitAbsolute = strAbsolutePath.Split(delimiter);
            string strFile = splitAbsolute[splitAbsolute.Length - 1];
            return strFile;
        }

        static public string CreateRelativePath(string strAbsolutePath,
            string strRelativeToPath)
        {
            strAbsolutePath = strAbsolutePath.ToUpper();
            strRelativeToPath = strRelativeToPath.ToUpper();
            string delimStr = "\\/";
            char [] delimiter = delimStr.ToCharArray();
            string [] splitAbsolute = null;
            string [] splitRelativeToPath = null;

            splitAbsolute = strAbsolutePath.Split(delimiter);
            splitRelativeToPath = strRelativeToPath.Split(delimiter);
            string strCommonPath = new string(' ', 1);
            strCommonPath.Trim();
            string strRelativePath = strCommonPath;

            int iAbsolute = 0;
            int iRelativeTo = 0;
            while (iAbsolute < splitAbsolute.Length && 
                   iRelativeTo < strRelativeToPath.Length)
            {
                if (splitAbsolute[iAbsolute].Length == 0)
                {
                    iAbsolute++;
                    continue;
                }

                if (splitRelativeToPath[iRelativeTo].Length == 0)
                {
                    iRelativeTo++;
                    continue;
                }

                if (splitAbsolute[iAbsolute] == 
                    splitRelativeToPath[iRelativeTo])
                {
                    strCommonPath += splitAbsolute[iAbsolute] + '\\';
                    iRelativeTo++;
                    iAbsolute++;
                }
                else
                {
                    break;
                }
            }

            // If the drive isn't even the same, return the full 
            // path
            if (iAbsolute == 0 || iRelativeTo == 0)
                return strAbsolutePath;
            
            for (int i = iRelativeTo; i < splitRelativeToPath.Length - 1; i++)
            {
                if (splitRelativeToPath[i].Length == 0)
                    continue;

                strRelativePath += ".." + '\\';
            }

            for (int i = iAbsolute; i < splitAbsolute.Length - 1; i++)
            {
                if (splitAbsolute[i].Length == 0)
                    continue;

                strRelativePath += splitAbsolute[i] + '\\';
            }

            strRelativePath += splitAbsolute[splitAbsolute.Length - 1];
            return strRelativePath;
        }

        [System.Runtime.InteropServices.DllImport("kernel32.dll")]
        static extern uint GetLongPathName(string strShortName,
            System.Text.StringBuilder sbLongNameBuff, uint uiBufferSize);

        public static string ToLongPathName(string strShortName)
        {
            System.Text.StringBuilder sbLongNameBuffer =
                new System.Text.StringBuilder(512);
            uint uiBufferSize = (uint) sbLongNameBuffer.Capacity;

            GetLongPathName(strShortName, sbLongNameBuffer, uiBufferSize);

            return sbLongNameBuffer.ToString();
        }
    }
}
