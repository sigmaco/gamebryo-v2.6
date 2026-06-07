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

#include "TerrainPluginPCH.h"
#include "MTerrainPlugin.h"
#include "MUCBrushELementPanel.h"
#include "MUCBrushELement.h"

using namespace Emergent::Gamebryo::SceneDesigner::TerrainPlugin;

void MUCBrushElementPanel::AddItemToList(
    System::Windows::Forms::UserControl* pmControl)
{
    IUICommandService* pmMessageService = MGetService(IUICommandService);
    UICommand* pmCommand = pmMessageService->GetCommand("EnableTerrainBrush");
    pmCommand->DoClick(this, NULL);

    m_pmBasePanel->Focus();
    m_iScrollValue = m_pmScrollBar1->Value;
    m_iPositionModifierY = pmControl->Height;
    
    m_pmBasePanel->Controls->Add(pmControl);
    m_iIndexOfItemToDrag = m_pmBasePanel->Controls->IndexOf(pmControl);
    
    pmControl->Width = m_pmBasePanel->Width - 20;
    pmControl->Location = System::Drawing::Point(m_iNextPositionX, 
        m_iNextPositionY - m_pmScrollBar1->Value);
    
    RedrawControl();
    
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::AddItemToList(
    MUCBrushElement* pmControl)
{
    IUICommandService* pmMessageService = MGetService(IUICommandService);
    UICommand* pmCommand = pmMessageService->GetCommand("EnableTerrainBrush");
    pmCommand->DoClick(this, NULL);

    m_pmBasePanel->Focus();
    m_iScrollValue = m_pmScrollBar1->Value;
    m_iPositionModifierY = pmControl->Height;
    
    m_pmBasePanel->Controls->Add(pmControl);
    m_iIndexOfItemToDrag = m_pmBasePanel->Controls->IndexOf(pmControl);
    
    pmControl->Width = m_pmBasePanel->Width - 20;
    pmControl->Location = System::Drawing::Point(m_iNextPositionX, 
        m_iNextPositionY - m_pmScrollBar1->Value);

    pmControl->ActivateOperation();
    
    RedrawControl();
    
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RemoveItemFromList(
    System::Windows::Forms::Control* pmControl)
{
    MUCBrushElement* pmTemp = dynamic_cast<MUCBrushElement*>(pmControl);
    if (pmTemp)
    {
        pmTemp->RemovingTool(); 
    }
    m_pmBasePanel->Controls->Remove(pmControl);
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RemoveAll()
{
    for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
    {
        MUCBrushElement* pmTemp = dynamic_cast<MUCBrushElement*>
            (m_pmBasePanel->Controls->get_Item(i));
        if (pmTemp)
        {
            RemoveItemFromList(pmTemp);
            i--;
        }
    }
}
//---------------------------------------------------------------------------
bool MUCBrushElementPanel::GetControlAtPosition(Point kPoint, bool bDrop)
{
    IEnumerator* kEnum = m_pmBasePanel->Controls->GetEnumerator();
    for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
    {
        Point kPos = m_pmBasePanel->Controls->get_Item(i)->Location;
        int iWidth = m_pmBasePanel->Controls->get_Item(i)->Width;
        int iHeight = m_pmBasePanel->Controls->get_Item(i)->Height;

        if (kPoint.X > kPos.X && kPoint.X < kPos.X + iWidth)
        {
            if (!bDrop)
            {
                if (kPoint.Y > kPos.Y && 
                    kPoint.Y < kPos.Y + iHeight)
                {
                    m_iIndexOfItemToDrag = i;                    
                    m_iScrollValue = m_pmScrollBar1->Value;
                    kEnum = 0;
                    return true;
                }
            }
            else
            {
                if (kPoint.Y  > kPos.Y && 
                    kPoint.Y  < kPos.Y + iHeight)
                {
                    m_iIndexWhereToDrop = i;
                    m_iDropScrollValue = m_pmScrollBar1->Value;
                    kEnum = 0;
                    return true;
                }
            }
        }
    }
    kEnum = 0;
    return false;
    
}
//---------------------------------------------------------------------------
Control* MUCBrushElementPanel::GetControlByName(const char* pcName)
{
    String* pmName = new String(pcName);
    for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
    {
        if (String::Compare(m_pmBasePanel->Controls->get_Item(i)->Name,
            pmName)==0)
        {
            return m_pmBasePanel->Controls->get_Item(i);
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
Control* MUCBrushElementPanel::FindFirstBrushElement(String* pmElementName, 
    String* pmOperationName)
{
    if (pmOperationName && pmElementName)
    {
        for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
        {
            MUCBrushElement* pmItem = dynamic_cast<MUCBrushElement*>
                (m_pmBasePanel->Controls->get_Item(i));

            if (pmItem)
            {
                MBrushOperation* pmOperation = pmItem->GetOperation();
                if (String::Compare(
                    pmOperation->m_pmElement->m_pmName, pmElementName) == 0 && 
                    String::Compare(
                    pmOperation->m_pmName, pmOperationName) == 0)
                {
                    return pmItem;
                }
            }
        }
    }
    else if (pmOperationName)
    {
        return GetControlByName(MStringToCharPointer(pmOperationName));
    }
    else if (pmElementName)
    {
        for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
        {
            MUCBrushElement* pmItem = dynamic_cast<MUCBrushElement*>
                (m_pmBasePanel->Controls->get_Item(i));

            if (pmItem)
            {
                MBrushOperation* pmOperation = pmItem->GetOperation();
                if (String::Compare(
                    pmOperation->m_pmElement->m_pmName, pmElementName) == 0)
                {
                    return pmItem;
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RedrawControl()
{
    m_iNextPositionX = 0;
    m_iNextPositionY = 0;

    bool bDisable = true;
    int iMax = 0;
    int iStep = 0;

    m_iScrollValue = m_pmScrollBar1->Value;
    m_pmScrollBar1->Value = 0;

    for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
    {
        Control* pmListItem = m_pmBasePanel->Controls->get_Item(i);
        
        if (pmListItem->Visible)
        {
            pmListItem->Location = 
                Point(0, (m_iNextPositionY) 
                - m_pmScrollBar1->Value);
            
            m_iNextPositionX += m_iPositionModifierX;
            m_iNextPositionY += pmListItem->Height;
            
            iMax = (m_iNextPositionY - m_pmBasePanel->Height) + 
                    pmListItem->Height;

            iStep = pmListItem->Height;
        }
            
    }
    
    if (m_iNextPositionY >= m_pmBasePanel->Height)
    {
        m_pmScrollBar1->Enabled = true;
        m_pmScrollBar1->Maximum = iMax; 
        m_pmScrollBar1->LargeChange = iStep;
        m_pmScrollBar1->SmallChange = 1;

        bDisable = false;            
    }    

    if (bDisable && !m_bDragging)
    {
        m_pmScrollBar1->Value = 0;
        m_pmScrollBar1->Enabled = false;
    }
    else
    {
        if (m_iScrollValue < m_pmScrollBar1->Maximum)
        {
            m_pmScrollBar1->Value = m_iScrollValue;
        }
        else
        {
            m_pmScrollBar1->Value = m_pmScrollBar1->Maximum;
        }
    }    

    Update();
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::Update()
{
    for (int i = 0; i < m_pmBasePanel->Controls->Count; i++)
    {
        MUCBrushElement* pmControl = dynamic_cast<MUCBrushElement*>(
            m_pmBasePanel->Controls->get_Item(i)
            );
        if (pmControl)
            pmControl->Update();
    }    
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::SetDraggedObject(MUCBrushElement* pmNewTool)
{
    if (m_pmDraggedObject)
        m_pmDraggedObject = 0;

    m_pmDraggedObject = pmNewTool;
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RaiseMouseDownEvent(System::Object*  sender,
    System::Windows::Forms::MouseEventArgs*  e)
{
    this->MUCBrushElementPanel_MouseDown(sender,e);
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RaiseMouseUpEvent(System::Object*  sender,
    System::Windows::Forms::MouseEventArgs*  e)
{
    this->MUCBrushElementPanel_MouseUp(sender,e);
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RaiseMouseMoveEvent(System::Object*  sender,
    System::Windows::Forms::MouseEventArgs*  e)
{
    this->MUCBrushElementPanel_MouseMove(sender,e);
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RaiseDragDropEvent(System::Object*  sender,
    System::Windows::Forms::DragEventArgs*  e)
{
    this->MUCBrushElementPanel_DragDrop(sender,e);
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RaiseDragOverEvent(System::Object*  sender,
    System::Windows::Forms::DragEventArgs*  e)
{
    this->MUCBrushElementPanel_DragOver(sender,e);
}
//---------------------------------------------------------------------------
void MUCBrushElementPanel::RaiseGiveFeedbackEvent(System::Object*  sender,
    System::Windows::Forms::GiveFeedbackEventArgs*  e)
{
    this->MUCBrushElementPanel_GiveFeedback(sender,e);
}

//===========================================================================
// Event handlers
//===========================================================================

System::Void MUCBrushElementPanel::BasePanel_Scroll(System::Object*  
    sender, System::Windows::Forms::ScrollEventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::BasePanel_ControlRemoved(System::Object*  
    sender, System::Windows::Forms::ControlEventArgs*  e) 
{
    NI_UNUSED_ARG(sender);
    m_pmBasePanel->Controls->Remove(e->Control);
    if(!m_bDragging)
    {
        m_iScrollValue  = m_pmScrollBar1->Value;
    }
    
    RedrawControl();
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_DragDrop(
    System::Object*  sender, 
    System::Windows::Forms::DragEventArgs*  e)
{
    if (e->Effect == DragDropEffects::Move)
    {
        Control* pmItem = m_pmBasePanel->Controls->get_Item
                (m_iIndexOfItemToDrag);
        
        Point kLoc = Point(e->get_X(),e->get_Y());
        Point kScreenLoc = dynamic_cast<Control*>(sender)->PointToScreen(kLoc);
        
        //Remove item from the list
        m_pmBasePanel->Controls->RemoveAt(m_iIndexOfItemToDrag);
        pmItem->Visible = true;

        m_pmPictureBox1->Visible = false;
        
        // Insert the item.
        if (GetControlAtPosition(PointToClient(kLoc), true))
        {
            
            Control* pmCollection[] = 
                new Control*[m_pmBasePanel->Controls->Count+1];
            
            for (int i = 0; i < m_pmBasePanel->Controls->Count+1; i++)
            {
                if (i < m_iIndexWhereToDrop)
                {
                    Control* pmListItem = m_pmBasePanel->Controls->get_Item(i);
                    pmCollection[i] = pmListItem;
                }
                else if (i == m_iIndexWhereToDrop)
                {
                    pmCollection[i] = pmItem;
                    
                }
                else
                {
                    Control* pmListItem = 
                        m_pmBasePanel->Controls->get_Item(i-1);
                    pmCollection[i] = pmListItem;
                }
                
            }

            m_pmBasePanel->Controls->Clear();
            m_pmBasePanel->Controls->AddRange(pmCollection);

            MTerrainPlugin::GetInstance()->GetBrush()->MoveOperationTo
                (dynamic_cast<MUCBrushElement*>(pmItem)->GetOperation(),
                m_iIndexWhereToDrop);
            
            RedrawControl();
            
          
        }
        else
        {
            AddItemToList(dynamic_cast<UserControl*>(pmItem));
        }
        
        m_pmBasePanel->Update();
        m_bDragging = false;
        m_pmDraggedObject = NULL;
        m_pmScrollBar1->Value = m_iDropScrollValue;
    }

    
    
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_DragEnter(
    System::Object*  sender, 
    System::Windows::Forms::DragEventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    if(m_pmDraggedObject)
    {
        AddItemToList(m_pmDraggedObject);
        m_pmDraggedObject->Visible = false;
        m_pmDraggedObject = NULL;
    }
    
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_DragLeave(
    System::Object*  sender, 
    System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    if(!m_pmDraggedObject)
    {
        m_pmDraggedObject = dynamic_cast<MUCBrushElement*>(m_pmBasePanel->
            Controls->get_Item(m_iIndexOfItemToDrag));
    }
    RemoveItemFromList(m_pmDraggedObject);
    m_pmPictureBox1->Visible = false;
    RedrawControl();
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_DragOver(
    System::Object*, 
    System::Windows::Forms::DragEventArgs*  e)
{
    e->Effect = DragDropEffects::Move;
    Point kPosition = PointToClient(Point(e->X,e->Y));

    if(GetControlAtPosition(kPosition, true))
    {
        Point kLoc;
        m_pmPictureBox1->Visible = true;
        
        Control* pmControl = 
            m_pmBasePanel->Controls->get_Item(m_iIndexWhereToDrop);

        if(m_iIndexWhereToDrop > m_iIndexOfItemToDrag)
        {
            kLoc = Point(pmControl->Location.get_X(), 
                pmControl->Location.get_Y() + 
                pmControl->Height - m_pmPictureBox1->Height);
        }
        else
        {
            kLoc = Point(pmControl->Location.get_X(), 
                pmControl->Location.get_Y());
        }

        m_pmPictureBox1->Location = kLoc;

        if(kPosition.Y <= 5)
        {
            m_pmScrollBar1->Value -= 1;
            m_iScrollValue = m_pmScrollBar1->Value;
        }
        else if(kPosition.Y >= m_pmBasePanel->Height - 5)
        {
            m_pmScrollBar1->Value += 1;
            m_iScrollValue = m_pmScrollBar1->Value;
        }

        
    }

}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_GiveFeedback(
    System::Object*, 
    System::Windows::Forms::GiveFeedbackEventArgs*  e) 
{
    if(m_bDragging)
        e->UseDefaultCursors = true;
    
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_QueryContinueDrag(
    System::Object*  sender,
    System::Windows::Forms::QueryContinueDragEventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_MouseDown(
    System::Object*  sender, 
    System::Windows::Forms::MouseEventArgs*  e)
{
    m_pmBasePanel->Focus();
    if(e->Button == MouseButtons::Left)
    {
        Point kLoc = Point(e->get_X(),e->get_Y());
        Point kScreenLoc = dynamic_cast<Control*>(sender)->PointToScreen(kLoc);
        
        if (GetControlAtPosition(PointToClient(kScreenLoc), false))
         {
            
            // Remember the point where the mouse down occurred. The DragSize 
            // indicates the size that the mouse can move before a drag event 
            // should be started.
            System::Drawing::Size kDragSize = SystemInformation::DragSize;
            
            // Create a rectangle using the DragSize, with the mouse position 
            // being at the center of the rectangle.
            m_kDragingBox = System::Drawing::Rectangle
                (Point(e->X - (kDragSize.Width / 2),e->Y - 
                (kDragSize.Height / 2)),kDragSize);
         }
         else
         {
            m_kDragingBox = Rectangle::Empty;
         }
    }
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_MouseUp(
    System::Object*  sender, 
    System::Windows::Forms::MouseEventArgs*  e)
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    // Reset the drag rectangle when the mouse button is raised.
    m_kDragingBox = Rectangle::Empty;
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_MouseMove(
    System::Object*, 
    System::Windows::Forms::MouseEventArgs*  e)
{

    if ( e->Button == MouseButtons::Left )
    {
            
        // If the mouse moves outside the rectangle, start the drag.
        if ( m_kDragingBox != Rectangle::Empty &&  
            !m_kDragingBox.Contains( e->X, e->Y ) )
        {           
                            
            // The screenOffset is used to account for any desktop bands
            // that may be at the top or left side of the screen when
            // determining when to cancel the drag drop operation.
            m_kScreenOffset = SystemInformation::WorkingArea.Location;
              
            m_bDragging = true;
            m_pmDraggedObject = NULL;
            // Proceed with the drag-and-drop, passing in the list item.
            Object* pmToDrag = dynamic_cast<Object*>
                (m_pmBasePanel->Controls->get_Item(m_iIndexOfItemToDrag));
            this->m_pmBasePanel->DoDragDrop(pmToDrag, DragDropEffects::Move);
        }
    }
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_MouseHover(
    System::Object*  sender, System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_MouseWheel(
    System::Object*  sender, System::Windows::Forms::MouseEventArgs*  e)
{
    NI_UNUSED_ARG(sender);
    if ((m_pmBasePanel->Focused || this->Focused) && m_pmScrollBar1->Enabled)
    {
        int iStep = 20 * e->get_Delta()/120;
        if (e->get_Delta() > 0)
        {
            if (m_pmScrollBar1->Value - iStep > m_pmScrollBar1->Minimum)
            {
                m_pmScrollBar1->Value -= iStep;
                m_iScrollValue = m_pmScrollBar1->Value;
            }
            else
            {
                m_pmScrollBar1->Value = m_pmScrollBar1->Minimum;
                m_iScrollValue = m_pmScrollBar1->Value;
            }
        }
        else
        {
            if (m_pmScrollBar1->Value - iStep < 
                m_pmScrollBar1->Maximum - m_pmScrollBar1->LargeChange)
            {
                m_pmScrollBar1->Value -= iStep;
                m_iScrollValue = m_pmScrollBar1->Value;
            }
            else
            {
                m_pmScrollBar1->Value = 
                    m_pmScrollBar1->Maximum - m_pmScrollBar1->LargeChange;
                m_iScrollValue = m_pmScrollBar1->Value;
            }
        }
    }
}
//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::ScrollBar1_ValueChanged(System::Object*  
    sender, System::EventArgs*  e) 
{
    NI_UNUSED_ARG(e);
    NI_UNUSED_ARG(sender);
    {
         for(int i = 0; i < m_pmBasePanel->Controls->Count; i++)
         {

             m_pmBasePanel->Controls->get_Item(i)->Location =
                Point(m_pmBasePanel->Controls->get_Item(i)->Location.X,
                m_pmBasePanel->Controls->get_Item(i)->Location.Y - 
                (m_pmScrollBar1->Value - m_iCurrentScrollValue));
         }

         m_iCurrentScrollValue = m_pmScrollBar1->Value;
    }
}

//---------------------------------------------------------------------------
System::Void MUCBrushElementPanel::MUCBrushElementPanel_Resized(
    System::Object*, System::EventArgs*)
{
    for(int i = 0; i < m_pmBasePanel->Controls->Count; i++)
    {
        m_pmBasePanel->Controls->get_Item(i)->Width = this->Width - 20;
    }
}
