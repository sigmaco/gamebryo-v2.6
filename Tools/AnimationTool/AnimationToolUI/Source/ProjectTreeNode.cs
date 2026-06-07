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
using System.IO;
using System.Windows.Forms;
using System.Collections;

using NiManagedToolInterface;

namespace AnimationToolUI
{
    // All new additions to NodeType should be added to the end of the
    // enumeration, right before Undefined, to avoid breaking streaming.
    public enum NodeType : int
    {
        CharacterModelRoot = 0,
        SequenceFolderRoot,
        SequenceFolder,
        Sequence,
        SequenceGroupFolderRoot,
        SequenceGroupFolder,
        SequenceGroup,
        SequenceIDOverviewRoot,
        GroupIDOverviewRoot,
        InteractivePreviewRoot,
        TransitionsPreviewRoot,
        Undefined
    }

    /// <summary>
    /// Summary description for ProjectTreeNode.
    /// </summary>
    public class ProjectTreeNode : TreeNode
    {
        private NodeType m_eType;

        // Accessor property
        public NodeType NodeType
        {
            get
            {
                return m_eType;
            }
            set
            {
                if (value != m_eType)
                {
                    m_eType = value;
                    ImageIndex = (int)value;
                    SelectedImageIndex = (int)value;
                }
            }
        }

        #region Constructors
        public ProjectTreeNode(string text, NodeType eType ) : 
            base(text, (int)eType, (int)eType)
        {
            m_eType = eType;
        }
        public ProjectTreeNode(
            string text, NodeType eType, Object kExtraData) :
            base(text,(int)eType, (int)eType)
        {
            m_eType = eType;
            Tag = kExtraData; 
        }

        // TreeNode(string text, System.Windows.Forms.TreeNode[] children)
        public ProjectTreeNode(
            string text, System.Windows.Forms.TreeNode[] children, 
            NodeType eType ) : base(text, (int)eType, (int)eType, children)
        {
            m_eType = eType;
        }
        public ProjectTreeNode(
            string text, System.Windows.Forms.TreeNode[] children, 
            NodeType eType, Object kExtraData) : 
            base(text, (int)eType, (int)eType,children)
        {
            m_eType = eType;
            Tag = kExtraData;
        }
        
        // TreeNode(string text, System.Windows.Forms.TreeNode singleChild)
        public ProjectTreeNode(string text, ProjectTreeNode singleChild, 
            NodeType eType ) : 
            base(text, (int)eType, 
                 (int)eType,new ProjectTreeNode[1] { singleChild })
        {
            m_eType = eType;
        }
        public ProjectTreeNode(string text, ProjectTreeNode singleChild, 
            NodeType eType, Object kExtraData) : 
            base(text, (int)eType, (int)eType, 
            new ProjectTreeNode[1] { singleChild })
        {
            m_eType = eType;
            Tag = kExtraData;
        }
        #endregion

        public bool IsUndefined()
        {
            if (m_eType == NodeType.Undefined)
                return true;
            return false;
        }

        public bool IsRoot()
        {
            switch(m_eType)
            {
                case NodeType.CharacterModelRoot:
                case NodeType.SequenceFolderRoot:
                case NodeType.SequenceGroupFolderRoot:
                case NodeType.SequenceIDOverviewRoot:
                case NodeType.InteractivePreviewRoot:
                case NodeType.TransitionsPreviewRoot:
                    return true;
            }
            return false;

        }

        public bool IsFolder()
        {
            switch(m_eType)
            {
                case NodeType.SequenceFolderRoot:
                case NodeType.SequenceGroupFolderRoot:
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceFolder:
                    return true;
            }
            return false;
        }

        public bool IsDraggable()
        {
            switch (NodeType)
            {
                case NodeType.Sequence:
                case NodeType.SequenceFolder:
                case NodeType.SequenceGroup:
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceFolderRoot:
                    return true;
            }

            return false;
        }

        public bool CanDropInto(ProjectTreeNode kIntoNode)
        {
            if (IsDraggable() == false)
                return false;

            // 4 types of objects are draggable
            switch (NodeType)
            {
                case NodeType.Sequence:
                case NodeType.SequenceFolder:
                    if (kIntoNode.NodeType == NodeType.SequenceFolder ||
                        kIntoNode.NodeType == NodeType.SequenceFolderRoot ||
                        kIntoNode.NodeType == NodeType.SequenceGroup)
                        return true;
                    break;
                case NodeType.SequenceGroup:
                case NodeType.SequenceGroupFolder:
                    if (kIntoNode.NodeType == NodeType.SequenceGroupFolder ||
                        kIntoNode.NodeType == NodeType.SequenceGroupFolderRoot)
                        return true;
                    break;
                case NodeType.SequenceFolderRoot:
                    if (kIntoNode.NodeType == NodeType.SequenceGroup)
                        return true;
                    break;
            }

            return false;
        }

