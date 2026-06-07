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
using System.Collections;
using System.Diagnostics;
using System.Windows.Forms;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// class Invalidator: 
    /// </summary>
    public class Invalidator
    {
        private static Invalidator ms_pkThis = null;
        public static Invalidator Instance
        {
            get
            {
                Debug.Assert(ms_pkThis != null,
                    "Static Invalidator pointer is null");
                return ms_pkThis;
            }
        }

        public static void Init()
        {
            ms_pkThis = new Invalidator();
        }

        public static void Shutdown()
        {
            ms_pkThis.Dispose();
            ms_pkThis = null;
        }

        private System.Windows.Forms.Timer m_Timer;
        private ArrayList m_aControls = new ArrayList();

        public Invalidator() 
        {
            m_Timer = new System.Windows.Forms.Timer();
            m_Timer.Interval = 1;
            m_Timer.Tick += new EventHandler(this.OnTick);
            m_Timer.Start();
        }

        public void Dispose()
        {
            m_Timer.Stop();
            m_Timer.Dispose();
            m_Timer = null;
        }

        private void OnTick(object sender, EventArgs e)
        {
            Update(false);
        }

        public void RegisterControl(Control control)
        {
            m_aControls.Add(control);
        }

        public void UnregisterControl(Control control)
        {
            m_aControls.Remove(control);
        }

        // Limit frame rate to 100 FPS.
        private const float ms_fMinFrameTime = 0.01f;

        private float m_fLastTime = float.NegativeInfinity;
           
        public void Update(bool bForceUpdate)
        {
            float fCurrentTime = MUtility.GetCurrentTimeInSec();
            if (m_fLastTime == float.NegativeInfinity)
            {
                m_fLastTime = fCurrentTime;
            }
            if (!bForceUpdate && fCurrentTime - m_fLastTime < ms_fMinFrameTime)
            {
                return;
            }

            m_fLastTime = fCurrentTime;

            foreach (Control control in m_aControls)
            {
                control.Invalidate();
            }
        }
    }
}
