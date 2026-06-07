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
using System.Drawing;
using System.Reflection;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using WeifenLuo.WinFormsUI;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
    /// <summary>
    /// Summary description for CommandWidgetAdapter.
    /// </summary>
    public class CommandPanelAdapter : CommandPanel
    {
        #region Private Data
        private Container components = null;
        private Panel m_panelHost;
        private Form m_wrappedForm;
        /// <summary>
        /// storage for a hack for Weifen Luo bug
        /// </summary>
        private Control m_dummyControl;
        #endregion

        public CommandPanelAdapter(Form wrappedForm) 
        {
            m_wrappedForm = wrappedForm;

            this.Icon = m_wrappedForm.Icon;

            //Note: the following is a hack to fix an appearent bug in the
            //Weifen Luo docking manager (As of v. 0.99.0.3)
            //where the last control added will not behave properly if the
            //Anchor property is has the AnchorStyle.Bottom flag set
            int controlCount = wrappedForm.Controls.Count;
            if (controlCount > 0)
            {
                Control lastControl = 
                    m_wrappedForm.Controls[controlCount-1];
                Button dummyButton = new Button();
                dummyButton.Visible = true;
                dummyButton.Enabled = false;
                dummyButton.Size = lastControl.Size;
                dummyButton.Location = lastControl.Location;
                dummyButton.Anchor = AnchorStyles.Top | AnchorStyles.Left;
                wrappedForm.Controls.Add(dummyButton);
                m_dummyControl = dummyButton;
                //This handler will hide the control after the smoke clears
                Application.Idle += new EventHandler(AppIdleHack);
            }
            //end hack
            InitializeComponent();

            SetWindowDock();

            m_wrappedForm.TopLevel = false;
            //Set up parameters for wrapped form
            m_wrappedForm.Anchor = AnchorStyles.Top | AnchorStyles.Bottom |
                AnchorStyles.Left | AnchorStyles.Right;
            m_wrappedForm.Dock = DockStyle.Fill;
            m_wrappedForm.FormBorderStyle = FormBorderStyle.None;
            this.MinimumSize = m_wrappedForm.MinimumSize;
            this.m_panelHost.Controls.Add(m_wrappedForm);
            m_wrappedForm.Show();
        }


        public Form InternalForm
        {
            get
            { return m_wrappedForm; }
        }



        public override string Text
        {
            get
            {
                if (m_wrappedForm != null)
                    return m_wrappedForm.Text;
                return null;
            }
            set
            {
                m_wrappedForm.Text = value;
            }
        }

        protected override string GetPersistString()
        {
            return Text;
        }



        /// <summary>
        /// This method is related to the hack mentioned in the constructor
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AppIdleHack(object sender, EventArgs e)
        {
            m_dummyControl.Visible = false;
            Application.Idle -= new EventHandler(AppIdleHack);

        }

        #region Windows Form Designer generated code


        private void InitializeComponent()
        {
            this.m_panelHost = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // m_panelHost
            // 
            this.m_panelHost.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_panelHost.Location = new System.Drawing.Point(0, 0);
            this.m_panelHost.Name = "m_panelHost";
            this.m_panelHost.Size = new System.Drawing.Size(300, 300);
            this.m_panelHost.TabIndex = 3;
            // 
            // CommandPanelAdapter
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(300, 300);
            this.Controls.Add(this.m_panelHost);
            this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)(((((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft) 
                | WeifenLuo.WinFormsUI.DockAreas.DockRight) 
                | WeifenLuo.WinFormsUI.DockAreas.DockTop) 
                | WeifenLuo.WinFormsUI.DockAreas.DockBottom)));
            this.DockPadding.Bottom = 20;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.HideOnClose = true;
            this.Name = "CommandPanelAdapter";
            this.ResumeLayout(false);

        }

        #endregion

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        private void SetWindowDock()
        {
            DockPositionAttribute position = 
                GetDockPositionAttribute(m_wrappedForm);

            DefaultDock defaultDock;// = GetDock(m_wrappedForm);
            if (position != null)
            {
                defaultDock = position.Position;
            }
            else
            {
                defaultDock = DefaultDock.DockRight;
            }


            DockState dockState = 
                (DockState) Enum.Parse(typeof(DockState),
                 defaultDock.ToString());

            //if the panel states "document" is it's default dock state,
            //then allow it to be docked in the "document" state
            if (dockState == DockState.Document)
            {
                this.DockableAreas = 
                    (DockAreas) ( DockAreas.Document);
               
            }

            bool bAllowFloat = true;
            if (position != null)
            {
                bAllowFloat = position.AllowFloat;
            }
            if (bAllowFloat)
            {
                this.DockableAreas = 
                    (DockAreas) (this.DockableAreas | DockAreas.Float);
                
            }
            else
            {
                this.DockableAreas = 
                    (DockAreas) (this.DockableAreas & ~DockAreas.Float);
                
            }

            this.ShowHint = dockState;
        }

        private DockPositionAttribute GetDockPositionAttribute(object obj)
        {
            Type type = obj.GetType();
            object[] attributes = 
                type.GetCustomAttributes(typeof(DockPositionAttribute), false);
            if (attributes != null && attributes.Length == 1)
            {
                DockPositionAttribute dock = 
                    (DockPositionAttribute)attributes[0];
                return dock;
            }
            return null;
        }

        private DefaultDock GetDock(object obj)
        {
            DockPositionAttribute position = GetDockPositionAttribute(obj);
            if (position != null)
            {
                return position.Position;
            }
            return DefaultDock.DockRight;
        }

        
    }
}