        public bool ContainsInHierarchy(ProjectTreeNode kNodeToTest)
        {
            // Need to find out if kNodeToTest is a child ancestor.
            ProjectTreeNode kParent = (ProjectTreeNode)kNodeToTest.Parent;

            while (kParent != null)
            {
                if (kParent == this)
                    return true;

                kParent = (ProjectTreeNode)kParent.Parent;
            }

            return false;
        }

        private bool DeleteChildren()
        {
            for (int i = Nodes.Count - 1; i >= 0; i--)
            {
                ProjectTreeNode node = (ProjectTreeNode) Nodes[i];
                if (!node.Delete())
                {
                    return false;
                }
            }

            return true;
        }

        public bool Delete()
        {
            if (CanDelete() == false)
                return false;

            // Check to see if user is sure.
            DialogResult kYesNoResult = DialogResult.Yes;
            string[] aStrings = new string[1];
            switch (NodeType)
            {
                case NodeType.Sequence:
                {
                    aStrings[0] = ((MSequence) Tag).Name;

                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_DELETE_SEQUENCE);
                    MLogger.LogGeneral(strLog + aStrings[0]);

                    kYesNoResult = 
                        MessageBoxManager.DoMessageBox("DeleteSequence.rtf",
                        "Delete Sequence Warning", MessageBoxButtons.YesNo,
                        MessageBoxIcon.Warning, aStrings);
                    break;
                }
                case NodeType.SequenceGroup:
                {
                    aStrings[0] = ((MSequenceGroup) Tag).Name;

                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_DELETE_SEQUENCE_GROUP);
                    MLogger.LogGeneral(strLog + aStrings[0]);

                    kYesNoResult = 
                        MessageBoxManager.DoMessageBox(
                        "DeleteSequenceGroup.rtf", 
                        "Delete Sequence Group Warning",
                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning,
                        aStrings);
                    break;
                }
                case NodeType.SequenceFolder:
                case NodeType.SequenceGroupFolder:
                {
                    aStrings[0] = this.Text;

                    string strLog = UserLogs.GetLogString(
                        UserLogs.LogType.PROJECTVIEW_DELETE_FOLDER);
                    MLogger.LogGeneral(strLog + aStrings[0]);

                    kYesNoResult = MessageBoxManager.DoMessageBox(
                        "DeleteFolder.rtf", "Delete Folder Warning",
                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning,
                        aStrings);
                    break;
                }
                default:
                    break;
            }

            if (kYesNoResult == DialogResult.No)
            {
                MLogger.LogGeneral("   Canceled");
                return false;
            }

            MLogger.LogGeneral("Deleting \"" + Text + "\".");
            
            if (NodeType == NodeType.Sequence)
            {
                ArrayList kList = AnimationHelpers.GetChainDependencies(
                    (MSequence)Tag);
                if (kList.Count != 0)
                {
                    string [] strTrans = new string[kList.Count];
                    int j = 0;
                    foreach (MTransition tran in kList)
                    {
                        strTrans[j] = "\"" + tran.Source + "\" to \"" + 
                            tran.Destination + "\"";
                        j++;
                    }

                    string [] strVariables = new string[3];
                    strVariables[0] = Text;
                    strVariables[1] = "sequence";
                    strVariables[2] = 
                        MessageBoxManager.CreateBulletedList(strTrans);

                    if (MessageBoxManager.DoMessageBox(
                        "DependentChainsFoundDeleteDelete.rtf",
                        "Chain Transition Dependencies Found", 
                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning, 
                        strVariables) != DialogResult.Yes)
                    {
                        return false;
                    }
                }

                // Now look for dependencies in sequence groups
                kList = AnimationHelpers.GetSequenceGroupDependencies(
                    (MSequence)Tag);
                if (kList.Count != 0)
                {
                    string [] strSeqGrps = new string[kList.Count];
                    int j = 0;
                    foreach (MSequenceGroup seqGrp in kList)
                    {
                        strSeqGrps[j] = "\"" + seqGrp.Name + "\"";
                        j++;
                    }

                    string [] strVariables = new string[2];
                    strVariables[0] = Text;
                    strVariables[1] = MessageBoxManager.CreateBulletedList(
                        strSeqGrps);

                    if (MessageBoxManager.DoMessageBox(
                        "DependentSequenceGroupsFound.rtf",
                        "Sequence Group Dependencies Found", 
                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning, 
                        strVariables) != DialogResult.Yes)
                    {
                        return false;
                    }
                }

                
            }

            switch (NodeType)
            {
                case NodeType.Sequence:
                    Remove();
                    MFramework.Instance.Animation.RemoveSequence(
                        ((MSequence) Tag).SequenceID);
                    break;
                case NodeType.SequenceGroup:
                    Remove();
                    MFramework.Instance.Animation.RemoveSequenceGroup(
                        ((MSequenceGroup) Tag).GroupID);
                    break;
                case NodeType.SequenceFolder:
                case NodeType.SequenceGroupFolder:
                    if (!DeleteChildren())
                    {
                        return false;
                    }
                    Remove();
                    break;
                default:
                    Remove();
                    break;
            }

