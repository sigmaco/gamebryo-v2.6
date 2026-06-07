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

using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    public partial class DeleteLayerDialog : Form
    {
        private String m_DirtyFileMessage =
            "The layer is not saved.  Removing it will cause changes " +
            "to be lost.";
        private String m_ReadOnlyFileMessage = 
            "The layer is read-only. It will be forced to be writable " +
            "to delete it.";

        public DeleteLayerDialog()
        {
            m_pmLayer = null;
            InitializeComponent();
        }

        private MLayer m_pmLayer;

        public MLayer Layer
        {
            set
            {
                m_pmLayer = value;
            }
        }

        public bool DeleteFile
        {
            get
            {
                return cbDeleteFile.Checked;
            }
        }

        private void cbDeleteFile_CheckedChanged(object sender, EventArgs e)
        {
            //lbDirtyText.Enabled = !cbDeleteFile.Checked;
            if (cbDeleteFile.Checked)
            {
                if (!m_pmLayer.Writable)
                {
                    ShowMessage(m_ReadOnlyFileMessage);
                }
                else
                {
                    HideMessage();
                }
            }
            else
            {
                if (m_pmLayer.Dirty)
                {
                    ShowMessage(m_DirtyFileMessage);
                }
                else
                {
                    HideMessage();
                }
            }
                
        }

        private void HideMessage()
        {
            if (lbDirtyText.Visible)
            {
                lbDirtyText.Visible = false;
                Height -= lbDirtyText.Height;
            }
        }

        private void ShowMessage(String message)
        {
            if (!lbDirtyText.Visible)
            {
                lbDirtyText.Visible = true;
                Height += lbDirtyText.Height;
            }
            lbDirtyText.Text = message;
        }

        private void DeleteLayerDialog_Load(object sender, EventArgs e)
        {
            m_pbIcon.Image = System.Drawing.SystemIcons.Question.ToBitmap();

            Debug.Assert(m_pmLayer != null);
            if (m_pmLayer.Dirty)
            {
                ShowMessage(m_DirtyFileMessage);
            }
            else
            {
                HideMessage();
            }
        }
    }
}