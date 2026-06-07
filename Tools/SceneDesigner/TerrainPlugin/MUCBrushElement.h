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

#ifndef MUCBRUSHELEMENT_H
#define MUCBRUSHELEMENT_H

#include "MBrushOperation.h"
#include "MUCBrushElementPanel.h"


namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{

    public __gc class MUCBrushElement : public  
        System::Windows::Forms::UserControl
    {

    public:
        MUCBrushElement(void)
        {
            InitializeComponent();
            m_iNextPropertyX = 0;
            m_iNextPropertyY = 0;
            m_pmOperation = 0;
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~MUCBrushElement()
        {
            
            if (m_pmComponents)
            {
                m_pmComponents->Dispose();
            }

            Controls->Clear();
            
                
        }

    private:
        System::Windows::Forms::Panel*  m_pmPropertiesPanel;
        System::Windows::Forms::Button*  m_pmBtnDropDown;
        System::Windows::Forms::Label*  m_pmElementDesc;
        System::Windows::Forms::Label*  m_pmElementName;

        System::Windows::Forms::PictureBox*  m_pmPicImage;
        System::Windows::Forms::ContextMenuStrip*  m_pmContextMenu;
        System::Windows::Forms::ToolStripMenuItem*  
            m_pmRemoveToolStripMenuItem;
        System::Windows::Forms::ToolStripMenuItem*  
            m_pmUnfoldToolStripMenuItem;
        System::ComponentModel::IContainer*  m_pmComponents;

    private:
        /// <summary>
        /// Required designer variable.
        /// </summary>
        int m_iNextPropertyX;
        int m_iNextPropertyY;
        
    private: 
        MBrushOperation* m_pmOperation;
        
    public: 
        void SetTitle(char* pcTitle);
        void SetOperation(MBrushOperation* pmToAdd);
        void SetDescription(char* pcDesc);
        void SetPicture(char* pcPath);
        void SetPicture(Image* pmImage);

        void ActivateOperation();

        void RemovingTool();

        MBrushOperation* GetOperation();
        
        virtual void Update();
        

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void)
        {
            this->m_pmComponents = (new System::ComponentModel::Container());
            this->m_pmPropertiesPanel = (new System::Windows::Forms::Panel());
            this->m_pmBtnDropDown = (new System::Windows::Forms::Button());
            this->m_pmElementDesc = (new System::Windows::Forms::Label());
            this->m_pmElementName = (new System::Windows::Forms::Label());
            this->m_pmPicImage = (new System::Windows::Forms::PictureBox());
            this->m_pmContextMenu = (new System::Windows::Forms::
                ContextMenuStrip(this->m_pmComponents));
            this->m_pmRemoveToolStripMenuItem = (new System::Windows::Forms::
                ToolStripMenuItem());
            this->m_pmUnfoldToolStripMenuItem = (new System::Windows::Forms::
                ToolStripMenuItem());
            (__try_cast<System::ComponentModel::ISupportInitialize*  >
                (this->m_pmPicImage))->BeginInit();
            this->m_pmContextMenu->SuspendLayout();
            this->SuspendLayout();
            // 
            // m_PropertiesPanel
            // 
            this->m_pmPropertiesPanel->AllowDrop = true;
            this->m_pmPropertiesPanel->AutoScroll = true;
            this->m_pmPropertiesPanel->AutoSize = true;
            this->m_pmPropertiesPanel->AutoSizeMode = 
                System::Windows::Forms::AutoSizeMode::GrowAndShrink;
            this->m_pmPropertiesPanel->BackColor = 
                System::Drawing::SystemColors::Window;
            this->m_pmPropertiesPanel->BorderStyle = 
                System::Windows::Forms::BorderStyle::Fixed3D;
            this->m_pmPropertiesPanel->Dock = 
                System::Windows::Forms::DockStyle::Bottom;
            this->m_pmPropertiesPanel->Location = 
                System::Drawing::Point(0, 47);
            this->m_pmPropertiesPanel->MinimumSize = 
                System::Drawing::Size(190, 4);
            this->m_pmPropertiesPanel->Name = S"m_PropertiesPanel";
            this->m_pmPropertiesPanel->Size = System::Drawing::Size(243, 4);
            this->m_pmPropertiesPanel->TabIndex = 9;
            this->m_pmPropertiesPanel->Visible = false;
            // 
            // m_BtnDropDown
            // 
            this->m_pmBtnDropDown->AutoSize = true;
            this->m_pmBtnDropDown->AutoSizeMode = 
                System::Windows::Forms::AutoSizeMode::GrowAndShrink;
            this->m_pmBtnDropDown->Dock = 
                System::Windows::Forms::DockStyle::Right;
            this->m_pmBtnDropDown->Location = System::Drawing::Point(243, 0);
            this->m_pmBtnDropDown->Name = S"m_BtnDropDown";
            this->m_pmBtnDropDown->Size = System::Drawing::Size(23, 51);
            this->m_pmBtnDropDown->TabIndex = 8;
            this->m_pmBtnDropDown->Text = S">";
            this->m_pmBtnDropDown->UseVisualStyleBackColor = true;
            this->m_pmBtnDropDown->Click += 
                new System::EventHandler(this, 
                &MUCBrushElement::BtnDropDown_Click);
            // 
            // m_ElementDesc
            // 
            this->m_pmElementDesc->AutoSize = true;
            this->m_pmElementDesc->Location = System::Drawing::Point(57, 29);
            this->m_pmElementDesc->Name = S"m_ElementDesc";
            this->m_pmElementDesc->Size = System::Drawing::Size(0, 13);
            this->m_pmElementDesc->TabIndex = 7;
            this->m_pmElementDesc->MouseClick += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseClick);
            // 
            // m_ElementName
            // 
            this->m_pmElementName->AutoSize = true;
            this->m_pmElementName->Font = 
                (new System::Drawing::Font(S"Microsoft Sans Serif", 9.75F, 
                System::Drawing::FontStyle::Bold, 
                System::Drawing::GraphicsUnit::Point, 
                (System::Byte)0));
            this->m_pmElementName->Location = System::Drawing::Point(52, 5);
            this->m_pmElementName->Name = S"m_ElementName";
            this->m_pmElementName->Size = System::Drawing::Size(55, 16);
            this->m_pmElementName->TabIndex = 6;
            this->m_pmElementName->Text = S"Label1";
            this->m_pmElementName->MouseClick += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseClick);
            // 
            // m_PicImage
            // 
            this->m_pmPicImage->BackColor = 
                System::Drawing::SystemColors::ButtonHighlight;
            this->m_pmPicImage->BorderStyle = 
                System::Windows::Forms::BorderStyle::FixedSingle;
            this->m_pmPicImage->Location = System::Drawing::Point(4, 5);
            this->m_pmPicImage->Name = S"m_PicImage";
            this->m_pmPicImage->Size = System::Drawing::Size(42, 37);
            this->m_pmPicImage->SizeMode = 
                System::Windows::Forms::PictureBoxSizeMode::CenterImage;
            this->m_pmPicImage->TabIndex = 5;
            this->m_pmPicImage->TabStop = false;
            this->m_pmPicImage->MouseClick += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseClick);
            // 
            // m_ContextMenu
            // 
            System::Windows::Forms::ToolStripItem* __mcTemp__1[] = 
                new System::Windows::Forms::ToolStripItem*[2];
            __mcTemp__1[0] = this->m_pmRemoveToolStripMenuItem;
            __mcTemp__1[1] = this->m_pmUnfoldToolStripMenuItem;
            this->m_pmContextMenu->Items->AddRange(__mcTemp__1);
            this->m_pmContextMenu->Name = S"contextMenuStrip1";
            this->m_pmContextMenu->ShowImageMargin = false;
            this->m_pmContextMenu->Size = System::Drawing::Size(100, 48);
            // 
            // m_RemoveToolStripMenuItem
            // 
            this->m_pmRemoveToolStripMenuItem->Name = 
                S"m_RemoveToolStripMenuItem";
            this->m_pmRemoveToolStripMenuItem->Size = 
                System::Drawing::Size(99, 22);
            this->m_pmRemoveToolStripMenuItem->Text = S"Remove";
            this->m_pmRemoveToolStripMenuItem->Click += 
                new System::EventHandler(this, 
                &MUCBrushElement::RemoveToolStripMenuItem_Click);
            // 
            // m_UnfoldToolStripMenuItem
            // 
            this->m_pmUnfoldToolStripMenuItem->Name = 
                S"m_UnfoldToolStripMenuItem";
            this->m_pmUnfoldToolStripMenuItem->Size = 
                System::Drawing::Size(99, 22);
            this->m_pmUnfoldToolStripMenuItem->Text = S"Unfold";
            this->m_pmUnfoldToolStripMenuItem->Click += 
                new System::EventHandler(this, 
                &MUCBrushElement::UnfoldToolStripMenuItem_Click);
            // 
            // AeUCBrushElement
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->AutoSizeMode = 
                System::Windows::Forms::AutoSizeMode::GrowAndShrink;
            this->BackColor = System::Drawing::SystemColors::ControlLight;
            this->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
            this->Controls->Add(this->m_pmPropertiesPanel);
            this->Controls->Add(this->m_pmBtnDropDown);
            this->Controls->Add(this->m_pmPicImage);
            this->Controls->Add(this->m_pmElementName);
            this->Controls->Add(this->m_pmElementDesc);
            this->Name = S"MUCBrushElement";
            this->Size = System::Drawing::Size(266, 51);
            this->DragOver += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_DragOver);
            this->MouseDown += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseDown);
            this->MouseMove += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseMove);
            this->MouseClick += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseClick);
            this->DragDrop += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_DragDrop);
            this->DragEnter += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_DragEnter);
            this->MouseUp += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_MouseUp);
            this->GiveFeedback += 
                new System::Windows::Forms::GiveFeedbackEventHandler(this, 
                &MUCBrushElement::MUCBrushElement_GiveFeedback);
            (__try_cast<System::ComponentModel::ISupportInitialize*  >
                (this->m_pmPicImage))->EndInit();
            this->m_pmContextMenu->ResumeLayout(false);
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    
    private: 
        
        //===================================================================
        // Handled Events
        //===================================================================

        System::Void BtnDropDown_Click(System::Object*  sender, 
            System::EventArgs*  e) ;

        System::Void MUCBrushElement_MouseClick(System::Object*  sender, 
             System::Windows::Forms::MouseEventArgs*  e) ;

        System::Void UnfoldToolStripMenuItem_Click(System::Object*  sender,
             System::EventArgs*  e) ;

        System::Void RemoveToolStripMenuItem_Click(System::Object*  sender, 
             System::EventArgs*  e) ;
        
        System::Void MUCBrushElement_MouseDown(System::Object*  sender, 
             System::Windows::Forms::MouseEventArgs*  e) 
        {
            NI_UNUSED_ARG(sender);
            (dynamic_cast<MUCBrushElementPanel*>
                (Parent->Parent))->RaiseMouseDownEvent(this,e);
        }
        System::Void MUCBrushElement_MouseUp(System::Object*  sender, 
             System::Windows::Forms::MouseEventArgs*  e) 
        {
            NI_UNUSED_ARG(sender);
            (dynamic_cast<MUCBrushElementPanel*>
                (Parent->Parent))->RaiseMouseUpEvent(this,e);
        }
        System::Void MUCBrushElement_MouseMove(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e) 
        {
            NI_UNUSED_ARG(sender);
            (dynamic_cast<MUCBrushElementPanel*>
                (Parent->Parent))->RaiseMouseMoveEvent(this,e);
        }
        System::Void MUCBrushElement_DragDrop(System::Object*  sender,
            System::Windows::Forms::DragEventArgs*  e) 
        {
            NI_UNUSED_ARG(sender);
            (dynamic_cast<MUCBrushElementPanel*>
                (Parent->Parent))->RaiseDragDropEvent(this,e);
        }
        System::Void MUCBrushElement_DragOver(System::Object*  sender, 
            System::Windows::Forms::DragEventArgs*  e) 
        {
            NI_UNUSED_ARG(sender);
            (dynamic_cast<MUCBrushElementPanel*>
                (Parent->Parent))->RaiseDragOverEvent(this,e);
        }
        System::Void MUCBrushElement_DragEnter(System::Object*  sender, 
            System::Windows::Forms::DragEventArgs*  e) 
        {
            NI_UNUSED_ARG(e);
            NI_UNUSED_ARG(sender);
        }
        System::Void MUCBrushElement_GiveFeedback(System::Object*  sender, 
             System::Windows::Forms::GiveFeedbackEventArgs*  e) 
        {
            NI_UNUSED_ARG(sender);
            (dynamic_cast<MUCBrushElementPanel*>
                (Parent->Parent))->RaiseGiveFeedbackEvent(this,e);
        }
};

}
}
}
}

#endif
