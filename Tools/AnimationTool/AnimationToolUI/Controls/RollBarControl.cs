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

using System.Reflection;
using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for RollBarControl.
    /// </summary>
    public class RollBarControl : System.Windows.Forms.UserControl, IPostShow
    {
        #region Delegates

        private AppEvents.EventHandler_RollbarSplitterMoved 
            m_RollbarSplitterMoved = null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_RollbarSplitterMoved = 
                new AppEvents.EventHandler_RollbarSplitterMoved(
                    OnRollbarSplitterMoved);

            // Register delegates.
            AppEvents.RollbarSplitterMoved += m_RollbarSplitterMoved;
            Debug.Assert(AppEvents.GetRollbarSplitterMovedCount() == 1,
                "Only one handler for the RollbarSplitterMovedEvent can be " +
                "registered at a time.");
        }

        private void UnregisterDelegates()
        {
            if (m_RollbarSplitterMoved != null)
                AppEvents.RollbarSplitterMoved -= m_RollbarSplitterMoved;
        }

        #endregion

        private const int PLUS_IMAGE_INDEX = 0;
        private const int MINUS_IMAGE_INDEX = 1;

        private UserControl[] m_aucUserCtrls;
        private string[] m_astrNames;
        private bool[] m_abCheckStates;
        private int m_iPadding = 0;
        private CheckBox[] m_acbButtons;
        private Panel[] m_apnlPanels;
        public System.Windows.Forms.CheckBox cbTemplate;
        private System.Windows.Forms.ImageList ilImages;
        private System.ComponentModel.IContainer components;

        public void PostShow()
        {
            int i=0;
            foreach(UserControl uc in m_aucUserCtrls)
            {
                if (m_abCheckStates[i] == true)
                {
                    if ( ( uc is IPostShow ) )
                    {
                        ((IPostShow)uc).PostShow();
                    }
                }
                i++;
            }
        }

        public System.Windows.Forms.UserControl[] UserControls
        {
            get {return m_aucUserCtrls;}
        }

        public int Padding 
        {
            get {return m_iPadding;}
            set
            {
                m_iPadding = value;
                PlaceObjects();
            }
        }

        public RollBarControl(UserControl[] aucUserCtrls, string[] astrNames,
            bool[] abCheckStates, int iPadding)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            Debug.Assert(aucUserCtrls.Length == astrNames.Length &&
                aucUserCtrls.Length == abCheckStates.Length, "Array " +
                "lengths must be equal.");

            m_aucUserCtrls = aucUserCtrls;
            m_astrNames = astrNames;
            m_abCheckStates = abCheckStates;
            m_iPadding = iPadding;
        }

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                UnregisterDelegates();
                if (m_aucUserCtrls != null)
                {
                    foreach (UserControl uc in m_aucUserCtrls)
                    {
                        uc.Dispose();
                    }
                    m_aucUserCtrls = null;
                }

                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Component Designer generated code
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(RollBarControl));
            this.cbTemplate = new System.Windows.Forms.CheckBox();
            this.ilImages = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // cbTemplate
            // 
            this.cbTemplate.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.cbTemplate.Appearance = System.Windows.Forms.Appearance.Button;
            this.cbTemplate.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.cbTemplate.Cursor = System.Windows.Forms.Cursors.Default;
            this.cbTemplate.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.cbTemplate.ForeColor = System.Drawing.SystemColors.ControlText;
            this.cbTemplate.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cbTemplate.ImageIndex = 0;
            this.cbTemplate.ImageList = this.ilImages;
            this.cbTemplate.Location = new System.Drawing.Point(8, 8);
            this.cbTemplate.Name = "cbTemplate";
            this.cbTemplate.Size = new System.Drawing.Size(192, 24);
            this.cbTemplate.TabIndex = 0;
            this.cbTemplate.Text = "cbTemplate";
            this.cbTemplate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ilImages
            // 
            this.ilImages.ImageSize = new System.Drawing.Size(16, 16);
            this.ilImages.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilImages.ImageStream")));
            this.ilImages.TransparentColor = System.Drawing.Color.White;
            // 
            // RollBarControl
            // 
            this.AutoScroll = true;
            this.Controls.Add(this.cbTemplate);
            this.Name = "RollBarControl";
            this.Size = new System.Drawing.Size(208, 328);
            this.Load += new System.EventHandler(this.RollBarControl_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void RollBarControl_Load(object sender, System.EventArgs e)
        {
            RegisterDelegates();
            this.SetStyle( ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint | ControlStyles.DoubleBuffer,true);
            CreateObjects();
            PlaceObjects();
            cbTemplate.Visible = false;
        }

        private void CreateObjects()
        {
            // array of checkbuttons
            int iSize = m_aucUserCtrls.Length;
            m_acbButtons = new CheckBox[iSize];
            m_apnlPanels = new Panel[iSize];
             
            int i=0;
            foreach (string strName in m_astrNames)
            {
                if (strName == null)
                {
                    // nameless sections become always visible.
                    m_acbButtons[i] = null;
                }
                else
                {
                    // Make buttons
                    m_acbButtons[i] = new CheckBox();

                    Type type = m_acbButtons[i].GetType();
                    System.Reflection.PropertyInfo[] aPI =
                        type.GetProperties();
                    foreach (PropertyInfo pi in aPI)
                    {
                        if (pi.CanRead == false || pi.CanWrite == false)
                            continue;

                        string strPropName = pi.PropertyType.Name;
                        if (strPropName == "IWindowTarget")
                            continue;

                        pi.SetValue(m_acbButtons[i], pi.GetValue(cbTemplate,
                            null), null);
                    }
                                                
                    m_acbButtons[i].Text = strName;
                    m_acbButtons[i].Checked = m_abCheckStates[i];
                    SetCheckBoxAppearanceSettings(m_acbButtons[i]);
                    m_acbButtons[i].CheckedChanged +=
                        new System.EventHandler(this.CheckedChanged);

                    this.Controls.Add(m_acbButtons[i]);
                }

                // Make panels
                Size sz = m_aucUserCtrls[i].Size;
                Point pt  = m_aucUserCtrls[i].Location;
                m_apnlPanels[i] = new Panel();
                m_apnlPanels[i].Size = sz;
                m_apnlPanels[i].Location = pt;
                m_apnlPanels[i].Visible = false;
                m_apnlPanels[i].DockPadding.Left = 8;
                m_apnlPanels[i].DockPadding.Right = 8;
                m_apnlPanels[i].Anchor = AnchorStyles.Left | AnchorStyles.Top |
                    AnchorStyles.Right;
                m_aucUserCtrls[i].Dock = DockStyle.Fill;
                m_apnlPanels[i].Controls.Add(m_aucUserCtrls[i]);
                this.Controls.Add(m_apnlPanels[i]);

                if (m_acbButtons[i] != null)
                {
                    m_acbButtons[i].Show();
                }
                i++;
            }
        }

        private void SetCheckBoxAppearanceSettings(CheckBox cb)
        {
            if (cb.Checked)
            {
//                cb.BackColor = System.Drawing.SystemColors.ActiveBorder;
                cb.ImageIndex = MINUS_IMAGE_INDEX;
            }
            else
            {
//                cb.BackColor = System.Drawing.SystemColors.Control;
                cb.ImageIndex = PLUS_IMAGE_INDEX;
            }
        }

        private void PlaceObjects()
        {
            // Suspend layout to prevent sizes from changing during layout
            // computation.
            this.SuspendLayout();

            // for each checkbox...
            Point ptStart = cbTemplate.Location;
            int iCheckBoxWidth = cbTemplate.Width;

            int i=0;
            foreach (CheckBox cb in m_acbButtons)
            {
                // Set the width of the panel equal to the width of the
                // check box. The check boxes change width when the control
                // changes size.
                m_apnlPanels[i].Width = iCheckBoxWidth;

                if (cb == null)
                {
                    // When there is no button, always visible.
                    m_apnlPanels[i].Location = ptStart;
                    ptStart.Y += m_apnlPanels[i].Height + m_iPadding;
                    m_apnlPanels[i].Show();
                    i++;
                    continue;
                }

                cb.Location = ptStart;
                ptStart.Y += cb.Height + m_iPadding;
                cb.Show();

                if (cb.Checked)
                {
                    // Display it's panel
                    m_apnlPanels[i].Location = ptStart;
                    ptStart.Y += m_apnlPanels[i].Height + m_iPadding;
                    m_apnlPanels[i].Show();
                    m_abCheckStates[i] = true;
                }
                else
                {
                    m_apnlPanels[i].Visible = false;
                    m_abCheckStates[i] = false;
                }
                i++;
            }

            // Resume layout to have layout modifications take effect.
            this.ResumeLayout();

            this.Parent.Invalidate(true);
        }

        private void OnRollbarSplitterMoved()
        {
            PlaceObjects();
        }

        public bool[] GetCheckStates()
        {
            return m_abCheckStates;
        }

        public void SetCheckStates(bool[] bCheckStates)
        {
            for (int i = 0; i < m_abCheckStates.Length && 
                i < bCheckStates.Length; i++)
            {
                m_abCheckStates[i] = bCheckStates[i];
            }

            if (m_acbButtons != null)
            {
                int i = 0;
                foreach (CheckBox cb in m_acbButtons)
                {
                    if (cb != null)
                    {
                        cb.Checked = bCheckStates[i];
                    }
                    i++;
                }
                PlaceObjects();
            }
        }

        private void CheckedChanged(object sender, System.EventArgs e)
        {
            SetCheckBoxAppearanceSettings((CheckBox) sender);
            PlaceObjects();
        }

        static public void SplitterMoving(
            object sender, 
            System.Windows.Forms.UserControl uc,
            System.Windows.Forms.Panel splitterPanel)
        {
            Splitter sp = (Splitter)sender;
            int iSplitPosition = sp.SplitPosition;

            int iMin = sp.MinSize;
            
            if (iSplitPosition == iMin)
            {
                Size szTotal = uc.Size;
                // Make some room...
                int iDiff = iMin; 
                Size szNew = new Size(szTotal.Width, szTotal.Height + iDiff);
                uc.Size = szNew;
                splitterPanel.Size = 
                    new Size(splitterPanel.Width, splitterPanel.Height);

                Control parent = uc.Parent;
                parent.Size = szNew;

                AppEvents.RaiseRollbarSplitterMoved();
            }
        }

        static public void SplitterMoved(
            object sender,
            System.Windows.Forms.UserControl uc,
            System.Windows.Forms.Panel  splitterPanel,
            int iDefaultHeight) 
        {
            // For purpose of calculation, we must remove the right anchor...
            // (I havent' figured out why exactly)
            if(uc.Parent != null)
            {
                uc.Parent.Anchor = System.Windows.Forms.AnchorStyles.Left |
                System.Windows.Forms.AnchorStyles.Top;
            }
            
            Splitter sp = (Splitter)sender;
            
            Size szTotal = uc.Size;

            if (splitterPanel.Height < iDefaultHeight)
            {
                int iDiff = iDefaultHeight - splitterPanel.Height;
                Size szNew = new Size(szTotal.Width, szTotal.Height + iDiff);
                uc.Size = szNew;
                splitterPanel.Size = new Size(splitterPanel.Width,
                    iDefaultHeight);
                
                if (uc.Parent != null)
                {
                    Control parent = uc.Parent;
                    parent.Size = szNew;
                }
            }
            else if (splitterPanel.Height > iDefaultHeight)
            {
                int iDiff = splitterPanel.Height - iDefaultHeight;
                Size szNew = new Size(szTotal.Width, szTotal.Height - iDiff);
                uc.Size = szNew;
                splitterPanel.Size = new Size(splitterPanel.Width,
                    iDefaultHeight);
                
                if (uc.Parent != null)
                {
                    Control parent = uc.Parent;
                    parent.Size = szNew;
                }
            }

            // For purpose of calculation, we had to remove the right anchor..
            // (I havent' figured out why exactly) Now we add it back.
            if (uc.Parent != null)
            {
                uc.Parent.Anchor = System.Windows.Forms.AnchorStyles.Left |
                    System.Windows.Forms.AnchorStyles.Right |
                    System.Windows.Forms.AnchorStyles.Top;
            }

            AppEvents.RaiseRollbarSplitterMoved();
        }
    }
}
