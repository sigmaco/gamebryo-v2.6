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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// AddRemovePhysicsSequenceForm is used to add and remove sequences
    /// from the set of physical sequences.
    /// </summary>
    public class AddRemovePhysicsSequenceForm : System.Windows.Forms.Form
    {
#if EE_PHYSX_BUILD

        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.Splitter splitter2;
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.ImageList ilAddRemove;
        private System.Windows.Forms.ToolTip ttAddRemoveSeqToGroup;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private OOGroup.Windows.Forms.ImageButton m_btnAdd;
        private OOGroup.Windows.Forms.ImageButton m_btnRemove;
        private System.Windows.Forms.Label m_lblSeqInGrp;
        private System.Windows.Forms.ListBox m_lbAvailableSequences;
        private System.Windows.Forms.Label m_lblAvailableSequences;
        private System.Windows.Forms.ListBox m_lbPhysicalSequences;

        public AddRemovePhysicsSequenceForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            // Do any other constructor stuff here.
        }

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

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AddRemovePhysicsSequenceForm));
            this.panel2 = new System.Windows.Forms.Panel();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_btnAdd = new OOGroup.Windows.Forms.ImageButton();
            this.ilAddRemove = new System.Windows.Forms.ImageList(this.components);
            this.m_btnRemove = new OOGroup.Windows.Forms.ImageButton();
            this.panel3 = new System.Windows.Forms.Panel();
            this.m_lbPhysicalSequences = new System.Windows.Forms.ListBox();
            this.m_lblSeqInGrp = new System.Windows.Forms.Label();
            this.m_lbAvailableSequences = new System.Windows.Forms.ListBox();
            this.m_lblAvailableSequences = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.splitter2 = new System.Windows.Forms.Splitter();
            this.ttAddRemoveSeqToGroup = new System.Windows.Forms.ToolTip(this.components);
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel2.Controls.Add(this.m_btnOK);
            this.panel2.Controls.Add(this.m_btnCancel);
            this.panel2.Controls.Add(this.m_btnAdd);
            this.panel2.Controls.Add(this.m_btnRemove);
            this.panel2.Location = new System.Drawing.Point(179, 58);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(108, 218);
            this.panel2.TabIndex = 2;
            // 
            // m_btnOK
            // 
            this.m_btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(8, 144);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.Size = new System.Drawing.Size(96, 28);
            this.m_btnOK.TabIndex = 2;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(8, 181);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.Size = new System.Drawing.Size(96, 28);
            this.m_btnCancel.TabIndex = 3;
            this.m_btnCancel.Text = "Cancel";
            // 
            // m_btnAdd
            // 
            this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnAdd.ImageIndex = 1;
            this.m_btnAdd.ImageList = this.ilAddRemove;
            this.m_btnAdd.Location = new System.Drawing.Point(8, 24);
            this.m_btnAdd.Name = "m_btnAdd";
            this.m_btnAdd.Size = new System.Drawing.Size(96, 28);
            this.m_btnAdd.TabIndex = 0;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.m_btnAdd, "Click to add the currently selected\nsequence to the sequence group.");
            this.m_btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // ilAddRemove
            // 
            this.ilAddRemove.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilAddRemove.ImageStream")));
            this.ilAddRemove.TransparentColor = System.Drawing.Color.White;
            this.ilAddRemove.Images.SetKeyName(0, "");
            this.ilAddRemove.Images.SetKeyName(1, "");
            // 
            // m_btnRemove
            // 
            this.m_btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnRemove.ImageIndex = 0;
            this.m_btnRemove.ImageList = this.ilAddRemove;
            this.m_btnRemove.Location = new System.Drawing.Point(8, 58);
            this.m_btnRemove.Name = "m_btnRemove";
            this.m_btnRemove.Size = new System.Drawing.Size(96, 28);
            this.m_btnRemove.TabIndex = 0;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.m_btnRemove, "Click to remove the currently selected\nsequence from the sequence group.");
            this.m_btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.m_lbPhysicalSequences);
            this.panel3.Controls.Add(this.m_lblSeqInGrp);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel3.Location = new System.Drawing.Point(300, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(172, 352);
            this.panel3.TabIndex = 4;
            // 
            // m_lbPhysicalSequences
            // 
            this.m_lbPhysicalSequences.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbPhysicalSequences.ItemHeight = 16;
            this.m_lbPhysicalSequences.Location = new System.Drawing.Point(3, 48);
            this.m_lbPhysicalSequences.Name = "m_lbPhysicalSequences";
            this.m_lbPhysicalSequences.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.m_lbPhysicalSequences.Size = new System.Drawing.Size(153, 260);
            this.m_lbPhysicalSequences.TabIndex = 1;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.m_lbPhysicalSequences, "Double-click a sequence to remove\nit from the sequence group.");
            this.m_lbPhysicalSequences.SelectedIndexChanged += new System.EventHandler(this.lbPhysicalSequences_SelectedIndexChanged);
            this.m_lbPhysicalSequences.DoubleClick += new System.EventHandler(this.lbPhysicalSequences_DoubleClick);
            // 
            // m_lblSeqInGrp
            // 
            this.m_lblSeqInGrp.Location = new System.Drawing.Point(3, 27);
            this.m_lblSeqInGrp.Name = "m_lblSeqInGrp";
            this.m_lblSeqInGrp.Size = new System.Drawing.Size(134, 18);
            this.m_lblSeqInGrp.TabIndex = 0;
            this.m_lblSeqInGrp.Text = "Physical Sequences";
            this.m_lblSeqInGrp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // m_lbAvailableSequences
            // 
            this.m_lbAvailableSequences.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbAvailableSequences.ItemHeight = 16;
            this.m_lbAvailableSequences.Location = new System.Drawing.Point(12, 48);
            this.m_lbAvailableSequences.Name = "m_lbAvailableSequences";
            this.m_lbAvailableSequences.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.m_lbAvailableSequences.Size = new System.Drawing.Size(153, 260);
            this.m_lbAvailableSequences.Sorted = true;
            this.m_lbAvailableSequences.TabIndex = 1;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.m_lbAvailableSequences, "Double-click a sequence to add\nit to the sequence group.");
            this.m_lbAvailableSequences.SelectedIndexChanged += new System.EventHandler(this.lbAvailableSequences_SelectedIndexChanged);
            this.m_lbAvailableSequences.DoubleClick += new System.EventHandler(this.lbAvailableSequences_DoubleClick);
            // 
            // m_lblAvailableSequences
            // 
            this.m_lblAvailableSequences.Location = new System.Drawing.Point(12, 27);
            this.m_lblAvailableSequences.Name = "m_lblAvailableSequences";
            this.m_lblAvailableSequences.Size = new System.Drawing.Size(134, 18);
            this.m_lblAvailableSequences.TabIndex = 0;
            this.m_lblAvailableSequences.Text = "Available Sequences";
            this.m_lblAvailableSequences.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.m_lbAvailableSequences);
            this.panel1.Controls.Add(this.m_lblAvailableSequences);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(173, 352);
            this.panel1.TabIndex = 0;
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Paint);
            // 
            // splitter1
            // 
            this.splitter1.Enabled = false;
            this.splitter1.Location = new System.Drawing.Point(173, 0);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(9, 352);
            this.splitter1.TabIndex = 1;
            this.splitter1.TabStop = false;
            // 
            // splitter2
            // 
            this.splitter2.Dock = System.Windows.Forms.DockStyle.Right;
            this.splitter2.Enabled = false;
            this.splitter2.Location = new System.Drawing.Point(290, 0);
            this.splitter2.Name = "splitter2";
            this.splitter2.Size = new System.Drawing.Size(10, 352);
            this.splitter2.TabIndex = 3;
            this.splitter2.TabStop = false;
            // 
            // AddRemovePhysicsSequenceForm
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 15);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(472, 352);
            this.ControlBox = false;
            this.Controls.Add(this.splitter2);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.MaximumSize = new System.Drawing.Size(480, 4728);
            this.MinimumSize = new System.Drawing.Size(480, 360);
            this.Name = "AddRemovePhysicsSequenceForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add/Remove Physical Sequences";
            this.Load += new System.EventHandler(this.AddRemovePhysicsSequenceForm_Load);
            this.panel2.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void AddRemovePhysicsSequenceForm_Load(
            object sender, System.EventArgs e)
        {
            // Initialize button images.
            m_btnAdd.SetImage((Bitmap) m_btnAdd.ImageList.Images
                [m_btnAdd.ImageIndex]);
            m_btnAdd.Enabled = false;

            m_btnRemove.SetImage((Bitmap)m_btnRemove.ImageList.Images
                [m_btnRemove.ImageIndex]);
            m_btnRemove.Enabled = false;

            ArrayList kAllSequences = MFramework.Instance.Animation.Sequences;
            ArrayList kPhysicalSequences =
                MFramework.Instance.Physics.PhysicalSequences;
            foreach (MSequence seq in kAllSequences)
            {
                if (kPhysicalSequences.Contains(seq))
                {
                    m_lbPhysicalSequences.Items.Add(seq);
                }
                else
                {
                    m_lbAvailableSequences.Items.Add(seq);
                }
            }
        }

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        public void DoIt()
        {
            ArrayList kAllSequences = MFramework.Instance.Animation.Sequences;
            ArrayList kPhysicalSequences =
                MFramework.Instance.Physics.PhysicalSequences;

            // Remove all of the old sequences
            ArrayList kRemovedSeqs = new ArrayList();
            foreach (MSequence seq in m_lbAvailableSequences.Items)
            {
                bool bFoundMatch = false;
                foreach (MSequence existing in kPhysicalSequences)
                {
                    if (existing == seq)
                    {
                        bFoundMatch = true;
                        break;
                    }
                }

                if (bFoundMatch)
                {
                    kRemovedSeqs.Add(seq);
                }
            }
            foreach (MSequence seq in kRemovedSeqs)
            {
                MFramework.Instance.Physics.RemoveSequence(seq);
            }

            // Add all of the new sequences
            ArrayList kAddedSeqs = new ArrayList();
            foreach (MSequence seq in m_lbPhysicalSequences.Items)
            {
                bool bFoundMatch = false;
                foreach (MSequence existing in kPhysicalSequences)
                {
                    if (existing == seq)
                    {
                        bFoundMatch = true;
                        break;
                    }
                }

                if (!bFoundMatch)
                {
                    kAddedSeqs.Add(seq);
                }
            }
            foreach (MSequence seq in kAddedSeqs)
            {
                MFramework.Instance.Physics.AddSequence(seq);
            }
        }

        private void btnAdd_Click(object sender, System.EventArgs e)
        {
            ArrayList selectedItems = new ArrayList();
            foreach (MSequence seq in m_lbAvailableSequences.SelectedItems)
            {
                selectedItems.Add(seq);
            }

            foreach (MSequence seq in selectedItems)
            {
                m_lbPhysicalSequences.Items.Add(seq);
                m_lbAvailableSequences.Items.Remove(seq);
            }
        }

        private void btnRemove_Click(object sender, System.EventArgs e)
        {
            ArrayList selectedItems = new ArrayList();
            foreach (MSequence seq in m_lbPhysicalSequences.SelectedItems)
            {
                selectedItems.Add(seq);
            }

            foreach (MSequence seq in selectedItems)
            {
                m_lbAvailableSequences.Items.Add(seq);
                m_lbPhysicalSequences.Items.Remove(seq);
            }
        }

        private void lbAvailableSequences_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            if (m_lbAvailableSequences.SelectedItems.Count > 0)
                m_btnAdd.Enabled = true;
            else
                m_btnAdd.Enabled = false;
        }

        private void lbPhysicalSequences_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            if (m_lbPhysicalSequences.SelectedItems.Count > 0)
                m_btnRemove.Enabled = true;
            else
                m_btnRemove.Enabled = false;
        }

        private void lbAvailableSequences_DoubleClick(
            object sender, System.EventArgs e)
        {
            btnAdd_Click(sender, e);
        }

        private void lbPhysicalSequences_DoubleClick(
            object sender, System.EventArgs e)
        {
            btnRemove_Click(sender, e);
        }

        private void panel1_Paint(object sender,
            System.Windows.Forms.PaintEventArgs e)
        {
        
        }

#endif  // #if EE_PHYSX_BUILD
    }
}
