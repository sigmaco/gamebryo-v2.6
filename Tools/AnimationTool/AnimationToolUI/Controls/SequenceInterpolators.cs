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
    /// Summary description for SequenceInterpolators.
    /// </summary>
    public class SequenceInterpolators : System.Windows.Forms.UserControl
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
            m_OnSequenceModified = new 
                MAnimation.__Delegate_OnSequenceModified(SequenceModified);

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
        private System.Windows.Forms.TreeView TreeView_Interpolators;
        private MSequence m_kSequence;
        private System.Windows.Forms.ToolTip ToolTip_Interpolators;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Splitter splitter1;
        private System.ComponentModel.IContainer components;
        private static uint NumInstances = 0;

        public SequenceInterpolators(MSequence kSequence)
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
                if (NumInstances != 0)
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
            this.TreeView_Interpolators = new System.Windows.Forms.TreeView();
            this.ToolTip_Interpolators = new System.Windows.Forms.ToolTip(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.SuspendLayout();
            // 
            // TreeView_Interpolators
            // 
            this.TreeView_Interpolators.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.TreeView_Interpolators.ImageIndex = -1;
            this.TreeView_Interpolators.Location = new System.Drawing.Point(5, 5);
            this.TreeView_Interpolators.Name = "TreeView_Interpolators";
            this.TreeView_Interpolators.SelectedImageIndex = -1;
            this.TreeView_Interpolators.Size = new System.Drawing.Size(254, 235);
            this.TreeView_Interpolators.TabIndex = 0;
            this.ToolTip_Interpolators.SetToolTip(this.TreeView_Interpolators, @"This panel displays the interpolators contained in the currently selected sequence. Interpolators
perform the actual work of the animation system per node in the hierarchy. Each interpolator can
be expanded to provide additional information about the keys it contains");
            // 
            // ToolTip_Interpolators
            // 
            this.ToolTip_Interpolators.AutomaticDelay = 750;
            // 
            // panel1
            // 
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(5, 248);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(254, 16);
            this.panel1.TabIndex = 2;
            // 
            // splitter1
            // 
            this.splitter1.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.splitter1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitter1.Location = new System.Drawing.Point(5, 240);
            this.splitter1.MinExtra = 100;
            this.splitter1.MinSize = 15;
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(254, 8);
            this.splitter1.TabIndex = 1;
            this.splitter1.TabStop = false;
            this.splitter1.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoved);
            this.splitter1.SplitterMoving += new System.Windows.Forms.SplitterEventHandler(this.splitter1_SplitterMoving);
            // 
            // SequenceInterpolators
            // 
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.TreeView_Interpolators);
            this.DockPadding.All = 5;
            this.Name = "SequenceInterpolators";
            this.Size = new System.Drawing.Size(264, 269);
            this.Load += new System.EventHandler(this.SequenceInterpolators_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void AddKeyInformationToNode(ref TreeNode kNode,
            MSequence.MInterpolatorInfo kInfo)
        {
            for (ushort ui = 0; ui < kInfo.GetKeyChannelCount(); ui++)
            {
                if (kInfo.GetKeyType(ui) == 
                    MSequence.MInterpolatorInfo.KeyType.NOINTERP)
                {
                    continue;
                }

                TreeNode tnKeyRoot = new TreeNode("[" + ui.ToString() + "] " +
                    kInfo.GetKeyContent(ui).ToString());
                kNode.Nodes.Add(tnKeyRoot);

                TreeNode tnChild = new TreeNode("Key Type: " +
                    kInfo.GetKeyType(ui).ToString());
                tnKeyRoot.Nodes.Add(tnChild);

                if (!kInfo.GetChannelPosed(ui))
                {
                    tnChild = new TreeNode("Key Count: " +
                        kInfo.GetKeyCount(ui).ToString());
                    tnKeyRoot.Nodes.Add(tnChild);

                    tnChild = new TreeNode("Allocated Size: " +
                        kInfo.GetAllocatedSize(ui).ToString() + " bytes");
                    tnKeyRoot.Nodes.Add(tnChild);
                }
                else 
                {
                    tnChild = new TreeNode(
                        "Channel Posed: true");
                    tnKeyRoot.Nodes.Add(tnChild);
                }
            }
        }

        private void AddControlPointInformationToNode(ref TreeNode kNode,
            MSequence.MInterpolatorInfo kInfo)
        {
            for (ushort ui = 0; ui < kInfo.GetKeyChannelCount(); ui++)
            {
                TreeNode tnKeyRoot = new TreeNode("[" + ui.ToString() + "] " +
                    "Dimension: " + 
                    kInfo.GetDimension(ui).ToString() +
                    " Degree: " + kInfo.GetDegree(ui).ToString());
                kNode.Nodes.Add(tnKeyRoot);

                TreeNode tnChild;

                if (!kInfo.GetChannelPosed(ui))
                {
                    tnChild = new TreeNode("Key Count: " +
                        kInfo.GetKeyCount(ui).ToString());
                    tnKeyRoot.Nodes.Add(tnChild);

                    tnChild = new TreeNode("Allocated Size: " +
                        kInfo.GetAllocatedSize(ui).ToString() + " bytes");
                    tnKeyRoot.Nodes.Add(tnChild);
                }
                else 
                {
                    tnChild = new TreeNode(
                        "Channel Posed: true");
                    tnKeyRoot.Nodes.Add(tnChild);
                }
            }
        }

        private void UpdateInterpolatorUI()
        {
            Debug.Assert(m_kSequence != null);
            try
            {
                TreeView_Interpolators.Nodes.Clear();
                if (m_kSequence.InterpolatorInfo.Length != 0)
                {
                    uint uiInterpIndex = 0;
                    foreach (MSequence.MInterpolatorInfo kInfo 
                        in m_kSequence.InterpolatorInfo)
                    {
                        if (kInfo != null)
                        {
                            try
                            {
                                TreeNode tnNew = new TreeNode("[" + 
                                    uiInterpIndex.ToString() + "] " + 
                                    kInfo.AVObjectName);
                                TreeView_Interpolators.Nodes.Add(tnNew);

                                TreeNode tnChild = new TreeNode(
                                    "Gamebryo Class: " + kInfo.RTTI);
                                tnNew.Nodes.Add(tnChild);

                                if (kInfo.PropertyType != null && 
                                    kInfo.PropertyType.Length > 0)
                                    tnChild = new TreeNode("Property Type: " + 
                                        kInfo.PropertyType);
                                else
                                    tnChild = new TreeNode(
                                        "Property Type: N/A");
                                tnNew.Nodes.Add(tnChild);

                                tnChild = new TreeNode("Controller Type: " + 
                                                       kInfo.ControllerType);
                                tnNew.Nodes.Add(tnChild);
                                
                                if (kInfo.ControllerID != null && 
                                    kInfo.ControllerID.Length > 0)
                                    tnChild = new TreeNode("ControllerID: " + 
                                                           kInfo.ControllerID);
                                else
                                    tnChild = new TreeNode(
                                        "ControllerID: N/A");
                                tnNew.Nodes.Add(tnChild);
                                
                                if (kInfo.InterpolatorID != null && 
                                    kInfo.InterpolatorID.Length > 0)
                                    tnChild = new TreeNode("InterpolatorID: " +
                                        kInfo.InterpolatorID);
                                else
                                    tnChild = new TreeNode(
                                        "InterpolatorID: N/A");
                                tnNew.Nodes.Add(tnChild);
                                
                                if (kInfo.IsBSplineInterpolator())
                                {
                                    tnChild = new TreeNode(
                                        "Control Point Array: ");
                                    tnNew.Nodes.Add(tnChild);
                                    AddControlPointInformationToNode(
                                        ref tnChild, kInfo);
                                }
                                else
                                {
                                    tnChild = new TreeNode("Keys Arrays: ");
                                    tnNew.Nodes.Add(tnChild);
                                    AddKeyInformationToNode(
                                        ref tnChild, kInfo);
                                }

                                uiInterpIndex++;
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

        
        private void SequenceInterpolators_Load(
            object sender, System.EventArgs e)
        {
            RegisterDelegates();
            m_szDefaultSizeOfPanel = panel1.Size;
            UpdateInterpolatorUI();
        }

        private void SequenceModified(MSequence.PropertyType ePropChanged,
            MSequence kSequence)
        {
            if (kSequence == m_kSequence)
                UpdateInterpolatorUI();
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
