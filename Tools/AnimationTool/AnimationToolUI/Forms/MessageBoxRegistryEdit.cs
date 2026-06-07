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
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for MessageBoxRegistryEdit.
    /// </summary>
    public class MessageBoxRegistryEdit : System.Windows.Forms.Form
    {
        private System.Windows.Forms.TreeView tvRegistry;
        private System.Windows.Forms.Label lbText;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;
        private System.Windows.Forms.Button btOK;

        public MessageBoxRegistryEdit()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            string[] aStrMsgBoxes = 
                MessageBoxManager.GetMessageBoxSubKeyNames();

            foreach (string str in aStrMsgBoxes)
            {

                bool bDoNotShow = 
                    MessageBoxManager.GetDoNotShowAgainFromName(str);                              

                System.Windows.Forms.TreeNode tn = new TreeNode(str);
                tn.Checked = !bDoNotShow;

                tvRegistry.Nodes.Add(tn);
            }
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
            this.tvRegistry = new System.Windows.Forms.TreeView();
            this.lbText = new System.Windows.Forms.Label();
            this.btOK = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // tvRegistry
            // 
            this.tvRegistry.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.tvRegistry.CheckBoxes = true;
            this.tvRegistry.ImageIndex = -1;
            this.tvRegistry.Location = new System.Drawing.Point(0, 40);
            this.tvRegistry.Name = "tvRegistry";
            this.tvRegistry.SelectedImageIndex = -1;
            this.tvRegistry.Size = new System.Drawing.Size(436, 184);
            this.tvRegistry.TabIndex = 0;
            this.tvRegistry.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.tvRegistry_AfterCheck);
            // 
            // lbText
            // 
            this.lbText.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.lbText.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbText.Location = new System.Drawing.Point(6, 8);
            this.lbText.Name = "lbText";
            this.lbText.Size = new System.Drawing.Size(428, 16);
            this.lbText.TabIndex = 1;
            this.lbText.Text = "Active Message Boxes";
            this.lbText.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btOK
            // 
            this.btOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.btOK.Location = new System.Drawing.Point(184, 232);
            this.btOK.Name = "btOK";
            this.btOK.Size = new System.Drawing.Size(72, 24);
            this.btOK.TabIndex = 2;
            this.btOK.Text = "OK";
            this.btOK.Click += new System.EventHandler(this.btOK_Click);
            // 
            // MessageBoxRegistryEdit
            // 
            this.AcceptButton = this.btOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(440, 266);
            this.Controls.Add(this.btOK);
            this.Controls.Add(this.lbText);
            this.Controls.Add(this.tvRegistry);
            this.Name = "MessageBoxRegistryEdit";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Message Boxes Registry Edit";
            this.Load += new System.EventHandler(this.MessageBoxRegistryEdit_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void MessageBoxRegistryEdit_Load(object sender, System.EventArgs e)
        {
        
        }

        private void tvRegistry_AfterCheck(object sender, System.Windows.Forms.TreeViewEventArgs e)
        {
            string strEntry = e.Node.Text;

            bool bDoNotShow = 
                MessageBoxManager.GetDoNotShowAgainFromName(strEntry); 

            // Toggle it.
            MessageBoxManager.SetDoNotShowAgainToKey(strEntry,
                !bDoNotShow);
        }

        private void btOK_Click(object sender, System.EventArgs e)
        {
            this.Close();        
        }
    }
}
