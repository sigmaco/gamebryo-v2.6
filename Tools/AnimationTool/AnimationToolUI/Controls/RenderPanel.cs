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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for RenderPanel.
    /// </summary>
    public class RenderPanel : System.Windows.Forms.UserControl
    {
        private bool m_bRendererCreated = false;
        private bool m_bAttemptCreate = true;
        private bool m_bNeedsRecreate = false;
        private bool m_bInitializingRenderer = false;

        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        private static uint NumInstances = 0;

        public RenderPanel()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            Debug.Assert(NumInstances == 0);
            NumInstances++;            
        }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                NumInstances--;
                Debug.Assert(NumInstances == 0, 
                    "There should only ever be one un-disposed" + 
                    " version of this class in memory at a time.");
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        protected override void OnResize(System.EventArgs e)
        {
            if (MFramework.InstanceIsValid())
            {
                m_bNeedsRecreate = true;

                MFramework.Instance.Renderer.ActiveCamera.ViewHeight = Height;
                MFramework.Instance.Renderer.ActiveCamera.ViewWidth = Width;
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_FRUSTUM);
            }

            base.OnResize(e);
        }

        protected override void OnPaint(PaintEventArgs e)
        {   
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                base.OnPaint(e);
            }
            else if (MFramework.Instance.Renderer.ActiveCamera == null ||
                !MFramework.Instance.Renderer.ActiveCamera.Active)
            {
                base.OnPaint(e);
            }
            else
            {
                if (!m_bRendererCreated && m_bAttemptCreate)
                {
                    InitRenderer();
                }
                else if (m_bRendererCreated && m_bNeedsRecreate)
                {
                    if (MFramework.Instance.Renderer.ReCreate(Handle))
                    {
                        m_bNeedsRecreate = false;
                    }
                }
                MFramework.Instance.Update();
            }
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                base.OnPaintBackground(pevent);
            }
            else if (MFramework.Instance.Renderer.ActiveCamera == null ||
                !MFramework.Instance.Renderer.ActiveCamera.Active)
            {
                base.OnPaintBackground(pevent);
            }
            else if (!MFramework.Instance.Renderer.Active)
            {
                base.OnPaintBackground(pevent);
            }
        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            // 
            // RenderPanel
            // 
            this.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.Name = "RenderPanel";
            this.Load += new System.EventHandler(this.RenderPanel_Load);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.RenderPanel_MouseUp);
            this.MouseEnter += new System.EventHandler(this.RenderPanel_MouseEnter);
            this.MouseHover += new System.EventHandler(this.RenderPanel_MouseHover);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.RenderPanel_KeyUp);
            this.Leave += new System.EventHandler(this.RenderPanel_Leave);
            this.DoubleClick += new System.EventHandler(this.RenderPanel_DoubleClick);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.RenderPanel_MouseMove);
            this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.RenderPanel_MouseWheel);
            this.MouseLeave += new System.EventHandler(this.RenderPanel_MouseLeave);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.RenderPanel_MouseDown);

        }
        #endregion

        private IntPtr m_parentHandle;
        public IntPtr ParentHandle
        {
            get
            {
                return m_parentHandle;
            }
            set
            {
                m_parentHandle = value;
            }
        }

        public void InitRenderer()
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid() || m_bInitializingRenderer)
            {
                return;
            }

            m_bInitializingRenderer = true;

            IntPtr childHandle = Handle;
            MFramework.Instance.Animation.OnNewKFM +=
                new MAnimation.__Delegate_OnNewKFM(this.OnKFMChanged);
            MFramework.Instance.Animation.OnKFMLoaded +=
                new MAnimation.__Delegate_OnKFMLoaded(this.OnKFMChanged);
            MFramework.Instance.Animation.OnActorManagerCreated +=
                new MAnimation.__Delegate_OnActorManagerCreated(
                this.OnActorManagerCreated);
            MFramework.Instance.Animation.OnModelPathAndRootChanged +=
                new MAnimation.__Delegate_OnModelPathAndRootChanged(
                this.OnModelPathAndRootChanged);
            MFramework.Instance.Animation.OnSequenceAdded +=
                new MAnimation.__Delegate_OnSequenceAdded(
                this.OnSequenceAdded);

            MFramework.Instance.Input.InitWindowHandle(this.Handle);
            MFramework.Instance.Renderer.ActiveCamera.ViewHeight = Height;
            MFramework.Instance.Renderer.ActiveCamera.ViewWidth = Width;
            
            if (MFramework.Instance.SceneGraph.SceneCount > 0)
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.ZOOM_EXTENTS);
            }

            MFramework.Instance.Input.RefreshData();
            MColor kColor = new MColor(BackColor.R /255.0f,
                BackColor.G /255.0f, BackColor.B /255.0f);
            MFramework.Instance.Renderer.Color = kColor;

            if (MFramework.Instance.Renderer.Create(childHandle, ParentHandle)
                == false)
            {
                MessageBox.Show("Renderer creation has failed.",
                    "Renderer creation failure!", MessageBoxButtons.OK,
                    MessageBoxIcon.Exclamation);
                m_bAttemptCreate = false;
            }
            else
            {
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.ZOOM_EXTENTS);

                m_bRendererCreated = true;
            }

            m_bInitializingRenderer = false;
        }

        private void OnModelPathAndRootChanged(string strModelPath,
            string strModelRoot)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            OnActorManagerCreated();
        }

        private void OnSequenceAdded(MSequence seq)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            if (MFramework.Instance.Animation.Sequences.Count == 1)
            {
                OnActorManagerCreated();
            }
        }

        private void OnActorManagerCreated()
        {
            try 
            {
                // To support design view of this control in Visual Studio.
                if (!MFramework.InstanceIsValid())
                {
                    return;
                }

                MFramework.Instance.Input.RefreshData();
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_ORBIT_POINT);
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_BACKGROUND_HEIGHT);
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.RESET_FRUSTUM);
                MFramework.Instance.Input.SubmitCommand(
                    MUICommand.CommandType.ZOOM_EXTENTS);
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void OnKFMChanged()
        {
            try 
            {
                // To support design view of this control in Visual Studio.
                if (!MFramework.InstanceIsValid())
                {
                    return;
                }

                MFramework.Instance.Input.RefreshData(); 
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void RenderPanel_DoubleClick(object sender,
            System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.DoubleClick();
        }

        private MUIState.MouseButtonType GetMouseButtonConversion(
            System.Windows.Forms.MouseEventArgs e)
        {
            MUIState.MouseButtonType eType =
                MUIState.MouseButtonType.MAX_MOUSE_BUTTON_TYPES;

            switch(e.Button)
            {
                case MouseButtons.Left:
                    eType = MUIState.MouseButtonType.LMB;
                    break;
                case MouseButtons.Middle:
                    eType = MUIState.MouseButtonType.MMB;
                    break;
                case MouseButtons.Right:
                    eType = MUIState.MouseButtonType.RMB;
                    break;
            };

            return eType;
        }

        public void ForceMouseShow()
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.ForceMouseShow();
            //MFramework.Instance.Input.ActiveUIState.MouseUp(
            //    NiManagedToolInterface.MUIState.MouseButtonType.MMB,0,0);
        }

        private void RenderPanel_MouseDown(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MUIState.MouseButtonType eType = GetMouseButtonConversion(e);
            MFramework.Instance.Input.ActiveUIState.MouseDown(eType, e.X,
                e.Y);
        }

        private void RenderPanel_MouseEnter(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.MouseEnter();
        }

        private void RenderPanel_MouseHover(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.MouseHover();
        }

        private void RenderPanel_MouseLeave(object sender, System.EventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.MouseLeave();

            ForceMouseShow();
        }

        private void RenderPanel_MouseMove(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.MouseMove(e.X, e.Y);
        }

        private void RenderPanel_MouseUp(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MUIState.MouseButtonType eType = GetMouseButtonConversion(e);
            MFramework.Instance.Input.ActiveUIState.MouseUp(eType, e.X, e.Y);
        }

        private void RenderPanel_MouseWheel(object sender,
            System.Windows.Forms.MouseEventArgs e)
        {
            // To support design view of this control in Visual Studio.
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            MFramework.Instance.Input.ActiveUIState.MouseWheel(e.Delta);
        }

        private void RenderPanel_KeyUp(
            object sender, System.Windows.Forms.KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F1 && HelpManager.HelpExists())
            {
                HelpManager.DoHelp(this, 
                    @"User_Interface_Description/"+
                    "Render_View.htm#Render_View_Display");
            }
        }

        private void RenderPanel_Load(object sender, System.EventArgs e)
        {
            if (!MFramework.InstanceIsValid())
            {
                return;
            }

            //Pass in the new window handle here
            MFramework.Instance.Renderer.RenderWindowHandle = Handle.ToInt32();
        }

        private void RenderPanel_Leave(object sender, System.EventArgs e)
        {
            int x=0;
            x++;
        }
    }
}
