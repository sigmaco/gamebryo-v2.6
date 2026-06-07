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

#ifndef MUCBRUSHELEMENTPANEL_H
#define MUCBRUSHELEMENTPANEL_H

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    public __gc class MUCBrushElement;

    public __gc class MUCBrushElementPanel : public  
        System::Windows::Forms::UserControl
    {

        public:
        MUCBrushElementPanel(void)
        {
            InitializeComponent();

            m_iNextPositionX = 0;
            m_iNextPositionY = 0;
            m_iPositionModifierX = 0;
            m_iPositionModifierY = 50;
            m_iCurrentScrollValue = 0;
            m_bDragging = false;
            m_pmDraggedObject = NULL;
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~MUCBrushElementPanel()
        {
            Controls->Clear();
        }
    
    private:
        /// <summary>
        /// Required designer variable.
        /// </summary>
        System::Windows::Forms::Panel*  m_pmBasePanel;

        int m_iNextPositionX;
        int m_iNextPositionY;
        int m_iPositionModifierX;
        int m_iPositionModifierY;
        int m_iCurrentScrollValue;
        bool m_bDragging;

        MUCBrushElement* m_pmDraggedObject;

        System::Drawing::Rectangle m_kDragingBox;
        System::Int32 m_iIndexOfItemToDrag;
        int m_iIndexWhereToDrop;
        int m_iScrollValue;
        int m_iDropScrollValue;
    private: System::Windows::Forms::VScrollBar*  m_pmScrollBar1;
    private: System::Windows::Forms::PictureBox*  m_pmPictureBox1;

        Point m_kScreenOffset;

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void)
        {
            this->m_pmBasePanel = (new System::Windows::Forms::Panel());
            this->m_pmPictureBox1 = (new System::Windows::Forms::PictureBox());
            this->m_pmScrollBar1 = (new System::Windows::Forms::VScrollBar());
            this->m_pmBasePanel->SuspendLayout();
            (__try_cast<System::ComponentModel::ISupportInitialize*  >
                (this->m_pmPictureBox1))->BeginInit();
            this->SuspendLayout();
            // 
            // m_kBasePanel
            // 
            this->m_pmBasePanel->AllowDrop = true;
            this->m_pmBasePanel->AutoSizeMode = 
                System::Windows::Forms::AutoSizeMode::GrowAndShrink;
            this->m_pmBasePanel->BackColor = 
                System::Drawing::SystemColors::Window;
            this->m_pmBasePanel->BackgroundImageLayout = 
                System::Windows::Forms::ImageLayout::Stretch;
            this->m_pmBasePanel->BorderStyle = 
                System::Windows::Forms::BorderStyle::Fixed3D;
            this->m_pmBasePanel->Controls->Add(this->m_pmPictureBox1);
            this->m_pmBasePanel->Dock = 
                System::Windows::Forms::DockStyle::Fill;
            this->m_pmBasePanel->Location = System::Drawing::Point(0, 0);
            this->m_pmBasePanel->Name = S"m_kBasePanel";
            this->m_pmBasePanel->Size = System::Drawing::Size(373, 329);
            this->m_pmBasePanel->TabIndex = 0;
            this->m_pmBasePanel->MouseWheel += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseWheel);
            this->m_pmBasePanel->DragOver += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragOver);
            this->m_pmBasePanel->MouseDown += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseDown);
            this->m_pmBasePanel->MouseMove += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseMove);
            this->m_pmBasePanel->ControlRemoved += 
                new System::Windows::Forms::ControlEventHandler(this, 
                &MUCBrushElementPanel::BasePanel_ControlRemoved);
            this->m_pmBasePanel->DragDrop += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragDrop);
            this->m_pmBasePanel->DragEnter += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragEnter);
            this->m_pmBasePanel->Scroll += 
                new System::Windows::Forms::ScrollEventHandler(this, 
                &MUCBrushElementPanel::BasePanel_Scroll);
            this->m_pmBasePanel->MouseHover += 
                new System::EventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseHover);
            this->m_pmBasePanel->MouseUp += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseUp);
            this->m_pmBasePanel->DragLeave += 
                new System::EventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragLeave);
            // 
            // pictureBox1
            // 
            this->m_pmPictureBox1->BackColor = 
                System::Drawing::SystemColors::HotTrack;
            this->m_pmPictureBox1->Location = System::Drawing::Point(2, 314);
            this->m_pmPictureBox1->Name = S"pictureBox1";
            this->m_pmPictureBox1->Size = System::Drawing::Size(353, 4);
            this->m_pmPictureBox1->TabIndex = 0;
            this->m_pmPictureBox1->TabStop = false;
            this->m_pmPictureBox1->Visible = false;
            // 
            // vScrollBar1
            // 
            this->m_pmScrollBar1->Dock = 
                System::Windows::Forms::DockStyle::Right;
            this->m_pmScrollBar1->Enabled = false;
            this->m_pmScrollBar1->LargeChange = 1;
            this->m_pmScrollBar1->Location = System::Drawing::Point(353, 0);
            this->m_pmScrollBar1->Maximum = 0;
            this->m_pmScrollBar1->Name = S"vScrollBar1";
            this->m_pmScrollBar1->Size = System::Drawing::Size(20, 329);
            this->m_pmScrollBar1->TabIndex = 1;
            this->m_pmScrollBar1->ValueChanged += 
                new System::EventHandler(this, 
                &MUCBrushElementPanel::ScrollBar1_ValueChanged);
            // 
            // AeUCBrushElementPanel
            // 
            this->AllowDrop = true;
            this->BackColor = System::Drawing::SystemColors::Window;
            this->Controls->Add(this->m_pmScrollBar1);
            this->Controls->Add(this->m_pmBasePanel);
            this->Name = S"AeUCBrushElementPanel";
            this->Size = System::Drawing::Size(373, 329);
            this->MouseWheel += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseWheel);
            this->QueryContinueDrag += 
                new System::Windows::Forms::QueryContinueDragEventHandler(this,
                &MUCBrushElementPanel::MUCBrushElementPanel_QueryContinueDrag);
            this->DragOver += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragOver);
            this->MouseDown += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseDown);
            this->MouseMove += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseMove);
            this->DragDrop += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragDrop);
            this->DragEnter += 
                new System::Windows::Forms::DragEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragEnter);
            this->MouseHover += 
                new System::EventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseHover);
            this->MouseUp += 
                new System::Windows::Forms::MouseEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_MouseUp);
            this->GiveFeedback += 
                new System::Windows::Forms::GiveFeedbackEventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_GiveFeedback);
            this->DragLeave += 
                new System::EventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_DragLeave);
            this->SizeChanged += 
                new System::EventHandler(this, 
                &MUCBrushElementPanel::MUCBrushElementPanel_Resized);
            this->m_pmBasePanel->ResumeLayout(false);
            (__try_cast<System::ComponentModel::ISupportInitialize*  >
                (this->m_pmPictureBox1))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion

    public:

        void AddItemToList(System::Windows::Forms::UserControl* pmControl);
        void AddItemToList(MUCBrushElement* pmControl);
        void RemoveItemFromList(System::Windows::Forms::Control* pmControl);
        void RemoveAll();
        bool GetControlAtPosition(Point kPoint, bool bDrop);
        Control* GetControlByName(const char* pcName);
        Control* FindFirstBrushElement(String* pmElementName, String* 
            pmOperationName);
        void RedrawControl();
        virtual void Update();
        void SetDraggedObject(MUCBrushElement* pmNewTool);

        void RaiseMouseDownEvent(System::Object*  sender,
            System::Windows::Forms::MouseEventArgs*  e);
        void RaiseMouseUpEvent(System::Object*  sender,
            System::Windows::Forms::MouseEventArgs*  e);
        void RaiseMouseMoveEvent(System::Object*  sender,
            System::Windows::Forms::MouseEventArgs*  e);
        
        void RaiseDragDropEvent(System::Object*  sender,
            System::Windows::Forms::DragEventArgs*  e);
        void RaiseDragOverEvent(System::Object*  sender,
            System::Windows::Forms::DragEventArgs*  e);
        void RaiseGiveFeedbackEvent(System::Object*  sender,
            System::Windows::Forms::GiveFeedbackEventArgs*  e);

    private: 

        //===================================================================
        // Event handlers
        //===================================================================

        System::Void BasePanel_Scroll(System::Object*  sender, 
            System::Windows::Forms::ScrollEventArgs*  e);
    
        System::Void BasePanel_ControlRemoved(System::Object*  sender, 
                 System::Windows::Forms::ControlEventArgs*  e);
        
        System::Void MUCBrushElementPanel_DragDrop(System::Object*  sender, 
             System::Windows::Forms::DragEventArgs*  e);
        System::Void MUCBrushElementPanel_DragEnter(System::Object*  sender, 
            System::Windows::Forms::DragEventArgs*  e) ;
        System::Void MUCBrushElementPanel_DragLeave(System::Object*  sender, 
            System::EventArgs*  e) ;
        System::Void MUCBrushElementPanel_DragOver(System::Object*  sender, 
            System::Windows::Forms::DragEventArgs*  e) ;
        System::Void MUCBrushElementPanel_GiveFeedback(System::Object*  sender,
            System::Windows::Forms::GiveFeedbackEventArgs*  e) ;
        System::Void MUCBrushElementPanel_QueryContinueDrag(
            System::Object*  sender, 
            System::Windows::Forms::QueryContinueDragEventArgs*  e);
        
        System::Void MUCBrushElementPanel_MouseDown(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e) ;
        System::Void MUCBrushElementPanel_MouseMove(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e) ;
        System::Void MUCBrushElementPanel_MouseUp(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e) ;
        System::Void MUCBrushElementPanel_MouseWheel(System::Object*  sender, 
            System::Windows::Forms::MouseEventArgs*  e);
        System::Void MUCBrushElementPanel_MouseHover(System::Object*  sender, 
            System::EventArgs*  e) ;
        
        System::Void ScrollBar1_ValueChanged(System::Object*  sender, 
             System::EventArgs*  e) ;

        System::Void MUCBrushElementPanel_Resized(System::Object*  sender, 
             System::EventArgs*  e) ;
         
        
};

}
}
}
}

#endif