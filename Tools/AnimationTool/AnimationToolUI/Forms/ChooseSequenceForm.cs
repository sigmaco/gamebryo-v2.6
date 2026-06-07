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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    public class ChooseSequenceForm : Form
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.
        /// </param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.labelSequencePrompt = new System.Windows.Forms.Label();
            this.listSequenceNames = new System.Windows.Forms.ListBox();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // labelSequencePrompt
            // 
            this.labelSequencePrompt.Location = new System.Drawing.Point(22, 9);
            this.labelSequencePrompt.Name = "labelSequencePrompt";
            this.labelSequencePrompt.Size = new System.Drawing.Size(424, 49);
            this.labelSequencePrompt.TabIndex = 0;
            this.labelSequencePrompt.Text = "labelSequencePrompt";
            // 
            // listSequenceNames
            // 
            this.listSequenceNames.FormattingEnabled = true;
            this.listSequenceNames.Location = new System.Drawing.Point(25, 61);
            this.listSequenceNames.Name = "listSequenceNames";
            this.listSequenceNames.Size = new System.Drawing.Size(421, 199);
            this.listSequenceNames.TabIndex = 1;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnCancel.Location = new System.Drawing.Point(237, 289);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 5;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btnOK.Location = new System.Drawing.Point(136, 289);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 4;
            this.btnOK.Text = "OK";
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // ChooseSequenceForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(491, 324);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.listSequenceNames);
            this.Controls.Add(this.labelSequencePrompt);
            this.Name = "ChooseSequenceForm";
            this.Text = "Select Missing Sequence";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label labelSequencePrompt;
        private System.Windows.Forms.ListBox listSequenceNames;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;

        public ChooseSequenceForm()
        {
            InitializeComponent();
            m_kSequenceName = null;
            m_kKFFile = null;
            m_kMissingComponent = null;
        }

        #region Data Members
        private string m_kSequenceName;
        public string SequenceName
        {
            get { return m_kSequenceName; }
        }

        private string m_kKFFile;
        public string KFFile
        {
            get { return m_kKFFile; }
            set { LoadKFFile(value); }
        }

        private string m_kMissingComponent;
        public string MissingComponent
        {
            get { return m_kMissingComponent; }
            set { m_kMissingComponent = value; UpdateLabel();  }
        }
        #endregion

        private bool LoadKFFile(string strFileName)
        {
            if (strFileName == null)
                return false;

            ArrayList list = MSequence.GetSequenceNamesFromKF(strFileName);
            if (list == null || list.Count == 0)
                return false;

            listSequenceNames.Items.Clear();

            foreach (String strSequenceName in list)
            {
                listSequenceNames.Items.Add(strSequenceName);
            }

            m_kKFFile = strFileName;

            return true;
        }

        private void UpdateLabel()
        {
            labelSequencePrompt.Text = 
                "Select one sequence to replace missing sequence " + 
                m_kMissingComponent + ":";
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            m_kSequenceName = (String)listSequenceNames.SelectedItem;
            DialogResult = DialogResult.OK;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
    }
}