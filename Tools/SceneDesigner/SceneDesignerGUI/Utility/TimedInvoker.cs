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
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;
using ThreadState = System.Threading.ThreadState;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
    /// <summary>
    /// Summary description for TimedInvoker.
    /// </summary>
    public class TimedInvoker
    {

        #region Private Data
        private Delegate m_callBack;
        private ISynchronizeInvoke m_invokableObject;
        private Thread m_thread;
        private TimeSpan m_interval;
        private TimeSpan m_timeOut;
        private bool m_bIsInvoking;
        private object m_lock;
        #endregion 

        public TimedInvoker()
        {
            m_lock = new object();
            m_bIsInvoking = false;
            m_timeOut = new TimeSpan(0, 0, 5);//default 5 second timeout
        }

        public Delegate CallBack
        {        
            set
            {
                lock(m_lock)
                {        
                    m_callBack = value;
                }
            }
        }

        public ISynchronizeInvoke SychronizedObject
        {
            set
            {
                lock (m_lock)
                {
                    m_invokableObject = value;
                }
            }
        }

        public TimeSpan Interval
        {
            set
            {
                lock (m_lock)
                {
                    m_interval = value;
                }
            }
        }

        public TimeSpan TimeOut
        {
            set
            {
                m_timeOut = value;
            }
        }
       
        public bool IsRunning
        {
            get
            {
                lock(m_lock)
                {
                    return (m_thread.ThreadState & ThreadState.Running) ==
                        ThreadState.Running;
                }
            }
        }

        public bool IsInvoking
        {
            get
            {
                lock(m_lock)
                {
                    return m_bIsInvoking;
                }
            }
        }

        public void Start()
        {
            lock (m_lock)
            {
                m_thread = new Thread(new ThreadStart(Run));
            }
            m_thread.Start();
        }

        public void Stop()
        {
            m_thread.Interrupt();
            if (!m_thread.Join(1000))
            {
                m_thread.Abort();
                Console.WriteLine("Forcibly terminating thread");
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <remarks>
        /// Control.Invoke has a known bug in Dot Net 1.1 sp1
        /// where it will hang(race condition) under stress.
        /// BeginInvoke is used instead, with a timeout value
        /// </remarks>
        private void Run()
        {
            try
            {
                while (true)
                {
                    //lock not needed for a sychronous Invoke
                    //Note: this means that all of the other lock calls are 
                    //probably unnesseary too, but I'll keep them in place
                    //since they may come in handy if we need to do 
                    //asyncronous calls, and their relative overhead is 
                    //minimal
                    //lock (m_lock)
                    //{
                        m_bIsInvoking = true;
                        IAsyncResult result = m_invokableObject.BeginInvoke(
                            m_callBack, new object[] {null, null});
                        WaitHandle waitHandle = result.AsyncWaitHandle;
                        if (waitHandle.WaitOne(m_timeOut, true))
                        {
                            m_invokableObject.EndInvoke(result);
                        }
                        else
                        {
                            Debug.WriteLine(
                                "Delegate Object appears to be hung");
                        }
                        m_bIsInvoking = false;
                    //}
                    Thread.Sleep(m_interval);                    
                }
            }
            catch (ThreadInterruptedException)
            {
                return;
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString(),
                    "Exception thrown in timer code");

            }
        }


    }
}
