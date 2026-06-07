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
using System.Collections;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
    /// <summary>
    /// Invalidates forms according to a timer.
    /// </summary>
    public class Invalidator
    {
        private static Invalidator ms_pmThis = null;
        public static Invalidator Instance
        {
            get
            {
                Debug.Assert(ms_pmThis != null,
                    "Static Invalidator pointer is null.");
                return ms_pmThis;
            }
        }

        public static void Init()
        {
            ms_pmThis = new Invalidator();
        }

        public static void Shutdown()
        {
            ms_pmThis.Dispose();
            ms_pmThis = null;
        }

        private System.Windows.Forms.Timer m_pmTimer;
        private ArrayList m_pmControls = new ArrayList();

        public Invalidator()
        {
            m_pmTimer = new System.Windows.Forms.Timer();
            m_pmTimer.Interval = 30;
            m_pmTimer.Tick += new EventHandler(this.OnTick);
        }

        public void Dispose()
        {
            m_pmTimer.Stop();
            m_pmTimer.Dispose();
            m_pmTimer = null;
        }

        public bool TimerEnabled
        {
            get
            {
                return m_pmTimer.Enabled;
            }
            set
            {
                m_pmTimer.Enabled = value;
            }
        }

        public int TimerInterval
        {
            get
            {
                return m_pmTimer.Interval;
            }
            set
            {
                m_pmTimer.Interval = value;
            }
        }

        private void OnTick(object sender, EventArgs e)
        {
            Update(false);
        }

        public void RegisterControl(Control control)
        {
            m_pmControls.Add(control);
        }

        public void UnregisterControl(Control control)
        {
            m_pmControls.Remove(control);
        }

        // Limit frame rate to 100 FPS.
        private const float ms_fMinFrameTime = 0.01f;

        private float m_fLastTime = float.NegativeInfinity;
        
        public void Update()
        {
            Update(true);
        }

        private void Update(bool bForceUpdate)
        {
            float fCurrentTime = MUtility.GetCurrentTimeInSec();
            if (m_fLastTime == float.NegativeInfinity)
            {
                m_fLastTime = fCurrentTime;
            }
            if (!bForceUpdate &&
                fCurrentTime - m_fLastTime < ms_fMinFrameTime)
            {
                return;
            }

            m_fLastTime = fCurrentTime;

            foreach (Control control in m_pmControls)
            {
                control.Invalidate();
            }
        }
    }
}
