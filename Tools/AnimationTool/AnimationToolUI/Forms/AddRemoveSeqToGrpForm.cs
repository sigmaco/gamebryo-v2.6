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
    /// Summary description for AddRemoveSeqToGrpForm.
    /// </summary>
    public class AddRemoveSeqToGrpForm : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label lblSeqInGrp;
        private OOGroup.Windows.Forms.ImageButton btnAdd;
        private OOGroup.Windows.Forms.ImageButton btnRemove;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.Splitter splitter2;
        private System.Windows.Forms.Button btnOK;
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.ImageList ilAddRemove;
        private System.Windows.Forms.ListBox lbSeqInGroup;
        private System.Windows.Forms.ListBox lbAvailableSequences;
        private System.Windows.Forms.Label lblAvailableSequences;
        private System.Windows.Forms.ToolTip ttAddRemoveSeqToGroup;
        private MSequenceGroup m_kSeqGroup;

        public AddRemoveSeqToGrpForm(MSequenceGroup kGroup)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            m_kSeqGroup = kGroup;
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(AddRemoveSeqToGrpForm));
            this.panel2 = new System.Windows.Forms.Panel();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnRemove = new OOGroup.Windows.Forms.ImageButton();
            this.ilAddRemove = new System.Windows.Forms.ImageList(this.components);
            this.btnAdd = new OOGroup.Windows.Forms.ImageButton();
            this.panel3 = new System.Windows.Forms.Panel();
            this.lbSeqInGroup = new System.Windows.Forms.ListBox();
            this.lblSeqInGrp = new System.Windows.Forms.Label();
            this.lbAvailableSequences = new System.Windows.Forms.ListBox();
            this.lblAvailableSequences = new System.Windows.Forms.Label();
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
            this.panel2.Controls.Add(this.btnOK);
            this.panel2.Controls.Add(this.btnCancel);
            this.panel2.Controls.Add(this.btnRemove);
            this.panel2.Controls.Add(this.btnAdd);
            this.panel2.Location = new System.Drawing.Point(152, 56);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(88, 192);
            this.panel2.TabIndex = 2;
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(4, 128);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(80, 24);
            this.btnOK.TabIndex = 2;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(4, 160);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(80, 24);
            this.btnCancel.TabIndex = 3;
            this.btnCancel.Text = "Cancel";
            // 
            // btnRemove
            // 
            this.btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnRemove.ImageIndex = 0;
            this.btnRemove.ImageList = this.ilAddRemove;
            this.btnRemove.Location = new System.Drawing.Point(4, 72);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(80, 24);
            this.btnRemove.TabIndex = 1;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.btnRemove, "Click to remove the currently selected\nsequence from the sequence group.");
            this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // ilAddRemove
            // 
            this.ilAddRemove.ImageSize = new System.Drawing.Size(16, 16);
            this.ilAddRemove.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("ilAddRemove.ImageStream")));
            this.ilAddRemove.TransparentColor = System.Drawing.Color.White;
            // 
            // btnAdd
            // 
            this.btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnAdd.ImageIndex = 1;
            this.btnAdd.ImageList = this.ilAddRemove;
            this.btnAdd.Location = new System.Drawing.Point(4, 40);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(80, 24);
            this.btnAdd.TabIndex = 0;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.btnAdd, "Click to add the currently selected\nsequence to the sequence group.");
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.lbSeqInGroup);
            this.panel3.Controls.Add(this.lblSeqInGrp);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel3.Location = new System.Drawing.Point(248, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(144, 310);
            this.panel3.TabIndex = 4;
            // 
            // lbSeqInGroup
            // 
            this.lbSeqInGroup.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lbSeqInGroup.Location = new System.Drawing.Point(8, 40);
            this.lbSeqInGroup.Name = "lbSeqInGroup";
            this.lbSeqInGroup.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.lbSeqInGroup.Size = new System.Drawing.Size(128, 251);
            this.lbSeqInGroup.TabIndex = 1;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.lbSeqInGroup, "Double-click a sequence to remove\nit from the sequence group.");
            this.lbSeqInGroup.DoubleClick += new System.EventHandler(this.lbSeqInGroup_DoubleClick);
            this.lbSeqInGroup.SelectedIndexChanged += new System.EventHandler(this.lbSeqInGroup_SelectedIndexChanged);
            // 
            // lblSeqInGrp
            // 
            this.lblSeqInGrp.Location = new System.Drawing.Point(16, 16);
            this.lblSeqInGrp.Name = "lblSeqInGrp";
            this.lblSeqInGrp.Size = new System.Drawing.Size(112, 16);
            this.lblSeqInGrp.TabIndex = 0;
            this.lblSeqInGrp.Text = "Sequences in Group";
            this.lblSeqInGrp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbAvailableSequences
            // 
            this.lbAvailableSequences.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lbAvailableSequences.Location = new System.Drawing.Point(8, 40);
            this.lbAvailableSequences.Name = "lbAvailableSequences";
            this.lbAvailableSequences.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.lbAvailableSequences.Size = new System.Drawing.Size(128, 251);
            this.lbAvailableSequences.Sorted = true;
            this.lbAvailableSequences.TabIndex = 1;
            this.ttAddRemoveSeqToGroup.SetToolTip(this.lbAvailableSequences, "Double-click a sequence to add\nit to the sequence group.");
            this.lbAvailableSequences.DoubleClick += new System.EventHandler(this.lbAvailableSequences_DoubleClick);
            this.lbAvailableSequences.SelectedIndexChanged += new System.EventHandler(this.lbAvailableSequences_SelectedIndexChanged);
            // 
            // lblAvailableSequences
            // 
            this.lblAvailableSequences.Location = new System.Drawing.Point(16, 16);
            this.lblAvailableSequences.Name = "lblAvailableSequences";
            this.lblAvailableSequences.Size = new System.Drawing.Size(112, 16);
            this.lblAvailableSequences.TabIndex = 0;
            this.lblAvailableSequences.Text = "Available Sequences";
            this.lblAvailableSequences.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.lbAvailableSequences);
            this.panel1.Controls.Add(this.lblAvailableSequences);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(144, 310);
            this.panel1.TabIndex = 0;
            // 
            // splitter1
            // 
            this.splitter1.Enabled = false;
            this.splitter1.Location = new System.Drawing.Point(144, 0);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(8, 310);
            this.splitter1.TabIndex = 1;
            this.splitter1.TabStop = false;
            // 
            // splitter2
            // 
            this.splitter2.Dock = System.Windows.Forms.DockStyle.Right;
            this.splitter2.Enabled = false;
            this.splitter2.Location = new System.Drawing.Point(240, 0);
            this.splitter2.Name = "splitter2";
            this.splitter2.Size = new System.Drawing.Size(8, 310);
            this.splitter2.TabIndex = 3;
            this.splitter2.TabStop = false;
            // 
            // AddRemoveSeqToGrpForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(392, 310);
            this.ControlBox = false;
            this.Controls.Add(this.splitter2);
            this.Controls.Add(this.splitter1);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.MaximumSize = new System.Drawing.Size(400, 4098);
            this.MinimumSize = new System.Drawing.Size(400, 312);
            this.Name = "AddRemoveSeqToGrpForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add / Remove Sequences to Sequence Group";
            this.Load += new System.EventHandler(this.AddRemoveSeqToGrpForm_Load);
            this.panel2.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void AddRemoveSeqToGrpForm_Load(
            object sender, System.EventArgs e)
        {
            // Initialize button images.
            btnAdd.SetImage((Bitmap) btnAdd.ImageList.Images
                [btnAdd.ImageIndex]);
            btnRemove.SetImage((Bitmap) btnRemove.ImageList.Images
                [btnRemove.ImageIndex]);

            btnAdd.Enabled = false;
            btnRemove.Enabled = false;
            ArrayList kSequences = MFramework.Instance.Animation.Sequences;
            foreach (MSequence seq in kSequences)
            {
                if (m_kSeqGroup.GetSequenceInfoIndex(seq.SequenceID) != -1)
                {
                    lbSeqInGroup.Items.Add(seq);
                }
                else
                {
                    lbAvailableSequences.Items.Add(seq);
                }
            }
        }

        private void btnOK_Click(object sender, System.EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        public void DoIt()
        {
            // First add all the new sequences
            foreach (MSequence seq in lbSeqInGroup.Items)
            {
                if (m_kSeqGroup.GetSequenceInfoIndex(seq.SequenceID) == -1)
                {
                    m_kSeqGroup.AddSequence(seq.SequenceID);
                }
            }

            // Remove all of the unwanted sequences
            ArrayList kDeletedSeqs = new ArrayList();
            foreach (
                MSequenceGroup.MSequenceInfo kInfo in m_kSeqGroup.SequenceInfo)
            {
                bool bFoundMatch = false;
                foreach (MSequence seq in lbSeqInGroup.Items)
                {
                    if (kInfo.SequenceID == seq.SequenceID)
                    {
                        bFoundMatch = true;
                        break;
                    }
                }

                if (!bFoundMatch)
                {
                    kDeletedSeqs.Add(kInfo.SequenceID);
                }
            }

            foreach (uint SequenceID in kDeletedSeqs)
            {
                int iIndex = m_kSeqGroup.GetSequenceInfoIndex(SequenceID);
                m_kSeqGroup.GetAt(iIndex).Deactivate();
                m_kSeqGroup.RemoveSequence(SequenceID);
            }
        }

        private void btnAdd_Click(object sender, System.EventArgs e)
        {
            ArrayList selectedItems = new ArrayList();
            foreach (MSequence seq in lbAvailableSequences.SelectedItems)
            {
                selectedItems.Add(seq);
            }

            foreach (MSequence seq in selectedItems)
            {
                lbSeqInGroup.Items.Add(seq);
                lbAvailableSequences.Items.Remove(seq);
            }
        }

        private void btnRemove_Click(object sender, System.EventArgs e)
        {
            ArrayList selectedItems = new ArrayList();
            foreach (MSequence seq in lbSeqInGroup.SelectedItems)
            {
                selectedItems.Add(seq);
            }

            foreach (MSequence seq in selectedItems)
            {
                lbSeqInGroup.Items.Remove(seq);
                lbAvailableSequences.Items.Add(seq);
            }
        }

        private void lbSeqInGroup_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            if (lbSeqInGroup.SelectedItems.Count > 0)
                 btnRemove.Enabled = true;
            else
                btnRemove.Enabled = false;
        }

        private void lbAvailableSequences_SelectedIndexChanged(
            object sender, System.EventArgs e)
        {
            if (lbAvailableSequences.SelectedItems.Count > 0)
                btnAdd.Enabled = true;
            else
                btnAdd.Enabled = false;
        }

        private void lbAvailableSequences_DoubleClick(
            object sender, System.EventArgs e)
        {
            btnAdd_Click(sender, e);
        }

        private void lbSeqInGroup_DoubleClick(
            object sender, System.EventArgs e)
        {
            btnRemove_Click(sender, e);
        }
    }
}
