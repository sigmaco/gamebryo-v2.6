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
using System.Runtime.InteropServices;
using System.Text;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// This class contains definitions for native Win32 calls
    /// </summary>
    public class NativeWin32
    {
        public NativeWin32()
        {
        }
        
        #region Constants
        internal const int MAX_PATH = 260;
        #endregion

        #region Types

        #endregion

        #region Kernal32 Win32API

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        internal static extern int GetLongPathName(
            [MarshalAs(UnmanagedType.LPTStr)]
            string path,
            [MarshalAs(UnmanagedType.LPTStr)]
            StringBuilder longPath,
            int longPathLength);
        #endregion

        #region User32 Win32API

        [DllImport("user32.dll", SetLastError=true, CharSet=CharSet.Auto)]
        internal static extern IntPtr SetActiveWindow(IntPtr hwnd);

        #endregion
    }

}
