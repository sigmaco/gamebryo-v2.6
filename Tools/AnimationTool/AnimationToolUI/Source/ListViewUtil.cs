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
using System.Windows.Forms;
using System.Collections;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    /// <summary>
    /// Summary description for ListViewUtil.
    /// </summary>
    public class ListViewUtil
    {
        public ListViewUtil()
        {
        }

        public static void SelectPrevious(ref ListView lv)
        {
            // Goto the previous line in the list.
            int iCurrentIndex = lv.SelectedIndices[0];
            int iNewIndex = iCurrentIndex - 1;
            if (iNewIndex < 0)
            {
                iNewIndex = lv.Items.Count - 1;
            }

            if (iNewIndex != iCurrentIndex)
            {
                // This line will cause the listview to do a callback!!!
                lv.Items[iNewIndex].Selected = true;
                lv.Select();
                lv.SelectedItems[0].BeginEdit();
            }
        }

        public static void SelectNext(ref ListView lv)
        {
            if (lv.SelectedIndices.Count == 0)
                return;

            // Goto to the next line in the list
            int iCurrentIndex = lv.SelectedIndices[0];
            int iNewIndex = iCurrentIndex + 1;
            if (iNewIndex >= lv.Items.Count)
            {
                iNewIndex = 0;
            }

            if (iNewIndex != iCurrentIndex)
            {
                // This line will cause the listview to do a callback!!!
                lv.Items[iNewIndex].Selected = true;
                lv.Select();
                lv.SelectedItems[0].BeginEdit();
            }
        }

        public static void RefreshListWith_SequenceID_Name(ref ListView kLV)
        {
            ArrayList akSeq = MFramework.Instance.Animation.Sequences;

            kLV.Items.Clear();

            if (akSeq == null)
                return;

            foreach (MSequence kSeq in akSeq)
            {
                string strID = kSeq.SequenceID.ToString();
                string strName = kSeq.Name;

                ListViewItem kItem =
                    new ListViewItem( new string[] { strID, strName}, -1);

                kItem.Tag = kSeq;

                kLV.Items.Add( kItem );
            }

            kLV.Sort();
        }

        public static void RefreshListWith_GroupID_Name(ref ListView lv)
        {
            ArrayList aGroups = MFramework.Instance.Animation.SequenceGroups;

            lv.Items.Clear();

            if (aGroups == null)
            {
                return;
            }

            foreach (MSequenceGroup group in aGroups)
            {
                string strID = group.GroupID.ToString();
                string strName = group.Name;

                ListViewItem item = new ListViewItem(new string[] {strID,
                    strName}, -1);

                item.Tag = group;
                lv.Items.Add(item);
            }

            lv.Sort();
        }

        public static void RefreshListWith_Key_Mod_Name(
            ref ListView kLV, Hashtable kHT)
        {
            //MSequence[] akSeq = MFramework.Instance.Animation.Sequences;

            kLV.Items.Clear();

            // For each sequence.
            //if (akSeq == null)
            //    return;

            //Hashtable kHT = ProjectData.GetSeqInputInfoHash();

            // foreach entry in the hash...
            foreach (DictionaryEntry e in kHT)
            {
                ProjectData.SeqInputInfo kKSM = (ProjectData.SeqInputInfo)
                    e.Value;

                MSequence kSeq = kKSM.m_Seq;
                uint uiID = kSeq.SequenceID;
                string strName = kSeq.Name;

                ListViewItem kItem;

                ProjectData.SeqInputInfo kEntry =
                    (ProjectData.SeqInputInfo) kHT[uiID];

                string strKeyStroke = kEntry.m_strKeyStroke;
                if (strKeyStroke == null)
                {
                    strKeyStroke = " ";
                }
                string strModifier = kEntry.m_strModifier;
                if (strModifier == null)
                {
                    strModifier = " ";
                }

                kItem = new ListViewItem( new string[]
                    { strKeyStroke, strModifier, strName }, -1);
                kItem.Tag = kSeq;
                kLV.Items.Add( kItem );             
            }
            kLV.Sort();
        }
    }
}
