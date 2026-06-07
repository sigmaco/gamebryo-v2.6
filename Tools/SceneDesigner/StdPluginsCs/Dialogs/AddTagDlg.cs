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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    public partial class AddTagDlg : Form
    {
        private string m_strTag;
        public string TagText
        {
            get
            {
                return m_strTag;
            }
            set
            {
                m_strTag = value;
            }
        }

        public AddTagDlg()
        {
            InitializeComponent();
        }

        private void AddTagDlg_Load(object sender, EventArgs e)
        {
            m_tbTag.Text = m_strTag;
        }

        private void m_btnOK_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(m_tbTag.Text))
            {
                MessageBox.Show("The entered tag cannot be empty. Please " +
                    "try again.", "Invalid Tag", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }

            m_strTag = m_tbTag.Text;
            DialogResult = DialogResult.OK;
        }
    }
}
