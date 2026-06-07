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

#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework

{
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    /// <summary> 
    /// Summary for Form1
    ///
    /// WARNING: 
    /// If you change the name of this class, you will need to change the 
    /// 'Resource File Name' property for the managed resource compiler tool 
    /// associated with all .resx files this class depends on.  Otherwise,
    /// the designers will not be able to interact properly with localized
    /// resources associated with this form.
    /// </summary>
    private __gc class MCollectionPropertyEditorForm : 
        public System::Windows::Forms::Form
    {   
    public:
        MCollectionPropertyEditorForm();

        //__property PropertyDescriptor* get_Descriptor(PropertyDescriptor* 
        //    pmDescriptor);
        //__property void set_Descriptor(PropertyDescriptor* pmDescriptor);

        __property Object* get_EditValue()[];
        __property void set_EditValue(Object* pmValue[]);

    protected:
        void Dispose(Boolean disposing)
        {
            if (disposing && components)
            {
                components->Dispose();
            }
            __super::Dispose(disposing);
        }


    private: System::Windows::Forms::Label *  label2;



    private: System::Windows::Forms::NumericUpDown *  m_pmctlNumberOfItems;
    private: System::Windows::Forms::Button *  m_btnOk;
    private: System::Windows::Forms::Button *  m_btnCancel;

    private:
        /// <summary>
        /// Required designer variable.
        /// </summary>
        System::ComponentModel::Container * components;

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        // Begin auto generated code


        void InitializeComponent(void)
        {
            this->label2 = new System::Windows::Forms::Label();
            this->m_pmctlNumberOfItems = new System::Windows::Forms::NumericUpDown();
            this->m_btnOk = new System::Windows::Forms::Button();
            this->m_btnCancel = new System::Windows::Forms::Button();
            (__try_cast<System::ComponentModel::ISupportInitialize *  >(this->m_pmctlNumberOfItems))->BeginInit();
            this->SuspendLayout();
            // 
            // label2
            // 
            this->label2->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->label2->Location = System::Drawing::Point(16, 24);
            this->label2->Name = S"label2";
            this->label2->TabIndex = 2;
            this->label2->Text = S"Number of Items:";
            // 
            // m_pmctlNumberOfItems
            // 
            this->m_pmctlNumberOfItems->Location = System::Drawing::Point(128, 24);
            System::Int32 __mcTemp__1[] = new System::Int32[4];
            __mcTemp__1[0] = 100000;
            __mcTemp__1[1] = 0;
            __mcTemp__1[2] = 0;
            __mcTemp__1[3] = 0;
            this->m_pmctlNumberOfItems->Maximum = System::Decimal(__mcTemp__1);
            System::Int32 __mcTemp__2[] = new System::Int32[4];
            __mcTemp__2[0] = 1;
            __mcTemp__2[1] = 0;
            __mcTemp__2[2] = 0;
            __mcTemp__2[3] = 0;
            this->m_pmctlNumberOfItems->Minimum = System::Decimal(__mcTemp__2);
            this->m_pmctlNumberOfItems->Name = S"m_pmctlNumberOfItems";
            this->m_pmctlNumberOfItems->Size = System::Drawing::Size(56, 20);
            this->m_pmctlNumberOfItems->TabIndex = 4;
            System::Int32 __mcTemp__3[] = new System::Int32[4];
            __mcTemp__3[0] = 1;
            __mcTemp__3[1] = 0;
            __mcTemp__3[2] = 0;
            __mcTemp__3[3] = 0;
            this->m_pmctlNumberOfItems->Value = System::Decimal(__mcTemp__3);
            // 
            // m_btnOk
            // 
            this->m_btnOk->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
            this->m_btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->m_btnOk->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->m_btnOk->Location = System::Drawing::Point(16, 72);
            this->m_btnOk->Name = S"m_btnOk";
            this->m_btnOk->TabIndex = 5;
            this->m_btnOk->Text = S"OK";
            this->m_btnOk->Click += new System::EventHandler(this, &MCollectionPropertyEditorForm::OnOk_Click);
            // 
            // m_btnCancel
            // 
            this->m_btnCancel->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
            this->m_btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->m_btnCancel->FlatStyle = System::Windows::Forms::FlatStyle::System;
            this->m_btnCancel->Location = System::Drawing::Point(120, 72);
            this->m_btnCancel->Name = S"m_btnCancel";
            this->m_btnCancel->TabIndex = 6;
            this->m_btnCancel->Text = S"Cancel";
            // 
            // MCollectionPropertyEditorForm
            // 
            this->AcceptButton = this->m_btnOk;
            this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
            this->CancelButton = this->m_btnCancel;
            this->ClientSize = System::Drawing::Size(218, 110);
            this->ControlBox = false;
            this->Controls->Add(this->m_btnCancel);
            this->Controls->Add(this->m_btnOk);
            this->Controls->Add(this->m_pmctlNumberOfItems);
            this->Controls->Add(this->label2);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->Name = S"MCollectionPropertyEditorForm";
            this->ShowInTaskbar = false;
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
            this->Text = S"Collection Editor";
            (__try_cast<System::ComponentModel::ISupportInitialize *  >(this->m_pmctlNumberOfItems))->EndInit();
            this->ResumeLayout(false);

        }   

        // End auto generated code        

        Object* m_amEditValue[];
        void OnOk_Click(System::Object* sender, System::EventArgs*  e);
    };
}}}}
