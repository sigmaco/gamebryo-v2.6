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
using System.Collections;
using System.Windows.Forms;
using System.Diagnostics;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for InputManager.
    /// </summary>
    public class InputManager
    {
        #region Member Variables

        public enum InputMode
        {
            Toggle,
            Interactive
        }

        private static InputManager m_pkThis = null;
        private InputMode m_eMode = InputMode.Interactive;
        private bool m_bEnabled = true;
        private Hashtable m_hashIDToInput = null;
        private Hashtable m_hashInputToID = null;

        private Hashtable m_hashPressedKeys = new Hashtable();
        private Stack m_stackPressedKeys = new Stack();
        private bool m_bAltKeyPressed = false;
        private bool m_bControlKeyPressed = false;
        private bool m_bShiftKeyPressed = false;

        private ArrayList m_aForms = new ArrayList();
        #endregion

        #region Properties

        public static InputManager Instance
        {
            get
            {
                if (m_pkThis == null)
                {
                    m_pkThis = new InputManager();
                }
                return m_pkThis;
            }
        }

        public InputMode Mode
        {
            get {return m_eMode;}
            set {m_eMode = value;}
        }

        public bool Enabled
        {
            get {return m_bEnabled;}
        }

        public Hashtable HashIDToInput
        {
            get {return m_hashIDToInput;}
            set
            {
                m_hashIDToInput = value;
                PopulateInputToIDHashtable();
            }
        }

        #endregion

        #region Member Functions

        private InputManager()
        {
            AppEvents.KeystrokeBindingsChanged += new AppEvents
                .EventHandler_KeystrokeBindingsChanged(
                this.PopulateInputToIDHashtable);
        }

        public void EnableInputHandling(InputMode mode,
            Hashtable hashIDToInput)
        {
            this.Mode = mode;
            this.HashIDToInput = hashIDToInput;
            m_bEnabled = true;
        }

        public void DisableInputHandling()
        {
            m_bEnabled = false;
        }

        public void AddForm(System.Windows.Forms.Form kForm)
        {
            foreach (Form currentForm in m_aForms)
            {
                if (currentForm == kForm)
                    return;
            }

            int iFormIndex = m_aForms.Add(kForm);
            
        }

        public void RemoveForm(System.Windows.Forms.Form kForm)
        {
            int iFound = m_aForms.IndexOf(kForm);
            if (iFound >= 0)
            {
                m_aForms.RemoveAt(iFound);
                m_aForms.TrimToSize();
            }
       }


        private void PopulateInputToIDHashtable()
        {
            if (m_hashIDToInput == null)
            {
                m_hashInputToID = null;
                return;
            }

            m_hashInputToID = new Hashtable();
            foreach (DictionaryEntry e in m_hashIDToInput)
            {
                ProjectData.SeqInputInfo info = (ProjectData.SeqInputInfo)
                    e.Value;
                if (info.m_eKeyStroke != Keys.None)
                {
                    m_hashInputToID.Add(info.m_eKeyStroke,
                        info.m_Seq.SequenceID);
                }
            }
        }

        public bool OnKeyDown(Keys keyData)
        {
            if (!m_bEnabled || m_hashInputToID == null)
            {
                return false;
            }

            bool bActiveFormIsInList = false;
            foreach (Form kMaybeActiveForm in m_aForms)
            {
                if (kMaybeActiveForm.ContainsFocus)
                {
                    bActiveFormIsInList = true;
                    break;
                }
            }

            if (!bActiveFormIsInList)
            {
                return false;
            }

            switch (m_eMode)
            {
                case InputMode.Toggle:
                    if (m_hashInputToID.ContainsKey(keyData))
                    {
                        AppEvents.RaiseToggleSequence((uint)
                            m_hashInputToID[keyData]);
                        return true;
                    }
                    break;
                case InputMode.Interactive:
                    Keys keyCode = keyData & Keys.KeyCode;
                    if (!m_hashPressedKeys.Contains(keyCode))
                    {
                        bool bPushed = false;
                        switch (keyCode)
                        {
                            case Keys.Alt:
                            case Keys.Menu:
                                m_bAltKeyPressed = true;
                                break;
                            case Keys.Control:
                            case Keys.ControlKey:
                                m_bControlKeyPressed = true;
                                break;
                            case Keys.Shift:
                            case Keys.ShiftKey:
                                m_bShiftKeyPressed = true;
                                break;
                            default:
                                m_stackPressedKeys.Push(keyCode);
                                bPushed = true;
                                break;
                        }
                        m_hashPressedKeys.Add(keyCode, null);

                        if (!RaiseInteractiveEvent())
                        {
                            if (bPushed)
                            {
                                m_stackPressedKeys.Pop();
                            }
                            return false;
                        }
                    }
                    return true;
            }

            return false;
        }

        private bool RaiseInteractiveEvent()
        {
            Debug.Assert(m_eMode == InputMode.Interactive &&
                m_hashInputToID != null, "RaiseInteractiveEvent can only " +
                "be called in Interactive mode,");

            Keys keyMostRecent;
            if (m_stackPressedKeys.Count == 0)
            {
                keyMostRecent = Keys.None;
            }
            else
            {
                keyMostRecent = (Keys) m_stackPressedKeys.Peek();
                while (!m_hashPressedKeys.Contains(keyMostRecent))
                {
                    m_stackPressedKeys.Pop();
                    if (m_stackPressedKeys.Count == 0)
                    {
                        keyMostRecent = Keys.None;
                        break;
                    }
                    else
                    {
                        keyMostRecent = (Keys) m_stackPressedKeys.Peek();
                    }
                }
            }

            Keys keyData = keyMostRecent;
            if (m_bAltKeyPressed)
            {
                keyData |= Keys.Alt;
            }
            if (m_bControlKeyPressed)
            {
                keyData |= Keys.Control;
            }
            if (m_bShiftKeyPressed)
            {
                keyData |= Keys.Shift;
            }

            if (keyData == Keys.None)
            {
                AppEvents.RaiseInteractiveSequence(true, uint.MaxValue);
                return true;
            }
            else if (m_hashInputToID.ContainsKey(keyData))
            {
                AppEvents.RaiseInteractiveSequence(false, (uint)
                    m_hashInputToID[keyData]);
                return true;
            }
            else if (m_hashInputToID.ContainsKey(keyMostRecent))
            {
                AppEvents.RaiseInteractiveSequence(false, (uint)
                    m_hashInputToID[keyMostRecent]);
                return true;
            }

            return false;
        }

        public bool OnKeyUp(Keys keyCode)
        {
            if (!m_bEnabled || m_eMode != InputMode.Interactive ||
                m_hashInputToID == null)
            {
                return false;
            }

            switch (keyCode)
            {
                case Keys.Alt:
                case Keys.Menu:
                    m_bAltKeyPressed = false;
                    break;
                case Keys.Control:
                case Keys.ControlKey:
                    m_bControlKeyPressed = false;
                    break;
                case Keys.Shift:
                case Keys.ShiftKey:
                    m_bShiftKeyPressed = false;
                    break;
            }
            m_hashPressedKeys.Remove(keyCode);

            return RaiseInteractiveEvent();
        }

        #endregion

    }
}
