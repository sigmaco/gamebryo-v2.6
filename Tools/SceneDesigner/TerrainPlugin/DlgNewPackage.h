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
#include "NiFilename.h"

// This file has been auto generated

namespace Emergent { namespace Gamebryo { namespace SceneDesigner {
    namespace TerrainPlugin 
{
    /// <summary> 
    /// Summary for MSampleUIEditorDialog
    ///
    /// WARNING: If you change the name of this class, you will need to change the 
    ///          'Resource File Name' property for the managed resource compiler tool 
    ///          associated with all .resx files this class depends on.  Otherwise,
    ///          the designers will not be able to interact properly with localized
    ///          resources associated with this form.
    /// </summary>
    public __gc class DlgNewPackage : public System::Windows::Forms::Form
    {
    public:
        DlgNewPackage(void)
        {
            InitializeComponent();
        }

    System::Windows::Forms::Button* btCancel;
    System::Windows::Forms::Button* btOK;
    System::Windows::Forms::TextBox* tbInput;
    System::Windows::Forms::TextBox* tbLocation;
    public: 

    public: 
    private: System::Windows::Forms::Label*  label1;
    private: System::Windows::Forms::Button*  btBrowse;

             System::Windows::Forms::Label* lbText;
    // Note that the sliders are NOT an authoritative source of size data, always read via the GET functions.

    private: System::Void InitializeComponent() {
                 System::ComponentModel::ComponentResourceManager*  resources = (new System::ComponentModel::ComponentResourceManager(__typeof(DlgNewPackage)));

                 this->btCancel = (new System::Windows::Forms::Button());
                 this->btOK = (new System::Windows::Forms::Button());
                 this->tbInput = (new System::Windows::Forms::TextBox());
                 this->lbText = (new System::Windows::Forms::Label());
                 this->tbLocation = (new System::Windows::Forms::TextBox());
                 this->label1 = (new System::Windows::Forms::Label());
                 this->btBrowse = (new System::Windows::Forms::Button());
                 this->SuspendLayout();
                 // 
                 // btCancel
                 // 
                 this->btCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
                 this->btCancel->Location = System::Drawing::Point(196, 90);
                 this->btCancel->Name = S"btCancel";
                 this->btCancel->Size = System::Drawing::Size(75, 23);
                 this->btCancel->TabIndex = 0;
                 this->btCancel->Text = S"Cancel";
                 this->btCancel->UseVisualStyleBackColor = true;
                 // 
                 // btOK
                 // 
                 this->btOK->DialogResult = System::Windows::Forms::DialogResult::OK;
                 this->btOK->Location = System::Drawing::Point(115, 90);
                 this->btOK->Name = S"btOK";
                 this->btOK->Size = System::Drawing::Size(75, 23);
                 this->btOK->TabIndex = 1;
                 this->btOK->Text = S"OK";
                 this->btOK->UseVisualStyleBackColor = true;
                 // 
                 // tbInput
                 // 
                 this->tbInput->Location = System::Drawing::Point(6, 25);
                 this->tbInput->Name = S"tbInput";
                 this->tbInput->Size = System::Drawing::Size(265, 20);
                 this->tbInput->TabIndex = 2;
                 // 
                 // lbText
                 // 
                 this->lbText->AutoSize = true;
                 this->lbText->Location = System::Drawing::Point(3, 9);
                 this->lbText->Name = S"lbText";
                 this->lbText->Size = System::Drawing::Size(84, 13);
                 this->lbText->TabIndex = 3;
                 this->lbText->Text = S"Package Name:";
                 // 
                 // tbLocation
                 // 
                 this->tbLocation->Location = System::Drawing::Point(6, 64);
                 this->tbLocation->Name = S"tbLocation";
                 this->tbLocation->Size = System::Drawing::Size(241, 20);
                 this->tbLocation->TabIndex = 4;
                 // 
                 // label1
                 // 
                 this->label1->AutoSize = true;
                 this->label1->Location = System::Drawing::Point(3, 48);
                 this->label1->Name = S"label1";
                 this->label1->Size = System::Drawing::Size(97, 13);
                 this->label1->TabIndex = 5;
                 this->label1->Text = S"Package Location:";
                 // 
                 // btBrowse
                 // 
                 this->btBrowse->Location = System::Drawing::Point(245, 62);
                 this->btBrowse->Name = S"btBrowse";
                 this->btBrowse->Size = System::Drawing::Size(26, 23);
                 this->btBrowse->TabIndex = 6;
                 this->btBrowse->Text = S"...";
                 this->btBrowse->UseVisualStyleBackColor = true;
                 this->btBrowse->Click += new System::EventHandler(this, &DlgNewPackage::btBrowse_Click);
                 // 
                 // DlgNewPackage
                 // 
                 this->AcceptButton = this->btOK;
                 this->BackColor = System::Drawing::SystemColors::Control;
                 this->CancelButton = this->btCancel;
                 this->ClientSize = System::Drawing::Size(278, 119);
                 this->Controls->Add(this->btBrowse);
                 this->Controls->Add(this->tbLocation);
                 this->Controls->Add(this->label1);
                 this->Controls->Add(this->tbInput);
                 this->Controls->Add(this->lbText);
                 this->Controls->Add(this->btOK);
                 this->Controls->Add(this->btCancel);
                 this->Icon = (__try_cast<System::Drawing::Icon*>(resources->GetObject(S"$this.Icon")));
                 this->Name = S"DlgNewPackage";
                 this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
                 this->Text = S"New Material Package";
                 this->ResumeLayout(false);
                 this->PerformLayout();

             }    
    private: System::Void btBrowse_Click(System::Object*, System::EventArgs*) {
                // Figure out where to save the package
                SaveFileDialog* pmSaveFileDialog = new SaveFileDialog();
                pmSaveFileDialog->Filter = "Package Files (*.xml)|*.xml|All Files (*.*)|*.*";
                pmSaveFileDialog->CheckFileExists = false;
                pmSaveFileDialog->Title = "Select new package location";

                if(pmSaveFileDialog->ShowDialog()==DialogResult::Cancel)
                {
                    return;
                }

                tbLocation->Text = pmSaveFileDialog->FileName;
             }
};
}}}}
//END auto generated code
