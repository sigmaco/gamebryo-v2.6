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
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs
{
    /// <summary>
    /// Summary description for Win32Utility.
    /// </summary>
    public class Win32Utility
    {

        public static readonly int FLASHW_STOP = 0;
        public static readonly int FLASHW_CAPTION = 1;
        public static readonly int FLASHW_TRAY = 2;
        public static readonly int FLASHW_ALL = 3;
        public static readonly int FLASHW_TIMER = 4;
        public static readonly int FLASHW_TIMERNOFG = 12;

        [StructLayout(LayoutKind.Sequential)]
        public struct FLASHWINFO 
        {
            public Int32 cbSize;
            public IntPtr hwnd;
            public Int32 dwFlags;
            public Int32 uCount;
            public Int32 dwTimeout;
        }

        [DllImport("user32.dll")]
        private static extern int FlashWindowEx(ref FLASHWINFO pwfi);


        public static int FlashForm(Form form, int flag, int count, int timeout)
        {
            FLASHWINFO pwfi = new FLASHWINFO();
            pwfi.cbSize = Marshal.SizeOf(pwfi);
            pwfi.hwnd =  form.Handle ;
            pwfi.dwFlags = flag;
            pwfi.uCount = count;
            pwfi.dwTimeout = timeout;
            Int32 intRet = FlashWindowEx(ref pwfi);    
            return intRet;
        }

    }

}
