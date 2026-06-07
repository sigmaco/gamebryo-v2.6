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

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors
{
    public partial class EntityTagsEditorDialog : Form
    {
        public EntityTagsEditorDialog()
        {
            InitializeComponent();
        }

        private string m_strTagsString;
        public string TagsString
        {
            get
            {
                return m_strTagsString;
            }
            set
            {
                m_strTagsString = value;
            }
        }

        private string FindUniqueNodeName(string strProposedName)
        {
            foreach (TreeNode node in m_tvTags.Nodes)
            {
                if (node.Text.Equals(strProposedName))
                {
                    string strBaseName = strProposedName;

                    int iSuffix = 2;
                    int iIndex = strProposedName.LastIndexOf(' ');
                    if (iIndex == strProposedName.Length - 1)
                    {
                        iIndex = strProposedName.LastIndexOf(' ', iIndex - 1);
                    }
                    if (iIndex > -1)
                    {
                        string strSuffix = strProposedName.Substring(iIndex +
                            1);
                        try
                        {
                            iSuffix = Int32.Parse(strSuffix) + 1;
                            strBaseName = strProposedName.Substring(0, iIndex);
                        }
                        catch (FormatException)
                        {
                            // The suffix is not an integer; leave the base
                            // name set to the proposed name.
                        }
                    }

                    return FindUniqueNodeName(string.Format("{0} {1:D02}",
                        strBaseName, iSuffix));
                }
            }

            return strProposedName;
        }

        private void EntityTagsEditorDialog_Load(object sender, EventArgs e)
        {
            m_tvTags.BeginUpdate();

            m_tvTags.Nodes.Clear();
            if (!string.IsNullOrEmpty(m_strTagsString))
            {
                string[] astrTags = m_strTagsString.Split(
                    MEntity.TagDelimiterString.ToCharArray());
                foreach (string strTag in astrTags)
                {
                    if (!string.IsNullOrEmpty(strTag))
                    {
                        m_tvTags.Nodes.Add(new TreeNode(strTag));
                    }
                }
            }

            m_tvTags.EndUpdate();
        }

        private void m_btnOK_Click(object sender, EventArgs e)
        {
            List<string> tags = new List<string>(m_tvTags.Nodes.Count);
            foreach (TreeNode node in m_tvTags.Nodes)
            {
                if (!string.IsNullOrEmpty(node.Text))
                {
                    tags.Add(node.Text);
                }
            }

            if (tags.Count > 0)
            {
                m_strTagsString = string.Join(MEntity.TagDelimiterString,
                    tags.ToArray());
                m_strTagsString += MEntity.TagDelimiterString;
            }
            else
            {
                m_strTagsString = null;
            }
        }

        private void m_btnAdd_Click(object sender, EventArgs e)
        {
            TreeNode newNode = new TreeNode(FindUniqueNodeName("New Tag"));
            m_tvTags.Nodes.Add(newNode);
            m_tvTags.SelectedNode = newNode;
            newNode.BeginEdit();
        }

        private void m_btnRemove_Click(object sender, EventArgs e)
        {
            if (m_tvTags.SelectedNode != null)
            {
                m_tvTags.Nodes.Remove(m_tvTags.SelectedNode);
            }
        }

        private void m_tvTags_NodeMouseDoubleClick(object sender,
            TreeNodeMouseClickEventArgs e)
        {
            e.Node.BeginEdit();
        }

        private void m_tvTags_KeyUp(object sender, KeyEventArgs e)
        {
            if (m_tvTags.SelectedNode != null)
            {
                switch (e.KeyCode)
                {
                    case Keys.F2:
                        m_tvTags.SelectedNode.BeginEdit();
                        break;
                    case Keys.Delete:
                        m_tvTags.Nodes.Remove(m_tvTags.SelectedNode);
                        break;
                }
            }
        }

        private void m_tvTags_AfterLabelEdit(object sender,
            NodeLabelEditEventArgs e)
        {
            if (e.Label == null)
            {
                return;
            }

            if (e.Label.Contains(new string(
                MEntity.TagDelimiterString.ToCharArray())))
            {
                MessageBox.Show(string.Format("The tag delimiter character " +
                    "'{0}' cannot be used in a tag.",
                    MEntity.TagDelimiterString), "Invalid Tag Text",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.CancelEdit = true;
            }
            else if (e.Label.Equals(string.Empty))
            {
                MessageBox.Show("The tag text cannot be empty. Click the " +
                    "\"Remove\" button to remove a tag.", "Invalid Tag Text",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                e.CancelEdit = true;
            }
        }
    }
}
