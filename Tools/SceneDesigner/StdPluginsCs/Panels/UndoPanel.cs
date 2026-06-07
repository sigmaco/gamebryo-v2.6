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
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    /// <summary>
    /// Summary description for UndoPanel.
    /// </summary>
    public class UndoPanel : System.Windows.Forms.Form
    {
        private System.Windows.Forms.Button m_btnExecute;
        private System.Windows.Forms.RadioButton m_rbUndo;
        private System.Windows.Forms.GroupBox m_gbCommandType;
        private System.Windows.Forms.RadioButton m_rbRedo;
        private System.Windows.Forms.Label m_lblDummy;
        private System.Windows.Forms.ListBox m_lbCommands;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public UndoPanel()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(UndoPanel));
            this.m_lbCommands = new System.Windows.Forms.ListBox();
            this.m_btnExecute = new System.Windows.Forms.Button();
            this.m_rbUndo = new System.Windows.Forms.RadioButton();
            this.m_gbCommandType = new System.Windows.Forms.GroupBox();
            this.m_rbRedo = new System.Windows.Forms.RadioButton();
            this.m_lblDummy = new System.Windows.Forms.Label();
            this.m_gbCommandType.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_lbCommands
            // 
            this.m_lbCommands.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
                | System.Windows.Forms.AnchorStyles.Left) 
                | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lbCommands.IntegralHeight = false;
            this.m_lbCommands.Location = new System.Drawing.Point(8, 8);
            this.m_lbCommands.Name = "m_lbCommands";
            this.m_lbCommands.Size = new System.Drawing.Size(480, 248);
            this.m_lbCommands.TabIndex = 0;
            this.m_lbCommands.DoubleClick += new System.EventHandler(this.m_lbCommands_DoubleClick);
            // 
            // m_btnExecute
            // 
            this.m_btnExecute.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_btnExecute.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnExecute.Location = new System.Drawing.Point(292, 280);
            this.m_btnExecute.Name = "m_btnExecute";
            this.m_btnExecute.Size = new System.Drawing.Size(60, 23);
            this.m_btnExecute.TabIndex = 2;
            this.m_btnExecute.Text = "Undo";
            this.m_btnExecute.Click += new System.EventHandler(this.m_btnExecute_Click);
            // 
            // m_rbUndo
            // 
            this.m_rbUndo.Location = new System.Drawing.Point(16, 16);
            this.m_rbUndo.Name = "m_rbUndo";
            this.m_rbUndo.Size = new System.Drawing.Size(56, 24);
            this.m_rbUndo.TabIndex = 0;
            this.m_rbUndo.Text = "Undo";
            this.m_rbUndo.CheckedChanged += new System.EventHandler(this.m_rbUndo_CheckedChanged);
            // 
            // m_gbCommandType
            // 
            this.m_gbCommandType.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.m_gbCommandType.Controls.Add(this.m_rbRedo);
            this.m_gbCommandType.Controls.Add(this.m_rbUndo);
            this.m_gbCommandType.Location = new System.Drawing.Point(142, 264);
            this.m_gbCommandType.Name = "m_gbCommandType";
            this.m_gbCommandType.Size = new System.Drawing.Size(144, 48);
            this.m_gbCommandType.TabIndex = 1;
            this.m_gbCommandType.TabStop = false;
            this.m_gbCommandType.Text = "Show commands to:";
            // 
            // m_rbRedo
            // 
            this.m_rbRedo.Location = new System.Drawing.Point(80, 16);
            this.m_rbRedo.Name = "m_rbRedo";
            this.m_rbRedo.Size = new System.Drawing.Size(56, 24);
            this.m_rbRedo.TabIndex = 1;
            this.m_rbRedo.Text = "Redo";
            this.m_rbRedo.CheckedChanged += new System.EventHandler(this.m_rbRedo_CheckedChanged);
            // 
            // m_lblDummy
            // 
            this.m_lblDummy.Location = new System.Drawing.Point(224, 80);
            this.m_lblDummy.Name = "m_lblDummy";
            this.m_lblDummy.Size = new System.Drawing.Size(8, 8);
            this.m_lblDummy.TabIndex = 3;
            // 
            // UndoPanel
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(496, 318);
            this.Controls.Add(this.m_gbCommandType);
            this.Controls.Add(this.m_btnExecute);
            this.Controls.Add(this.m_lbCommands);
            this.Controls.Add(this.m_lblDummy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "UndoPanel";
            this.Text = "Undo";
            this.Load += new System.EventHandler(this.UndoPanel_Load);
            this.m_gbCommandType.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private void UndoPanel_Load(object sender, System.EventArgs e)
        {
            RegisterEventHandlers();
            SetUndoMode();
            SyncRadioButtonsToExecuteMode();
        }

        #region Event Handlers
        private void RegisterEventHandlers()
        {
            CommandService.CommandsAddedToUndoStack +=
                new UndoRedoHandler(this.OnUndoStackChanged);
            CommandService.CommandsRemovedFromUndoStack +=
                new UndoRedoHandler(this.OnUndoStackChanged);
            CommandService.CommandsAddedToRedoStack +=
                new UndoRedoHandler(this.OnRedoStackChanged);
            CommandService.CommandsRemovedFromRedoStack +=
                new UndoRedoHandler(this.OnRedoStackChanged);
            MFramework.Instance.EventManager.LongOperationCompleted +=
                new MEventManager.__Delegate_LongOperationCompleted(
                this.OnLongOperationCompleted);
        }

        private void OnUndoStackChanged(ICommand[] amCommands)
        {
            if (m_bUndoMode && !MFramework.Instance.PerformingLongOperation)
            {
                SyncListBoxToCommandService();
            }
        }

        private void OnRedoStackChanged(ICommand[] amCommands)
        {
            if (!m_bUndoMode && !MFramework.Instance.PerformingLongOperation)
            {
                SyncListBoxToCommandService();
            }
        }

        private void OnLongOperationCompleted()
        {
            SyncListBoxToCommandService();
        }
        #endregion

        #region CommandService
        private static ICommandService ms_pmCommandService = null;

        private static ICommandService CommandService
        {
            get
            {
                if (ms_pmCommandService == null)
                {
                    ms_pmCommandService = ServiceProvider.Instance.GetService(
                        typeof(ICommandService)) as ICommandService;
                    Debug.Assert(ms_pmCommandService != null, "Command " +
                        "service not found!");
                }
                return ms_pmCommandService;
            }
        }
        #endregion

        #region Execute Mode
        private bool m_bUndoMode = true;

        private void SetUndoMode()
        {
            m_bUndoMode = true;
            m_btnExecute.Text = "Undo";
            SyncListBoxToCommandService();
        }

        private void SetRedoMode()
        {
            m_bUndoMode = false;
            m_btnExecute.Text = "Redo";
            SyncListBoxToCommandService();
        }
        #endregion

        #region Radio Buttons
        private bool m_bSynching = false;

        private void SyncRadioButtonsToExecuteMode()
        {
            m_bSynching = true;
            if (m_bUndoMode)
            {
                m_rbUndo.Checked = true;
                m_rbRedo.Checked = false;
            }
            else
            {
                m_rbUndo.Checked = false;
                m_rbRedo.Checked = true;
            }
            m_bSynching = false;
        }

        private void m_rbUndo_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bSynching && m_rbUndo.Checked == true)
            {
                SetUndoMode();
            }
        }

        private void m_rbRedo_CheckedChanged(object sender,
            System.EventArgs e)
        {
            if (!m_bSynching && m_rbRedo.Checked == true)
            {
                SetRedoMode();
            }
        }
        #endregion

        #region List Box
        private class CommandWrapper
        {
            public CommandWrapper(ICommand pmCommand)
            {
                Debug.Assert(pmCommand != null, "Null command provided to " +
                    "constructor!");
                m_pmCommand = pmCommand;
            }

            private ICommand m_pmCommand;
            public ICommand Command
            {
                get
                {
                    return m_pmCommand;
                }
                set
                {
                    if (value != null)
                    {
                        m_pmCommand = value;
                    }
                }
            }

            public override string ToString()
            {
                return m_pmCommand.Name;
            }
        }

        private void SyncListBoxToCommandService()
        {
            m_lbCommands.Items.Clear();

            ICommand[] amCommands = null;
            if (m_bUndoMode)
            {
                amCommands = CommandService.GetUndoCommands();
            }
            else
            {
                amCommands = CommandService.GetRedoCommands();
            }

            foreach (ICommand pmCommand in amCommands)
            {
                m_lbCommands.Items.Add(new CommandWrapper(pmCommand));
            }

            if (m_lbCommands.Items.Count > 0)
            {
                m_lbCommands.SelectedIndex = 0;
                m_btnExecute.Enabled = true;
            }
            else
            {
                m_btnExecute.Enabled = false;
            }
        }
        #endregion

        #region Command Execution
        private void ExecuteCommand()
        {
            if (m_lbCommands.SelectedIndex > -1)
            {
                if (m_bUndoMode)
                {
                    CommandService.UndoCommands(
                        (uint) m_lbCommands.SelectedIndex + 1);
                }
                else
                {
                    CommandService.RedoCommands(
                        (uint) m_lbCommands.SelectedIndex + 1);
                }
                SyncListBoxToCommandService();
            }
        }

        private void m_btnExecute_Click(object sender, System.EventArgs e)
        {
            ExecuteCommand();
        }

        private void m_lbCommands_DoubleClick(object sender,
            System.EventArgs e)
        {
            ExecuteCommand();
        }
        #endregion
    }
}
