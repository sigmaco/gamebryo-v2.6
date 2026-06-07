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
    /// Summary description for SequenceTextKeys.
    /// </summary>
    public class SequenceTextKeys : System.Windows.Forms.UserControl
    {
        #region Delegates

        private MAnimation.__Delegate_OnNewKFM m_OnNewKFM = null;
        private MAnimation.__Delegate_OnKFMLoaded m_OnKFMLoaded = null;
        private MAnimation.__Delegate_OnSequenceModified 
            m_OnSequenceModified = null;

        private void RegisterDelegates()
        {
            UnregisterDelegates();

            // Create delegates.
            m_OnNewKFM = new MAnimation.__Delegate_OnNewKFM(KFMChanged);
            m_OnKFMLoaded = new MAnimation.__Delegate_OnKFMLoaded(KFMChanged);
            m_OnSequenceModified = 
                new MAnimation.__Delegate_OnSequenceModified(SequenceModified);

            // Register delegates.
            MFramework.Instance.Animation.OnNewKFM += m_OnNewKFM;
            MFramework.Instance.Animation.OnKFMLoaded += m_OnKFMLoaded;
            MFramework.Instance.Animation.OnSequenceModified += 
                m_OnSequenceModified;
        }

        private void UnregisterDelegates()
        {
            if (m_OnNewKFM != null)
                MFramework.Instance.Animation.OnNewKFM -= m_OnNewKFM;
            if (m_OnKFMLoaded != null)
                MFramework.Instance.Animation.OnKFMLoaded -= m_OnKFMLoaded;
            if (m_OnSequenceModified != null)
                MFramework.Instance.Animation.OnSequenceModified -= 
                    m_OnSequenceModified;
        }

        #endregion

        private Size m_szDefaultSizeOfPanel;
        private MSequence m_kSequence;
        private System.Windows.Forms.TreeView TreeView_TextKeys;
        private System.Windows.Forms.ToolTip ToolTip_TextKey;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Splitter splitter1;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public SequenceTextKeys(MSequence kSequence)
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();
            Debug.Assert(NumInstances == 0);
            NumInstances++;            
            m_kSequence = kSequence;
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
                UnregisterDelegates();
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
            this.TreeView_TextKeys = new System.Windows.Forms.TreeView();
            this.ToolTip_TextKey = new System.Windows.Forms.ToolTip(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.SuspendLayout();
            // 
            // TreeView_TextKeys
            // 
            this.TreeView_TextKeys.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.TreeView_TextKeys.ImageIndex = -1;
            this.TreeView_TextKeys.Location = new System.Drawing.Point(5, 5);
            this.TreeView_TextKeys.Name = "TreeView_TextKeys";
            this.TreeView_TextKeys.SelectedImageIndex = -1;
            this.TreeView_TextKeys.Size = new System.Drawing.Size(270, 70);
            this.TreeView_TextKeys.TabIndex = 0;
            this.ToolTip_TextKey.SetToolTip(this.TreeView_TextKeys, "This panel displays the text keys in the currently selected sequence. These keys " +
                "are\nused to provide information about the animation to the application. The valu" +
                "e in brackets\nis the time at which the key occurs. The text is the content of th" +
                "e text key.");
            // 
            // ToolTip_TextKey
            // 
            this.ToolTip_TextKey.AutomaticDelay = 750;
            // 
            // panel1
            // 
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(5, 83);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(270, 16);
            this.panel1.TabIndex = 2;
            // 
            // splitter1
            // 
            this.splitter1.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(5, 75);
            this.splitter1.MinExtra = 50;
            this.splitter1.MinSize = 15;
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(270, 8);
            this.splitter1.TabIndex = 1;
            this.splitter1.TabStop = false;
            this.splitter1.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoved);
            this.splitter1.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoving);
            // 
            // SequenceTextKeys
            // 
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.TreeView_TextKeys);
            this.DockPadding.All = 5;
            this.Name = "SequenceTextKeys";
            this.Size = new System.Drawing.Size(280, 104);
            this.Load += new System.EventHandler(this.SequenceTextKeys_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void UpdateTextKeyUI()
        {
            Debug.Assert(m_kSequence != null);
            try
            {
                TreeView_TextKeys.Nodes.Clear();
                if (m_kSequence.TextKeys != null &&
                    m_kSequence.TextKeys.Length != 0)
                {
                    uint uiIndex = 0;
                    foreach (MSequence.MTextKey kTextKey 
                        in m_kSequence.TextKeys)
                    {
                        if (kTextKey != null)
                        {
                            try
                            {
                                TreeNode tnNew = new TreeNode("[" + 
                                    kTextKey.Time.ToString("f4") + "] " + 
                                    kTextKey.Text);
                                TreeView_TextKeys.Nodes.Add(tnNew);


                                uiIndex++;
                            }
                            catch(Exception e)
                            {
                                Debug.Assert(false, e.Message);
                            }
                        }
                    }
                }
            }
            catch(Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void SequenceTextKeys_Load(object sender, System.EventArgs e)
        {
            RegisterDelegates();
            m_szDefaultSizeOfPanel = panel1.Size;
            UpdateTextKeyUI();
        }

        private void SequenceModified(MSequence.PropertyType ePropChanged,
            MSequence kSequence)
        {
            if (kSequence == m_kSequence)
                UpdateTextKeyUI();
        }

        private void KFMChanged()
        {
            try
            {
                m_kSequence = null;
            }
            catch (Exception e)
            {
                Debug.Assert(false, e.Message);
            }
        }

        private void splitter1_SplitterMoved(object sender,
            System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoved(sender, this, panel1,
                m_szDefaultSizeOfPanel.Height);
        }

        private void splitter1_SplitterMoving(object sender,
            System.Windows.Forms.SplitterEventArgs e)
        {
            RollBarControl.SplitterMoving(sender, this, panel1);
        }
    }
}