            return true;
        }

        public bool CanDelete()
        {
            switch (NodeType)
            {
                case NodeType.SequenceFolder:
                case NodeType.Sequence:
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceGroup:
                    return true;
            }
            return false;
        }

        public ProjectTreeNode FindNodeByID(uint uID)
        {
            switch(NodeType)
            {
                case NodeType.SequenceFolderRoot:
                case NodeType.SequenceFolder:
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceGroupFolderRoot:
                    foreach (ProjectTreeNode child in Nodes)
                    {
                        ProjectTreeNode kRes = child.FindNodeByID(uID);
                        if (kRes != null)
                            return kRes;
                    }
                    break;
                case NodeType.Sequence:
                    if (((MSequence)Tag).SequenceID == uID)
                        return this;
                    break;
                case NodeType.SequenceGroup:
                    if (((MSequenceGroup)Tag).GroupID == uID)
                        return this;
                    break;
            }

            return null;
        }

        // IO
        public bool Read(BinaryReader br)
        {
            NodeType eNodeType = (NodeType)br.ReadInt32();
            this.NodeType = eNodeType; 

            Text = br.ReadString();
                    
            switch(eNodeType)
            {
                case NodeType.CharacterModelRoot:
                case NodeType.SequenceFolder:
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceIDOverviewRoot:
                case NodeType.InteractivePreviewRoot:
                case NodeType.TransitionsPreviewRoot:
                    break;

                case NodeType.Sequence:
                {
                    uint uiSequenceID = br.ReadUInt32();
                    // Find MSequence that fits ID and assign...
                    // Then create properties.
                    Tag = MFramework.Instance.Animation.GetSequence(
                        uiSequenceID);
                    if (Tag == null)
                    {
                        return false;
                    }

                    Text = ((MSequence)Tag).Name;

                    break;
                }

                case NodeType.SequenceGroup:
                {
                    uint uiGroupID = br.ReadUInt32();
                    // Find MSequenceGroup that fits ID and assign...
                    // Then create properties.
                    Tag = MFramework.Instance.Animation.GetSequenceGroup(
                        uiGroupID);
                    if (Tag == null)
                    {
                        return false;
                    }

                    Text = ((MSequenceGroup)Tag).Name;

                    break;
                }
            }

            if (IsFolder() == false)
                return true;

            // Now read Children...
            int iChildren = br.ReadInt32(); 

            for (int i=0; i < iChildren; i++)
            {
                ProjectTreeNode kPVNChild =
                    new ProjectTreeNode("< Undefined >", NodeType.Undefined);

                if (kPVNChild.Read(br))
                {
                    Nodes.Add(kPVNChild);
                }
            }

            return true;
        }

        public void Write(BinaryWriter w)
        {
            if (IsUndefined() == true)
                return;

            // Everything needs it's node type saved for reconstruction.
            NodeType eNodeType = NodeType; 
            w.Write((int)eNodeType);

            // Folders need to have names saved.
            // Sequences need to have ID's saved.
            // Sequence Groups need to have ID's saved.
            // 
            // Properties should not be saved (they will be
            // created from the sequences and sequence groups
            //
            w.Write(Text);
            switch(eNodeType)
            {
                case NodeType.CharacterModelRoot:
                case NodeType.SequenceFolder:
                case NodeType.SequenceGroupFolder:
                case NodeType.SequenceIDOverviewRoot:
                case NodeType.InteractivePreviewRoot:
                case NodeType.TransitionsPreviewRoot:
                    break;

                case NodeType.Sequence:
                {
                    MSequence kMSeq = (MSequence)Tag;
                    w.Write(kMSeq.SequenceID);
                    break;
                }

                case NodeType.SequenceGroup:
                {
                    MSequenceGroup kMSeqGrp = (MSequenceGroup)Tag;
                    w.Write(kMSeqGrp.GroupID);
                    break;
                }
            }

            if (IsFolder() == false)
                return;

            // Now write Children...
            w.Write(Nodes.Count); // Write the number of children...

            foreach (ProjectTreeNode child in Nodes)
            {
                child.Write(w);
            }
        }

        public bool DoesFolderExist(string strName)
        {
            ArrayList akFolders = GetFolders();
            
            for(int i=0; i<akFolders.Count; i++)
            {
                ProjectTreeNode kFolder = (ProjectTreeNode)(akFolders[i]);

                if (kFolder.Text == strName)
                    return true;
            }

            return false;
        }

        public ArrayList GetFolders()
        {
            ArrayList kList = new ArrayList();

            foreach (ProjectTreeNode kChild in Nodes)
            {
                if (kChild.IsFolder())
                    kList.Add(kChild);
            }

            return kList;
        }

        public ArrayList GetObjects()
        {
            ArrayList kList = new ArrayList();

            foreach (ProjectTreeNode kChild in Nodes)
            {
                if (kChild.IsFolder() == false)
                    kList.Add(kChild);
            }

            return kList;
        }

    }
}
