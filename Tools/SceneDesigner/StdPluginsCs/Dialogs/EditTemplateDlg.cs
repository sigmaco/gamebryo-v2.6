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
using System.Diagnostics;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs
{
    /// <summary>
    /// Summary description for EditTemplateDlg.
    /// </summary>
    public class EditTemplateDlg : System.Windows.Forms.Form
    {
        #region Private Data
        MEntity m_entity;
        IComponentService m_componentService;
        IUICommandService m_uiCommandService;
        ICommandService m_commandService;
        
        //ArrayList m_componentOperations;
        #endregion

        private System.Windows.Forms.Label m_lblEntityTemplate;
        private System.Windows.Forms.ListBox m_lbPropertySets;
        private System.Windows.Forms.ListBox m_lbEntityTemplate;
        private System.Windows.Forms.Button m_btnAdd;
        private System.Windows.Forms.Button m_btnRemove;
        private System.Windows.Forms.Button m_btnOK;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.Label m_lblComponents;
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public EditTemplateDlg(MEntity template)
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();
            // Templates cannot be prefabs so we will only 
            // get one entity when we clone this.
            MEntity[] amClones = template.Clone(template.Name, false);
            Debug.Assert(amClones.Length == 1);
            m_entity = amClones[0];
            UICommandService.BindCommands(this);
            //m_componentOperations = new ArrayList();
        }

        private enum ComponentOperationAction
        {
            Add,
            Remove
        }

        private class ComponentOperation
        {
            public ComponentOperationAction Action;
            public MComponent Component;
        }

        private class ComponentWrapper
        {
            public MComponent Component;

            public override string ToString()
            {
                return Component.Name;
            }

        }

        private IComponentService ComponentService
        {
            get
            {
                if (m_componentService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_componentService = 
                        sp.GetService(typeof(IComponentService))
                        as IComponentService;
                }
                return m_componentService;
            }
        }


        private MFramework FW
        {
            get { return MFramework.Instance; }
        }

        private IUICommandService UICommandService
        {
            get
            {
                if (m_uiCommandService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_uiCommandService = 
                        sp.GetService(typeof(IUICommandService))
                        as IUICommandService;
                }
                return m_uiCommandService;
            }
        }

        private ICommandService CommandService
        {
            get
            {
                if (m_commandService == null)
                {
                    ServiceProvider sp = ServiceProvider.Instance;
                    m_commandService = sp.GetService(typeof(ICommandService))
                        as ICommandService;
                }
                return m_commandService;
            }
        }

        public void ApplyOperations(MEntity entity)
        {
            ArrayList operations = BuildOperations(entity);
            if (operations.Count > 0)
            {
                string description = 
                    string.Format("Components changed in template \"{0}\"",
                    entity.Name);
                CommandService.BeginUndoFrame(description);
                foreach(ComponentOperation operation in operations)
                {
                    switch(operation.Action)
                    {
                        case ComponentOperationAction.Add:
                        {
                            entity.AddComponent(operation.Component, false,
                                true);
                            break;
                        }
                        case ComponentOperationAction.Remove:
                        {
                            MComponent componentToRemove = 
                                entity.GetComponentByTemplateID(
                                operation.Component.TemplateID);
                            entity.RemoveComponent(componentToRemove, false,
                                true);
                            break;
                        }
                    }
                }
                CommandService.EndUndoFrame(true);
            }
        }

        private ArrayList BuildOperations(MEntity entity)
        {
            ArrayList operationList = new ArrayList();
            //Look for components to add...
            MComponent[] newComponents = m_entity.GetComponents();
            foreach (MComponent newComponent in newComponents)
            {
                MComponent oldComponent = entity.GetComponentByTemplateID(
                    newComponent.TemplateID);
                if (oldComponent == null)
                {
                    ComponentOperation operation = new ComponentOperation();
                    operation.Action = ComponentOperationAction.Add;
                    operation.Component = ComponentService.CloneComponentByID(
                        newComponent.TemplateID);
                    operationList.Add(operation);
                }
            }
            //Now, look for components to remove
            MComponent[] oldComponents = entity.GetComponents();
            foreach (MComponent oldComponent in oldComponents)
            {
                MComponent newComponent = m_entity.GetComponentByTemplateID(
                    oldComponent.TemplateID);
                if (newComponent == null)
                {
                    ComponentOperation operation = new ComponentOperation();
                    operation.Action = ComponentOperationAction.Remove;
                    operation.Component = ComponentService.CloneComponentByID(
                        oldComponent.TemplateID);
                    operationList.Add(operation);
                }
            }
            return operationList;
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
                MFramework.Instance.EntityFactory.Remove(m_entity);
                UICommandService.UnBindCommands(this);
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
            System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(EditTemplateDlg));
            this.m_lblComponents = new System.Windows.Forms.Label();
            this.m_lblEntityTemplate = new System.Windows.Forms.Label();
            this.m_lbPropertySets = new System.Windows.Forms.ListBox();
            this.m_lbEntityTemplate = new System.Windows.Forms.ListBox();
            this.m_btnAdd = new System.Windows.Forms.Button();
            this.m_btnRemove = new System.Windows.Forms.Button();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_lblComponents
            // 
            this.m_lblComponents.AutoSize = true;
            this.m_lblComponents.Location = new System.Drawing.Point(8, 8);
            this.m_lblComponents.Name = "m_lblComponents";
            this.m_lblComponents.Size = new System.Drawing.Size(130, 16);
            this.m_lblComponents.TabIndex = 0;
            this.m_lblComponents.Text = "Assignable Components:";
            // 
            // m_lblEntityTemplate
            // 
            this.m_lblEntityTemplate.AutoSize = true;
            this.m_lblEntityTemplate.Location = new System.Drawing.Point(240, 8);
            this.m_lblEntityTemplate.Name = "m_lblEntityTemplate";
            this.m_lblEntityTemplate.Size = new System.Drawing.Size(86, 16);
            this.m_lblEntityTemplate.TabIndex = 3;
            this.m_lblEntityTemplate.Text = "Entity Template:";
            // 
            // m_lbPropertySets
            // 
            this.m_lbPropertySets.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.m_lbPropertySets.IntegralHeight = false;
            this.m_lbPropertySets.Location = new System.Drawing.Point(8, 24);
            this.m_lbPropertySets.Name = "m_lbPropertySets";
            this.m_lbPropertySets.Size = new System.Drawing.Size(184, 208);
            this.m_lbPropertySets.Sorted = true;
            this.m_lbPropertySets.TabIndex = 1;
            this.m_lbPropertySets.DoubleClick += new System.EventHandler(this.m_lbPropertySets_DoubleClick);
            this.m_lbPropertySets.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.m_lbPropertySets_DrawItem);
            // 
            // m_lbEntityTemplate
            // 
            this.m_lbEntityTemplate.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.m_lbEntityTemplate.IntegralHeight = false;
            this.m_lbEntityTemplate.Location = new System.Drawing.Point(240, 24);
            this.m_lbEntityTemplate.Name = "m_lbEntityTemplate";
            this.m_lbEntityTemplate.Size = new System.Drawing.Size(184, 208);
            this.m_lbEntityTemplate.Sorted = true;
            this.m_lbEntityTemplate.TabIndex = 4;
            this.m_lbEntityTemplate.DoubleClick += new System.EventHandler(this.m_lbEntityTemplate_DoubleClick);
            this.m_lbEntityTemplate.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.m_lbEntityTemplate_DrawItem);
            // 
            // m_btnAdd
            // 
            this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnAdd.Location = new System.Drawing.Point(200, 96);
            this.m_btnAdd.Name = "m_btnAdd";
            this.m_btnAdd.Size = new System.Drawing.Size(32, 23);
            this.m_btnAdd.TabIndex = 2;
            this.m_btnAdd.Text = "-->";
            this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
            // 
            // m_btnRemove
            // 
            this.m_btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnRemove.Location = new System.Drawing.Point(200, 128);
            this.m_btnRemove.Name = "m_btnRemove";
            this.m_btnRemove.Size = new System.Drawing.Size(32, 23);
            this.m_btnRemove.TabIndex = 5;
            this.m_btnRemove.Text = "<--";
            this.m_btnRemove.Click += new System.EventHandler(this.m_btnRemove_Click);
            // 
            // m_btnOK
            // 
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnOK.Location = new System.Drawing.Point(135, 240);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.TabIndex = 6;
            this.m_btnOK.Text = "OK";
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.m_btnCancel.Location = new System.Drawing.Point(224, 240);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.TabIndex = 7;
            this.m_btnCancel.Text = "Cancel";
            // 
            // EditTemplateDlg
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(434, 270);
            this.Controls.Add(this.m_lbEntityTemplate);
            this.Controls.Add(this.m_lbPropertySets);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_btnRemove);
            this.Controls.Add(this.m_btnAdd);
            this.Controls.Add(this.m_lblEntityTemplate);
            this.Controls.Add(this.m_lblComponents);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "EditTemplateDlg";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Edit Entity Template";
            this.Load += new System.EventHandler(this.EditTemplateDlg_Load);
            this.ResumeLayout(false);

        }
        #endregion

        #region Winforms Handlers

        private void EditTemplateDlg_Load(object sender, System.EventArgs e)
        {
            RefreshComponentLists();
        }

        private void m_btnRemove_Click(object sender, System.EventArgs e)
        {
            ListBox.SelectedObjectCollection selectedItems = 
                m_lbEntityTemplate.SelectedItems;
            foreach (ComponentWrapper wrapper in selectedItems)
            {
                MComponent component = 
                    FindComponent(wrapper.Component.TemplateID,
                    m_entity.GetComponents());
                int oldSize = m_entity.GetComponents().Length;                
                m_entity.RemoveComponent(component, true, false);
//                ComponentOperation operation = new ComponentOperation();
//                operation.Action = ComponentOperationAction.Remove;
//                operation.Component = component;
//                m_componentOperations.Add(operation);
                int newSize = m_entity.GetComponents().Length;
                Debug.Assert(newSize == (oldSize - 1));
            }
            RefreshComponentLists();
        }

        private void m_btnAdd_Click(object sender, System.EventArgs e)
        {
            ListBox.SelectedObjectCollection selectedItems = 
                m_lbPropertySets.SelectedItems;
            foreach (ComponentWrapper wrapper in selectedItems)
            {
                MComponent component = 
                    ComponentService.CloneComponentByID(wrapper.Component
                    .TemplateID);
                m_entity.AddComponent(component, true, false);
//                ComponentOperation operation = new ComponentOperation();
//                operation.Action = ComponentOperationAction.Add;
//                operation.Component = component;
//                m_componentOperations.Add(operation);
            }
            RefreshComponentLists();
        }

        private void m_lbPropertySets_DoubleClick(
            object sender, System.EventArgs e)
        {
            if (CanAdd(m_lbPropertySets.SelectedIndex))
            {
                m_btnAdd_Click(sender, e);
            }
        }

        private void m_lbEntityTemplate_DoubleClick(
            object sender, System.EventArgs e)
        {
            if(CanRemove(m_lbEntityTemplate.SelectedIndex))
            {
                m_btnRemove_Click(sender, e);                
            }
        
        }

        private void m_lbPropertySets_DrawItem(object sender, 
            System.Windows.Forms.DrawItemEventArgs e)
        {
            e.DrawBackground();
            e.DrawFocusRectangle();
            if (e.Index >= 0)
            {
                string text = m_lbPropertySets.Items[e.Index].ToString();
                Color textColor;
                if (CanAdd(e.Index))
                {
                    textColor = e.ForeColor;
                }
                else
                {
                    textColor = Color.Gray;
                }
                using (Brush brush = new SolidBrush(textColor))
                {
                    e.Graphics.DrawString(
                        text, e.Font, brush, 0, e.Bounds.Top);
                }
            }
        }

        private void m_lbEntityTemplate_DrawItem(object sender, 
            System.Windows.Forms.DrawItemEventArgs e)
        {
            e.DrawBackground();
            e.DrawFocusRectangle();
            if (e.Index >= 0)
            {
                string text = m_lbEntityTemplate.Items[e.Index].ToString();
                Color textColor;
                if (CanRemove(e.Index))
                {
                    textColor = e.ForeColor;
                }
                else
                {
                    textColor = Color.Gray;
                }
                using (Brush brush = new SolidBrush(textColor))
                {
                    e.Graphics.DrawString(text, e.Font, brush,
                        0, e.Bounds.Top);
                }
            }
        }

        #endregion

        #region Helper Methods
        private void RefreshComponentLists()
        {
            m_lbPropertySets.Items.Clear();
            m_lbEntityTemplate.Items.Clear();
            //string[] componentNames = ComponentService.GetComponentNames(); 
            MComponent[] allComponents = ComponentService.GetAllComponents();
            MComponent[] existingComponents = 
                m_entity.GetComponents();
            foreach (MComponent availableComponent in allComponents)
            {
                if (!ComponentService.IsComponentVisibleToUser(
                    availableComponent))
                {
                    continue;
                }

                if (FindComponent(availableComponent.TemplateID,
                    existingComponents) == null)
                {
                    ComponentWrapper wrapper = new ComponentWrapper();
                    wrapper.Component = availableComponent;
                    m_lbPropertySets.Items.Add(wrapper);
                }
            }
            foreach (MComponent component in existingComponents)
            { 
                ComponentWrapper wrapper = 
                    new ComponentWrapper();
                wrapper.Component = component;
                m_lbEntityTemplate.Items.Add(wrapper);
            }           
            
        }

        private static MComponent FindComponent(Guid Id, 
            MComponent[] existingComponents)
        {
            foreach (MComponent existingComponent in existingComponents)
            {
                if (existingComponent.TemplateID.Equals(Id))
                {
                    return existingComponent;
                }
            }
            return null;
        }

        private bool CanAdd(int index)
        {
            if (index >= 0)
            {
                ComponentWrapper wrapper = m_lbPropertySets.Items[index] 
                    as ComponentWrapper;
                MComponent component = 
                    wrapper.Component;
                return m_entity.CanAddComponent(component);
            }
            return false;
        }

        private bool CanRemove(int index)
        {
            if (index >= 0)
            {
                string text = m_lbEntityTemplate.Items[index].ToString();
                ComponentWrapper wrapper = m_lbEntityTemplate.Items[index] as 
                    ComponentWrapper;
                MComponent component = 
                    FindComponent(
                    wrapper.Component.TemplateID, m_entity.GetComponents());
                bool canRemove = m_entity.CanRemoveComponent(component);
                MComponent serviceComponent = 
                    ComponentService.GetComponentByID(component.TemplateID);
                return canRemove && (serviceComponent != null);
            }
            return false;
        }

        #endregion

        [UICommandHandler("Idle")]
        private void OnIdle(object sender, EventArgs args)
        {
            m_btnRemove.Enabled = CanRemove(m_lbEntityTemplate.SelectedIndex);

            m_btnAdd.Enabled = CanAdd(m_lbPropertySets.SelectedIndex);
        }


    }
}
