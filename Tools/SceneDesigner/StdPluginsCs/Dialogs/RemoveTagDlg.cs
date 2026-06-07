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
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    public partial class RemoveTagDlg : Form
    {
        private string[] m_astrInputTags = new string[0];
        public string[] InputTags
        {
            get
            {
                return m_astrInputTags;
            }
            set
            {
                m_astrInputTags = value;
            }
        }

        private List<string> m_listTagsToRemove = new List<string>();
        public string[] TagsToRemove
        {
            get
            {
                return m_listTagsToRemove.ToArray();
            }
        }

        public RemoveTagDlg()
        {
            InitializeComponent();
        }

        private void RemoveTagDlg_Load(object sender, EventArgs e)
        {
            m_lbTags.BeginUpdate();
            m_lbTags.Items.Clear();
            foreach (string strTag in m_astrInputTags)
            {
                if (!string.IsNullOrEmpty(strTag))
                {
                    m_lbTags.Items.Add(strTag);
                }
            }
            m_lbTags.EndUpdate();
        }

        private void m_btnOK_Click(object sender, EventArgs e)
        {
            m_listTagsToRemove.Clear();
            foreach (string strTag in m_lbTags.SelectedItems)
            {
                if (!string.IsNullOrEmpty(strTag))
                {
                    m_listTagsToRemove.Add(strTag);
                }
            }

            if (m_listTagsToRemove.Count == 0)
            {
                MessageBox.Show("At least one tag must be selected for " +
                    "removal. Please try again.", "No Tags Selected",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            DialogResult = DialogResult.OK;
        }

        private void m_lbTags_DoubleClick(object sender, EventArgs e)
        {
            m_btnOK_Click(sender, e);
        }
    }
}